#ifndef CLASSNODEHELPER_H
#define CLASSNODEHELPER_H

#include <map>
#include <string>


namespace yy {
class location;
}
typedef yy::location  LocationType;


class VariableManager;
class StatementClass;
class ReferementClass;
class AssignementClass;

namespace Variables {

class ClassClass;
class FunctionDefinitionClass;
class FunctionDefinitionBaseClass;
}
class VariableClass;
class VariableTypeDescriptorClass;



class ClassNodeHelper {

public:
    ClassNodeHelper(VariableManager &Variables) : Variables(Variables) {}
    typedef std::map<std::string, std::shared_ptr<VariableClass>> AttributeListType;

private:
    struct ClassNodeDescriptor {
        ClassNodeDescriptor(std::string Name) : Name(std::move(Name)) {}
        std::string Name;
        std::shared_ptr<Variables::ClassClass> BaseClass;
        AttributeListType ClassAttributes;
        AttributeListType ObjectAttributes;

    };
    VariableManager &Variables;

    std::vector<ClassNodeDescriptor> PendingClassDefinitions;

public:
    void StartClassDefinition(std::string ClassName) {PendingClassDefinitions.push_back(ClassName);}
    void EndClassDefinition()  {PendingClassDefinitions.pop_back();}
private:
public:
    bool SetBaseClass(std::string Name);
    bool AddClassAttribute(std::string Name, std::shared_ptr<VariableClass> ClassAttribute);
    bool AddObjectAttribute(std::string Name, std::shared_ptr<VariableClass> Attribute);
    AttributeListType const &GetClassAttributes() {return PendingClassDefinitions.back().ClassAttributes;}
    AttributeListType const &GetObjectAttributes() {return PendingClassDefinitions.back().ObjectAttributes;}
    
};

#endif // CLASSNODEHELPER_H
