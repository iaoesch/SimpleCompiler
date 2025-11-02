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

class VariableClass;

/* Class definition            */
class VariableManager
{
   std::map<std::string, std::shared_ptr<VariableClass>> Variables;
   
   // Data
   public:

   VariableClass *GetVariableReference(std::string Name);
};


/*****************************************************************************/
/*  End Header  : BTError                                                    */
/*****************************************************************************/
#endif


