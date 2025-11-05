#include <iostream>
#include <cmath>
#include <typeinfo>
#include "compact.h"

static int NodeNumber = 1;

using std::endl;

double            ExpressionClass::Evaluate() const {std::cout << "\nVirtual Call expression Evaluate()"; return 0;};
std::shared_ptr<ExpressionClass> ExpressionClass::Derive(VariableReferenceType ToDerive) const {std::cout << "\nVirtual Call expression Derive()"; return NULL;};
void              ExpressionClass::Print(std::ostream &s) const {std::cout << "\nVirtual Call expression print()"; };
std::shared_ptr<ExpressionClass> ExpressionClass::Clone() const {std::cout << "\nVirtual Call expression Clone()"; return NULL;};
std::shared_ptr<ExpressionClass> ExpressionClass::Optimize() {std::cout << "\nVirtual Call expression Optimize()"; return NULL;};
bool              ExpressionClass::IsConstant() {std::cout << "\nVirtual Call expression IsConstant()"; return false;};
bool              ExpressionClass::IsSame(std::shared_ptr<ExpressionClass>Other) {std::cout << "\nVirtual Call expression IsSame()"; return false;};
void              ExpressionClass::DrawNode(std::ostream &s, int MyNodeNumber) const {std::cout << "\nVirtual Call expression DrawNode()";};




double            InverseClass::Evaluate() const {return (1 / Operand->Evaluate()); };
std::shared_ptr<ExpressionClass> InverseClass::Derive(VariableReferenceType TD) const {return std::make_shared<NegationClass>(std::make_shared<MultiplyClass>(std::make_shared<InverseClass>( std::make_shared<SquareClass>(Operand->Clone())), Operand->Derive(TD))); };
void              InverseClass::Print(std::ostream &s) const { s << "1.0 / ("; Operand->Print(s); s << ")";  };
std::shared_ptr<ExpressionClass> InverseClass::Clone() const {return std::make_shared<InverseClass>(*this); };
std::shared_ptr<ExpressionClass> InverseClass::Optimize()
{
  Operand = Operand->Optimize();
  return shared_from_this();
};

void              InverseClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
   int NodeNumber1 = NodeNumber++;
   s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> 1/x |<f2> \"];" << endl;
   s << "\"Node" << MyNodeNumber << "\":f0 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
   Operand->DrawNode(s, NodeNumber1);
};



double            SquareClass::Evaluate() const {double tmp = Operand->Evaluate(); return tmp*tmp; };
std::shared_ptr<ExpressionClass> SquareClass::Derive(VariableReferenceType TD) const {return std::make_shared<MultiplyClass>(std::make_shared<MultiplyClass>(std::make_shared<ConstantClass>(2.0), Operand->Clone()), Operand->Derive(TD)); };
void              SquareClass::Print(std::ostream &s) const { s << "("; Operand->Print(s); s << ")^2.0";  };
std::shared_ptr<ExpressionClass> SquareClass::Clone() const {return std::make_shared<SquareClass>(*this); };
std::shared_ptr<ExpressionClass> SquareClass::Optimize()
{
  Operand = Operand->Optimize();
  return shared_from_this();
};
void              SquareClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
   int NodeNumber1 = NodeNumber++;
   s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> x^2|<f2> \"];" << endl;
   s << "\"Node" << MyNodeNumber << "\":f0 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
   Operand->DrawNode(s, NodeNumber1);
};


double            NegationClass::Evaluate() const {return - Operand->Evaluate(); };
std::shared_ptr<ExpressionClass> NegationClass::Derive(VariableReferenceType TD) const {return std::make_shared<NegationClass>(Operand->Derive(TD)); };
void              NegationClass::Print(std::ostream &s) const { s << "-("; Operand->Print(s); s << ")";  };
std::shared_ptr<ExpressionClass> NegationClass::Clone() const {return std::make_shared<NegationClass>(*this); };
std::shared_ptr<ExpressionClass> NegationClass::Optimize()
{
  Operand = Operand->Optimize();
  return shared_from_this();
};
void              NegationClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
   int NodeNumber1 = NodeNumber++;
   s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> -|<f2> \"];" << endl;
   s << "\"Node" << MyNodeNumber << "\":f0 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
   Operand->DrawNode(s, NodeNumber1);
};


