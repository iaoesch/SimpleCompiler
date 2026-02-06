#include <iostream>
#include <cmath>
#include <typeinfo>
#include <sstream>
#include "compact.h"

// Helpervlass to suppress Warning on typeid on polymorphic shared/unique ptr
template <typename T> auto &Type_Id( T const& obj ) { return typeid( obj );}

static int NodeNumber = 1;

static ErrorEnvironment Errorenv;
static Environment *DefaultEnvironment = &Errorenv;
void SetNodeDefaultEnvironment(Environment &Env) {DefaultEnvironment = &Env;}

int GetNextNodeNumber()
{
    return NodeNumber++;
}

void RestartNodeNumber()
{
    NodeNumber = 1;
}

using std::endl;

static Environment DummyEnvironment;

static std::string EscapeStringForDot(const std::string& input)
{
    std::string output;
    output.reserve(input.size());
    for (const char c: input) {
        switch (c) {
        case '\a':  output += "\\a";        break;
        case '\b':  output += "\\b";        break;
        case '\f':  output += "\\f";        break;
        case '\n':  output += "\\n";        break;
        case '\r':  output += "\\r";        break;
        case '\t':  output += "\\t";        break;
        case '\v':  output += "\\v";        break;
        case '\\':  output += "\\\\";        break;
        case '|':   output += "\\|";        break;
        case '>':   output += "\\>";        break;
        case '<':   output += "\\<";        break;
        case '"':   output += "\\\"";        break;
        default:    output += c;            break;
        }
    }

    return output;
}


Variables::VariableContentClass ExpressionClass::Evaluate() const
{
    return Evaluate(DummyEnvironment);
}

Variables::VariableContentClass            ExpressionClass::Evaluate(Environment &Env) const { (void)Env; Env.DebugOutput() << "\nVirtual Call expression Evaluate(Env)"; return 0.0;};
std::shared_ptr<ExpressionClass> ExpressionClass::Derive(VariableReferenceType ToDerive) const { (void)ToDerive; DefaultEnvironment->DebugOutput() << "\nVirtual Call expression Derive()"; return NULL;};
void              ExpressionClass::Print(std::ostream &s) const { (void)s; DefaultEnvironment->DebugOutput() << "\nVirtual Call expression print()"; };
std::shared_ptr<ExpressionClass> ExpressionClass::Clone() const {DefaultEnvironment->DebugOutput() << "\nVirtual Call expression Clone()"; return NULL;};
std::shared_ptr<ExpressionClass> ExpressionClass::Optimize(Environment &Env) { (void)Env; Env.DebugOutput() << "\nVirtual Call expression Optimize(Environment &Env)"; return NULL;};
bool              ExpressionClass::IsConstant() {DefaultEnvironment->DebugOutput() << "\nVirtual Call expression IsConstant()"; return false;};
bool              ExpressionClass::IsSame(std::shared_ptr<ExpressionClass>Other) { (void)Other; DefaultEnvironment->DebugOutput() << "\nVirtual Call expression IsSame()"; return false;};
void              ExpressionClass::DrawNode(std::ostream &s, int MyNodeNumber) const { (void)MyNodeNumber;  (void)s; DefaultEnvironment->DebugOutput() << "\nVirtual Call expression DrawNode()";};




Variables::VariableContentClass            InverseClass::Evaluate(Environment &Env) const {return (1LL / Operand->Evaluate(Env)); };
std::shared_ptr<ExpressionClass> InverseClass::Derive(VariableReferenceType TD) const {return std::make_shared<NegationClass>(std::make_shared<MultiplyClass>(std::make_shared<InverseClass>( std::make_shared<SquareClass>(Operand->Clone(), GetLocation()), GetLocation()), Operand->Derive(TD), GetLocation()), GetLocation()); };
void              InverseClass::Print(std::ostream &s) const { s << "1.0 / ("; Operand->Print(s); s << ")";  };
std::shared_ptr<ExpressionClass> InverseClass::Clone() const {return std::make_shared<InverseClass>(*this); };
std::shared_ptr<ExpressionClass> InverseClass::Optimize(Environment &Env)
{
  Operand = Operand->Optimize(Env);
  return shared_from_this();
};

void              InverseClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
   int NodeNumber1 = NodeNumber++;
   s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> 1/x |<f2> \"];" << endl;
   s << "\"Node" << MyNodeNumber << "\":f0 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
   Operand->DrawNode(s, NodeNumber1);
};



Variables::VariableContentClass            SquareClass::Evaluate(Environment &Env) const {Variables::VariableContentClass tmp = Operand->Evaluate(Env); return tmp*tmp; };
std::shared_ptr<ExpressionClass> SquareClass::Derive(VariableReferenceType TD) const {return std::make_shared<MultiplyClass>(std::make_shared<MultiplyClass>(std::make_shared<ConstantClass>(2.0, GetLocation()), Operand->Clone(), GetLocation()), Operand->Derive(TD), GetLocation()); };
void              SquareClass::Print(std::ostream &s) const { s << "("; Operand->Print(s); s << ")^2.0";  };
std::shared_ptr<ExpressionClass> SquareClass::Clone() const {return std::make_shared<SquareClass>(*this); };
std::shared_ptr<ExpressionClass> SquareClass::Optimize(Environment &Env)
{
  Operand = Operand->Optimize(Env);
  return shared_from_this();
};
void              SquareClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
   int NodeNumber1 = NodeNumber++;
   s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> x^2|<f2> \"];" << endl;
   s << "\"Node" << MyNodeNumber << "\":f0 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
   Operand->DrawNode(s, NodeNumber1);
};


Variables::VariableContentClass            NegationClass::Evaluate(Environment &Env) const {return - Operand->Evaluate(Env); };
std::shared_ptr<ExpressionClass> NegationClass::Derive(VariableReferenceType TD) const {return std::make_shared<NegationClass>(Operand->Derive(TD), GetLocation()); };
void              NegationClass::Print(std::ostream &s) const { s << "-("; Operand->Print(s); s << ")";  };
std::shared_ptr<ExpressionClass> NegationClass::Clone() const {return std::make_shared<NegationClass>(*this); };
std::shared_ptr<ExpressionClass> NegationClass::Optimize(Environment &Env)
{
  Operand = Operand->Optimize(Env);
  return shared_from_this();
};
void              NegationClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
   int NodeNumber1 = NodeNumber++;
   s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> -|<f2> \"];" << endl;
   s << "\"Node" << MyNodeNumber << "\":f0 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
   Operand->DrawNode(s, NodeNumber1);
};


