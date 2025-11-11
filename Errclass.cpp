/*****************************************************************************/
/*  Class      : BTerror                                        Version 1.0  */
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
/*  History    : 15.02.1997  IO Created                                      */
/*                                                                           */
/*  File       : errclass.cpp                                                */
/*                                                                           */
/*****************************************************************************/
/*    SWS Diplomarbeit 1997 Bonetracker from Ivo Oesch and Daniel Buergin    */
/*****************************************************************************/

/* imports */
#include "errclass.hpp"
#include "errtexts.ipp"
#include <stdio.h>

/* Class constant declaration  */

/* Class Type declaration      */

/* Class data declaration      */

/* Class procedure declaration */

/*****************************************************************************/
/*  Method      : ErrorBaseClass                                                    */
/*****************************************************************************/
/*                                                                           */
/*  Function    : Constructors for errorclass                                */
/*                                                                           */
/*  Type        : constructor                                                */
/*                                                                           */
/*  Input Para  : Initializing-values                                        */
/*                                                                           */
/*  Output Para : None                                                       */
/*                                                                           */
/*  Author      : I. Oesch                                                   */
/*                                                                           */
/*  History     : 15.02.1997  IO  Created                                    */
/*                                                                           */
/*****************************************************************************/
#if 0
ErrorBaseClass::ErrorBaseClass(ErrorBaseClass::ErrorCodes e, int q, int l) 
  : Line(l), 
    Qualifier(q), 
    Where(NULL),
    What(NULL),
    SourceFile("") ,
    SourceLine(0) ,
    ErrorCode(e)
{ 
}

ErrorBaseClass::ErrorBaseClass(ErrorBaseClass::ErrorCodes e, const char *aWhere, const char *aWhat) 
  : Line(0),
    Qualifier(0), 
    Where(aWhere), 
    What(aWhat), 
    SourceFile(""),
    SourceLine(0),
    ErrorCode(e)
{
}


#ifdef DEBUG_INFO_ENABLED
ErrorBaseClass::ErrorBaseClass(const char *File, int CodeLine, ErrorBaseClass::ErrorCodes e, int q, int l)
  : Line(l),
    Qualifier(q), 
    Where(NULL),
    What(NULL),
    SourceFile(File),
    SourceLine(CodeLine),
    ErrorCode(e) 
{  
}

ErrorBaseClass::ErrorBaseClass(const char *File, int CodeLine, ErrorBaseClass::ErrorCodes e, const char *aWhere, const char *aWhat)
  : Line(0),
    Qualifier(0), 
    Where(aWhere), 
    What(aWhat), 
    SourceFile(File), 
    SourceLine(CodeLine),
    ErrorCode(e)
{  
}
#endif
 
/*****************************************************************************/
/*  End  Method : BTError                                                    */
/*****************************************************************************/


/*****************************************************************************/
/*  Method      : GetString                                                  */
/*****************************************************************************/
/*                                                                           */
/*  Function    : Delivers back acording string for errorcode                */
/*                                                                           */
/*  Type        : public                                                     */
/*                                                                           */
/*  Input Para  : None                                                       */
/*                                                                           */
/*  Output Para : Pointer to error-descripting string                        */
/*                                                                           */
/*  Author      : I. Oesch                                                   */
/*                                                                           */
/*  History     : 15.02.1997  IO  Created                                    */
/*                                                                           */
/*****************************************************************************/

