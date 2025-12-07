#include <iostream>
#include <list>
#include <memory>
#include "environment.hpp"
#include "variableclass.h"
#include "location.hh"

extern int GetNextNodeNumber();
extern void RestartNodeNumber();
extern void DrawStatementNodeList(const std::list<std::shared_ptr<StatementClass>> &Statements, std::ostream &os, int ParentNodeNumber);


typedef yy::location  LocationType;

static inline LocationType operator | (LocationType const &o1, LocationType const &o2)
{
    LocationType Result;
    if (o1.begin.line < o2.begin.line) {
        Result.begin = o1.begin;
    } else if (o1.begin.line > o2.begin.line) {
        Result.begin = o2.begin;
    } else if (o1.begin.column < o2.begin.column) {
        Result.begin = o1.begin;
    } else {
        Result.begin = o2.begin;
    }
    if (o1.end.line > o2.end.line) {
        Result.end = o1.end;
    } else if (o1.end.line < o2.end.line) {
        Result.end = o2.end;
    } else if (o1.end.column > o2.end.column) {
        Result.end = o1.end;
    } else {
        Result.end = o2.end;
    }
    return Result;
}

class VariableClasse_no_longer_used {
   std::string Name;
   double      Value;

   public:

               VariableClasse_no_longer_used(std::string N, double V) : Name(N), Value(V) {}
   std::string GetName()  const {return Name; }
   double      GetValue() const {return Value; }
   void        SetValue(double v) {Value = v; }
};

typedef  std::shared_ptr<VariableClass> VariableReferenceType;

class ExpressionClass : public std::enable_shared_from_this<ExpressionClass>{
   public:
    ExpressionClass(const LocationType &Loc) : Location(Loc) {}
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

   const LocationType &GetLocation() const {return Location;}
   const LocationType GetMergedLocation() const {return MergeLocations();}

   private:
   virtual const TypeDescriptorClass   GetType() const = 0;
   virtual const LocationType MergeLocations() const {return Location;}
       const LocationType Location;

};

class ValueClass : public ExpressionClass {

   public:
                            ValueClass(const LocationType &Loc) : ExpressionClass(Loc) {}
   virtual                 ~ValueClass() override{}
 //                           virtual Variables::VariableContentClass &GetWriteReferenceToContent();
};

class UnaryOperationClass : public ExpressionClass {

   public:
   //protected:
   std::shared_ptr<ExpressionClass>Operand;

   public:
   UnaryOperationClass(const UnaryOperationClass &) = default;
   UnaryOperationClass(UnaryOperationClass &&) = delete;
   UnaryOperationClass &operator=(const UnaryOperationClass &) = delete;
   UnaryOperationClass &operator=(UnaryOperationClass &&) = delete;
   UnaryOperationClass(std::shared_ptr<ExpressionClass> e,
                       const LocationType &Loc)
       : ExpressionClass(Loc), Operand(e) {}
   // UnaryOperationClass(std::shared_ptr<ExpressionClass>e, const LocatonType
   // &Loc) : ExpressionClass(Loc), Operand(e) {}
   virtual ~UnaryOperationClass() override { /* delete Operand;*/ }
   virtual bool IsConstant() override { return Operand->IsConstant(); }
   virtual bool
   IsSame(std::shared_ptr<ExpressionClass> Other) override; // = 0;

   // ExpressionClass interface
   private:
   virtual const TypeDescriptorClass GetType() const override;
   virtual const LocationType MergeLocations() const override {return GetLocation() | Operand->GetMergedLocation();}
};

class BinaryOperationClass : public ExpressionClass {

   public:
   //protected:
   std::shared_ptr<ExpressionClass>LeftOperand;
   std::shared_ptr<ExpressionClass>RightOperand;

