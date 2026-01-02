#include "testclass.h"

#include "driver.hh"


TestClass::TestClass(Environment &Env, SystemInterfaceClass *SystemInterface_)
    : Env(Env), SystemInterface(SystemInterface_)
{

}

void TestClass::DoAllTests()
{
    std::string Code = "a:=2;";
    std::map<std::string, std::shared_ptr<GlobalVariableClass>> Expected;
    Expected["a"] = std::make_shared<GlobalVariableClass>("a", VariableTypeDescriptorClass(VariableTypeDescriptorClass::Type::Undefined), VariableClass::StorageClass::ReadAndWrite);
    Expected["a"]->SetValue(Variables::VariableContentClass(2LL));

        std::vector<std::string> Resultes = DoOneTest(Code, Expected);
    if (Resultes.size() == 0) {
        std::cout << "\n *** All Tests Passed ***\n";
    } else {
        std::cout << "\n ### Tests failed ###\n\n";
        for (auto &s: Resultes) {
           std::cout << s << "\n";
        }
    }
}

std::vector<std::string> TestClass::DoOneTest (std::string Codeblock, std::map<std::string, std::shared_ptr<GlobalVariableClass>> Expected)
{
    std::vector<std::string> Errors;
    std::unique_ptr<driver> drv = std::make_unique<driver>(Env, SystemInterface);
    drv->SetParserDebugLevel(0);
    drv->result.clear();
    try {
        if (drv->parse(Codeblock.c_str())) {
            Errors.push_back("Abnormal parsing end");
            return Errors;
        }
    } catch (ErrorBaseClass &e) {
        std::ostringstream os;
        os << " at Location: " << drv->location;
        Errors.push_back(std::string("Exception: ") + e.what() + os.str());
        return Errors;
    } catch (std::exception &e) {
        std::ostringstream os;
        os << " at Location: " << drv->location;
        Errors.push_back(std::string("Exception: ") + e.what() + os.str());
        return Errors;
    } catch (...) {
        std::ostringstream os;
        os << " at Location: " << drv->location;
        Errors.push_back(std::string("Unknown Exception ") + os.str());
        return Errors;
    }
    drv->Run();
    for (auto &v: Expected) {
        std::shared_ptr<VariableClass> V =
            drv->Variables.GetVariableReferenceForContext(v.first, 0);
        if (V == nullptr) {
            Errors.push_back(std::string("Variable '") + v.first + "' not found in generated context");
        } else {
            if (V->Type() == v.second->Type()) {
                if (V->GetValue() == v.second->GetValue()) {


                } else {
                    std::ostringstream os;
                    os << "Expected: " << v.second->GetValue() << ", got: " << V->GetValue();
                    Errors.push_back(std::string("Variable '") + v.first + "' has wrong content, " + os.str());
                }
            } else {
                std::ostringstream os;
                os << "Expected: " << v.second->Type() << ", got: " << V->Type();
                Errors.push_back(std::string("Variable '") + v.first + "' has wrong type, " + os.str());
            }
        }
    }
    return Errors;
}

