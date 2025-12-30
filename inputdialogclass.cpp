#include "inputdialogclass.h"

//InputDialogClass::InputDialogClass() {}
#include <QtWidgets>


//! [0]
InputDialogClass::InputDialogClass(const DialogDescriptor *Description, std::vector<ValueType> *Results_, QWidget *parent)
    : QDialog(parent)
{
    Results = Results_;

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &InputDialogClass::verify);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &InputDialogClass::reject);
    //! [0]
    QFormLayout *Formlayout = new QFormLayout;
    for (auto &e: Description->InputFieldDescriptors) {
        QLineEdit *w = new QLineEdit;
        InputFields.push_back({w, e.Type});
        switch (e.Type) {

        case ElementType::Int:    if(std::holds_alternative<int64_t>(e.Default)){
                                      w->setText(QString::number(std::get<int64_t>(e.Default)));
                                  }
                                  w->setValidator(new QIntValidator);
                                  break;

        case ElementType::Float:  if(std::holds_alternative<double>(e.Default)){
                                     w->setText(QString::number(std::get<double>(e.Default)));
                                  }
                                  w->setValidator(new QDoubleValidator);
                                  break;

        case ElementType::String: if(std::holds_alternative<std::string>(e.Default)){
                                     w->setText(QString::fromStdString(std::get<std::string>(e.Default)));
                                  }
                                  break;
        }

        Formlayout->addRow(QString::fromStdString(e.Label), w);
    }

    //! [1]
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(new QLabel(QString::fromStdString(Description->Description)));
    mainLayout->addLayout(Formlayout);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(QString::fromStdString(Description->Title));
}
//! [1]

//! [2]

//! [7]
void InputDialogClass::verify()
{
    Results->clear();
    for (auto &w: InputFields) {
        if (!w.InputFields->hasAcceptableInput()) {
           reject();
           return;
        }
        switch(w.FieldType) {

        case ElementType::Int:      Results->push_back(w.InputFields->text().toInt()); break;
        case ElementType::Float:    Results->push_back(w.InputFields->text().toDouble()); break;
        case ElementType::String:   Results->push_back(w.InputFields->text().toStdString()); break;
        default:                    Results->push_back(std::monostate()); break;
        }
    }
    accept();
    return;
}
