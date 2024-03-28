#ifndef SIMULATION_H
#define SIMULATION_H

#include "obstacle.h"
#include <list>

class Simulation
{
public:
    Simulation(); // constructor

    void resetAll();
private:
    std::list<Obstacle> obstacles; // TODO: NOT USE A LIST ?!
    unsigned int number_of_obstacles;
};

#endif // SIMULATION_H
