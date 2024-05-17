#include "algorithme.h"
#include <QDebug>
#include <limits>
#include <cmath>

/*
Algorithme::Algorithme(Simulation* simulation, qreal resolution)
    : simulation(simulation), resolution(resolution) {}

void Algorithme::runBruteForceOptimization() {
    if (simulation->baseStations.isEmpty()) {
        qDebug() << "No base stations available.";
        return;
    } else {
        qDebug() << "Base station found at (" << simulation->baseStations[0]->x() << ", " << simulation->baseStations[0]->y() << ")";
    }

    qreal total_power = computeTotalPowerAtPosition(simulation->baseStations[0], simulation->baseStations[0]->x(), simulation->baseStations[0]->y());
    qDebug() << "power total :" << total_power;
}

void Algorithme::setTransmitterPosition(Transmitter* transmitter, qreal x, qreal y) {
    transmitter->changeCoordinates(QPointF(x, y));
}

qreal Algorithme::computeTotalPowerAtPosition(Transmitter* transmitter, qreal x, qreal y) {
    setTransmitterPosition(transmitter, x, y);

    qreal totalPower = 0;
    if (simulation->cells.isEmpty()) {
        qDebug() << "No cells available.";
        return totalPower;
    }

    for (const auto& row : simulation->cells) {
        for (Receiver* cell : row) {
            totalPower += cell->power;
        }
    }
    return totalPower;
}
*/

