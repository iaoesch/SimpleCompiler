
#include "classnodehelper.h"
#include "Errclass.hpp"
#include "varmanag.hpp"
#include "variablecontentclass.h"
#include "compact.h"

void ClassNodeHelper::StartClassDefinition(std::string ClassName)
{
    PendingClassDefinitions.push_back(ClassName);
    std::shared_ptr<VariableClass> NewClass = Variables.CreateClass(ClassName, VariableTypeDescriptorClass(TypeDescriptorClass::Type::Class),0.0);;
    if (NewClass == nullptr) {
        throw SyntaxErrorClass("Class '" + ClassName + "' not created");
    }
    PendingClassDefinitions.back().NewClass = NewClass;
}

void ClassNodeHelper::EndClassDefinition()
{
    if (PendingClassDefinitions.empty()) {
        throw INTERNAL_ERROR_OBJECT("EndClassDefinition without pending classdefinition");
    }
    PendingClassDefinitions.pop_back();
    Variables.LeaveContext();
}

void ClassNodeHelper::StartMemberDefinition()
{
    if (PendingClassDefinitions.empty()) {
        throw INTERNAL_ERROR_OBJECT("StartMemberDefinition without pending classdefinition");
    }
    Variables.StartClass(PendingClassDefinitions.back().NewClassContent);
    PendingClassDefinitions.back().ClassContext = Variables.CreateNewContext(PendingClassDefinitions.back().Name + "Members");
    PendingClassDefinitions.back().NewClassContent->setContextForParsing(PendingClassDefinitions.back().ClassContext);

}

void ClassNodeHelper::EndMemberDefinition()
{
    Variables.EndClass();
}

bool ClassNodeHelper::SetBaseClass(std::string Name)
{
    if (PendingClassDefinitions.empty()) {
        throw INTERNAL_ERROR_OBJECT("SetBaseClass without pending classdefinition");
    }
    if (Name != "") {
        std::shared_ptr<VariableClass> BaseClass = Variables.GetVariableReference(Name);
        if (BaseClass == nullptr) {
            throw SyntaxErrorClass("Baseclass '" + Name + "' not found");
        }
        if (BaseClass->ContaindedType() != TypeDescriptorClass::Type::Class) {
            throw SyntaxErrorClass("Identifier '" + Name + "' refers not a class");
        }
        if (PendingClassDefinitions.back().BaseClass != nullptr) {
            throw INTERNAL_ERROR_OBJECT("SetBaseClass with allready set baseclass");
        }
        if (BaseClass->GetValue().holds_alternative<std::shared_ptr<Variables::ClassClass>>() == false) {
            throw SyntaxErrorClass("Identifier '" + Name + "' holds not a class");
        }
        PendingClassDefinitions.back().BaseClass = BaseClass->GetValue().GetValue<std::shared_ptr<Variables::ClassClass>>();
    } else {
        if (PendingClassDefinitions.back().BaseClass != nullptr) {
            throw INTERNAL_ERROR_OBJECT("SetBaseClass with allready set baseclass");
        }
    }
    std::shared_ptr<Variables::ClassClass> NewClassContent = std::make_shared<Variables::ClassClass>(PendingClassDefinitions.back().Name, PendingClassDefinitions.back().BaseClass);
    PendingClassDefinitions.back().NewClassContent = NewClassContent;
    PendingClassDefinitions.back().NewClass->SetInitialValue(Variables::VariableContentClass(NewClassContent));
    return true;
}

bool ClassNodeHelper::AddClassAttribute(std::string Name, std::shared_ptr<VariableClass> ClassAttribute)
{
    if (PendingClassDefinitions.empty()) {
        throw INTERNAL_ERROR_OBJECT("AddClassAttribute without pending classdefinition");
    }
    auto const&[it, Success] = PendingClassDefinitions.back().ClassAttributes.insert({Name, ClassAttribute});
    return Success;
}

bool ClassNodeHelper::AddObjectAttribute(std::string Name, std::shared_ptr<VariableClass> Attribute)
{
    if (PendingClassDefinitions.empty()) {
        throw INTERNAL_ERROR_OBJECT("AddObjectAttribute without pending classdefinition");
    }
    auto const&[it, Success] = PendingClassDefinitions.back().ObjectAttributes.insert({Name, Attribute});
    return Success;
}

std::unique_ptr<VariableTypeDescriptorClass> ClassNodeHelper::MakeTypeFromClassName(std::string ClassName)
{
    std::shared_ptr<VariableClass> Var = Variables.GetVariableReference(ClassName);
    if (Var == nullptr) {
        throw(SyntaxErrorClass("Class '" + ClassName + "' not fond"));
    }
    if (Var->Type() != TypeDescriptorClass::Type::Class) {
        throw(SyntaxErrorClass("'" + ClassName + "' is not a class"));
    }
    if ( ! Var->GetValue().holds_alternative<std::shared_ptr<Variables::ClassClass>>()) {
        throw(SyntaxErrorClass("'" + ClassName + "' refers not to a class"));
    }
    std::shared_ptr<Variables::ClassClass> TheClass = Var->GetValue().GetValue<std::shared_ptr<Variables::ClassClass>>();
    return std::make_unique<VariableTypeDescriptorClass>(ObjectReferenceDescriptorClass(TheClass));
}

std::shared_ptr<InstanceConstructionClass> ClassNodeHelper::MakeObjectFromClassName(std::string ClassName, const LocationType &l)
{
    throw(INTERNAL_ERROR_OBJECT("Not implemented and probably not used ever"));
    std::shared_ptr<VariableClass> Var = Variables.GetVariableReference(ClassName);
    if (Var == nullptr) {
        throw(SyntaxErrorClass("Class '" + ClassName + "' not fond"));
    }
    if (Var->Type() != TypeDescriptorClass::Type::Class) {
        throw(SyntaxErrorClass("'" + ClassName + "' is not a class"));
    }
    if ( ! Var->GetValue().holds_alternative<std::shared_ptr<Variables::ClassClass>>()) {
        throw(SyntaxErrorClass("'" + ClassName + "' refers not to a class"));
    }
    std::shared_ptr<Variables::ClassClass> TheClass = Var->GetValue().GetValue<std::shared_ptr<Variables::ClassClass>>();
   // return std::make_shared<InstanceConstructionClass>(TheClass, l);
}
