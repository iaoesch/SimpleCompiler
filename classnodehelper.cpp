
#include "classnodehelper.h"
#include "Errclass.hpp"
#include "varmanag.hpp"

bool ClassNodeHelper::SetBaseClass(std::string Name)
{
    if (PendingClassDefinitions.empty()) {
        throw INTERNAL_ERROR_OBJECT("SetBaseClass without pending classdefinition");
    }
    std::shared_ptr<VariableClass> BaseClass = Variables.GetVariableReference(Name);
    if (BaseClass == nullptr) {
        throw SyntaxErrorClass("Baseclass '" + Name + "' not found");
    }
    if (BaseClass->ContaindedType() != TypeDescriptorClass::Type::Class) {
        throw SyntaxErrorClass("Identifier '" + Name + "' refers not a class");
    }
    if (PendingClassDefinitions.back().BaseClass != nullptr) {
        throw INTERNAL_ERROR_OBJECT("SetBaseClass without allready set baseclass");
    }
    if (BaseClass->GetValue().holds_alternative<std::shared_ptr<Variables::ClassClass>>() == false) {
        throw SyntaxErrorClass("Identifier '" + Name + "' holds not a class");
    }
    PendingClassDefinitions.back().BaseClass = BaseClass->GetValue().GetValue<std::shared_ptr<Variables::ClassClass>>();
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
