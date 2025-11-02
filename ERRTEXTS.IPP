#ifndef ERRORTEXTS_IPP
#define ERRORTEXTS_IPP
/*****************************************************************************/
/*  Header     : Errortexts                                     Version 1.0  */
/*****************************************************************************/
/*                                                                           */
/*  Function   : Defines the errortexts for bonetracker                      */
/*               (Allows easier porting to other languages)                  */
/*                                                                           */
/*                                                                           */
/*  Author     : I. Oesch                                                    */
/*                                                                           */
/*  History    : 07.03.1997  IO Created                                      */
/*                                                                           */
/*  File       : errtexts.ipp                                                */
/*                                                                           */
/*****************************************************************************/
/*    SWS Diplomarbeit 1997 Bonetracker from Ivo Oesch and Daniel Buergin    */
/*****************************************************************************/

/* imports */

/* Constant definitions */
#define BT_NO_ERROR_TXT                          "no Error"
#define BT_INTERNAL_INCONSISTENCY_TXT            "internal inconsitency"
#define BT_NOT_IMPLEMENTED_YET_TXT               "comand not implemented yet"
#define BT_OUT_OF_MEMORY_TXT                     "out of memory"
#define BT_INDEX_OUT_OF_RANGE_TXT                "index is out of range"
#define BT_SIZE_MISSMATCH_TXT                    "size missmatch"
#define BT_DIVISION_BY_ZERO_TXT                  "divide by zero"
#define BT_IO_ERROR_TXT                          "general io error (File ?)"
#define BT_SYNTAX_ERROR_TXT                      "syntax error"
#define BT_RESERVED_KEYWORD_ERROR_TXT            "keyword is reserved"
#define BT_IDENTIFIER_TOO_LONG_TXT               "identifier is too long"
#define BT_STRING_TOO_LONG_TXT                   "string is too long"
#define BT_ILLEGAL_CHARACTER_IN_STRING_TXT       "illegal character in string"
#define BT_ILLEGAL_CHARACTER_TXT                 "illegal character"
#define BT_ILLEGAL_ARGUMENT_TXT                  "illegal argument(s)"
#define BT_UNEXPECTED_TOKEN_TXT                  "unexpected token found"
#define BT_UNEXPECTED_END_OF_FILE_TXT            "unexpected end of file found"
#define BT_TYPE_MISSMATCH_TXT                    "types don't match"
#define BT_UNEXPECTED_FILEEND_IN_COMMENT_TXT     "fileend in coment encountered"
#define BT_ADBOARD_IO_ERROR_TXT                  "ad board: io error"
#define BT_OPTOTRACK_IO_ERROR_TXT                "optotrack: io error"
#define BT_OPTOTRACK_ILLEGAL_NBR_OF_MARKERS_TXT  "optotrack: illegal number of markers"
#define BT_OPTOTRACK_DATABUFFER_ERROR_TXT        "optotrack: databuffer error"
#define BT_CANNOT_CREATE_DIRECTORY_TXT           "cannot create directory"
#define BT_CANNOT_CHANGE_TO_DIRECTORY_TXT        "cannot change to directory"
#define BT_STACK_IS_EMPTY_TXT                    "stack is empty"
#define BT_NOT_A_VALID_2D_DATAFILE_TXT           "this is not a valid 2d-datafile"
#define BT_NOT_A_VALID_RIGIDBODYFILE_TXT         "this is not a valid rigidbodyfile"
#define BT_IMAGINARY_MARKERS_NOT_SUPPORTED_TXT   "imaginary markers are not suported"
#define BT_TOO_LESS_VALID_MARKERS_TXT            "not enough valid markers"
#define BT_TOO_MUCH_MARKERS_IN_RIGIDBODY_TXT     "too much markers in rigidbody"
#define BT_INVALID_MARKERNUMBER_TXT              "invalid markernumber"
#define BT_MISMATCH_IN_MARKERRANGE_TXT           "number of markers don't match"
#define BT_CONVERT_PROJECT_DATA_ERROR_TXT        "convert projectdata error"
#define BT_INTERNAL_FATAL_TXT                    "internal fatal error"
#define BT_NOT_A_VALID_SPINE_DATAFILE_TXT        "this is not a valid spinedatafile"
#define BT_NOT_A_VALID_ASCII_DATAFILE_TXT        "this is not a valid ASCII datafile"
#define BT_ERROR_IN_ASCII_FILE_FRAMENBRSEQ_TXT   "invalid framenumbersequence in ASCII file"

#define BT_UNKNOWN_ERRORCODE_TXT                 "Unknown internal error (%d)"
#define BT_SCRIPT_ERRORCODE_TXT                  "Error in Scriptfile <%s>, line %d: %s"  

/*****************************************************************************/
/*  End Header  : ErrorTexts                                                 */
/*****************************************************************************/
#endif

