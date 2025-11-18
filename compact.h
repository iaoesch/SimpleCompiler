#include <iostream>
#include <list>
#include <memory>
#include "environment.hpp"
#include "variableclass.h"

extern int GetNextNodeNumber();
extern void DrawStatementNodeList(const std::list<std::shared_ptr<StatementClass>> &Statements, std::ostream &os, int ParentNodeNumber);




class VariableClasse {
   std::string Name;
   double      Value;

   public:

               VariableClasse(std::string N, double V) : Name(N), Value(V) {}
   std::string GetName()  const {return Name; }
   double      GetValue() const {return Value; }
   void        SetValue(double v) {Value = v; }
};

typedef  std::shared_ptr<VariableClass> VariableReferenceType;

class ExpressionClass : public std::enable_shared_from_this<ExpressionClass>{
   public:
   virtual                  ~ExpressionClass() {}
   Variables::VariableContentClass   Evaluate() const;// = 0;
   virtual Variables::VariableContentClass   Evaluate(Environment &Env) const;// = 0;
   virtual std::shared_ptr<ExpressionClass>  Derive(VariableReferenceType ToDerive) const;// = 0;
   virtual void              Print(std::ostream &s) const;// = 0;
   virtual std::shared_ptr<ExpressionClass> Clone() const;// = 0;
   virtual std::shared_ptr<ExpressionClass> Optimize(Environment &Env);// = 0;
   virtual bool              IsConstant();// = 0;
   virtual bool              IsSame(std::shared_ptr<ExpressionClass>Other);// = 0;
   virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const;
   const TypeDescriptorClass           Type() {return GetType();}
   private:
   virtual const TypeDescriptorClass   GetType() const = 0;

};

class ValueClass : public ExpressionClass {

   public:
   virtual                 ~ValueClass() override{}
};

class UnaryOperationClass : public ExpressionClass {

   public:
   //protected:
   std::shared_ptr<ExpressionClass>Operand;

   public:
                             UnaryOperationClass(std::shared_ptr<ExpressionClass>e) : Operand(e) {}
   virtual                  ~UnaryOperationClass() override {/* delete Operand;*/ }
   virtual bool              IsConstant() override {return Operand->IsConstant();}
   virtual bool              IsSame(std::shared_ptr<ExpressionClass>Other) override ;// = 0;

   // ExpressionClass interface
   private:
   virtual const TypeDescriptorClass GetType() const override;
};

class BinaryOperationClass : public ExpressionClass {

   public:
   //protected:
   std::shared_ptr<ExpressionClass>LeftOperand;
   std::shared_ptr<ExpressionClass>RightOperand;

   public:
                             BinaryOperationClass(std::shared_ptr<ExpressionClass>l, std::shared_ptr<ExpressionClass>r) : LeftOperand(l), RightOperand(r) {}
   virtual                  ~BinaryOperationClass() override {/*delete LeftOperand; delete RightOperand;*/}
   virtual bool              IsConstant() override {return LeftOperand->IsConstant()&&RightOperand->IsConstant();}
   virtual bool              IsSame(std::shared_ptr<ExpressionClass>Other) override;// = 0;

   private:
   virtual const TypeDescriptorClass GetType() const override;
};

class ConstantClass : public ValueClass {
   Variables::VariableContentClass Value;

   public:
                             ConstantClass(Variables::VariableContentClass v) : Value(v) {}
   virtual                  ~ConstantClass() override {}
   virtual Variables::VariableContentClass            Evaluate(Environment &Env) const override { return Value; }
   virtual std::shared_ptr<ExpressionClass> Derive(VariableReferenceType ToDerive) const override { return std::make_shared<ConstantClass>(0.0); }
   virtual void              Print(std::ostream &s) const override { s << Value; }
   virtual std::shared_ptr<ExpressionClass> Clone() const override { return std::make_shared<ConstantClass>(*this); }
   virtual std::shared_ptr<ExpressionClass> Optimize(Environment &Env) override { return shared_from_this(); }
   virtual bool              IsConstant() override {return true;}
   virtual bool              IsSame(std::shared_ptr<ExpressionClass>Other) override;// = 0;
   virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const override;

   private:
   virtual const TypeDescriptorClass GetType() const override;
};

