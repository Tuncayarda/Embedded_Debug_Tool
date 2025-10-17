#include "actionwindow.h"
#include "ui_actionwindow.h"
#include <QStringList>
#include <QRadioButton>

AddActionWindow::AddActionWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddActionWindow)
{
    ui->setupUi(this);

    on_actionBox_currentIndexChanged(ui->actionBox->currentIndex());

    ui->initialRHButton->setChecked(true);
    ui->targetRHButton->setChecked(true);
    ui->finalRHButton->setChecked(true);

    ui->initialWHButton->setChecked(true);
    ui->targetWHButton->setChecked(true);
    ui->finalWHButton->setChecked(true);

    ui->initialTHButton->setChecked(true);
    ui->targetTHButton->setChecked(true);

    connect(ui->actionBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            ui->stackedWidget, &QStackedWidget::setCurrentIndex);

    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->addButton,    &QPushButton::clicked, this, &QDialog::accept);
}

AddActionWindow::~AddActionWindow()
{
    delete ui;
}

void AddActionWindow::on_actionBox_currentIndexChanged(int idx)
{
    ui->stackedWidget->setCurrentIndex(idx);
}
