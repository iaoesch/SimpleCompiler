%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.1"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
  # include <string>
  # include <list>
  # include <vector>
  #include"variableclass.h"
  class driver;
  class ExpressionClass;
  class FunctionCallClass;
  class StatementClass;
  class ConditionalExpressionClass;
  class VariableClass;
  class VariableValueClass;
  class IndexExpressionClass;
  class WritableValueClass;

  typedef std::vector<std::shared_ptr<IndexExpressionClass>> IndexList;
  typedef std::variant<std::string, int64_t> KeyTypeUnion;
  typedef std::pair<KeyTypeUnion, Variables::VariableContentClass> MapEntryType;
  typedef std::vector<MapEntryType> MapEntryListType;

  namespace Variiiiables {
     class FunctionDefinitionClass;
     class VariableContentClass;
  }
  typedef std::shared_ptr<Variables::FunctionDefinitionBaseClass> FunctionDefinitionClassSharedPtr;

  class FktDefContainer {
     public:
     std::shared_ptr<Variables::FunctionDefinitionClass> ptr;
  };

}

// The parsing context.
%param { driver& drv }

%locations

%define parse.trace
%define parse.error verbose

%code {
# include "driver.hh"
# include "compact.h"
# include "variableclass.h"
#include "typedescriptorclass.hpp"

}
// typen: integer, float, string, stack, liste, array, map, function, dynamic, fixed

// fixed a = 5.0;
// fixed b = array of nnn integer
// c = "Hallo"
// a = 7;   ok
// b[n] = 7; ok
// a = {1, 4, 8} -> error change type not allowed
// c = 7; -> ok, change type

%define api.token.prefix {TOK_}
%token
  END  0  "end of file"
  ASSIGN  ":="
  MINUS   "-"
  PLUS    "+"
  STAR    "*"
  SLASH   "/"
  LPAREN  "("
  RPAREN  ")"
  LBRACKET "["
  RBRACKET  "]"
  LBRACE   "{"
  RBRACE   "}"
  KOMMA   ","
  DOT      "."
  SEMICOLON  ";"
  COLON      ":"
  TRIPPLEDOT  "..."
  SHIFTLEFT  "<<"
  SHIFTRIGHT  ">>"
  APPEND  "<<="
  REMOVE  "=>>"
  IF       "if"
  THEN     "then"
  ELSE     "else"
  ENDIF    "endif"
  REPEAT  "repeat"
  UNTIL   "until"
  INTRODUCING "introducing"
  FUNCTION "function"
  ENDFUNCTION "endfunction"
  RETURNING "returning"
  SEND "send"
  WITH "with"
  TO "to"
  CLASS "class"
  ENDCLASS "endclass"
  BASED    "based"
  ON      "on"
  METHOD "method"
  TAKING "taking"

  AS "as"
  TYPEOF "typeof"
  HOLDING "holding"
  INTEGER "integer"
  FLOAT "float"
  BOOLEAN "boolean"
  ARRAY "array"
  OF "of"
  LIST "list"
  STACK "stack"
  MAP "map"
  ANY "any"
  AND     "and"
  OR      "or"
  NOT     "not"
  ARROWRIGHT "->"
  ARROWLEFT "<-"
  LESSTHAN "<"
  LESSORSAME "<="
  EQUAL   "=="
  NOTEQUAL "!="
  MORETHAN   ">"
  MOREORSAME   ">="
  COMPILE "compile"
  RUN     "run"
  PRINT    "print"
  INPUT    "input"
  DUMP    "dump"
  TREE    "tree"
  DEBUG   "debug"
;

%token <std::string> IDENTIFIER "identifier"
%token <int64_t> INTEGER_LIT "integerliteral"
%token <std::string> STRING_LIT "stringliteral"
%token <double> FLOAT_LIT "floatliteral"


