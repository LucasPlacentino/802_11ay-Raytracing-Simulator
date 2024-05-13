#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDesktopServices>
#include <QProcess>
#include <stdio.h>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>

#include "simulation.h"
#include "tp4.h"
QGraphicsView* TP4view;

Simulation simulation = Simulation(); // The global simulation object, use `extern Simulation simulation;` in other files?

int currentEditingBaseStation_index = 0; // The base station that is currently selected for user edit

QList<qreal> resolutions = {0.5, 0.25, 0.125, 0.0625};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // MainWindow constructor, is ran on program's UI launch
    this->setWindowIcon(QIcon(":/assets/icon.png"));
    initFirstBaseStation();
    ui->setupUi(this);
    showFirstBaseStation();

    toggleCoverageParametersLayout(true);
    toggleCellParametersLayout(false);

    // //SimulationGraphicsScene* simulation_scene = new SimulationGraphicsScene(this);
    // //QGraphicsScene* simulation_scene = new QGraphicsScene(this);
    // //simulation.scene = simulation_scene;
    // //simulation.scene->setSceneRect(QRectF(0,0, 690, 450)); // if not set, QGraphicsScene will use the bounding area of all items, as returned by itemsBoundingRect(), as the scene rect.
    // //qDebug() << "scene pointer (&simulation_scene): " << simulation_scene;
    // //qDebug() << "scene pointer (simulation.scene): " << simulation.scene;
    // // TESTING :
    // //simulation_scene->addRect(20, 20, 60, 60, QPen(Qt::red, 3), QBrush(Qt::green)); simulation_scene->addEllipse(120, 20, 60, 60, QPen(Qt::red, 3), QBrush(Qt::yellow)); simulation_scene->addPolygon(QPolygonF() << QPointF(220, 80) << QPointF(280, 80) << QPointF(250, 20), QPen(Qt::blue, 3), QBrush(Qt::magenta));
    // //?????????????? :
    // //simulation.scene->setBackgroundBrush(Qt::black);
    // //simulation_scene->setBackgroundBrush(QBrush(Qt::black, Qt::SolidPattern));
    // //simulation.scene->setForegroundBrush(Qt::white);
    // //simulation.scene->addText("Hello World!");

    // //simulation.scene->drawScene();

    // //ui->simulationGraphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate); // needed ?
    // //ui->simulationGraphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate); // needed ?

    // //simulation.view = ui->simulationGraphicsView;
    // //qDebug() << "view pointer (ui->simulationGraphicsView): " << &(ui->simulationGraphicsView);
    // //qDebug() << "view pointer (simulation.view): " << &simulation.view;
    // //ui->simulationGraphicsView->setScene(simulation.scene);
    //ui->simulationGraphicsView->setScene(simulation_scene);
    // //qDebug() << "scene (simulation.view->scene()): " << simulation.view->scene();
    // //qDebug() << "scene (ui->simulationGraphicsView->scene()): " << ui->simulationGraphicsView->scene();

    // //ui->simulationGraphicsView->scene()->setSceneRect(ui->simulationGraphicsView->frameRect());
    //ui->simulationGraphicsView->fitInView(simulation_scene->itemsBoundingRect());

    // //simulation.view->setBackgroundBrush(Qt::black);
    // //simulation.view->setBackgroundBrush(QBrush(Qt::black, Qt::SolidPattern));
    // //simulation.view->setForegroundBrush(Qt::white);
    //ui->simulationGraphicsView->setBackgroundBrush(Qt::black);
    //ui->simulationGraphicsView->setBackgroundBrush(QBrush(Qt::black, Qt::SolidPattern));
    //ui->simulationGraphicsView->setForegroundBrush(Qt::white);
    // //simulation.view->scene()->setBackgroundBrush(Qt::black);
    // //simulation.view->scene()->setBackgroundBrush(QBrush(Qt::black, Qt::SolidPattern));
    // //simulation.view->scene()->setForegroundBrush(Qt::white);

    //ui->simulationGraphicsView->setRenderHints(QPainter::Antialiasing); // ?

    // //simulation.view->scene()->update();
    //ui->simulationGraphicsView->scene()->update();
    // ////simulation.view->viewport()->update();
    // ////ui->simulationGraphicsView->viewport()->update();
    // //simulation.view->show();
    //ui->simulationGraphicsView->show();

    for (qreal resolution : resolutions) {
        ui->resolutionComboBox->addItem(QString("%1 m").arg(resolution));
    }
    ui->resolutionComboBox->setCurrentIndex(0);

    this->setWindowIcon(QIcon(":/assets/icon.png"));
}

