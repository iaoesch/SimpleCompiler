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
#include "varmanag.hpp"
#include "compact.h"
#include "Errclass.hpp"

/* Class constant declaration  */

/* Class Type declaration      */

/* Class data declaration      */

/* Class procedure declaration */

/*****************************************************************************/
/*  Method      : BTError                                                    */
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
BTError::BTError(BTError::ErrorCodes e, int q, int l) 
  : Line(l), 
    Qualifier(q), 
    Where(NULL),
    What(NULL),
    SourceFile("") ,
    SourceLine(0) ,
    ErrorCode(e)
{ 
}

BTError::BTError(BTError::ErrorCodes e, const char *aWhere, const char *aWhat) 
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
BTError::BTError(const char *File, int CodeLine, BTError::ErrorCodes e, int q, int l)
  : Line(l),
    Qualifier(q), 
    Where(NULL),
    What(NULL),
    SourceFile(File),
    SourceLine(CodeLine),
    ErrorCode(e) 
{  
}

BTError::BTError(const char *File, int CodeLine, BTError::ErrorCodes e, const char *aWhere, const char *aWhat)
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

const char *BTError::GetString(void) 
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

const char *BTError::GetDebugInfo(void) 
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

extern VariableType RecursiveDefined;
#define RECURSIVE_ERROR_FLAG (&RecursiveDefined)

extern VariableType *MakeVariable(char *Name, ExpressionNodeType *Value);
extern ExpressionNodeType *EvaluateVariable(VariableType *Variable);
extern char *GetVariableName(VariableType *Variable);
extern void  AssignVariable(VariableType *Variable, ExpressionNodeType *Value);
extern ExpressionNodeType *EvaluateGlobalVariableByName(char *Name);
extern void DumpVariableTable(OutputStreamType *Stream);



extern FunctionType *DefineFunction(char *Name, ExpressionNodeType *Formula, ExpressionNodeType *ParameterList);
extern ExpressionNodeType *GetFunctionFormula(FunctionType *Variable);
extern ExpressionNodeType *GetFunctionParameterList(FunctionType *Variable);
extern char *GetFunctionName(FunctionType *Variable);
extern void DumpFunctionTable(OutputStreamType *Stream);


extern void StartLocalTable(void);
extern void StopLocalTable(void);
extern void ClearLocalTable(void);



enum VariableLookup {GLOBAL_ONLY = 1, LOCAL_ONLY = 2};

struct Variable {
   char *Name;
   ExpressionNodeType *Value;
   VariableType *Next;
};

struct Function {
   char *Name;
   ExpressionNodeType *Formula;
   ExpressionNodeType *Parameters;
   FunctionType *Next;
};

VariableType RecursiveDefined = {"<Recursive Defined!>", NULL, NULL};

static FunctionType *GlobalFunctionTable = NULL;
static VariableType *GlobalVariableTable = NULL;
static VariableType *LocalVariableTable = NULL;
static int CreateLocalVariablesEnabled = 0;

static VariableType *CreateVariable(char *Name, ExpressionNodeType *Value)
{
   VariableType *Var = malloc(sizeof(VariableType) + strlen(Name) + 2);
   if (Var == NULL) {
      printf("\nOut of Memory in CreateVariable\n");
      return NULL;
   }
   Var->Name = ((char *)Var) + sizeof(VariableType);
   strcpy(Var->Name, Name);
   Var->Value = Value;
   Var->Next  = NULL;
   return Var;
}

static VariableType *FindVariable(char *Name, int LocalOrGlobal)
{
   VariableType *p = NULL;

   if ((LocalOrGlobal & GLOBAL_ONLY) == 0) {
      for (p = LocalVariableTable; p != NULL; p = p->Next) {
         if (strcmp(p->Name, Name) == 0) {
            break;
         }
      }
   }
   if (((LocalOrGlobal & LOCAL_ONLY) == 0) && (p == NULL)) {
      for (p = GlobalVariableTable; p != NULL; p = p->Next) {
         if (strcmp(p->Name, Name) == 0) {
            break;
         }
      }
   }
   return p;
}

void DumpVariableTable(OutputStreamType *Stream)
{
   VariableType *p = NULL;

   for (p = GlobalVariableTable; p != NULL; p = p->Next) {

      PRINTF(Stream, "%5s: ", p->Name);
      if (p->Value == NULL) {
         PRINTF(Stream, "<empty>\n");
      } else {
         PrintTree(p->Value, Stream);
         PRINTF(Stream, "\n");
      }
   }
}