%type  <std::shared_ptr<ExpressionClass>> exp comp term factor unary primary exp_or_star
%type  <std::list<std::shared_ptr<StatementClass>>> statements
%type  <std::shared_ptr<StatementClass>> statement
%type  <std::shared_ptr<StatementClass>> assignment
%type  <std::shared_ptr<StatementClass>> referement
%type  <std::shared_ptr<StatementClass>> loopstatement ifstatement returnstatement
%type  <FunctionDefinitionClassSharedPtr> functiondefinition functionBodydefinition Anonymeousfunctiondefinition
%type  <std::shared_ptr<VariableValueClass>> variabledefinition
%type  <std::shared_ptr<FunctionCallClass>> functioncall sendmessage
%type  <std::shared_ptr<ConditionalExpressionClass>> condexp
%type  <std::vector<std::shared_ptr<VariableClass>>> argumentlist
%type  <std::shared_ptr<StatementClass>> Positionalparameter Namedparameter
%type  <std::list<std::shared_ptr<StatementClass>>> parameterlist Positionalparameterlist Namedparameterlist messageparameterlist
%type  <std::shared_ptr<WritableValueClass>> assignable
%type  <Variables::VariableContentClass> literal mapliteral
%type  <Variables::VariableContentClass> numericliteral
%type  <Variables::VariableContentClass> arrayliteral listliteral
%type  <Variables::VariableContentClass> arraycontentliteral
%type  <Variables::ArrayClass::ArrayContentType> arrayentries
%type  <Variables::ArrayClass::ArrayContentType> subarrayliteral
%type  <Variables::ArrayClass::VectorOfRows> arraysequence
%type  <Variables::ArrayClass::Row> literalsequence
%type  <Variables::ListClass> listentries
%type  <std::unique_ptr<VariableTypeDescriptorClass>> typedefinition returntype.opt
%type  <std::vector<std::shared_ptr<ExpressionClass>>> print explist
%type  <std::vector<int64_t>> Dimensions
%type  <MapDescriptorClass::KeyTypesType> keytype mapkeytype
%type  <std::shared_ptr<IndexExpressionClass>> rangedindex combinedindex
%type  <IndexList> rangedindexes
%type  <KeyTypeUnion> key
%type  <MapEntryType> mapentry
%type  <MapEntryListType> mapentries





%printer { yyoutput << $$; } <*>;
%printer { yyoutput << "array row [ , , ..."  << "]"; } <Variables::ArrayClass::Row>
%printer { yyoutput << "array vector of rows [[],[] .. []"  << "]"; } <Variables::ArrayClass::VectorOfRows>
%printer { yyoutput << "array content"; } <Variables::ArrayClass::ArrayContentType>
%printer { yyoutput << "Statement list[" << $$.size() << "]"; } <std::list<std::shared_ptr<StatementClass>>>
%printer { yyoutput << "Parameter list[" << $$.size() << "]"; } <std::vector<std::shared_ptr<VariableClass>>>
%printer { yyoutput << "expression list[" << $$.size() << "]"; } <std::vector<std::shared_ptr<ExpressionClass>>>
%printer { yyoutput << "dimension vector[" << $$.size() << "]"; } <std::vector<int64_t>>
%printer { yyoutput << "map key [" << int($$) << "]"; } <MapDescriptorClass::KeyTypesType>
%printer { yyoutput << "IndexExpression "; }<std::shared_ptr<IndexExpressionClass>>
%printer { yyoutput << "IndexList "; }<IndexList>
%printer { yyoutput << "MapEntryListType "; }<MapEntryListType>
%printer { yyoutput << "MapEntryType "; }<MapEntryType>
%printer { yyoutput << "KeyTypeUnion "; }<KeyTypeUnion>
%printer { yyoutput << "List "; }<Variables::ListClass>

%%
%start unit;
unit:
   input
|  unit input
;

input:
   statement {/*drv.execute($1)*/; drv.AddStatement($1);}
|  definition
|  command ";"
;

command:
   "dump" {drv.Dump();}
|  "debug" "integerliteral" {set_debug_level($2);}
|  "run"  {drv.Run();}
|  "run" "identifier" {drv.Run($2);}
|  "identifier" {drv.Print($1);}
|  "tree" "stringliteral" {drv.Tree($2);}
;

statements:
  definition             {$$ = std::list<std::shared_ptr<StatementClass>>();}
| statements definition  {$$ = $1;}
| statement              {$$ = std::list<std::shared_ptr<StatementClass>>(); $$.push_back($1);}
| statements statement   {$1.push_back($2); $$ = $1;}
;

