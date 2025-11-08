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
#include <map>
#include <string>
#include "variableclass.h"


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

};

/* Class definition            */
class VariableManager
{
    std::vector<std::shared_ptr<VariableContextClass>> ContextStack;
   
   // Data
   public:

   void CreateNewContext(std::string Name);
   void LeaveContext();
   std::shared_ptr<VariableClass> GetOrCreateVariable(std::string Name, const TypeDescriptorClass &Type, double Value);
   std::shared_ptr<VariableClass> CreateVariable(std::string Name, const TypeDescriptorClass &Type, double Value);
   std::shared_ptr<VariableClass> GetVariableReference(std::string Name);
};


/*****************************************************************************/
/*  End Header  : BTError                                                    */
/*****************************************************************************/
#endif


