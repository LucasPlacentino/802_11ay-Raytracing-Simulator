#include "algorithme.h"
#include <QDebug>
#include <limits>
#include <cmath>

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
