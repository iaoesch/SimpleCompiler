#include <sstream>

#include "variableclass.h"
#include "Errclass.hpp"
#include "compact.h"
#include "varmanag.hpp"

static ErrorEnvironment Errorenv;
Environment *VariableClass::DefaultEnvironment = &Errorenv;


void VariableClass::SetContext(VariableContextManageClass *Context)
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

Variables::VariableContentClass const &GlobalVariableClass::GetInitialValue() const
{
   // throw INTERNAL_ERROR_OBJECT("Global has no initial value");
    return Content;
}



Variables::VariableContentClass &GlobalVariableClass::GetWriteReferenceToValue()
{
   if (!IsWriteable()) { throw RuntimeErrorClass("Variable not writeable", -1);}
   return Content;
}

void GlobalVariableClass::SetValue(Variables::VariableContentClass v)
{
    DefaultEnvironment->DebugOutput() << "GlbSet:" << v;
    if (!IsWriteable()) { throw RuntimeErrorClass("Variable not writeable", -1);}
    PrepareForAssignment(v);
    if (IsAssignable(v)) {
        Content = v;
    } else {
        DefaultEnvironment->DebugOutput() << "GlbSetexcp:" << v;
        std::stringstream s;
        s << "Incompatible Type, assigning " << v.getType() << " to " << Type();
        throw RuntimeErrorClass(s.str(), -1);
    }

}

