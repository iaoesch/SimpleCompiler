#include "testclass.h"

#include "driver.hh"



TestClass::TestClass(Environment &Env, SystemInterfaceClass *SystemInterface_)
    : Env(Env), SystemInterface(SystemInterface_)
{
    BuildAllTests();
}

std::shared_ptr<GlobalVariableClass> TestClass::MakeVariable(std::string Name, int64_t Value)
{
   std::shared_ptr<GlobalVariableClass> Var = std::make_shared<GlobalVariableClass>(Name, VariableTypeDescriptorClass(VariableTypeDescriptorClass::Type::Undefined), VariableClass::StorageClass::RW | VariableClass::StorageClass::Local);
   Var->SetValue(Variables::VariableContentClass(Value));
   return Var;
}

std::shared_ptr<GlobalVariableClass> TestClass::MakeVariable(std::string Name, double Value)
{
    std::shared_ptr<GlobalVariableClass> Var = std::make_shared<GlobalVariableClass>(Name, VariableTypeDescriptorClass(VariableTypeDescriptorClass::Type::Undefined), VariableClass::StorageClass::RW | VariableClass::StorageClass::Local);
    Var->SetValue(Variables::VariableContentClass(Value));
    return Var;
}

std::shared_ptr<GlobalVariableClass> TestClass::MakeVariable(std::string Name, std::vector<Variables::VariableContentClass> Value)
{
    std::shared_ptr<GlobalVariableClass> Var = std::make_shared<GlobalVariableClass>(Name, VariableTypeDescriptorClass(VariableTypeDescriptorClass::Type::Undefined), VariableClass::StorageClass::RW | VariableClass::StorageClass::Local);
    std::shared_ptr<Variables::ClassClass> NewClass = std::make_shared<Variables::ClassClass>(Name, nullptr);
    for (auto &r: Value) {
        NewClass->GetObjectStorageInitialValues().push_back(r);
        NewClass->GetObjectVariableReferences().push_back(std::make_shared<AttributeIndexVariableClass>("Hans", r.getType(), 1, NewClass, VariableClass::StorageClass::RW | VariableClass::StorageClass::Local));
    }
    Var->SetValue(Variables::VariableContentClass(NewClass));
    return Var;
}

