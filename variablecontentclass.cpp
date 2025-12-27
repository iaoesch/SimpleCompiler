#include "variableclass.h"
#include "Errclass.hpp"
#include "compact.h"
#include "varmanag.hpp"
#include "variablecontentclass.h"


#if 0
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
#endif

namespace Variables {

FunctionDefinitionBaseClass::FunctionDefinitionBaseClass(const std::string &Name_, const std::vector<std::shared_ptr<VariableClass> > &Parameters, VariableManager::LocalStorageType StorageTemplate_, LocationType const &Loc)
    : Parameters(Parameters), Name(Name_),
    StorageTemplate(std::move(StorageTemplate_)),
    //ActiveReturnVariable(nullptr),
    Location(Loc)
   {}

FunctionDefinitionBaseClass::FunctionDefinitionBaseClass(const std::string &Name_, LocationType const &Loc)
    : Parameters(), Name(Name_),
    StorageTemplate(),
    //ActiveReturnVariable(nullptr),
    Location(Loc)
{}

void FunctionDefinitionBaseClass::Set(const std::vector<std::shared_ptr<VariableClass> > &Parameters_, LocationType const &Loc)
{
    Parameters = Parameters_;
    Location += Loc;
}

void FunctionDefinitionClass::Set(const std::list<std::shared_ptr<StatementClass> > &Statements_, LocationType const &Loc)
{
    Statements = Statements_;
    Location += Loc;
}

void FunctionDefinitionBaseClass::Set(LocalStorageType StorageTemplate_, LocationType const &Loc)
{
    StorageTemplate = std::move(StorageTemplate_);
    Location += Loc;
}


void FunctionDefinitionBaseClass::Print(std::ostream &s) const
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
    }
}

void FunctionDefinitionClass::Print(std::ostream &s) const
{
    {
        FunctionDefinitionBaseClass::Print(s);
        for(auto &r: Statements) {
            r->Print(s);
        }
    }

}

void FunctionDefinitionBaseClass::DrawDeclarationNode(std::ostream &s, int MyNodeNumber) const
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

std::shared_ptr<VariableClass> FunctionDefinitionBaseClass::GetParameterByName(std::string Name)
{
    auto Var = std::find_if(Parameters.begin(), Parameters.end(), [Name](auto const &v){return v->GetName() == Name;});
    if (Var == Parameters.end()) {
        return nullptr;
    } else {
        return *Var;
    }
}

std::shared_ptr<VariableClass> FunctionDefinitionBaseClass::GetParameterByIndex(int i)
{
    size_t index = static_cast<size_t>(i);
    if(index >= Parameters.size()) {
       return nullptr;
    } else {
        return Parameters[index];
    }
}

const TypeDescriptorClass &FunctionDefinitionBaseClass::GetReturnType() const
{
    if (ReturnType == nullptr) {
        static VariableTypeDescriptorClass Undef(TypeDescriptorClass::Type::Undefined);
        return Undef;
    } else {
        return *ReturnType;
    }
}

FunctionDefinitionClass::FunctionDefinitionClass(const std::string &Name_, const std::vector<std::shared_ptr<VariableClass> > &Parameters, const std::list<std::shared_ptr<StatementClass> > &Statements, VariableManager::LocalStorageType StorageTemplate_, LocationType const &Loc)
    : FunctionDefinitionBaseClass(Name_, Parameters, std::move(StorageTemplate_), Loc)
{}

FunctionDefinitionClass::FunctionDefinitionClass(const std::string &Name_, LocationType const &Loc)
    : FunctionDefinitionBaseClass(Name_, Loc)
{}

PredefinedFunctionDefinitionClass::PredefinedFunctionDefinitionClass(const std::string &Name_, const std::vector<std::shared_ptr<VariableClass> > &Parameters, Callable *Function_, LocalStorageType StorageTemplate_, LocationType const &Loc)
    : FunctionDefinitionBaseClass(Name_, Parameters, std::move(StorageTemplate_), Loc),
    Function(Function_)
{}

PredefinedFunctionDefinitionClass::PredefinedFunctionDefinitionClass(const std::string &Name_, Callable *Function_, const LocationType &Loc)
    : FunctionDefinitionBaseClass(Name_, Loc),
    Function(Function_)
{}

void PredefinedFunctionDefinitionClass::Print(std::ostream &s) const
{
    FunctionDefinitionBaseClass::Print(s);
    s << "<internal>\n";
}