Variables::VariableContentClass            LogarithmClass::Evaluate(Environment &Env) const {return log(Operand->Evaluate(Env)); };
std::shared_ptr<ExpressionClass> LogarithmClass::Derive(VariableReferenceType TD) const {return std::make_shared<MultiplyClass>(std::make_shared<InverseClass>(Operand->Clone(), GetLocation()), Operand->Derive(TD), GetLocation()); };
void              LogarithmClass::Print(std::ostream &s) const { s << "ln("; Operand->Print(s); s << ")";  };
std::shared_ptr<ExpressionClass> LogarithmClass::Clone() const {return std::make_shared<LogarithmClass>(*this); };
std::shared_ptr<ExpressionClass> LogarithmClass::Optimize(Environment &Env)
{
  Operand = Operand->Optimize(Env);
  return shared_from_this();
};
void              LogarithmClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
   int NodeNumber1 = NodeNumber++;
   s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> ln|<f2> \"];" << endl;
   s << "\"Node" << MyNodeNumber << "\":f0 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
   Operand->DrawNode(s, NodeNumber1);
};


Variables::VariableContentClass            ExponentialClass::Evaluate(Environment &Env) const {return exp(Operand->Evaluate(Env)); };
std::shared_ptr<ExpressionClass> ExponentialClass::Derive(VariableReferenceType TD) const {return std::make_shared<MultiplyClass>(std::make_shared<ExponentialClass>(*this), Operand->Derive(TD), GetLocation()); };
void              ExponentialClass::Print(std::ostream &s) const { s << "exp("; Operand->Print(s); s << ")";  };
std::shared_ptr<ExpressionClass> ExponentialClass::Clone() const {return std::make_shared<ExponentialClass>(*this); };
std::shared_ptr<ExpressionClass> ExponentialClass::Optimize(Environment &Env)
{
  Operand = Operand->Optimize(Env);

  if (Operand->IsConstant()) {
     //delete this; // Dangerous !!!
     return std::make_shared<ConstantClass>(exp(Operand->Evaluate(Env)), GetLocation());
  }

  if (Operand->IsConstant()) {
     //delete this; // Dangerous !!!
     return std::make_shared<ConstantClass>(exp(Operand->Evaluate(Env)), GetLocation());
  }

  if (Type_Id(*Operand) == typeid(LogarithmClass)) {
     return (std::dynamic_pointer_cast<LogarithmClass>(Operand))->Operand->Clone();
  }

  if (Type_Id(*Operand) == typeid(MultiplyClass)) {
     std::shared_ptr<MultiplyClass> op = std::dynamic_pointer_cast<MultiplyClass>(Operand);
     if (Type_Id(*(op->LeftOperand)) == typeid(LogarithmClass)) {
        return std::make_shared<PowerClass>( (std::dynamic_pointer_cast<LogarithmClass>(op->LeftOperand))->Operand->Clone(),
                                     op->RightOperand->Clone(), GetLocation());
     }
     if (Type_Id(*(op->RightOperand)) == typeid(LogarithmClass)) {
        return std::make_shared<PowerClass>( (std::dynamic_pointer_cast<LogarithmClass>(op->RightOperand))->Operand->Clone(),
                                     op->LeftOperand->Clone(), GetLocation());
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


Variables::VariableContentClass            SquareRootClass::Evaluate(Environment &Env) const {return sqrt(Operand->Evaluate(Env)); };
std::shared_ptr<ExpressionClass> SquareRootClass::Derive(VariableReferenceType TD) const {return std::make_shared<MultiplyClass>(std::make_shared<InverseClass>(std::make_shared<MultiplyClass>(std::make_shared<ConstantClass>(2.0, GetLocation()), std::make_shared<SquareRootClass>(*this), GetLocation()), GetLocation()), Operand->Derive(TD), GetLocation()); };
void              SquareRootClass::Print(std::ostream &s) const { s << "sqrt("; Operand->Print(s); s << ")"; };
std::shared_ptr<ExpressionClass> SquareRootClass::Clone() const {return std::make_shared<SquareRootClass>(*this); };
std::shared_ptr<ExpressionClass> SquareRootClass::Optimize(Environment &Env)
{
  Operand = Operand->Optimize(Env);
  return shared_from_this();
};
void              SquareRootClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
   int NodeNumber1 = NodeNumber++;
   s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> sqrt|<f2> \"];" << endl;
   s << "\"Node" << MyNodeNumber << "\":f0 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
   Operand->DrawNode(s, NodeNumber1);
};

Variables::VariableContentClass            PowerClass::Evaluate(Environment &Env) const {return pow(LeftOperand->Evaluate(Env), RightOperand->Evaluate(Env)); };
std::shared_ptr<ExpressionClass> PowerClass::Derive(VariableReferenceType TD) const {return std::make_shared<MultiplyClass>(std::make_shared<ExponentialClass>(std::make_shared<MultiplyClass>(std::make_shared<LogarithmClass>(LeftOperand->Clone(), GetLocation()),
                                                                                                                                   RightOperand->Clone()
                                                                                                                               , GetLocation()), GetLocation()),
                                                                                        std::make_shared<AdditionClass>(std::make_shared<MultiplyClass>(std::make_shared<LogarithmClass>(LeftOperand->Clone(), GetLocation()), RightOperand->Derive(TD), GetLocation()),
                                                                                                          std::make_shared<MultiplyClass>(std::make_shared<InverseClass>(LeftOperand->Clone(), GetLocation()),
                                                                                                                            std::make_shared<MultiplyClass>(LeftOperand->Derive(TD), RightOperand->Clone(), GetLocation())
                                                                                                                           , GetLocation())
                                                                                                          , GetLocation())
                                                                                        , GetLocation());}

void              PowerClass::Print(std::ostream &s) const { s << "("; LeftOperand->Print(s); s << ") ^ ("; RightOperand->Print(s); s << ")"; };
std::shared_ptr<ExpressionClass> PowerClass::Clone() const {return std::make_shared<PowerClass>(*this); };
std::shared_ptr<ExpressionClass> PowerClass::Optimize(Environment &Env)
{
  LeftOperand  = LeftOperand->Optimize(Env);
  RightOperand = RightOperand->Optimize(Env);


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
        LeftOperand = std::make_shared<ConstantClass>(LeftOperand->Evaluate(Env) *rop->LeftOperand->Evaluate(Env));
        RightOperand = rop->RightOperand;
     }
  }
#endif
  bool LeftConst  = LeftOperand->IsConstant();
  bool RightConst = RightOperand->IsConstant();
  Variables::VariableContentClass ValLeft  = LeftOperand->Evaluate(Env);
  Variables::VariableContentClass ValRight = RightOperand->Evaluate(Env);

  if (LeftConst  && (ValLeft == 0.0)) {
     //delete this; // Dangerous !!!
     return std::make_shared<ConstantClass>(0.0, GetMergedLocation());
  }

  if (RightConst  && (ValRight == 0.0)) {
     //delete this; // Dangerous !!!
     return std::make_shared<ConstantClass>(1.0, GetMergedLocation());
  }

  if (RightConst  && (ValRight == 1.0)) {
     //delete this; // Dangerous !!!
     return RightOperand;
  }

  if (LeftConst && RightConst) {
     //delete this; // Dangerous !!!
     return std::make_shared<ConstantClass>(pow(ValLeft, ValRight), GetMergedLocation());
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



Variables::VariableContentClass            MultiplyClass::Evaluate(Environment &Env) const {return LeftOperand->Evaluate(Env) * RightOperand->Evaluate(Env); };
std::shared_ptr<ExpressionClass> MultiplyClass::Derive(VariableReferenceType TD) const {return std::make_shared<AdditionClass>(std::make_shared<MultiplyClass>(LeftOperand->Clone(), RightOperand->Derive(TD), GetLocation()), std::make_shared<MultiplyClass>(LeftOperand->Derive(TD), RightOperand->Clone(), GetLocation()), GetLocation()); };
void              MultiplyClass::Print(std::ostream &s) const { s << "("; LeftOperand->Print(s); s << ") * ("; RightOperand->Print(s); s << ")"; };
std::shared_ptr<ExpressionClass> MultiplyClass::Clone() const {return std::make_shared<MultiplyClass>(*this); };
std::shared_ptr<ExpressionClass> MultiplyClass::Optimize(Environment &Env)
{
  LeftOperand  = LeftOperand->Optimize(Env);
  RightOperand = RightOperand->Optimize(Env);

  /* make constant allways left */
  if (RightOperand->IsConstant()) {
     std::shared_ptr<ExpressionClass>ep = LeftOperand;
     LeftOperand = RightOperand;
     RightOperand = ep;
  }

  /* Convert trees */
  if (   (Type_Id(*LeftOperand) == typeid(MultiplyClass))
      && (Type_Id(*RightOperand) == typeid(MultiplyClass))){
     std::shared_ptr<MultiplyClass> lop = std::dynamic_pointer_cast<MultiplyClass>(LeftOperand);
     std::shared_ptr<MultiplyClass> rop = std::dynamic_pointer_cast<MultiplyClass>(RightOperand);
     LeftOperand = lop->LeftOperand;
     lop->LeftOperand = lop->RightOperand;
     lop->RightOperand = rop;
     RightOperand = lop;
  }
  /* make constant allways left */
  if (Type_Id(*LeftOperand) == typeid(MultiplyClass)) {
     std::shared_ptr<ExpressionClass>ep = LeftOperand;
     LeftOperand = RightOperand;
     RightOperand = ep;
  }
  if (Type_Id(*RightOperand) == typeid(MultiplyClass)){
     std::shared_ptr<MultiplyClass> rop = std::dynamic_pointer_cast<MultiplyClass>(RightOperand);
     if (rop->LeftOperand->IsConstant()) {
        std::shared_ptr<ExpressionClass>ep = LeftOperand;
        LeftOperand = rop->LeftOperand;
        rop->LeftOperand = ep;
     }
  }

  if (Type_Id(*RightOperand) == typeid(MultiplyClass)){
     std::shared_ptr<MultiplyClass> rop = std::dynamic_pointer_cast<MultiplyClass>(RightOperand);
     if (  rop->LeftOperand->IsConstant()
         &&LeftOperand->IsConstant()) {
        LeftOperand = std::make_shared<ConstantClass>(LeftOperand->Evaluate(Env) * rop->LeftOperand->Evaluate(Env), LeftOperand->GetMergedLocation() | rop->LeftOperand->GetMergedLocation() );
        RightOperand = rop->RightOperand;
     }
  }

  bool LeftConst  = LeftOperand->IsConstant();
  bool RightConst = RightOperand->IsConstant();
  Variables::VariableContentClass ValLeft = LeftOperand->Evaluate(Env);
  Variables::VariableContentClass ValRight = RightOperand->Evaluate(Env);

  if (   (LeftConst  && (ValLeft == 0.0))
       ||(RightConst && (ValRight == 0.0))) {
     //delete this; // Dangerous !!!
     return std::make_shared<ConstantClass>(0.0, GetMergedLocation());
  }
  if (LeftConst && RightConst) {
     //delete this; // Dangerous !!!
     return std::make_shared<ConstantClass>(ValLeft * ValRight, GetMergedLocation());
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


Variables::VariableContentClass            AdditionClass::Evaluate(Environment &Env) const {return LeftOperand->Evaluate(Env) + RightOperand->Evaluate(Env); };
std::shared_ptr<ExpressionClass> AdditionClass::Derive(VariableReferenceType TD) const {return std::make_shared<AdditionClass>(LeftOperand->Derive(TD), RightOperand->Derive(TD), GetLocation()); };
void              AdditionClass::Print(std::ostream &s) const { s << "("; LeftOperand->Print(s); s << ") + ("; RightOperand->Print(s); s << ")"; };
std::shared_ptr<ExpressionClass> AdditionClass::Clone() const {return std::make_shared<AdditionClass>(*this); };
std::shared_ptr<ExpressionClass> AdditionClass::Optimize(Environment &Env)
{
  LeftOperand  = LeftOperand->Optimize(Env);
  RightOperand = RightOperand->Optimize(Env);

  bool LeftConst  = LeftOperand->IsConstant();
  bool RightConst = RightOperand->IsConstant();
  Variables::VariableContentClass ValLeft  = LeftOperand->Evaluate(Env);
  Variables::VariableContentClass ValRight = RightOperand->Evaluate(Env);

  if (LeftConst && RightConst) {
     //delete this; // Dangerous !!!
     return std::make_shared<ConstantClass>(ValLeft + ValRight, GetMergedLocation());
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
     std::shared_ptr<ExpressionClass>ep = std::make_shared<MultiplyClass>(std::make_shared<ConstantClass>(2.0, GetLocation()), RightOperand, GetLocation());
     RightOperand = NULL;
     //delete this; // Dangerous !!!
     return ep;
  }
  if (Type_Id(*LeftOperand) == typeid(MultiplyClass)) {
     std::shared_ptr<MultiplyClass> lop = std::dynamic_pointer_cast<MultiplyClass>(LeftOperand);
     if (Type_Id(*RightOperand) == typeid(MultiplyClass)) {
        std::shared_ptr<MultiplyClass> rop = std::dynamic_pointer_cast<MultiplyClass>(RightOperand);
        if (lop->RightOperand->IsSame(rop->RightOperand)) {
           std::shared_ptr<ExpressionClass>ep1 = std::make_shared<AdditionClass>(lop->LeftOperand, rop->LeftOperand, GetLocation());
           std::shared_ptr<ExpressionClass>ep2 = std::make_shared<MultiplyClass>(ep1, lop->RightOperand, GetLocation());
           return ep2;
        }
        if (lop->LeftOperand->IsSame(rop->LeftOperand)) {
           std::shared_ptr<ExpressionClass>ep1 = std::make_shared<AdditionClass>(lop->RightOperand, rop->RightOperand, GetLocation());
           std::shared_ptr<ExpressionClass>ep2 = std::make_shared<MultiplyClass>(ep1, lop->LeftOperand, GetLocation());
           return ep2;
        }
        if (lop->LeftOperand->IsSame(rop->RightOperand)) {
           std::shared_ptr<ExpressionClass>ep1 = std::make_shared<AdditionClass>(lop->RightOperand, rop->LeftOperand, GetLocation());
           std::shared_ptr<ExpressionClass>ep2 = std::make_shared<MultiplyClass>(ep1, lop->LeftOperand, GetLocation());
           return ep2;
        }
        if (lop->RightOperand->IsSame(rop->LeftOperand)) {
           std::shared_ptr<ExpressionClass>ep1 = std::make_shared<AdditionClass>(lop->LeftOperand, rop->RightOperand, GetLocation());
           std::shared_ptr<ExpressionClass>ep2 = std::make_shared<MultiplyClass>(ep1, lop->RightOperand, GetLocation());
           return ep2;
        }
     }
        if (lop->RightOperand->IsSame(RightOperand)) {
           std::shared_ptr<ExpressionClass>ep1 = std::make_shared<AdditionClass>(std::make_shared<ConstantClass>(1.0, GetLocation()), lop->LeftOperand, GetLocation());
           std::shared_ptr<ExpressionClass>ep2 = std::make_shared<MultiplyClass>(ep1, RightOperand, GetLocation());
           return ep2;
        }
        if (lop->LeftOperand->IsSame(RightOperand)) {
           std::shared_ptr<ExpressionClass>ep1 = std::make_shared<AdditionClass>(std::make_shared<ConstantClass>(1.0, GetLocation()), lop->RightOperand, GetLocation());
           std::shared_ptr<ExpressionClass>ep2 = std::make_shared<MultiplyClass>(ep1, RightOperand, GetLocation());
           return ep2;
        }

   }
     if (Type_Id(*RightOperand) == typeid(MultiplyClass)) {
        std::shared_ptr<MultiplyClass> rop = std::dynamic_pointer_cast<MultiplyClass>(RightOperand);
        if (rop->LeftOperand->IsSame(LeftOperand)) {
           std::shared_ptr<ExpressionClass>ep1 = std::make_shared<AdditionClass>(std::make_shared<ConstantClass>(1.0, GetLocation()), rop->RightOperand, GetLocation());
           std::shared_ptr<ExpressionClass>ep2 = std::make_shared<MultiplyClass>(ep1, LeftOperand, GetLocation());
           return ep2;
        }
        if (rop->RightOperand->IsSame(LeftOperand)) {
           std::shared_ptr<ExpressionClass>ep1 = std::make_shared<AdditionClass>(std::make_shared<ConstantClass>(1.0, GetLocation()), rop->LeftOperand, GetLocation());
           std::shared_ptr<ExpressionClass>ep2 = std::make_shared<MultiplyClass>(ep1, LeftOperand, GetLocation());
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



Variables::VariableContentClass  ExtractionClass::Evaluate(Environment &Env) const {return Operand->Evaluate(Env); };
std::shared_ptr<ExpressionClass> ExtractionClass::Derive(VariableReferenceType TD) const {return std::make_shared<AdditionClass>(Operand->Derive(TD), Operand->Derive(TD), GetLocation()); };
void              ExtractionClass::Print(std::ostream &s) const { s << "("; Operand->Print(s); s << ") =>> "; };
std::shared_ptr<ExpressionClass> ExtractionClass::Clone() const {return std::make_shared<ExtractionClass>(*this); };
std::shared_ptr<ExpressionClass> ExtractionClass::Optimize(Environment &Env)
{
    return shared_from_this();
};
void              ExtractionClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
    int NodeNumber1 = NodeNumber++;
    s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> =>>|<f2> \"];" << endl;
    s << "\"Node" << MyNodeNumber << "\":f0 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
    Operand->DrawNode(s, NodeNumber1);
};

Variables::VariableContentClass  CompositionClass::Evaluate(Environment &Env) const {return LeftOperand->Evaluate(Env) + RightOperand->Evaluate(Env); };
std::shared_ptr<ExpressionClass> CompositionClass::Derive(VariableReferenceType TD) const {return std::make_shared<AdditionClass>(LeftOperand->Derive(TD), RightOperand->Derive(TD), GetLocation()); };
void              CompositionClass::Print(std::ostream &s) const { s << "("; LeftOperand->Print(s); s << ") <<= ("; RightOperand->Print(s); s << ")"; };
std::shared_ptr<ExpressionClass> CompositionClass::Clone() const {return std::make_shared<CompositionClass>(*this); };
std::shared_ptr<ExpressionClass> CompositionClass::Optimize(Environment &Env)
{
    return shared_from_this();
};
void              CompositionClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
    int NodeNumber1 = NodeNumber++;
    int NodeNumber2 = NodeNumber++;
    s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> <<=|<f2> \"];" << endl;
    s << "\"Node" << MyNodeNumber << "\":f0 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
    s << "\"Node" << MyNodeNumber << "\":f2 -> \"Node" << NodeNumber2 << "\":f1;" << endl;
    LeftOperand->DrawNode(s, NodeNumber1);
    RightOperand->DrawNode(s, NodeNumber2);
};

bool UnaryOperationClass::IsSame(std::shared_ptr<ExpressionClass>Other)
{
   if (Type_Id(*this) == Type_Id(*Other)) {
      return Operand->IsSame(std::dynamic_pointer_cast<UnaryOperationClass>(Other)->Operand);
   }
   return false;
}

bool BinaryOperationClass::IsSame(std::shared_ptr<ExpressionClass>Other)
{
   if (Type_Id(*this) == Type_Id(*Other)) {
      return  ( LeftOperand->IsSame(std::dynamic_pointer_cast<BinaryOperationClass>(Other)->LeftOperand))
            &&( RightOperand->IsSame(std::dynamic_pointer_cast<BinaryOperationClass>(Other)->RightOperand));
   }
   return false;
}

const TypeDescriptorClass BinaryOperationClass::GetType() const
{
    return CommonType(LeftOperand->Type(), RightOperand->Type());
}

bool ConstantClass::IsSame(std::shared_ptr<ExpressionClass>Other)
{
   if (Type_Id(*this) == Type_Id(*Other)) {
      return  ( Value == std::dynamic_pointer_cast<ConstantClass>(Other)->Value);
   }
   return false;
}
void ConstantClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
    std::ostringstream UnescapedValue;
    UnescapedValue << Value;
    std::string EscapedValue = EscapeStringForDot(UnescapedValue.str());
   s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> " << EscapedValue << "|<f2> \"];" << endl;
}

const TypeDescriptorClass ConstantClass::GetType() const
{
    return Value.getType();
};


bool VariableValueClass::IsSame(std::shared_ptr<ExpressionClass>Other)
{
   //cout << "Var";
   //cout << typeid(*this).name() << ":" << typeid(*Other).name();
   if (Type_Id(*this) == Type_Id(*Other)) {
      //cout << "sameid";
      return  ( Val == std::dynamic_pointer_cast<VariableValueClass>(Other)->Val);
   }
   return false;
}
void              VariableValueClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
   s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> " << Val->GetName() << "|<f2> \"];" << endl;
}

VariableReferenceType VariableValueClass::GetWriteReferenceToContent(ModeType Mode)
{
    (void) Mode; // Variable allways exist...
    return Val;
}

const TypeDescriptorClass VariableValueClass::GetType() const
{
    return Val->Type();
};



void StatementClass::Print(std::ostream &s[[maybe_unused]]) const
{
   DefaultEnvironment->DebugOutput() << "\nVirtual Call StatementClass print()";
}

std::shared_ptr<StatementClass> StatementClass::Clone() const
{
    DefaultEnvironment->DebugOutput() << "\nVirtual Call StatementClass Clone()";
    return nullptr;
}

std::shared_ptr<StatementClass> StatementClass::Optimize(Environment &Env)
{
    (void)Env;
    Env.DebugOutput() << "\nVirtual Call StatementClass Optimize(Environment &Env)";
    return nullptr;
}

void StatementClass::DrawNode(std::ostream &s[[maybe_unused]], int MyNodeNumber[[maybe_unused]]) const
{
    DefaultEnvironment->DebugOutput() << "\nVirtual Call StatementClass DrawNode()";

}

StatementResultClass StatementClass::Execute(Environment &Env[[maybe_unused]]) const
{
    Env.DebugOutput() << "\nVirtual Call StatementClass Execute()";
    return{};
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

std::shared_ptr<StatementClass> AssignementClass::Optimize(Environment &Env)
{
    (void)Env;
    return shared_from_this();
}

void AssignementClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
    int NodeNumber1 = NodeNumber++;
    int NodeNumber2 = NodeNumber++;
    s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> := |<f2> \"];" << endl;
    s << "\"Node" << MyNodeNumber << "\":f0 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
    s << "\"Node" << MyNodeNumber << "\":f2 -> \"Node" << NodeNumber2 << "\":f1;" << endl;
    AssignedExpression->DrawNode(s, NodeNumber2);
    Variable->DrawNode(s, NodeNumber1);

}

StatementResultClass AssignementClass::Execute(Environment &Env) const
{
    Env.Tracing(GetLocation(), "Assign(...)");
    VariableReferenceType ReferedVariable = Variable->GetWriteReferenceToContent(WritableValueClass::IfNotExistCreateIfPossible);
    if (ReferedVariable == nullptr) {
        throw SyntaxErrorClass("Assign: Could not get write reference to '" + Variable->GetName() + "'");
    }
    try {
        Variables::VariableContentClass Result = AssignedExpression->Evaluate(Env);
        Env.DebugOutput() << "AsgExe:" << Result;
        if (Result.Isempty()) {
            ReferedVariable->SetValue(Variables::VariableContentClass(AssignedExpression));
        } else {
            ReferedVariable->SetValue(Result);
        }
    }
    catch (RuntimeErrorClass &e) {
        std::stringstream s;
        s << "while assigning at [" << GetLocation() << "]";
        e.ExtendMessage(s.str(), GetLocation().begin.line);
        throw e;
    }
    return {};
}

ReturningStatementClass::~ReturningStatementClass()
{

}

void ReturningStatementClass::Print(std::ostream &s) const
{
    { s << "Returning ";
        ReturnedExpression->Print(s);
        s << ";" << std::endl; }
}

std::shared_ptr<StatementClass> ReturningStatementClass::Clone() const
{
    return std::make_shared<ReturningStatementClass>(*this);
}

std::shared_ptr<StatementClass> ReturningStatementClass::Optimize(Environment &Env)
{
    (void)Env;
    return shared_from_this();
}

void ReturningStatementClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
    int NodeNumber2 = NodeNumber++;
    s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> Returning |<f2> \"];" << endl;
    s << "\"Node" << MyNodeNumber << "\":f2 -> \"Node" << NodeNumber2 << "\":f1;" << endl;
    ReturnedExpression->DrawNode(s, NodeNumber2);

}

StatementResultClass ReturningStatementClass::Execute(Environment &Env) const
{
    Env.Tracing(GetLocation(), "Returning(...)");
    //VariableReferenceType ReferedVariable = Variable->GetWriteReferenceToContent(WritableValueClass::IfNotExistCreateIfPossible);
    try {
        Variables::VariableContentClass Result = ReturnedExpression->Evaluate(Env);
        Env.DebugOutput() << "retExe:" << Result;
        if (Result.Isempty()) {
           // ReferedVariable->SetValue(Variables::VariableContentClass(AssignedExpression));
            return std::make_shared<Variables::VariableContentClass>(ReturnedExpression);

        } else {
            //ReferedVariable->SetValue(Result);
            return std::make_shared<Variables::VariableContentClass>(Result);
        }
    }
    catch (RuntimeErrorClass &e) {
        std::stringstream s;
        s << "at [" << GetLocation() << "]";
        e.ExtendMessage(s.str(), GetLocation().begin.line);
        throw e;
    }
}

bool ConditionalExpressionClass::Evaluate(Environment &Env) const
{
    (void)Env;
    Env.DebugOutput() << "\nVirtual Call ConditionalExpressionClass Evaluate(Env)";
    return false;
}

void ConditionalExpressionClass::Print(std::ostream &s[[maybe_unused]]) const
{
    DefaultEnvironment->DebugOutput() << "\nVirtual Call ConditionalExpressionClass print()";
}

std::shared_ptr<ConditionalExpressionClass> ConditionalExpressionClass::Clone() const
{
    DefaultEnvironment->DebugOutput() << "\nVirtual Call ConditionalExpressionClass Clone()";
    return nullptr;
}

std::shared_ptr<ConditionalExpressionClass> ConditionalExpressionClass::Optimize(Environment &Env)
{
    (void)Env;
    Env.DebugOutput() << "\nVirtual Call ConditionalExpressionClass Optimize(Environment &Env)";
    return nullptr;
}

bool ConditionalExpressionClass::IsConstant()
{
    DefaultEnvironment->DebugOutput() << "\nVirtual Call ConditionalExpressionClass IsConstant()";
    return false;
}

bool ConditionalExpressionClass::IsSame(std::shared_ptr<ConditionalExpressionClass> Other[[maybe_unused]])
{
    DefaultEnvironment->DebugOutput() << "\nVirtual Call ConditionalExpressionClass IsSame()";
    return false;
}

void ConditionalExpressionClass::DrawNode(std::ostream &s[[maybe_unused]], int MyNodeNumber[[maybe_unused]]) const
{
    DefaultEnvironment->DebugOutput() << "\nVirtual Call ConditionalExpressionClass DrawNode()";

}

bool BinaryConditionalOperationClass::IsSame(std::shared_ptr<ConditionalExpressionClass> Other)
{
    if (Type_Id(*this) == Type_Id(*Other)) {
        return  ( LeftOperand->IsSame(std::dynamic_pointer_cast<BinaryConditionalOperationClass>(Other)->LeftOperand))
        &&( RightOperand->IsSame(std::dynamic_pointer_cast<BinaryConditionalOperationClass>(Other)->RightOperand));
    }
    return false;
 }

bool AndClass::Evaluate(Environment &Env) const
{
    return LeftOperand->Evaluate(Env) && RightOperand->Evaluate(Env);
}

void AndClass::Print(std::ostream &s) const
{
   s << "("; LeftOperand->Print(s); s << ") AND ("; RightOperand->Print(s); s << ")";
}

std::shared_ptr<ConditionalExpressionClass> AndClass::Clone() const
{
    return std::make_shared<AndClass>(*this);
}

std::shared_ptr<ConditionalExpressionClass> AndClass::Optimize(Environment &Env)
{
    (void)Env;
    return shared_from_this();
}

void AndClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
    int NodeNumber1 = NodeNumber++;
    int NodeNumber2 = NodeNumber++;
    s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> and |<f2> \"];" << endl;
    s << "\"Node" << MyNodeNumber << "\":f0 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
    s << "\"Node" << MyNodeNumber << "\":f2 -> \"Node" << NodeNumber2 << "\":f1;" << endl;
    LeftOperand->DrawNode(s, NodeNumber1);
    RightOperand->DrawNode(s, NodeNumber2);

}

bool BinaryRelationalOperationClass::IsSame(std::shared_ptr<ConditionalExpressionClass> Other)
{
    if (typeid(*this) == Type_Id(*Other)) {
        return  ( LeftOperand->IsSame(std::dynamic_pointer_cast<BinaryRelationalOperationClass>(Other)->LeftOperand))
        &&( RightOperand->IsSame(std::dynamic_pointer_cast<BinaryRelationalOperationClass>(Other)->RightOperand));
    }
    return false;

}

void BinaryRelationalOperationClass::DrawNode(std::ostream &s, int MyNodeNumber, std::string Label) const
{
    int NodeNumber1 = NodeNumber++;
    int NodeNumber2 = NodeNumber++;
    s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> " << Label << " |<f2> \"];" << endl;
    s << "\"Node" << MyNodeNumber << "\":f0 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
    s << "\"Node" << MyNodeNumber << "\":f2 -> \"Node" << NodeNumber2 << "\":f1;" << endl;
    LeftOperand->DrawNode(s, NodeNumber1);
    RightOperand->DrawNode(s, NodeNumber2);

}


bool SameAsClass::Evaluate(Environment &Env) const
{
    return LeftOperand->Evaluate(Env) == RightOperand->Evaluate(Env);
}

void SameAsClass::Print(std::ostream &s) const
{
    s << "("; LeftOperand->Print(s); s << ") == ("; RightOperand->Print(s); s << ")";
}

std::shared_ptr<ConditionalExpressionClass> SameAsClass::Clone() const
{
    return std::make_shared<SameAsClass>(*this);
}

std::shared_ptr<ConditionalExpressionClass> SameAsClass::Optimize(Environment &Env)
{
    (void)Env;
    return shared_from_this();
}

void SameAsClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
    BinaryRelationalOperationClass::DrawNode(s, MyNodeNumber, "==");
}


