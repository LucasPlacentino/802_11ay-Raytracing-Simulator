#ifndef RAY_H
#define RAY_H

#include <QGraphicsLineItem>


class Ray : public QGraphicsLineItem
{
public:
    Ray();

    qreal start_x;
    qreal start_y;
    qreal end_x;
    qreal end_y;


private:

};

#endif // RAY_H