   public:
   BinaryOperationClass(const BinaryOperationClass &) = default;
   BinaryOperationClass(BinaryOperationClass &&) = delete;
   BinaryOperationClass &operator=(const BinaryOperationClass &) = delete;
   BinaryOperationClass &operator=(BinaryOperationClass &&) = delete;
   BinaryOperationClass(std::shared_ptr<ExpressionClass> l,
                        std::shared_ptr<ExpressionClass> r,
                        const LocationType &Loc)
       : ExpressionClass(Loc), LeftOperand(l), RightOperand(r) {}
   virtual ~BinaryOperationClass()
       override { /*delete LeftOperand; delete RightOperand;*/ }
   virtual bool IsConstant() override {
       return LeftOperand->IsConstant() && RightOperand->IsConstant();
   }
   virtual bool              IsSame(std::shared_ptr<ExpressionClass>Other) override;// = 0;

   private:
   virtual const TypeDescriptorClass GetType() const override;
   virtual const LocationType MergeLocations() const override {return GetLocation() | LeftOperand->GetMergedLocation() | RightOperand->GetMergedLocation();}
};

class ConstantClass : public ValueClass {
   Variables::VariableContentClass Value;

   public:
   ConstantClass(const ConstantClass &) = default;
   ConstantClass(ConstantClass &&) = delete;
   ConstantClass &operator=(const ConstantClass &) = delete;
   ConstantClass &operator=(ConstantClass &&) = delete;
   ConstantClass(Variables::VariableContentClass v, const LocationType &Loc)
       : ValueClass(Loc), Value(v) {}

   virtual ~ConstantClass() override {}
   virtual Variables::VariableContentClass
   Evaluate(Environment &Env) const override {
       (void)Env;
       return Value;
   }
   virtual std::shared_ptr<ExpressionClass> Derive(VariableReferenceType ToDerive) const override { (void)ToDerive; return std::make_shared<ConstantClass>(0.0, GetLocation()); }
   virtual void              Print(std::ostream &s) const override { s << Value; }
   virtual std::shared_ptr<ExpressionClass> Clone() const override { return std::make_shared<ConstantClass>(*this); }
   virtual std::shared_ptr<ExpressionClass> Optimize(Environment &Env) override { (void)Env; return shared_from_this(); }
   virtual bool              IsConstant() override {return true;}
   virtual bool              IsSame(std::shared_ptr<ExpressionClass>Other) override;// = 0;
   virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const override;
 //  virtual Variables::VariableContentClass &GetWriteReferenceToContent() override;

   private:
   virtual const TypeDescriptorClass GetType() const override;
};

class WritableValueClass : public ValueClass {

public:
    enum ModeType {IfNotExistDoNotCreate, IfNotExistCreateIfPossible};
    using ValueClass::ValueClass;
    virtual VariableReferenceType GetWriteReferenceToContent(ModeType Mode) = 0;
    virtual const std::string &GetName() const = 0;
};

class VariableValueClass : public WritableValueClass {
   const VariableReferenceType Val;

   public:
   VariableValueClass(VariableReferenceType v, const LocationType &Loc) : WritableValueClass(Loc), Val(v) {if(v==nullptr){throw INTERNAL_ERROR_OBJECT("VariableValueClass(nullptr)");}}
                             VariableValueClass(const VariableValueClass &v, const LocationType &Loc) : WritableValueClass(Loc), Val(v.Val) {}
   virtual                  ~VariableValueClass() override {}
   virtual Variables::VariableContentClass  Evaluate(Environment &Env) const override{ (void)Env; return Val->GetValue().Isempty()?Variables::VariableContentClass(std::const_pointer_cast<ExpressionClass>(shared_from_this())):Val->GetValue(); }
   virtual std::shared_ptr<ExpressionClass> Derive(VariableReferenceType ToDerive) const override { if (ToDerive == Val) {return std::make_shared<ConstantClass>(1.0, GetLocation());} else {return std::make_shared<ConstantClass>(0.0, GetLocation());}}
   virtual void              Print(std::ostream &s) const override { s << Val->GetName(); }
   virtual std::shared_ptr<ExpressionClass> Clone() const override { return std::make_shared<VariableValueClass>(*this); }
   virtual std::shared_ptr<ExpressionClass> Optimize(Environment &Env) override { (void)Env; return shared_from_this(); }
   virtual bool              IsConstant() override {return false;}
   virtual bool              IsSame(std::shared_ptr<ExpressionClass>Other) override;// = 0;
   virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const override;
   virtual VariableReferenceType GetWriteReferenceToContent(ModeType Mode) override;

