#ifndef RAYSEGMENT_H
#define RAYSEGMENT_H

#include <QGraphicsLineItem>
#include <QLineF>


class RaySegment : public QLineF
{
public:
    qreal distance; //? not used ?
    QGraphicsLineItem* graphics = new QGraphicsLineItem(); // segment's QGraphicsItem
    RaySegment(qreal start_x, qreal start_y, qreal end_x, qreal end_y);
};

#endif // RAYSEGMENT_H
