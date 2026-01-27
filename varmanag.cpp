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


std::shared_ptr<VariableContextClass> VariableManager::CreateNewContext(std::string Name, ParentVisibility ParentVisibilityMode)
{
    std::shared_ptr<VariableContextClass> NewContext;
    if (ContextStack.empty()) {
        NewContext = std::make_shared<VariableContextManageClass>(Name);
    } else {
        NewContext = ContextStack.back()->CreateSubContext(Name, ParentVisibilityMode == HideParent);
    }
    ContextStack.push_back(NewContext);
    Contexts.push_back(NewContext);
    return NewContext;
}

std::shared_ptr<VariableContextProxyClass> VariableManager::CreateNewProxyContext(std::string Name, ParentVisibility ParentVisibilityMode)
{
    std::shared_ptr<VariableContextProxyClass> NewContext;
    if (ContextStack.empty()) {
        NewContext = std::make_shared<VariableContextProxyClass>(Name);
    } else {
        NewContext = ContextStack.back()->CreateProxySubContext(
            Name, ParentVisibilityMode == HideParent);
    }
    ContextStack.push_back(NewContext);
    Contexts.push_back(NewContext);
    return NewContext;
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
    LocalStorageInfoStack.push_back({LocalStorageContextType(FktTemplate{Parent->GetStorageTemplate(), Parent}), DefaultStorage});
    DefaultStorage = VariableClass::StorageClass::Local | VariableClass::StorageClass::RW;
  //  LocalsParent = Parent;
}

void VariableManager::EndLocal()
{
    if (LocalStorageInfoStack.empty()) {
        throw INTERNAL_ERROR_OBJECT("Internal, local stack empty");
    }
    LocalStorageInfoStack.pop_back();
    if (LocalStorageInfoStack.empty()) {
        Local = false;
        LocalOffset = 0;
        LocalClassOffset = 0;
        DefaultStorage = VariableClass::StorageClass::Global | VariableClass::StorageClass::RW;
    } else {
        Local = true;
        DefaultStorage = LocalStorageInfoStack.back().DefaultStorage;
        if (std::holds_alternative<FktTemplate>(LocalStorageInfoStack.back().LocalStorageTemplates)) {
           LocalOffset = static_cast<decltype(LocalOffset)>(std::get<FktTemplate>(LocalStorageInfoStack.back().LocalStorageTemplates).LocalStorageTemplates.size());
           LocalClassOffset = 0;
        } else if (std::holds_alternative<ClassTemplate>(LocalStorageInfoStack.back().LocalStorageTemplates)) {
            LocalOffset = static_cast<decltype(LocalOffset)>(std::get<ClassTemplate>(LocalStorageInfoStack.back().LocalStorageTemplates).LocalAttrubiteStorageTemplates.size());
            LocalClassOffset = static_cast<decltype(LocalOffset)>(std::get<ClassTemplate>(LocalStorageInfoStack.back().LocalStorageTemplates).LocalClassAttributeStorageTemplates.size());
        } else {
            throw INTERNAL_ERROR_OBJECT("Internal, local stack unknown type");
        }
    }
}

void VariableManager::StartClass(std::shared_ptr<Variables::ClassClass> Parent)
{
    Local = true;
    LocalClassOffset = Parent->GetClassStorageTemplateFirstOffset();
    LocalOffset = Parent->GetStorageTemplateFirstOffset();
    LocalStorageInfoStack.push_back({LocalStorageContextType(ClassTemplate{Parent->GetClassStorageTemplate(), Parent->GetObjectStorageInitialValues(), Parent->GetObjectVariableReferences(), Parent}), DefaultStorage});
    DefaultStorage = VariableClass::StorageClass::Class | VariableClass::StorageClass::RW;
    //  LocalsParent = Parent;
}

void VariableManager::EndClass()
{
    if (LocalStorageInfoStack.empty()) {
        throw INTERNAL_ERROR_OBJECT("Internal, local stack empty");
    }
    LocalStorageInfoStack.pop_back();
    if (LocalStorageInfoStack.empty()) {
        Local = false;
        LocalOffset = 0;
        LocalClassOffset = 0;
        DefaultStorage = VariableClass::StorageClass::Global | VariableClass::StorageClass::RW;
    } else {
        Local = true;
        DefaultStorage = LocalStorageInfoStack.back().DefaultStorage;
        if (std::holds_alternative<FktTemplate>(LocalStorageInfoStack.back().LocalStorageTemplates)) {
            LocalOffset = static_cast<decltype(LocalOffset)>(std::get<FktTemplate>(LocalStorageInfoStack.back().LocalStorageTemplates).LocalStorageTemplates.size());
            LocalClassOffset = 0;
        } else if (std::holds_alternative<ClassTemplate>(LocalStorageInfoStack.back().LocalStorageTemplates)) {
            LocalOffset = static_cast<decltype(LocalOffset)>(std::get<ClassTemplate>(LocalStorageInfoStack.back().LocalStorageTemplates).LocalAttrubiteStorageTemplates.size());
            LocalClassOffset = static_cast<decltype(LocalOffset)>(std::get<ClassTemplate>(LocalStorageInfoStack.back().LocalStorageTemplates).LocalClassAttributeStorageTemplates.size());
        } else {
            throw INTERNAL_ERROR_OBJECT("Internal, local stack unknown type");
        }
    }
}