/*| statements error ";"  {  drv.halt(); yyerrok; $$ = $1; drv.GetOutputStream() << "size = " << $1.size() << std::endl; /* simple error recovery  };*/



statement:
  assignment ";"        {$$ = $1;}
| loopstatement ";"     {$$ = $1;}
| ifstatement ";"       {$$ = $1;}
| returnstatement ";"   {$$ = $1;}
| print ";"             {$$ = std::make_shared<PrintStatementClass>($1, @$);}
| functioncall ";"      {$$ = std::make_shared<FunctionCallStatementClass>($1, @$);}
| sendmessage ";"       {$$ = std::make_shared<FunctionCallStatementClass>($1, @$);}
| error ";"             {$$ = std::make_shared<ErrorStatement>(@$);}
;

print:
  "print" "(" explist ")" {$$ = $3;}
;

explist:
   exp              {$$ = std::vector<std::shared_ptr<ExpressionClass>>(); $$.push_back($1);}
|  explist "," exp  {$$ = $1; $$.push_back($3);}
;

returnstatement:
  "returning" exp {$$ = std::make_shared<ReturningStatementClass>($2, @$);}
;

loopstatement:
  "repeat" statements "until" "(" condexp ")" {$$ = std::make_shared<RepeatLoopClass>($2, $5, @$);}
;


ifstatement:
  "if" "(" condexp ")" "then" statements "else" statements "endif" {$$ = std::make_shared<ifClass>($6, $8, $3, @$);}
;

assignment:
  assignable ":=" exp {
                            $$ = std::make_shared<AssignementClass>($3, $1, @$);
                            drv.GetOutputStream() << "asg:";
                       /*     $3->Print(drv.GetOutputStream());
                            $$->Print(drv.GetOutputStream());
                            try { drv.GetOutputStream() << "eval:" << $3->Evaluate();}
                            catch (...) {
                               drv.GetOutputStream() << "eval: <Exception>";
                            }*/

                      }
| assignable ":=" Anonymeousfunctiondefinition {
                                                 $$ = std::make_shared<AssignementClass>(std::make_shared<ConstantClass>(Variables::VariableContentClass($3), @1), $1, @$);
                                                 drv.GetOutputStream() << "asg:";
                                                 $3->Print(drv.GetOutputStream());
                                                 $$->Print(drv.GetOutputStream());
                                                 /*drv.GetOutputStream() << "eval:" << $3->Evaluate();*/
                                                }

| assignable ":=" typedefinition {
    $$ = std::make_shared<AssignementClass>(std::make_shared<ConstantClass>($3->ToValueType(), @3), $1, @$);
    drv.GetOutputStream() << "asg:";
    drv.GetOutputStream() << $3;
    $$->Print(drv.GetOutputStream());


}
;

/* assignable ":=" exp { $$ = std::make_shared<AssignementClass>($3, drv.Variables.GetOrCreateVariable($1, $3->Type(), 0.0)); }; */

assignable:
  "identifier"  { $$ = std::make_shared<VariableValueClass>(drv.Variables.GetVariableReferenceCreateIfNotFound($1, VariableTypeDescriptorClass(TypeDescriptorClass::Type::Undefined)), @1); }
| assignable "[" rangedindexes "]" { $$ = std::make_shared<IndexedValueClass>($1, $3, @$);}
| assignable "[" exp "]" { $$ = std::make_shared<IndexedValueClass>($1, $3, @$);}
| assignable "{" rangedindexes "}"
;


rangedindexes:
   rangedindex   {$$ = IndexList(); $$.push_back($1);}
|  exp "," combinedindex {$$ = IndexList(); $$.push_back(std::make_shared<SingleIndexExpressionClass>($1));  $$.push_back($3);}
|  rangedindexes "," combinedindex {$$ = $1;  $$.push_back($3);}
;

combinedindex:
   exp           {$$ = std::make_shared<SingleIndexExpressionClass>($1);}
|  rangedindex   {$$ = $1;}
;

rangedindex:
   "..." exp     {$$ = std::make_shared<RangedIndexExpressionClass>(nullptr, $2);}
