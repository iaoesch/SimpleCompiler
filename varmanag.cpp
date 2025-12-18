/*****************************************************************************/
/*  Class      : BTerror                                        Version 1.0  */
/*****************************************************************************/
/*                                                                           */
/*  Function   : Defines the errorclass used in Bonetracker-exceptionhandling*/
/*                                                                           */
/*                                                                           */
/*  Methodes   : BTError()                                                   */
/*              ~BTError()                                                   */
/*               GetString()                                                 */
/*               GetDebugInfo()                                              */
/*                                                                           */
/*  Author     : I. Oesch                                                    */
/*                                                                           */
/*  History    : 15.02.1997  IO Created                                      */
/*                                                                           */
/*  File       : errclass.cpp                                                */
/*                                                                           */
/*****************************************************************************/
/*    SWS Diplomarbeit 1997 Bonetracker from Ivo Oesch and Daniel Buergin    */
/*****************************************************************************/

/* imports */
#include "varmanag.hpp"
#include "compact.h"
#include "Errclass.hpp"
#include <cassert>

/* Class constant declaration  */

/* Class Type declaration      */

/* Class data declaration      */

/* Class procedure declaration */

/*****************************************************************************/
/*  Method      : BTError                                                    */
/*****************************************************************************/
/*                                                                           */
/*  Function    : Constructors for errorclass                                */
/*                                                                           */
/*  Type        : constructor                                                */
/*                                                                           */
/*  Input Para  : Initializing-values                                        */
/*                                                                           */
/*  Output Para : None                                                       */
/*                                                                           */
/*  Author      : I. Oesch                                                   */
/*                                                                           */
/*  History     : 15.02.1997  IO  Created                                    */
/*                                                                           */
/*****************************************************************************/
#if 0
#endif
/*****************************************************************************/
/*  End  Method : GetDebugInfo                                               */
/*****************************************************************************/

/*****************************************************************************/
/*  End Class   : BTError                                                    */
/*****************************************************************************/



void VariableManager::CreateNewContext(std::string Name)
{
    if (ContextStack.empty()) {
        ContextStack.push_back(std::make_shared<VariableContextClass>(Name));
    } else {
        ContextStack.push_back(ContextStack.back()->CreateSubContext(Name));
    }
    Contexts.push_back(ContextStack.back());
}

void VariableManager::LeaveContext(int Levels)
{
    while (Levels > 0) {
        if (ContextStack.empty()) {
            throw INTERNAL_ERROR_OBJECT("popping empty contextstack");
        }

        ContextStack.pop_back();
        Levels--;
    }
}

void VariableManager::StartLocal(std::shared_ptr<Variables::FunctionDefinitionClass> Parent)
{
    Local = true;
    LocalOffset = 0;
    LocalStorageTemplates.push_back(LocalStorageType());
    LocalsParent = Parent;
}

VariableManager::LocalStorageType VariableManager::EndLocal()
{
    LocalStorageType Storage;
    if (LocalStorageTemplates.empty()) {
        throw INTERNAL_ERROR_OBJECT("Internal, local stack empty");
    }
    std::swap(Storage, LocalStorageTemplates.back());
    LocalStorageTemplates.pop_back();
    if (LocalStorageTemplates.empty()) {
        Local = false;
        LocalOffset = 0;
    } else {
        Local = true;
        LocalOffset = static_cast<decltype(LocalOffset)>(LocalStorageTemplates.back().size());
    }
    return Storage;
}

std::shared_ptr<VariableClass> VariableManager::CreateFunction(std::string Name, const VariableTypeDescriptorClass &Type, double Value)
{
    (void) Value;
    return CreateSymbol(Name, Type, VariableClass::StorageClass::Code);
}

std::shared_ptr<VariableClass> VariableManager::CreateConstant(std::string Name, const VariableTypeDescriptorClass &Type, double Value)
{
    (void) Value;
    return CreateSymbol(Name, Type, VariableClass::StorageClass::ReadOnly);
}

std::shared_ptr<VariableClass> VariableManager::CreateVariable(std::string Name, const VariableTypeDescriptorClass &Type, double Value)
{
    (void) Value;
    return CreateSymbol(Name, Type, VariableClass::StorageClass::ReadAndWrite);
}

