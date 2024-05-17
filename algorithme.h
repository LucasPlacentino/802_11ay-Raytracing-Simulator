#ifndef ALGORITHME_H
#define ALGORITHME_H

#include "simulation.h"

class Algorithme {
public:
    /*
    Algorithme(Simulation* simulation, qreal resolution);
    void runBruteForceOptimization();
    void setTransmitterPosition(Transmitter* transmitter, qreal x, qreal y);
    qreal computeTotalPowerAtPosition(Transmitter* transmitter, qreal x, qreal y);
    */

private:
    Simulation* simulation;
    qreal resolution;
};

void runAlgo();

#endif // ALGORITHME_H
