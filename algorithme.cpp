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
    Simulation* bestSim = nullptr;
    qreal best_average_pow = 0;
    for (Simulation* sim : sim_list) {
        qreal total_pow = 0;
        for (QList<Receiver*> cell_line : sim->cells) {
            for (Receiver* cell : cell_line) {
                total_pow += cell->power;
            }
        }
        qreal average_pow = total_pow/sim->cells.length();
        qDebug() << "Sim average power:" << average_pow << "W";
        if (average_pow > best_average_pow) {
            // this sim is a better sim
            best_average_pow = average_pow;
            bestSim = sim;
        }
    }
    qDebug() << "Best sim: transmitter at" << bestSim->baseStations[0]->getCoordinates();
    qDebug() << "Best average power:" << best_average_pow << "W";
    return bestSim;
}
