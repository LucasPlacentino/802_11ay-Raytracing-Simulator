#ifndef SIMULATION_H
#define SIMULATION_H

#include <QElapsedTimer>
#include <QGraphicsView>
#include <QProgressBar>

#include "obstacle.h"
#include "ray.h"
#include "receiver.h"
#include "transmitter.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QProgressBar>
#include <QVector2D>
#include <complex>
#include "parameters.h"
#include "transmitter.h"
#include "receiver.h"
#include "obstacle.h"
#include "ray.h"


class Simulation
{
public:
    Simulation(); // constructor
    bool ran = false;
    bool is_running = false;
    qreal resolution = 0.5; // meters, default is 0.5m x 0.5m cells

    void run(QProgressBar* progress_bar);
    void createBaseStation(Transmitter* transmitter);
    Transmitter* getBaseStation(int index);
    void deleteBaseStation(int index);
    QList<Obstacle*>* getObstacles();
    unsigned int getNumberOfObstacles();
    unsigned int getNumberOfBaseStations();

    qint64 getSimulationTime() const;

    QGraphicsScene* scene = nullptr;
    QGraphicsView* view = nullptr;

    bool lift_is_on_floor = false;
    bool show_cell_outline = false;
    int max_ray_reflections = 2;
    bool showRaySingleCell = false;

    // --- NEW : ---
    QList<Transmitter*> baseStations; // TX list
    QList<QList<Receiver*>> cells; // RX matrix
    QList<Obstacle*> obstacles; // walls list

    qreal singleCellX=2;
    qreal singleCellY=2;
    QList<QVector2D> singleCellSimReflectionPoints = {};
    // -------------
public:
    QElapsedTimer timer;
    qint64 simulation_time;

    void createCellsMatrix();

    // --- new : ---
    void createWalls();
    QGraphicsScene* createGraphicsScene();
    void computeReflections(Receiver* _RX, const QVector2D& _TX);
    void computeDirect(Receiver* _RX, const QVector2D& _TX);
    void addReflection(Ray* _ray, const QVector2D& _p1, const QVector2D& _p2, Obstacle* wall);
    complex<qreal> makeTransmission(RaySegment* ray_segment, Obstacle* wall);
    void checkTransmissions(Ray* _ray, QList<Obstacle*> _reflection_walls);
    complex<qreal> computePerpendicularGamma(qreal _cos_theta_i, qreal _cos_theta_t, Obstacle* wall);
    complex<qreal> computeReflectionCoeff(qreal _cos_theta_i, qreal _sin_theta_i, qreal _cos_theta_t, qreal _sin_theta_t, Obstacle* wall);
    complex<qreal> computeTransmissionCoeff(qreal _cos_theta_i, qreal _sin_theta_i, qreal _cos_theta_t, qreal _sin_theta_t, Obstacle* wall);
    bool checkSameSideOfWall(const QVector2D& _normal, const QVector2D& _TX, const QVector2D& _RX);
    bool checkRaySegmentIntersectsWall(const Obstacle* wall, RaySegment* ray_segment, QPointF* intersection_point=nullptr);
    QVector2D computeImage(const QVector2D& _point, Obstacle* wall);
    QVector2D calculateReflectionPoint(const QVector2D& _start, const QVector2D& _end, Obstacle* wall);

    void showView();
    void addLegend(QGraphicsScene* scene);
};

#endif // SIMULATION_H