void TestClass::BuildAllTests()
{
    VariableClass::SetDefaultEnvironment(Env);
    TestVector;
    std::map<std::string, std::shared_ptr<GlobalVariableClass>> Expected;
    std::vector<std::string> Unexpected;
    int Errors = 0;

    Unexpected.push_back("x");
    std::string Code = "a:=2;";
    Expected["a"] = MakeVariable("a", 2LL);
    TestVector.push_back({"Assignment", Code, Expected, Unexpected, NoException});

    Code = "a:=5.5;";
    Expected["a"] = MakeVariable("a", 5.5);
    TestVector.push_back({"Assignment", Code, Expected, Unexpected, NoException});

    Code = "a:=5; \nb:=3; \nc:=a+b;";
    Expected["a"] = MakeVariable("a", 5LL);
    Expected["b"] = MakeVariable("b", 3LL);
    Expected["c"] = MakeVariable("c", 8LL);
    TestVector.push_back({"Assignment", Code, Expected, Unexpected, NoException});

    Code = "a:=5; \nb:=3; \nc:=7; \nd:=a+b;\nd:=d+c;";
    Expected["a"] = MakeVariable("a", 5LL);
    Expected["b"] = MakeVariable("b", 3LL);
    Expected["c"] = MakeVariable("c", 7LL);
    Expected["d"] = MakeVariable("d", 15LL);
    TestVector.push_back({"Addition chain", Code, Expected, Unexpected, NoException});

    Code = "a:=5; \nb:=3; \nc:=7; \nd:=a+b+c;";
    Expected["a"] = MakeVariable("a", 5LL);
    Expected["b"] = MakeVariable("b", 3LL);
    Expected["c"] = MakeVariable("c", 7LL);
    Expected["d"] = MakeVariable("d", 15LL);
    TestVector.push_back({"Addition chain", Code, Expected, Unexpected, NoException});

    Code = "a:=5; \n"
           "b:=3; \n"
           "c:=7; \n"
           "if (a==b) then\n"
           "  d:=a;\n"
           "else\n"
           "  d:=c;\n"
           "endif;\n";
    Expected["a"] = MakeVariable("a", 5LL);
    Expected["b"] = MakeVariable("b", 3LL);
    Expected["c"] = MakeVariable("c", 7LL);
    Expected["d"] = MakeVariable("d", 7LL);
    TestVector.push_back({"if", Code, Expected, Unexpected, NoException});

    Code = "a:=5; \n"
           "b:=5; \n"
           "c:=7; \n"
           "if (a==b) then\n"
           "  d:=a;\n"
           "else\n"
           "  d:=c;\n"
           "endif;\n";
    Expected["a"] = MakeVariable("a", 5LL);
    Expected["b"] = MakeVariable("b", 5LL);
    Expected["c"] = MakeVariable("c", 7LL);
    Expected["d"] = MakeVariable("d", 5LL);
    TestVector.push_back({"if", Code, Expected, Unexpected, NoException});

    Code = "a:=5; \n"
           "b:=3; \n"
           "c:=a+b; \n"
           "y:= 17;\n"
           "function test returning integer (a)\n"
           "   x:=b; \n"
           "   b:=19;\n "
           "   returning a+2; \n" //  Should end function
           "   b:=20;\n "         // Not executed
           "   returning a+19; \n"// Not executed
           "   b:=21; \n"         // Not executed
           "endfunction \n"
           "z:=test(99);\n";
    Expected.clear();
    Expected["a"] = MakeVariable("a", 5LL);
    Expected["b"] = MakeVariable("b", 19LL);
    Expected["c"] = MakeVariable("c", 8LL);
    Expected["y"] = MakeVariable("y", 17LL);
    Expected["z"] = MakeVariable("z", 101LL);
    TestVector.push_back({"Assignment", Code, Expected, Unexpected, NoException});

    Code = "a:=5; \n"
           "b:=3; \n"
           "c:=a+b; \n"
           "x:= 17;\n"
           "function test returning integer (a)\n"
           "   w:=b; \n"
           "   x:=w; \n"
           "   returning a+2; \n"
           "endfunction \n"
           "z:=test(99);\n";
    Expected.clear();
    Expected["a"] = MakeVariable("a", 5LL);
    Expected["b"] = MakeVariable("b", 3LL);
    Expected["c"] = MakeVariable("c", 8LL);
    Expected["x"] = MakeVariable("x", 3LL);
    Expected["z"] = MakeVariable("z", 101LL);
    Unexpected.clear();
    Unexpected.push_back("w");
    TestVector.push_back({"Assignment", Code, Expected, Unexpected, NoException});

    Code = "a:=5; \n"
           "b:=3; \n"
           "c:=a+b; \n"
           "xt1wt1:= 17;\n"
           "xt1wt1s1:= 17;\n"
           "xt1wt1s1s1:= 17;\n"
           "xt1wt1s1s2:= 17;\n"
           "xt2wt2:= 17;\n"
           "yt2wt2:= 0;\n"
           "xt2wt2s1:= 17;\n"
           "xt2wt1s2:= 17;\n"
           "xt1wt1s2:= 17;\n"

           "x4:= 17;\n"
           "x5:= 17;\n"
           "function test2 returning integer (a)\n"
           "   wt2:=a+6; \n"
           "   function test2sub1 returning integer (a)\n"
           "      wt2s1:=a + 8; \n"
           "      xt2wt2s1:=wt2s1; \n"
           "      returning a+128; \n"
           "   endfunction \n"
           "   x:=wt2; \n"
           "   r:=test2sub1(a+50); \n"
           "   xt2wt2:=wt2; \n"
           "   yt2wt2:=yt2wt2+wt2; \n"
           "   returning r+32; \n"
           "endfunction \n"
           "function test1 returning integer (a)\n"
           "   wt1:=a+1; \n"
           "   function test1sub2 returning integer (a)\n"
           "      wt1s2:=a+7; \n"
           "      x:=wt1s2; \n"
           "      r:=test2(a+10); \n"
           "      xt1wt1s2:=wt1s2; \n"
           "      returning r+16; \n"
           "   endfunction \n"
           "   function test1sub1 returning integer (a)\n"
           "      wt1s1:=a+1; \n"
           "      q:=a+2; \n"
           "      function test1sub1sub1 returning integer (a)\n"
           "         wt1s1s1:=a+4; \n"
           "         xt1wt1s1s1:=wt1s1s1; \n"
           "         r1:=test2(a+100); \n"
           "         r2:=test1sub2(a+200); \n"
           "         returning r1+r2+8; \n"
           "      endfunction \n"
           "      function test1sub1sub2 returning integer (a)\n"
           "         wt1s1s2:=a+3; \n"
           "         r:=test1sub1sub1(a+1000); \n"
           "         xt1wt1s1s2:=wt1s1s2; \n"
           "         returning r+4; \n"
           "      endfunction \n"
           "      wt1s1:=q; \n"
           "      xt1wt1s1:=wt1s1; \n"
           "      r:=test1sub1sub2(a+10000); \n"
           "      returning r+2; \n"
           "   endfunction \n"
           "   xt1wt1:=999; \n"
           "   r:=test1sub1(a+100000); \n"
           "   xt1wt1:=wt1; \n"
           "   returning r + 1; \n"
           "endfunction \n"
           "z1:=test1(1000000);\n"
           "z2:=test1(2000000);\n";
    Expected.clear();
    Expected["a"] = MakeVariable("a", 5LL);
    Expected["b"] = MakeVariable("b", 3LL);
    Expected["c"] = MakeVariable("c", 8LL);
    Expected["xt1wt1"]     = MakeVariable("x", 2000001LL);
    Expected["xt1wt1s1"]   = MakeVariable("x", 2100002LL);
    Expected["xt1wt1s1s2"] = MakeVariable("x", 2110003LL);
    Expected["xt1wt1s1s1"] = MakeVariable("x", 2111004LL);
    //Expected["xt2wt2"]     = MakeVariable("x", 2111106LL);
    //Expected["yt2wt2"]     = MakeVariable("x", 2111106LL);
    //Expected["xt2wt2s1"]   = MakeVariable("x", 2111156LL);
    Expected["xt1wt1s2"]   = MakeVariable("x", 2111207LL);
    Expected["xt2wt2"]     = MakeVariable("x", 2111216LL);
    Expected["xt2wt2s1"]   = MakeVariable("x", 2111268LL);
    Expected["yt2wt2"]     = MakeVariable("x", 1111106LL + 1111216LL + 2111106LL + 2111216LL);



    Expected["z1"] = MakeVariable("z", 1111260LL + 1111150LL + 1+2+4+8+16+32+32+128+128);
    Expected["z2"] = MakeVariable("z", 2111260LL + 2111150LL + 1+2+4+8+16+32+32+128+128);
    Unexpected.clear();
    Unexpected.push_back("w");
    TestVector.push_back({"Argument an returnvalue passing", Code, Expected, Unexpected, NoException});

    Code = "a:=5; \n"
           "b:=3; \n"
           "c:=a+b; \n"
           "v:= 17;\n"
           "x:= 17;\n"
           "y:= 17;\n"
           "function test returning integer (a)\n"
           "   w:=b+10; \n"
           "   x:=w; \n"
           "   function testlocal returning integer (b)\n"
           "      q:=w; \n"
           "      v:=c; \n"
           "      w:=99; \n"
           "      returning a+2; \n"
           "   endfunction \n"
           "   r:=testlocal(a);\n"
           "   y:=w; \n"
           "   returning r+10; \n"
           "endfunction \n"
           "z:=test(100);\n";
    Expected.clear();
    Expected["a"] = MakeVariable("a", 5LL);
    Expected["b"] = MakeVariable("b", 3LL);
    Expected["c"] = MakeVariable("c", 8LL);
    Expected["v"] = MakeVariable("x", 8LL);
    Expected["x"] = MakeVariable("x", 13LL);
    Expected["y"] = MakeVariable("x", 99LL);
    Expected["z"] = MakeVariable("z", 112LL);
    Unexpected.clear();
    Unexpected.push_back("w");
    TestVector.push_back({"Parent variable access", Code, Expected, Unexpected , NoException});

    Code = "a:=5; \n"
           "b:=3; \n"
           "c:=a+b; \n"
           "v:= 17;\n"
           "x:= 17;\n"
           "y:= 17;\n"
           "function test returning integer (a)\n"
           "   w:=b+10; \n"
           "   x:=w; \n"
           "   function testlocal returning integer (b)\n"
           "      function testlocallocal returning integer (b)\n"
           "         q:=w; \n"
           "         v:=c; \n"
           "         w:=99; \n"
           "         returning b+2; \n"
           "      endfunction \n"
           "      r:=testlocallocal(b);\n"
           "      returning r+30; \n"
           "   endfunction \n"
           "   r:=testlocal(a);\n"
           "   y:=w; \n"
           "   returning r+100; \n"
           "endfunction \n"
           "z:=test(7000);\n";
    Expected.clear();
    Expected["a"] = MakeVariable("a", 5LL);
    Expected["b"] = MakeVariable("b", 3LL);
    Expected["c"] = MakeVariable("c", 8LL);
    Expected["v"] = MakeVariable("x", 8LL);
    Expected["x"] = MakeVariable("x", 13LL);
    Expected["y"] = MakeVariable("x", 99LL);
    Expected["z"] = MakeVariable("z", 7132LL);
    Unexpected.clear();
    Unexpected.push_back("w");
    TestVector.push_back({"Parent variable access", Code, Expected, Unexpected, NoException});


    Code = "a:=5; \n"
           "b:=3; \n"
           "c:=a+b; \n"
           "v:= 17;\n"
           "x:= 17;\n"
           "y:= 17;\n"
           "function test returning integer (a)\n"
           "   w:=b+10; \n"
           "   x:=w; \n"
           "      function testlocallocal2 returning integer (b)\n"
           "         q:=w; \n"
           "         v:=c; \n"
           "         w:=99; \n"
           "         returning b+3; \n"
           "      endfunction \n"
           "   function testlocal returning integer (b)\n"
           "      function testlocallocal returning integer (b)\n"
           "         r:=testlocallocal2(b);\n"
           "         returning r+2; \n"
           "      endfunction \n"
           "      r:=testlocallocal(b);\n"
           "      returning r+30; \n"
           "   endfunction \n"
           "   r:=testlocal(a);\n"
           "   y:=w; \n"
           "   returning r+100; \n"
           "endfunction \n"
           "z:=test(7000);\n";
    Expected.clear();
    Expected["a"] = MakeVariable("a", 5LL);
    Expected["b"] = MakeVariable("b", 3LL);
    Expected["c"] = MakeVariable("c", 8LL);
    Expected["v"] = MakeVariable("x", 8LL);
    Expected["x"] = MakeVariable("x", 13LL);
    Expected["y"] = MakeVariable("x", 99LL);
    Expected["z"] = MakeVariable("z", 7135LL);
    Unexpected.clear();
    Unexpected.push_back("w");
    TestVector.push_back({"Parent variable access", Code, Expected, Unexpected, NoException});

    Code = "class TestClass \n"
           "a as integer; \n"
           "endclass; \n";

    Expected.clear();
    Expected["TestClass"] = MakeVariable("a", std::vector<Variables::VariableContentClass>{Variables::VariableContentClass::MakeEmpty(ValueTypeDescriptorClass(TypeDescriptorClass::Type::Integer))});
    Unexpected.clear();
    TestVector.push_back({"class definition", Code, Expected, Unexpected, NoException});

    Code = "class TestClass \n"
           "a as integer; \n"
           "endclass; \n"
           "method TestMethod taking a,b returning integer of class TestClass:\n"
           "x := a + b;\n"
           "endmethod\n";

    Expected.clear();
    Expected["TestClass"] = MakeVariable("a", std::vector<Variables::VariableContentClass>{Variables::VariableContentClass::MakeEmpty(ValueTypeDescriptorClass(TypeDescriptorClass::Type::Integer))});
    Unexpected.clear();
    TestVector.push_back({"Method definition", Code, Expected, Unexpected, NoException});

    Code = "class TestClass \n"
           "a as integer; \n"
           "endclass; \n"
           "method TestMethod taking a,b returning integer of class TestClass:\n"
           "x := a + b;\n"
           "endmethod\n"
           "o as TestClass;\n"
           "o := new TestClass;\n"
           "print(o);\n"
        ;

    Expected.clear();
    Expected["TestClass"] = MakeVariable("a", std::vector<Variables::VariableContentClass>{Variables::VariableContentClass::MakeEmpty(ValueTypeDescriptorClass(TypeDescriptorClass::Type::Integer))});
    Unexpected.clear();
    TestVector.push_back({"object definition", Code, Expected, Unexpected, NoException});


    Code = "class TestClass \n"
           "w as integer; \n"
           "endclass; \n"
           "method TestMethod taking a,b returning integer of class TestClass:\n"
           "x := a + b;\n"
           "returning x;\n"
           "endmethod\n"
           "o as TestClass;\n"
           "o := new TestClass;\n"
           "print(o);\n"
           "z := tell o to TestMethod with [a:=7, b:=11];"
           "print(z);\n"
        ;

    Expected.clear();
    Expected["TestClass"] = MakeVariable("a", std::vector<Variables::VariableContentClass>{Variables::VariableContentClass::MakeEmpty(ValueTypeDescriptorClass(TypeDescriptorClass::Type::Integer))});
    Expected["z"] = MakeVariable("z", 18LL);
    Unexpected.clear();
    Unexpected.push_back("a");
    Unexpected.push_back("b");
    Unexpected.push_back("x");
    Unexpected.push_back("w");

    TestVector.push_back({"object method call", Code, Expected, Unexpected, NoException});

    Code = "class TestClass \n"
           "w as integer; \n"
           "endclass; \n"
           "method StoreSumm taking a,b returning integer of class TestClass:\n"
           "w := a + b;\n"
           "returning 13;\n"
           "endmethod\n"
           "method GetSumm taking a,b returning integer of class TestClass:\n"
           "returning w;\n"
           "endmethod\n"
           "o as TestClass;\n"
           "o := new TestClass;\n"
           "print(o);\n"
           "z := tell o to StoreSumm with [a:=7, b:=11];\n"
           "print(o);\n"
           "y := tell o to GetSumm   with [a:=99, b:=113];\n"
           "print(z);\n"
           "print(y);\n"
        ;

    Expected.clear();
    Expected["TestClass"] = MakeVariable("a", std::vector<Variables::VariableContentClass>{Variables::VariableContentClass::MakeEmpty(ValueTypeDescriptorClass(TypeDescriptorClass::Type::Integer))});
    Expected["y"] = MakeVariable("y", 18LL);
    Expected["z"] = MakeVariable("z", 13LL);
    Unexpected.clear();
    Unexpected.push_back("a");
    Unexpected.push_back("b");
    Unexpected.push_back("x");
    Unexpected.push_back("w");

    TestVector.push_back({"object method call using this", Code, Expected, Unexpected, NoException});

    Code = "class TestClass \n"
           "w as integer; \n"
           "endclass; \n"
           "method StoreSumm taking a,b returning integer of class TestClass:\n"
           "w := a + b;\n"
           "returning 13;\n"
           "endmethod\n"
           "method GetSumm taking a,b returning integer of class TestClass:\n"
           "returning w;\n"
           "endmethod\n"
           "o as TestClass;\n"
           "o := new TestClass;\n"
           "print(o);\n"
           "z := tell o to StoreSumm with [a:=7, b:=11];\n"
           "print(o);\n"
           "p as TestClass;\n"
           "p := o;\n"
           "y := tell p to GetSumm   with [a:=99, b:=113];\n"
           "print(z);\n"
           "print(y);\n"
        ;

    Expected.clear();
    Expected["TestClass"] = MakeVariable("a", std::vector<Variables::VariableContentClass>{Variables::VariableContentClass::MakeEmpty(ValueTypeDescriptorClass(TypeDescriptorClass::Type::Integer))});
    Expected["y"] = MakeVariable("y", 18LL);
    Expected["z"] = MakeVariable("z", 13LL);
    Unexpected.clear();
    Unexpected.push_back("a");
    Unexpected.push_back("b");
    Unexpected.push_back("x");
    Unexpected.push_back("w");

    TestVector.push_back({"object passing", Code, Expected, Unexpected, NoException});

    Code = "class TestClass \n"
           "w as integer; \n"
           "endclass; \n"
           "method StoreSumm taking a,b returning integer of class TestClass:\n"
           "w := a + b;\n"
           "returning 13;\n"
           "endmethod\n"
           "method GetSumm taking a,b returning integer of class TestClass:\n"
           "returning w;\n"
           "endmethod\n"
           "o as TestClass;\n"
           "o := new TestClass;\n"
           "print(o);\n"
           "z := tell o to StoreSumm with [a:=7, b:=11];\n"
           "print(o);\n"
           "p as TestClass;\n"
           "y := tell p to GetSumm   with [a:=99, b:=113];\n"
           "print(z);\n"
           "print(y);\n"
        ;

    Expected.clear();
    Expected["TestClass"] = MakeVariable("a", std::vector<Variables::VariableContentClass>{Variables::VariableContentClass::MakeEmpty(ValueTypeDescriptorClass(TypeDescriptorClass::Type::Integer))});
    Expected["y"] = MakeVariable("y", 18LL);
    Expected["z"] = MakeVariable("z", 13LL);
    Unexpected.clear();
    Unexpected.push_back("a");
    Unexpected.push_back("b");
    Unexpected.push_back("x");
    Unexpected.push_back("w");

    TestVector.push_back({"object passing", Code, Expected, Unexpected, RuntimeException});

}