class VariableValueClass : public ValueClass {
   const VariableReferenceType Val;

   public:
                             VariableValueClass(VariableReferenceType v) : Val(v) {}
                             VariableValueClass(const VariableValueClass &v) : Val(v.Val) {}
   virtual                  ~VariableValueClass() override {}
   virtual Variables::VariableContentClass  Evaluate(Environment &Env) const override{ return Val->GetValue(); }
   virtual std::shared_ptr<ExpressionClass> Derive(VariableReferenceType ToDerive) const override { if (ToDerive == Val) {return std::make_shared<ConstantClass>(1.0);} else {return std::make_shared<ConstantClass>(0.0);}}
   virtual void              Print(std::ostream &s) const override { s << Val->GetName(); }
   virtual std::shared_ptr<ExpressionClass> Clone() const override { return std::make_shared<VariableValueClass>(*this); }
   virtual std::shared_ptr<ExpressionClass> Optimize(Environment &Env) override { return shared_from_this(); }
   virtual bool              IsConstant() override {return false;}
   virtual bool              IsSame(std::shared_ptr<ExpressionClass>Other) override;// = 0;
   virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const override;

   private:
   virtual const TypeDescriptorClass GetType() const override;
};

class FunctionClass;

class FunctionCallClass : public ValueClass {
    std::shared_ptr<Variables::FunctionDefinitionClass> TheFunction;
    std::list<std::shared_ptr<StatementClass>> Assignements;


public:
    FunctionCallClass(std::shared_ptr<Variables::FunctionDefinitionClass> f, std::list<std::shared_ptr<StatementClass>> a) : TheFunction(f), Assignements(a) {}
    FunctionCallClass(const FunctionCallClass &f) = default;
    virtual                  ~FunctionCallClass() override {}
    virtual Variables::VariableContentClass  Evaluate(Environment &Env) const override;//Val->GetValue(); }
    virtual std::shared_ptr<ExpressionClass> Derive(VariableReferenceType ToDerive) const override { if (ToDerive == ToDerive) {return std::make_shared<ConstantClass>(1.0);} else {return std::make_shared<ConstantClass>(0.0);}}
    virtual void              Print(std::ostream &s) const override;
    virtual std::shared_ptr<ExpressionClass> Clone() const override { return std::make_shared<FunctionCallClass>(*this); }
    virtual std::shared_ptr<ExpressionClass> Optimize(Environment &Env) override { return shared_from_this(); }
    virtual bool              IsConstant() override {return false;}
    virtual bool              IsSame(std::shared_ptr<ExpressionClass>Other) override;// = 0;
    virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const override;

    const std::string &GetName() {return TheFunction->GetName();}
private:
    virtual const TypeDescriptorClass GetType() const override;
};


class InverseClass : public UnaryOperationClass {

   public:
                             InverseClass(std::shared_ptr<ExpressionClass>e) : UnaryOperationClass(e) {}
                             InverseClass(const InverseClass &v) : UnaryOperationClass(v) {}
   virtual                  ~InverseClass() override {}
   virtual Variables::VariableContentClass            Evaluate(Environment &Env) const override;//{return (1 / Operand->Evaluate(Env)); };
   virtual std::shared_ptr<ExpressionClass> Derive(VariableReferenceType  ToDerive) const override;// {return new NegationClass(new MultiplyClass(new InverseClass( new SquareClass(Operand->Clone())), Operand->Derive())); };
   virtual void              Print(std::ostream &s) const override;//{ s << "1.0 / ("; Operand->Print(s); s << ")";  };
   virtual std::shared_ptr<ExpressionClass> Clone() const override;//{return new InverseClass(*this); };
   virtual std::shared_ptr<ExpressionClass> Optimize(Environment &Env) override;// { return NULL; };
   virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const override;

};

class SquareClass : public UnaryOperationClass {

