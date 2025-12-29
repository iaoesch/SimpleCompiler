#ifndef SYSTEMINTERFACECLASS_H
#define SYSTEMINTERFACECLASS_H

#include "variablecontentclass.h"
#include <QObject>

class PrecompiledFunctionManagerClass;

class SystemInterfaceClass : public QObject
{
    Q_OBJECT
public:
    explicit SystemInterfaceClass(QObject *parent = nullptr);

    void Register(PrecompiledFunctionManagerClass &PrecompiledFunctionManager);

signals:
private:
    Variables::VariableContentClass Input(std::string Title, std::string Description, Variables::ListClass &InputElements);
};

#endif // SYSTEMINTERFACECLASS_H
