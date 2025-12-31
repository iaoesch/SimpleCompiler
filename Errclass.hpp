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
#include <string>
#define DEBUG_INFO_ENABLED

// Folowing macro is thought as aide for debugging, it adds
// filename and filenumber to errormessage

// usage:

// throw (BTError(_BTERROR::WHAT_REASON_EVER_IS_APROPRIATE));
// throw (BTError(_BTERROR::WHAT_REASON_EVER_IS_APROPRIATE, additional, arguments));

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
//#define AT __FILE__ ":" TOSTRING(__LINE__)

#define FATAL_ERROR_OBJECT(Message) InternalErrorClass(Message " in " __FILE__ " at line " TOSTRING(__LINE__))
#define INTERNAL_ERROR_OBJECT(Message) InternalErrorClass((Message) + std::string(" in ") + __FILE__ + "in Function '" + __func__ + "' at line " + std::to_string(__LINE__))
#define SIGNAL_UNIMPLEMENTED() throw (INTERNAL_ERROR_OBJECT("Unimplemented Funcion"))

/* Class Type declaration      */

/* Class data declaration      */

/* Class definition            */
class ErrorBaseClass : public std::exception
{
   // Data
   public:

   private:
   
   // Methods
   public:
           ErrorBaseClass() {}
           virtual ~ErrorBaseClass(void) override {}


           // exception interface
       public:
           virtual const char *what() const noexcept override {return "BaseException";}
};

class FatalErrorClass : public ErrorBaseClass
{
    // Data
public:
    // No dynamic memory, just plain old const c-str
    const char *Message;

private:

    // Methods
public:
    FatalErrorClass(const char *aWhat) : Message(aWhat) {}
    virtual ~FatalErrorClass(void) override {}


    // exception interface
public:
    virtual const char *what() const noexcept override {return Message;}
};

class InternalErrorClass : public ErrorBaseClass
{
    // Data
public:
    const std::string Message;

private:

    // Methods
public:
    InternalErrorClass(const std::string &aWhat) : Message(aWhat) {}
    virtual ~InternalErrorClass(void) override {}


    // exception interface
public:
    virtual const char *what() const noexcept override {return Message.c_str();}
};


class SyntaxErrorClass : public ErrorBaseClass
{
    // Data
public:
    std::string Message;

private:

    // Methods
public:
    SyntaxErrorClass(std::string const &aWhat) : Message(aWhat) {}
    virtual ~SyntaxErrorClass(void) override {}


    // exception interface
public:
    virtual const char *what() const noexcept override {return Message.c_str();}
};

class RuntimeErrorClass : public ErrorBaseClass
{
    // Data
public:
    std::string Message;

private:

    // Methods
public:
    RuntimeErrorClass(std::string const &aWhat) : Message(aWhat) {}
    virtual ~RuntimeErrorClass(void) override {}

    void ExtendMessage(const std::string &NewLine) {Message.append("\n" + NewLine);}
    // exception interface
public:
    virtual const char *what() const noexcept override {return Message.c_str();}
};


class PropagatedErrorClass : public ErrorBaseClass
{
    // Data
public:
    std::string Message;

private:
    std::unique_ptr<ErrorBaseClass> ParentError;
    // Methods
public:
    PropagatedErrorClass(ErrorBaseClass &ParentError, std::string const &aWhat) : Message(aWhat) {}
    virtual ~PropagatedErrorClass(void) override {}


    // exception interface
public:
    virtual const char *what() const noexcept override {return Message.c_str();}
};

/*****************************************************************************/
/*  End Header  : BTError                                                    */
/*****************************************************************************/
#endif


