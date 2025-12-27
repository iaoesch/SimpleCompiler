#ifndef PRECOMPILEDFUNCTIONMANAGERCLASS_H
#define PRECOMPILEDFUNCTIONMANAGERCLASS_H

#include "typedescriptorclass.hpp"
#include "variablecontentclass.h"
#include <string>
namespace Variables {
   class Callable;
}
class VariableManager;

class PrecompiledFunctionManagerClass
{
    VariableManager &Variables;

public:

    struct FunctionsParameterDescriptor {
        std::string Name;
        VariableTypeDescriptorClass TypeDescriptor;
        Variables::VariableContentClass DefaultValue;
    };

    struct FunctionsDescriptor {
        FunctionsDescriptor(Variables::Callable &Function_, const VariableTypeDescriptorClass &ReturnType_) : Function(Function_), Returntype(ReturnType_){}
        std::vector<FunctionsParameterDescriptor> Parameterlist;
        Variables::Callable &Function;
        VariableTypeDescriptorClass Returntype;
    };


    PrecompiledFunctionManagerClass(VariableManager &Variables_);
    void RegisterFunction(std::string Name, FunctionsDescriptor &Fkt);
};

#endif // PRECOMPILEDFUNCTIONMANAGERCLASS_H
