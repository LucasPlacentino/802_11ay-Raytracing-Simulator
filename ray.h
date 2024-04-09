#ifndef RAY_H
#define RAY_H

#include <QGraphicsLineItem>


class Ray : public QGraphicsLineItem
{
public:
    Ray(QPointF start_point, QPointF end_point);
    // OR ? :
    //Ray(Transmitter* base_station, Receiver* cell);
    //Ray(QVector2D transmitter_vector, QVector2D cell_vector);

    int num_reflections = 0;
    void addPoint(QPointF point);
    QList<QPointF> getFinishedRayPoints() const;

private:
    QList<QPointF> points;
    QPointF end_point;

};

#endif // RAY_H