double            LogarithmClass::Evaluate() const {return log(Operand->Evaluate()); };
std::shared_ptr<ExpressionClass> LogarithmClass::Derive(VariableReferenceType TD) const {return std::make_shared<MultiplyClass>(std::make_shared<InverseClass>(Operand->Clone()), Operand->Derive(TD)); };
void              LogarithmClass::Print(std::ostream &s) const { s << "ln("; Operand->Print(s); s << ")";  };
std::shared_ptr<ExpressionClass> LogarithmClass::Clone() const {return std::make_shared<LogarithmClass>(*this); };
std::shared_ptr<ExpressionClass> LogarithmClass::Optimize()
{
  Operand = Operand->Optimize();
  return shared_from_this();
};
void              LogarithmClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
   int NodeNumber1 = NodeNumber++;
   s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> ln|<f2> \"];" << endl;
   s << "\"Node" << MyNodeNumber << "\":f0 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
   Operand->DrawNode(s, NodeNumber1);
};


double            ExponentialClass::Evaluate() const {return exp(Operand->Evaluate()); };
std::shared_ptr<ExpressionClass> ExponentialClass::Derive(VariableReferenceType TD) const {return std::make_shared<MultiplyClass>(std::make_shared<ExponentialClass>(*this), Operand->Derive(TD)); };
void              ExponentialClass::Print(std::ostream &s) const { s << "exp("; Operand->Print(s); s << ")";  };
std::shared_ptr<ExpressionClass> ExponentialClass::Clone() const {return std::make_shared<ExponentialClass>(*this); };
std::shared_ptr<ExpressionClass> ExponentialClass::Optimize()
{
  Operand = Operand->Optimize();

  if (Operand->IsConstant()) {
     //delete this; // Dangerous !!!
     return std::make_shared<ConstantClass>(exp(Operand->Evaluate()));
  }

  if (Operand->IsConstant()) {
     //delete this; // Dangerous !!!
     return std::make_shared<ConstantClass>(exp(Operand->Evaluate()));
  }

  if (typeid(*Operand) == typeid(LogarithmClass)) {
     return (std::dynamic_pointer_cast<LogarithmClass>(Operand))->Operand->Clone();
  }

  if (typeid(*Operand) == typeid(MultiplyClass)) {
     std::shared_ptr<MultiplyClass> op = std::dynamic_pointer_cast<MultiplyClass>(Operand);
     if (typeid(*(op->LeftOperand)) == typeid(LogarithmClass)) {
        return std::make_shared<PowerClass>( (std::dynamic_pointer_cast<LogarithmClass>(op->LeftOperand))->Operand->Clone(),
                                     op->RightOperand->Clone());
     }
     if (typeid(*(op->RightOperand)) == typeid(LogarithmClass)) {
        return std::make_shared<PowerClass>( (std::dynamic_pointer_cast<LogarithmClass>(op->RightOperand))->Operand->Clone(),
                                     op->LeftOperand->Clone());
     }
  }

  return shared_from_this();
};
void              ExponentialClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
   int NodeNumber1 = NodeNumber++;
   s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> exp|<f2> \"];" << endl;
   s << "\"Node" << MyNodeNumber << "\":f0 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
   Operand->DrawNode(s, NodeNumber1);
};


double            SquareRootClass::Evaluate() const {return sqrt(Operand->Evaluate()); };
std::shared_ptr<ExpressionClass> SquareRootClass::Derive(VariableReferenceType TD) const {return std::make_shared<MultiplyClass>(std::make_shared<InverseClass>(std::make_shared<MultiplyClass>(std::make_shared<ConstantClass>(2.0), std::make_shared<SquareRootClass>(*this))), Operand->Derive(TD)); };
void              SquareRootClass::Print(std::ostream &s) const { s << "sqrt("; Operand->Print(s); s << ")"; };
std::shared_ptr<ExpressionClass> SquareRootClass::Clone() const {return std::make_shared<SquareRootClass>(*this); };
std::shared_ptr<ExpressionClass> SquareRootClass::Optimize()
{
  Operand = Operand->Optimize();
  return shared_from_this();
};
void              SquareRootClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
   int NodeNumber1 = NodeNumber++;
   s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> sqrt|<f2> \"];" << endl;
   s << "\"Node" << MyNodeNumber << "\":f0 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
   Operand->DrawNode(s, NodeNumber1);
};

