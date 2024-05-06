#ifndef RAY_H
#define RAY_H

#include "raysegment.h"
#include "receiver.h"
#include "parameters.h"

#include <QGraphicsLineItem>
#include <complex>

using namespace std;

enum Interaction {
    TRANSMISSION,
    REFLECTION
};

struct InteractionPoint {
    QPointF coordinates; // TODO: change to QVector2D ?
    Interaction type;
    qreal power_attenutation;
    qreal incidence_angle; // needed ?
};

class Ray : public QGraphicsLineItem
{
public:
    /*
    //Ray(QPointF start_point, QPointF end_point);
    // OR ? :
    Ray(QPointF start_point, QSharedPointer<Receiver> target_cell);
    ////Ray(QVector2D transmitter_vector, QVector2D cell_vector);

    int num_reflections = 0;
    void addPoint(QPointF point);
    void addInteractionPoint(InteractionPoint point);
    QList<QPointF> getFinishedRayPoints() const;
    */


    Ray(QPointF start, QPointF end);
    QList<RaySegment*> segments; // list of this ray's segment(s)
    int num_reflections = 0;
    QPointF start; // ray's starting point (TX)
    QPointF end; // ray's end point (RX)
    QList<complex<qreal>> coeffsList; // list of ray's transmission and reflection coefficients
    qreal totalCoeffs=1; // product of all of the ray's coefficients (|T_1|^2 * |G_1|^2 * ...)
    qreal distance=1e10; //? not used ? // total ray's distance (default is 1e10 instead of 0 to avoid inf power)

    void addCoeff(qreal coeff_module);

    qreal getTotalCoeffs();

    QList<QGraphicsLineItem*> getSegmentsGraphics();

    qreal getTotalDistance() const;


private:
    QList<InteractionPoint> interaction_points;
    QList<QPointF> points; // TODO: change to QVector2D ?
    QSharedPointer<Receiver> target_cell;
    QPointF end_point;

};

#endif // RAY_H
