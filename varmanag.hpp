#ifndef VARIABLE_MANAGER_CLASS_HPP
#define VARIABLE_MANAGER_CLASS_HPP
/*****************************************************************************/
/*  Header     : VariableManager                                Version 1.0  */
/*****************************************************************************/
/*                                                                           */
/*  Function   : Manages the Variables in our System                         */
/*                                                                           */
/*                                                                           */
/*  Methodes   : VariableManager()                                           */
/*              ~VariableManager()                                           */
/*               GetVariable()                                               */
/*                                                                           */
/*  Author     : I. Oesch                                                    */
/*                                                                           */
/*  History    : 26.02.2002  IO Created                                      */
/*                                                                           */
/*  File       : varmanag.hpp                                                */
/*                                                                           */
/*****************************************************************************/
/*    SWS Diplomarbeit 1997 Bonetracker from Ivo Oesch and Daniel Buergin    */
/*****************************************************************************/

/* imports */

/* Class constant declaration  */

/* Class Type declaration      */

/* Class data declaration      */
#include "variableclass.h"
#include <map>
#include <string>
//#include "variableclass.h"
class VariableClass;
class VariableTypeDescriptorClass;
namespace Variables {
    class FunctionDefinitionClass;
    class VariableContentClass;
}

class VariableContextProxyClass;
class VariableContextManageClass;

class VariableContextClass {
    protected:
        VariableContextClass *ParentContext;
        std::vector<std::shared_ptr<VariableContextClass>> Children;
        const std::string Name;
        bool ParentIsHidden;

    public:

        VariableContextClass(const std::string &Name_, VariableContextClass *Parent_ = nullptr, bool HideParent = false) : ParentContext(Parent_), Name(Name_), ParentIsHidden(HideParent) {}
        //    friend std::shared_ptr<VariableContextClass> std::make_shared<VariableContextClass, const std::string&,VariableContextClass*, bool>(const std::string &, VariableContextClass* &&, bool &&);
        //    friend std::shared_ptr<VariableContextClass> std::make_shared<VariableContextClass, std::string, VariableContextClass*, bool>(std::string, VariableContextClass*, bool);
    public:
        std::shared_ptr<VariableContextManageClass> CreateSubContext(const std::string &Name, bool HideParent = false);
        std::shared_ptr<VariableContextProxyClass> CreateProxySubContext(const std::string &Name, bool HideParent = false);

        virtual std::shared_ptr<VariableClass> RegisterVariable(const std::string Name, std::shared_ptr<VariableClass> Var, bool OverwriteAllowed = false) = 0;
        virtual std::shared_ptr<VariableClass> LookupVariable(const std::string Name) = 0;

        // For Testpurposes
        virtual std::shared_ptr<VariableClass> LookupVariableInThisContextOnly(const std::string Name) = 0;
        virtual size_t GetNumberOfVariables() const = 0;

        virtual void Dump(std::ostream &s) = 0;
        const std::string &GetName() {return Name;}
};

class VariableContextManageClass : public VariableContextClass {
    std::map<std::string, std::shared_ptr<VariableClass>> Variables;

public:

    VariableContextManageClass(const std::string &Name_, VariableContextClass *Parent_ = nullptr, bool HideParent = false) : VariableContextClass(Name_, Parent_, HideParent) {}
//    friend std::shared_ptr<VariableContextClass> std::make_shared<VariableContextClass, const std::string&,VariableContextClass*, bool>(const std::string &, VariableContextClass* &&, bool &&);
//    friend std::shared_ptr<VariableContextClass> std::make_shared<VariableContextClass, std::string, VariableContextClass*, bool>(std::string, VariableContextClass*, bool);
public:
   // std::shared_ptr<VariableContextClass> CreateSubContext(const std::string &Name_, bool HideParent = false) {Children.push_back(std::make_shared<VariableContextManageClass>(Name_, this, HideParent)); return Children.back();}

    std::shared_ptr<VariableClass> RegisterVariable(const std::string Name, std::shared_ptr<VariableClass> Var, bool OverwriteAllowed = false) override;
    std::shared_ptr<VariableClass> LookupVariable(const std::string Name) override;

    // For Testpurposes
    std::shared_ptr<VariableClass> LookupVariableInThisContextOnly(const std::string Name) override;
    size_t GetNumberOfVariables() const override {return Variables.size();}

    void Dump(std::ostream &s) override;
};


