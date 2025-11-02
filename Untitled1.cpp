#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "compact.h"
#include "ParserManual.hpp"
#include "varmanag.hpp"

/* forward declaration */
std::shared_ptr<ExpressionClass>GetExpression(ParserClass &Parser);

/* Special variable for derivation */
VariableClass VarX("x", 3);

static VariableManager Variables;

/*****************************************************************************/
/*  Method      : GetFunction                                                  */
/*****************************************************************************/
/*                                                                           */
/*  Function    : Consumes a factor from an expression and generates the     */
/*                corresponding nodes in the expressiontree                  */
/*                                                                           */
/*  Type        : Global                                                     */
/*                                                                           */
/*  Input Para  : Parser: Parser, delivering tokens from inputstream         */
/*                                                                           */
/*  Output Para : Node for expressiontree                                    */
/*                                                                           */
/*  Author      : I. Oesch                                                   */
/*                                                                           */
/*  History     : 01.01.1997  IO  Created                                    */
/*                                                                           */
/*****************************************************************************/
std::shared_ptr<ExpressionClass>GetFunction(ParserClass &Parser)
{
   /* Method data declaration      */
   std::shared_ptr<ExpressionClass> Ep1;

   /* Method code declaration      */

   /* Get next token from inputstream and handle it */
   switch (Parser.GetLastToken()) {

      /* Expression starting with -, must be negation */
      case ParserClass::T_SUB:
         Parser.NextSymbol();
         /* Create Node, for subnode call ourself recursively */
         Ep1 = std::make_shared<InverseClass>(GetFunction(Parser));
         break;

      /* Expression starting with '(', must be subexpression */
      case ParserClass::T_OPENPARANTHESIS:
         Parser.NextSymbol();

         /* Get and create the subexpression */
         Ep1 = GetExpression(Parser);

         /* Next token must be a ')' */
         Parser.ConsumeToken(ParserClass::T_CLOSEPARANTHESIS);
         break;

      /* Next token is a numerical Value */
      case ParserClass::T_VALUE:
         /* Create a constant node with the value */
         Ep1 = std::make_shared<ConstantClass>(Parser.GetLastValue());
         Parser.NextSymbol();
      break;

      /* Next token is a identifier (Variable) */
      case ParserClass::T_IDENTIFIER:
         /* Check if variable was 'x' (special case for derivation) */
         /* since currently we derivate fixed for x                 */
         if (Parser.GetLastString()[0] == 'x') {
            /* Create reference to Variable x */
            Ep1 = std::make_shared<VariableValueClass>(&VarX);
         } else {
            /* Create reference and new variable */
            Ep1 = std::make_shared<VariableValueClass>(Variables.GetVariableReference(Parser.GetLastString()));
         }
         Parser.NextSymbol();
      break;

      /* Everything else is an error */
      default:
         throw (BTError(_BTERROR::BT_SYNTAX_ERROR));

   }
   return Ep1;
}
/*****************************************************************************/
/*  End  Method : GetFunction                                                  */
/*****************************************************************************/

/*****************************************************************************/
/*  Method      : GetFactor                                                    */
/*****************************************************************************/
/*                                                                           */
/*  Function    : Consumes a Term from an expression and generates the       */
/*                corresponding nodes in the expressiontree                  */
/*                                                                           */
/*  Type        : Global                                                     */
/*                                                                           */
/*  Input Para  : Parser: Parser, delivering tokens from inputstream         */
/*                                                                           */
/*  Output Para : Node for expressiontree                                    */
/*                                                                           */
/*  Author      : I. Oesch                                                   */
/*                                                                           */
/*  History     : 01.01.1997  IO  Created                                    */
/*                                                                           */
/*****************************************************************************/
std::shared_ptr<ExpressionClass>GetFactor(ParserClass &Parser)
{
   /* Method data declaration      */

   /* Method code declaration      */

   /* A term must contain at least a factor, so get it */
   std::shared_ptr<ExpressionClass>Ep1 = GetFunction(Parser);

   /* If next token ist '*', we handle a multiplication */
   if (Parser.GetLastToken() == ParserClass::T_POW) {
      Parser.NextSymbol();
      /* Get second term for power and create power-node */
      std::shared_ptr<ExpressionClass>Ep2 = GetFactor(Parser);
      Ep1 = std::make_shared<PowerClass>(Ep1, Ep2);
   }

   return Ep1;
}
/*****************************************************************************/
/*  End  Method : GetFactor                                                  */
/*****************************************************************************/


