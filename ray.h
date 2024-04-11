#ifndef RAY_H
#define RAY_H

#include "receiver.h"

#include <QGraphicsLineItem>

enum Interaction {
    TRANSMISSION,
    REFLECTION
};

struct InteractionPoint {
    QPointF coordinates;
    Interaction type;
    qreal power_attenutation;
    qreal incidence_angle; // needed ?
};

class Ray : public QGraphicsLineItem
{
public:
    //Ray(QPointF start_point, QPointF end_point);
    // OR ? :
    Ray(QPointF start_point, QSharedPointer<Receiver> target_cell);
    ////Ray(QVector2D transmitter_vector, QVector2D cell_vector);

    int num_reflections = 0;
    void addPoint(QPointF point);
    void addInteractionPoint(InteractionPoint point);
    QList<QPointF> getFinishedRayPoints() const;

private:
    QList<InteractionPoint> interaction_points;
    QList<QPointF> points;
    QSharedPointer<Receiver> target_cell;
    QPointF end_point;

};

#endif // RAY_H
