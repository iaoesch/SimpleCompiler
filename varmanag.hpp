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
class TypeDescriptorClass;
namespace Variables {
    class FunctionDefinitionClass;
    class VariableContentClass;
}


class VariableContextClass {
    std::map<std::string, std::shared_ptr<VariableClass>> Variables;
    VariableContextClass *ParentContext;
    std::vector<std::shared_ptr<VariableContextClass>> Children;
    const std::string Name;

public:

    VariableContextClass(const std::string &Name_, VariableContextClass *Parent_ = nullptr) : ParentContext(Parent_), Name(Name_) {}
    std::shared_ptr<VariableContextClass> CreateSubContext(const std::string &Name) {Children.push_back(std::make_shared<VariableContextClass>(Name, this)); return Children.back();}

    std::shared_ptr<VariableClass> RegisterVariable(const std::string Name, std::shared_ptr<VariableClass> Var, bool OverwriteAllowed = false);
    std::shared_ptr<VariableClass> LookupVariable(const std::string Name);

    void Dump(std::ostream &s);
    const std::string &GetName() {return Name;}

};

/* Class definition            */
class VariableManager
{
    std::vector<std::shared_ptr<VariableContextClass>> ContextStack;
    std::vector<std::shared_ptr<VariableContextClass>> Contexts;
    bool Local;
    std::shared_ptr<Variables::FunctionDefinitionClass> LocalsParent;
    uint32_t LocalOffset;
public:
    typedef std::vector<Variables::VariableContentClass> LocalStorageType;
private:
    std::vector<LocalStorageType> LocalStorageTemplates;


   // Data
   public:

    VariableManager() : Local(false), LocalOffset(0) {}

   void CreateNewContext(std::string Name);
   void LeaveContext(int Levels = 1);
   void StartLocal(std::shared_ptr<Variables::FunctionDefinitionClass> Parent);
   LocalStorageType EndLocal();
   std::shared_ptr<VariableClass> GetOrCreateVariable(std::string Name, const TypeDescriptorClass &Type, double Value);
   std::shared_ptr<VariableClass> CreateVariable(std::string Name, const TypeDescriptorClass &Type, double Value);
   std::shared_ptr<VariableClass> GetVariableReference(std::string Name);
   std::shared_ptr<VariableClass> GetVariableReferenceCreateIfNotFound(std::string Name, const TypeDescriptorClass &RequiredType);


   void Dump(std::ostream &s);
};


/*****************************************************************************/
/*  End Header  : BTError                                                    */
/*****************************************************************************/
#endif