   public:
                             SquareClass(std::shared_ptr<ExpressionClass>e) : UnaryOperationClass(e) {}
                             SquareClass(const SquareClass &v) : UnaryOperationClass(v) {}
   virtual                  ~SquareClass() override {}
   virtual Variables::VariableContentClass            Evaluate(Environment &Env) const override;//{double tmp = Operand->Evaluate(Env); return tmp*tmp; };
   virtual std::shared_ptr<ExpressionClass> Derive(VariableReferenceType  ToDerive) const override;//{return new MultiplyClass(new MultiplyClass(new ConstantClass(2.0), Operand->Clone()), Operand->Derive()); };
   virtual void              Print(std::ostream &s) const override;//{ s << "("; Operand->Print(s); s << ")^2.0";  };
   virtual std::shared_ptr<ExpressionClass> Clone() const override;//{return new SquareClass(*this); };
   virtual std::shared_ptr<ExpressionClass> Optimize(Environment &Env) override;// { return NULL; };
   virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const override;
};

class NegationClass : public UnaryOperationClass {

   public:
                             NegationClass(std::shared_ptr<ExpressionClass>e) : UnaryOperationClass(e) {}
                             NegationClass(const NegationClass &v) : UnaryOperationClass(v) {}
   virtual                  ~NegationClass() override {}
   virtual Variables::VariableContentClass            Evaluate(Environment &Env) const override;//{return - Operand->Evaluate(Env); };
   virtual std::shared_ptr<ExpressionClass> Derive(VariableReferenceType  ToDerive) const override;//{return new NegateClass(Operand->Derive()); };
   virtual void              Print(std::ostream &s) const override;//{ s << "-("; Operand->Print(s); s << ")";  };
   virtual std::shared_ptr<ExpressionClass> Clone() const override;//{return new NegationClass(*this); };
   virtual std::shared_ptr<ExpressionClass> Optimize(Environment &Env) override;// { return NULL; };
   virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const override;
};

class LogarithmClass : public UnaryOperationClass {

   public:
                             LogarithmClass(std::shared_ptr<ExpressionClass>e) : UnaryOperationClass(e) {}
                             LogarithmClass(const LogarithmClass &v) : UnaryOperationClass(v) {}
   virtual                  ~LogarithmClass() override{}
   virtual Variables::VariableContentClass            Evaluate(Environment &Env) const override;//{return (log(Operand->Evaluate(Env)); };
   virtual std::shared_ptr<ExpressionClass> Derive(VariableReferenceType  ToDerive) const override;//{return new MultiplyClass(new InverseClass(Operand->Clone()), Operand->Derive()); };
   virtual void              Print(std::ostream &s) const override;//{ s << "ln("; Operand->Print(s); s << ")";  };
   virtual std::shared_ptr<ExpressionClass> Clone() const override;//{return new LogarithmClass(*this); };
   virtual std::shared_ptr<ExpressionClass> Optimize(Environment &Env) override;// { return NULL; };
   virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const override;
};

class ExponentialClass : public UnaryOperationClass {

   public:
                             ExponentialClass(std::shared_ptr<ExpressionClass>e) : UnaryOperationClass(e) {}
                             ExponentialClass(const ExponentialClass &v) : UnaryOperationClass(v) {}
   virtual                  ~ExponentialClass() override {}
   virtual Variables::VariableContentClass            Evaluate(Environment &Env) const override;//{return (exp(Operand->Evaluate(Env)); };
   virtual std::shared_ptr<ExpressionClass> Derive(VariableReferenceType  ToDerive) const override;//{return new MultiplyClass(new ExponentialClass(*this)), Operand->Derive()); };
   virtual void              Print(std::ostream &s) const override;//{ s << "exp("; Operand->Print(s); s << ")";  };
   virtual std::shared_ptr<ExpressionClass> Clone() const override;//{return new ExponentialClass(*this); };
   virtual std::shared_ptr<ExpressionClass> Optimize(Environment &Env) override;// { return NULL; };
   virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const override;
};

class SquareRootClass : public UnaryOperationClass {

   public:
    SquareRootClass(std::shared_ptr<ExpressionClass>e) : UnaryOperationClass(e) {}
                             SquareRootClass(const SquareRootClass &v) : UnaryOperationClass(v) {}
   virtual                  ~SquareRootClass() override {}
   virtual Variables::VariableContentClass            Evaluate(Environment &Env) const override;//{return (sqrt(Operand->Evaluate(Env)); };
   virtual std::shared_ptr<ExpressionClass> Derive(VariableReferenceType  ToDerive) const override;//{return new MultiplyClass(new InverseClass(new MultiplyClass(new ConstantClass(2.0), new SquareRootClass(*this))), Operand->Derive()); };
   virtual void              Print(std::ostream &s) const override;//{ s << "sqrt("; Operand->Print(s); s << ")"; };
   virtual std::shared_ptr<ExpressionClass> Clone() const override;//{return new SquareRootClass(*this); };
   virtual std::shared_ptr<ExpressionClass> Optimize(Environment &Env) override;// { return NULL; };
   virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const override;
};