double            PowerClass::Evaluate() const {return pow(LeftOperand->Evaluate(), RightOperand->Evaluate()); };
std::shared_ptr<ExpressionClass> PowerClass::Derive(VariableReferenceType TD) const {return std::make_shared<MultiplyClass>(std::make_shared<ExponentialClass>(std::make_shared<MultiplyClass>(std::make_shared<LogarithmClass>(LeftOperand->Clone()),
                                                                                                                                   RightOperand->Clone()
                                                                                                                               )),
                                                                                        std::make_shared<AdditionClass>(std::make_shared<MultiplyClass>(std::make_shared<LogarithmClass>(LeftOperand->Clone()), RightOperand->Derive(TD)),
                                                                                                          std::make_shared<MultiplyClass>(std::make_shared<InverseClass>(LeftOperand->Clone()),
                                                                                                                            std::make_shared<MultiplyClass>(LeftOperand->Derive(TD), RightOperand->Clone())
                                                                                                                           )
                                                                                                          )
                                                                                        );}

void              PowerClass::Print(std::ostream &s) const { s << "("; LeftOperand->Print(s); s << ") ^ ("; RightOperand->Print(s); s << ")"; };
std::shared_ptr<ExpressionClass> PowerClass::Clone() const {return std::make_shared<PowerClass>(*this); };
std::shared_ptr<ExpressionClass> PowerClass::Optimize()
{
  LeftOperand  = LeftOperand->Optimize();
  RightOperand = RightOperand->Optimize();


  /* Convert trees */
#if 0
  /* make constant allways left */
  if (typeid(*RigthOperand) == typeid(MultiplyClass)) {
     std::shared_ptr<ExpressionClass>ep = LeftOperand;
     LeftOperand = RightOperand;
     RightOperand = ep;
  }
  if (typeid(*RightOperand) == typeid(MultiplyClass)){
     MultiplyClass *rop = std::dynamic_pointer_cast<MultiplyClass>(RightOperand);
     if (rop->LeftOperand->IsConstant()) {
        std::shared_ptr<ExpressionClass>ep = LeftOperand;
        LeftOperand = rop->LeftOperand;
        rop->LeftOperand = ep;
     }
  }

  if (typeid(*RightOperand) == typeid(MultiplyClass)){
     MultiplyClass *rop = std::dynamic_pointer_cast<MultiplyClass>(RightOperand);
     if (  rop->LeftOperand->IsConstant()
         &&LeftOperand->IsConstant()) {
        LeftOperand = std::make_shared<ConstantClass>(LeftOperand->Evaluate() *rop->LeftOperand->Evaluate());
        RightOperand = rop->RightOperand;
     }
  }
#endif
  bool LeftConst  = LeftOperand->IsConstant();
  bool RightConst = RightOperand->IsConstant();
  double ValLeft  = LeftOperand->Evaluate();
  double ValRight = RightOperand->Evaluate();

  if (LeftConst  && (ValLeft == 0.0)) {
     //delete this; // Dangerous !!!
     return std::make_shared<ConstantClass>(0.0);
  }

  if (RightConst  && (ValRight == 0.0)) {
     //delete this; // Dangerous !!!
     return std::make_shared<ConstantClass>(1.0);
  }

  if (RightConst  && (ValRight == 1.0)) {
     //delete this; // Dangerous !!!
     return RightOperand;
  }

  if (LeftConst && RightConst) {
     //delete this; // Dangerous !!!
     return std::make_shared<ConstantClass>(pow(ValLeft, ValRight));
  }
  return shared_from_this();
};

void              PowerClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
   int NodeNumber1 = NodeNumber++;
   int NodeNumber2 = NodeNumber++;
   s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> ^|<f2> \"];" << endl;
   s << "\"Node" << MyNodeNumber << "\":f0 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
   s << "\"Node" << MyNodeNumber << "\":f2 -> \"Node" << NodeNumber2 << "\":f1;" << endl;
   LeftOperand->DrawNode(s, NodeNumber1);
   RightOperand->DrawNode(s, NodeNumber2);
};