bool NotSameAsClass::Evaluate(Environment &Env) const
{
    return LeftOperand->Evaluate(Env) != RightOperand->Evaluate(Env);
}

void NotSameAsClass::Print(std::ostream &s) const
{
    s << "("; LeftOperand->Print(s); s << ") != ("; RightOperand->Print(s); s << ")";
}

std::shared_ptr<ConditionalExpressionClass> NotSameAsClass::Clone() const
{
    return std::make_shared<NotSameAsClass>(*this);
}

std::shared_ptr<ConditionalExpressionClass> NotSameAsClass::Optimize(Environment &Env)
{
    (void)Env;
    return shared_from_this();
}

void NotSameAsClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
    BinaryRelationalOperationClass::DrawNode(s, MyNodeNumber, "!=");
}


bool LessThanClass::Evaluate(Environment &Env) const
{
    return LeftOperand->Evaluate(Env) < RightOperand->Evaluate(Env);
}

void LessThanClass::Print(std::ostream &s) const
{
    s << "("; LeftOperand->Print(s); s << ") < ("; RightOperand->Print(s); s << ")";
}

std::shared_ptr<ConditionalExpressionClass> LessThanClass::Clone() const
{
    return std::make_shared<LessThanClass>(*this);
}

std::shared_ptr<ConditionalExpressionClass> LessThanClass::Optimize(Environment &Env)
{
    (void)Env;
    return shared_from_this();
}

void LessThanClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
    BinaryRelationalOperationClass::DrawNode(s, MyNodeNumber, "\\<");
}


bool LessOrSameThanClass::Evaluate(Environment &Env) const
{
    return LeftOperand->Evaluate(Env) <= RightOperand->Evaluate(Env);
}

void LessOrSameThanClass::Print(std::ostream &s) const
{
    s << "("; LeftOperand->Print(s); s << ") <= ("; RightOperand->Print(s); s << ")";
}

std::shared_ptr<ConditionalExpressionClass> LessOrSameThanClass::Clone() const
{
    return std::make_shared<LessOrSameThanClass>(*this);
}

std::shared_ptr<ConditionalExpressionClass> LessOrSameThanClass::Optimize(Environment &Env)
{
    (void)Env;
    return shared_from_this();
}

void LessOrSameThanClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
    BinaryRelationalOperationClass::DrawNode(s, MyNodeNumber, "\\<=");
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

std::shared_ptr<StatementClass> RepeatLoopClass::Optimize(Environment &Env)
{
    (void)Env;
    return shared_from_this();
}

void DrawStatementNodeList(const std::list<std::shared_ptr<StatementClass>> &Statements, std::ostream &os, int ParentNodeNumber) {


    for (auto const &s: Statements) {
        int MyNodeNumber = NodeNumber++;
        os << "\"Node" << ParentNodeNumber << "\":f2 -> \"Node" << MyNodeNumber << "\":f1;" << endl;
        s->DrawNode(os, MyNodeNumber);
        ParentNodeNumber = MyNodeNumber;
    }
}

void RepeatLoopClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
    int NodeNumber1 = NodeNumber++;
    s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> repeat |<f2> \"];" << endl;
    s << "\"Node" << MyNodeNumber << "\":f1 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
    Condition->DrawNode(s, NodeNumber1);
    DrawStatementNodeList(Statements, s, MyNodeNumber);

}


void ifClass::Print(std::ostream &s) const
{
    s << "if (";
    Condition->Print(s); s << ") then \n";
    for(auto &r: TrueStatements) {
        r->Print(s);
    }
    s << "else \n";
    for(auto &r: FalseStatements) {
        r->Print(s);
    }
    s << "endif ;\n";
}

std::shared_ptr<StatementClass> ifClass::Clone() const
{
    return std::make_shared<ifClass>(*this);
}

std::shared_ptr<StatementClass> ifClass::Optimize(Environment &Env)
{
    (void)Env;
    return shared_from_this();
}

void ifClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
    int NodeNumber1 = NodeNumber++;
    s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> if |<f2> \"];" << endl;
    s << "\"Node" << MyNodeNumber << "\":f1 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
    Condition->DrawNode(s, NodeNumber1);
    DrawStatementNodeList(TrueStatements, s, MyNodeNumber);
    DrawStatementNodeList(FalseStatements, s, MyNodeNumber);

}


