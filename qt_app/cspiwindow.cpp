#include "cspiwindow.h"
#include "ui_cspiwindow.h"

CSPIWindow::CSPIWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CSPIWindow)
{
    ui->setupUi(this);

    connect(ui->lineEdit_threshold, &QLineEdit::textChanged,
            this, &CSPIWindow::updateTxHighlight);
    connect(ui->textEdit_tx_data, &QTextEdit::textChanged,
            this, &CSPIWindow::updateTxHighlight);
    connect(ui->spinBox_transfer_size,   &QSpinBox::valueChanged,
            this, &CSPIWindow::updateTxHighlight);
}

CSPIWindow::~CSPIWindow()
{
    delete ui;
}



