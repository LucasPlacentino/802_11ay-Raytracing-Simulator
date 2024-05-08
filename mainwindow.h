#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "transmitter.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_runSimulationButton_clicked();


    void on_sliderBaseStationPower_valueChanged(int value);

    void on_actionExit_triggered();

    void on_actionReset_triggered();

    void on_actionSee_Github_triggered();

    void on_actionAbout_triggered();

    void on_actionSave_image_triggered();

    void on_spinBoxBaseStationPower_valueChanged(int value);

    void on_transmitterSelector_activated(int index);

    void on_addTransmitterButton_clicked();

    void on_deleteBaseStationPushButton_clicked();

    void on_baseStationXspinBox_valueChanged(double arg1);

    void on_baseStationYspinBox_valueChanged(double arg1);

    void on_singleCellRadioButton_clicked(bool checked);

    void on_coverageHeatmapRadioButton_clicked(bool checked);

    void on_liftOnFloorCheckBox_clicked(bool checked);

    void on_showCellOutlineCheckBox_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    bool runSimulation();
    void changeBaseStationPower(int value);
    void changeBaseStationCoordinates(QPointF point);
    void initFirstBaseStation();
    void showFirstBaseStation();
    void showBaseStationPower(int value_dBm);
    void showBaseStationCoordinates(QPointF point);

    void toggleCellParametersLayout(bool enabled);
    void toggleCoverageParametersLayout(bool enabled);
};
#endif // MAINWINDOW_H