void PredefinedFunctionDefinitionClass::DrawDefinitionNode(std::ostream &s, int MyNodeNumber) const
{
    DrawDeclarationNode(s, MyNodeNumber);
    s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> call internal |<f2> \"];" << std::endl;
}


ArrayClass::ArrayClass(const ArrayContentType &r) : Data(r) , BaseType(TypeDescriptorClass::Type::Undefined)
{
    CommonInitialization();
}

ProxyVariableClass ArrayClass::GetOrCreateIndexedElement(std::string BaseName, ElementSelectorType Selector) const
{
    return GetIndexedElement(BaseName, Selector, true);
}

void ArrayClass::CommonInitialization()
{
    bool SizeMismatch = false;
    Dimensions.clear();
    ValueTypeDescriptorClass Type(TypeDescriptorClass::Type::Undefined);
    DetectArrayStructure(Data, Dimensions, Type, SizeMismatch, 0);
    if (SizeMismatch) {
        FillUpMissingElements(Data, Dimensions, VariableContentClass(0.0), 0);
    }
    BaseType = Type;
}

ValueTypeDescriptorClass ArrayClass::GetTypeDescriptor() const
{
   // ArrayDescriptorClass Arraydescriptor;

    return ValueTypeDescriptorClass(ArrayDescriptorClass(Dimensions, std::make_unique<VariableTypeDescriptorClass>(BaseType)));
}

void ArrayClass::PrintDimensions(std::ostream &s) const
{
    s << "[";
    for (auto n: Dimensions) {
        s << n << ",";
    }
    s << "]";
}
void ArrayClass::PrintDetail(const ArrayContent &Data, std::ostream &s, int &Limit, int Indent) const
{
    if (Limit <= 0) {
        return;
    }
    for (int i = 0; i < Indent; i++) {
        s << "   ";
    }
    s << "[";

    if (std::holds_alternative<VectorOfRows>(Data)) {
        VectorOfRows const &Rows = std::get<VectorOfRows>(Data);

        for (auto const &r: Rows.Data) {
            s << "\n";
            PrintDetail(r->Data, s, Limit, Indent + 3);
            if (Limit <= 0) {
                return;
            }
        }
        s << "\n";
        for (int i = 0; i < Indent; i++) {
            s << "   ";
        }
    } else if(std::holds_alternative<Row>(Data)) {
        Row const &Rows = std::get<Row>(Data);
        bool first = true;
        for (auto const &v: Rows.Data) {
            if (first == false) {
                s << ", ";
            }
            first = false;
            s << *v;
            Limit--;
            if (Limit <= 0) {
                s << "\n ... ]";
                return;
            }
        }

    } else {
        throw INTERNAL_ERROR_OBJECT("Arrayclass content invalid");
    }
    s << "]";
}

void ArrayClass::PrintDetail(std::ostream &s, int Limit) const
{
    PrintDetail(Data, s, Limit, 0);
}

std::string ElementSelectorType::ToText() const
{
    std::string Text = "[";
    for(auto &i: *this) {
        if (std::holds_alternative<ArrayIndexType>(i)) {
            ArrayIndexType SimpleSelector = std::get<ArrayIndexType>(i);
            Text.append(std::to_string(SimpleSelector));
        } else if(std::holds_alternative<IndexRangeType>(i)) {
            IndexRangeType RangeSelector = std::get<IndexRangeType>(i);
            Text.append(std::to_string(RangeSelector.From));
            Text.append(":");
            Text.append(std::to_string(RangeSelector.To));
        } else if(std::holds_alternative<MapStringIndexType>(i)) {
            Text.append("\"");
            Text.append(std::get<MapStringIndexType>(i));
            Text.append("\"");
        } else {
            // Should not happen...
            throw INTERNAL_ERROR_OBJECT("Unknown selector kind");
        }
        Text.append(",");
    }
    Text.append("]");
    return Text;
}


ProxyVariableClass ArrayClass::GetIndexedElement(std::string BaseName, ElementSelectorType Selector, bool CreateIfNeeded) const
{
    return ProxyVariableClass("@" + BaseName + Selector.ToText(), BaseType, GetIndexedElement(Selector, CreateIfNeeded), VariableClass::StorageClass::ReadAndWrite);
}