std::vector<std::string> TestClass::DoAllTests()
{
    std::vector<std::string> FailedTests;
    int Errors = 0;
    for (auto &Test: TestVector) {
        int Result = DoSingleTest(Test);
        Errors += Result;
        if (Result != 0) {
           FailedTests.push_back(Test.Codeblock);
        }
    }
    if (Errors == 0) {
        std::cout << "\n *** All Tests Passed ***\n";
    } else {
        std::cout << "\n ### " << Errors << " Tests failed ###\n\n";

    }
    return FailedTests;
}

int TestClass::DoSingleTest(TestDataType const &TestData)
{

    std::cout << "\n *** Ececuting Test " + TestData.Title + " *** ...";
    std::vector<std::string> Results = DoOneTest(TestData.Codeblock, TestData.Expected, TestData.Unexpected, TestData.ExpectedException);
    if (Results.size() == 0) {
        std::cout << " *** Test Passed ***\n";
    } else {
        std::cout << "\n ### Test failed ###\n\n";
        for (auto &s: Results) {
            std::cout << s << "\n";
        }
    }
    return Results.empty()?0:1;
}

std::vector<std::string> TestClass::DoOneTest (std::string Codeblock, ExpectedVariableType Expected, std::vector<std::string> Unexpected, ExceptionFilter ExpectedExceptions)
{
    std::vector<std::string> Errors;
    std::unique_ptr<driver> drv = std::make_unique<driver>(Env, SystemInterface);
    drv->SetParserDebugLevel(0);
    drv->result.clear();
    try {
        if (drv->parse(Codeblock.c_str())) {
            if ((ExpectedExceptions & ParsingException) != 0) {
                return {};
            }
            Errors.push_back("Abnormal parsing end");
            return Errors;
        }
    } catch (ErrorBaseClass &e) {
        if ((ExpectedExceptions & ParsingException) != 0) {
            return {};
        }
        std::ostringstream os;
        os << " at Location: " << drv->location;
        Errors.push_back(std::string("Exception: ") + e.what() + os.str());
        return Errors;
    } catch (std::exception &e) {
        if ((ExpectedExceptions & StdException) != 0) {
            return {};
        }
        std::ostringstream os;
        os << " at Location: " << drv->location;
        Errors.push_back(std::string("Exception: ") + e.what() + os.str());
        return Errors;
    } catch (...) {
        if ((ExpectedExceptions & OtherException) != 0) {
            return {};
        }
        std::ostringstream os;
        os << " at Location: " << drv->location;
        Errors.push_back(std::string("Unknown Exception ") + os.str());
        return Errors;
    }
    if (!drv->GetErrors().empty()) {
        if ((ExpectedExceptions & ParsingException) != 0) {
            return {};
        }
        Errors.push_back(std::string("Syntax error in testcode"));
        return Errors;
    }
    try {
        drv->Run();
    } catch (ErrorBaseClass &e) {
        if ((ExpectedExceptions & RuntimeException) != 0) {
            return {};
        }
        std::ostringstream os;
        Errors.push_back(std::string("Runtime Exception: ") + e.what());
        return Errors;
    } catch (std::exception &e) {
        if ((ExpectedExceptions & StdException) != 0) {
            return {};
        }
        Errors.push_back(std::string("Runtime Exception: ") + e.what());
        return Errors;
    } catch (...) {
        if ((ExpectedExceptions & OtherException) != 0) {
            return {};
        }
        Errors.push_back(std::string("Unknown Exception at runtime "));
        return Errors;
    }
    if (ExpectedExceptions != NoException) {
        Errors.push_back(std::string("Expected exception not thrown..."));
        return Errors;
    }

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
                    os << "Expected content: " << v.second->GetValue() << ", got: " << V->GetValue();
                    os << "\nExpected detail:\n ";
                    v.second->GetValue().PrintDetail(os, 100);
                    os << "\ngotten detail:\n ";
                    V->GetValue().PrintDetail(os, 100);
                    Errors.push_back(std::string("Variable '") + v.first + "' has wrong content, " + os.str());
                }
            } else {
                std::ostringstream os;
                os << "Expected type: " << v.second->Type() << ", got: " << V->Type();
                Errors.push_back(std::string("Variable '") + v.first + "' has wrong type, " + os.str());
            }
        }
    }
    for (auto &v: Unexpected) {
        std::shared_ptr<VariableClass> V =
            drv->Variables.GetVariableReferenceForContext(v, 0);
        if (V != nullptr) {
            Errors.push_back(std::string("Variable '") + v + "' found in generated context, but should not exist here");
        }
    }

    return Errors;
}

