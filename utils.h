#ifndef UTILS_H
#define UTILS_H

#include <QVector2D>


QVector2D transpose2DVector(QVector2D *vector, bool dir)
{
    qreal x = vector->x();
    qreal y = vector->y();
    if (dir) {
        x = -x;
    }  else {
        y = -y;
    }
    return QVector2D(y,x);
}

bool checkSameSideOfWall(QVector2D n, QVector2D rx, QVector2D tx)
{
    float left = QVector2D::dotProduct(n,rx);
    bool left_sign = left > 0.0;
    float right = QVector2D::dotProduct(n,tx);
    bool right_sign = right > 0.0;
    return left_sign==right_sign;
}

#endif // UTILS_H
