/********************************************************************************
** Form generated from reading UI file 'cspiwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CSPIWINDOW_H
#define UI_CSPIWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CSPIWindow
{
public:
    QFrame *frame;
    QWidget *widget_mode;
    QRadioButton *radioButton_mode_0;
    QLabel *label_mode;
    QRadioButton *radioButton_mode_1;
    QRadioButton *radioButton_mode_2;
    QRadioButton *radioButton_mode_3;
    QWidget *widget_word;
    QLabel *label_word_size;
    QComboBox *comboBox_word;
    QWidget *widget_data;
    QLabel *label_5_data_tx;
    QTextEdit *textEdit_tx_data;
    QPushButton *setButton;
    QPushButton *clrButton;
    QLabel *label_th_vals;
    QPushButton *importButton;
    QPushButton *stopButton;
    QPushButton *terminateButton;
    QWidget *widget_read_size;
    QLabel *label_read_size;
    QSpinBox *spinBox_read_size;
    QWidget *widget_transfer_size;
    QLabel *label_transfer_size;
    QSpinBox *spinBox_transfer_size;
    QWidget *widget_threshold;
    QLabel *label_threshold;
    QLineEdit *lineEdit_threshold;
    QWidget *widget_alert;
    QLabel *label_alert;
    QSpinBox *spinBox_pin_no;
    QComboBox *comboBox_pin_port;

    void setupUi(QWidget *CSPIWindow)
    {
        if (CSPIWindow->objectName().isEmpty())
            CSPIWindow->setObjectName("CSPIWindow");
        CSPIWindow->resize(260, 433);
        QFont font;
        font.setFamilies({QString::fromUtf8("Menlo")});
        font.setPointSize(12);
        CSPIWindow->setFont(font);
        frame = new QFrame(CSPIWindow);
        frame->setObjectName("frame");
        frame->setGeometry(QRect(10, 10, 240, 411));
        frame->setFrameShape(QFrame::Shape::StyledPanel);
        frame->setFrameShadow(QFrame::Shadow::Raised);
        widget_mode = new QWidget(frame);
        widget_mode->setObjectName("widget_mode");
        widget_mode->setGeometry(QRect(10, 10, 221, 31));
        radioButton_mode_0 = new QRadioButton(widget_mode);
        radioButton_mode_0->setObjectName("radioButton_mode_0");
        radioButton_mode_0->setGeometry(QRect(60, 10, 31, 20));
        radioButton_mode_0->setChecked(true);
        label_mode = new QLabel(widget_mode);
        label_mode->setObjectName("label_mode");
        label_mode->setGeometry(QRect(10, 10, 41, 16));
        radioButton_mode_1 = new QRadioButton(widget_mode);
        radioButton_mode_1->setObjectName("radioButton_mode_1");
        radioButton_mode_1->setGeometry(QRect(100, 10, 31, 20));
        radioButton_mode_2 = new QRadioButton(widget_mode);
        radioButton_mode_2->setObjectName("radioButton_mode_2");
        radioButton_mode_2->setGeometry(QRect(140, 10, 31, 20));
        radioButton_mode_3 = new QRadioButton(widget_mode);
        radioButton_mode_3->setObjectName("radioButton_mode_3");
        radioButton_mode_3->setGeometry(QRect(180, 10, 31, 20));
        widget_word = new QWidget(frame);
        widget_word->setObjectName("widget_word");
        widget_word->setGeometry(QRect(10, 40, 221, 31));
        label_word_size = new QLabel(widget_word);
        label_word_size->setObjectName("label_word_size");
        label_word_size->setGeometry(QRect(10, 10, 81, 16));
        comboBox_word = new QComboBox(widget_word);
        comboBox_word->addItem(QString());
        comboBox_word->addItem(QString());
        comboBox_word->addItem(QString());
        comboBox_word->addItem(QString());
        comboBox_word->addItem(QString());
        comboBox_word->setObjectName("comboBox_word");
        comboBox_word->setGeometry(QRect(110, 0, 103, 32));
        widget_data = new QWidget(frame);
        widget_data->setObjectName("widget_data");
        widget_data->setGeometry(QRect(10, 190, 221, 211));
        label_5_data_tx = new QLabel(widget_data);
        label_5_data_tx->setObjectName("label_5_data_tx");
        label_5_data_tx->setGeometry(QRect(10, 10, 58, 16));
        textEdit_tx_data = new QTextEdit(widget_data);
        textEdit_tx_data->setObjectName("textEdit_tx_data");
        textEdit_tx_data->setGeometry(QRect(10, 30, 201, 121));
        setButton = new QPushButton(widget_data);
        setButton->setObjectName("setButton");
        setButton->setGeometry(QRect(170, 150, 41, 32));
        clrButton = new QPushButton(widget_data);
        clrButton->setObjectName("clrButton");
        clrButton->setGeometry(QRect(70, 150, 41, 32));
        label_th_vals = new QLabel(widget_data);
        label_th_vals->setObjectName("label_th_vals");
        label_th_vals->setGeometry(QRect(90, 10, 121, 21));
        label_th_vals->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        label_th_vals->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
        importButton = new QPushButton(widget_data);
        importButton->setObjectName("importButton");
        importButton->setGeometry(QRect(10, 150, 51, 32));
        stopButton = new QPushButton(widget_data);
        stopButton->setObjectName("stopButton");
        stopButton->setGeometry(QRect(120, 150, 41, 32));
        terminateButton = new QPushButton(widget_data);
        terminateButton->setObjectName("terminateButton");
        terminateButton->setGeometry(QRect(120, 180, 91, 32));
        widget_read_size = new QWidget(frame);
        widget_read_size->setObjectName("widget_read_size");
        widget_read_size->setGeometry(QRect(10, 100, 221, 31));
        label_read_size = new QLabel(widget_read_size);
        label_read_size->setObjectName("label_read_size");
        label_read_size->setGeometry(QRect(10, 10, 71, 16));
        spinBox_read_size = new QSpinBox(widget_read_size);
        spinBox_read_size->setObjectName("spinBox_read_size");
        spinBox_read_size->setGeometry(QRect(120, 10, 91, 22));
        widget_transfer_size = new QWidget(frame);
        widget_transfer_size->setObjectName("widget_transfer_size");
        widget_transfer_size->setGeometry(QRect(10, 70, 221, 31));
        label_transfer_size = new QLabel(widget_transfer_size);
        label_transfer_size->setObjectName("label_transfer_size");
        label_transfer_size->setGeometry(QRect(10, 10, 101, 16));
        spinBox_transfer_size = new QSpinBox(widget_transfer_size);
        spinBox_transfer_size->setObjectName("spinBox_transfer_size");
        spinBox_transfer_size->setGeometry(QRect(120, 10, 91, 22));
        spinBox_transfer_size->setMinimum(0);
        spinBox_transfer_size->setMaximum(255);
        spinBox_transfer_size->setValue(0);
        widget_threshold = new QWidget(frame);
        widget_threshold->setObjectName("widget_threshold");
        widget_threshold->setGeometry(QRect(10, 130, 221, 31));
        label_threshold = new QLabel(widget_threshold);
        label_threshold->setObjectName("label_threshold");
        label_threshold->setGeometry(QRect(10, 10, 71, 16));
        lineEdit_threshold = new QLineEdit(widget_threshold);
        lineEdit_threshold->setObjectName("lineEdit_threshold");
        lineEdit_threshold->setGeometry(QRect(120, 10, 91, 21));
        widget_alert = new QWidget(frame);
        widget_alert->setObjectName("widget_alert");
        widget_alert->setGeometry(QRect(20, 160, 211, 31));
        label_alert = new QLabel(widget_alert);
        label_alert->setObjectName("label_alert");
        label_alert->setGeometry(QRect(0, 10, 91, 16));
        spinBox_pin_no = new QSpinBox(widget_alert);
        spinBox_pin_no->setObjectName("spinBox_pin_no");
        spinBox_pin_no->setGeometry(QRect(160, 10, 42, 22));
        comboBox_pin_port = new QComboBox(widget_alert);
        comboBox_pin_port->addItem(QString());
        comboBox_pin_port->addItem(QString());
        comboBox_pin_port->addItem(QString());
        comboBox_pin_port->addItem(QString());
        comboBox_pin_port->addItem(QString());
        comboBox_pin_port->addItem(QString());
        comboBox_pin_port->setObjectName("comboBox_pin_port");
        comboBox_pin_port->setGeometry(QRect(105, 10, 55, 25));

        retranslateUi(CSPIWindow);

        comboBox_word->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(CSPIWindow);
    } // setupUi

    void retranslateUi(QWidget *CSPIWindow)
    {
        CSPIWindow->setWindowTitle(QCoreApplication::translate("CSPIWindow", "Form", nullptr));
        radioButton_mode_0->setText(QCoreApplication::translate("CSPIWindow", "0", nullptr));
        label_mode->setText(QCoreApplication::translate("CSPIWindow", "Mode:", nullptr));
        radioButton_mode_1->setText(QCoreApplication::translate("CSPIWindow", "1", nullptr));
        radioButton_mode_2->setText(QCoreApplication::translate("CSPIWindow", "2", nullptr));
        radioButton_mode_3->setText(QCoreApplication::translate("CSPIWindow", "3", nullptr));
        label_word_size->setText(QCoreApplication::translate("CSPIWindow", "Word Size:", nullptr));
        comboBox_word->setItemText(0, QCoreApplication::translate("CSPIWindow", "4 Bit", nullptr));
        comboBox_word->setItemText(1, QCoreApplication::translate("CSPIWindow", "8 Bit", nullptr));
        comboBox_word->setItemText(2, QCoreApplication::translate("CSPIWindow", "16 Bit", nullptr));
        comboBox_word->setItemText(3, QCoreApplication::translate("CSPIWindow", "32 Bit", nullptr));
        comboBox_word->setItemText(4, QCoreApplication::translate("CSPIWindow", "64 Bit", nullptr));

        label_5_data_tx->setText(QCoreApplication::translate("CSPIWindow", "Data TX:", nullptr));
        setButton->setText(QCoreApplication::translate("CSPIWindow", "SET", nullptr));
        clrButton->setText(QCoreApplication::translate("CSPIWindow", "CLR", nullptr));
        label_th_vals->setText(QCoreApplication::translate("CSPIWindow", "0/0", nullptr));
        importButton->setText(QCoreApplication::translate("CSPIWindow", "Import", nullptr));
        stopButton->setText(QCoreApplication::translate("CSPIWindow", "STP", nullptr));
        terminateButton->setText(QCoreApplication::translate("CSPIWindow", "Terminate", nullptr));
        label_read_size->setText(QCoreApplication::translate("CSPIWindow", "Read Size:", nullptr));
        label_transfer_size->setText(QCoreApplication::translate("CSPIWindow", "Transfer Size:", nullptr));
        label_threshold->setText(QCoreApplication::translate("CSPIWindow", "Threshold:", nullptr));
        lineEdit_threshold->setText(QCoreApplication::translate("CSPIWindow", "00", nullptr));
        label_alert->setText(QCoreApplication::translate("CSPIWindow", "Alert Pin:", nullptr));
        comboBox_pin_port->setItemText(0, QCoreApplication::translate("CSPIWindow", "A", nullptr));
        comboBox_pin_port->setItemText(1, QCoreApplication::translate("CSPIWindow", "B", nullptr));
        comboBox_pin_port->setItemText(2, QCoreApplication::translate("CSPIWindow", "C", nullptr));
        comboBox_pin_port->setItemText(3, QCoreApplication::translate("CSPIWindow", "D", nullptr));
        comboBox_pin_port->setItemText(4, QCoreApplication::translate("CSPIWindow", "E", nullptr));
        comboBox_pin_port->setItemText(5, QCoreApplication::translate("CSPIWindow", "F", nullptr));

    } // retranslateUi

};

namespace Ui {
    class CSPIWindow: public Ui_CSPIWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CSPIWINDOW_H
