#ifndef EXEPTION_ERROR_CLASS_HPP
#define EXEPTION_ERROR_CLASS_HPP
/*****************************************************************************/
/*  Header     : BTError                                        Version 1.0  */
/*****************************************************************************/
/*                                                                           */
/*  Function   : Defines the errorclass used in Bonetracker-exceptionhandling*/
/*                                                                           */
/*                                                                           */
/*  Methodes   : BTError()                                                   */
/*              ~BTError()                                                   */
/*               GetString()                                                 */
/*               GetDebugInfo()                                              */
/*                                                                           */
/*  Author     : I. Oesch                                                    */
/*                                                                           */
/*  History    : 01.01.1997  IO Created                                      */
/*                                                                           */
/*  File       : errclass.hpp                                                */
/*                                                                           */
/*****************************************************************************/
/*    SWS Diplomarbeit 1997 Bonetracker from Ivo Oesch and Daniel Buergin    */
/*****************************************************************************/

/* imports */

/* Class constant declaration  */
#include <exception>
#define DEBUG_INFO_ENABLED

// Folowing macro is thought as aide for debugging, it adds
// filename and filenumber to errormessage

// usage:

// throw (BTError(_BTERROR::WHAT_REASON_EVER_IS_APROPRIATE));
// throw (BTError(_BTERROR::WHAT_REASON_EVER_IS_APROPRIATE, additional, arguments));

#define ERROR_OBJECT_HELPER(Message, FILE, LINE) ErrorBaseClass(Message "in" FILE "at line " #LINE)
#define ERROR_OBJECT(Message) ERROR_OBJECT_HELPER(Message, __FILE__,  __LINE__)
/* Class Type declaration      */

/* Class data declaration      */

/* Class definition            */
class ErrorBaseClass : public std::exception
{
   // Data
   public:
         const char *Message;

   private:
   
   // Methods
   public:
           ErrorBaseClass(const char *aWhat) : Message(aWhat) {}
           virtual ~ErrorBaseClass(void) override {}


           // exception interface
       public:
           virtual const char *what() const noexcept override {return Message;}
};

/*****************************************************************************/
/*  End Header  : BTError                                                    */
/*****************************************************************************/
#endif