|  exp "..."     {$$ = std::make_shared<RangedIndexExpressionClass>($1, nullptr);}
|  exp "..." exp {$$ = std::make_shared<RangedIndexExpressionClass>($1, $3);}
|  "..."         {$$ = std::make_shared<RangedIndexExpressionClass>(nullptr, nullptr);}
;


referement:
  "identifier" "->" exp { $$ = std::make_shared<AssignementClass>($3, drv.Variables.GetOrCreateVariable($1, 0.0), @1+@2); };

definition:
  functiondefinition {}
|  methodedefinition {}
|  variabledefinition ";" {}
;

variabledefinition:
  "identifier" "as" typedefinition  { $$ = std::make_shared<VariableValueClass>(drv.Variables.GetVariableReferenceCreateIfNotFound($1, *$3), @$); }
| "identifier" "as" typedefinition "=" exp
| "identifier" "as" "class"
| "identifier" "as" "class" "from" "identifier"
| "class" "identifier" {drv.CurrentClass.StartClassDefinition($2);} baseclass.opt {drv.CurrentClass.StartMemberDefinition();} attributes "endclass" {drv.CurrentClass.EndMemberDefinition(); drv.CurrentClass.EndClassDefinition();}
;

baseclass.opt:
   %empty                    {drv.CurrentClass.SetBaseClass("");}
|  "based" "on" "identifier" {drv.CurrentClass.SetBaseClass($3);}
;

attributes:
   %empty
|  attributelist
;

attributelist:
   singleattribute
|  attributelist singleattribute
;

singleattribute:
   variabledefinition ";"
|  "static" variabledefinition ";"
;

keytype:
  "integer"  { $$ = MapDescriptorClass::KeyTypesType::Integer;}
| "boolean"  { $$ = MapDescriptorClass::KeyTypesType::Bool;}
| "string"   { $$ = MapDescriptorClass::KeyTypesType::String;}
;

mapkeytype:
   keytype                 { $$ = $1;}
|  mapkeytype "," keytype  { $$ = $1 | $3;}
;

typedefinition:
  "integer"  { $$ = std::make_unique<VariableTypeDescriptorClass>(TypeDescriptorClass::Type::Integer);}
| "boolean"  { $$ = std::make_unique<VariableTypeDescriptorClass>(TypeDescriptorClass::Type::Bool);}
| "string"   { $$ = std::make_unique<VariableTypeDescriptorClass>(TypeDescriptorClass::Type::String);}
| "float"    { $$ = std::make_unique<VariableTypeDescriptorClass>(TypeDescriptorClass::Type::Float);}
| "array" "[" Dimensions "]" "of" typedefinition  { $$ = std::make_unique<VariableTypeDescriptorClass>(ArrayDescriptorClass($3, std::move($6)));}
| "list"  { $$ = std::make_unique<VariableTypeDescriptorClass>(TypeDescriptorClass::Type::List);}
| "any"   { $$ = std::make_unique<VariableTypeDescriptorClass>(TypeDescriptorClass::Type::Dynamic);}
| "stack" "of" typedefinition  { $$ = std::make_unique<VariableTypeDescriptorClass>(StackDescriptorClass(std::move($3)));}
| "map" "[" mapkeytype "]" "of" typedefinition { $$ = std::make_unique<VariableTypeDescriptorClass>(MapDescriptorClass($3, std::move($6)));}
| "typeof" "(" exp ")"   { $$ = std::make_unique<VariableTypeDescriptorClass>($3->Type());}

;

Dimensions:
  exp_or_star     {$$ = std::vector<int64_t>(); $$.push_back($1->Evaluate().GetValue<int64_t>());}
| Dimensions "," exp_or_star {$$ = $1; $$.push_back($3->Evaluate().GetValue<int64_t>());}
;

exp_or_star:
   exp   {$$ = $1;}
|  "*"   {$$ = std::make_shared<ConstantClass>(Variables::VariableContentClass(-1LL), @1);}
;

sendmessage:
  "send"
  "identifier" <FunctionDefinitionClassSharedPtr>{$$ = drv.Currentfunction.BeginFunctionCall($2, @2);}
  "with"
  messageparameterlist
  "to"
  "*" {$$ = std::make_shared<FunctionCallClass>($3, $5, @$);}

