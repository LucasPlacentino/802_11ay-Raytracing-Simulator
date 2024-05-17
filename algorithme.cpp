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

Simulation* runAlgo(QList<Simulation*> sim_list, qreal resolution) {
    Simulation* bestSimAveragePower = nullptr;
    Simulation* bestSimLessDisconnectedCells = nullptr;
    qreal best_average_pow_dbm = 0;
    int min_num_of_disconnected_cells = sim_list[0]->cells.length()*sim_list[0]->cells[0].length();
    int g=0;
    for (Simulation* sim : sim_list) {
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
        }
        if (num_of_disconnected_cells < min_num_of_disconnected_cells) {
            min_num_of_disconnected_cells = num_of_disconnected_cells;
            bestSimLessDisconnectedCells = sim;
        }
        qDebug() << "Sim" << g << ", TX" << sim->baseStations[0]->getCoordinates() << ", average power:" << average_pow_dbm << "dBm";
    }
    qDebug() << "Best sim average power: transmitter at" << bestSimAveragePower->baseStations[0]->getCoordinates();
    qDebug() << "Best average power:" << best_average_pow_dbm << "dBm";
    qDebug() << "-----------";
    qDebug() << "Best sim min num of disconnected cells: transmitter at" << bestSimLessDisconnectedCells->baseStations[0]->getCoordinates();
    return bestSimAveragePower;
}
