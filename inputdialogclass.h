#ifndef INPUTDIALOGCLASS_H
#define INPUTDIALOGCLASS_H

// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause


#include <QDialog>
#include <QList>
#include <QPair>

QT_BEGIN_NAMESPACE
class QCheckBox;
class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QTableWidget;
class QTextEdit;
class QWidget;
QT_END_NAMESPACE



class InputDialogClass : public QDialog
{
    Q_OBJECT

public:
    InputDialogClass(const QString &title, const std::string &Description, QWidget *parent);

public slots:
    void verify();

public:

    typedef std::variant<std::monostate, int64_t, double, std::string> ValueType;

    struct ElementType {
        std::string Label;
        enum Type {Int, Float, String} Type;
        ValueType Default;
    };

    struct DialogDescriptor {
        std::string Title;
        std::string Description;
        std::vector<ElementType> InputFieldDescriptors;
    };

private:
    QDialogButtonBox *buttonBox;


    struct InputDescriptor {
        QLineEdit *InputFields;
        enum ElementType::Type FieldType;
    };

    std::vector<InputDescriptor> InputFields;
    std::vector<ValueType> Results;
};


#endif // INPUTDIALOGCLASS_H