VariableContentClass &ArrayClass::GetIndexedElement(ElementSelectorType Selector, bool CreateIfNeeded) const
{
    (void) CreateIfNeeded; // Creating not existing parts not permitted yet
    if (Selector.size() != Dimensions.size()) {
        throw RuntimeErrorClass("Dimension missmatch");
    }
    const ArrayContentType *CurrentElement = &Data;
    for(auto &i: Selector) {
        if (std::holds_alternative<ArrayIndexType>(i)) {
            ArrayIndexType SimpleSelector = std::get<ArrayIndexType>(i);
            if (std::holds_alternative<Row>(*CurrentElement)) {
                return *(std::get<Row>(*CurrentElement).Data.at(SimpleSelector));
            } else if (std::holds_alternative<VectorOfRows>(*CurrentElement)) {
                CurrentElement = &(std::get<VectorOfRows>(*CurrentElement).Data.at(SimpleSelector)->Data);
            } else {
                throw INTERNAL_ERROR_OBJECT("Unknown vector element type");
            }
        } else if(std::holds_alternative<IndexRangeType>(i)) {
            throw INTERNAL_ERROR_OBJECT("Ranged indices not supported");
        } else if(std::holds_alternative<MapStringIndexType>(i)) {
            throw RuntimeErrorClass("Key indices for array not supported");
        } else {
            // Should not happen...
            throw INTERNAL_ERROR_OBJECT("Unknown selector kind");
        }
    }
    throw INTERNAL_ERROR_OBJECT("internal dimension trouble");
}

void ArrayClass::DetectArrayStructure(const ArrayContent &Data, std::vector<uint64_t> &Dimensions, ValueTypeDescriptorClass &ContentType, bool &SizeMissmatch, int Deepth)
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
        throw INTERNAL_ERROR_OBJECT("Arrayclass content invalid");
    }
}

void ArrayClass::FillUpMissingElements(ArrayContent &Data, DimensionType const &Dimensions, const VariableContentClass &FillValue, unsigned int Deepth)
{
    if (std::holds_alternative<VectorOfRows>(Data)) {
        VectorOfRows &Rows = std::get<VectorOfRows>(Data);
        if (Deepth >= Dimensions.size()) {
            throw INTERNAL_ERROR_OBJECT("Arrayclass inconsistent Dimension");
        } else if (Dimensions[Deepth] < Rows.Size()) {
            throw INTERNAL_ERROR_OBJECT("Arrayclass inconsistent Size");
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
            throw INTERNAL_ERROR_OBJECT("Arrayclass inconsistent Dimension");
        } else while (Dimensions[Deepth] > Rows.Size()) {
            Rows.AppendElement(FillValue);
        }
    } else {
        throw INTERNAL_ERROR_OBJECT("Arrayclass content invalid");
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
                   [&s](const std::monostate &arg) { (void)arg; s << "[empty]"; },
                   [&s](int64_t arg) { s << "[int:" << arg << "]"; },
                   [&s](double arg) { s << "[float:"  << std::fixed << arg << "]"; },
                   [&s](const StackClass &arg) { (void)arg; s << "<stack]"; },
                   [&s](const ListClass &arg) { (void)arg; s << "[list]"; },
                   [&s](const ArrayClass &arg) { s << "[Array "; arg.PrintDimensions(s); s << "]";  },
                   [&s](const MapClass &arg) { (void)arg; s << "[map]"; },
                   [&s](const std::shared_ptr<ExpressionClass> &arg) { s << "[expression]\n"; arg->Print(s);  },
                   [&s](const std::shared_ptr<FunctionDefinitionBaseClass> &arg) { s << "[function]\n"; arg->Print(s);  },
                   [&s](const std::shared_ptr<VariableContentClass> &arg) { s << "[varcont:" << *arg << "]"; },
                   [&s](const std::string& arg) { s << '"' << arg << '"'; },
                   [&s](const TypeDescriptorClass& arg) { s << '"' << arg << '"'; }
               }, v.Data);
    return s;
}


