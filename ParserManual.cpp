/*****************************************************************************/
/*  Class      : ParserClass                                    Version 1.0  */
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
/*  History    : 01.01.1997  IO Created                                      */
/*                                                                           */
/*  File       : Parser.cpp                                                  */
/*                                                                           */
/*****************************************************************************/
/*    SWS Diplomarbeit 1997 Bonetracker from Ivo Oesch and Daniel Buergin    */
/*****************************************************************************/

/* imports */
#include "ParserManual.hpp"
#include <ctype.h>
#include <string.h>

/* Class constant declaration  */

// needed since Borland-C sttically bound library seems not to
// initialize is.... Table correct

#if 1
#define isprint(x) (((x) <  127)&&((x) >= ' '))
#define isdigit(x) (((x) <= '9')&&((x) >= '0'))
#define isspace(x) (((x) == ' ')||((x) == '\t')||((x) == '\n')||((x) == '\r'))
#define isalpha(x) ((((x) <= 'z')&&((x) >= 'a'))||(((x) <= 'Z')&&((x) >= 'A'))||((x) == '_'))
#define isalnum(x) (isdigit(x)||isalpha(x))
#endif

/* Class Type declaration      */

/* Class data declaration      */

/* Class procedure declaration */

/*****************************************************************************/
/*  Method      : ParserClass                                                */
/*****************************************************************************/
/*                                                                           */
/*  Function    : Constructor                                                */
/*                                                                           */
/*  Type        : Constructor                                                */
/*                                                                           */
/*  Input Para  : InputStream: Stream which is to be scanned                 */
/*                                                                           */
/*  Output Para : None                                                       */
/*                                                                           */
/*  Author      : I. Oesch                                                   */
/*                                                                           */
/*  History     : 01.01.1997  IO  Created                                    */
/*                                                                           */
/*****************************************************************************/
ParserClass::ParserClass(std::istream &InputStream) : Input(InputStream)
{
   Value = 0.0;
   Character = 0;
   Token = T_EOF;
   Buffer[0] = 0;
   CurrentLine = 1;
}
/*****************************************************************************/
/*  End Method  : ParserClass                                                */
/*****************************************************************************/

/*****************************************************************************/
/*  Method      : ConsumeToken                                               */
/*****************************************************************************/
/*                                                                           */
/*  Function    : Check if the current token is as requested. get nextsymbol */
/*                if ok, otherwise throw error                               */
/*                                                                           */
/*  Type        : Public                                                     */
/*                                                                           */
/*  Input Para  : T: Token to consume (Token which must be current)          */
/*                                                                           */
/*  Output Para : None                                                       */
/*                                                                           */
/*  Author      : I. Oesch                                                   */
/*                                                                           */
/*  History     : 15.02.1997  IO  Created                                    */
/*                                                                           */
/*****************************************************************************/

void ParserClass::ConsumeToken(ParserClass::TokenEnum T)
{
   if (Token != T) {
      throw (BTError(_BTERROR::BT_UNEXPECTED_TOKEN));
   }
   NextSymbol();
}

void ParserClass::ConsumeTokenAndNewline(ParserClass::TokenEnum T)
{
   while (Token == T_NEWLINE) {
      NextSymbol();
   }
   if (Token != T) {
      throw (BTError(_BTERROR::BT_UNEXPECTED_TOKEN));
   }
   NextSymbol();
}

void ParserClass::ConsumeAllExcept(ParserClass::TokenEnum T)
{
   while (Token != T) {
      if (Token == T_EOF) {
         throw (BTError(_BTERROR::BT_UNEXPECTED_END_OF_FILE));
      }
      NextSymbol();
   }
}

void ParserClass::ConsumeIdentifier(const char *Name)
{
   if (  (Token != T_IDENTIFIER)
       ||(strcmp (Buffer, Name) != 0)) {
      throw (BTError(_BTERROR::BT_UNEXPECTED_TOKEN));
   }
   NextSymbol();
}


/*****************************************************************************/
/*  End Method  : ConsumeToken                                               */
/*****************************************************************************/


/*****************************************************************************/
/*  Method      : NextSymolSkipNewline                                       */
/*****************************************************************************/
/*                                                                           */
/*  Function    : Gets the next Symbol from the inputstream, skips newlines  */
/*                                                                           */
/*  Type        : Public                                                     */
/*                                                                           */
/*  Input Para  : None                                                       */
/*                                                                           */
/*  Output Para : None                                                       */
/*                                                                           */
/*  Author      : I. Oesch                                                   */
/*                                                                           */
/*  History     : 15.02.1997  IO  Created                                    */
/*                                                                           */
/*****************************************************************************/

