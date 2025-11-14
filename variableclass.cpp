#include "variableclass.h"
#include "Errclass.hpp"
#include "compact.h"
#include "varmanag.hpp"




void VariableClass::SetContext(VariableContextClass *Context)
{
    if (MyContext != nullptr) {
        throw ERROR_OBJECT("Changing context of variable is not allowed");
    }
    MyContext = Context;
}

Variables::VariableContentClass VariableClass::GetValue() const
{
    return Content;
}

void VariableClass::SetValue(Variables::VariableContentClass v)
{
    Content = v;
}

void VariableClass::Print(std::ostream &s)
{
    s << Content;
}

void VariableClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
    s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> " << Name << "\\n\\<" << MyContext->GetName() << "\\>|<f2> \"];" << std::endl;
}

const TypeDescriptorClass &VariableClass::GetType() const
{
    return Content.getType();
}


Variables::VariableContentClass DoubleVariableClass::GetValue() const
{
    Variables::VariableContentClass C = TypeDescriptorClass(TypeDescriptorClass::Type::Float);
    return C;
}

void DoubleVariableClass::SetValue(Variables::VariableContentClass v)
{
    Value = v.GetValue<double>();
}

DoubleVariableClass::DoubleVariableClass(const std::string &Name_, double Value) : VariableClass(Name_, TypeDescriptorClass(TypeDescriptorClass::Type::Float)),
    Value(Value)
{

}