void VariableContentClass::PrintDetail(std::ostream &s, int Limit) const
{
         std::visit(overloaded{
                       [&s](const std::monostate &arg) { (void)arg; s << "[empty]"; },
                       [&s](int64_t arg) { s << "[int:" << arg << "]"; },
                       [&s](double arg) { s << "[float:"  << std::fixed << arg << "]"; },
                       [&s, Limit](const StackClass &arg) { s << "[Stack:"; arg.PrintDetail(s, Limit); s << "]"; },
                       [&s, Limit](const ListClass &arg) { s << "[list:"; arg.PrintDetail(s, Limit); s << "]"; },
                       [&s, Limit](const ArrayClass &arg) { s << "[Array "; arg.PrintDetail(s, Limit); s << "]";  },
                       [&s, Limit](const MapClass &arg) { s << "[map:"; arg.PrintDetail(s, Limit); s << "]"; },
                   [&s](const std::shared_ptr<ExpressionClass> &arg) { s << "[expression>\n"; auto v = arg->Evaluate(); if (!v.Isempty()) {s << v;}; arg->Print(s);  },
                       [&s](const std::shared_ptr<FunctionDefinitionBaseClass> &arg) { s << "[function>\n"; arg->Print(s);  },
                       [&s](const std::shared_ptr<VariableContentClass> &arg) { s << "[varcont:" << *arg << "]"; },
                       [&s](const std::string& arg) { s << '"' << arg << '"'; },
                       [&s](const TypeDescriptorClass& arg) { s << '"' << arg << '"'; }

                   }, Data);

}



VariableContentClass operator +(const VariableContentClass &l, const VariableContentClass &r)
{
    VariableContentClass Result = VariableContentClass::MakeUndefined();
    std::visit(overloaded{

              [&Result](int64_t arg1, int64_t arg2) { Result = VariableContentClass(arg1 + arg2); },
              [&Result](double arg1, double arg2)   { Result = VariableContentClass(arg1 + arg2); },
              [&Result](auto &arg1, auto &arg2) { (void)arg1; (void)arg2; (void)Result;} // All other cases: do nothing
               }, l.Data, r.Data);
    return Result;
}

bool ArrayClass::Row::GetCommonType(ValueTypeDescriptorClass &Type) const
{
    for(auto const &e: Data) {
        if (e->getType() == TypeDescriptorClass::Type::Illegal) {
           //TypesMatching = false;
           Type = ValueTypeDescriptorClass(TypeDescriptorClass::Type::Illegal);
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
                Type = ValueTypeDescriptorClass(TypeDescriptorClass::Type::Illegal);
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
                Type = ValueTypeDescriptorClass(TypeDescriptorClass::Type::Illegal);
                // No need to check further
                return false;
            }
        } else if (e->getType() == TypeDescriptorClass::Type::Array) {
            //TypesMatching = false;
            Type = ValueTypeDescriptorClass(TypeDescriptorClass::Type::Illegal);
            // No need to check further
            return false;
        } else if (e->getType() == TypeDescriptorClass::Type::Function) {
            //TypesMatching = false;
            Type = ValueTypeDescriptorClass(TypeDescriptorClass::Type::Illegal);
            // No need to check further
            return false;
        } else if (e->getType() == TypeDescriptorClass::Type::Dynamic) {
            //TypesMatching = false;
            Type = ValueTypeDescriptorClass(TypeDescriptorClass::Type::Illegal);
            // No need to check further
            return false;

        } else if (Type == e->getType()) {
            // do nothing
        } else {
            //TypesMatching = false;
            Type = ValueTypeDescriptorClass(TypeDescriptorClass::Type::Illegal);
            // No need to check further
            return false;
        }
    }
    return true;
}

bool operator <(const VariableContentClass &r, const VariableContentClass &l)
{
    bool Result = false;
    std::visit(overloaded{

        [&Result](int64_t arg1, int64_t arg2) { Result = arg1 < arg2; },
            [&Result](double arg1, double arg2)   { Result = arg1 < arg2; },
            [&Result](auto &arg1, auto &arg2) { (void)arg1; (void)arg2; (void)Result;} // All other cases: do nothing
    }, l.Data, r.Data);
    return Result;
}

bool operator ==(const VariableContentClass &r, const VariableContentClass &l)
{
    bool Result = false;
    std::visit(overloaded{

        [&Result](int64_t arg1, int64_t arg2) { Result = arg1 == arg2; },
        [&Result](double arg1, double arg2)   { Result = arg1 == arg2; },
        [&Result](std::string arg1, std::string arg2)   { Result = arg1 == arg2; },
        //[&Result](auto arg1, decltype(arg1) arg2)   { Result = arg1 == arg2; },
        //[&Result]<class T>(T arg1, T arg2)   { Result = arg1 == arg2; },

                   [&Result](auto &arg1, auto &arg2) {(void)arg1; (void)arg2; (void)Result; } // All other cases: do nothing
    }, l.Data, r.Data);
    return Result;
}

