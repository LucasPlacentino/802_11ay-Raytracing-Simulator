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

Simulation* runAlgo(QList<Simulation*> sim_list, qreal resolution);

#endif // ALGORITHME_H