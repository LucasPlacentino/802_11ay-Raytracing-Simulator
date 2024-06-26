#include "ray.h"

#include <QPen>
#include "parameters.h"

Ray::Ray(QPointF start, QPointF end) {
    // Ray object constructor
    this->start=start;
    this->end=end;
}

void Ray::addCoeff(complex<qreal> coeff) {
    // add a Transmission or Reflection coefficient to this ray's list of coeffs
    //qDebug() << "Adding coeff to ray:" << coeff_module;
    // which one to use ? :
    if (coeff.real() != coeff.real() || coeff.imag() != coeff.imag() ||coeff != coeff) {
        qDebug() << "NaN coeff";
    }
    this->coeffsList.append(coeff);
    //this->totalCoeffs*=pow(abs(coeff),2);
}

qreal Ray::getTotalCoeffs() {
    // returns the total product of all of the ray's coefficients multiplied by the exponent term
    //qreal res = totalCoeffs;
    //res *= pow(abs(exp(-j*beta_0*this->distance)/this->distance),2); // exp term
    ////qDebug() << "getTotalCoeffs ray:" << res;
    //return res;

    qreal distance_ray = getTotalDistance();
    //qDebug() << distance_ray;
    qreal res = 1;
    complex<qreal> res_q = 1;
    for (complex<qreal> coeff : this->coeffsList) {
        res_q*=coeff; // all coeffs
    }
    res = pow(abs(res_q),2);
    res*=pow(abs(exp(-j*beta_0*distance_ray)/distance_ray),2); // exp term
    //qDebug() << "computeAllCoeffs:" << res;
    if (res != res) {
        qDebug() << "NaN Total Coeff";
    }
    return res;
}

QList<QGraphicsLineItem*> Ray::getSegmentsGraphics(){
    // returns this ray's graphics: list of its segment's QGraphicsItems
    //qDebug() << "Getting ray segments graphics";
    QPen ray_pen;
    ray_pen.setWidthF(0.1);
    // set ray graphics color depending on number of reflections
    //qDebug() << "This ray has" << this->segments.length() << "segments, so" << this->segments.length()-1 << "reflections";
    this->num_reflections=this->segments.length()-1;
    switch (this->num_reflections){
    case 0: // no reflections: direct ray
        //qDebug() << "This ray is direct";
        ray_pen.setColor(Qt::green);
        break;
    case 1: // 1 reflection
        //qDebug() << "This ray has 1 reflection";
        ray_pen.setColor(Qt::red);
        break;
    case 2: // 2 reflection
        //qDebug() << "This ray has 2 reflections";
        ray_pen.setColor(Qt::yellow);
        break;
    }
    QList<QGraphicsLineItem*> ray_graphics;
    for (RaySegment* ray_segment: this->segments) {
        //for (int i=0; i<this->segments.length(); i++) {
        //qDebug() << "Adding this segment to list ray_graphics";
        ray_segment->graphics->setPen(ray_pen);
        //qDebug() << "before" << ray_segment->graphics;
        ray_graphics.append(ray_segment->graphics);
        //qDebug() << "after:" << ray_graphics;
    }
    return ray_graphics;
}

qreal Ray::getTotalDistance() {
    // returns the total distance of this ray (which is the either the sum of the distances of each segments,
    // or the distance between the last image and RX)

    // TODO: ?
    //qreal d = 0;
    //for (RaySegment* segment : this->segments) {
    //    d += segment->distance;
    //}
    //return d;

    // ray's distance has already been changed
    //qDebug() << "Ray getTotalDisctance:" << this->distance;
    return this->distance;
}