const char *ErrorBaseClass::GetString(void) 
{
/* Class data declaration      */

   typedef struct ErrorInfoType {
      ErrorCodes   Error;
      int          Type;
      const char * Text;
   } ErrorInfoType; 
   
   static char ErrorMessageBuffer[80];
   
   static const ErrorInfoType ErrorTexts[] =
   {
     {BT_NO_ERROR,                        0, BT_NO_ERROR_TXT},
     {BT_INTERNAL_INCONSISTENCY,          0, BT_INTERNAL_INCONSISTENCY_TXT},     
     {BT_NOT_IMPLEMENTED_YET,             0, BT_NOT_IMPLEMENTED_YET_TXT},  
     {BT_OUT_OF_MEMORY,                   0, BT_OUT_OF_MEMORY_TXT},
     {BT_INDEX_OUT_OF_RANGE,              0, BT_INDEX_OUT_OF_RANGE_TXT}, 
     {BT_DIVISION_BY_ZERO,                0, BT_DIVISION_BY_ZERO_TXT},
     {BT_SYNTAX_ERROR,                    0, BT_SYNTAX_ERROR_TXT},
     {BT_RESERVED_KEYWORD_ERROR,          0, BT_RESERVED_KEYWORD_ERROR_TXT},      
     {BT_IDENTIFIER_TOO_LONG,             0, BT_IDENTIFIER_TOO_LONG_TXT},  
     {BT_STRING_TOO_LONG,                 0, BT_STRING_TOO_LONG_TXT},
     {BT_ILLEGAL_CHARACTER_IN_STRING,     0, BT_ILLEGAL_CHARACTER_IN_STRING_TXT},          
     {BT_ILLEGAL_CHARACTER,               0, BT_ILLEGAL_CHARACTER_TXT}, 
     {BT_ILLEGAL_ARGUMENT,                0, BT_ILLEGAL_ARGUMENT_TXT},
     {BT_UNEXPECTED_TOKEN,                0, BT_UNEXPECTED_TOKEN_TXT},
     {BT_UNEXPECTED_END_OF_FILE,          0, BT_UNEXPECTED_END_OF_FILE_TXT},     
     {BT_TYPE_MISSMATCH,                  0, BT_TYPE_MISSMATCH_TXT},
     {BT_UNEXPECTED_FILEEND_IN_COMMENT,   0, BT_UNEXPECTED_FILEEND_IN_COMMENT_TXT},            
     {BT_INTERNAL_FATAL,                  0, BT_INTERNAL_FATAL_TXT},
     {BT_IO_ERROR      ,                  0, BT_IO_ERROR_TXT},
     
   };          
      
/* Class code declaration      */
   for (int i = 0; i <= (sizeof (ErrorTexts)/sizeof (ErrorTexts[0])); i++) {
      if (ErrorTexts[i].Error == ErrorCode) {
         return (ErrorTexts[i].Text);
      }
   }
   sprintf(ErrorMessageBuffer, BT_UNKNOWN_ERRORCODE_TXT, ErrorCode);
   return (ErrorMessageBuffer);
}

/*****************************************************************************/
/*  End  Method : GetString                                                  */
/*****************************************************************************/

/*****************************************************************************/
/*  Method      : GetDebugInfo                                               */
/*****************************************************************************/
/*                                                                           */
/*  Function    : Delivers back acording string for errorcode                */
/*                                                                           */
/*  Type        : public                                                     */
/*                                                                           */
/*  Input Para  : None                                                       */
/*                                                                           */
/*  Output Para : Pointer to debuginfo-string                                */
/*                                                                           */
/*  Author      : I. Oesch                                                   */
/*                                                                           */
/*  History     : 15.02.1997  IO  Created                                    */
/*                                                                           */
/*****************************************************************************/

#ifdef DEBUG_INFO_ENABLED

const char *ErrorBaseClass::GetDebugInfo(void) 
{
/* Class data declaration      */

   static char ErrorMessageBuffer[200];
   
/* Class code declaration      */
   if (SourceFile != NULL) {
      sprintf(ErrorMessageBuffer, "(Debuginfo: module '%s', line %d)", SourceFile, SourceLine);
      return (ErrorMessageBuffer);
   } else {
      return "(Debuginfo: No location info)";
   }      
}
#endif

/*****************************************************************************/
/*  End  Method : GetDebugInfo                                               */
/*****************************************************************************/

/*****************************************************************************/
/*  End Class   : ErrorBaseClass                                                    */
/*****************************************************************************/

#endif