/*****************************************************************************/
/*  Method      : GetTerm                                                    */
/*****************************************************************************/
/*                                                                           */
/*  Function    : Consumes a Term from an expression and generates the       */
/*                corresponding nodes in the expressiontree                  */
/*                                                                           */
/*  Type        : Global                                                     */
/*                                                                           */
/*  Input Para  : Parser: Parser, delivering tokens from inputstream         */
/*                                                                           */
/*  Output Para : Node for expressiontree                                    */
/*                                                                           */
/*  Author      : I. Oesch                                                   */
/*                                                                           */
/*  History     : 01.01.1997  IO  Created                                    */
/*                                                                           */
/*****************************************************************************/
std::shared_ptr<ExpressionClass>GetTerm(ParserClass &Parser)
{
   /* Method data declaration      */

   /* Method code declaration      */

   /* A term must contain at least a factor, so get it */
   std::shared_ptr<ExpressionClass>Ep1 = GetFactor(Parser);

   /* If next token ist '*', we handle a multiplication */
   if (Parser.GetLastToken() == ParserClass::T_MUL) {
      Parser.NextSymbol();
      /* Get second factor for multiplication and create multiply-node */
      std::shared_ptr<ExpressionClass>Ep2 = GetTerm(Parser);
      Ep1 = std::make_shared<MultiplyClass>(Ep1, Ep2);

     /* If next token ist '/', we handle a division */
   } else if (Parser.GetLastToken() == ParserClass::T_DIV) {
      Parser.NextSymbol();
      /* Get second factor for division and create divide-node */
      std::shared_ptr<ExpressionClass>Ep2 = GetTerm(Parser);
      Ep1 = std::make_shared<MultiplyClass>(Ep1, std::make_shared<InverseClass>(Ep2));
   }
   return Ep1;
}
/*****************************************************************************/
/*  End  Method : GetTerm                                                    */
/*****************************************************************************/

/*****************************************************************************/
/*  Method      : GetExpression                                              */
/*****************************************************************************/
/*                                                                           */
/*  Function    : Consumes an Expression from an expression and generates    */
/*                the corresponding nodes in the expressiontree              */
/*                                                                           */
/*  Type        : Global                                                     */
/*                                                                           */
/*  Input Para  : Parser: Parser, delivering tokens from inputstream         */
/*                                                                           */
/*  Output Para : Node for expressiontree                                    */
/*                                                                           */
/*  Author      : I. Oesch                                                   */
/*                                                                           */
/*  History     : 01.01.1997  IO  Created                                    */
/*                                                                           */
/*****************************************************************************/
std::shared_ptr<ExpressionClass>GetExpression(ParserClass &Parser)
{
   /* Method data declaration      */

   /* Method code declaration      */

   /* An expression must contain at least a term, so get it */
   std::shared_ptr<ExpressionClass>Ep1 = GetTerm(Parser);

   /* If next token ist '+', we handle a addition */
   if (Parser.GetLastToken() == ParserClass::T_ADD) {
      Parser.NextSymbol();

      /* Get second expression for addition and create addition-node */
      std::shared_ptr<ExpressionClass>Ep2 = GetExpression(Parser);
      Ep1 = std::make_shared<AdditionClass>(Ep1, Ep2);

   /* If next token ist '-', we handle a subtraction */
   } else if (Parser.GetLastToken() == ParserClass::T_SUB) {
      Parser.NextSymbol();

      /* Get second expression for subtraction and create subtraction-node */
      std::shared_ptr<ExpressionClass>Ep2 = GetExpression(Parser);
      Ep1 = std::make_shared<AdditionClass>(Ep1, std::make_shared<NegationClass>(Ep2));
   }
   return Ep1;
}
/*****************************************************************************/
/*  End  Method : GetExpression                                              */
/*****************************************************************************/

