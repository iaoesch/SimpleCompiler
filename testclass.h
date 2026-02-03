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
    enum ExceptionFilter {NoException = 0, StdException = 1, ParsingException = 2, RuntimeException = 4, OtherException = 128};

    struct TestDataType {
       std::string Title;
       std::string Codeblock;
       ExpectedVariableType Expected;
       std::vector<std::string> Unexpected;
       ExceptionFilter ExpectedException;
    };

    std::vector<TestDataType> TestVector;

public:
    TestClass (Environment &Env, SystemInterfaceClass *SystemInterface_);

    std::vector<std::string> DoAllTests();
private:


    std::vector<std::string> DoOneTest(std::string Codeblock, ExpectedVariableType Expected, std::vector<std::string> Unexpected, ExceptionFilter ExpectedExceptions);
    std::shared_ptr<GlobalVariableClass> MakeVariable(std::string Name, int64_t Value);
    int DoSingleTest(TestDataType const &TestData);
    std::shared_ptr<GlobalVariableClass> MakeVariable(std::string Name, std::vector<Variables::VariableContentClass> Value, std::shared_ptr<Variables::ClassClass> Parent = nullptr);
    void BuildAllTests();
    std::shared_ptr<GlobalVariableClass> MakeVariable(std::string Name, double Value);
};

#endif // TESTCLASS_H