MainWindow::~MainWindow()
{
    // MainWindow destructor
    delete ui;
}

void MainWindow::on_runSimulationButton_clicked()
{
    // User clicked on the "Run Simulation" button

    /*
    if (simulation.ran) {
        ui->runSuccessOrFailText->setStyleSheet("color: darkorange;");
        ui->runSuccessOrFailText->setText("Please reset: [Menu->Reset App], or (Ctrl+R)");
        return;
    }
    */

    if (simulation.is_running) {
        return;
    }

    qInfo("\n### Starting simulation ###");
    QProgressBar* progress_bar = ui->simulationProgressBar;
    progress_bar->setValue(100);
    ui->runSimulationButton->setEnabled(false);
    ui->runSimulationButton->setStyleSheet("font-size: 14px;");
    ui->runSimulationButton->setText("[Menu->Reset App] or (Ctrl+R) to reset");
    bool res = runSimulation(progress_bar);
    progress_bar->setValue(100);
    // Turn the text green if simulation ran successfully, red otherwise
    ui->runSuccessOrFailText->setStyleSheet(res ? "color: green;" : "color: red;");
    ui->runSuccessOrFailText->setText(res ? QString("Success, took %1ms").arg(simulation.getSimulationTime()): "Failed");

    // -- TEST : ? --
    //simulation.view->scene()->update();
    //ui->simulationGraphicsView->scene()->update();
    ////simulation.view->viewport()->update();
    ////ui->simulationGraphicsView->viewport()->update();
    //simulation.view->show();
    //ui->simulationGraphicsView->show();

    simulation.is_running = false;
}

bool MainWindow::runSimulation(QProgressBar* progress_bar)
{
    //// debug :
    ////simulation.test();

    // Run the simulation and returns true if no errors ocurred
    simulation.ran = true;
    simulation.is_running = true;
    try {
        simulation.run(progress_bar);

        qInfo("### Simulation ended successfully ###\n");
        return true;
    } catch (...) {
        qInfo("#!#!#!# Simulation failed #!#!#!#\n");
        //qError(error);
        return false;
    }
}

void MainWindow::changeBaseStationPower(int value)
{
    // Modify the current editing base station with the new user chosen power
    Transmitter* base_station = simulation.getBaseStation(currentEditingBaseStation_index);
    base_station->setPower_dBm(value);
    showBaseStationPower(value);
}

void MainWindow::on_sliderBaseStationPower_valueChanged(int value)
{
    // User changed the current editing base station power (with slider)
    changeBaseStationPower(value);
}

void MainWindow::on_spinBoxBaseStationPower_valueChanged(int value)
{
    // User changed the current editing base station power (with spin box)
    changeBaseStationPower(value);
}

void MainWindow::changeBaseStationCoordinates(QPointF point)
{
    // Modify the current editing base station with the new user chosen coordinates
    //Transmitter* base_station = simulation.getBaseStation(currentEditingBaseStation_index);
    Transmitter* base_station = simulation.baseStations.at(currentEditingBaseStation_index);
    base_station->changeCoordinates(point);
    showBaseStationCoordinates(point);
}

void MainWindow::on_baseStationXspinBox_valueChanged(double x)
{
    // User changed the current editing base station X coordinate
    //Transmitter* base_station = simulation.getBaseStation(currentEditingBaseStation_index);
    Transmitter* base_station = simulation.baseStations.at(currentEditingBaseStation_index);
    QPointF coordinates = base_station->toPointF();
    //QPointF coordinates = base_station->getCoordinates();
    coordinates.setX(x);
    changeBaseStationCoordinates(coordinates);
}

void MainWindow::on_baseStationYspinBox_valueChanged(double y)
{
    // User changed the current editing base station Y coordinate
    //Transmitter* base_station = simulation.getBaseStation(currentEditingBaseStation_index);
    Transmitter* base_station = simulation.baseStations.at(currentEditingBaseStation_index);
    QPointF coordinates = base_station->toPointF();
    //QPointF coordinates = base_station->getCoordinates();
    coordinates.setY(y);
    changeBaseStationCoordinates(coordinates);
}

