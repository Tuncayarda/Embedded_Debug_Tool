/********************************************************************************
** Form generated from reading UI file 'addactionwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDACTIONWINDOW_H
#define UI_ADDACTIONWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AddActionWindow
{
public:
    QPushButton *addButton;
    QComboBox *actionBox;
    QStackedWidget *stackedWidget;
    QWidget *delay_page;
    QFrame *frame_delay;
    QLineEdit *lineEdit_duration_d;
    QLabel *label_duration_d;
    QLineEdit *lineEdit_dependency_d;
    QLabel *label_dependency_d;
    QWidget *pin_read_page;
    QFrame *frame_read;
    QWidget *w_final_r;
    QLabel *label_rfinal;
    QRadioButton *finalRHButton;
    QRadioButton *finalRLButton;
    QWidget *w_initial_r;
    QRadioButton *initialRLButton;
    QLabel *label_rinitial;
    QRadioButton *initialRHButton;
    QWidget *w_target_r;
    QRadioButton *targetRLButton;
    QRadioButton *targetRHButton;
    QLabel *label_rtarget;
    QLabel *label_pin_no_r;
    QLabel *label_duration_r;
    QLineEdit *lineEdit_duration_r;
    QSpinBox *spinBox_r;
    QLineEdit *lineEdit_dependency_r;
    QLabel *label_dependency_r;
    QLabel *label_port_r;
    QComboBox *comboBox_port_r;
    QWidget *pin_write_page;
    QFrame *frame_write;
    QWidget *w_final_w;
    QRadioButton *finalWLButton;
    QRadioButton *finalWHButton;
    QLabel *label_wfinal;
    QWidget *w_initial_w;
    QRadioButton *initialWLButton;
    QRadioButton *initialWHButton;
    QLabel *label_winitial;
    QWidget *w_target_w;
    QRadioButton *targetWLButton;
    QRadioButton *targetWHButton;
    QLabel *label_wtarget;
    QSpinBox *spinBox_w;
    QLineEdit *lineEdit_dependency_w;
    QLabel *label_duration_w;
    QLabel *label_dependency_w;
    QLineEdit *lineEdit_duration_w;
    QLabel *label_pin_no_w;
    QComboBox *comboBox_port_w;
    QLabel *label_port_r_w;
    QWidget *pin_trigger_page;
    QFrame *frame_trigger;
    QWidget *w_target_t;
    QRadioButton *targetTLButton;
    QRadioButton *targetTHButton;
    QLabel *label_ttarget;
    QLabel *label_pin_no_t;
    QWidget *w_initial_t;
    QRadioButton *initialTLButton;
    QLabel *label_tinitial;
    QRadioButton *initialTHButton;
    QSpinBox *spinBox_t;
    QComboBox *comboBox_port_t;
    QLabel *label_port_t;
    QLabel *label_timeout_t;
    QLineEdit *lineEdit_timeout_t;
    QLineEdit *lineEdit_dependency_t;
    QLabel *label_dependency_t;
    QPushButton *cancelButton;

    void setupUi(QDialog *AddActionWindow)
    {
        if (AddActionWindow->objectName().isEmpty())
            AddActionWindow->setObjectName("AddActionWindow");
        AddActionWindow->resize(240, 320);
        addButton = new QPushButton(AddActionWindow);
        addButton->setObjectName("addButton");
        addButton->setGeometry(QRect(130, 280, 90, 32));
        actionBox = new QComboBox(AddActionWindow);
        actionBox->addItem(QString());
        actionBox->addItem(QString());
        actionBox->addItem(QString());
        actionBox->addItem(QString());
        actionBox->setObjectName("actionBox");
        actionBox->setGeometry(QRect(10, 10, 121, 32));
        stackedWidget = new QStackedWidget(AddActionWindow);
        stackedWidget->setObjectName("stackedWidget");
        stackedWidget->setGeometry(QRect(20, 50, 201, 221));
        delay_page = new QWidget();
        delay_page->setObjectName("delay_page");
        frame_delay = new QFrame(delay_page);
        frame_delay->setObjectName("frame_delay");
        frame_delay->setGeometry(QRect(0, 0, 201, 221));
        frame_delay->setFrameShape(QFrame::Shape::StyledPanel);
        frame_delay->setFrameShadow(QFrame::Shadow::Raised);
        lineEdit_duration_d = new QLineEdit(frame_delay);
        lineEdit_duration_d->setObjectName("lineEdit_duration_d");
        lineEdit_duration_d->setGeometry(QRect(100, 20, 91, 21));
        label_duration_d = new QLabel(frame_delay);
        label_duration_d->setObjectName("label_duration_d");
        label_duration_d->setGeometry(QRect(10, 20, 81, 16));
        lineEdit_dependency_d = new QLineEdit(frame_delay);
        lineEdit_dependency_d->setObjectName("lineEdit_dependency_d");
        lineEdit_dependency_d->setGeometry(QRect(100, 50, 91, 21));
        label_dependency_d = new QLabel(frame_delay);
        label_dependency_d->setObjectName("label_dependency_d");
        label_dependency_d->setGeometry(QRect(10, 50, 81, 16));
        stackedWidget->addWidget(delay_page);
        pin_read_page = new QWidget();
        pin_read_page->setObjectName("pin_read_page");
        frame_read = new QFrame(pin_read_page);
        frame_read->setObjectName("frame_read");
        frame_read->setGeometry(QRect(0, 0, 201, 221));
        frame_read->setFrameShape(QFrame::Shape::StyledPanel);
        frame_read->setFrameShadow(QFrame::Shadow::Raised);
        w_final_r = new QWidget(frame_read);
        w_final_r->setObjectName("w_final_r");
        w_final_r->setGeometry(QRect(0, 120, 201, 31));
        label_rfinal = new QLabel(w_final_r);
        label_rfinal->setObjectName("label_rfinal");
        label_rfinal->setGeometry(QRect(10, 10, 91, 16));
        finalRHButton = new QRadioButton(w_final_r);
        finalRHButton->setObjectName("finalRHButton");
        finalRHButton->setGeometry(QRect(100, 10, 31, 20));
        finalRLButton = new QRadioButton(w_final_r);
        finalRLButton->setObjectName("finalRLButton");
        finalRLButton->setGeometry(QRect(150, 10, 31, 20));
        w_initial_r = new QWidget(frame_read);
        w_initial_r->setObjectName("w_initial_r");
        w_initial_r->setGeometry(QRect(0, 70, 201, 31));
        initialRLButton = new QRadioButton(w_initial_r);
        initialRLButton->setObjectName("initialRLButton");
        initialRLButton->setGeometry(QRect(150, 0, 31, 20));
        label_rinitial = new QLabel(w_initial_r);
        label_rinitial->setObjectName("label_rinitial");
        label_rinitial->setGeometry(QRect(10, 0, 91, 16));
        initialRHButton = new QRadioButton(w_initial_r);
        initialRHButton->setObjectName("initialRHButton");
        initialRHButton->setGeometry(QRect(100, 0, 31, 20));
        w_target_r = new QWidget(frame_read);
        w_target_r->setObjectName("w_target_r");
        w_target_r->setGeometry(QRect(0, 100, 201, 31));
        targetRLButton = new QRadioButton(w_target_r);
        targetRLButton->setObjectName("targetRLButton");
        targetRLButton->setGeometry(QRect(150, 0, 31, 20));
        targetRHButton = new QRadioButton(w_target_r);
        targetRHButton->setObjectName("targetRHButton");
        targetRHButton->setGeometry(QRect(100, 0, 31, 20));
        label_rtarget = new QLabel(w_target_r);
        label_rtarget->setObjectName("label_rtarget");
        label_rtarget->setGeometry(QRect(10, 0, 91, 16));
        label_pin_no_r = new QLabel(frame_read);
        label_pin_no_r->setObjectName("label_pin_no_r");
        label_pin_no_r->setGeometry(QRect(10, 40, 81, 16));
        label_duration_r = new QLabel(frame_read);
        label_duration_r->setObjectName("label_duration_r");
        label_duration_r->setGeometry(QRect(10, 160, 81, 16));
        lineEdit_duration_r = new QLineEdit(frame_read);
        lineEdit_duration_r->setObjectName("lineEdit_duration_r");
        lineEdit_duration_r->setGeometry(QRect(100, 160, 91, 21));
        spinBox_r = new QSpinBox(frame_read);
        spinBox_r->setObjectName("spinBox_r");
        spinBox_r->setGeometry(QRect(100, 40, 91, 22));
        spinBox_r->setDisplayIntegerBase(10);
        lineEdit_dependency_r = new QLineEdit(frame_read);
        lineEdit_dependency_r->setObjectName("lineEdit_dependency_r");
        lineEdit_dependency_r->setGeometry(QRect(100, 190, 91, 21));
        label_dependency_r = new QLabel(frame_read);
        label_dependency_r->setObjectName("label_dependency_r");
        label_dependency_r->setGeometry(QRect(10, 190, 81, 16));
        label_port_r = new QLabel(frame_read);
        label_port_r->setObjectName("label_port_r");
        label_port_r->setGeometry(QRect(10, 10, 91, 16));
        comboBox_port_r = new QComboBox(frame_read);
        comboBox_port_r->addItem(QString());
        comboBox_port_r->addItem(QString());
        comboBox_port_r->addItem(QString());
        comboBox_port_r->addItem(QString());
        comboBox_port_r->addItem(QString());
        comboBox_port_r->addItem(QString());
        comboBox_port_r->setObjectName("comboBox_port_r");
        comboBox_port_r->setGeometry(QRect(95, 5, 101, 32));
        stackedWidget->addWidget(pin_read_page);
        pin_write_page = new QWidget();
        pin_write_page->setObjectName("pin_write_page");
        frame_write = new QFrame(pin_write_page);
        frame_write->setObjectName("frame_write");
        frame_write->setGeometry(QRect(0, 0, 201, 221));
        frame_write->setFrameShape(QFrame::Shape::StyledPanel);
        frame_write->setFrameShadow(QFrame::Shadow::Raised);
        w_final_w = new QWidget(frame_write);
        w_final_w->setObjectName("w_final_w");
        w_final_w->setGeometry(QRect(0, 130, 201, 31));
        finalWLButton = new QRadioButton(w_final_w);
        finalWLButton->setObjectName("finalWLButton");
        finalWLButton->setGeometry(QRect(150, 0, 31, 20));
        finalWHButton = new QRadioButton(w_final_w);
        finalWHButton->setObjectName("finalWHButton");
        finalWHButton->setGeometry(QRect(100, 0, 31, 20));
        label_wfinal = new QLabel(w_final_w);
        label_wfinal->setObjectName("label_wfinal");
        label_wfinal->setGeometry(QRect(10, 0, 91, 16));
        w_initial_w = new QWidget(frame_write);
        w_initial_w->setObjectName("w_initial_w");
        w_initial_w->setGeometry(QRect(0, 70, 201, 31));
        initialWLButton = new QRadioButton(w_initial_w);
        initialWLButton->setObjectName("initialWLButton");
        initialWLButton->setGeometry(QRect(150, 0, 31, 20));
        initialWHButton = new QRadioButton(w_initial_w);
        initialWHButton->setObjectName("initialWHButton");
        initialWHButton->setGeometry(QRect(100, 0, 31, 20));
        label_winitial = new QLabel(w_initial_w);
        label_winitial->setObjectName("label_winitial");
        label_winitial->setGeometry(QRect(10, 0, 91, 16));
        w_target_w = new QWidget(frame_write);
        w_target_w->setObjectName("w_target_w");
        w_target_w->setGeometry(QRect(0, 100, 201, 31));
        targetWLButton = new QRadioButton(w_target_w);
        targetWLButton->setObjectName("targetWLButton");
        targetWLButton->setGeometry(QRect(150, 0, 31, 20));
        targetWHButton = new QRadioButton(w_target_w);
        targetWHButton->setObjectName("targetWHButton");
        targetWHButton->setGeometry(QRect(100, 0, 31, 20));
        label_wtarget = new QLabel(w_target_w);
        label_wtarget->setObjectName("label_wtarget");
        label_wtarget->setGeometry(QRect(10, 0, 91, 16));
        spinBox_w = new QSpinBox(frame_write);
        spinBox_w->setObjectName("spinBox_w");
        spinBox_w->setGeometry(QRect(100, 40, 91, 22));
        lineEdit_dependency_w = new QLineEdit(frame_write);
        lineEdit_dependency_w->setObjectName("lineEdit_dependency_w");
        lineEdit_dependency_w->setGeometry(QRect(100, 190, 91, 21));
        label_duration_w = new QLabel(frame_write);
        label_duration_w->setObjectName("label_duration_w");
        label_duration_w->setGeometry(QRect(10, 160, 81, 16));
        label_dependency_w = new QLabel(frame_write);
        label_dependency_w->setObjectName("label_dependency_w");
        label_dependency_w->setGeometry(QRect(10, 190, 81, 16));
        lineEdit_duration_w = new QLineEdit(frame_write);
        lineEdit_duration_w->setObjectName("lineEdit_duration_w");
        lineEdit_duration_w->setGeometry(QRect(100, 160, 91, 21));
        label_pin_no_w = new QLabel(frame_write);
        label_pin_no_w->setObjectName("label_pin_no_w");
        label_pin_no_w->setGeometry(QRect(10, 40, 91, 16));
        comboBox_port_w = new QComboBox(frame_write);
        comboBox_port_w->addItem(QString());
        comboBox_port_w->addItem(QString());
        comboBox_port_w->addItem(QString());
        comboBox_port_w->addItem(QString());
        comboBox_port_w->addItem(QString());
        comboBox_port_w->addItem(QString());
        comboBox_port_w->setObjectName("comboBox_port_w");
        comboBox_port_w->setGeometry(QRect(95, 5, 101, 32));
        label_port_r_w = new QLabel(frame_write);
        label_port_r_w->setObjectName("label_port_r_w");
        label_port_r_w->setGeometry(QRect(10, 10, 91, 16));
        stackedWidget->addWidget(pin_write_page);
        pin_trigger_page = new QWidget();
        pin_trigger_page->setObjectName("pin_trigger_page");
        frame_trigger = new QFrame(pin_trigger_page);
        frame_trigger->setObjectName("frame_trigger");
        frame_trigger->setGeometry(QRect(0, 0, 201, 221));
        frame_trigger->setFrameShape(QFrame::Shape::StyledPanel);
        frame_trigger->setFrameShadow(QFrame::Shadow::Raised);
        w_target_t = new QWidget(frame_trigger);
        w_target_t->setObjectName("w_target_t");
        w_target_t->setGeometry(QRect(0, 90, 201, 31));
        targetTLButton = new QRadioButton(w_target_t);
        targetTLButton->setObjectName("targetTLButton");
        targetTLButton->setGeometry(QRect(150, 0, 31, 20));
        targetTHButton = new QRadioButton(w_target_t);
        targetTHButton->setObjectName("targetTHButton");
        targetTHButton->setGeometry(QRect(100, 0, 31, 20));
        label_ttarget = new QLabel(w_target_t);
        label_ttarget->setObjectName("label_ttarget");
        label_ttarget->setGeometry(QRect(10, 0, 91, 16));
        label_pin_no_t = new QLabel(frame_trigger);
        label_pin_no_t->setObjectName("label_pin_no_t");
        label_pin_no_t->setGeometry(QRect(10, 30, 81, 16));
        w_initial_t = new QWidget(frame_trigger);
        w_initial_t->setObjectName("w_initial_t");
        w_initial_t->setGeometry(QRect(0, 60, 201, 31));
        initialTLButton = new QRadioButton(w_initial_t);
        initialTLButton->setObjectName("initialTLButton");
        initialTLButton->setGeometry(QRect(150, 0, 31, 20));
        label_tinitial = new QLabel(w_initial_t);
        label_tinitial->setObjectName("label_tinitial");
        label_tinitial->setGeometry(QRect(10, 0, 91, 16));
        initialTHButton = new QRadioButton(w_initial_t);
        initialTHButton->setObjectName("initialTHButton");
        initialTHButton->setGeometry(QRect(100, 0, 31, 20));
        spinBox_t = new QSpinBox(frame_trigger);
        spinBox_t->setObjectName("spinBox_t");
        spinBox_t->setGeometry(QRect(100, 30, 91, 22));
        spinBox_t->setDisplayIntegerBase(10);
        comboBox_port_t = new QComboBox(frame_trigger);
        comboBox_port_t->addItem(QString());
        comboBox_port_t->addItem(QString());
        comboBox_port_t->addItem(QString());
        comboBox_port_t->addItem(QString());
        comboBox_port_t->addItem(QString());
        comboBox_port_t->addItem(QString());
        comboBox_port_t->setObjectName("comboBox_port_t");
        comboBox_port_t->setGeometry(QRect(95, -5, 101, 32));
        label_port_t = new QLabel(frame_trigger);
        label_port_t->setObjectName("label_port_t");
        label_port_t->setGeometry(QRect(10, 0, 91, 16));
        label_timeout_t = new QLabel(frame_trigger);
        label_timeout_t->setObjectName("label_timeout_t");
        label_timeout_t->setGeometry(QRect(10, 120, 81, 16));
        lineEdit_timeout_t = new QLineEdit(frame_trigger);
        lineEdit_timeout_t->setObjectName("lineEdit_timeout_t");
        lineEdit_timeout_t->setGeometry(QRect(100, 120, 91, 21));
        lineEdit_dependency_t = new QLineEdit(frame_trigger);
        lineEdit_dependency_t->setObjectName("lineEdit_dependency_t");
        lineEdit_dependency_t->setGeometry(QRect(100, 150, 91, 21));
        label_dependency_t = new QLabel(frame_trigger);
        label_dependency_t->setObjectName("label_dependency_t");
        label_dependency_t->setGeometry(QRect(10, 150, 81, 16));
        stackedWidget->addWidget(pin_trigger_page);
        cancelButton = new QPushButton(AddActionWindow);
        cancelButton->setObjectName("cancelButton");
        cancelButton->setGeometry(QRect(20, 280, 90, 32));

        retranslateUi(AddActionWindow);

        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(AddActionWindow);
    } // setupUi

    void retranslateUi(QDialog *AddActionWindow)
    {
        AddActionWindow->setWindowTitle(QCoreApplication::translate("AddActionWindow", "Dialog", nullptr));
        addButton->setText(QCoreApplication::translate("AddActionWindow", "Add", nullptr));
        actionBox->setItemText(0, QCoreApplication::translate("AddActionWindow", "DELAY", nullptr));
        actionBox->setItemText(1, QCoreApplication::translate("AddActionWindow", "PIN_READ", nullptr));
        actionBox->setItemText(2, QCoreApplication::translate("AddActionWindow", "PIN_WRITE", nullptr));
        actionBox->setItemText(3, QCoreApplication::translate("AddActionWindow", "PIN_TRIGGER", nullptr));

        label_duration_d->setText(QCoreApplication::translate("AddActionWindow", "Duration(ms):", nullptr));
        label_dependency_d->setText(QCoreApplication::translate("AddActionWindow", "Dependency:", nullptr));
        label_rfinal->setText(QCoreApplication::translate("AddActionWindow", "Final Level:", nullptr));
        finalRHButton->setText(QCoreApplication::translate("AddActionWindow", "H", nullptr));
        finalRLButton->setText(QCoreApplication::translate("AddActionWindow", "L", nullptr));
        initialRLButton->setText(QCoreApplication::translate("AddActionWindow", "L", nullptr));
        label_rinitial->setText(QCoreApplication::translate("AddActionWindow", "Initial Level: ", nullptr));
        initialRHButton->setText(QCoreApplication::translate("AddActionWindow", "H", nullptr));
        targetRLButton->setText(QCoreApplication::translate("AddActionWindow", "L", nullptr));
        targetRHButton->setText(QCoreApplication::translate("AddActionWindow", "H", nullptr));
        label_rtarget->setText(QCoreApplication::translate("AddActionWindow", "Target Level:", nullptr));
        label_pin_no_r->setText(QCoreApplication::translate("AddActionWindow", "Input Pin No:", nullptr));
        label_duration_r->setText(QCoreApplication::translate("AddActionWindow", "Duration(ms):", nullptr));
        label_dependency_r->setText(QCoreApplication::translate("AddActionWindow", "Dependency:", nullptr));
        label_port_r->setText(QCoreApplication::translate("AddActionWindow", "Input Pin Port:", nullptr));
        comboBox_port_r->setItemText(0, QCoreApplication::translate("AddActionWindow", "PORT A", nullptr));
        comboBox_port_r->setItemText(1, QCoreApplication::translate("AddActionWindow", "PORT B", nullptr));
        comboBox_port_r->setItemText(2, QCoreApplication::translate("AddActionWindow", "PORT C", nullptr));
        comboBox_port_r->setItemText(3, QCoreApplication::translate("AddActionWindow", "PORT D", nullptr));
        comboBox_port_r->setItemText(4, QCoreApplication::translate("AddActionWindow", "PORT E", nullptr));
        comboBox_port_r->setItemText(5, QCoreApplication::translate("AddActionWindow", "PORT F", nullptr));

        finalWLButton->setText(QCoreApplication::translate("AddActionWindow", "L", nullptr));
        finalWHButton->setText(QCoreApplication::translate("AddActionWindow", "H", nullptr));
        label_wfinal->setText(QCoreApplication::translate("AddActionWindow", "Final Level:", nullptr));
        initialWLButton->setText(QCoreApplication::translate("AddActionWindow", "L", nullptr));
        initialWHButton->setText(QCoreApplication::translate("AddActionWindow", "H", nullptr));
        label_winitial->setText(QCoreApplication::translate("AddActionWindow", "Initial Level: ", nullptr));
        targetWLButton->setText(QCoreApplication::translate("AddActionWindow", "L", nullptr));
        targetWHButton->setText(QCoreApplication::translate("AddActionWindow", "H", nullptr));
        label_wtarget->setText(QCoreApplication::translate("AddActionWindow", "Target Level:", nullptr));
        label_duration_w->setText(QCoreApplication::translate("AddActionWindow", "Duration(ms):", nullptr));
        label_dependency_w->setText(QCoreApplication::translate("AddActionWindow", "Dependency:", nullptr));
        label_pin_no_w->setText(QCoreApplication::translate("AddActionWindow", "Output Pin No:", nullptr));
        comboBox_port_w->setItemText(0, QCoreApplication::translate("AddActionWindow", "PORT A", nullptr));
        comboBox_port_w->setItemText(1, QCoreApplication::translate("AddActionWindow", "PORT B", nullptr));
        comboBox_port_w->setItemText(2, QCoreApplication::translate("AddActionWindow", "PORT C", nullptr));
        comboBox_port_w->setItemText(3, QCoreApplication::translate("AddActionWindow", "PORT D", nullptr));
        comboBox_port_w->setItemText(4, QCoreApplication::translate("AddActionWindow", "PORT E", nullptr));
        comboBox_port_w->setItemText(5, QCoreApplication::translate("AddActionWindow", "PORT F", nullptr));

        label_port_r_w->setText(QCoreApplication::translate("AddActionWindow", "Input Pin Port:", nullptr));
        targetTLButton->setText(QCoreApplication::translate("AddActionWindow", "L", nullptr));
        targetTHButton->setText(QCoreApplication::translate("AddActionWindow", "H", nullptr));
        label_ttarget->setText(QCoreApplication::translate("AddActionWindow", "Target Level:", nullptr));
        label_pin_no_t->setText(QCoreApplication::translate("AddActionWindow", "Input Pin No:", nullptr));
        initialTLButton->setText(QCoreApplication::translate("AddActionWindow", "L", nullptr));
        label_tinitial->setText(QCoreApplication::translate("AddActionWindow", "Initial Level: ", nullptr));
        initialTHButton->setText(QCoreApplication::translate("AddActionWindow", "H", nullptr));
        comboBox_port_t->setItemText(0, QCoreApplication::translate("AddActionWindow", "PORT A", nullptr));
        comboBox_port_t->setItemText(1, QCoreApplication::translate("AddActionWindow", "PORT B", nullptr));
        comboBox_port_t->setItemText(2, QCoreApplication::translate("AddActionWindow", "PORT C", nullptr));
        comboBox_port_t->setItemText(3, QCoreApplication::translate("AddActionWindow", "PORT D", nullptr));
        comboBox_port_t->setItemText(4, QCoreApplication::translate("AddActionWindow", "PORT E", nullptr));
        comboBox_port_t->setItemText(5, QCoreApplication::translate("AddActionWindow", "PORT F", nullptr));

        label_port_t->setText(QCoreApplication::translate("AddActionWindow", "Input Pin Port:", nullptr));
        label_timeout_t->setText(QCoreApplication::translate("AddActionWindow", "Timeout(ms):", nullptr));
        label_dependency_t->setText(QCoreApplication::translate("AddActionWindow", "Dependency:", nullptr));
        cancelButton->setText(QCoreApplication::translate("AddActionWindow", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AddActionWindow: public Ui_AddActionWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDACTIONWINDOW_H