class PowerClass : public BinaryOperationClass {

   public:
                             PowerClass(std::shared_ptr<ExpressionClass>e1, std::shared_ptr<ExpressionClass>e2) : BinaryOperationClass(e1, e2) {}
                             PowerClass(const PowerClass &v) : BinaryOperationClass(v) {}
   virtual                  ~PowerClass() override {}
   virtual Variables::VariableContentClass            Evaluate(Environment &Env) const override;//{return LeftOperand->Evaluate(Env) * RigthOperand->Evaluate(Env); };
   virtual std::shared_ptr<ExpressionClass> Derive(VariableReferenceType  ToDerive) const override;//{return new AdditionClass(new MultiplyClass(LeftOperand->Clone(), RigthOperand->Derive()), new MultiplyClass(LeftOperand->Derive(), RigthOperand->Clone())); };
   virtual void              Print(std::ostream &s) const override;//{ s << "("; LeftOperand->Print(s); s << ") * ("; RigthOperand->Print(s); s << ")"; };
   virtual std::shared_ptr<ExpressionClass> Clone() const override;//{return new MultiplyClass(*this); };
   virtual std::shared_ptr<ExpressionClass> Optimize(Environment &Env) override;// { return NULL; };
   virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const override;
};

class MultiplyClass : public BinaryOperationClass {

   public:
                             MultiplyClass(std::shared_ptr<ExpressionClass>e1, std::shared_ptr<ExpressionClass>e2) : BinaryOperationClass(e1, e2) {}
                             MultiplyClass(const MultiplyClass &v) : BinaryOperationClass(v) {}
   virtual                  ~MultiplyClass() override {}
   virtual Variables::VariableContentClass            Evaluate(Environment &Env) const override;//{return LeftOperand->Evaluate(Env) * RigthOperand->Evaluate(Env); };
   virtual std::shared_ptr<ExpressionClass> Derive(VariableReferenceType  ToDerive) const override;//{return new AdditionClass(new MultiplyClass(LeftOperand->Clone(), RigthOperand->Derive()), new MultiplyClass(LeftOperand->Derive(), RigthOperand->Clone())); };
   virtual void              Print(std::ostream &s) const override;//{ s << "("; LeftOperand->Print(s); s << ") * ("; RigthOperand->Print(s); s << ")"; };
   virtual std::shared_ptr<ExpressionClass> Clone() const override;//{return new MultiplyClass(*this); };
   virtual std::shared_ptr<ExpressionClass> Optimize(Environment &Env) override;// { return NULL; };
   virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const override;
};


class AdditionClass : public BinaryOperationClass {

   public:
                             AdditionClass(std::shared_ptr<ExpressionClass>e1, std::shared_ptr<ExpressionClass>e2) : BinaryOperationClass(e1, e2) {}
                             AdditionClass(const AdditionClass &v) : BinaryOperationClass(v) {}
   virtual                  ~AdditionClass() override {}
   virtual Variables::VariableContentClass            Evaluate(Environment &Env) const override;//{return LeftOperand->Evaluate(Env) + RigthOperand->Evaluate(Env); };
   virtual std::shared_ptr<ExpressionClass> Derive(VariableReferenceType  ToDerive) const override ;//{return new AdditionClass(LeftOperand->Derive()), RigthOperand->Derive()); };
   virtual void              Print(std::ostream &s) const override;//{ s << "("; LeftOperand->Print(s); s << ") * ("; RigthOperand->Print(s); s << ")"; };
   virtual std::shared_ptr<ExpressionClass> Clone() const override;//{return new AdditionClass(*this); };
   virtual std::shared_ptr<ExpressionClass> Optimize(Environment &Env) override;// { return NULL; };
   virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const override;
};

