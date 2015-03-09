#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    knn = new GLANN(512,1,this);

    ui->verticalLayout->addWidget(knn);
}

MainWindow::~MainWindow()
{
    delete ui;
}