|  "send"
   "identifier" <FunctionDefinitionClassSharedPtr>{drv.Currentfunction.BeginMethodCall($2, @2);}
   "with"
   messageparameterlist
   "to"
   "identifier" {$$ = std::make_shared<FunctionCallClass>($3, $5, @$);}

|  "tell"
   "identifier" {drv.Currentfunction.BeginMethodCallForObject($2, @2);}
   "to"
   "identifier" {drv.Currentfunction.SetCalledMethodForObject($5, @5);}
   "with"
   messageparameterlist {drv.Currentfunction.SetParameterAssignListForCalledMethod(std::move($8), @8); $$ = drv.Currentfunction.FinishMethodCall(@$);}
;

messageparameterlist:
  %empty                    {$$ = std::list<std::shared_ptr<StatementClass>>();}
| Namedparameterlist        {$$ = $1;}
;


functioncall:
  "identifier" <FunctionDefinitionClassSharedPtr>{$$ = drv.Currentfunction.BeginFunctionCall($1, @1);}
  "("
  parameterlist
  ")" {$$ = std::make_shared<FunctionCallClass>($2, $4, @$);};

parameterlist:
  %empty                    {$$ = std::list<std::shared_ptr<StatementClass>>();}
| Positionalparameterlist   {$$ = $1;}
| Namedparameterlist        {$$ = $1;}
;

Positionalparameterlist:
  Positionalparameter                            {$$ = std::list<std::shared_ptr<StatementClass>>();$$.push_back($1);}
| Positionalparameterlist "," Positionalparameter    {$$ = $1; $$.push_back($3);}
;

Positionalparameter:
  exp       {$$ = drv.Currentfunction.MakeAssignBySequence($1, @1);}
| "->" exp    {$$ = drv.Currentfunction.MakeRefBySequence($2, @1+@2);}
;


Namedparameterlist:
  Namedparameter                      {$$ = std::list<std::shared_ptr<StatementClass>>(); $$.push_back($1);}
| Namedparameterlist "," Namedparameter   {$$ = $1; $$.push_back($3);}
;

Namedparameter:
   "identifier" "->" exp  {$$ = drv.Currentfunction.MakeRef($1, $3, @1+@2);}
|  "identifier" ":=" exp  {$$ = drv.Currentfunction.MakeAssign($1, $3, @1+@2);};


  /* method print taking a, b, c returning int of class Output:
        x;
        y;
        endmethod;
  */

methodedefinition:
   "method" "identifier" "taking"
   argumentlist
   returntype.opt
   "of"
   "class"
   "identifier" ":"
   statements
   "endmethod"
;

Anonymeousfunctiondefinition:
  "function" {
                 drv.Currentfunction.BeginFunctionDefinition(@1);
             }
  functionBodydefinition
             {
                 $$ = $3;
              }
;


functiondefinition:
  "function" "identifier"
       {
           drv.Currentfunction.BeginFunctionDefinition($2, @2);
       }
  functionBodydefinition
       {
          $$ = $4;
       }
;

functionBodydefinition:
                          {
                             /*FktDefContainer tmp;*/
                             /*auto ptr = drv.Currentfunction.GetReference();*/
                             /*$<FktDefContainer>$ = tmp;*/
                             /*drv.Variables.StartLocal(ptr);*/
                             /*drv.Variables.CreateNewContext(drv.Currentfunction.GetName()+"Params");*/
                             drv.Currentfunction.StartParameterDefinition();
                          }
  returntype.opt
  "(" argumentlist ")"    {
                              drv.Currentfunction.SetReturnType(std::move($2));
                              drv.Currentfunction.Set($4, @4);
                              drv.Currentfunction.EndParameterDefinition();
                              drv.Currentfunction.StartCodeDefinition();
                              /*drv.Variables.CreateNewContext(drv.Currentfunction.GetName());*/
                              //auto ReturnValue = drv.Variables.CreateVariable($<std::string>0, *$2, 0.0);
                          }
  statements
  "endfunction" {
                      /**$<FktDefContainer>3 = Variables::FunctionDefinitionClass($5, $8);*/
                      /*$$ = $<FktDefContainer>3.ptr;*/
                      /*auto StorageTemplate = */
                      /*drv.Variables.EndLocal();*/
                      //drv.Currentfunction.Define($4, $7, std::move(StorageTemplate), @1+@5);
                      drv.Currentfunction.Set($7, @7);
                      drv.Currentfunction.EndCodeDefinition();
                      /*drv.Currentfunction.Set(std::move(StorageTemplate), @1+@5);*/
                      $$ = drv.Currentfunction.Get(@7);
                      drv.Currentfunction.EndFunctionDefinition(@$);
                      /*drv.Variables.LeaveContext(2);*/
                }
