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
#include "simulationgraphicsscene.h"

//SimulationGraphicsScene* simulation_scene; // global QGraphicsScene scene object
Simulation simulation = Simulation(); // The global simulation object, use `extern Simulation simulation;` in other files?
//QGraphicsView simulation_view(simulation_scene);

int currentEditingBaseStation_index = 0; // The base station that is currently selected for user edit

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // MainWindow constructor, is ran on program's UI launch
    initFirstBaseStation();
    ui->setupUi(this);
    showFirstBaseStation();

    toggleCoverageParametersLayout(true);
    toggleCellParametersLayout(false);

    SimulationGraphicsScene* simulation_scene = new SimulationGraphicsScene(this);
    simulation.scene = simulation_scene;
    //simulation.scene->setSceneRect(QRectF(0,0, 690, 450)); // if not set, QGraphicsScene will use the bounding area of all items, as returned by itemsBoundingRect(), as the scene rect.
    qDebug() << "scene pointer (&simulation_scene): " << simulation_scene;
    qDebug() << "scene pointer (simulation.scene): " << simulation.scene;


    // TESTING :
    simulation_scene->addRect(20, 20, 60, 60, QPen(Qt::red, 3), QBrush(Qt::green)); simulation_scene->addEllipse(120, 20, 60, 60, QPen(Qt::red, 3), QBrush(Qt::yellow)); simulation_scene->addPolygon(QPolygonF() << QPointF(220, 80) << QPointF(280, 80) << QPointF(250, 20), QPen(Qt::blue, 3), QBrush(Qt::magenta));



    //?????????????? :
    simulation.scene->setBackgroundBrush(Qt::black);
    simulation_scene->setBackgroundBrush(QBrush(Qt::black, Qt::SolidPattern));
    simulation.scene->setForegroundBrush(Qt::white);
    simulation.scene->addText("Hello World!");

    //simulation.scene->drawScene();

    //ui->simulationGraphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate); // needed ?
    //ui->simulationGraphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate); // needed ?

    //simulation.view = ui->simulationGraphicsView;
    //qDebug() << "view pointer (ui->simulationGraphicsView): " << &(ui->simulationGraphicsView);
    //qDebug() << "view pointer (simulation.view): " << &simulation.view;
    //ui->simulationGraphicsView->setScene(simulation.scene);
    ui->simulationGraphicsView->setScene(simulation_scene);
    //qDebug() << "scene (simulation.view->scene()): " << simulation.view->scene();
    //qDebug() << "scene (ui->simulationGraphicsView->scene()): " << ui->simulationGraphicsView->scene();

    //ui->simulationGraphicsView->scene()->setSceneRect(ui->simulationGraphicsView->frameRect());
    ui->simulationGraphicsView->fitInView(simulation_scene->itemsBoundingRect());

    //simulation.view->setBackgroundBrush(Qt::black);
    //simulation.view->setBackgroundBrush(QBrush(Qt::black, Qt::SolidPattern));
    //simulation.view->setForegroundBrush(Qt::white);
    ui->simulationGraphicsView->setBackgroundBrush(Qt::black);
    ui->simulationGraphicsView->setBackgroundBrush(QBrush(Qt::black, Qt::SolidPattern));
    ui->simulationGraphicsView->setForegroundBrush(Qt::white);
    //simulation.view->scene()->setBackgroundBrush(Qt::black);
    //simulation.view->scene()->setBackgroundBrush(QBrush(Qt::black, Qt::SolidPattern));
    //simulation.view->scene()->setForegroundBrush(Qt::white);

    ui->simulationGraphicsView->setRenderHints(QPainter::Antialiasing); // ?

    //simulation.view->scene()->update();
    ui->simulationGraphicsView->scene()->update();
    ////simulation.view->viewport()->update();
    ////ui->simulationGraphicsView->viewport()->update();
    //simulation.view->show();
    ui->simulationGraphicsView->show();
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

    qInfo("Starting simulation");
    bool res = runSimulation();
    // Turn the text green if simulation ran successfully, red otherwise
    ui->runSuccessOrFailText->setStyleSheet(res ? "color: green;" : "color: red;");
    ui->runSuccessOrFailText->setText(res ? QString("Success, took %1ms").arg(simulation.getSimulationTime()): "Failed");

    // -- TEST : ? --
    //simulation.view->scene()->update();
    ui->simulationGraphicsView->scene()->update();
    ////simulation.view->viewport()->update();
    ////ui->simulationGraphicsView->viewport()->update();
    //simulation.view->show();
    ui->simulationGraphicsView->show();
    simulation.is_running = false;
}