double            MultiplyClass::Evaluate() const {return LeftOperand->Evaluate() * RightOperand->Evaluate(); };
std::shared_ptr<ExpressionClass> MultiplyClass::Derive(VariableReferenceType TD) const {return std::make_shared<AdditionClass>(std::make_shared<MultiplyClass>(LeftOperand->Clone(), RightOperand->Derive(TD)), std::make_shared<MultiplyClass>(LeftOperand->Derive(TD), RightOperand->Clone())); };
void              MultiplyClass::Print(std::ostream &s) const { s << "("; LeftOperand->Print(s); s << ") * ("; RightOperand->Print(s); s << ")"; };
std::shared_ptr<ExpressionClass> MultiplyClass::Clone() const {return std::make_shared<MultiplyClass>(*this); };
std::shared_ptr<ExpressionClass> MultiplyClass::Optimize()
{
  LeftOperand  = LeftOperand->Optimize();
  RightOperand = RightOperand->Optimize();

  /* make constant allways left */
  if (RightOperand->IsConstant()) {
     std::shared_ptr<ExpressionClass>ep = LeftOperand;
     LeftOperand = RightOperand;
     RightOperand = ep;
  }

  /* Convert trees */
  if (   (typeid(*LeftOperand) == typeid(MultiplyClass))
      && (typeid(*RightOperand) == typeid(MultiplyClass))){
     std::shared_ptr<MultiplyClass> lop = std::dynamic_pointer_cast<MultiplyClass>(LeftOperand);
     std::shared_ptr<MultiplyClass> rop = std::dynamic_pointer_cast<MultiplyClass>(RightOperand);
     LeftOperand = lop->LeftOperand;
     lop->LeftOperand = lop->RightOperand;
     lop->RightOperand = rop;
     RightOperand = lop;
  }
  /* make constant allways left */
  if (typeid(*LeftOperand) == typeid(MultiplyClass)) {
     std::shared_ptr<ExpressionClass>ep = LeftOperand;
     LeftOperand = RightOperand;
     RightOperand = ep;
  }
  if (typeid(*RightOperand) == typeid(MultiplyClass)){
     std::shared_ptr<MultiplyClass> rop = std::dynamic_pointer_cast<MultiplyClass>(RightOperand);
     if (rop->LeftOperand->IsConstant()) {
        std::shared_ptr<ExpressionClass>ep = LeftOperand;
        LeftOperand = rop->LeftOperand;
        rop->LeftOperand = ep;
     }
  }

  if (typeid(*RightOperand) == typeid(MultiplyClass)){
     std::shared_ptr<MultiplyClass> rop = std::dynamic_pointer_cast<MultiplyClass>(RightOperand);
     if (  rop->LeftOperand->IsConstant()
         &&LeftOperand->IsConstant()) {
        LeftOperand = std::make_shared<ConstantClass>(LeftOperand->Evaluate() *rop->LeftOperand->Evaluate());
        RightOperand = rop->RightOperand;
     }
  }

  bool LeftConst  = LeftOperand->IsConstant();
  bool RightConst = RightOperand->IsConstant();
  double ValLeft  = LeftOperand->Evaluate();
  double ValRight = RightOperand->Evaluate();

  if (   (LeftConst  && (ValLeft == 0.0))
       ||(RightConst && (ValRight == 0.0))) {
     //delete this; // Dangerous !!!
     return std::make_shared<ConstantClass>(0.0);
  }
  if (LeftConst && RightConst) {
     //delete this; // Dangerous !!!
     return std::make_shared<ConstantClass>(ValLeft * ValRight);
  }
  if (LeftConst  && (ValLeft == 1.0)) {
     std::shared_ptr<ExpressionClass>ep = RightOperand;
     RightOperand = NULL;
     //delete this; // Dangerous !!!
     return ep;
  }
  if (RightConst && (ValRight == 1.0)) {
     std::shared_ptr<ExpressionClass>ep = LeftOperand;
     LeftOperand = NULL;
     //delete this; // Dangerous !!!
     return ep;
  }
  return shared_from_this();
};
void              MultiplyClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
   int NodeNumber1 = NodeNumber++;
   int NodeNumber2 = NodeNumber++;
   s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> *|<f2> \"];" << endl;
   s << "\"Node" << MyNodeNumber << "\":f0 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
   s << "\"Node" << MyNodeNumber << "\":f2 -> \"Node" << NodeNumber2 << "\":f1;" << endl;
   LeftOperand->DrawNode(s, NodeNumber1);
   RightOperand->DrawNode(s, NodeNumber2);
};