namespace Variables {
FunctionDefinitionClass::FunctionDefinitionClass(const std::string &Name_, const std::list<std::shared_ptr<VariableClass> > &Parameters, const std::list<std::shared_ptr<StatementClass> > &Statements)
    : Parameters(Parameters), Statements(Statements), Name(Name_)
{}

void FunctionDefinitionClass::Print(std::ostream &s) const
{
    {
        bool first = true;
        s << Name;
        s << "(";
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
    }

}

void FunctionDefinitionClass::DrawDeclarationNode(std::ostream &s, int MyNodeNumber) const
{
    s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> function\\n"
      << Name << "(";
        for (auto const &p: Parameters) {
         s << p->GetName() << ", ";
    }
    s  << ")|<f2> \"];" << std::endl;
}

void FunctionDefinitionClass::DrawDefinitionNode(std::ostream &s, int MyNodeNumber) const
{
    DrawDeclarationNode(s, MyNodeNumber);
    s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> call |<f2> \"];" << std::endl;
    DrawStatementNodeList(Statements, s, MyNodeNumber);

}


ArrayClass::ArrayClass(const ArrayContentType &r) : Data(r)
{
    CommonInitialization();
}

void ArrayClass::CommonInitialization()
{
    bool SizeMismatch = false;
    Dimensions.clear();
    TypeDescriptorClass Type(TypeDescriptorClass::Type::Undefined);
    DetectArrayStructure(Data, Dimensions, Type, SizeMismatch, 0);
    if (SizeMismatch) {
        FillUpMissingElements(Data, Dimensions, VariableContentClass(0.0), 0);
    }
}

TypeDescriptorClass ArrayClass::GetTypeDescriptor() const
{
   // ArrayDescriptorClass Arraydescriptor;

  // TypeDescriptor Descriptor =
}

void ArrayClass::PrintDimensions(std::ostream &s) const
{
    s << "<";
    for (auto n: Dimensions) {
        s << n << ",";
    }
    s << ">";
}

void ArrayClass::DetectArrayStructure(const ArrayContent &Data, std::vector<uint32_t> &Dimensions, TypeDescriptorClass &ContentType, bool &SizeMissmatch, int Deepth)
{
    if (std::holds_alternative<VectorOfRows>(Data)) {
        VectorOfRows const &Rows = std::get<VectorOfRows>(Data);
        if (Deepth >= Dimensions.size()) {
            Dimensions.push_back(Rows.Size());
        } else if (Dimensions[Deepth] < Rows.Size()) {
            Dimensions[Deepth] = Rows.Size();
            SizeMissmatch = true;
        } else if (Dimensions[Deepth] > Rows.Size()) {
            SizeMissmatch = true;
        }
        for (auto const &r: Rows.Data) {
            DetectArrayStructure(r->Data, Dimensions, ContentType, SizeMissmatch, Deepth + 1);
        }
    } else if(std::holds_alternative<Row>(Data)) {
        Row const &Rows = std::get<Row>(Data);
        if (Deepth >= Dimensions.size()) {
            Dimensions.push_back(Rows.Size());
        } else if (Dimensions[Deepth] < Rows.Size()) {
            Dimensions[Deepth] = Rows.Size();
        }
        Rows.GetCommonType(ContentType);
    } else {
        throw ERROR_OBJECT("Arrayclass content invalid");
    }
}

void ArrayClass::FillUpMissingElements(ArrayContent &Data, std::vector<uint32_t> const &Dimensions, const VariableContentClass &FillValue, int Deepth)
{
    if (std::holds_alternative<VectorOfRows>(Data)) {
        VectorOfRows &Rows = std::get<VectorOfRows>(Data);
        if (Deepth >= Dimensions.size()) {
            throw ERROR_OBJECT("Arrayclass inconsistent Dimension");
        } else if (Dimensions[Deepth] < Rows.Size()) {
            throw ERROR_OBJECT("Arrayclass inconsistent Size");
        } else while (Dimensions[Deepth] > Rows.Size()) {
           if (Dimensions.size() == Deepth-1) {
               Rows.AppendElement(ArrayClass::CreateRowOfValues());
           } else {
               Rows.AppendElement(ArrayClass::CreateRowOfRows());
           }
        }
        for (auto const &r: Rows.Data) {
            FillUpMissingElements(r->Data, Dimensions, FillValue, Deepth + 1);
        }
    } else if(std::holds_alternative<Row>(Data)) {
        Row &Rows = std::get<Row>(Data);
        if (Deepth >= Dimensions.size()) {
            throw ERROR_OBJECT("Arrayclass inconsistent Dimension");
        } else while (Dimensions[Deepth] > Rows.Size()) {
            Rows.AppendElement(FillValue);
        }
    } else {
        throw ERROR_OBJECT("Arrayclass content invalid");
    }
}


template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

std::ostream &operator <<(std::ostream &s, const VariableContentClass &v)
{
    std::visit(overloaded{
                   [&s](const std::monostate &arg) { s << "empty"; },
                   [&s](int64_t arg) { s << arg << ' '; },
                   [&s](double arg) { s << std::fixed << arg << ' '; },
                   [&s](const StackClass &arg) { s << "<stack>"; },
                   [&s](const ListClass &arg) { s << "<list>"; },
                   [&s](const ArrayClass &arg) { s << "<Array "; arg.PrintDimensions(s); s << ">";  },
                   [&s](const MapClass &arg) { s << "<map>"; },
                   [&s](const std::shared_ptr<ExpressionClass> &arg) { s << "<expression>\n"; arg->Print(s);  },
                   [&s](const std::shared_ptr<FunctionDefinitionClass> &arg) { s << "<function>\n"; arg->Print(s);  },
                   [&s](const std::string& arg) { s << '"' << arg << '"'; }
               }, v.Data);
    return s;
}

VariableContentClass operator +(const VariableContentClass &l, const VariableContentClass &r)
{
    VariableContentClass Result = VariableContentClass::MakeUndefined();
    std::visit(overloaded{

              [&Result](int64_t arg1, int64_t arg2) { Result = VariableContentClass(arg1 + arg2); },
              [&Result](double arg1, double arg2)   { Result = VariableContentClass(arg1 + arg2); },
              [&Result](auto &arg1, auto &arg2) { } // All other cases: do nothing
               }, l.Data, r.Data);
    return Result;
}

bool ArrayClass::Row::GetCommonType(TypeDescriptorClass &Type) const
{
    for(auto const &e: Data) {
        if (e->getType() == TypeDescriptorClass::Type::Illegal) {
           //TypesMatching = false;
           Type = TypeDescriptorClass(TypeDescriptorClass::Type::Illegal);
           // No need to check further
           return false;
        } else if (Type == TypeDescriptorClass::Type::Undefined) {
            Type = e->getType();
        } else if (Type == TypeDescriptorClass::Type::Integer) {
            if (e->getType() == TypeDescriptorClass::Type::Integer) {
               // Do nothing
            } else if (e->getType() == TypeDescriptorClass::Type::Float) {
                Type = e->getType();
            } else {
                //TypesMatching = false;
                Type = TypeDescriptorClass(TypeDescriptorClass::Type::Illegal);
                // No need to check further
                return false;
            }
        } else if (Type == TypeDescriptorClass::Type::Float) {
            if (e->getType() == TypeDescriptorClass::Type::Float) {
                // Do nothing
            } else if (e->getType() == TypeDescriptorClass::Type::Integer) {
                // Do nothing
            } else {
                //TypesMatching = false;
                Type = TypeDescriptorClass(TypeDescriptorClass::Type::Illegal);
                // No need to check further
                return false;
            }
        } else if (e->getType() == TypeDescriptorClass::Type::Array) {
            //TypesMatching = false;
            Type = TypeDescriptorClass(TypeDescriptorClass::Type::Illegal);
            // No need to check further
            return false;
        } else if (e->getType() == TypeDescriptorClass::Type::Function) {
            //TypesMatching = false;
            Type = TypeDescriptorClass(TypeDescriptorClass::Type::Illegal);
            // No need to check further
            return false;
        } else if (e->getType() == TypeDescriptorClass::Type::Dynamic) {
            //TypesMatching = false;
            Type = TypeDescriptorClass(TypeDescriptorClass::Type::Illegal);
            // No need to check further
            return false;

        } else if (Type == e->getType()) {
            // do nothing
        } else {
            //TypesMatching = false;
            Type = TypeDescriptorClass(TypeDescriptorClass::Type::Illegal);
            // No need to check further
            return false;
        }
    }
    return true;
}

}


