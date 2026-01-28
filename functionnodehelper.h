#ifndef FUNCTIONNODEHELPER_H
#define FUNCTIONNODEHELPER_H

#include <list>
#include <string>
#include <vector>
#include"varmanag.hpp"


namespace yy {
class location;
}
typedef yy::location  LocationType;


class VariableManager;
class StatementClass;
class ReferementClass;
class AssignementClass;
class MethodCallClass;
class FunctionCallClass;

namespace Variables {
class FunctionDefinitionClass;
class FunctionDefinitionBaseClass;
}
class VariableClass;
class VariableTypeDescriptorClass;



class FunctionNodeHelper {
    VariableManager &Variables;
    std::map<std::string, std::list<std::shared_ptr<StatementClass>>> KnownFunctions;
    
    struct FunctionDefinitionInfoType__ {
        std::shared_ptr<Variables::FunctionDefinitionClass> CurrentFunction;
        std::string Name;
        std::shared_ptr<VariableClass> VariableHoldingCurrentFunction;
        std::unique_ptr<VariableTypeDescriptorClass> ReturnType;
    };

    struct MethodDefinitionInfoType {
        std::shared_ptr<Variables::MethodDefinitionClass> CurrentMethod;
        std::string Name;
        std::shared_ptr<VariableClass> VariableHoldingCurrentFunction;
        std::unique_ptr<VariableTypeDescriptorClass> ReturnType;
        //     int NextPositionalParameter;
        std::shared_ptr<VariableContextProxyClass> ProxyContext;
        std::shared_ptr<Variables::ClassClass> ClassForMethod;
    };
    typedef std::variant<FunctionDefinitionInfoType__, MethodDefinitionInfoType> FunctionOrMethodDefinitionInfoType;

    std::vector<FunctionOrMethodDefinitionInfoType> FunctionsDefinitonsPending;
    
    uint32_t AnonymeousElementCounter;
    
    struct FunctionCallInfoType {
        std::shared_ptr<Variables::FunctionDefinitionBaseClass> CurrentFunction;
        int NextPositionalParameter;
    };
    struct MethodCallInfoType {
        std::shared_ptr<Variables::MethodDefinitionClass> CurrentMethod;
        int NextPositionalParameter;
        std::shared_ptr<const Variables::ClassClass> UsedClass;
        std::shared_ptr<VariableClass> UsedObject;
        std::list<std::shared_ptr<StatementClass>> Assignements;
        std::shared_ptr<AssignementClass> ThisAssignement;
    };
    typedef std::variant<FunctionCallInfoType, MethodCallInfoType> CallInfoType;
    std::vector<CallInfoType> FunctionCallsPending;
    
    
public:
    explicit FunctionNodeHelper(VariableManager &Variables)
        : Variables(Variables) ,/* CurrentFunction(nullptr), NextPositionalParameter(-1),*/ AnonymeousElementCounter(0) {}
    std::shared_ptr<Variables::FunctionDefinitionClass> BeginFunctionDefinition(std::string Name, const LocationType &l);
    std::shared_ptr<Variables::FunctionDefinitionClass> BeginFunctionDefinition(const LocationType &l);
    void StartParameterDefinition() {
        if (FunctionsDefinitonsPending.empty()) {
            throw(INTERNAL_ERROR_OBJECT("<GetReference()> Not inside function"));
        }
        Variables.StartLocal(std::get<FunctionDefinitionInfoType__>(FunctionsDefinitonsPending.back()).CurrentFunction);
        Variables.CreateNewContext(std::get<FunctionDefinitionInfoType__>(FunctionsDefinitonsPending.back()).Name + "Params");
    }

    void EndParameterDefinition() {}

    void StartCodeDefinition() {
        if (FunctionsDefinitonsPending.empty()) {
            throw(INTERNAL_ERROR_OBJECT("<GetReference()> Not inside function"));
        }
        if (std::holds_alternative<FunctionDefinitionInfoType__>(FunctionsDefinitonsPending.back())) {
            Variables.CreateNewContext(std::get<FunctionDefinitionInfoType__>(FunctionsDefinitonsPending.back()).Name);
        } else if (std::holds_alternative<MethodDefinitionInfoType>(FunctionsDefinitonsPending.back())) {
            Variables.CreateNewContext(std::get<MethodDefinitionInfoType>(FunctionsDefinitonsPending.back()).Name);
        } else {
            throw(INTERNAL_ERROR_OBJECT("<Set()> No valid Infotype"));
        }
    }