/*****************************************************************************/
/*  Method      : main                                                       */
/*****************************************************************************/
/*                                                                           */
/*  Function    : Startingpoint for our little calculator, shows just the    */
/*                Usage of it                                                */
/*                                                                           */
/*  Type        : Global                                                     */
/*                                                                           */
/*  Input Para  : argc, argv:  Commandline arguments                         */
/*                                                                           */
/*  Output Para : Errorcode for operating-System                             */
/*                                                                           */
/*  Author      : I. Oesch                                                   */
/*                                                                           */
/*  History     : 01.01.1997  IO  Created                                    */
/*                                                                           */
/*****************************************************************************/
int main2(int argc, char *argv[])
{
   /* Method data declaration      */

   /* Method code declaration      */

   std::cout << "Please Enter Formula f(x):";
   try {

      /* Variables to contain the expressiontrees */
      std::shared_ptr<ExpressionClass>Expression = NULL;
      std::shared_ptr<ExpressionClass>Derivated  = NULL;

      /* Create the Parser to read from stdin */
      ParserClass Parser(std::cin);

      /* Fetch first token to start parsing */
      Parser.NextSymbol();

      /* Parse the expression */
      Expression = GetExpression(Parser);

      /* print the parsed expression */
      Expression->Print(std::cout);
      std::cout << std::endl;

      /* Optimize and print the expression multiple times */
      Expression = Expression->Optimize();
      Expression->Print(std::cout);
      std::cout << std::endl;

      Expression = Expression->Optimize();
      Expression->Print(std::cout);
      std::cout << std::endl;

      Expression = Expression->Optimize();
      Expression->Print(std::cout);
      std::cout << std::endl;

      Expression = Expression->Optimize();
      Expression->Print(std::cout);
      std::cout << std::endl;

      /* Create the derivate of the expression */
      Derivated = Expression->Derive(&VarX);
      Derivated->Print(std::cout);
      std::cout << std::endl;

      /* Optimize and print the derivate multiple times */
      Derivated = Derivated->Optimize();
      Derivated->Print(std::cout);
      std::cout << std::endl;

      Derivated = Derivated->Optimize();
      Derivated->Print(std::cout);
      std::cout << std::endl;

      Derivated = Derivated->Optimize();
      Derivated->Print(std::cout);
      std::cout << std::endl;

      /* Create a small function-table for the expressions */
      std::cout << "\nFunctiontable\n" << std::endl;

      for (int i = -3; i < 4; i++) {
         /* Set x to the actual Value and evaluate the expressions */
         VarX.SetValue(i);
         std::cout << i << "    " << Expression->Evaluate() << "    ";
         std::cout << Derivated->Evaluate() << std::endl;
      }

      /* Let the user interactively evaluate the expressions */
      double x = 0;
      do {
         std::cout << "Enter Value to evaluate formula for (99 to end):";
         std::cin >> x;
         VarX.SetValue(x);
         std::cout << "\nf(" << x << ") = " << Expression->Evaluate() << std::endl;
         std::cout << "\nf'(" << x << ") = " << Derivated->Evaluate() << std::endl;
      } while (x != 99);

      std::ofstream Drawing("DrawDot.dot");
      Drawing << "digraph g {" << std::endl;
      Drawing << "node [shape = record,height=.1];" << std::endl;
      Expression->DrawNode(Drawing, 0);
      Drawing << "}" << std::endl;

//      system("dot -Tpng DrawDot.dot -o tree.png");
      system("N:\\Labor\\Info\\Tools\\executables\\graphviz-2.8\\bin\\dot.exe -Tpng DrawDot.dot -o tree.png");

      std::ofstream Drawing2("DrawDot2.dot");
      Drawing2 << "digraph g {" << std::endl;
      Drawing2 << "node [shape = record,height=.1];" << std::endl;
      Derivated->DrawNode(Drawing2, 0);
      Drawing2 << "}" << std::endl;

//      system("dot -Tpng DrawDot2.dot -o tree2.png");
      system("N:\\Labor\\Info\\Tools\\executables\\graphviz-2.8\\bin\\dot.exe -Tpng DrawDot2.dot -o tree2.png");
      //system("N:\\Labor\\Info\\Tools\\Graphviz\\bin\\dot.exe -Tpng DrawDot2.dot -o tree2.png");
  }
  catch (BTError &e) {
     /* Catch any error who might have happened in the above code */
     std::cout << "Error: " << e.GetString() << std::endl;
  }

  /*  Just for Test purposes */
  /*
  std::shared_ptr<ExpressionClass>ep1 = std::make_shared<ConstantClass(3);
  VariableClass *Var = std::make_shared<VariableClass("x", 3);
  std::shared_ptr<ExpressionClass>epx = std::make_shared<VariableValueClass(Var);
  std::shared_ptr<ExpressionClass>ep3 = std::make_shared<MultiplyClass(ep1, epx);
  ep1 = std::make_shared<AdditionClass(std::make_shared<ConstantClass(5), epx);
  ep1 = std::make_shared<SquareRootClass(ep1);
  ep1 = std::make_shared<AdditionClass (ep1, ep3);

  ep1->Print(cout);
  cout << endl;
  ep3 = ep1->Derive(Var);
  ep3->Print(cout);

  ep3 = ep3->Optimize();
  cout << endl;

  ep3->Print(cout);

  cout << endl;
  ep1 = ep3->Derive(Var);
  ep1->Print(cout);

  ep1 = ep1->Optimize();
  cout << endl;

  ep1->Print(cout); */

  system("PAUSE");
  return 0;
}
/*****************************************************************************/
/*  End  Method : main                                                       */
/*****************************************************************************/
