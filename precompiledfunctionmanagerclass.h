#ifndef PRECOMPILEDFUNCTIONMANAGERCLASS_H
#define PRECOMPILEDFUNCTIONMANAGERCLASS_H

#include "typedescriptorclass.hpp"
#include "variablecontentclass.h"
#include <string>
namespace Variables {
   class Callable;
}
class VariableManager;


class FunctionInterfaceBase : public Variables::Callable{

public:
    struct FunctionsParameterDescriptor {
        std::string Name;
        VariableTypeDescriptorClass TypeDescriptor;
        Variables::VariableContentClass DefaultValue;
    };

    typedef std::vector<FunctionsParameterDescriptor> ParameterListType;

    ParameterListType GetParameterDescriptorList() {return ParameterList;}
    VariableTypeDescriptorClass GetReturnType() {return Returntype;}


protected:
    VariableTypeDescriptorClass Returntype;
    ParameterListType           ParameterList;

};



class PrecompiledFunctionManagerClass
{
    VariableManager &Variables;

public:


    PrecompiledFunctionManagerClass(VariableManager &Variables_);
    void RegisterFunction(std::string Name, std::shared_ptr<FunctionInterfaceBase> Fkt);
};

#endif // PRECOMPILEDFUNCTIONMANAGERCLASS_H