void MainWindow::on_actionExit_triggered()
{
    // User clicked on the menu's "Exit" button, or presse "CTRL+W"
    printf("Closing app...\n");
    qApp->quit();
}


void MainWindow::on_actionReset_triggered()
{
    // User clicked on the menu's "Reset" button, or presse "CTRL+R"
    qInfo("Resetting app...");

    // Reset all user input values to default/reset app
    //simulation.resetAll(); // not necessary as we restart the whole program

    qApp->quit();
    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
}


void MainWindow::on_actionSee_Github_triggered()
{
    // User clicked on the menu's "See Github" button
    QDesktopServices::openUrl(QUrl("https://github.com/LucasPlacentino/802_11ay-Raytracing-Simulator", QUrl::TolerantMode));
}


void MainWindow::on_actionAbout_triggered()
{
    // User clicked on the menu's "About" button
    QMessageBox::about(this, tr("About this application"),
                       tr("This <b>802.11ay Raytracing Simulator</b> was made as a "
                          "project for the course <b>ELEC-H304</b> Telecommunications Physics "
                          "at <b>École polytechnique de Bruxelles - ULB</b>."
                          "<br>This simulator uses..."));
}

void MainWindow::saveImage(QGraphicsView* view = simulation.view, bool isTP4 = false)
{
    QSize size = view->scene()->sceneRect().size().toSize()*10; // get the Scene size
    //QImage img(size, QImage::Format_ARGB32); // scene's size, Format_RGBA64 ?
    QImage img(size, QImage::Format_RGBA64);
    QPainter painter(&img);
    painter.setRenderHint(QPainter::Antialiasing);
    // renderscene() //&painter //? scene->render(&painter);
    view->render(&painter);

    QString img_filename= QFileDialog::getSaveFileName(
        this,
        tr("Save Image"),
        QDir::currentPath(),
        "PNG (*.png);;BMP Files (*.bmp);;JPEG (*.JPEG)"
        );
    bool success = img.save(img_filename);
    success? qInfo("Image saved") : qInfo("Cancelled");
}

void MainWindow::on_actionSave_image_triggered()
{
    // User clicked on the menu's "Save Image" button, captures an image of the simulation view.

    if (simulation.scene == nullptr || !simulation.ran) { // checks if a simulation has run
        qInfo("Cannot save simulation that has not already run.");
        //throw std::runtime_error("Cannot save simulation that has not already run.");
        return;
    }
    saveImage(simulation.view);

    //QSize size = simulation.scene->sceneRect().size().toSize()*10; // get the Scene size
    ////QImage img(size, QImage::Format_ARGB32); // scene's size, Format_RGBA64 ?
    //QImage img(size, QImage::Format_RGBA64);
    //QPainter painter(&img);
    //painter.setRenderHint(QPainter::Antialiasing);
    //// renderscene() //&painter //? scene->render(&painter);
    //simulation.view->render(&painter);

    //QString img_filename= QFileDialog::getSaveFileName(
    //    this,
    //    tr("Save Image"),
    //    QDir::currentPath(),
    //    "PNG (*.png);;BMP Files (*.bmp);;JPEG (*.JPEG)"
    //);
    //bool success = img.save(img_filename);
    //success? qInfo("Image saved") : qInfo("Cancelled");

}


void MainWindow::initFirstBaseStation()
{
    // Creates the first (non-deletable) Base Station
    simulation.baseStations.append(new Transmitter(9.4,7, 0, "Base Station 1"));
    //simulation.createBaseStation(new Transmitter(9.4,7));//new Transmitter(0, "Base Station 1", 20, QPointF(1,-1)) // TODO: QPoint
}

void MainWindow::showFirstBaseStation()
{
    // Makes sure the first (non-deletable) Base Station is shown on the base station user edit box
    QString new_item = QString("Base Station 1");
    ui->transmitterSelector->addItem(new_item);
    ui->transmitterSelector->setCurrentIndex(0);
    on_transmitterSelector_activated(0);
}

void MainWindow::showBaseStationPower(int value_dBm)
{
    // Updates the UI to show this base station power
    ui->spinBoxBaseStationPower->setValue(value_dBm);
    ui->sliderBaseStationPower->setValue(value_dBm);
}