void FunctionCallStatementClass::Print(std::ostream &s) const
{
    s << "function " << Function->GetName() << "(";
    //Function->Print(s);
    s << "endfunction" << std::endl;
}

std::shared_ptr<StatementClass> FunctionCallStatementClass::Clone() const
{
    return std::make_shared<FunctionCallStatementClass>(*this);
}

std::shared_ptr<StatementClass> FunctionCallStatementClass::Optimize(Environment &Env)
{
    (void)Env;
    return shared_from_this();
}

void FunctionCallStatementClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
    int NodeNumber1 = NodeNumber++;
    s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> call |<f2> \"];" << endl;
    s << "\"Node" << MyNodeNumber << "\":f0 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
    Function->DrawNode(s, NodeNumber1);

}

Variables::VariableContentClass FunctionCallClass::CommonEvaluate(Environment &Env, std::shared_ptr<Variables::FunctionDefinitionBaseClass> TheFunction) const
{
    if (Env.DoEvaluateFunctions) {
        TheFunction->CreateFrame();
        try {
           for (auto const &s: Assignements) {
               s->Execute(Env);
           }
        }
        catch (RuntimeErrorClass &e) {
            std::stringstream s;
            s << "while passing Arguments at [" << GetLocation() << "]";
            e.ExtendMessage(s.str(), GetLocation().begin.line);
            throw e;
        }
        Variables::VariableContentClass Result;
        try  {
            Result = TheFunction->Execute(Env);
        }
        catch (RuntimeErrorClass &e) {
           std::stringstream s;
           s << "while executing functionbody at [" << GetLocation() << "]";
           e.ExtendMessage(s.str(), GetLocation().begin.line);
           throw e;
        }

        TheFunction->ReleaseFrame();
        return Result;
    } else {
        return Variables::VariableContentClass::MakeUndefined();
    }
}