VariableType *MakeVariable(char *Name, ExpressionNodeType *Value)
{
   VariableType *p;


#if 0
   for (p = LocalVariableTable; p != NULL; p = p->Next) {
      if (strcmp(p->Name, Name) == 0) {
         break;
      }
   }
   if ((CreateLocalVariablesEnabled == 0) && (p == NULL)) {
      for (p = GlobalVariableTable; p != NULL; p = p->Next) {
         if (strcmp(p->Name, Name) == 0) {
            break;
         }
      }
   }
#else
   p = FindVariable(Name, CreateLocalVariablesEnabled==0?0:LOCAL_ONLY);
#endif

   if (p == NULL) {
      p = CreateVariable(Name, Value);
      if (CreateLocalVariablesEnabled == 1) {
         p->Next = LocalVariableTable;
         LocalVariableTable = p;
      } else {
         p->Next = GlobalVariableTable;
         GlobalVariableTable = p;
      }
      return p;
   } else {
      if (Value != NULL) {
         p->Value = Value;
      }
      return p;
   }
}

ExpressionNodeType *EvaluateVariable(VariableType *Variable)
{
      return Variable->Value;
}

ExpressionNodeType *EvaluateGlobalVariableByName(char *Name)
{
   VariableType *Variable = FindVariable(Name, GLOBAL_ONLY);
   if (Variable != NULL) {
      return Variable->Value;
   } else {
      return NULL;
   }
}

void  AssignVariable(VariableType *Variable, ExpressionNodeType *Value)
{
   Variable->Value = Value;
}


char *GetVariableName(VariableType *Variable)
{
      return Variable->Name;
}

static FunctionType *CreateFunction(char *Name, ExpressionNodeType *Formula, ExpressionNodeType *ParameterList)
{
   FunctionType *Var = malloc(sizeof(FunctionType) + strlen(Name) + 2);
   if (Var == NULL) {
      printf("\nOut of Memory in CreateFunction\n");
      return NULL;
   }
   Var->Name = ((char *)Var) + sizeof(FunctionType);
   strcpy(Var->Name, Name);
   Var->Formula = Formula;
   Var->Parameters = ParameterList;
   Var->Next  = NULL;
   return Var;
}

FunctionType *DefineFunction(char *Name, ExpressionNodeType *Formula, ExpressionNodeType *ParameterList)
{
   FunctionType *p;

   for (p = GlobalFunctionTable; p != NULL; p = p->Next) {
      if (strcmp(p->Name, Name) == 0) {
         break;
      }
   }
   if (p == NULL) {
      p = CreateFunction(Name, Formula, ParameterList);
      p->Next = GlobalFunctionTable;
      GlobalFunctionTable = p;
      return p;
   } else {
      if (Formula != NULL) {
         p->Formula = Formula;
         p->Parameters = ParameterList;
      }
      return p;
   }
}

void DumpFunctionTable(OutputStreamType *Stream)
{
   FunctionType *p;

   for (p = GlobalFunctionTable; p != NULL; p = p->Next) {
      PRINTF(Stream, "%5s(", p->Name);
      if (p->Parameters != NULL) {
         PrintTree(p->Parameters, Stream);
      }
      PRINTF(Stream, "):");
      if (p->Formula == NULL) {
         PRINTF(Stream, "<empty>\n");
      } else {
         PrintTree(p->Formula, Stream);
         PRINTF(Stream, "\n");
      }
   }
}


ExpressionNodeType *GetFunctionFormula(FunctionType *Variable)
{
      return Variable->Formula;
}

ExpressionNodeType *GetFunctionParameterList(FunctionType *Variable)
{
   return Variable->Parameters;
}

char *GetFunctionName(FunctionType *Variable)
{
      return Variable->Name;
}



void StartLocalTable(void)
{
   CreateLocalVariablesEnabled = 1;
}

void StopLocalTable(void)
{
   CreateLocalVariablesEnabled = 0;
}

void ClearLocalTable(void)
{
   LocalVariableTable = NULL;
   CreateLocalVariablesEnabled = 0;
}

#endif
/*****************************************************************************/
/*  End  Method : GetDebugInfo                                               */
/*****************************************************************************/

/*****************************************************************************/
/*  End Class   : BTError                                                    */
/*****************************************************************************/



