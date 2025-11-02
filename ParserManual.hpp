#ifndef PARSER_CLASS_HPP
#define PARSER_CLASS_HPP
/*****************************************************************************/
/*  Header     : ParserClass                                    Version 1.0  */
/*****************************************************************************/
/*                                                                           */
/*  Function   : This class parses a (Script) file and converts its contents */
/*               into a stream of tokens, and defines also some functions    */
/*               to make the parsing easier                                  */
/*               To fetch the next token from the inputfile NextSymbol()     */
/*               must be called, and if newlines should be ignored,          */
/*               NextSymbolSkipNewline() must be called                      */
/*               ConsumeToken() throws an exception if the current token     */
/*               is not the expected one, and then fetches the next token    */
/*                                                                           */
/*  Methodes   : ParserClass()                                               */
/*              ~ParserClass()                                               */
/*               NextSymbol()                                                */
/*               NextSymbolSkipNewline()                                     */
/*               SkipNewline()                                               */
/*               ConsumeAllExcept()                                          */
/*               ConsumeToken()                                              */
/*               ConsumeTokenAndNewline()                                    */
/*               ConsumeIdentifiert()                                        */
/*               GetLastToken()                                              */
/*               GetLastValue()                                              */
/*               GetLastCharacter()                                          */
/*               GetLastString()                                             */
/*               GetLLineNumber()                                            */
/*                                                                           */
/*  Author     : I. Oesch                                                    */
/*                                                                           */
/*  History    : 18.01.1997  IO Created                                      */
/*                                                                           */
/*  File       : Parser.hpp                                                  */
/*                                                                           */
/*****************************************************************************/
/*    SWS Diplomarbeit 1997 Bonetracker from Ivo Oesch and Daniel Buergin    */
/*****************************************************************************/

/* imports */
#include <iostream>
#include "errclass.hpp"

/* Class constant declaration  */

/* Class Type declaration      */

/* Class data declaration      */

/* Class definition            */
class ParserClass
{
   // Data
   public:
   typedef enum TokenEnum
   {
      T_EOF = 0,
      T_OPENBRACKET,
      T_CLOSEBRACKET,
      T_OPENCURLY,
      T_CLOSECURLY,
      T_OPENPARANTHESIS,
      T_CLOSEPARANTHESIS,
      T_SEPARATOR,
      T_ASSIGN,
      T_COLON,
      T_DOUBLECOLON,
      T_QUESTION,
      T_COMMAND,
      T_NEWLINE,
      T_ADD,
      T_SUB,
      T_MUL,
      T_DIV,
      T_NEG,
      T_INV,
      T_POW,
      T_ABS,
      T_SIN,
      T_COS,
      T_TAN,
      T_ASIN,
      T_ACOS,
      T_ATAN,
      T_ATAN2,
      T_EXP,
      T_LN,
      T_SQUARE,
      T_SQRT,
      T_IDENTIFIER,
      T_VALUE,
      T_STRING,
      T_CHARACTER,
      T_RESERVED,
      T_COMMENT,
   } TokenEnum;

   private:
#define MAX_BUFFER 255
   char   Buffer[MAX_BUFFER];
   double Value;
   char   Character;
   int    CurrentLine;
   TokenEnum Token;

   std::istream &Input;

   // Methods
   public:
           ParserClass(std::istream &InputStream);// : Input(InputStream);
          ~ParserClass(void) {};

           void  NextSymbol(void);
           void  NextSymbolSkipNewline(void);
           void  SkipNewline(void);

           void  ConsumeAllExcept(TokenEnum T);

           void  ConsumeToken(TokenEnum T);
           void  ConsumeTokenAndNewline(TokenEnum T);

           void  ConsumeIdentifier(const char *Name);

           TokenEnum  GetLastToken(void) {return Token; };
           double  GetLastValue(void) {return Value; };
           char  GetLastCharacter(void) {return Character; };
           int   GetLineNumber(void) {return CurrentLine; };
           const char *GetLastString(void) {return Buffer; };

   private:
           void AnalyzeKeyWord(const char *String);

};


/*****************************************************************************/
/*  End Header  : ParserClass                                                */
/*****************************************************************************/
#endif


