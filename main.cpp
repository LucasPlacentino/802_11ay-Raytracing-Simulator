#include <QApplication>
#include <QCoreApplication>
#include <QMainWindow>
#include <QWindow>
#include "simulation.h"
#include "algorithme.h"
#include "transmitter.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv); // QCoreApplication ?

    //Algorithme algo(simulation, resolution);
    //algo.runOptimization();
    runAlgo();

    qDebug() << "End program.";

    return app.exec();
    //return 0;
}