   private:
   virtual const TypeDescriptorClass GetType() const override;
   virtual const std::string &GetName() const override {return Val->GetName();}

};

class IndexExpressionClass {
public:
    virtual void              Print(std::ostream &s) const = 0;
    virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const = 0;
    virtual Variables::SingleElementSelectorType GetIndex() const = 0;
  //      struct IndexRangeType {IndexType From; IndexType To;};
  //  typedef std::variant<IndexType, IndexRangeType> SingleElementSelectorType;
  //  typedef std::vector<SingleElementSelectorType> ElementSelectorType;

};

class SingleIndexExpressionClass : public IndexExpressionClass{
    std::shared_ptr<ExpressionClass> Index;
public:
    explicit SingleIndexExpressionClass(std::shared_ptr<ExpressionClass> Index) : Index(std::move(Index)) {}
    virtual ~SingleIndexExpressionClass() {}
    virtual void Print(std::ostream &s) const override;
    virtual void DrawNode(std::ostream &s, int MyNodeNumber) const override;
    virtual Variables::SingleElementSelectorType GetIndex() const override {
        Variables::VariableContentClass Value = Index->Evaluate();
        if (Value.holds_alternative<int64_t>()) {
           int64_t t = Value.GetValue<int64_t>();
           if (t < 0) {
               return t;
           } else {
              return uint64_t(t);
           }
        } else if (Value.holds_alternative<std::string>()) {
            return Value.GetValue<std::string>();
        } else {
            throw RuntimeErrorClass("Illegal type in index...");
        }
    }
};

class RangedIndexExpressionClass : public IndexExpressionClass{
    std::shared_ptr<ExpressionClass> FromIndex;
    std::shared_ptr<ExpressionClass> ToIndex;
public:
    RangedIndexExpressionClass(std::shared_ptr<ExpressionClass> FromIndex,
                               std::shared_ptr<ExpressionClass> ToIndex)
        : FromIndex(std::move(FromIndex)), ToIndex(std::move(ToIndex)) {}
    virtual ~RangedIndexExpressionClass() {}
    virtual void Print(std::ostream &s) const override;
    virtual void DrawNode(std::ostream &s, int MyNodeNumber) const override;
    virtual Variables::SingleElementSelectorType GetIndex() const override;
};

typedef std::vector<std::shared_ptr<IndexExpressionClass>> IndexList;

class IndexedValueClass : public WritableValueClass {

public:
    //typedef std::vector<std::shared_ptr<IndexExpressionClass>> IndexList;

    IndexedValueClass(std::shared_ptr<WritableValueClass> IndexedValue_, IndexList Indices_, const LocationType &Loc) : WritableValueClass(Loc), Indices(Indices_), IndexedValue(IndexedValue_) {}
    IndexedValueClass(std::shared_ptr<WritableValueClass> IndexedValue_, std::shared_ptr<ExpressionClass> Indices_, const LocationType &Loc) : WritableValueClass(Loc), Indices(Indices_), IndexedValue(IndexedValue_) {}
   //( IndexedValueClass(const IndexedValueClass &v, const LocationType &Loc) : ValueClass(Loc), Val(v.Val) {}
    virtual                  ~IndexedValueClass() override {}
    virtual Variables::VariableContentClass  Evaluate(Environment &Env) const override;
    virtual std::shared_ptr<ExpressionClass> Derive(VariableReferenceType ToDerive) const override { (void)ToDerive; throw RuntimeErrorClass("cannot derive indexed expressions...");}
    virtual void              Print(std::ostream &s) const override;
    virtual std::shared_ptr<ExpressionClass> Clone() const override { return std::make_shared<IndexedValueClass>(*this); }
    virtual std::shared_ptr<ExpressionClass> Optimize(Environment &Env) override { (void)Env; return shared_from_this(); }
    virtual bool              IsConstant() override {return false;}
    virtual bool              IsSame(std::shared_ptr<ExpressionClass>Other) override;// = 0;
    virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const override;
    virtual VariableReferenceType GetWriteReferenceToContent(ModeType Mode) override;

private:
    virtual const TypeDescriptorClass GetType() const override {return  IndexedValue->Type();}
    virtual const std::string &GetName() const override {return IndexedValue->GetName();}