void VariableManager::CreateNewContext(std::string Name)
{
    if (ContextStack.empty()) {
        ContextStack.push_back(std::make_shared<VariableContextClass>(Name));
    } else {
        ContextStack.push_back(ContextStack.back()->CreateSubContext(Name));
    }
    Contexts.push_back(ContextStack.back());
}

void VariableManager::LeaveContext(int Levels)
{
    while (Levels > 0) {
        if (ContextStack.empty()) {
            throw ERROR_OBJECT("popping empty contextstack");
        }

        ContextStack.pop_back();
        Levels--;
    }
}

void VariableManager::StartLocal()
{
    Local = true;
    LocalOffset = 0;
}

void VariableManager::EndLocal()
{
    Local = false;
    LocalOffset = 0;
}

std::shared_ptr<VariableClass> VariableManager::CreateVariable(std::string Name, const TypeDescriptorClass &Type, double Value)
{
    if (ContextStack.empty()) {
        throw ERROR_OBJECT("No valid context");
        return nullptr;
    }
    std::shared_ptr<VariableClass> Var;
    if (Local) {
        Var = std::make_shared<LocalVariableClass>(Name, LocalOffset++);
    } else {
        Var = std::make_shared<GlobalVariableClass>(Name, Type);
    }
    return ContextStack.back()->RegisterVariable(Name, Var, false);

}


std::shared_ptr<VariableClass> VariableManager::GetOrCreateVariable(std::string Name, const TypeDescriptorClass &Type, double Value)
{
    if (ContextStack.empty()) {
        throw ERROR_OBJECT("No valid context");
        return nullptr;
    }
    auto Var = ContextStack.back()->LookupVariable(Name);
    if (Var != nullptr) {
        return Var;
    } else {
        return CreateVariable(Name, Type, Value);
    }
}


std::shared_ptr<VariableClass> VariableManager::GetVariableReference(std::string Name)
{
    if (ContextStack.empty()) {
        throw ERROR_OBJECT("No valid context");
        return nullptr;
    }
    std::shared_ptr<VariableClass> VarRef = ContextStack.back()->LookupVariable(Name);
    return VarRef;
}

std::shared_ptr<VariableClass> VariableManager::GetVariableReferenceCreateIfNotFound(std::string Name, const TypeDescriptorClass &RequiredType)
{
    if (ContextStack.empty()) {
        throw ERROR_OBJECT("No valid context");
        return nullptr;
    }
    std::shared_ptr<VariableClass> VarRef = GetVariableReference(Name);
    if (VarRef == nullptr) {
        if (Local) {
           VarRef = std::make_shared<LocalVariableClass>(Name, LocalOffset++);
        } else {
           VarRef = std::make_shared<GlobalVariableClass>(Name, RequiredType);
        }
        ContextStack.back()->RegisterVariable(Name, VarRef);
    }
    return VarRef;
}

void VariableManager::Dump(std::ostream &s)
{
    s << "Scopes:" << std::endl;
    for (auto &c: Contexts) {
        c->Dump(s);
    }
}

std::shared_ptr<VariableClass> VariableContextClass::RegisterVariable(const std::string Name, std::shared_ptr<VariableClass> Var, bool OverwriteAllowed)
{
    auto it = Variables.find(Name);
    if (it == Variables.end() || OverwriteAllowed) {
        Var->SetContext(this);
        Variables[Name] = Var;
        return Var;
    }
    return nullptr;


}

std::shared_ptr<VariableClass> VariableContextClass::LookupVariable(const std::string Name)
{
    auto it = Variables.find(Name);
    if (it == Variables.end()) {
        if (ParentContext != nullptr) {
            return ParentContext->LookupVariable(Name);
        }
        return nullptr;
    }
    return it->second;
}

void VariableContextClass::Dump(std::ostream &s)
{
    s << "Context <" << Name << ">" << std::endl;
    s << "Parent: <" << ((ParentContext != nullptr) ? ParentContext->Name : std::string(" --- ")) << ">" << std::endl;
    s << "Children:";
    if (Children.empty()) {
        s << " None " << std::endl;
    } else {
        for (auto &c: Children) {
            s << c->Name << ",";
        }
        s << std::endl;
    }
    s << "Content:" << std::endl;
    for (auto &i: Variables) {
        std::cout << i.first << "{ " << std::endl;
        i.second->Print(s);
        std::cout << "}" << std::endl;

    }

}