Variables::VariableContentClass FunctionDefinitionClass::Execute(Environment &Env) const
{
    std::shared_ptr<Variables::VariableContentClass> ReturnValue;
    std::ostringstream Output;
    Output  << "FunctionDefinitionClass::Execute '" << Name << "', Statements.size() = " << Statements.size();
    Env.Tracing(Location, Output.str());

    for (auto const &s: Statements) {
        std::cout << "executing << ";
        s->Print(std::cout);
        std::cout << ">>" << std::endl;
        auto r = s->Execute(Env);
        if (r) {
            if (std::holds_alternative<std::shared_ptr<Variables::VariableContentClass>>(*r)) {
                ReturnValue = std::get<std::shared_ptr<Variables::VariableContentClass>>(*r);
                break;
            }
        }
        std::cout << "executing done" << std::endl;
    }
    Env.Tracing(Location, "FunctionDefinitionClass::Execute '" + Name + "' done");

    if (ReturnValue != nullptr) {
        Env.OutputStream() << "Returning ";
        Env.OutputStream() << *ReturnValue;
        Env.OutputStream() << "\n";
       return *ReturnValue;
    } else {
        Env.OutputStream() << "Returning Nothing\n";
        return VariableContentClass::MakeUndefined();
    }
}

Variables::VariableContentClass PredefinedFunctionDefinitionClass::Execute(Environment &Env) const
{
    std::ostringstream Output;
    Output  << "PredefinedFunctionDefinitionClass::Execute '" << Name << "'";
    Env.Tracing(Location, Output.str());

    VariableContentClass ReturnValue = Function->Execute(ActiveStorage.back());

    Env.Tracing(Location, "FunctionDefinitionClass::Execute '" + Name + "' done");

        Env.OutputStream() << "Returning ";
        Env.OutputStream() << ReturnValue;
        Env.OutputStream() << "\n";
        return ReturnValue;

}


void StackClass::PrintDetail(std::ostream &s, int Limit) const
{
    (void)Limit;
    s << "[Detail Stack]";
}

void ListClass::PrintDetail(std::ostream &s, int Limit) const
{
    (void)Limit;
    s << "[Detail List]";
}

void SparseArrayClass::PrintDetail(std::ostream &s, int Limit) const
{
    (void)Limit;
    s << "[Detail sparse array]";

}

ValueTypeDescriptorClass MapClass::GetTypeDescriptor() const
{
        // ArrayDescriptorClass Arraydescriptor;

        return ValueTypeDescriptorClass(MapDescriptorClass(KeyType, std::make_unique<VariableTypeDescriptorClass>(BaseType)));
}

void MapClass::PrintDetail(std::ostream &s, int Limit) const
{
    (void)Limit;
    s << "[";
    if (std::holds_alternative<MapIntegerKeyType>(Data)) {
        for(auto const &e: std::get<MapIntegerKeyType>(Data)) {
            s << "<" << e.first << "=" << (*(e.second)) << ">, ";
        }
    } else if (std::holds_alternative<MapStringKeyType>(Data)) {
        for(auto const &e: std::get<MapStringKeyType>(Data)) {
            s << "<" << e.first << "=" << (*(e.second)) << ">, ";
        }
    } else if (std::holds_alternative<MapStringAndIntegerKeyType>(Data)) {
        for(auto const &e: std::get<MapStringAndIntegerKeyType>(Data)) {
            s << "<";
            if (std::holds_alternative<std::string>(e.first)) {
                s << "\"" << std::get<std::string>(e.first) << "\"" ;
            } else if (std::holds_alternative<int64_t>(e.first)) {
                s << std::get<int64_t>(e.first);
            } else {
                s << "<?>";
            }
            s << "=" << (*(e.second)) << ">, ";
        }
    } else {
        s << "<map unknown content>";
    }
    s << "]";
}

ProxyVariableClass MapClass::GetIndexedElement(std::string BaseName, ElementSelectorType Selector, bool CreateIfNeeded) const
{
    return ProxyVariableClass("@" + BaseName + Selector.ToText(), BaseType, GetIndexedElement(Selector, CreateIfNeeded), VariableClass::StorageClass::ReadAndWrite);
}

