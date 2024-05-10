#ifndef SIMULATION_H
#define SIMULATION_H

#include <QElapsedTimer>
#include <QGraphicsView>
#include <QProgressBar>

#include "obstacle.h"
#include "ray.h"
#include "receiver.h"
#include "simulationgraphicsscene.h"
#include "transmitter.h"

class Simulation
{
public:
    Simulation(); // constructor
    bool ran = false;
    bool is_running = false;
    qreal resolution = 0.5; // meters, 0.5m x 0.5m cells

    void run(QProgressBar* progress_bar);
    void clear();
    void resetAll(); // needed ?
    void createBaseStation(Transmitter* transmitter);
    Transmitter* getBaseStation(int index); // needed ?
    void deleteBaseStation(int index);
    //std::vector<Obstacle*>* getObstacles();
    QList<Obstacle*>* getObstacles(); // needed ?
    unsigned int getNumberOfObstacles(); // needed ?
    unsigned int getNumberOfBaseStations(); // needed ?

    qint64 getSimulationTime() const;

    QGraphicsScene* scene = nullptr;//new QGraphicsScene();//this->createGraphicsScene(&this->baseStations);
    QGraphicsView* view = nullptr;//new QGraphicsView(this->scene);

    bool lift_is_on_floor = false;
    bool show_cell_outline = false;
    int max_ray_reflections = 2;
    bool showRaySingleCell = false;

    // --- NEW : ---
    QList<Transmitter*> baseStations;// = {new Transmitter(9.4,7)}; // TX list // TODO: first: y=7 or y=-7 ?
    QList<QList<Receiver*>> cells; // RX matrix // TODO:
    QList<Obstacle*> obstacles;

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

    //QVector2D origin = QVector2D(0,0);
    //QGraphicsScene* createGraphicsScene(std::vector<Transmitter>* TX_list);
    //QList<QList<QSharedPointer<Receiver>>> cells_matrix;
    //void computeCell(QSharedPointer<Receiver> cell);
    //void traceRaysToCell(QPair<int,int> cell_index, int num_reflections);
    //void traceRay(QSharedPointer<Ray> ray, int reflections);
    //std::vector<Obstacle*> obstacles; // DONT USE A LIST ? => vector<Type> Use a QList (same as QVector)?
    //std::vector<Transmitter> baseStations;
    //QList<QSharedPointer<Ray>> rays_list;


    void createCellsMatrix(); // TODO: check if all correct

    //* --- new : ---
    void createWalls();
    QGraphicsScene* createGraphicsScene();
    void computeReflections(Receiver* _RX, const QVector2D& _TX);
    void computeDirect(Receiver* _RX, const QVector2D& _TX);
    void addReflection(Ray* _ray, const QVector2D& _p1, const QVector2D& _p2, Obstacle* wall);
    qreal makeTransmission(RaySegment* ray_segment, Obstacle* wall);
    void checkTransmissions(Ray* _ray, QList<Obstacle*> _reflection_walls);
    complex<qreal> computePerpendicularGamma(qreal _cos_theta_i, qreal _cos_theta_t, Obstacle* wall);
    complex<qreal> computeReflectionCoeff(qreal _cos_theta_i, qreal _sin_theta_i, qreal _cos_theta_t, qreal _sin_theta_t, Obstacle* wall);
    complex<qreal> computeTransmissionCoeff(qreal _cos_theta_i, qreal _sin_theta_i, qreal _cos_theta_t, qreal _sin_theta_t, Obstacle* wall);
    bool checkSameSideOfWall(const QVector2D& _normal, const QVector2D& _TX, const QVector2D& _RX);
    bool checkRaySegmentIntersectsWall(const Obstacle* wall, RaySegment* ray_segment, QPointF* intersection_point=nullptr);
    QVector2D computeImage(const QVector2D& _point, Obstacle* wall);
    QVector2D calculateReflectionPoint(const QVector2D& _start, const QVector2D& _end, Obstacle* wall);

#ifdef DRAW_RAYS
    void drawAllRays(QGraphicsScene* scene, Receiver* _RX)
#endif

    void showView();
    void addLegend(QGraphicsScene* scene);
};

#endif // SIMULATION_H