| error "endfunction" {$$ = std::make_shared<Variables::FunctionDefinitionClass>(Variables::FunctionDefinitionClass::MakeEmpty());}
;

returntype.opt:
 %empty                       {$$ = std::make_unique<VariableTypeDescriptorClass>(TypeDescriptorClass::Type::Undefined);}
 | "returning" typedefinition {$$ = std::move($2);}

argumentlist:
  "identifier"           {$$ = std::vector<std::shared_ptr<VariableClass>>(); auto var = drv.Variables.CreateVariable($1, VariableTypeDescriptorClass(TypeDescriptorClass::Type::Dynamic), 0.0); $$.push_back(var);}
| argumentlist "," "identifier" {auto var = drv.Variables.CreateVariable($3, VariableTypeDescriptorClass(TypeDescriptorClass::Type::Dynamic), 0.0); $1.push_back(var); $$ = $1; };

%left or;
%left and;
%left not;
%left ">" ">=" "==" "!=" "<" "<=";

// a >  b  -> b < a
// a >= b  -> b <= a
// a > b  -> b < a


condexp:
  condexp and condexp   { $$ = std::make_shared<AndClass>($1, $3, @2); }
| condexp or condexp   { $$ = std::make_shared<AndClass>($1, $3, @2); }
| not condexp    { $$ = std::make_shared<AndClass>($2, $2, @1); }
| exp ">" exp    { $$ = std::make_shared<LessThanClass>($3, $1, @2); }
| exp ">=" exp   { $$ = std::make_shared<LessOrSameThanClass>($3, $1, @2);}
| exp "==" exp   { $$ = std::make_shared<SameAsClass>($1, $3, @2); }
| exp "!=" exp   { $$ = std::make_shared<NotSameAsClass>($1, $3, @2); }
| exp "<" exp    { $$ = std::make_shared<LessThanClass>($1, $3, @2); }
| exp "<=" exp   { $$ = std::make_shared<LessOrSameThanClass>($1, $3, @2); }
| "(" condexp ")"   { std::swap ($$, $2); };


//%left "+" "-";
//%left "*" "/";
//expold:
//  expold "+" expold   { $$ = std::make_shared<AdditionClass>($1, $3); }
//| expold "-" expold   { $$ = std::make_shared<AdditionClass>($1, std::make_shared<NegationClass>($3)); }
//| expold "*" expold   { $$ = std::make_shared<MultiplyClass>($1, $3); }
//| expold "/" expold   { $$ = std::make_shared<MultiplyClass>($1, std::make_shared<InverseClass>($3)); }
//| "(" expold ")"   { std::swap ($$, $2); }
//| "identifier"  { $$ = std::make_shared<VariableValueClass>(drv.Variables.GetVariableReferenceCreateIfNotFound($1, TypeDescriptorClass(TypeDescriptorClass::Type::Undefined))); }
//| literal       { $$ = std::make_shared<ConstantClass>($1); }
//;

exp
: comp          { std::swap ($$, $1); }
| exp "<<=" comp { $$ = std::make_shared<CompositionClass>($1, $3, @2); }
;

comp
: term          { std::swap ($$, $1); }
| comp "+" term { $$ = std::make_shared<AdditionClass>($1, $3, @2); }
| comp "-" term { $$ = std::make_shared<AdditionClass>($1, std::make_shared<NegationClass>($3, @2), @2); }
;

term
: factor           { std::swap ($$, $1); }
| term "*" factor  { $$ = std::make_shared<MultiplyClass>($1, $3, @2); }
| term "/" factor  { $$ = std::make_shared<MultiplyClass>($1, std::make_shared<InverseClass>($3, @2), @2); }
;