void runAlgo() {

    qreal resolution = 0.5; // default is 0.5, or any resolution you need

    int number_of_tx_x = 15-1;
    int number_of_tx_y = 8-1;

    //Simulation bestSimAveragePower;
    QPointF bestSimAveragePowerTX;
    //Simulation bestSimLessDisconnectedCells;
    QPointF bestSimLessDisconnectedCellsTX;

    qreal best_average_pow_dbm = -99999;
    int min_num_of_disconnected_cells = 999999;

    qDebug() << "num of TXs:" << number_of_tx_x*number_of_tx_y;
    int g=0;
    for (int x=0; x<number_of_tx_x; x++) {
        for (int y=0; y<number_of_tx_y; y++) {
            Simulation sim = Simulation(false);
            sim.resolution = resolution;
            // Créez et ajoutez une base station à la simulation
            //Transmitter* baseStation = new Transmitter(7.5, 4.0, 0, "Base Station 1");
            Transmitter* baseStation = new Transmitter(0.5+qreal(x), 0.5+qreal(y),0,"opti_BS");

            //simulation.addBaseStation(baseStation);
            sim.baseStations = {};
            sim.baseStations.append(baseStation);

            sim.run(nullptr); // run sim

            //int min_num_of_disconnected_cells = (sim->cells[0].length()) * (sim->cells.length());
            //qDebug() <<

            bool better = false;
            g++;
            qreal total_pow_dbm = 0;
            int num_of_disconnected_cells = 0;
            int i = 0;
            for (QList<Receiver*> cell_line : sim.cells) {
                for (Receiver* cell : cell_line) {
                    i++;

                    qreal _rx_power = cell->computeTotalPower(sim.baseStations[0]);
                    cell->power = _rx_power;
                    cell->updateBitrateAndColor();

                    total_pow_dbm += 10*log10(cell->power*1000);
                    if (10*std::log10(cell->power*1000) < -90) {
                        num_of_disconnected_cells++;
                    }
                    //qDebug() << "Cell" << i;
                }
            }
            qreal average_pow_dbm;
            if ((sim.cells.length()) * (sim.cells[0].length()) != 0) {
                average_pow_dbm = total_pow_dbm/((sim.cells.length()) * (sim.cells[0].length()));
            } else {
                average_pow_dbm = 0;
            }
            if (average_pow_dbm > best_average_pow_dbm) {
                // this sim is a better sim

                //if (bestSimAveragePower.cells.isEmpty()) {
                //    for (QList<Receiver*> list : bestSimAveragePower.cells) {
                //        qDeleteAll(list);
                //    }
                //    qDeleteAll(bestSimAveragePower.obstacles);
                //    qDeleteAll(bestSimAveragePower.baseStations);
                //}

                best_average_pow_dbm = average_pow_dbm;
                //bestSimAveragePower = *sim;
                bestSimAveragePowerTX = sim.baseStations[0]->getCoordinates();
                better = true;
            }
            if (num_of_disconnected_cells < min_num_of_disconnected_cells) {

                //if (bestSimLessDisconnectedCells.cells.isEmpty()) {
                //    for (QList<Receiver*> list : bestSimLessDisconnectedCells.cells) {
                //        qDeleteAll(list);
                //    }
                //    qDeleteAll(bestSimLessDisconnectedCells.obstacles);
                //    qDeleteAll(bestSimLessDisconnectedCells.baseStations);
                //}

                min_num_of_disconnected_cells = num_of_disconnected_cells;
                //bestSimLessDisconnectedCells = *sim;
                bestSimLessDisconnectedCellsTX = sim.baseStations[0]->getCoordinates();
                better = true;
            }
            qDebug() << "Sim" << g << ", TX" << sim.baseStations[0]->getCoordinates() << ", average power:" << average_pow_dbm << "dBm";
            //if (!better) {
            //if (true) {
            //    for (QList<Receiver*> list : sim->cells) {
            //        qDeleteAll(list);
            //    }
            //    qDeleteAll(sim->obstacles);
            //    qDeleteAll(sim->baseStations);
            //    delete sim;
            //}
        }
    }
    qDebug() << "for loop finished";


    /*
    Simulation* bestSimAveragePower = nullptr;
    Simulation* bestSimLessDisconnectedCells = nullptr;
    qreal best_average_pow_dbm = 0;
    int min_num_of_disconnected_cells = sim_list[0]->cells.length()*sim_list[0]->cells[0].length();
    int g=0;
    while (sim_list.length() != 0) {
    //}
    //for (Simulation* sim : sim_list) {
        Simulation* sim = sim_list[0];
        //sim->run(nullptr); // run sim

        bool better = false;
        g++;
        qreal total_pow_dbm = 0;
        int num_of_disconnected_cells = 0;
        int i = 0;
        for (QList<Receiver*> cell_line : sim->cells) {
            for (Receiver* cell : cell_line) {
                i++;

                qreal _rx_power = cell->computeTotalPower(sim->baseStations[0]);
                cell->power = _rx_power;
                cell->updateBitrateAndColor();

                total_pow_dbm += 10*log10(cell->power*1000);
                if (10*std::log10(cell->power*1000) < -90) {
                    num_of_disconnected_cells++;
                }
                //qDebug() << "Cell" << i;
            }
        }
        qreal average_pow_dbm = total_pow_dbm/((sim->cells.length()) * (sim->cells[0].length()));
        if (average_pow_dbm > best_average_pow_dbm) {
            // this sim is a better sim
            best_average_pow_dbm = average_pow_dbm;
            bestSimAveragePower = sim;
            better = true;
        }
        if (num_of_disconnected_cells < min_num_of_disconnected_cells) {
            min_num_of_disconnected_cells = num_of_disconnected_cells;
            bestSimLessDisconnectedCells = sim;
            better = true;
        }
        qDebug() << "Sim" << g << ", TX" << sim->baseStations[0]->getCoordinates() << ", average power:" << average_pow_dbm << "dBm";
        if (!better) {
            sim_list.removeAt(0);
        }
    }
*/
    //qDebug() << "Best sim average power: transmitter at" << bestSimAveragePower.baseStations[0]->getCoordinates();
    qDebug() << "Best sim average power: transmitter at" << bestSimAveragePowerTX;
    qDebug() << "Best average power:" << best_average_pow_dbm << "dBm";
    qDebug() << "-----------";
    //qDebug() << "Best sim min num of disconnected cells: transmitter at" << bestSimLessDisconnectedCells.baseStations[0]->getCoordinates();
    qDebug() << "Best sim min num of disconnected cells: transmitter at" << bestSimLessDisconnectedCellsTX;
    qDebug() << "Min num of disconnected cells:" << min_num_of_disconnected_cells;

    //qDebug() << "best sim p*:" << &bestSimAveragePower;
    //return &bestSimAveragePower;
}