double            AdditionClass::Evaluate() const {return LeftOperand->Evaluate() + RightOperand->Evaluate(); };
std::shared_ptr<ExpressionClass> AdditionClass::Derive(VariableReferenceType TD) const {return std::make_shared<AdditionClass>(LeftOperand->Derive(TD), RightOperand->Derive(TD)); };
void              AdditionClass::Print(std::ostream &s) const { s << "("; LeftOperand->Print(s); s << ") + ("; RightOperand->Print(s); s << ")"; };
std::shared_ptr<ExpressionClass> AdditionClass::Clone() const {return std::make_shared<AdditionClass>(*this); };
std::shared_ptr<ExpressionClass> AdditionClass::Optimize()
{
  LeftOperand  = LeftOperand->Optimize();
  RightOperand = RightOperand->Optimize();

  bool LeftConst  = LeftOperand->IsConstant();
  bool RightConst = RightOperand->IsConstant();
  double ValLeft  = LeftOperand->Evaluate();
  double ValRight = RightOperand->Evaluate();

  if (LeftConst && RightConst) {
     //delete this; // Dangerous !!!
     return std::make_shared<ConstantClass>(ValLeft + ValRight);
  }
  if (LeftConst  && (ValLeft == 0.0)) {
     std::shared_ptr<ExpressionClass>ep = RightOperand;
     RightOperand = NULL;
     //delete this; // Dangerous !!!
     return ep;
  }
  if (RightConst && (ValRight == 0.0)) {
     std::shared_ptr<ExpressionClass>ep = LeftOperand;
     LeftOperand = NULL;
     //delete this; // Dangerous !!!
     return ep;
  }
  if (LeftOperand->IsSame(RightOperand)) {
     std::shared_ptr<ExpressionClass>ep = std::make_shared<MultiplyClass>(std::make_shared<ConstantClass>(2.0), RightOperand);
     RightOperand = NULL;
     //delete this; // Dangerous !!!
     return ep;
  }
  if (typeid(*LeftOperand) == typeid(MultiplyClass)) {
     std::shared_ptr<MultiplyClass> lop = std::dynamic_pointer_cast<MultiplyClass>(LeftOperand);
     if (typeid(*RightOperand) == typeid(MultiplyClass)) {
        std::shared_ptr<MultiplyClass> rop = std::dynamic_pointer_cast<MultiplyClass>(RightOperand);
        if (lop->RightOperand->IsSame(rop->RightOperand)) {
           std::shared_ptr<ExpressionClass>ep1 = std::make_shared<AdditionClass>(lop->LeftOperand, rop->LeftOperand);
           std::shared_ptr<ExpressionClass>ep2 = std::make_shared<MultiplyClass>(ep1, lop->RightOperand);
           return ep2;
        }
        if (lop->LeftOperand->IsSame(rop->LeftOperand)) {
           std::shared_ptr<ExpressionClass>ep1 = std::make_shared<AdditionClass>(lop->RightOperand, rop->RightOperand);
           std::shared_ptr<ExpressionClass>ep2 = std::make_shared<MultiplyClass>(ep1, lop->LeftOperand);
           return ep2;
        }
        if (lop->LeftOperand->IsSame(rop->RightOperand)) {
           std::shared_ptr<ExpressionClass>ep1 = std::make_shared<AdditionClass>(lop->RightOperand, rop->LeftOperand);
           std::shared_ptr<ExpressionClass>ep2 = std::make_shared<MultiplyClass>(ep1, lop->LeftOperand);
           return ep2;
        }
        if (lop->RightOperand->IsSame(rop->LeftOperand)) {
           std::shared_ptr<ExpressionClass>ep1 = std::make_shared<AdditionClass>(lop->LeftOperand, rop->RightOperand);
           std::shared_ptr<ExpressionClass>ep2 = std::make_shared<MultiplyClass>(ep1, lop->RightOperand);
           return ep2;
        }
     }
        if (lop->RightOperand->IsSame(RightOperand)) {
           std::shared_ptr<ExpressionClass>ep1 = std::make_shared<AdditionClass>(std::make_shared<ConstantClass>(1.0), lop->LeftOperand);
           std::shared_ptr<ExpressionClass>ep2 = std::make_shared<MultiplyClass>(ep1, RightOperand);
           return ep2;
        }
        if (lop->LeftOperand->IsSame(RightOperand)) {
           std::shared_ptr<ExpressionClass>ep1 = std::make_shared<AdditionClass>(std::make_shared<ConstantClass>(1.0), lop->RightOperand);
           std::shared_ptr<ExpressionClass>ep2 = std::make_shared<MultiplyClass>(ep1, RightOperand);
           return ep2;
        }

   }
     if (typeid(*RightOperand) == typeid(MultiplyClass)) {
        std::shared_ptr<MultiplyClass> rop = std::dynamic_pointer_cast<MultiplyClass>(RightOperand);
        if (rop->LeftOperand->IsSame(LeftOperand)) {
           std::shared_ptr<ExpressionClass>ep1 = std::make_shared<AdditionClass>(std::make_shared<ConstantClass>(1.0), rop->RightOperand);
           std::shared_ptr<ExpressionClass>ep2 = std::make_shared<MultiplyClass>(ep1, LeftOperand);
           return ep2;
        }
        if (rop->RightOperand->IsSame(LeftOperand)) {
           std::shared_ptr<ExpressionClass>ep1 = std::make_shared<AdditionClass>(std::make_shared<ConstantClass>(1.0), rop->LeftOperand);
           std::shared_ptr<ExpressionClass>ep2 = std::make_shared<MultiplyClass>(ep1, LeftOperand);
           return ep2;
        }

  }
  return shared_from_this();
};
void              AdditionClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
   int NodeNumber1 = NodeNumber++;
   int NodeNumber2 = NodeNumber++;
   s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> +|<f2> \"];" << endl;
   s << "\"Node" << MyNodeNumber << "\":f0 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
   s << "\"Node" << MyNodeNumber << "\":f2 -> \"Node" << NodeNumber2 << "\":f1;" << endl;
   LeftOperand->DrawNode(s, NodeNumber1);
   RightOperand->DrawNode(s, NodeNumber2);
};



