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

const Variables::VariableContentClass &GlobalVariableClass::GetValue() const
{
    return Content;
}


Variables::VariableContentClass &GlobalVariableClass::GetWriteReferenceToValue()
{
   if (!IsWriteable()) { throw RuntimeErrorClass("Variable not writeable");}
   return Content;
}

void GlobalVariableClass::SetValue(Variables::VariableContentClass v)
{
    std::cout << "GlbSet:" << v;
    if (!IsWriteable()) { throw RuntimeErrorClass("Variable not writeable");}
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

const Variables::VariableContentClass &LocalVariableClass::GetValue() const
{
    return Parent->GetVariableContentForOffset(Reference);
}

Variables::VariableContentClass &LocalVariableClass::GetWriteReferenceToValue()
{
    if (!IsWriteable()) { throw RuntimeErrorClass("Variable not writeable");}
    return Parent->GetVariableContentWriteReferenceForOffset(Reference);
}

void LocalVariableClass::SetValue(Variables::VariableContentClass v)
{
    if (!IsWriteable()) { throw RuntimeErrorClass("Variable not writeable");}
    PrepareForAssignment(v);
    if (IsAssignable(v)) {
        Parent->SetVariableContentForOffset(Reference, v);
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


const Variables::VariableContentClass &TemporaryVariableClass::GetValue() const
{
    return Content;
}

Variables::VariableContentClass &TemporaryVariableClass::GetWriteReferenceToValue()
{
    if (!IsWriteable()) { throw RuntimeErrorClass("Variable not writeable");}
    return Content;
}

void TemporaryVariableClass::SetValue(Variables::VariableContentClass v)
{
    std::cout << "TmpSet:" << v;
    if (!IsWriteable()) { throw RuntimeErrorClass("Variable not writeable");}
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

Variables::VariableContentClass const &ProxyVariableClass::GetValue() const
{
    return Content;
}

// *** Proxy

Variables::VariableContentClass &ProxyVariableClass::GetWriteReferenceToValue()
{
    if (IsWriteable()) { throw RuntimeErrorClass("Variable not writeable");}
    return Content;
}

void ProxyVariableClass::SetValue(Variables::VariableContentClass v)
{
    std::cout << "PxySet:" << v;
    if (!IsWriteable()) { throw RuntimeErrorClass("Variable not writeable");}
    PrepareForAssignment(v);
    if (IsAssignable(v)) {
        Content = v;
    } else {
        std::cout << "PxySetexcp:" << v;
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
