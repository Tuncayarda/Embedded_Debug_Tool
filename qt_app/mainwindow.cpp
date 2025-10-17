#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    connect(cspiWin, &CSPIWindow::payloadReady,
            this,    &MainWindow::onCspiPayloadReady);

    ui->setupUi(this);

    portTimer = new QTimer(this);
    connect(portTimer, &QTimer::timeout, this, &MainWindow::refreshPorts);
    portTimer->start(1000);
    refreshPorts();

    rebuildList();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionList_currentRowChanged(int row)
{
    bool editable = false;
    bool deletable = false;

    if (row >= 0 && row < (int)actionSet.nodes().size()) {
        Action* a = actionSet.nodes()[row].get();
        editable  = (a->kind != Kind::START);
        deletable = (a->kind != Kind::START);
    }

    ui->editActionButton->setEnabled(editable);
    ui->delActionButton->setEnabled(deletable);
}


