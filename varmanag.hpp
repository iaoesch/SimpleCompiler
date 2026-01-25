#ifndef VARIABLE_MANAGER_CLASS_HPP
#define VARIABLE_MANAGER_CLASS_HPP
/*****************************************************************************/
/*  Header     : VariableManager                                Version 1.0  */
/*****************************************************************************/
/*                                                                           */
/*  Function   : Manages the Variables in our System                         */
/*                                                                           */
/*                                                                           */
/*  Methodes   : VariableManager()                                           */
/*              ~VariableManager()                                           */
/*               GetVariable()                                               */
/*                                                                           */
/*  Author     : I. Oesch                                                    */
/*                                                                           */
/*  History    : 26.02.2002  IO Created                                      */
/*                                                                           */
/*  File       : varmanag.hpp                                                */
/*                                                                           */
/*****************************************************************************/
/*    SWS Diplomarbeit 1997 Bonetracker from Ivo Oesch and Daniel Buergin    */
/*****************************************************************************/

/* imports */

/* Class constant declaration  */

/* Class Type declaration      */

/* Class data declaration      */
#include "variableclass.h"
#include <map>
#include <string>
//#include "variableclass.h"
class VariableClass;
class VariableTypeDescriptorClass;
namespace Variables {
    class FunctionDefinitionClass;
    class VariableContentClass;
}


class VariableContextClass {
    std::map<std::string, std::shared_ptr<VariableClass>> Variables;
    VariableContextClass *ParentContext;
    std::vector<std::shared_ptr<VariableContextClass>> Children;
    const std::string Name;
    bool ParentIsHidden;

public:

    VariableContextClass(const std::string &Name_, VariableContextClass *Parent_ = nullptr, bool HideParent = false) : ParentContext(Parent_), Name(Name_), ParentIsHidden(HideParent) {}
//    friend std::shared_ptr<VariableContextClass> std::make_shared<VariableContextClass, const std::string&,VariableContextClass*, bool>(const std::string &, VariableContextClass* &&, bool &&);
//    friend std::shared_ptr<VariableContextClass> std::make_shared<VariableContextClass, std::string, VariableContextClass*, bool>(std::string, VariableContextClass*, bool);
public:
    std::shared_ptr<VariableContextClass> CreateSubContext(const std::string &Name, bool HideParent = false) {Children.push_back(std::make_shared<VariableContextClass>(Name, this, HideParent)); return Children.back();}

    std::shared_ptr<VariableClass> RegisterVariable(const std::string Name, std::shared_ptr<VariableClass> Var, bool OverwriteAllowed = false);
    std::shared_ptr<VariableClass> LookupVariable(const std::string Name);

    // For Testpurposes
    std::shared_ptr<VariableClass> LookupVariableInThisContextOnly(const std::string Name);
    size_t GetNumberOfVariables() const {return Variables.size();}

    void Dump(std::ostream &s);
    const std::string &GetName() {return Name;}

};

/* Class definition            */
class VariableManager
{
    std::vector<std::shared_ptr<VariableContextClass>> ContextStack;
    std::vector<std::shared_ptr<VariableContextClass>> Contexts;
    VariableClass::StorageClass DefaultStorage;
    bool Local;
//    std::shared_ptr<Variables::FunctionDefinitionBaseClass> LocalsParent;
    uint32_t LocalOffset;
    uint32_t LocalClassOffset;
public:
    enum ParentVisibility {ParentVisible, HideParent};
    typedef std::vector<Variables::VariableContentClass> LocalStorageType;
    typedef std::vector<std::shared_ptr<LateBindingVariableClass>> ObjectMemberVariableType;
private:
    struct LocalFunctionStorageContextType {
    //    LocalFunctionStorageContextType(LocalFunctionStorageContextType &&) = default;
    //    LocalFunctionStorageContextType(LocalFunctionStorageContextType const &) = default;
        LocalStorageType &LocalStorageTemplates;
        std::shared_ptr<Variables::FunctionDefinitionBaseClass> LocalsParent;

    };
    struct LocalClassStorageContextType {
   //     LocalClassStorageContextType(LocalClassStorageContextType &&) = default;
   //     LocalClassStorageContextType(LocalClassStorageContextType const &) = default;
        LocalStorageType &LocalClassAttributeStorageTemplates;
        LocalStorageType &LocalAttrubiteStorageTemplates;
        ObjectMemberVariableType &LocalVariableTemplates;
        std::shared_ptr<Variables::ClassClass> LocalsParent;
    };

    // Some shortcuts for easier use
    typedef LocalFunctionStorageContextType FktTemplate;
    typedef LocalClassStorageContextType ClassTemplate;
    typedef std::variant<FktTemplate, ClassTemplate> LocalStorageContextType;
    class LocalStorageInfoClass {
    public:
        LocalStorageContextType LocalStorageTemplates;
        VariableClass::StorageClass DefaultStorage;
    };
    std::vector<LocalStorageInfoClass> LocalStorageInfoStack;

    static Environment *DefaultEnvironment;

   // Data
    std::shared_ptr<VariableClass> CreateSymbol(std::string Name, const VariableTypeDescriptorClass &Type, VariableClass::StorageClass Storage);
public:
    static void SetDefaultEnvironment(Environment &Env) {DefaultEnvironment = &Env;}

    VariableManager() : DefaultStorage(VariableClass::StorageClass::Global|VariableClass::StorageClass::RW), Local(false), LocalOffset(0) {}

    void clear() {Local = false; LocalOffset = 0; ContextStack.clear(); Contexts.clear();}

   void CreateNewContext(std::string Name, ParentVisibility ParentVisibilityMode = ParentVisible);
   void LeaveContext(int Levels = 1);
   void StartLocal(std::shared_ptr<Variables::FunctionDefinitionBaseClass> Parent);
   void EndLocal();
   void StartClass(std::shared_ptr<Variables::ClassClass> Parent);
   void EndClass();
 //  std::shared_ptr<VariableClass> GetOrCreateVariable(std::string Name, const VariableTypeDescriptorClass &Type, double Value);
   std::shared_ptr<VariableClass> CreateVariable(std::string Name, const VariableTypeDescriptorClass &Type, double Value);
   std::shared_ptr<VariableClass> CreateConstant(std::string Name, const VariableTypeDescriptorClass &Type, double Value);
   std::shared_ptr<VariableClass> CreateFunction(std::string Name, const VariableTypeDescriptorClass &Type, double Value);
   std::shared_ptr<VariableClass> CreateClass(std::string Name, const VariableTypeDescriptorClass &Type, double Value);
   std::shared_ptr<VariableClass> GetVariableReference(std::string Name);
   std::shared_ptr<VariableClass> GetVariableReferenceCreateIfNotFound(std::string Name, const VariableTypeDescriptorClass &RequiredType);
 //  std::shared_ptr<VariableClass> CreateVariableAndGetReference(std::string Name, const VariableTypeDescriptorClass &RequiredType);

   // For Testpurposes
   size_t GetNumberOfContexts() const {return Contexts.size();}
   std::shared_ptr<VariableClass> GetVariableReferenceForContext(std::string Name, size_t Index);
   size_t GetNumberOfVariablesForContext(size_t Index) const {return Contexts[Index]->GetNumberOfVariables();}

   void Dump(std::ostream &s);
   std::shared_ptr<VariableClass> CreateMember(std::string Name, const VariableTypeDescriptorClass &Type, double Value);
};


/*****************************************************************************/
/*  End Header  : BTError                                                    */
/*****************************************************************************/
#endif


