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
   PrecompiledFunctionManager.RegisterFunction("input", InputMethod);
}

Variables::VariableContentClass SystemInterfaceClass::Input(std::string Title, std::string Description, Variables::ListClass &InputElements)
{
    InputDialogClass::DialogDescriptor Descriptor;
    Descriptor.Title = Title;
    Descriptor.Description = Description;
    for (auto &d: InputElements) {

    }
}
