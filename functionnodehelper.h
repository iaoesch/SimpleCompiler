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

namespace Variables {
class FunctionDefinitionClass;
class FunctionDefinitionBaseClass;
}
class VariableClass;
class VariableTypeDescriptorClass;

class FunctionNodeHelper {
    VariableManager &Variables;
    std::map<std::string, std::list<std::shared_ptr<StatementClass>>> KnownFunctions;
    
    struct FunctionDefinitionInfoType {
        std::shared_ptr<Variables::FunctionDefinitionClass> CurrentFunction;
        std::string Name;
        //std::shared_ptr<VariableClass> ReturnVariable;
        std::shared_ptr<VariableClass> VariableHoldingCurrentFunction;
        //Variables::VariableContentClass ReturnedValue;
        std::unique_ptr<VariableTypeDescriptorClass> ReturnType;
        //     int NextPositionalParameter;
    };
    std::vector<FunctionDefinitionInfoType> FunctionsDefinitonsPending;
    
    uint32_t AnonymeousElementCounter;
    
    struct FunctionCallInfoType {
        std::shared_ptr<Variables::FunctionDefinitionBaseClass> CurrentFunction;
        int NextPositionalParameter;
    };
    std::vector<FunctionCallInfoType> FunctionCallsPending;
    
    
public:
    explicit FunctionNodeHelper(VariableManager &Variables)
        : Variables(Variables) ,/* CurrentFunction(nullptr), NextPositionalParameter(-1),*/ AnonymeousElementCounter(0) {}
    std::shared_ptr<Variables::FunctionDefinitionClass> BeginFunctionDefinition(std::string Name, const LocationType &l);
    std::shared_ptr<Variables::FunctionDefinitionClass> BeginFunctionDefinition(const LocationType &l);
    void StartParameterDefinition() {
        if (FunctionsDefinitonsPending.empty()) {
            throw(INTERNAL_ERROR_OBJECT("<GetReference()> Not inside function"));
        }
        Variables.StartLocal(FunctionsDefinitonsPending.back().CurrentFunction);
        Variables.CreateNewContext(FunctionsDefinitonsPending.back().Name + "Params");
    }

    void EndParameterDefinition() {}

    void StartCodeDefinition() {
        if (FunctionsDefinitonsPending.empty()) {
            throw(INTERNAL_ERROR_OBJECT("<GetReference()> Not inside function"));
        }
        Variables.CreateNewContext(FunctionsDefinitonsPending.back().Name);
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
    std::shared_ptr<Variables::FunctionDefinitionClass> SetReturnType(std::unique_ptr<VariableTypeDescriptorClass> NewReturnType);
    void EndFunctionDefinition(const LocationType &l);
    
    std::shared_ptr<Variables::FunctionDefinitionBaseClass> BeginFunctionCall(std::string Name, const LocationType &l);
    std::shared_ptr<ReferementClass> MakeRef(const std::string Referer, std::shared_ptr<ExpressionClass> Refered, const LocationType &Loc);
    std::shared_ptr<AssignementClass> MakeAssign(const std::string Assignee, std::shared_ptr<ExpressionClass>  Assigned, LocationType const &Loc);
    std::shared_ptr<ReferementClass> MakeRefBySequence(std::shared_ptr<ExpressionClass> Refered, const LocationType &Loc);
    std::shared_ptr<AssignementClass> MakeAssignBySequence(std::shared_ptr<ExpressionClass>  Assigned, LocationType const &Loc);
    void EndFunctionCall(const LocationType &l);
    std::map<std::string, std::list<std::shared_ptr<StatementClass>>> const &GetListOfDefinedFunctions() const {return KnownFunctions;}
private:
    std::string GetQualifiedName();
};

#endif // FUNCTIONNODEHELPER_H