    void EndCodeDefinition() {
        Variables.EndLocal();
    }
    // std::shared_ptr<Variables::FunctionDefinitionClass> Define(Variables::FunctionDefinitionClass &&f, const LocationType &l);
    // std::shared_ptr<Variables::FunctionDefinitionClass> Define(const std::vector<std::shared_ptr<VariableClass> > &Parameters, const std::list<std::shared_ptr<StatementClass> > &Statements, Variables::FunctionDefinitionClass::LocalStorageType StorageTemplate, LocationType const &Loc);
    void Set(const std::vector<std::shared_ptr<VariableClass> > &Parameters, LocationType const &Loc);
    void Set(const std::list<std::shared_ptr<StatementClass> > &Statements, LocationType const &Loc);
    void Set(Variables::FunctionDefinitionClass::LocalStorageType StorageTemplate, LocationType const &Loc);
    std::string GetName();
    std::shared_ptr<Variables::FunctionDefinitionClass> GetReference();
    std::shared_ptr<Variables::FunctionDefinitionClass> Get(LocationType &l);
    void SetReturnType(std::unique_ptr<VariableTypeDescriptorClass> NewReturnType);
    void EndFunctionDefinition(const LocationType &l);
    
    void BeginMethodDefinition(std::string Name, const LocationType &l);
    void StartMethodParameterDefinition() {
        if (FunctionsDefinitonsPending.empty()) {
           throw(INTERNAL_ERROR_OBJECT("<StartMethodParameterDefinition()> Not inside function"));
        }
        std::get<MethodDefinitionInfoType>(FunctionsDefinitonsPending.back()).ProxyContext = Variables.CreateNewProxyContext(std::get<MethodDefinitionInfoType>(FunctionsDefinitonsPending.back()).Name + "AttributesProxy");
        Variables.StartLocal(std::get<MethodDefinitionInfoType>(FunctionsDefinitonsPending.back()).CurrentMethod);
        Variables.CreateNewContext(std::get<MethodDefinitionInfoType>(FunctionsDefinitonsPending.back()).Name + "Params");
    }
    void SetClassContext(std::string Classname) {
        if (FunctionsDefinitonsPending.empty()) {
            throw(INTERNAL_ERROR_OBJECT("<SetClassContext()> Not inside function"));
        }
        std::shared_ptr<VariableClass> Var = Variables.GetVariableReference(Classname);
        if (Var == nullptr) {
            throw SyntaxErrorClass("Class '" + Classname + "' not found");
        }
        if (Var->Type() != TypeDescriptorClass::Type::Class) {
            throw SyntaxErrorClass("'" + Classname + "' is not an class, cannot define a messages for it");
        }
        if (!Var->GetValue().holds_alternative<std::shared_ptr<Variables::ClassClass>>()) {
            throw SyntaxErrorClass("'" + Classname + "' contains not an class, cannot define a messages for it");
        }
        std::get<MethodDefinitionInfoType>(FunctionsDefinitonsPending.back()).ClassForMethod = Var->GetValue().GetValue<std::shared_ptr<Variables::ClassClass>>();

        auto Context = std::get<MethodDefinitionInfoType>(FunctionsDefinitonsPending.back()).ClassForMethod->getContextForParsing();
        std::get<MethodDefinitionInfoType>(FunctionsDefinitonsPending.back()).ProxyContext->SetReferedContext(Context);
    }
    void EndMethodDefinition(const LocationType &l) {
        (void) l;
        if (FunctionsDefinitonsPending.empty()) {
            throw(INTERNAL_ERROR_OBJECT("<EndMethodDefinition()> Not inside function"));
        }
        MethodDefinitionInfoType &Info = std::get<MethodDefinitionInfoType>(FunctionsDefinitonsPending.back());

        Info.ClassForMethod->AddMethod(Info.Name, Info.CurrentMethod);
        FunctionsDefinitonsPending.pop_back();
        Variables.LeaveContext(3);
    }
    std::shared_ptr<Variables::FunctionDefinitionBaseClass> BeginFunctionCall(std::string Name, const LocationType &l);
    std::shared_ptr<ReferementClass> MakeRef(const std::string Referer, std::shared_ptr<ExpressionClass> Refered, const LocationType &Loc);
    std::shared_ptr<AssignementClass> MakeAssign(const std::string Assignee, std::shared_ptr<ExpressionClass>  Assigned, LocationType const &Loc);
    std::shared_ptr<ReferementClass> MakeRefBySequence(std::shared_ptr<ExpressionClass> Refered, const LocationType &Loc);
    std::shared_ptr<AssignementClass> MakeAssignBySequence(std::shared_ptr<ExpressionClass>  Assigned, LocationType const &Loc);
    void EndFunctionCall(const LocationType &l);
    void EndMethodCall(const LocationType &l);
    std::map<std::string, std::list<std::shared_ptr<StatementClass>>> const &GetListOfDefinedFunctions() const {return KnownFunctions;}
    void BeginMethodCallForObject(std::string ObjectName, const LocationType &Loc);
    void SetCalledMethodForObject(std::string ObjectName, const LocationType &Loc);
    void SetParameterAssignListForCalledMethod(std::list<std::shared_ptr<StatementClass> > &&Assignements, const LocationType &Loc);
    std::shared_ptr<FunctionCallClass> FinishMethodCall(const LocationType &Loc);
private:
    std::string GetQualifiedName();
};

#endif // FUNCTIONNODEHELPER_H
