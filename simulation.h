#ifndef SIMULATION_H
#define SIMULATION_H

#include "obstacle.h"
#include "ray.h"
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
    unsigned int getNumberOfObstacles();
    int getNumberOfBaseStations();

    SimulationGraphicsScene* scene=nullptr;
    QGraphicsView* view=nullptr;

    bool lift_is_on_floor = false;
    int max_ray_reflections = 2;
    bool showRaySingleCell = false;

private:

    QList<QList<Receiver>> cells_matrix;
    void computeCell(Receiver cell);
    void traceRays(int num_reflections);
    void traceRay(Ray *ray, int reflections);

    std::vector<Obstacle> obstacles; // DONT USE A LIST ? => vector<Type> Use a QList (same as QVector)?
    std::vector<Transmitter> baseStations;

};

#endif // SIMULATION_H
