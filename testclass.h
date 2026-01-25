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

    typedef std::map<std::string, std::shared_ptr<GlobalVariableClass>> ExpectedVariableType;

    struct TestDataType {
       std::string Title;
       std::string Codeblock;
       ExpectedVariableType Expected;
       std::vector<std::string> Unexpected;
    };

    std::vector<TestDataType> TestVector;

public:
    TestClass (Environment &Env, SystemInterfaceClass *SystemInterface_);

    std::vector<std::string> DoAllTests();
private:
    std::vector<std::string> DoOneTest(std::string Codeblock, ExpectedVariableType Expected, std::vector<std::string> Unexpected);
    std::shared_ptr<GlobalVariableClass> MakeVariable(std::string Name, int64_t Value);
    int DoSingleTest(TestDataType const &TestData);
    std::shared_ptr<GlobalVariableClass> MakeVariable(std::string Name, std::vector<Variables::VariableContentClass> Value);
    void BuildAllTests();
    std::shared_ptr<GlobalVariableClass> MakeVariable(std::string Name, double Value);
};

#endif // TESTCLASS_H