Variables::VariableContentClass FunctionCallClass::Evaluate(Environment &Env) const
{
    return CommonEvaluate(Env, TheFunction);
}


Variables::VariableContentClass MethodCallClass::Evaluate(Environment &Env) const
{
    if (Env.DoEvaluateFunctions) {
        Variables::VariableContentClass ThisReference = ThisPointer->Evaluate(Env);
        if (ThisReference.holds_alternative<Variables::ObjectReference>()) {
            std::shared_ptr<const Variables::ClassClass> Class = ThisReference.GetValue<Variables::ObjectReference>()->GetTypeDescriptor().GetTypeDetails<ObjectReferenceDescriptorClass>().GetClass();
            if (Class == nullptr) {
                throw INTERNAL_ERROR_OBJECT("Objecdescriptor has no class");
            }
            std::shared_ptr<Variables::MethodDefinitionClass> TheMethod = Class->GetMethod(GetName());
            if (TheMethod != nullptr) {
               return CommonEvaluate(Env, TheMethod);
            } else {
                throw RuntimeErrorClass("Method '" + GetName() + "' for class '" + Class->GetName() + "' not found", GetLocation().begin.line);
            }
        } else {
            throw RuntimeErrorClass("Calling method for something not an object", GetLocation().begin.line);
        }
    } else {
        return Variables::VariableContentClass::MakeUndefined();
    }
}

void FunctionCallClass::Print(std::ostream &s) const { TheFunction->Print(s); }

bool FunctionCallClass::IsSame(std::shared_ptr<ExpressionClass> Other)
{
    (void)Other;
    return false;
}

void FunctionCallClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
    int NodeNumber1 = NodeNumber++;
    s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> call |<f2> \"];" << endl;
    s << "\"Node" << MyNodeNumber << "\":f0 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
    TheFunction->DrawDeclarationNode(s, NodeNumber1);
    DrawStatementNodeList(Assignements, s, MyNodeNumber);

}

const TypeDescriptorClass FunctionCallClass::GetType() const
{
    return TheFunction->GetReturnType();
}


Variables::VariableContentClass InstanceClass::Evaluate(Environment &Env) const
{
    if (Env.DoEvaluateFunctions) {
        return Variables::VariableContentClass((TheClass->CreateInstance()));
    } else {
        return Variables::VariableContentClass::MakeUndefined();
    }
}

void InstanceClass::Print(std::ostream &s) const { s << TheClass->GetName(); }

bool InstanceClass::IsSame(std::shared_ptr<ExpressionClass> Other)
{
    (void)Other;
    return false;
}

void InstanceClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
    //int NodeNumber1 = NodeNumber++;
    s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> Instance of " << TheClass->GetName() << " |<f2> \"];" << endl;
   // s << "\"Node" << MyNodeNumber << "\":f0 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
   // TheFunction->DrawDeclarationNode(s, NodeNumber1);
   // DrawStatementNodeList(Assignements, s, MyNodeNumber);

}

const TypeDescriptorClass InstanceClass::GetType() const
{
    return VariableTypeDescriptorClass(ObjectReferenceDescriptorClass(TheClass));
}


const TypeDescriptorClass UnaryOperationClass::GetType() const
{
    return Operand->Type();
}

void ErrorStatement::Print(std::ostream &s) const
{
    s << "<Error Node>";
}

std::shared_ptr<StatementClass> ErrorStatement::Clone() const
{
    return std::make_shared<ErrorStatement>(*this);
}

std::shared_ptr<StatementClass> ErrorStatement::Optimize(Environment &Env)
{
    (void)Env;
    return shared_from_this();
}

void ErrorStatement::DrawNode(std::ostream &s, int MyNodeNumber) const
{
    s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> error |<f2> \"];" << endl;
}


void PrintStatementClass::Print(std::ostream &s) const
{
    s << "print (\n";
    for (auto &r: Expressions) {r->Print(s);}
    s << ")" << std::endl;
}

std::shared_ptr<StatementClass> PrintStatementClass::Clone() const
{
    return std::make_shared<PrintStatementClass>(*this);

}

std::shared_ptr<StatementClass> PrintStatementClass::Optimize(Environment &Env)
{
    (void)Env;
    return shared_from_this();

}

void PrintStatementClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
    s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> print |<f2> \"];" << endl;
    for (auto const &e: Expressions ) {
        int NodeNumber1 = NodeNumber++;
        s << "\"Node" << MyNodeNumber << "\":f0 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
        e->DrawNode(s, NodeNumber1);
    }
}

StatementResultClass PrintStatementClass::Execute(Environment &Env) const
{
    Env.Tracing(GetLocation(), "Print(...)");
    Env.SetOutputStreamColor(Environment::Color::Red);
    for (auto &e: Expressions) {
        // Env.OutputStream() << e->Evaluate(Env);
        e->Evaluate(Env).PrintDetail(Env.OutputStream(), 200);
    }
    Env.OutputStream() << "\n";
    Env.RestoreOutputStreamColor();
    return {};
}