VariableContentClass &MapClass::GetIndexedElement(ElementSelectorType Selector, bool CreateIfNeeded) const
{
    if (Selector.size() != 1) {
        throw RuntimeErrorClass("Map allows only onedimensional access");
    }
    SingleElementSelectorType const &ElementSelector = Selector[0];
    if (std::holds_alternative<MapStringAndIntegerKeyType>(Data)) {
        KeyTypeUnion Key;
        if (std::holds_alternative<MapStringIndexType>(ElementSelector)) {
            Key = std::get<MapStringIndexType>(ElementSelector);
        } else if (std::holds_alternative<ArrayIndexType>(ElementSelector)) {
            Key = int64_t(std::get<ArrayIndexType>(ElementSelector));
        } else if (std::holds_alternative<MapIntegerIndexType>(ElementSelector)) {
            Key = std::get<MapIntegerIndexType>(ElementSelector);
        } else if (std::holds_alternative<IndexRangeType>(ElementSelector)) {
            throw RuntimeErrorClass("Keytype <Range> not supported for map access");
        } else {
            throw RuntimeErrorClass("Keytype <unknown> not supported for map access" + std::to_string(ElementSelector.index()) );
        }
        std::cout << "Key: union" << ":" << this  << std::endl;
        if (CreateIfNeeded) {
            auto &Element = std::get<MapStringAndIntegerKeyType>(Data)[Key];
            if (Element==nullptr) {
                Element = std::make_unique<Variables::VariableContentClass>(VariableContentClass::MakeEmpty(BaseType));
            }
            return *Element;

        } else {
           return *(std::get<MapStringAndIntegerKeyType>(Data).at(Key));
        }
    } else if (std::holds_alternative<MapIntegerKeyType>(Data)) {
        int64_t Key;
        if (std::holds_alternative<ArrayIndexType>(ElementSelector)) {
            Key = int64_t(std::get<ArrayIndexType>(ElementSelector));
        } else if (std::holds_alternative<MapIntegerIndexType>(ElementSelector)) {
            Key = std::get<MapIntegerIndexType>(ElementSelector);
        } else if (std::holds_alternative<MapStringIndexType>(ElementSelector)) {
            throw RuntimeErrorClass("Keytype <string> not supported for integer map access");
        } else if (std::holds_alternative<IndexRangeType>(ElementSelector)) {
            throw RuntimeErrorClass("Keytype <Range> not supported for integer map access");
        } else {
            throw RuntimeErrorClass("Keytype <unknown> not supported for integer map access: " + std::to_string(ElementSelector.index()) );
        }
        std::cout << "Key: " << Key << ":" << this << std::endl;
        if (CreateIfNeeded) {
            auto &Element = std::get<MapIntegerKeyType>(Data)[Key];
            if (Element==nullptr) {
                Element = std::make_unique<Variables::VariableContentClass>(VariableContentClass::MakeEmpty(BaseType));
            }
            return *Element;
        } else {
        return *(std::get<MapIntegerKeyType>(Data).at(Key));
        }
    } else if (std::holds_alternative<MapStringKeyType>(Data)) {
        std::string Key;
        if (std::holds_alternative<MapStringIndexType>(ElementSelector)) {
            Key = std::get<MapStringIndexType>(ElementSelector);
        } else if (std::holds_alternative<MapIntegerIndexType>(ElementSelector)) {
            throw RuntimeErrorClass("Keytype <integer> not supported for string map access");
        } else if (std::holds_alternative<ArrayIndexType>(ElementSelector)) {
            throw RuntimeErrorClass("Keytype <arrayindex> not supported for string map access");
        } else if (std::holds_alternative<IndexRangeType>(ElementSelector)) {
            throw RuntimeErrorClass("Keytype <Range> not supported for string map access");
        } else {
            throw RuntimeErrorClass("Keytype <unknown> not supported for string map access" + std::to_string(ElementSelector.index()) );
        }
        std::cout << "Key: " << Key << ":" << this << std::endl;
        if (CreateIfNeeded) {
            auto &Element = std::get<MapStringKeyType>(Data)[Key];
            if (Element==nullptr) {
                Element = std::make_unique<Variables::VariableContentClass>(VariableContentClass::MakeEmpty(BaseType));
            }
            return *Element;

        } else {
           return *(std::get<MapStringKeyType>(Data).at(Key));
        }
    } else {
        throw INTERNAL_ERROR_OBJECT("Unsupported maptype");
    }
    throw INTERNAL_ERROR_OBJECT("internal dimension trouble");
}