factor
: unary  {
             std::swap ($$, $1);
             auto exp = $$;
             exp->Print(drv.GetOutputStream());
         }
;

unary
: primary    { std::swap ($$, $1); $$->Print(drv.GetOutputStream()); }
| "+" unary  { std::swap ($$, $2); }
| "-" unary  {
                 $$ = std::make_shared<NegationClass>($2, @1);
              }
| unary "=>>"  {
                    $$ = std::make_shared<ExtractionClass>($1, @1);
                }
;

primary:
  assignable    { $$ = $1; }
| literal       { $$ = std::make_shared<ConstantClass>($1, @1); drv.GetOutputStream() << "parser: constant " << $1 << "\n";}
| "(" exp ")"   { std::swap ($$, $2); }
| functioncall  { $$ = $1;}
;



literal:
  arraycontentliteral {$$ = $1;}
| arrayliteral   {$$ = $1;}
;

arraycontentliteral:
  numericliteral {$$ = $1;}
| "stringliteral"       {$$ = Variables::VariableContentClass($1); }
| listliteral    {$$ = $1;}
| mapliteral     {$$ = $1;}
| typedefinition {
    $$ = Variables::VariableContentClass($1->ToValueType()); }
;

numericliteral:
  "integerliteral"   { $$ = Variables::VariableContentClass($1); }
| "floatliteral"     { $$ = Variables::VariableContentClass($1); }
;

arrayliteral:
   "[" arrayentries "]" {$$ = Variables::VariableContentClass(Variables::ArrayClass($2));}
;

arrayentries:
   literalsequence {$$ = Variables::ArrayClass::ArrayContentType($1);}
|  arraysequence   {$$ = Variables::ArrayClass::ArrayContentType($1);}
;

arraysequence:
   subarrayliteral     {$$ = Variables::ArrayClass::CreateRowOfRows(); $$.AppendElement($1);}
|  arraysequence "," subarrayliteral {$$ = $1;  $$.AppendElement($3);}
;

subarrayliteral:
   "[" arrayentries "]" {$$ = $2;}
;

literalsequence:
   arraycontentliteral   {$$ = Variables::ArrayClass::CreateRowOfValues(); $$.AppendElement($1);}
|  literalsequence "," arraycontentliteral {$$ = $1;  $$.AppendElement($3);}
;

listliteral:
   "{" listentries "}"    {$$ = Variables::VariableContentClass($2);}
;

listentries:
   literal                  {$$ = Variables::ListClass(); $$.Append(std::make_unique<Variables::VariableContentClass>($1));}
|  listentries "," literal  {$$ = $1; $$.Append(std::make_unique<Variables::VariableContentClass>($3));}
;

mapliteral:
   "[" mapentries "]"      {$$ = Variables::VariableContentClass(Variables::MapClass($2));}
;

mapentries:
   mapentry                 { $$ = MapEntryListType(); $$.push_back($1);}
|  mapentries "," mapentry  { $$ = $1; $$.push_back($3);}
;

mapentry:
   "<" key ":=" literal ">"  {$$ = {$2, $4};}
;

key:
   "stringliteral"    {$$ = $1;}
|  "integerliteral"   {$$ = $1;}
;

%%


/*
expr
: term
| term '+' term
| term '-' term
;

term
: factor
| factor '*' factor
| factor '/' factor
| factor '%' factor // if you have the % operator
;

factor
: unary
| unary '^' factor // if you have an exponentiation operator. Note right-associativity
;

unary
: primary
| '+' unary
| '-' unary
;

primary
: id
| constant
| '(' expr ')'
;
*/

void
yy::parser::error (const location_type& l, const std::string& m)
{
  std::cerr << l;
  std::cerr << ": ";
  std::cerr << m << '\n';
  for (int i = 0; i < l.begin.column; i++) {
     std::cerr << "~";
  }
  for (int i = l.begin.column; i <= l.end.column; i++) {
     std::cerr << "^";
  }
  std::cerr << '\n';
  //std::cerr << l << ": " << m << '\n';
  drv.ReportError(l, m);
}
