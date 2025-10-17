/********************************************************************************
** Form generated from reading UI file 'serialmonitor.ui'
**
** Created by: Qt User Interface Compiler version 6.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERIALMONITOR_H
#define UI_SERIALMONITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SerialMonitor
{
public:
    QCheckBox *checkBoxHex;
    QTextEdit *textEdit;
    QCheckBox *txCheckBox;
    QCheckBox *rxCheckBox;
    QPushButton *clearButton;

    void setupUi(QWidget *SerialMonitor)
    {
        if (SerialMonitor->objectName().isEmpty())
            SerialMonitor->setObjectName("SerialMonitor");
        SerialMonitor->resize(823, 448);
        QFont font;
        font.setFamilies({QString::fromUtf8("Menlo")});
        font.setPointSize(12);
        SerialMonitor->setFont(font);
        checkBoxHex = new QCheckBox(SerialMonitor);
        checkBoxHex->setObjectName("checkBoxHex");
        checkBoxHex->setGeometry(QRect(10, 420, 85, 20));
        textEdit = new QTextEdit(SerialMonitor);
        textEdit->setObjectName("textEdit");
        textEdit->setGeometry(QRect(10, 10, 800, 400));
        textEdit->setFont(font);
        textEdit->setReadOnly(true);
        txCheckBox = new QCheckBox(SerialMonitor);
        txCheckBox->setObjectName("txCheckBox");
        txCheckBox->setGeometry(QRect(720, 420, 41, 20));
        txCheckBox->setChecked(true);
        rxCheckBox = new QCheckBox(SerialMonitor);
        rxCheckBox->setObjectName("rxCheckBox");
        rxCheckBox->setGeometry(QRect(770, 420, 41, 20));
        rxCheckBox->setChecked(true);
        clearButton = new QPushButton(SerialMonitor);
        clearButton->setObjectName("clearButton");
        clearButton->setGeometry(QRect(670, 415, 41, 32));

        retranslateUi(SerialMonitor);

        QMetaObject::connectSlotsByName(SerialMonitor);
    } // setupUi

    void retranslateUi(QWidget *SerialMonitor)
    {
        SerialMonitor->setWindowTitle(QCoreApplication::translate("SerialMonitor", "Form", nullptr));
        checkBoxHex->setText(QCoreApplication::translate("SerialMonitor", "Hex", nullptr));
        txCheckBox->setText(QCoreApplication::translate("SerialMonitor", "TX", nullptr));
        rxCheckBox->setText(QCoreApplication::translate("SerialMonitor", "RX", nullptr));
        clearButton->setText(QCoreApplication::translate("SerialMonitor", "CLR", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SerialMonitor: public Ui_SerialMonitor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERIALMONITOR_H
