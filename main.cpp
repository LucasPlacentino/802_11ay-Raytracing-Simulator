#include <QCoreApplication>
#include "simulation.h"
#include "algorithme.h"
#include "transmitter.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Simulation simulation;

    // Créez et ajoutez une base station à la simulation
    Transmitter* baseStation = new Transmitter(7.5, 4.0, 0, "Base Station 1");
    simulation.addBaseStation(baseStation);

    qreal resolution = 0.5; // or any resolution you need
    Algorithme algo(&simulation, resolution);

    algo.runOptimization();

    return 0;
}