bool UnaryOperationClass::IsSame(std::shared_ptr<ExpressionClass>Other)
{
   if (typeid(*this) == typeid(*Other)) {
      return Operand->IsSame(std::dynamic_pointer_cast<UnaryOperationClass>(Other)->Operand);
   }
   return false;
}

bool BinaryOperationClass::IsSame(std::shared_ptr<ExpressionClass>Other)
{
   if (typeid(*this) == typeid(*Other)) {
      return  ( LeftOperand->IsSame(std::dynamic_pointer_cast<BinaryOperationClass>(Other)->LeftOperand))
            &&( RightOperand->IsSame(std::dynamic_pointer_cast<BinaryOperationClass>(Other)->RightOperand));
   }
   return false;
}

bool ConstantClass::IsSame(std::shared_ptr<ExpressionClass>Other)
{
   if (typeid(*this) == typeid(*Other)) {
      return  ( Value == std::dynamic_pointer_cast<ConstantClass>(Other)->Value);
   }
   return false;
}
void              ConstantClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
   s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> " << Value << "|<f2> \"];" << endl;
};


bool VariableValueClass::IsSame(std::shared_ptr<ExpressionClass>Other)
{
   //cout << "Var";
   //cout << typeid(*this).name() << ":" << typeid(*Other).name();
   if (typeid(*this) == typeid(*Other)) {
      //cout << "sameid";
      return  ( Val == std::dynamic_pointer_cast<VariableValueClass>(Other)->Val);
   }
   return false;
}
void              VariableValueClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
   s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> " << Val->GetName() << "|<f2> \"];" << endl;
};



void StatementClass::Print(std::ostream &s) const
{
   std::cout << "\nVirtual Call StatementClass print()";
}

std::shared_ptr<StatementClass> StatementClass::Clone() const
{
    std::cout << "\nVirtual Call StatementClass Clone()";

}

std::shared_ptr<StatementClass> StatementClass::Optimize()
{
    std::cout << "\nVirtual Call StatementClass Optimize()";

}

void StatementClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
    std::cout << "\nVirtual Call StatementClass DrawNode()";

}

AssignementClass::~AssignementClass()
{

}

void AssignementClass::Print(std::ostream &s) const
{
    { s << Variable->GetName() << " = ";
    AssignedExpression->Print(s);
      s << ";" << std::endl; }
}

std::shared_ptr<StatementClass> AssignementClass::Clone() const
{
    return std::make_shared<AssignementClass>(*this);
}

std::shared_ptr<StatementClass> AssignementClass::Optimize()
{

}

void AssignementClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
    
}

bool ConditionalExpressionClass::Evaluate() const
{
    std::cout << "\nVirtual Call ConditionalExpressionClass Evaluate()";
}