bool MainWindow::runSimulation()
{
    simulation.test();
    // Run the simulation and returns true if no errors ocurred
    simulation.ran = true;
    simulation.is_running = true;
    try {
        ////simulation = Simulation(); //do not override the object, change/reset global one

        // TODO: show a progress bar ? or at least a

        simulation.run(); // TODO: the run func

        qInfo("Simulation ended successfully");
        return true;
    } catch (...) {
        qInfo("Simulation failed");
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
    Transmitter* base_station = simulation.getBaseStation(currentEditingBaseStation_index);
    base_station->changeCoordinates(point);
    showBaseStationCoordinates(point);

    // TODO: redraw the base station at new position on view
}

void MainWindow::on_baseStationXspinBox_valueChanged(double x)
{
    // User changed the current editing base station X coordinate
    Transmitter* base_station = simulation.getBaseStation(currentEditingBaseStation_index);
    QPointF coordinates = base_station->getCoordinates();
    coordinates.setX(x);
    changeBaseStationCoordinates(coordinates);
}

void MainWindow::on_baseStationYspinBox_valueChanged(double y)
{
    // User changed the current editing base station Y coordinate
    Transmitter* base_station = simulation.getBaseStation(currentEditingBaseStation_index);
    QPointF coordinates = base_station->getCoordinates();
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
    qInfo("Resetting all values and restarting app...");

    // Reset all user input values to default/reset app
    //simulation.resetAll(); // not necessary as we restart the whole program?

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


void MainWindow::on_actionSave_image_triggered()
{
    // User clicked on the menu's "Save Image" button, captures an image of the simulation view.

    /*
    if (simulation.ran) // checks if a simulation has run
    {
        // TODO: save image from the simulation frame window

    } else { // no simulation has already run
        // do nothing?
    }
    */

    // get the simulation Scene ?
    //QGraphicsScene* scene = &simulation.simulation_scene;
    // scene is: simulation_scene or simulation->scene
    QSize size = simulation.scene->sceneRect().size().toSize(); // get the Scene size // FIXME: SEG FAULTS ?
    QImage img(size, QImage::Format_ARGB32); // scene's size, Format_RGBA64 ?
    QPainter painter(&img);
    painter.setRenderHint(QPainter::Antialiasing);
    // renderscene() //&painter //? scene->render(&painter);
    simulation.scene->render(&painter);

    QString img_filename= QFileDialog::getSaveFileName(
        this,
        tr("Save Image"),
        QDir::currentPath(),
        "PNG (*.png);;BMP Files (*.bmp);;JPEG (*.JPEG)"
    );
    bool success = img.save(img_filename);
    success? qInfo("Image saved") : qInfo("Cancelled");

}


void MainWindow::initFirstBaseStation()
{
    // Creates the first (non-deletable) Base Station
    //TODO: create a new transmitter object
    simulation.createBaseStation(
        Transmitter(0, "Base Station 1", 20, QPointF(1,-1)) // TODO: QPoint
        );
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
    //ui->
    ui->coverageHeatmapTestLabel->setEnabled(enabled);
    //...
}

void MainWindow::on_transmitterSelector_activated(int index)
{
    // User selected a base station in the dropdown
    qDebug() << "Selected base station: " << index+1;

    // set current editing transmitter to this one
    currentEditingBaseStation_index = index;
    showBaseStationPower(simulation.getBaseStation(currentEditingBaseStation_index)->getPower_dBm());
    //changeBaseStationPower(simulation.getBaseStation(currentEditingBaseStation_index).getPower_dBm());

    showBaseStationCoordinates(simulation.getBaseStation(currentEditingBaseStation_index)->getCoordinates());

    // TODO: highlight the current editing base station in another color on the view, redraw the transmitter
}


void MainWindow::on_addTransmitterButton_clicked()
{
    // User clicked "Add Base Station" button
    if (ui->transmitterSelector->count() < ui->transmitterSelector->maxCount()) //check if has not reached the max number of transmitters
    {
        qDebug("Added base station");

        //TODO: create a new transmitter object
        QString new_item = QString("Base Station %1").arg(ui->transmitterSelector->count()+1);
        ui->transmitterSelector->addItem(new_item);

        int new_item_index = ui->transmitterSelector->findText(new_item);

        simulation.createBaseStation(Transmitter(new_item_index, new_item, 20, QPointF(1,-1))); // TODO: QPoint

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

        // TODO: undraw this base station from the view
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

    // TODO: unhightlight the selected cell on the view
}


void MainWindow::on_singleCellRadioButton_clicked(bool checked)
{
    simulation.showRaySingleCell = checked;
    toggleCellParametersLayout(checked);
    toggleCoverageParametersLayout(!checked);
    qDebug() << "Coverage parameters" << (!checked? "enabled," : "disabled,") << "Cell parameters" << (checked? "enabled" : "disabled");

    // TODO: hightlight the selected cell on the view, redraw the cell
}


void MainWindow::on_liftOnFloorCheckBox_clicked(bool checked)
{
    qDebug() << "Set lift:" << checked;
    simulation.lift_is_on_floor = checked;
}

