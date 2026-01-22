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

static ErrorEnvironment Errorenv;
Environment *VariableManager::DefaultEnvironment = &Errorenv;


void VariableManager::CreateNewContext(std::string Name, ParentVisibility ParentVisibilityMode)
{
    if (ContextStack.empty()) {
        ContextStack.push_back(std::make_shared<VariableContextClass>(Name));
    } else {
        ContextStack.push_back(ContextStack.back()->CreateSubContext(Name, ParentVisibilityMode == HideParent));
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

void VariableManager::StartLocal(std::shared_ptr<Variables::FunctionDefinitionBaseClass> Parent)
{
    Local = true;
    LocalOffset = 0;
    LocalClassOffset = 0;
    LocalStorageTemplates.push_back(LocalStorageContextType(FktTemplate{Parent->GetStorageTemplate(), Parent}));
  //  LocalsParent = Parent;
}

void VariableManager::EndLocal()
{
    if (LocalStorageTemplates.empty()) {
        throw INTERNAL_ERROR_OBJECT("Internal, local stack empty");
    }
    LocalStorageTemplates.pop_back();
    if (LocalStorageTemplates.empty()) {
        Local = false;
        LocalOffset = 0;
        LocalClassOffset = 0;
    } else {
        Local = true;
        LocalOffset = static_cast<decltype(LocalOffset)>(std::get<FktTemplate>(LocalStorageTemplates.back()).LocalStorageTemplates.size());
        LocalClassOffset = 0;
    }
}

void VariableManager::StartClass(std::shared_ptr<Variables::ClassClass> Parent)
{
    Local = true;
    LocalClassOffset = Parent->GetClassStorageTemplateFirstOffset();
    LocalOffset = Parent->GetStorageTemplateFirstOffset();
    LocalStorageTemplates.push_back(LocalStorageContextType(ClassTemplate{Parent->GetClassStorageTemplate(), Parent->GetStorageTemplate(), Parent}));
    //  LocalsParent = Parent;
}

void VariableManager::EndClass()
{
    if (LocalStorageTemplates.empty()) {
        throw INTERNAL_ERROR_OBJECT("Internal, local stack empty");
    }
    LocalStorageTemplates.pop_back();
    if (LocalStorageTemplates.empty()) {
        Local = false;
        LocalOffset = 0;
        LocalClassOffset = 0;
    } else {
        Local = true;
        LocalOffset = static_cast<decltype(LocalOffset)>(std::get<ClassTemplate>(LocalStorageTemplates.back()).LocalAttrubiteStorageTemplates.size());
        LocalClassOffset = static_cast<decltype(LocalOffset)>(std::get<ClassTemplate>(LocalStorageTemplates.back()).LocalClassAttributeStorageTemplates.size());
    }
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
        DefaultEnvironment->DebugOutput() << "creating local <" << Name << ">\n";
        Var = std::make_shared<LocalVariableClass>(Name, Type, LocalOffset++, std::get<FktTemplate>(LocalStorageTemplates.back()).LocalsParent, Storage);
        std::get<FktTemplate>(LocalStorageTemplates.back()).LocalStorageTemplates.push_back(Variables::VariableContentClass::MakeEmpty(Type));
        assert(LocalOffset == std::get<FktTemplate>(LocalStorageTemplates.back()).LocalStorageTemplates.size());
    } else {
        DefaultEnvironment->DebugOutput() << "creating global <" << Name << ">\n";
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

std::shared_ptr<VariableClass> VariableManager::CreateVariableAndGetReference(std::string Name, const VariableTypeDescriptorClass &RequiredType)
{
    if (ContextStack.empty()) {
        throw INTERNAL_ERROR_OBJECT("No valid context");
        return nullptr;
    }
    std::shared_ptr<VariableClass> VarRef = GetVariableReference(Name);
    if (VarRef == nullptr) {
        VarRef = CreateVariable(Name, RequiredType, 0.0);
        return VarRef;
    } else {
        // Could not create Variable, exists allready
        return nullptr;
    }
}

std::shared_ptr<VariableClass> VariableManager::GetVariableReferenceForContext(std::string Name, size_t Index)
{
    if (Index >= Contexts.size()) {
        return nullptr;
    }
    std::shared_ptr<VariableClass> VarRef = Contexts[Index]->LookupVariable(Name);
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
    throw RuntimeErrorClass("Variable '" + Name + "' allready defined", -1);
    return nullptr;
}

std::shared_ptr<VariableClass> VariableContextClass::LookupVariable(const std::string Name)
{
    auto it = Variables.find(Name);
    if (it == Variables.end()) {
        if ((ParentIsHidden == false) && (ParentContext != nullptr)) {
            return ParentContext->LookupVariable(Name);
        }
        return nullptr;
    }
    return it->second;
}

std::shared_ptr<VariableClass> VariableContextClass::LookupVariableInThisContextOnly(const std::string Name)
{
    auto it = Variables.find(Name);
    if (it == Variables.end()) {
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
        s << i.first << "{ " << std::endl;
        i.second->Print(s);
        s << "}" << std::endl;

    }

}