void ConditionalExpressionClass::Print(std::ostream &s) const
{
    std::cout << "\nVirtual Call ConditionalExpressionClass print()";
}

std::shared_ptr<ConditionalExpressionClass> ConditionalExpressionClass::Clone() const
{
    std::cout << "\nVirtual Call ConditionalExpressionClass Clone()";
}

std::shared_ptr<ConditionalExpressionClass> ConditionalExpressionClass::Optimize()
{
    std::cout << "\nVirtual Call ConditionalExpressionClass Optimize()";
}

bool ConditionalExpressionClass::IsConstant()
{
    std::cout << "\nVirtual Call ConditionalExpressionClass IsConstant()";

}

bool ConditionalExpressionClass::IsSame(std::shared_ptr<ConditionalExpressionClass> Other)
{
    std::cout << "\nVirtual Call ConditionalExpressionClass IsSame()";

}

void ConditionalExpressionClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
    std::cout << "\nVirtual Call ConditionalExpressionClass DrawNode()";

}

bool BinaryConditionalOperationClass::IsSame(std::shared_ptr<ConditionalExpressionClass> Other)
{
    if (typeid(*this) == typeid(*Other)) {
        return  ( LeftOperand->IsSame(std::dynamic_pointer_cast<BinaryConditionalOperationClass>(Other)->LeftOperand))
        &&( RightOperand->IsSame(std::dynamic_pointer_cast<BinaryConditionalOperationClass>(Other)->RightOperand));
    }
    return false;
 }

bool AndClass::Evaluate() const
{
    return LeftOperand->Evaluate() && RightOperand->Evaluate();
}

void AndClass::Print(std::ostream &s) const
{
   s << "("; LeftOperand->Print(s); s << ") AND ("; RightOperand->Print(s); s << ")";
}

std::shared_ptr<ConditionalExpressionClass> AndClass::Clone() const
{
    return std::make_shared<AndClass>(*this);
}

std::shared_ptr<ConditionalExpressionClass> AndClass::Optimize()
{
    return shared_from_this();
}

void AndClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{

}

bool BinaryRelationalOperationClass::IsSame(std::shared_ptr<ConditionalExpressionClass> Other)
{
    if (typeid(*this) == typeid(*Other)) {
        return  ( LeftOperand->IsSame(std::dynamic_pointer_cast<BinaryRelationalOperationClass>(Other)->LeftOperand))
        &&( RightOperand->IsSame(std::dynamic_pointer_cast<BinaryRelationalOperationClass>(Other)->RightOperand));
    }
    return false;

}

bool LessThanClass::Evaluate() const
{
    return LeftOperand->Evaluate() < RightOperand->Evaluate();
}

void LessThanClass::Print(std::ostream &s) const
{
    s << "("; LeftOperand->Print(s); s << ") < ("; RightOperand->Print(s); s << ")";
}

std::shared_ptr<ConditionalExpressionClass> LessThanClass::Clone() const
{
    return std::make_shared<LessThanClass>(*this);
}

std::shared_ptr<ConditionalExpressionClass> LessThanClass::Optimize()
{
    return shared_from_this();
}

void LessThanClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{

}

void RepeatLoopClass::Print(std::ostream &s) const
{
    s << "repeat\n";
    for(auto &r: Statements) {
        r->Print(s);
    }
    s << "until (";
    Condition->Print(s); s << ");\n";
}

std::shared_ptr<StatementClass> RepeatLoopClass::Clone() const
{
    return std::make_shared<RepeatLoopClass>(*this);
}

std::shared_ptr<StatementClass> RepeatLoopClass::Optimize()
{
    return shared_from_this();
}

void RepeatLoopClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{

}

void FunctionClass::Print(std::ostream &s) const
{
    s << "function " << Name << "(";
    bool first = true;
    for(auto &r: Parameters) {
        if (first) {
            first = false;
        } else {
            std::cout << ", ";
        }
        std::cout << r->GetName();
    }
    s << ")" << std::endl;
    for(auto &r: Statements) {
        r->Print(s);
    }
    s << "endfunction" << std::endl;
}

std::shared_ptr<StatementClass> FunctionClass::Clone() const
{
    return std::make_shared<FunctionClass>(*this);
}

std::shared_ptr<StatementClass> FunctionClass::Optimize()
{
    return shared_from_this();
}

void FunctionClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
    
}
