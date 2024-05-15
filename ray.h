#ifndef RAY_H
#define RAY_H

#include "raysegment.h"

#include <QGraphicsLineItem>
#include <complex>

using namespace std;

class Ray
{
public:
    Ray(QPointF start, QPointF end);
    QList<RaySegment*> segments; // list of this ray's segment(s)
    int num_reflections = 0;
    QPointF start; // ray's starting point (TX)
    QPointF end; // ray's end point (RX)
    QList<complex<qreal>> coeffsList; // list of ray's transmission and reflection coefficients
    //qreal totalCoeffs=1; // product of all of the ray's coefficients (|T_1|^2 * |G_1|^2 * ...)
    qreal distance=1e12; // default distance, WILL be changed anyway programmatically
    void addCoeff(complex<qreal> coeff);

    qreal getTotalCoeffs();

    QList<QGraphicsLineItem*> getSegmentsGraphics();

    qreal getTotalDistance();
};

#endif // RAY_H
