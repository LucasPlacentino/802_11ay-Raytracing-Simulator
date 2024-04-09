#ifndef RAY_H
#define RAY_H

#include <QGraphicsLineItem>


class Ray : public QGraphicsLineItem
{
public:
    Ray(QPointF start_point, QPointF end_point);

    QList<QPointF> points;
    int num_reflections = 0;

private:

};

#endif // RAY_H
