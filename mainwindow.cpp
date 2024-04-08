#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDesktopServices>
#include <QProcess>
#include <stdio.h>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>

// https://doc.qt.io/qt-6/graphicsview.html (replaces QCanvas) :
#include <QGraphicsScene> // The QGraphicsScene class provides a surface for managing a large number of 2D graphical items.
#include <QGraphicsView> // displays the content of the QGraphicsScene

////#include <QPainter> // use QPainter to render the floorplan and rays ?

#include "simulation.h"

Simulation simulation; // use `extern Simulation simulation;` in other files?

int currentEditingBaseStation_index = 0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    initFirstBaseStation();
    ui->setupUi(this);
    showFirstBaseStation();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_runSimulationButton_clicked()
{
    // TODO: run whole simulation
    qInfo("Starting simulation");
    bool res = runSimulation();
    // TODO: show a small simualtion ran successfully text
    ui->runSuccessOrFailText->setStyleSheet(res ? "color: green;" : "color: red;");
    ui->runSuccessOrFailText->setText(res ? "Success": "Failed");
}

bool MainWindow::runSimulation()
{
    try {
        // TODO
        ////simulation = Simulation(); //do not override the object, change/reset global one

        qInfo("Simulation ended successfully");
        return true;
    } catch (...) {
        qInfo("Simulation ended unsuccessfully");
        //qError(error);
        return false;
    }
}

void MainWindow::changeBaseStationPower(int value)
{
    // TODO: change value for this specific base station
    Transmitter* base_station = simulation.getBaseStation(currentEditingBaseStation_index);
    base_station->setPower_dBm(value);
    showBaseStationPower(value);
}

void MainWindow::on_sliderBaseStationPower_valueChanged(int value)
{
    changeBaseStationPower(value);
}

void MainWindow::on_spinBoxBaseStationPower_valueChanged(int value)
{
    changeBaseStationPower(value);
}


void MainWindow::on_actionExit_triggered()
{
    printf("Closing app...\n");
    qApp->quit();
}


void MainWindow::on_actionReset_triggered()
{
    qInfo("Resetting all values and restarting app...");

    // TODO: reset all user input values to default/reset app
    simulation.resetAll();

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
    if (simulation.ran) // checks if a simulation has run
    {
        // TODO: save image from the simulation frame window

    } else { // no simulation has already run
        // do nothing?
    }
    */

    // get the simulation Scene ?
    QGraphicsScene* scene = &simulation.simulation_scene;

    //size = scene->sceneRect().size().toSize(); // get the Scene size
    QImage img(size(), QImage::Format_ARGB32); // scene's size, Format_RGBA64 ?
    QPainter painter(&img);
    painter.setRenderHint(QPainter::Antialiasing);
    // renderscene() //&painter //? scene->render(&painter);
    //scene->render(&painter);

    QString img_filename= QFileDialog::getSaveFileName(
        this,
        tr("Save Image"),
        QDir::currentPath(),
        "PNG (*.png);;BMP Files (*.bmp);;JPEG (*.JPEG)"
    );
    bool success = img.save(img_filename);
    success? qInfo("Image saved") : qInfo("Cancelled");

}


void MainWindow::initFirstBaseStation() {
    //TODO: create a new transmitter object
    simulation.createBaseStation(
        Transmitter(0, "Base Station 1", 20, QPoint(1,1)) // TODO: QPoint
        );
}

void MainWindow::showFirstBaseStation() {
    QString new_item = QString("Base Station 1");
    ui->transmitterSelector->addItem(new_item);
    ui->transmitterSelector->setCurrentIndex(0);
    on_transmitterSelector_activated(0);
}

void MainWindow::showBaseStationPower(int value_dBm) {
    ui->spinBoxBaseStationPower->setValue(value_dBm);
    ui->sliderBaseStationPower->setValue(value_dBm);
}

void MainWindow::on_transmitterSelector_activated(int index)
{
    qDebug() << "Selected base station: " << index+1;

    // set current editing transmitter to this one
    currentEditingBaseStation_index = index;
    showBaseStationPower(simulation.getBaseStation(currentEditingBaseStation_index)->getPower_dBm());
    //changeBaseStationPower(simulation.getBaseStation(currentEditingBaseStation_index).getPower_dBm());
}


void MainWindow::on_addTransmitterButton_clicked()
{
    if (ui->transmitterSelector->count() < ui->transmitterSelector->maxCount()) //check if has not reached the max number of transmitters
    {
        qDebug("Added base station");

        //TODO: create a new transmitter object
        QString new_item = QString("Base Station %1").arg(ui->transmitterSelector->count()+1);
        ui->transmitterSelector->addItem(new_item);

        int new_item_index = ui->transmitterSelector->findText(new_item);

        simulation.createBaseStation(Transmitter(new_item_index, new_item, 20, QPoint(1,1))); // TODO: QPoint

        on_transmitterSelector_activated(new_item_index);
        ui->transmitterSelector->setCurrentIndex(new_item_index);


    } else {
        qInfo("There is already the maximum number of base stations");
    }
}


void MainWindow::on_deleteBaseStationPushButton_clicked()
{
    //currentEditingBaseStation_index
    if (ui->transmitterSelector->count()>1 && currentEditingBaseStation_index != 0)
    {
        ui->transmitterSelector->removeItem(currentEditingBaseStation_index);
        simulation.deleteBaseStation(currentEditingBaseStation_index);

        int previous_item_index = currentEditingBaseStation_index-1;
        on_transmitterSelector_activated(previous_item_index);
        ui->transmitterSelector->setCurrentIndex(previous_item_index);
    } else {
        qDebug("Cannot delete Base Station 1");
    }
}

