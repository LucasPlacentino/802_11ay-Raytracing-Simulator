#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    printf("MainWindow - Class init\n");
    ui->setupUi(this);

    printf("MainWindow - Class init end\n");
}

MainWindow::~MainWindow()
{
    delete ui;
}
