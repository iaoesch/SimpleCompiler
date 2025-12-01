#include <sstream>

#include "variableclass.h"
#include "Errclass.hpp"
#include "compact.h"
#include "varmanag.hpp"




void VariableClass::SetContext(VariableContextClass *Context)
{
    if (MyContext != nullptr) {
        throw INTERNAL_ERROR_OBJECT("Changing context of variable is not allowed");
    }
    MyContext = Context;
}

Variables::VariableContentClass GlobalVariableClass::GetValue() const
{
    return Content;
}

void GlobalVariableClass::SetValue(Variables::VariableContentClass v)
{
    std::cout << "GlbSet:" << v;
    PrepareForAssignment(v);
    if (IsAssignable(v)) {
        Content = v;
    } else {
        std::cout << "GlbSetexcp:" << v;
        std::stringstream s;
        s << "Incompatible Type, assigning " << v.getType() << " to " << Type();
        throw RuntimeErrorClass(s.str());
    }

}

void GlobalVariableClass::Print(std::ostream &s)
{
    s <<  "<" << GetName() << ":" << Type()  << ": " << Content << ">";
}

Variables::VariableContentClass LocalVariableClass::GetValue() const
{
    return Parent->GetVariableContentForOffset(Reference);
}

void LocalVariableClass::SetValue(Variables::VariableContentClass v)
{
    PrepareForAssignment(v);
    if (IsAssignable(v)) {
        Parent->GetVariableContentForOffset(Reference) = v;
    } else {
        std::stringstream s;
        s << "Incompatible Type, assigning " << v.getType() << " to " << Type();
        throw RuntimeErrorClass(s.str());
    }

}

void LocalVariableClass::Print(std::ostream &s)
{
    s << "<" << GetName() << ":" << Type() << ": Local>";
}


void VariableClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
    s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> " << Name << "\\n\\<" << (MyContext==nullptr?"$null$":MyContext->GetName()) << "\\>|<f2> \"];" << std::endl;
}



const ValueTypeDescriptorClass &GlobalVariableClass::GetContainedType() const
{
    return Content.getType();
}

const ValueTypeDescriptorClass &LocalVariableClass::GetContainedType() const
{
    return Parent->GetVariableContentForOffset(Reference).getType();
}


Variables::VariableContentClass TemporaryVariableClass::GetValue() const
{
    return Content;
}

void TemporaryVariableClass::SetValue(Variables::VariableContentClass v)
{
    std::cout << "TmpSet:" << v;
    PrepareForAssignment(v);
    if (IsAssignable(v)) {
        Content = v;
    } else {
        std::cout << "TmpSetexcp:" << v;
        std::stringstream s;
        s << "Incompatible Type, assigning " << v.getType() << " to " << Type();
        throw RuntimeErrorClass(s.str());
    }
}

void TemporaryVariableClass::Print(std::ostream &s)
{
    s <<  "<" << GetName() << ":" << Type()  << ": " << Content << ">";
}

const ValueTypeDescriptorClass &TemporaryVariableClass::GetContainedType() const
{
    return Content.getType();
}

Variables::VariableContentClass ProxyVariableClass::GetValue() const
{
    return Content;
}

void ProxyVariableClass::SetValue(Variables::VariableContentClass v)
{
    std::cout << "TmpSet:" << v;
    PrepareForAssignment(v);
    if (IsAssignable(v)) {
        Content = v;
    } else {
        std::cout << "TmpSetexcp:" << v;
        std::stringstream s;
        s << "Incompatible Type, assigning " << v.getType() << " to " << Type();
        throw RuntimeErrorClass(s.str());
    }
}

void ProxyVariableClass::Print(std::ostream &s)
{
    s <<  "<" << GetName() << ":" << Type()  << ": " << Content << ">";
}

const ValueTypeDescriptorClass &ProxyVariableClass::GetContainedType() const
{
    return Content.getType();
}