void ParserClass::NextSymbolSkipNewline(void)
{
   NextSymbol();
   while (Token == T_NEWLINE) {
      NextSymbol();
   }
}

void ParserClass::SkipNewline(void)
{
   while (Token == T_NEWLINE) {
      NextSymbol();
   }
}

/*****************************************************************************/
/*  End Method  : NextSymolSkipNewline                                       */
/*****************************************************************************/



/*****************************************************************************/
/*  Method      : NextSymol                                                  */
/*****************************************************************************/
/*                                                                           */
/*  Function    : Gets the next Symbol from the inputstream                  */
/*                                                                           */
/*  Type        : Public                                                     */
/*                                                                           */
/*  Input Para  : None                                                       */
/*                                                                           */
/*  Output Para : None                                                       */
/*                                                                           */
/*  Author      : I. Oesch                                                   */
/*                                                                           */
/*  History     : 01.01.1997  IO  Created                                    */
/*                                                                           */
/*****************************************************************************/

void ParserClass::NextSymbol(void)
{

/* Class data declaration      */
   char c;
   char *Ptr;

/* Class code declaration      */
   /* As long as file is not finished */
   if (!Input.eof())
   {
      if (!Input)//!Input.good())
      {
         throw (BTError(_BTERROR::BT_IO_ERROR, 0, CurrentLine));
      }

      /* entrypoint for comment-filter */
      restart:

      Input.get(c);

      /* overread whitespaces */
      while (isspace(c)&&(c != '\n'))
      {
         Input.get(c);
      }

      if (Input.eof())
      {
        Token = T_EOF;

      /* Check if start with digit (don't forget negative numbers !!! */
      } else if (isdigit(c)/*||(c == '-')*/) {  // forget negative numbers !!!
         int Signum = 1;

         // Will not happen, negative numbers will be handled otherwise...
         //if (c == '-') {
         //   Input.get(c);
         //   if (isdigit(c)) {
         //      Signum = -1;
         //   } else {
         //      Token = T_SUB;
         //      /* put the character back for further examination */
         //      Input.putback(c);
         //      return;
         //   }
         //}

         /* put the character back for further examination */
         Input.putback(c);

         /* Get the number */
         Input >> Value;
         if (Signum < 0) {
            Value = -Value;
         }
         Token = T_VALUE;

      } else if (isalpha(c)) {

         /* Read in the Keyword or identifier */
         for (Ptr = Buffer; isalnum(c)&&(!Input.eof()); )
         {
           *Ptr++ = c;
            if (!Input)//!Input.good())
            {
               throw (BTError(_BTERROR::BT_IO_ERROR, 1, CurrentLine));
            }
            if (Ptr >= (&(Buffer[MAX_BUFFER-2])))
            {
               throw (BTError(_BTERROR::BT_IDENTIFIER_TOO_LONG, 0, CurrentLine));
            }
            /* get next character */
            Input.get(c);
         }
         /* Put back the character read to much */
         Input.putback(c);

         /* End the identifierstring */
         *Ptr = 0;

         /* Analyze the identifier */
         AnalyzeKeyWord(Buffer);

      } else if (c == '\"') {

         /* get next character */
         Input.get(c);

         /* Read in the String */
         for (Ptr = Buffer; c != '\"'; )
         {
           *Ptr++ = c;
            if (!Input)//!Input.good())
            {
               throw (BTError(_BTERROR::BT_IO_ERROR, 2, CurrentLine));
            }
            if (Ptr >= (&(Buffer[MAX_BUFFER-2])))
            {
               throw (BTError(_BTERROR::BT_STRING_TOO_LONG, 0, CurrentLine));
            }
            if (!isprint(c))
            {
               throw (BTError(_BTERROR::BT_ILLEGAL_CHARACTER_IN_STRING, 0, CurrentLine));
            }
            /* get next character */
            Input.get(c);
         }

         /* End the identifierstring */
         *Ptr = 0;

         /* No Keyword, must be variable */
         Token = T_STRING;

      } else if (c == '+') {
         Token = T_ADD;
      } else if (c == '-') {
         Token = T_SUB;
      } else if (c == '*') {
         Token = T_MUL;
      } else if (c == '/') {
         /* get next character */
         Input.get(c);
         if (c == '*') {
            /* comment started, read till comment end */
            Input.get(c);

            while (1) {
               char Lastchar = c;
               Input.get(c);

               if (Input.eof())
               {
                  throw (BTError(_BTERROR::BT_UNEXPECTED_FILEEND_IN_COMMENT, 1, CurrentLine));
               }
               if ((Lastchar == '*')&&(c=='/')) {
                  /* End of comment found */
                  break;
               }
               if (Lastchar == '\n') {
                  CurrentLine++;
               }
            }
            /* fetch next token */
            goto restart;

         } else if (c == '/') {
            // Single line comment
            while (c != '\n') {
               Input.get(c);
               if (Input.eof()) {
                  Token = T_EOF;
                  return;
               }
            }
            CurrentLine++;
            Token = T_NEWLINE;
         } else {
           /* Put back the character read to much */
           Input.putback(c);

           Token = T_DIV;
        }
      } else if (c == ';') {
         // Single line comment
         while (c != '\n') {
            Input.get(c);
            if (Input.eof()) {
               Token = T_EOF;
               return;
            }
         }
         CurrentLine++;
         Token = T_NEWLINE;
      } else if (c == '^') {
         Token = T_POW;
      } else if (c == '?') {
         Token = T_QUESTION;
      } else if (c == '!') {
         Token = T_COMMAND;
      } else if (c == '[') {
         Token = T_OPENBRACKET;
      } else if (c == ']') {
         Token = T_CLOSEBRACKET;
      } else if (c == '{') {
         Token = T_OPENCURLY;
      } else if (c == '}') {
         Token = T_CLOSECURLY;
      } else if (c == '(') {
         Token = T_OPENPARANTHESIS;
      } else if (c == ')') {
         Token = T_CLOSEPARANTHESIS;
      } else if (c == '=') {
         Token = T_ASSIGN;
      } else if (c == ',') {
         Token = T_SEPARATOR;
      } else if (c == ':') {
         /* get next character */
         Input.get(c);
         if (c == ':') {
            /* doublecolon */
           Token = T_DOUBLECOLON;
         } else {
           /* Put back the character read to much */
           Input.putback(c);
           Token = T_COLON;
        }
      } else if (c == '\n') {
         CurrentLine++;
         Token = T_NEWLINE;
      } else if (isprint(c)) {
         Token = T_CHARACTER;
         Character = c;
      } else {
         throw (BTError(_BTERROR::BT_ILLEGAL_CHARACTER, c, CurrentLine));
      }
   } else {
      Token = T_EOF;
   }
}