class ConditionalExpressionClass : public std::enable_shared_from_this<ConditionalExpressionClass>{
public:
    virtual                  ~ConditionalExpressionClass() {}
    virtual bool              Evaluate(Environment &Env) const;// = 0;
    virtual void              Print(std::ostream &s) const;// = 0;
    virtual std::shared_ptr<ConditionalExpressionClass> Clone() const;// = 0;
    virtual std::shared_ptr<ConditionalExpressionClass> Optimize(Environment &Env);// = 0;
    virtual bool              IsConstant();// = 0;
    virtual bool              IsSame(std::shared_ptr<ConditionalExpressionClass>Other);// = 0;
    virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const;
};

class BinaryConditionalOperationClass : public ConditionalExpressionClass {

public:
    //protected:
    std::shared_ptr<ConditionalExpressionClass>LeftOperand;
    std::shared_ptr<ConditionalExpressionClass>RightOperand;

public:
    BinaryConditionalOperationClass(std::shared_ptr<ConditionalExpressionClass>l, std::shared_ptr<ConditionalExpressionClass>r) : LeftOperand(l), RightOperand(r) {}
    virtual                  ~BinaryConditionalOperationClass() override {/*delete LeftOperand; delete RightOperand;*/}
    virtual bool              IsConstant() override {return LeftOperand->IsConstant()&&RightOperand->IsConstant();}
    virtual bool              IsSame(std::shared_ptr<ConditionalExpressionClass>Other) override;// = 0;
};


class AndClass : public BinaryConditionalOperationClass {

public:
    AndClass(std::shared_ptr<ConditionalExpressionClass>e1, std::shared_ptr<ConditionalExpressionClass>e2) : BinaryConditionalOperationClass(e1, e2) {}
    AndClass(const AndClass &v) : BinaryConditionalOperationClass(v) {}
    virtual                  ~AndClass() override {}
    virtual bool              Evaluate(Environment &Env) const override;//{return LeftOperand->Evaluate(Env) + RigthOperand->Evaluate(Env); };
    virtual void              Print(std::ostream &s) const override;//{ s << "("; LeftOperand->Print(s); s << ") * ("; RigthOperand->Print(s); s << ")"; };
    virtual std::shared_ptr<ConditionalExpressionClass> Clone() const override;//{return new AdditionClass(*this); };
    virtual std::shared_ptr<ConditionalExpressionClass> Optimize(Environment &Env) override;// { return NULL; };
    virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const override;
};

class BinaryRelationalOperationClass : public ConditionalExpressionClass {

public:
    //protected:
    std::shared_ptr<ExpressionClass>LeftOperand;
    std::shared_ptr<ExpressionClass>RightOperand;

public:
    BinaryRelationalOperationClass(std::shared_ptr<ExpressionClass>l, std::shared_ptr<ExpressionClass>r) : LeftOperand(l), RightOperand(r) {}
    virtual                  ~BinaryRelationalOperationClass() override {/*delete LeftOperand; delete RightOperand;*/}
    virtual bool              IsConstant() override {return LeftOperand->IsConstant()&&RightOperand->IsConstant();}
    virtual bool              IsSame(std::shared_ptr<ConditionalExpressionClass>Other) override;// = 0;
};

class LessThanClass : public BinaryRelationalOperationClass {

public:
    LessThanClass(std::shared_ptr<ExpressionClass>e1, std::shared_ptr<ExpressionClass>e2) : BinaryRelationalOperationClass(e1, e2) {}
    LessThanClass(const LessThanClass &v) : BinaryRelationalOperationClass(v) {}
    virtual                  ~LessThanClass() override {}
    virtual bool              Evaluate(Environment &Env) const override;//{return LeftOperand->Evaluate(Env) + RigthOperand->Evaluate(Env); };
    virtual void              Print(std::ostream &s) const override;//{ s << "("; LeftOperand->Print(s); s << ") * ("; RigthOperand->Print(s); s << ")"; };
    virtual std::shared_ptr<ConditionalExpressionClass> Clone() const override;//{return new AdditionClass(*this); };
    virtual std::shared_ptr<ConditionalExpressionClass> Optimize(Environment &Env) override;// { return NULL; };
    virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const override;
};


