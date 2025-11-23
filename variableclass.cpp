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
    Content = v;
}

void GlobalVariableClass::Print(std::ostream &s)
{
    s <<  "<" << GetName() << ": " << Content << ">";
}

Variables::VariableContentClass LocalVariableClass::GetValue() const
{
    return Parent->GetVariableContentForOffset(Reference);
}

void LocalVariableClass::SetValue(Variables::VariableContentClass v)
{
    Parent->GetVariableContentForOffset(Reference) = v;
}

void LocalVariableClass::Print(std::ostream &s)
{
    s << "<" << GetName() << ": Local>";
}


void VariableClass::DrawNode(std::ostream &s, int MyNodeNumber) const
{
    s << "Node" << MyNodeNumber << "[label = \"<f0> |<f1> " << Name << "\\n\\<" << MyContext->GetName() << "\\>|<f2> \"];" << std::endl;
}

const ValueTypeDescriptorClass &GlobalVariableClass::GetContainedType() const
{
    return Content.getType();
}

const ValueTypeDescriptorClass &LocalVariableClass::GetContainedType() const
{
    return Parent->GetVariableContentForOffset(Reference).getType();
}