TypeDescriptorClass CommonType(const TypeDescriptorClass &t1, const TypeDescriptorClass &t2)
{
    using Type = TypeDescriptorClass::Type;

    // List dominates everything
    if (t1.MyType == Type::List) {
        return t1;
    }
    if (t2.MyType == Type::List) {
        return t2;
    }

    if (t1.MyType == t2.MyType) {
        switch(t1.MyType) {

        case Type::Undefined:
        case Type::Integer:
        case Type::Float:
        case Type::Bool:
        case Type::String:
        case Type::List:
        case Type::Map:
        case Type::Expression:
        case Type::Illegal:
        case Type::Function: return t1;
        case Type::Stack:   {
            auto tc = CommonType(*(std::get<StackDescriptorClass>(t1.Descriptor).BaseType), *(std::get<StackDescriptorClass>(t2.Descriptor).BaseType));
            if (tc.MyType == Type::Undefined) {
                return tc;
            } else {
                // Use common basetype
                auto rt = t1;
                rt.MyType = tc.MyType;
                return rt;
            }
        }
            break;

        case Type::Array:   {auto tc = CommonType(*(std::get<ArrayDescriptorClass>(t1.Descriptor).BaseType), *(std::get<ArrayDescriptorClass>(t2.Descriptor).BaseType));
            if (tc.MyType == Type::Undefined) {
                return tc;
            } else if (std::get<ArrayDescriptorClass>(t1.Descriptor).Dimensions == std::get<ArrayDescriptorClass>(t2.Descriptor).Dimensions) {
                // Use common basetype
                auto rt = t1;
                rt.MyType = tc.MyType;
                return rt;
            } else {
                // dimensions differ
                return TypeDescriptorClass(Type::Undefined);
            }
        }
        break;


        case Type::Dynamic:  return CommonType(*(std::get<DynamicDescriptorClass>(t1.Descriptor).CurrentType), *(std::get<DynamicDescriptorClass>(t2.Descriptor).CurrentType));

        }
    }
    if ((t1.MyType == Type::Integer) && (t2.MyType == Type::Float)) {
        return t2;
    }
    if ((t2.MyType == Type::Integer) && (t1.MyType == Type::Float)) {
        return t1;
    }
    return TypeDescriptorClass(Type::Undefined);
}
ArrayDescriptorClass::ArrayDescriptorClass(
    std::vector<int> Dimensions, std::unique_ptr<TypeDescriptorClass> BaseType)
    : Dimensions(std::move(Dimensions)), BaseType(std::move(BaseType)) {}

void TypeDescriptorClass::ChangeDynamicType(const TypeDescriptorClass &NewType)
{
    if (MyType != Type::Dynamic) {
        throw ERROR_OBJECT("Cannot change type");
    }
    if (NewType.MyType == Type::Dynamic) {
        Descriptor = NewType.Descriptor;
    } else {
        *(std::get<DynamicDescriptorClass>(Descriptor).CurrentType) = NewType;
    }
}

const TypeDescriptorClass &TypeDescriptorClass::GetDynamicType() const
{
    return *(std::get<DynamicDescriptorClass>(Descriptor).CurrentType);
}