    std::variant<IndexList, std::shared_ptr<ExpressionClass>> Indices;
    std::shared_ptr<WritableValueClass> IndexedValue;

    Variables::ElementSelectorType BuildSelector() const;
};

class FunctionClass;

class FunctionCallClass : public ValueClass {
    std::shared_ptr<Variables::FunctionDefinitionClass> TheFunction;
    std::list<std::shared_ptr<StatementClass>> Assignements;
   // std::vector<Variables::VariableContentClass> StorageTemplate;

public:
    FunctionCallClass(std::shared_ptr<Variables::FunctionDefinitionClass> f, std::list<std::shared_ptr<StatementClass>> a, const LocationType &Loc) : ValueClass(Loc), TheFunction(f), Assignements(a) {}
    FunctionCallClass(const FunctionCallClass &f) = default;
    virtual                  ~FunctionCallClass() override {}
    virtual Variables::VariableContentClass  Evaluate(Environment &Env) const override;//Val->GetValue(); }
    virtual std::shared_ptr<ExpressionClass> Derive(VariableReferenceType ToDerive) const override { if (ToDerive == ToDerive) {return std::make_shared<ConstantClass>(1.0, GetLocation());} else {return std::make_shared<ConstantClass>(0.0, GetLocation());}}
    virtual void              Print(std::ostream &s) const override;
    virtual std::shared_ptr<ExpressionClass> Clone() const override { return std::make_shared<FunctionCallClass>(*this); }
    virtual std::shared_ptr<ExpressionClass> Optimize(Environment &Env) override { (void)Env; return shared_from_this(); }
    virtual bool              IsConstant() override {return false;}
    virtual bool              IsSame(std::shared_ptr<ExpressionClass>Other) override;// = 0;
    virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const override;

    const std::string &GetName() {return TheFunction->GetName();}
  //  Variables::VariableContentClass &GetContentForOffset(uint32_t Offset);

private:
    virtual const TypeDescriptorClass GetType() const override;
};


class InverseClass : public UnaryOperationClass {

