#ifndef VARIABLECLASS_H
#define VARIABLECLASS_H


#include <string>
class VariableContextClass;

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
