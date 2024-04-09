#ifndef SIMULATION_H
#define SIMULATION_H

#include "obstacle.h"
#include "receiver.h"
#include "simulationgraphicsscene.h"
#include "transmitter.h"

class Simulation
{
public:
    Simulation(); // constructor

    void run();
    void resetAll();
    void createBaseStation(Transmitter transmitter);
    Transmitter* getBaseStation(int index);
    void deleteBaseStation(int index);
    std::vector<Obstacle>* getObstacles();
    int getNumberOfBaseStations();

    SimulationGraphicsScene* scene=nullptr;
    QGraphicsView* view=nullptr;

private:

    QList<QList<Receiver>> cells_matrix;
    void computeCell(Receiver cell);

    std::vector<Obstacle> obstacles; // DONT USE A LIST ? => vector<Type>
    unsigned int number_of_obstacles;
    std::vector<Transmitter> baseStations;

};

#endif // SIMULATION_H