   public:
                             InverseClass(std::shared_ptr<ExpressionClass>e, const LocationType &Loc) : UnaryOperationClass(e, Loc) {}
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
                             SquareClass(std::shared_ptr<ExpressionClass>e, const LocationType &Loc) : UnaryOperationClass(e, Loc) {}
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
                             NegationClass(std::shared_ptr<ExpressionClass>e, const LocationType &Loc) : UnaryOperationClass(e, Loc) {}
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
                             LogarithmClass(std::shared_ptr<ExpressionClass>e, const LocationType &Loc) : UnaryOperationClass(e, Loc) {}
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
                             ExponentialClass(std::shared_ptr<ExpressionClass>e, const LocationType &Loc) : UnaryOperationClass(e, Loc) {}
                             ExponentialClass(const ExponentialClass &v) :  UnaryOperationClass(v) {}
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
    SquareRootClass(std::shared_ptr<ExpressionClass>e, const LocationType &Loc) : UnaryOperationClass(e, Loc) {}
                             SquareRootClass(const SquareRootClass &v) :  UnaryOperationClass(v) {}
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
                             PowerClass(std::shared_ptr<ExpressionClass>e1, std::shared_ptr<ExpressionClass>e2, const LocationType &Loc) : BinaryOperationClass(e1, e2, Loc) {}
                             PowerClass(const PowerClass &v) :  BinaryOperationClass(v) {}
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
                             MultiplyClass(std::shared_ptr<ExpressionClass>e1, std::shared_ptr<ExpressionClass>e2, const LocationType &Loc) : BinaryOperationClass(e1, e2, Loc) {}
                             MultiplyClass(const MultiplyClass &v) :  BinaryOperationClass(v) {}
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
                             AdditionClass(std::shared_ptr<ExpressionClass>e1, std::shared_ptr<ExpressionClass>e2, const LocationType &Loc) : BinaryOperationClass(e1, e2, Loc) {}
                             AdditionClass(const AdditionClass &v) :  BinaryOperationClass(v) {}
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
    explicit ConditionalExpressionClass(LocationType const &Location)
        : Location(Location) {}
    ConditionalExpressionClass(const ConditionalExpressionClass &) = default;
    ConditionalExpressionClass(ConditionalExpressionClass &&) = delete;
    ConditionalExpressionClass &
    operator=(const ConditionalExpressionClass &) = delete;
    ConditionalExpressionClass &operator=(ConditionalExpressionClass &&) = delete;
    virtual ~ConditionalExpressionClass() {}
    virtual bool Evaluate(Environment &Env) const;                     // = 0;
    virtual void Print(std::ostream &s) const;                         // = 0;
    virtual std::shared_ptr<ConditionalExpressionClass> Clone() const; // = 0;
    virtual std::shared_ptr<ConditionalExpressionClass>
    Optimize(Environment &Env); // = 0;
    virtual bool IsConstant();  // = 0;
    virtual bool
    IsSame(std::shared_ptr<ConditionalExpressionClass> Other); // = 0;
    virtual void DrawNode(std::ostream &s, int MyNodeNumber) const;
    const LocationType &GetLocation() const { return Location; }
    const LocationType GetMergedLocation() const {return MergeLocations();}

private:
   // virtual const TypeDescriptorClass GetType() const = 0;
    const LocationType Location;
    virtual const LocationType MergeLocations() const {return Location;}

};

class BinaryConditionalOperationClass : public ConditionalExpressionClass {

public:
    //protected:
    std::shared_ptr<ConditionalExpressionClass>LeftOperand;
    std::shared_ptr<ConditionalExpressionClass>RightOperand;

public:
    BinaryConditionalOperationClass(std::shared_ptr<ConditionalExpressionClass>l, std::shared_ptr<ConditionalExpressionClass>r, const LocationType &Loc) : ConditionalExpressionClass(Loc), LeftOperand(l), RightOperand(r) {}
    virtual                  ~BinaryConditionalOperationClass() override {/*delete LeftOperand; delete RightOperand;*/}
    virtual bool              IsConstant() override {return LeftOperand->IsConstant()&&RightOperand->IsConstant();}
    virtual bool              IsSame(std::shared_ptr<ConditionalExpressionClass>Other) override;// = 0;
private:
    virtual const LocationType MergeLocations() const override {return GetLocation() | LeftOperand->GetMergedLocation() | RightOperand->GetMergedLocation();}
};


class AndClass : public BinaryConditionalOperationClass {

public:
    AndClass(std::shared_ptr<ConditionalExpressionClass>e1, std::shared_ptr<ConditionalExpressionClass>e2, const LocationType &Loc) : BinaryConditionalOperationClass(e1, e2, Loc) {}
    AndClass(const AndClass &v) :  BinaryConditionalOperationClass(v) {}
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
    BinaryRelationalOperationClass(std::shared_ptr<ExpressionClass>l, std::shared_ptr<ExpressionClass>r, const LocationType &Loc) : ConditionalExpressionClass(Loc), LeftOperand(l), RightOperand(r) {}
    virtual                  ~BinaryRelationalOperationClass() override {/*delete LeftOperand; delete RightOperand;*/}
    virtual bool              IsConstant() override {return LeftOperand->IsConstant()&&RightOperand->IsConstant();}
    virtual bool              IsSame(std::shared_ptr<ConditionalExpressionClass>Other) override;// = 0;

private:
    virtual const LocationType MergeLocations() const override {return GetLocation() | LeftOperand->GetMergedLocation() | RightOperand->GetMergedLocation();}
};

