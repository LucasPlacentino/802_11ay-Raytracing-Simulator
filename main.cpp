#include <QCoreApplication>
#include <QMainWindow>
#include "simulation.h"
#include "algorithme.h"
#include "transmitter.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QMainWindow window;

    qreal resolution = 0.5; // default is 0.5, or any resolution you need

    int number_of_tx_x = 15-1;
    int number_of_tx_y = 8-1;

    QList<Simulation*> sim_list;
    for (int x=0; x<number_of_tx_x; x++) {
        for (int y=0; y<number_of_tx_y; y++) {
            Simulation* simulation = new Simulation(false);
            simulation->resolution = resolution;
            // Créez et ajoutez une base station à la simulation
            //Transmitter* baseStation = new Transmitter(7.5, 4.0, 0, "Base Station 1");
            Transmitter* baseStation = new Transmitter(0.5+qreal(x), 0.5+qreal(y),0,"opti_BS");

            //simulation.addBaseStation(baseStation);
            simulation->baseStations = {};
            simulation->baseStations.append(baseStation);

            //simulation->run(nullptr); // run sim
            sim_list.append(simulation);
        }
    }

    //Algorithme algo(simulation, resolution);
    //algo.runOptimization();
    Simulation* bestSim = runAlgo(sim_list,resolution);
    qDebug() << "best sim p*:" << bestSim;
    qDeleteAll(sim_list);

    qDebug() << "End program.";

    return app.exec();
    //return 0;
}
