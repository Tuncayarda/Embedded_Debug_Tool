/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QFrame *frame;
    QLabel *actionListLabel;
    QPushButton *addActionButton;
    QListWidget *actionList;
    QPushButton *executeButton;
    QPushButton *editActionButton;
    QPushButton *delActionButton;
    QPushButton *wFlashButton;
    QPushButton *cFlashButton;
    QCheckBox *flashBootcheckBox;
    QPushButton *serialMonitorButton;
    QComboBox *comboBox_port;
    QPushButton *connButton;
    QPushButton *resetButton;
    QPushButton *cSPIButton;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 350);
        QFont font;
        font.setFamilies({QString::fromUtf8("Menlo")});
        font.setPointSize(11);
        font.setBold(false);
        MainWindow->setFont(font);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        frame = new QFrame(centralwidget);
        frame->setObjectName("frame");
        frame->setGeometry(QRect(10, 50, 781, 261));
        frame->setFont(font);
        frame->setAutoFillBackground(false);
        frame->setFrameShape(QFrame::Shape::StyledPanel);
        frame->setFrameShadow(QFrame::Shadow::Raised);
        actionListLabel = new QLabel(frame);
        actionListLabel->setObjectName("actionListLabel");
        actionListLabel->setGeometry(QRect(10, 10, 91, 16));
        addActionButton = new QPushButton(frame);
        addActionButton->setObjectName("addActionButton");
        addActionButton->setGeometry(QRect(10, 220, 90, 32));
        actionList = new QListWidget(frame);
        actionList->setObjectName("actionList");
        actionList->setGeometry(QRect(10, 30, 761, 181));
        actionList->setResizeMode(QListView::ResizeMode::Adjust);
        actionList->setViewMode(QListView::ViewMode::ListMode);
        executeButton = new QPushButton(frame);
        executeButton->setObjectName("executeButton");
        executeButton->setGeometry(QRect(680, 220, 90, 32));
        editActionButton = new QPushButton(frame);
        editActionButton->setObjectName("editActionButton");
        editActionButton->setEnabled(false);
        editActionButton->setGeometry(QRect(110, 220, 90, 32));
        delActionButton = new QPushButton(frame);
        delActionButton->setObjectName("delActionButton");
        delActionButton->setEnabled(false);
        delActionButton->setGeometry(QRect(210, 220, 90, 32));
        wFlashButton = new QPushButton(frame);
        wFlashButton->setObjectName("wFlashButton");
        wFlashButton->setGeometry(QRect(480, 220, 90, 32));
        cFlashButton = new QPushButton(frame);
        cFlashButton->setObjectName("cFlashButton");
        cFlashButton->setGeometry(QRect(580, 220, 90, 32));
        flashBootcheckBox = new QCheckBox(frame);
        flashBootcheckBox->setObjectName("flashBootcheckBox");
        flashBootcheckBox->setGeometry(QRect(680, 10, 101, 20));
        serialMonitorButton = new QPushButton(centralwidget);
        serialMonitorButton->setObjectName("serialMonitorButton");
        serialMonitorButton->setGeometry(QRect(670, 10, 120, 32));
        comboBox_port = new QComboBox(centralwidget);
        comboBox_port->setObjectName("comboBox_port");
        comboBox_port->setGeometry(QRect(10, 10, 261, 32));
        connButton = new QPushButton(centralwidget);
        connButton->setObjectName("connButton");
        connButton->setGeometry(QRect(280, 10, 100, 32));
        resetButton = new QPushButton(centralwidget);
        resetButton->setObjectName("resetButton");
        resetButton->setGeometry(QRect(610, 10, 50, 32));
        cSPIButton = new QPushButton(centralwidget);
        cSPIButton->setObjectName("cSPIButton");
        cSPIButton->setGeometry(QRect(550, 10, 50, 32));
        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionListLabel->setText(QCoreApplication::translate("MainWindow", "Action List", nullptr));
        addActionButton->setText(QCoreApplication::translate("MainWindow", "Add Action", nullptr));
        executeButton->setText(QCoreApplication::translate("MainWindow", "Execute", nullptr));
        editActionButton->setText(QCoreApplication::translate("MainWindow", "Edit Action", nullptr));
        delActionButton->setText(QCoreApplication::translate("MainWindow", "Del Action", nullptr));
        wFlashButton->setText(QCoreApplication::translate("MainWindow", "Write Flash", nullptr));
        cFlashButton->setText(QCoreApplication::translate("MainWindow", "Clear Flash", nullptr));
        flashBootcheckBox->setText(QCoreApplication::translate("MainWindow", "Flash Boot", nullptr));
        serialMonitorButton->setText(QCoreApplication::translate("MainWindow", "Serial Monitor", nullptr));
        connButton->setText(QCoreApplication::translate("MainWindow", "Connect", nullptr));
        resetButton->setText(QCoreApplication::translate("MainWindow", "RESET", nullptr));
        cSPIButton->setText(QCoreApplication::translate("MainWindow", "cSPI", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