class LessThanClass : public BinaryRelationalOperationClass {

public:
    LessThanClass(std::shared_ptr<ExpressionClass>e1, std::shared_ptr<ExpressionClass>e2, const LocationType &Loc) : BinaryRelationalOperationClass(e1, e2, Loc) {}
    LessThanClass(const LessThanClass &v) :  BinaryRelationalOperationClass(v) {}
    virtual                  ~LessThanClass() override {}
    virtual bool              Evaluate(Environment &Env) const override;//{return LeftOperand->Evaluate(Env) + RigthOperand->Evaluate(Env); };
    virtual void              Print(std::ostream &s) const override;//{ s << "("; LeftOperand->Print(s); s << ") * ("; RigthOperand->Print(s); s << ")"; };
    virtual std::shared_ptr<ConditionalExpressionClass> Clone() const override;//{return new AdditionClass(*this); };
    virtual std::shared_ptr<ConditionalExpressionClass> Optimize(Environment &Env) override;// { return NULL; };
    virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const override;
};


class StatementClass : public std::enable_shared_from_this<StatementClass>{
public:
    explicit StatementClass(const LocationType &Location) : Location(Location) {}
    virtual ~StatementClass() {}
    virtual void Print(std::ostream &s) const;                          // = 0;
    virtual std::shared_ptr<StatementClass> Clone() const;              // = 0;
    virtual std::shared_ptr<StatementClass> Optimize(Environment &Env); // = 0;
    virtual void DrawNode(std::ostream &s, int MyNodeNumber) const;
    virtual void Execute(Environment &Env) const; // = 0;
    const LocationType &GetLocation() const { return Location; }

private:
    const LocationType Location;
};

class AssignementClass : public StatementClass {
    std::shared_ptr<ExpressionClass> AssignedExpression;
  //  const std::shared_ptr<VariableClass> Variable;
    std::shared_ptr<WritableValueClass> Variable;

public:
    AssignementClass(std::shared_ptr<ExpressionClass> _AssignedExpression, const std::shared_ptr<WritableValueClass> _Variable, const LocationType &Loc) :
        StatementClass(Loc), AssignedExpression(_AssignedExpression), Variable(_Variable) {}

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
    RepeatLoopClass(std::list<std::shared_ptr<StatementClass>> _Statements, std::shared_ptr<ConditionalExpressionClass> _Condition, const LocationType &Loc) :
        StatementClass(Loc), Statements(_Statements), Condition(_Condition) {}

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
    FunctionCallStatementClass(std::shared_ptr<FunctionCallClass> f, const LocationType &Loc) :
        StatementClass(Loc), Function(f) {}

    virtual                  ~FunctionCallStatementClass()  override{}
    virtual void              Print(std::ostream &s) const override;// = 0;
    virtual std::shared_ptr<StatementClass> Clone() const override;// = 0;
    virtual std::shared_ptr<StatementClass> Optimize(Environment &Env) override;// = 0;
    virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const override;


    // StatementClass interface
public:
    virtual void Execute(Environment &Env) const override;
};



class PrintStatementClass : public StatementClass {

    std::vector<std::shared_ptr<ExpressionClass>> Expressions;

public:
    PrintStatementClass(const std::vector<std::shared_ptr<ExpressionClass>> &Expressions_, const LocationType &Loc) : StatementClass(Loc), Expressions(Expressions_) {}

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
    ErrorStatement(const LocationType &Loc) : StatementClass(Loc) {}

    virtual                  ~ErrorStatement() override {}
    virtual void              Print(std::ostream &s) const override;// = 0;
    virtual std::shared_ptr<StatementClass> Clone() const override;// = 0;
    virtual std::shared_ptr<StatementClass> Optimize(Environment &Env) override;// = 0;
    virtual void              DrawNode(std::ostream &s, int MyNodeNumber) const override;

};