class StatementClass : public std::enable_shared_from_this<StatementClass>{
public:
    virtual                  ~StatementClass() {}
    virtual void              Print(std::ostream &s) const;// = 0;
    virtual std::shared_ptr<StatementClass> Clone() const;// = 0;
    virtual std::shared_ptr<StatementClass> Optimize(Environment &Env);// = 0;
    virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const;
    virtual void              Execute(Environment &Env) const;// = 0;
};

class AssignementClass : public StatementClass {
    std::shared_ptr<ExpressionClass> AssignedExpression;
    const VariableReferenceType Variable;

public:
    AssignementClass(std::shared_ptr<ExpressionClass> _AssignedExpression, VariableReferenceType _Variable) :
        AssignedExpression(_AssignedExpression), Variable(_Variable) {}

    virtual                  ~AssignementClass() override;
    virtual void              Print(std::ostream &s) const override;// = 0;
    virtual std::shared_ptr<StatementClass> Clone() const override;// = 0;
    virtual std::shared_ptr<StatementClass> Optimize(Environment &Env) override;// = 0;
    virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const override;
    virtual void              Execute(Environment &Env) const override;// = 0;
};

class ReferementClass  : public AssignementClass {
    using AssignementClass::AssignementClass;
};

class RepeatLoopClass : public StatementClass {
    std::list<std::shared_ptr<StatementClass>> Statements;
    std::shared_ptr<ConditionalExpressionClass> Condition;

public:
    RepeatLoopClass(std::list<std::shared_ptr<StatementClass>> _Statements, std::shared_ptr<ConditionalExpressionClass> _Condition) :
        Statements(_Statements), Condition(_Condition) {}

    virtual                  ~RepeatLoopClass() override {}
    virtual void              Print(std::ostream &s) const override;// = 0;
    virtual std::shared_ptr<StatementClass> Clone() const override;// = 0;
    virtual std::shared_ptr<StatementClass> Optimize(Environment &Env) override;// = 0;
    virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const override;


    // StatementClass interface
public:
    virtual void Execute(Environment &Env) const override;
};

class FunctionCallStatementClass : public StatementClass {
    //std::list<std::shared_ptr<StatementClass>> Statements;
    //std::list<std::shared_ptr<VariableClass>> Parameters;
    //const std::string Name;
    std::shared_ptr<FunctionCallClass> Function;

public:
 //   FunctionCallStatementClass(const std::string Name_, std::list<std::shared_ptr<VariableClass>> _Parameters, std::list<std::shared_ptr<StatementClass>> _Statements) :
 //       Name(Name_), Statements(_Statements), Parameters(_Parameters) {}
    FunctionCallStatementClass(std::shared_ptr<FunctionCallClass> f) :
        Function(f) {}

    virtual                  ~FunctionCallStatementClass()  override{}
    virtual void              Print(std::ostream &s) const override;// = 0;
    virtual std::shared_ptr<StatementClass> Clone() const override;// = 0;
    virtual std::shared_ptr<StatementClass> Optimize(Environment &Env) override;// = 0;
    virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const override;

};

class PrintStatementClass : public StatementClass {

    std::vector<std::shared_ptr<ExpressionClass>> Expressions;

public:
    PrintStatementClass(const std::vector<std::shared_ptr<ExpressionClass>> &Expressions_) : Expressions(Expressions_) {}

    virtual                  ~PrintStatementClass()  override {}
    virtual void              Print(std::ostream &s) const override;// = 0;
    virtual std::shared_ptr<StatementClass> Clone() const override;// = 0;
    virtual std::shared_ptr<StatementClass> Optimize(Environment &Env) override;// = 0;
    virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const override;

    // StatementClass interface
public:
    virtual void Execute(Environment &Env) const override;
};

class ErrorStatement : public StatementClass {

public:
    //   FunctionCallStatementClass(const std::string Name_, std::list<std::shared_ptr<VariableClass>> _Parameters, std::list<std::shared_ptr<StatementClass>> _Statements) :
    //       Name(Name_), Statements(_Statements), Parameters(_Parameters) {}
    ErrorStatement() {}

    virtual                  ~ErrorStatement() override {}
    virtual void              Print(std::ostream &s) const override;// = 0;
    virtual std::shared_ptr<StatementClass> Clone() const override;// = 0;
    virtual std::shared_ptr<StatementClass> Optimize(Environment &Env) override;// = 0;
    virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const override;

};