class VariableContextProxyClass  : public VariableContextClass {
    std::shared_ptr<VariableContextClass> TheRealContext;
    bool ReadOnly;

public:

    VariableContextProxyClass(const std::string &Name_, VariableContextClass *Parent_ = nullptr, bool HideParent = false) : VariableContextClass(Name_, Parent_, HideParent), TheRealContext(nullptr), ReadOnly(true) {}
    //    friend std::shared_ptr<VariableContextClass> std::make_shared<VariableContextClass, const std::string&,VariableContextClass*, bool>(const std::string &, VariableContextClass* &&, bool &&);
    //    friend std::shared_ptr<VariableContextClass> std::make_shared<VariableContextClass, std::string, VariableContextClass*, bool>(std::string, VariableContextClass*, bool);
public:
    void SetReferedContext(std::shared_ptr<VariableContextClass> Ref) {TheRealContext = Ref;}
    std::shared_ptr<VariableClass> RegisterVariable(const std::string Name, std::shared_ptr<VariableClass> Var, bool OverwriteAllowed = false) override
    {
        if ((TheRealContext != nullptr) && (ReadOnly == false)) {
            return TheRealContext->RegisterVariable(Name, Var, OverwriteAllowed);
        } else {
            throw (INTERNAL_ERROR_OBJECT("Using unavaillable Proxy (ro = " + std::to_string(ReadOnly) + ")"));
        }
    }
    std::shared_ptr<VariableClass> LookupVariable(const std::string Name) override
    {
        if (TheRealContext != nullptr) {
            // we must follow parents here, but which one?
            // parent of proxy or parent of proxied?
            // maybe a call of LookupVariableInThisContextOnly() is better here?
            return TheRealContext->LookupVariable(Name);
        } else {
            // we probably should follow parents here, but which one?
            // parent of proxy or parent of proxied?
            //
            // we cannot throw here, as it is legal to look while proxy is not ready
            //throw (INTERNAL_ERROR_OBJECT("Using unavaillable Proxy (ro = " + std::to_string(ReadOnly) + ")"));
            // for now we just signal 'not found'
            return nullptr;
        }
    }

    // For Testpurposes
    std::shared_ptr<VariableClass> LookupVariableInThisContextOnly(const std::string Name) override
    {
        if (TheRealContext != nullptr) {
            return TheRealContext->LookupVariableInThisContextOnly(Name);
        } else {
            // we cannot throw here, as it is legal to look while proxy is not ready
            // throw (INTERNAL_ERROR_OBJECT("Using unavaillable Proxy (ro = " + std::to_string(ReadOnly) + ")"));
            return nullptr;
        }
    }
    size_t GetNumberOfVariables() const override
    {
        if (TheRealContext != nullptr) {
            return TheRealContext->GetNumberOfVariables();
        } else {
            throw (INTERNAL_ERROR_OBJECT("Using unavaillable Proxy (ro = " + std::to_string(ReadOnly) + ")"));
        }
    }

    void Dump(std::ostream &s) override
    {
        s << "Proxy context '" << Name << "' for: ";
        if (TheRealContext != nullptr) {
            return TheRealContext->Dump(s);
        } else {
            s << "<nullptr>\n";
           // throw (INTERNAL_ERROR_OBJECT("Using unavaillable Proxy (ro = " + std::to_string(ReadOnly) + ")"));
        }
    }

};

inline std::shared_ptr<VariableContextManageClass> VariableContextClass::CreateSubContext(const std::string &Name, bool HideParent) {auto Context = std::make_shared<VariableContextManageClass>(Name, this, HideParent); Children.push_back(Context); return Context;}
inline std::shared_ptr<VariableContextProxyClass> VariableContextClass::CreateProxySubContext(const std::string &Name, bool HideParent) {auto Proxy = std::make_shared<VariableContextProxyClass>(Name, this, HideParent); Children.push_back(Proxy); return Proxy;}


