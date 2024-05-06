#ifndef SIMULATION_H
#define SIMULATION_H

#include "obstacle.h"
#include "ray.h"
#include "receiver.h"
#include "simulationgraphicsscene.h"
#include "transmitter.h"
#include "parameters.h"

#include <QElapsedTimer>
#include <QGraphicsView>

class Simulation
{
public:
    Simulation(); // constructor
    bool ran = false;
    bool is_running = false;

    void run();
    void clear();
    void resetAll();
    void createBaseStation(Transmitter transmitter);
    Transmitter* getBaseStation(int index);
    void deleteBaseStation(int index);
    std::vector<Obstacle*>* getObstacles();
    unsigned int getNumberOfObstacles();
    int getNumberOfBaseStations();
    qint64 getSimulationTime() const;

    QGraphicsScene* scene = nullptr;//new QGraphicsScene();//this->createGraphicsScene(&this->baseStations);
    QGraphicsView* view = nullptr;//new QGraphicsView(this->scene);

    bool lift_is_on_floor = false;
    int max_ray_reflections = 2;
    bool showRaySingleCell = false;

    // --- NEW : ---
    QList<Transmitter*> baseStations; // TX list // TODO:
    QList<QList<Receiver*>> cells; // RX matrix // TODO:

    QGraphicsScene* createGraphicsScene();
    // -------------


    // test:
    void test();
private:
    QElapsedTimer timer;
    qint64 simulation_time;

    //qreal max_x = 15;
    //qreal min_x = 0;
    //qreal max_y = 0;
    //qreal min_y = -8;
    qreal resolution = 0.5;

    //QVector2D origin = QVector2D(0,0);

    QGraphicsScene* createGraphicsScene(std::vector<Transmitter>* TX_list);

    QList<QList<QSharedPointer<Receiver>>> cells_matrix;
    void computeCell(QSharedPointer<Receiver> cell);
    void traceRaysToCell(QPair<int,int> cell_index, int num_reflections);
    void traceRay(QSharedPointer<Ray> ray, int reflections);

    std::vector<Obstacle*> obstacles; // DONT USE A LIST ? => vector<Type> Use a QList (same as QVector)?
    //std::vector<Transmitter> baseStations;

    QList<QSharedPointer<Ray>> rays_list;

    void createCellsMatrix();

};

#endif // SIMULATION_H