std::shared_ptr<VariableClass> VariableManager::CreateSymbol(std::string Name, const VariableTypeDescriptorClass &Type, VariableClass::StorageClass Storage)
{
    if (ContextStack.empty()) {
        throw INTERNAL_ERROR_OBJECT("No valid context");
        return nullptr;
    }
    std::shared_ptr<VariableClass> Var;
    if (Local && (Storage == VariableClass::StorageClass::ReadAndWrite)) {
        Var = std::make_shared<LocalVariableClass>(Name, Type, LocalOffset++, LocalsParent, Storage);
        LocalStorageTemplates.back().push_back(Variables::VariableContentClass(Type));
        assert(LocalOffset == LocalStorageTemplates.back().size());
    } else {
        Var = std::make_shared<GlobalVariableClass>(Name, Type, Storage);
    }
    return ContextStack.back()->RegisterVariable(Name, Var, false);

}

#if 0

std::shared_ptr<VariableClass> VariableManager::GetOrCreateVariable(std::string Name, const VariableTypeDescriptorClass &Type, double Value)
{
    if (ContextStack.empty()) {
        throw INTERNAL_ERROR_OBJECT("No valid context");
        return nullptr;
    }
    auto Var = ContextStack.back()->LookupVariable(Name);
    if (Var != nullptr) {
        return Var;
    } else {
        return CreateVariable(Name, Type, Value);
    }
}
#endif

std::shared_ptr<VariableClass> VariableManager::GetVariableReference(std::string Name)
{
    if (ContextStack.empty()) {
        throw INTERNAL_ERROR_OBJECT("No valid context");
        return nullptr;
    }
    std::shared_ptr<VariableClass> VarRef = ContextStack.back()->LookupVariable(Name);
    return VarRef;
}

std::shared_ptr<VariableClass> VariableManager::GetVariableReferenceCreateIfNotFound(std::string Name, const VariableTypeDescriptorClass &RequiredType)
{
    if (ContextStack.empty()) {
        throw INTERNAL_ERROR_OBJECT("No valid context");
        return nullptr;
    }
    std::shared_ptr<VariableClass> VarRef = GetVariableReference(Name);
    if (VarRef == nullptr) {
#if 1
        VarRef = CreateVariable(Name, RequiredType, 0.0);
#else
        if (Local) {
           VarRef = std::make_shared<LocalVariableClass>(Name, RequiredType, LocalOffset++, LocalsParent);
            LocalStorageTemplates.back().push_back(Variables::VariableContentClass(RequiredType));
            assert(LocalOffset == LocalStorageTemplates.back().size());
        } else {
           VarRef = std::make_shared<GlobalVariableClass>(Name, RequiredType);
        }
        VarRef = ContextStack.back()->RegisterVariable(Name, VarRef);
#endif
    }
    return VarRef;
}

void VariableManager::Dump(std::ostream &s)
{
    s << "Scopes:" << std::endl;
    for (auto &c: Contexts) {
        c->Dump(s);
    }
}

std::shared_ptr<VariableClass> VariableContextClass::RegisterVariable(const std::string Name, std::shared_ptr<VariableClass> Var, bool OverwriteAllowed)
{
    auto it = Variables.find(Name);
    if (it == Variables.end() || OverwriteAllowed) {
        Var->SetContext(this);
        Variables[Name] = Var;
        return Var;
    }
    return nullptr;
}

std::shared_ptr<VariableClass> VariableContextClass::LookupVariable(const std::string Name)
{
    auto it = Variables.find(Name);
    if (it == Variables.end()) {
        if (ParentContext != nullptr) {
            return ParentContext->LookupVariable(Name);
        }
        return nullptr;
    }
    return it->second;
}

void VariableContextClass::Dump(std::ostream &s)
{
    s << "Context <" << Name << ">" << std::endl;
    s << "Parent: <" << ((ParentContext != nullptr) ? ParentContext->Name : std::string(" --- ")) << ">" << std::endl;
    s << "Children:";
    if (Children.empty()) {
        s << " None " << std::endl;
    } else {
        for (auto &c: Children) {
            s << c->Name << ",";
        }
        s << std::endl;
    }
    s << "Content:" << std::endl;
    for (auto &i: Variables) {
        std::cout << i.first << "{ " << std::endl;
        i.second->Print(s);
        std::cout << "}" << std::endl;

    }

}