/* Class definition            */
class VariableManager
{
    std::vector<std::shared_ptr<VariableContextClass>> ContextStack;
    std::vector<std::shared_ptr<VariableContextClass>> Contexts;
    VariableClass::StorageClass DefaultStorage;
    bool Local;
//    std::shared_ptr<Variables::FunctionDefinitionBaseClass> LocalsParent;
    uint32_t LocalOffset;
    uint32_t LocalClassOffset;
public:
    enum ParentVisibility {ParentVisible, HideParent};
    typedef std::vector<Variables::VariableContentClass> LocalStorageType;
    typedef std::vector<std::shared_ptr<LateBindingVariableClass>> ObjectMemberVariableType;
private:
    struct LocalFunctionStorageContextType {
    //    LocalFunctionStorageContextType(LocalFunctionStorageContextType &&) = default;
    //    LocalFunctionStorageContextType(LocalFunctionStorageContextType const &) = default;
        LocalStorageType &LocalStorageTemplates;
        std::shared_ptr<Variables::FunctionDefinitionBaseClass> LocalsParent;

    };
    struct LocalClassStorageContextType {
   //     LocalClassStorageContextType(LocalClassStorageContextType &&) = default;
   //     LocalClassStorageContextType(LocalClassStorageContextType const &) = default;
        LocalStorageType &LocalClassAttributeStorageTemplates;
        LocalStorageType &LocalAttrubiteStorageTemplates;
        ObjectMemberVariableType &LocalVariableTemplates;
        std::shared_ptr<Variables::ClassClass> LocalsParent;
    };

    // Some shortcuts for easier use
    typedef LocalFunctionStorageContextType FktTemplate;
    typedef LocalClassStorageContextType ClassTemplate;
    typedef std::variant<FktTemplate, ClassTemplate> LocalStorageContextType;
    class LocalStorageInfoClass {
    public:
        LocalStorageContextType LocalStorageTemplates;
        VariableClass::StorageClass DefaultStorage;
    };
    std::vector<LocalStorageInfoClass> LocalStorageInfoStack;

    static Environment *DefaultEnvironment;

   // Data
    std::shared_ptr<VariableClass> CreateSymbol(std::string Name, const VariableTypeDescriptorClass &Type, VariableClass::StorageClass Storage);
public:
    static void SetDefaultEnvironment(Environment &Env) {DefaultEnvironment = &Env;}

    VariableManager() : DefaultStorage(VariableClass::StorageClass::Global|VariableClass::StorageClass::RW), Local(false), LocalOffset(0) {}

    void clear() {Local = false; LocalOffset = 0; ContextStack.clear(); Contexts.clear();}

    std::shared_ptr<VariableContextClass> CreateNewContext(std::string Name, ParentVisibility ParentVisibilityMode = ParentVisible);
   std::shared_ptr<VariableContextProxyClass> CreateNewProxyContext(std::string Name, ParentVisibility ParentVisibilityMode = ParentVisible);
   void LeaveContext(int Levels = 1);
   void StartLocal(std::shared_ptr<Variables::FunctionDefinitionBaseClass> Parent);
   void EndLocal();
   void StartClass(std::shared_ptr<Variables::ClassClass> Parent);
   void EndClass();
 //  std::shared_ptr<VariableClass> GetOrCreateVariable(std::string Name, const VariableTypeDescriptorClass &Type, double Value);
   std::shared_ptr<VariableClass> CreateVariable(std::string Name, const VariableTypeDescriptorClass &Type, double Value);
   std::shared_ptr<VariableClass> CreateConstant(std::string Name, const VariableTypeDescriptorClass &Type, double Value);
   std::shared_ptr<VariableClass> CreateFunction(std::string Name, const VariableTypeDescriptorClass &Type, double Value);
   std::shared_ptr<VariableClass> CreateClass(std::string Name, const VariableTypeDescriptorClass &Type, double Value);
   std::shared_ptr<VariableClass> GetVariableReference(std::string Name);
   std::shared_ptr<VariableClass> GetVariableReferenceCreateIfNotFound(std::string Name, const VariableTypeDescriptorClass &RequiredType);
 //  std::shared_ptr<VariableClass> CreateVariableAndGetReference(std::string Name, const VariableTypeDescriptorClass &RequiredType);

   // For Testpurposes
   size_t GetNumberOfContexts() const {return Contexts.size();}
   std::shared_ptr<VariableClass> GetVariableReferenceForContext(std::string Name, size_t Index);
   size_t GetNumberOfVariablesForContext(size_t Index) const {return Contexts[Index]->GetNumberOfVariables();}

   void Dump(std::ostream &s);
   std::shared_ptr<VariableClass> CreateMember(std::string Name, const VariableTypeDescriptorClass &Type, double Value);
};


/*****************************************************************************/
/*  End Header  : BTError                                                    */
/*****************************************************************************/
#endif