void GlobalVariableClass::SetInitialValue(Variables::VariableContentClass v)
{
    if (Initialized) { throw INTERNAL_ERROR_OBJECT("Variable initialiced twice");}
    Initialized = true;
    DefaultEnvironment->DebugOutput() << "GlbSet:" << v;
    if (!IsWriteable()) { throw RuntimeErrorClass("Variable not writeable", -1);}
    PrepareForAssignment(v);
    if (IsAssignable(v)) {
        Content = v;
    } else {
        DefaultEnvironment->DebugOutput() << "GlbSetexcp:" << v;
        std::stringstream s;
        s << "Incompatible Type, assigning " << v.getType() << " to " << Type();
        throw RuntimeErrorClass(s.str(), -1);
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

Variables::VariableContentClass const &LocalVariableClass::GetInitialValue() const
{
    return Parent->GetInitialVariableContentForOffset(Reference);
}


Variables::VariableContentClass &LocalVariableClass::GetWriteReferenceToValue()
{
    if (!IsWriteable()) { throw RuntimeErrorClass("Variable not writeable", -1);}
    return Parent->GetVariableContentWriteReferenceForOffset(Reference);
}

void LocalVariableClass::SetValue(Variables::VariableContentClass v)
{
    if (!IsWriteable()) { throw RuntimeErrorClass("Variable not writeable", -1);}
    PrepareForAssignment(v);
    if (IsAssignable(v)) {
        Parent->SetVariableContentForOffset(Reference, v);
    } else {
        std::stringstream s;
        s << "Incompatible Type, assigning " << v.getType() << " to " << Type();
        throw RuntimeErrorClass(s.str(), -1);
    }

}

void LocalVariableClass::SetInitialValue(Variables::VariableContentClass v)
{
    if (Initialized) { throw INTERNAL_ERROR_OBJECT("Variable initialiced twice");}
    Initialized = true;
    if (!IsWriteable()) { throw RuntimeErrorClass("Variable not writeable", -1);}
    PrepareForAssignment(v);
    if (IsAssignable(v)) {
        Parent->InitializeVariableContentForOffset(Reference, v);
    } else {
        std::stringstream s;
        s << "Incompatible Type, assigning " << v.getType() << " to " << Type();
        throw RuntimeErrorClass(s.str(), -1);
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

const Variables::VariableContentClass &LateBindingVariableClass::GetValue() const
{
    Variables::VariableContentClass v = BoundMethod->GetVariableContentForOffset(ThisOffset);
    const Variables::ObjectClass &r = v.GetValue<Variables::ObjectClass>();
    return r.GetVariableContentForOffset(Reference);
}

Variables::VariableContentClass const &LateBindingVariableClass::GetInitialValue() const
{
    throw INTERNAL_ERROR_OBJECT("Atributes have no initial value");
}


Variables::VariableContentClass &LateBindingVariableClass::GetWriteReferenceToValue()
{
    if (!IsWriteable()) { throw RuntimeErrorClass("Variable not writeable", -1);}
    Variables::VariableContentClass v = BoundMethod->GetVariableContentForOffset(ThisOffset);
    Variables::ObjectClass &r = v.GetWriteReference<Variables::ObjectClass>();
    return r.GetVariableContentWriteReferenceForOffset(Reference);
}

void LateBindingVariableClass::SetValue(Variables::VariableContentClass v)
{
    if (!IsWriteable()) { throw RuntimeErrorClass("Variable not writeable", -1);}
    PrepareForAssignment(v);
    if (IsAssignable(v)) {
        Variables::VariableContentClass v = BoundMethod->GetVariableContentForOffset(ThisOffset);
        Variables::ObjectClass &r = v.GetWriteReference<Variables::ObjectClass>();
        return r.SetVariableContentForOffset(Reference, v);
    } else {
        std::stringstream s;
        s << "Incompatible Type, assigning " << v.getType() << " to " << Type();
        throw RuntimeErrorClass(s.str(), -1);
    }

}

void LateBindingVariableClass::SetInitialValue(Variables::VariableContentClass v)
{
    if (Initialized) { throw INTERNAL_ERROR_OBJECT("Variable initialiced twice");}
    Initialized = true;
    if (!IsWriteable()) { throw RuntimeErrorClass("Variable not writeable", -1);}
    PrepareForAssignment(v);
    if (IsAssignable(v)) {
        Variables::VariableContentClass v = BoundMethod->GetVariableContentForOffset(ThisOffset);
        Variables::ObjectClass &r = v.GetWriteReference<Variables::ObjectClass>();
        return r.InitializeVariableContentForOffset(Reference, v);
    } else {
        std::stringstream s;
        s << "Incompatible Type, assigning " << v.getType() << " to " << Type();
        throw RuntimeErrorClass(s.str(), -1);
    }

}

void LateBindingVariableClass::Print(std::ostream &s)
{
    s << "<" << GetName() << ":" << Type() << ": Late binding>";

}

const ValueTypeDescriptorClass &LateBindingVariableClass::GetContainedType() const
{
    return ValueTypeDescriptorClass(ValueTypeDescriptorClass::Type::Undefined);
  //  return Parent->GetVariableContentForOffset(Reference).getType();
}

const Variables::VariableContentClass &AttributeIndexVariableClass::GetValue() const
{
    return MemberPtr;
    //throw INTERNAL_ERROR_OBJECT("Atribute index has no value");
}

Variables::VariableContentClass const &AttributeIndexVariableClass::GetInitialValue() const
{
    return MemberPtr;
    //throw INTERNAL_ERROR_OBJECT("Atribute index have no value");
}


Variables::VariableContentClass &AttributeIndexVariableClass::GetWriteReferenceToValue()
{
    throw INTERNAL_ERROR_OBJECT("Atribute index has no value");
}

void AttributeIndexVariableClass::SetValue(Variables::VariableContentClass v)
{
    throw INTERNAL_ERROR_OBJECT("Atribute index has no value");
}

void AttributeIndexVariableClass::SetInitialValue(Variables::VariableContentClass v)
{
    throw INTERNAL_ERROR_OBJECT("Atribute index has no value");
}

void AttributeIndexVariableClass::Print(std::ostream &s)
{
    s << "<" << GetName() << ":" << Type() << ": Attribute Index>";
}

const ValueTypeDescriptorClass &AttributeIndexVariableClass::GetContainedType() const
{
    return ValueTypeDescriptorClass(ValueTypeDescriptorClass::Type::MemberPointer);
    //  return Parent->GetVariableContentForOffset(Reference).getType();
}


const Variables::VariableContentClass &TemporaryVariableClass::GetValue() const
{
    return Content;
}

Variables::VariableContentClass const &TemporaryVariableClass::GetInitialValue() const
{
    throw INTERNAL_ERROR_OBJECT("Temporary has no initial value");
    return Content;
}


Variables::VariableContentClass &TemporaryVariableClass::GetWriteReferenceToValue()
{
    if (!IsWriteable()) { throw RuntimeErrorClass("Variable not writeable", -1);}
    return Content;
}

void TemporaryVariableClass::SetValue(Variables::VariableContentClass v)
{
    DefaultEnvironment->DebugOutput() << "TmpSet:" << v;
    if (!IsWriteable()) { throw RuntimeErrorClass("Variable not writeable", -1);}
    PrepareForAssignment(v);
    if (IsAssignable(v)) {
        Content = v;
    } else {
        DefaultEnvironment->DebugOutput() << "TmpSetexcp:" << v;
        std::stringstream s;
        s << "Incompatible Type, assigning " << v.getType() << " to " << Type();
        throw RuntimeErrorClass(s.str(), -1);
    }
}

void TemporaryVariableClass::SetInitialValue(Variables::VariableContentClass v)
{
    if (Initialized) { throw INTERNAL_ERROR_OBJECT("Variable initialiced twice");}
    Initialized = true;
    DefaultEnvironment->DebugOutput() << "TmpSet:" << v;
    if (!IsWriteable()) { throw RuntimeErrorClass("Variable not writeable", -1);}
    PrepareForAssignment(v);
    if (IsAssignable(v)) {
        Content = v;
    } else {
        DefaultEnvironment->DebugOutput() << "TmpSetexcp:" << v;
        std::stringstream s;
        s << "Incompatible Type, assigning " << v.getType() << " to " << Type();
        throw RuntimeErrorClass(s.str(), -1);
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

Variables::VariableContentClass const &ProxyVariableClass::GetInitialValue() const
{
    throw INTERNAL_ERROR_OBJECT("Proxy has no initial value");
    return Content;
}


// *** Proxy

Variables::VariableContentClass &ProxyVariableClass::GetWriteReferenceToValue()
{
    if (IsWriteable()) { throw RuntimeErrorClass("Variable not writeable", -1);}
    return Content;
}

void ProxyVariableClass::SetValue(Variables::VariableContentClass v)
{
    DefaultEnvironment->DebugOutput() << "PxySet:" << v;
    if (!IsWriteable()) { throw RuntimeErrorClass("Variable not writeable", -1);}
    PrepareForAssignment(v);
    if (IsAssignable(v)) {
        Content = v;
    } else {
        DefaultEnvironment->DebugOutput() << "PxySetexcp:" << v;
        std::stringstream s;
        s << "Incompatible Type, assigning " << v.getType() << " to " << Type();
        throw RuntimeErrorClass(s.str(), -1);
    }
}

void ProxyVariableClass::SetInitialValue(Variables::VariableContentClass v)
{
    DefaultEnvironment->DebugOutput() << "PxySetInitial:" << v;
    if (Initialized) { throw INTERNAL_ERROR_OBJECT("Variable initialiced twice");}
    Initialized = true;
    if (!IsWriteable()) { throw RuntimeErrorClass("Variable not writeable", -1);}
    PrepareForAssignment(v);
    if (IsAssignable(v)) {
        Content = v;
    } else {
        DefaultEnvironment->DebugOutput() << "PxySetexcp:" << v;
        std::stringstream s;
        s << "Incompatible Type, assigning " << v.getType() << " to " << Type();
        throw RuntimeErrorClass(s.str(), -1);
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
