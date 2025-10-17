/********************************************************************************
** Form generated from reading UI file 'debugmonitor.ui'
**
** Created by: Qt User Interface Compiler version 6.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEBUGMONITOR_H
#define UI_DEBUGMONITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_debugMonitor
{
public:

    void setupUi(QWidget *debugMonitor)
    {
        if (debugMonitor->objectName().isEmpty())
            debugMonitor->setObjectName("debugMonitor");
        debugMonitor->resize(640, 480);

        retranslateUi(debugMonitor);

        QMetaObject::connectSlotsByName(debugMonitor);
    } // setupUi

    void retranslateUi(QWidget *debugMonitor)
    {
        debugMonitor->setWindowTitle(QCoreApplication::translate("debugMonitor", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class debugMonitor: public Ui_debugMonitor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEBUGMONITOR_H
