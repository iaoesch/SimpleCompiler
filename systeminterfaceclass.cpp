#include "systeminterfaceclass.h"
#include "PredefinedFunctionHelper.hpp"
#include "precompiledfunctionmanagerclass.h"
#include "inputdialogclass.h"

SystemInterfaceClass::SystemInterfaceClass(QObject *parent)
    : QObject{parent}
{}

void SystemInterfaceClass::Register(PrecompiledFunctionManagerClass &PrecompiledFunctionManager)
{
   std::shared_ptr<FunctionInterfaceBase> InputMethod;
  // MethodInterface M(this, &SystemInterfaceClass::Input);
   InputMethod = std::make_shared<decltype(MethodInterface(this, &SystemInterfaceClass::Input))>(this, &SystemInterfaceClass::Input);
   PrecompiledFunctionManager.RegisterFunction("INPUT", InputMethod);
}

Variables::VariableContentClass SystemInterfaceClass::Input(const std::string Title, const std::string Description, const Variables::ListClass &InputElements)
{
    InputDialogClass::DialogDescriptor Descriptor;
    Descriptor.Title = Title;
    Descriptor.Description = Description;
    for (auto const &d: InputElements) {
        if (d->holds_alternative<Variables::ListClass>()) {
            auto &e = d->GetValue<Variables::ListClass>();
            InputDialogClass::ElementType FieldDescriptor;
            if (e.size() > 1) {
                if (e[0]->holds_alternative<std::string>()) {
                    FieldDescriptor.Label = e[0]->GetValue<std::string>();
                } else {
                    throw RuntimeErrorClass("expected string as first entry for Input ", -1);
                }
                if (e[1]->holds_alternative<TypeDescriptorClass>()) {
                    const TypeDescriptorClass &t = e[1]->GetValue<TypeDescriptorClass>();
                    if (t == TypeDescriptorClass::Type::Float) {
                        FieldDescriptor.Type = InputDialogClass::ElementType::Float;
                    } else if (t == TypeDescriptorClass::Type::Integer) {
                        FieldDescriptor.Type = InputDialogClass::ElementType::Int;
                    } else if (t == TypeDescriptorClass::Type::String) {
                        FieldDescriptor.Type = InputDialogClass::ElementType::String;
                    } else {
                        throw RuntimeErrorClass("unsupported type for Input ", -1);
                    }
                } else {
                    throw RuntimeErrorClass("expected type as second entry for Input ", -1);
                }
                FieldDescriptor.Default = std::monostate();
                if (e.size() == 3) {
                    if (e[2]->holds_alternative<std::string>()) {
                       FieldDescriptor.Default = e[2]->GetValue<std::string>();
                    } else if (e[2]->holds_alternative<int64_t>()) {
                        FieldDescriptor.Default = e[2]->GetValue<int64_t>();
                    } else if (e[2]->holds_alternative<double>()) {
                        FieldDescriptor.Default = e[2]->GetValue<double>();
                    } else {
                        // ignore for now
                    }

                } else if (e.size() > 3){
                    throw RuntimeErrorClass("too many (>3) arguments for Input ", -1);
                }
            } else {
                throw RuntimeErrorClass("too few arguments for Input ", -1);
            }
            Descriptor.InputFieldDescriptors.push_back(FieldDescriptor);
        } else {
            throw RuntimeErrorClass("Invalid type for Input (List expected)", -1);
        }
    }
    std::vector<InputDialogClass::ValueType> Result;
    emit ShowInputDialog(&Descriptor, &Result);
    Variables::ListClass TransformedResults;
    for (auto &r: Result) {
        if (std::holds_alternative<std::string>(r)) {
            TransformedResults.Append(std::make_unique<Variables::VariableContentClass>(std::get<std::string>(r)));
        } else if (std::holds_alternative<int64_t>(r)) {
            TransformedResults.Append(std::make_unique<Variables::VariableContentClass>(std::get<int64_t>(r)));
        } else if (std::holds_alternative<double>(r)) {
            TransformedResults.Append(std::make_unique<Variables::VariableContentClass>(std::get<double>(r)));
        } else {
            TransformedResults.Append(std::make_unique<Variables::VariableContentClass>(Variables::VariableContentClass::MakeUndefined()));
        }
    }
    return TransformedResults;
}