/*****************************************************************************/
/*  End  Method : NextSymbol                                                 */
/*****************************************************************************/

/*****************************************************************************/
/*  Method      : AnalyzeKeyWord                                             */
/*****************************************************************************/
/*                                                                           */
/*  Function    : Return the Token corresponding to the given string         */
/*                could be done in a hashtable for more efficiency, is       */
/*                currently only a big comparetree, but does not matter much */
/*                since script is parsed only once anyway (and wil never be  */
/*                long enough that performance-gain will be significant)     */
/*                                                                           */
/*  Type        : Private                                                    */
/*                                                                           */
/*  Input Para  : String: String to convert into token                       */
/*                                                                           */
/*  Output Para : None                                                       */
/*                                                                           */
/*  Author      : I. Oesch                                                   */
/*                                                                           */
/*  History     : 01.01.1997  IO  Created                                    */
/*                                                                           */
/*****************************************************************************/

void ParserClass::AnalyzeKeyWord(const char *String)
{
/* Class data declaration      */

/* Class code declaration      */

   if (strcmp(String, "neg") == 0) {
      Token = T_NEG;
   } else if (strcmp(String, "inv") == 0) {
      Token = T_INV;
   } else if (strcmp(String, "abs") == 0) {
      Token = T_ABS;
   } else if (strcmp(String, "sin") == 0) {
      Token = T_SIN;
   } else if (strcmp(String, "cos") == 0) {
      Token = T_COS;
   } else if (strcmp(String, "tan") == 0) {
      Token = T_TAN;
   } else if (strcmp(String, "arcsin") == 0) {
      Token = T_ASIN;
   } else if (strcmp(String, "arccos") == 0) {
      Token = T_ACOS;
   } else if (strcmp(String, "arctan") == 0) {
      Token = T_ATAN;
   } else if (strcmp(String, "arctan2") == 0) {
      Token = T_ATAN2;
   } else if (strcmp(String, "exp") == 0) {
      Token = T_EXP;
   } else if (strcmp(String, "ln") == 0) {
      Token = T_LN;
   } else if (strcmp(String, "square") == 0) {
      Token = T_SQUARE;
   } else if (strcmp(String, "sqrt") == 0) {
      Token = T_SQRT;
   } else {
      Token = T_IDENTIFIER;
   }
}

/*****************************************************************************/
/*  End  Method : AnalyzeKeyWord                                             */
/*****************************************************************************/

/*****************************************************************************/
/*  End Class   : ParserClass                                                */
/*****************************************************************************/



