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
    std::vector<Obstacle*>* getObstacles();
    unsigned int getNumberOfObstacles();
    int getNumberOfBaseStations();

    SimulationGraphicsScene* scene=nullptr;
    QGraphicsView* view=nullptr;

    bool lift_is_on_floor = false;
    int max_ray_reflections = 2;
    bool showRaySingleCell = false;


    void test();
private:

    QList<QList<QSharedPointer<Receiver>>> cells_matrix;
    void computeCell(Receiver cell);
    void traceRaysToCell(QPair<int,int> cell_index, int num_reflections);
    void traceRay(QSharedPointer<Ray> ray, int reflections);

    std::vector<Obstacle*> obstacles; // DONT USE A LIST ? => vector<Type> Use a QList (same as QVector)?
    std::vector<Transmitter> baseStations;

    QList<QSharedPointer<Ray>> rays_list;

};

#endif // SIMULATION_H
