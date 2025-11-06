#ifndef VARIABLECLASS_H
#define VARIABLECLASS_H


#include <list>
#include <map>
#include <string>
#include <vector>
class ExpressionClass;
class StatementClass;
class VariableClass;

class VariableContextClass;

class ArrayDescriptorClass;

using TypeDescriptor = std::variant<std::monostate, ArrayDescriptorClass>;

class TypeDescriptorClass {

public:
    enum class Type {
        Integer, Float, Bool, String, Stack, List, Array, Map, Function, Dynamic//, fixed
    };

    std::unique_ptr<TypeDescriptor> Descriptor;
};

class StackDescriptorClass  {
public:
    std::unique_ptr<TypeDescriptor> BaseType;
};

class ArrayDescriptorClass  {
public:
    std::vector<int> Dimensions; // -1 = flexible dimension
    std::unique_ptr<TypeDescriptor> BaseType;
};

class DynamicDescriptorClass {
public:
    std::unique_ptr<TypeDescriptor> CurrentType;
};

class ValueClass;

class StackClass {
    std::vector<std::unique_ptr<ValueClass>> Data;
};

class ListClass {
    std::list<std::unique_ptr<ValueClass>> Data;
};

class ArrayClass {

    std::vector<std::unique_ptr<ArrayClass>> Data;
};

class SparseArrayClass {

    std::map<int, std::unique_ptr<ValueClass>> Data;
};

class MapClass {

    std::map<std::string, std::unique_ptr<ValueClass>> Data;
};

class FunctionClass {
    std::list<std::shared_ptr<StatementClass>> Statements;
    std::list<std::shared_ptr<VariableClass>> Parameters;

};

class ValueClass {
    typedef std::variant<std::monostate,
                         int64_t,
                         double,
                         std::string,
                         StackClass,
                         ListClass,
                         ArrayClass,
                         MapClass,
                         FunctionClass> dataType;

    dataType Data;
    TypeDescriptorClass Type;
    std::shared_ptr<ExpressionClass> AssignedExpression;
};

class VariableClass
{
    VariableContextClass *MyContext;
    const std::string Name;

public:
    VariableClass(const std::string &Name_) : MyContext(nullptr), Name(Name_) {}
    virtual ~VariableClass() {}
    void SetContext(VariableContextClass *Context);
    const std::string GetName()  const {return Name;}
    virtual double      GetValue() const = 0;
    virtual void        SetValue(double v) = 0;
};

class DoubleVariableClass : public VariableClass {

    double Value;

    // VariableClass interface
public:
    DoubleVariableClass(const std::string &Name_, double Value);
    virtual double GetValue() const override;
    virtual void SetValue(double v) override;
};

#endif // VARIABLECLASS_H