StatementResultClass RepeatLoopClass::Execute(Environment &Env) const
{
    Env.Tracing(GetLocation(), "Repeat(...)");
    do {
        for (auto const &s: Statements) {
            auto r = s->Execute(Env);
            if (r) {
                return r;
            }
        }
        Env.ThrowIfStoppRequested();
    } while (Condition->Evaluate(Env) == false);
    return {};
}

StatementResultClass ifClass::Execute(Environment &Env) const
{
    Env.Tracing(GetLocation(), "If(...)");
    if (Condition->Evaluate(Env) == true) {
        for (auto const &s: TrueStatements) {
            auto r = s->Execute(Env);
            if (r) {
                return r;
            }
        }
    } else {
        for (auto const &s: FalseStatements) {
            auto r = s->Execute(Env);
            if (r) {
                return r;
            }
        }
    }
    return {};
}


StatementResultClass FunctionCallStatementClass::Execute(Environment &Env) const
{
    Env.Tracing(GetLocation(), "Call Fkt(...)");
    Function->Evaluate(Env);
    return {};
}

Variables::ElementSelectorType IndexedValueClass::BuildSelector() const
{
    Variables::ElementSelectorType Selector;
    if (std::holds_alternative<IndexList>(Indices)) {
        Selector.reserve(std::get<IndexList>(Indices).size());
        for (auto const &i: std::get<IndexList>(Indices) ) {

            // Build index vector
            Selector.push_back(i->GetIndex());
        }
    } else if (std::holds_alternative<std::shared_ptr<ExpressionClass>>(Indices)) {

        auto Result = std::get<std::shared_ptr<ExpressionClass>>(Indices)->Evaluate();

        if (Result.holds_alternative<int64_t>()) {
            int64_t t = Result.GetValue<int64_t>();
            if (t < 0) {
                Selector.push_back(t);
            } else {
                Selector.push_back(Variables::ArrayIndexType(t));
            }
        } else if (Result.holds_alternative<std::string>()) {
            Selector.push_back(Result.GetValue<std::string>());
        } else {

            // here we could handle vector n or n*2 for ranges and list
            throw RuntimeErrorClass("Index other than integer or string not allowed yet", -1);
        }
    } else {
        throw INTERNAL_ERROR_OBJECT("unknown index type");
    }
    //VariableTypeDescriptorClass t(Type());
    return Selector;
}

Variables::VariableContentClass IndexedValueClass::Evaluate(Environment &Env) const
{
    (void)Env;
    //return IndexedValue->Evaluate().Isempty()?Variables::VariableContentClass(std::const_pointer_cast<ExpressionClass>(shared_from_this())):Val->GetValue();
    const VariableReferenceType &ReferedVariable = IndexedValue->GetWriteReferenceToContent(IfNotExistDoNotCreate);
    const Variables::VariableContentClass &ReferedContent = ReferedVariable->GetValue();
    return ReferedContent.at(BuildSelector());
}


void IndexedValueClass::Print(std::ostream &s) const
{
    s << "(";
    IndexedValue->Print(s);
    s << "[";
    if (std::holds_alternative<IndexList>(Indices)) {
        for (auto const &i: std::get<IndexList>(Indices)) {
            i->Print(s);
            s << ",";
        }
    } else if (std::holds_alternative<std::shared_ptr<ExpressionClass>>(Indices)) {
        std::get<std::shared_ptr<ExpressionClass>>(Indices)->Print(s);
    } else {
        throw INTERNAL_ERROR_OBJECT("unknown index type");
    }

    s << "]";
    s << ")";
}

bool IndexedValueClass::IsSame(std::shared_ptr<ExpressionClass> Other)
{
    (void)Other;
    return false;
}

void IndexedValueClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
    s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> [...] |<f2> \"];" << endl;
    int NodeNumber1 = NodeNumber++;
    s << "\"Node" << MyNodeNumber << "\":f0 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
    IndexedValue->DrawNode(s, NodeNumber1);
    if (std::holds_alternative<IndexList>(Indices)) {
        for (auto const &i: std::get<IndexList>(Indices) ) {
            NodeNumber1 = NodeNumber++;
            s << "\"Node" << MyNodeNumber << "\":f2 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
            i->DrawNode(s, NodeNumber1);
        }
    } else if (std::holds_alternative<std::shared_ptr<ExpressionClass>>(Indices)) {
        NodeNumber1 = NodeNumber++;
        s << "\"Node" << MyNodeNumber << "\":f2 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
        std::get<std::shared_ptr<ExpressionClass>>(Indices)->DrawNode(s, NodeNumber1);
    } else {
        throw INTERNAL_ERROR_OBJECT("unknown index type");
    }


}

VariableReferenceType IndexedValueClass::GetWriteReferenceToContent(ModeType Mode)
{
    const VariableReferenceType &ReferedVariable = IndexedValue->GetWriteReferenceToContent(Mode);
    const Variables::VariableContentClass &ReferedContent = ReferedVariable->GetValue();
    Variables::VariableContentClass &SelectedElement = (Mode == IfNotExistCreateIfPossible) ? ReferedContent[BuildSelector()] : ReferedContent.at(BuildSelector());
    return std::make_shared<ProxyVariableClass>(GetName(), ReferedContent.getContainedType(), SelectedElement, IndexedValue->GetStorageClass());
}

void SingleIndexExpressionClass::Print(std::ostream &s) const
{
    Index->Print(s);
}

void SingleIndexExpressionClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
    // Just forward to subnode
    Index->DrawNode(s, MyNodeNumber);
}

void RangedIndexExpressionClass::Print(std::ostream &s) const
{
    if (FromIndex != nullptr) {
        FromIndex->Print(s);
    } else {

    }
    s << "...";
    if (ToIndex != nullptr) {
        ToIndex->Print(s);
    } else {

    }
}

void RangedIndexExpressionClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
    s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> Range |<f2> \"];" << endl;
    int NodeNumber1 = NodeNumber++;
    int NodeNumber2 = NodeNumber++;
    s << "\"Node" << MyNodeNumber << "\":f0 -> \"Node" << NodeNumber1 << "\":f1;" << endl;
    s << "\"Node" << MyNodeNumber << "\":f2 -> \"Node" << NodeNumber2 << "\":f1;" << endl;
    FromIndex->DrawNode(s, NodeNumber1);
    ToIndex->DrawNode(s, NodeNumber2);
}
Variables::SingleElementSelectorType RangedIndexExpressionClass::GetIndex() const
{
    uint64_t From = 0;
    uint64_t To = std::numeric_limits<uint64_t>::max();
    if (FromIndex != nullptr) {
        int64_t t = FromIndex->Evaluate().GetValue<int64_t>();
        if (t < 0) {
            throw RuntimeErrorClass("Negative Index not allowed", -1);
        }
        From = uint64_t(t);
    }
    if (ToIndex != nullptr) {
        int64_t t = ToIndex->Evaluate().GetValue<int64_t>();
        if (t < 0) {
            throw RuntimeErrorClass("Negative Index not allowed", -1);
        }
        To = uint64_t(t);
    }
    return Variables::IndexRangeType{From, To};
}
