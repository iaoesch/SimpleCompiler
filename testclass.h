#ifndef TESTCLASS_H
#define TESTCLASS_H

#include "variableclass.h"

#include <vector>

class Environment;
class SystemInterfaceClass;

class TestClass
{
    Environment &Env;
    SystemInterfaceClass *SystemInterface;

public:
    TestClass (Environment &Env, SystemInterfaceClass *SystemInterface_);

    void DoAllTests();
private:
    std::vector<std::string> DoOneTest(std::string Codeblock, std::map<std::string, std::shared_ptr<GlobalVariableClass> > Expected);
};

#endif // TESTCLASS_H
