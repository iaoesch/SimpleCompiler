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
#define DEBUG_INFO_ENABLED
#ifndef NULL
#define NULL 0
#endif

// Folowing macro is thought as aide for debugging, it adds
// filename and filenumber to errormessage

// usage:

// throw (BTError(_BTERROR::WHAT_REASON_EVER_IS_APROPRIATE));
// throw (BTError(_BTERROR::WHAT_REASON_EVER_IS_APROPRIATE, additional, arguments));

#ifdef DEBUG_INFO_ENABLED
#define _BTERROR __FILE__, __LINE__, BTError
#else
#define _BTERROR BTError
#endif
/* Class Type declaration      */

/* Class data declaration      */

/* Class definition            */
class BTError
{
   // Data
   public:
               int   Line;            /* Linenumber in Files */
         const int   Qualifier;       /* Qualifier for internal use */
         const char *Where;
         const char *What;
         const char *SourceFile;
         const int   SourceLine;
         enum ErrorCodes
         {
            BT_NO_ERROR         =0,
            BT_INTERNAL_INCONSISTENCY,
            BT_NOT_IMPLEMENTED_YET,
            BT_OUT_OF_MEMORY,
            BT_INDEX_OUT_OF_RANGE,
            BT_DIVISION_BY_ZERO,
            BT_SYNTAX_ERROR,
            BT_RESERVED_KEYWORD_ERROR,
            BT_IDENTIFIER_TOO_LONG,
            BT_STRING_TOO_LONG,
            BT_ILLEGAL_CHARACTER_IN_STRING,
            BT_ILLEGAL_CHARACTER,
            BT_ILLEGAL_ARGUMENT,
            BT_UNEXPECTED_TOKEN,
            BT_UNEXPECTED_END_OF_FILE,
            BT_TYPE_MISSMATCH,
            BT_UNEXPECTED_FILEEND_IN_COMMENT,
            BT_INTERNAL_FATAL,
            BT_IO_ERROR,
         } ErrorCode;

   private:
   
   // Methods
   public:
           BTError(ErrorCodes e = BT_NO_ERROR, int q = 0, int l = 0);
           BTError(ErrorCodes e, const char *aWhere, const char *aWhat = NULL);
#ifdef DEBUG_INFO_ENABLED
           BTError(const char *File, int CodeLine, ErrorCodes e, const char *aWhere, const char *aWhat = NULL);
           BTError(const char *File, int CodeLine, ErrorCodes e = BT_NO_ERROR, int q = 0, int l = 0);

           DEBUG_INFO_ENABLED

           const char *GetDebugInfo(void); 
#endif
           virtual ~BTError(void) {}
           virtual const char *GetString(void);
          
};

/*****************************************************************************/
/*  End Header  : BTError                                                    */
/*****************************************************************************/
#endif


