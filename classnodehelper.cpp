
#include "classnodehelper.h"
#include "Errclass.hpp"
#include "varmanag.hpp"
#include "variablecontentclass.h"

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
    Variables.CreateNewContext(PendingClassDefinitions.back().Name + "Params");
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
