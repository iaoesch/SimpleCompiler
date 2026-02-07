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
        std::vector<std::shared_ptr<VariableClass> > Parameters;
        //LocationType ParameterLocations;

    };

    struct MethodDefinitionInfoType {
        std::shared_ptr<Variables::MethodDefinitionClass> CurrentMethod;
        std::string Name;
        std::shared_ptr<VariableClass> VariableHoldingCurrentFunction;
        std::unique_ptr<VariableTypeDescriptorClass> ReturnType;
        //     int NextPositionalParameter;
        std::shared_ptr<VariableContextProxyForClassmemberClass> ProxyContext;
        std::shared_ptr<Variables::ClassClass> ClassForMethod;
        std::vector<std::shared_ptr<VariableClass> > Parameters;
        std::shared_ptr<VariableClass> ThisParameter;
        //LocationType ParameterLocations;
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
    explicit FunctionNodeHelper(VariableManager &Variables);
    std::shared_ptr<Variables::FunctionDefinitionClass> BeginFunctionDefinition(std::string Name, const LocationType &l);
    std::shared_ptr<Variables::FunctionDefinitionClass> BeginFunctionDefinition(const LocationType &l);
    void StartParameterDefinition();

    void EndParameterDefinition();

    void StartCodeDefinition();

    void EndCodeDefinition() {
        Variables.EndLocal();
    }
    // std::shared_ptr<Variables::FunctionDefinitionClass> Define(Variables::FunctionDefinitionClass &&f, const LocationType &l);
    // std::shared_ptr<Variables::FunctionDefinitionClass> Define(const std::vector<std::shared_ptr<VariableClass> > &Parameters, const std::list<std::shared_ptr<StatementClass> > &Statements, Variables::FunctionDefinitionClass::LocalStorageType StorageTemplate, LocationType const &Loc);
  //  void Set(const std::vector<std::shared_ptr<VariableClass> > &Parameters, LocationType const &Loc);
    void Set(const std::list<std::shared_ptr<StatementClass> > &Statements, LocationType const &Loc);
    void Set(Variables::FunctionDefinitionClass::LocalStorageType StorageTemplate, LocationType const &Loc);
    std::string GetName();
    std::shared_ptr<Variables::FunctionDefinitionClass> GetReference();
    std::shared_ptr<Variables::FunctionDefinitionClass> Get(LocationType &l);
    void SetReturnType(std::unique_ptr<VariableTypeDescriptorClass> NewReturnType);
    void EndFunctionDefinition(const LocationType &l);
    
    void BeginMethodDefinition(std::string Name, const LocationType &l);
    void StartMethodParameterDefinition();
    void SetClassContext(std::string Classname);
    void EndMethodDefinition(const LocationType &l);
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
    void BeginConstructorMethodCallForObject(std::shared_ptr<Variables::ClassClass> UsedClass, std::string MethodName, const LocationType &Loc);
    void SetParameterAssignListForCalledMethod(std::list<std::shared_ptr<StatementClass> > &&Assignements, const LocationType &Loc);
    std::shared_ptr<FunctionCallClass> FinishMethodCall(const LocationType &Loc);
    std::shared_ptr<FunctionCallClass> FinishConstructorMethodCall(const LocationType &Loc);
    std::shared_ptr<VariableClass> AddParameter(std::string Name, const VariableTypeDescriptorClass &Type, const LocationType &Loc);
private:
    std::string GetQualifiedName();
};

#endif // FUNCTIONNODEHELPER_H