void MainWindow::showBaseStationCoordinates(QPointF point)
{
    // Updates the UI to show these base station coordinates
    ui->baseStationXspinBox->setValue(point.x());
    ui->baseStationYspinBox->setValue(point.y());
}

void MainWindow::toggleCellParametersLayout(bool enabled)
{
    //ui->cellParametersLayout->setEnabled(enabled);
    ui->cellCoordinatesLabel->setEnabled(enabled);
    ui->cellCoordinatesXLabel->setEnabled(enabled);
    ui->cellCoordinatesYLabel->setEnabled(enabled);
    ui->cellXspinBox->setEnabled(enabled);
    ui->cellYspinBox->setEnabled(enabled);
}

void MainWindow::toggleCoverageParametersLayout(bool enabled)
{
    ui->showCellOutlineCheckBox->setEnabled(enabled);
}

void MainWindow::on_transmitterSelector_activated(int index)
{
    // User selected a base station in the dropdown
    qDebug() << "Selected base station: " << index+1;

    // set current editing transmitter to this one
    currentEditingBaseStation_index = index;
    showBaseStationPower(simulation.getBaseStation(currentEditingBaseStation_index)->getPower_dBm());
    //changeBaseStationPower(simulation.getBaseStation(currentEditingBaseStation_index).getPower_dBm());

    showBaseStationCoordinates(simulation.baseStations.at(currentEditingBaseStation_index)->toPointF());
    //showBaseStationCoordinates(simulation.getBaseStation(currentEditingBaseStation_index)->getCoordinates());
}


void MainWindow::on_addTransmitterButton_clicked()
{
    // User clicked "Add Base Station" button
    if (ui->transmitterSelector->count() < ui->transmitterSelector->maxCount()) //check if has not reached the max number of transmitters
    {
        qDebug("Added base station");

        //TODO: create a new transmitter object
        QString new_item_name = QString("Base Station %1").arg(ui->transmitterSelector->count()+1);
        ui->transmitterSelector->addItem(new_item_name);

        int new_item_index = ui->transmitterSelector->findText(new_item_name);

        simulation.baseStations.append(new Transmitter(5,5, new_item_index, new_item_name));
        //simulation.createBaseStation(Transmitter(new_item_index, new_item, 20, QPointF(1,-1))); // TODO: QPoint

        on_transmitterSelector_activated(new_item_index);
        ui->transmitterSelector->setCurrentIndex(new_item_index);


    } else {
        qInfo("There is already the maximum number of base stations");
    }
}


void MainWindow::on_deleteBaseStationPushButton_clicked()
{
    // User clicked on the "Delete Base Station" button
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

void MainWindow::on_coverageHeatmapRadioButton_clicked(bool checked)
{
    simulation.showRaySingleCell = !checked;
    toggleCoverageParametersLayout(checked);
    toggleCellParametersLayout(!checked);
    qDebug() << "Coverage parameters" << (checked? "enabled," : "disabled,") << "Cell parameters" << (!checked? "enabled" : "disabled");
}


void MainWindow::on_singleCellRadioButton_clicked(bool checked)
{
    simulation.showRaySingleCell = checked;
    toggleCellParametersLayout(checked);
    toggleCoverageParametersLayout(!checked);
    qDebug() << "Coverage parameters" << (!checked? "enabled," : "disabled,") << "Cell parameters" << (checked? "enabled" : "disabled");
}


void MainWindow::on_liftOnFloorCheckBox_clicked(bool checked)
{
    qDebug() << "Set lift:" << checked;
    simulation.lift_is_on_floor = checked;
}


void MainWindow::on_showCellOutlineCheckBox_toggled(bool checked)
{
    simulation.show_cell_outline = checked;
    qDebug() << "Show cell outline:" << checked;
}


void MainWindow::on_resolutionComboBox_currentIndexChanged(int index)
{
    simulation.resolution = resolutions[index];
    qInfo() << "Changed resolution to" << resolutions[index] << "m";
}


void MainWindow::on_actionRun_TP4_Simulation_triggered()
{
    TP4view = runTP4();
}


void MainWindow::on_actionSave_TP4_image_triggered()
{
    saveImage(TP4view);
}