std::shared_ptr<VariableClass> VariableManager::CreateClass(std::string Name, const VariableTypeDescriptorClass &Type, double Value)
{
    (void) Value;
    return CreateSymbol(Name, Type, DefaultStorage);
}

std::shared_ptr<VariableClass> VariableManager::CreateFunction(std::string Name, const VariableTypeDescriptorClass &Type, double Value)
{
    (void) Value;
    return CreateSymbol(Name, Type, VariableClass::StorageClass::Code);
}

std::shared_ptr<VariableClass> VariableManager::CreateConstant(std::string Name, const VariableTypeDescriptorClass &Type, double Value)
{
    (void) Value;
    return CreateSymbol(Name, Type, VariableClass::StorageClass::RO);
}

std::shared_ptr<VariableClass> VariableManager::CreateVariable(std::string Name, const VariableTypeDescriptorClass &Type, double Value)
{
    (void) Value;
    return CreateSymbol(Name, Type, DefaultStorage);
}

std::shared_ptr<VariableClass> VariableManager::CreateMember(std::string Name, const VariableTypeDescriptorClass &Type, double Value)
{
    (void) Value;
    return CreateSymbol(Name, Type, VariableClass::StorageClass::RW | VariableClass::StorageClass::Class);
}

std::shared_ptr<VariableClass> VariableManager::CreateSymbol(std::string Name, const VariableTypeDescriptorClass &Type, VariableClass::StorageClass Storage)
{
    if (ContextStack.empty()) {
        throw INTERNAL_ERROR_OBJECT("No valid context");
        return nullptr;
    }
    std::shared_ptr<VariableClass> Var = ContextStack.back()->LookupVariable(Name);
    if (Var != nullptr) {
        if (ContextStack.back()->LookupVariableInThisContextOnly(Name) != nullptr) {
            throw SyntaxErrorClass("Variable '" + Name + "' Allready defined");
        } else {
            DefaultEnvironment->OutputStream() << "Warning, '" << Name << "shadows another variable\n";
        }
    }
    typedef VariableClass::StorageClass StorageClass;
    if (Local && (Storage == (StorageClass::RW | StorageClass::Local))) {
        DefaultEnvironment->DebugOutput() << "creating local <" << Name << ">\n";
        Var = std::make_shared<LocalVariableClass>(Name, Type, LocalOffset++, std::get<FktTemplate>(LocalStorageInfoStack.back().LocalStorageTemplates).LocalsParent, Storage);
        std::get<FktTemplate>(LocalStorageInfoStack.back().LocalStorageTemplates).LocalStorageTemplates.push_back(Variables::VariableContentClass::MakeEmpty(Type));
        assert(LocalOffset == std::get<FktTemplate>(LocalStorageInfoStack.back().LocalStorageTemplates).LocalStorageTemplates.size());
    } else if (Local && (Storage == (StorageClass::RW | StorageClass::Class))) {
        DefaultEnvironment->DebugOutput() << "creating member " << Name << ">\n";
        auto tmp = std::make_shared<LateBindingVariableClass>(Name, Type, LocalOffset++, /*std::get<ClassTemplate>(LocalStorageInfoStack.back().LocalStorageTemplates).LocalsParent,*/ Storage);
        Var = tmp;
        std::get<ClassTemplate>(LocalStorageInfoStack.back().LocalStorageTemplates).LocalAttrubiteStorageTemplates.push_back(Variables::VariableContentClass::MakeEmpty(Type));
        std::get<ClassTemplate>(LocalStorageInfoStack.back().LocalStorageTemplates).LocalVariableTemplates.push_back(tmp);
        assert(LocalOffset == std::get<ClassTemplate>(LocalStorageInfoStack.back().LocalStorageTemplates).LocalAttrubiteStorageTemplates.size());
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
#if 0
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
#endif

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

std::shared_ptr<VariableClass> VariableContextManageClass::RegisterVariable(const std::string Name, std::shared_ptr<VariableClass> Var, bool OverwriteAllowed)
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

std::shared_ptr<VariableClass> VariableContextManageClass::LookupVariable(const std::string Name)
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

std::shared_ptr<VariableClass> VariableContextManageClass::LookupVariableInThisContextOnly(const std::string Name)
{
    auto it = Variables.find(Name);
    if (it == Variables.end()) {
        return nullptr;
    }
    return it->second;
}

void VariableContextManageClass::Dump(std::ostream &s)
{
    s << "Context <" << GetName() << ">" << std::endl;
    s << "Parent: <" << ((ParentContext != nullptr) ? ParentContext->GetName() : std::string(" --- ")) << ">" << std::endl;
    s << "Children:";
    if (Children.empty()) {
        s << " None " << std::endl;
    } else {
        for (auto &c: Children) {
            s << c->GetName() << ",";
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

