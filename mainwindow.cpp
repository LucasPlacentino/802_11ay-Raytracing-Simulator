#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDesktopServices>
#include <QProcess>
#include <stdio.h>
#include <QMessageBox>


//Simulation simulation = NULL;

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

void MainWindow::on_runSimulationButton_clicked()
{
    // TODO: run whole simulation
    printf("Starting simulation");
    bool res = runSimulation();
    // TODO: show a small simualtion ran successfully text
    ui->runSuccessOrFailText->setStyleSheet(res ? "color: green;" : "color: red;");
    ui->runSuccessOrFailText->setText(res ? "Success": "Failed");
}

bool MainWindow::runSimulation()
{
    try {
        // TODO
        //simulation = new Simulation.create();
        return true;
    } catch (...) {
        return false;
    }
}



void MainWindow::on_sliderBaseStationPower_valueChanged(int value)
{
    ui->sliderBaseStationPowerValueText->setText(QString::number(value));
}


void MainWindow::on_actionExit_triggered()
{
    // TODO: close app
    qApp->quit();
}


void MainWindow::on_actionReset_triggered()
{
    // TODO: reset all user input values to default/reset app
    qApp->quit();
    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
}


void MainWindow::on_actionSee_Github_triggered()
{
    // TODO: open github repo
    QDesktopServices::openUrl(QUrl("https://github.com/LucasPlacentino/802_11ay-Raytracing-Simulator", QUrl::TolerantMode));
}


void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About this application"),
                       tr("This <b>802.11ay Raytracing Simulator</b> was made as a "
                          "project for the course <b>ELEC-H304</b> Telecommunications Physics "
                          "at <b>École polytechnique de Bruxelles - ULB</b>."
                          "<br>This simulator uses..."));
}


void MainWindow::on_actionSave_image_triggered()
{
    /*
    if (simulation.ran)
    {
        // TODO: save image from the simulation frame window
    } else {
        // do nothing?
    }
    */
}

