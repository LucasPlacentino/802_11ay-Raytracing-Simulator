#ifndef SIMULATION_H
#define SIMULATION_H

#include "obstacle.h"
#include "simulationgraphicsscene.h"
#include "transmitter.h"

class Simulation
{
public:
    Simulation(); // constructor

    void resetAll();
    void createBaseStation(Transmitter transmitter);
    Transmitter* getBaseStation(int index);
    void deleteBaseStation(int index);

    SimulationGraphicsScene* scene=nullptr;

private:

    std::vector<Obstacle> obstacles; // DONT USE A LIST ? => vector<Type>
    unsigned int number_of_obstacles;
    std::vector<Transmitter> baseStations;

};

#endif // SIMULATION_H
