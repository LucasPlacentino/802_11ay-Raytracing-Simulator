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

private:
    Ui::MainWindow *ui;
    bool runSimulation();
    void changeBaseStationPower(int value);
    void initFirstBaseStation();
    void showFirstBaseStation();
    void showBaseStationPower(int value_dBm);
};
#endif // MAINWINDOW_H