MapClass::MapClass(const MapClass &s) : KeyType(s.KeyType), BaseType(s.BaseType) {

    //auto CopyMap = [](auto &Src){}
    if (std::holds_alternative<MapStringKeyType>(s.Data)) {
        Data = CopyMap<MapStringKeyType>(s);
    } else if (std::holds_alternative<MapIntegerKeyType>(s.Data)) {
        Data = CopyMap<MapIntegerKeyType>(s);
    } else if (std::holds_alternative<MapStringAndIntegerKeyType>(s.Data)) {
        Data = CopyMap<MapStringAndIntegerKeyType>(s);
    } else {
        throw INTERNAL_ERROR_OBJECT("Unexpected maptype");
    }
}

MapClass &MapClass::operator =(const MapClass &s) { //= default; //{ (void)s; SIGNAL_UNIMPLEMENTED();}
    if (std::holds_alternative<MapStringKeyType>(s.Data)) {
        Data = CopyMap<MapStringKeyType>(s);
    } else if (std::holds_alternative<MapIntegerKeyType>(s.Data)) {
        Data = CopyMap<MapIntegerKeyType>(s);
    } else if (std::holds_alternative<MapStringAndIntegerKeyType>(s.Data)) {
        Data = CopyMap<MapStringAndIntegerKeyType>(s);
    } else {
        throw INTERNAL_ERROR_OBJECT("Unexpected maptype");
    }
    return *this;
}

MapClass::MapClass(const MapEntryListType &Initializer)
    : BaseType(TypeDescriptorClass::Type::Undefined) {
    // Check indextype
    KeyType = MapDescriptorClass::KeyTypesType::None;
    for (auto const &e : Initializer) {
        if (std::holds_alternative<std::string>(e.first)) {
            KeyType = KeyType | MapDescriptorClass::KeyTypesType::String;
        } else if (std::holds_alternative<int64_t>(e.first)) {
            KeyType = KeyType | MapDescriptorClass::KeyTypesType::Integer;
            //  } else if (std::holds_alternative<bool>(e.first)) {
            //      KeyType = KeyType | MapDescriptorClass::KeyTypesType::bool;
        } else {
            throw INTERNAL_ERROR_OBJECT("Unexpected type");
        }
        if (BaseType == TypeDescriptorClass::Type::Undefined) {
            BaseType = e.second.getType();
        } else if (BaseType == e.second.getType()) {
            // All types same...
        } else if (   (BaseType == TypeDescriptorClass::Type::Integer)
                   &&(e.second.getType() == TypeDescriptorClass::Type::Float)) {
            // Float dominates int
            BaseType = e.second.getType();
        } else if ((e.second.getType() == TypeDescriptorClass::Type::Illegal)) {
            // illegal dominates all
            BaseType = e.second.getType();
        } else {
            // We have a type mix
            BaseType = TypeDescriptorClass::Type::Dynamic;
        }
    }
    switch (KeyType) {
    default:
    case MapDescriptorClass::KeyTypesType::None: throw INTERNAL_ERROR_OBJECT("Map without keytype");

    case MapDescriptorClass::KeyTypesType::Integer:
    {
        MapIntegerKeyType NewMap;
        for(auto const &e: Initializer) {
            NewMap[std::get<int64_t>(e.first)] = std::make_unique<VariableContentClass>((e.second));
        }
        Data = std::move(NewMap);
    }
    break;

    case MapDescriptorClass::KeyTypesType::String:
    {
        MapStringKeyType NewMap;
        for(auto const &e: Initializer) {
            NewMap[std::get<std::string>(e.first)] = std::make_unique<VariableContentClass>((e.second));
        }
        Data = std::move(NewMap);
    }
        //  case static_cast<MapDescriptorClass::KeyTypesType>(static_cast<int>(MapDescriptorClass::KeyTypesType::Integer) | static_cast<int>(MapDescriptorClass::KeyTypesType::String)):
    case (MapDescriptorClass::KeyTypesType::Integer | MapDescriptorClass::KeyTypesType::String):
    {
        MapStringAndIntegerKeyType NewMap;
        for(auto const &e: Initializer) {
            NewMap[e.first] = std::make_unique<VariableContentClass>((e.second));
        }
        Data = std::move(NewMap);
    }
    }
}

ProxyVariableClass MapClass::GetOrCreateIndexedElement(std::string BaseName, ElementSelectorType Selector) const
{
    return GetIndexedElement(BaseName, Selector, true);
}


} // namespace Variables
