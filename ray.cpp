#include "ray.h"

#include <QPen>


/*
Ray::Ray(QPointF start_point, QPointF end_point)
{
    this->end_point = end_point;
    this->points.push_back(start_point);
}
*/
/*
Ray::Ray(QPointF start_point, QSharedPointer<Receiver> target_cell)
{
    this->points.push_back(start_point);
    this->target_cell = target_cell;
    // TODO: ?
}


void Ray::addPoint(QPointF point)
{
    this->points.push_back(point);
}

void Ray::addInteractionPoint(InteractionPoint point)
{
    this->interaction_points.push_back(point);
}

QList<QPointF> Ray::getFinishedRayPoints() const
{
    QList<QPointF> finished_ray = this->points;
    finished_ray.push_front(this->end_point); // adds end point (receiver point) to end of ray's list of points
    return finished_ray;
}
*/

Ray::Ray(QPointF start, QPointF end) {
    // Ray object constructor
    this->start=start;
    this->end=end;
}

void Ray::addCoeff(qreal coeff_module) {
    // add a Transmission or Reflection coefficient to this ray's list of coeffs
    qDebug() << "Adding coeff to ray:" << coeff_module;
    // which one to use ? :
    this->coeffsList.append(coeff_module);
    this->totalCoeffs*=pow(coeff_module,2);
}

qreal Ray::getTotalCoeffs() {
    // returns the total product of all of the ray's coefficients multiplied by the exponent term
    qreal res = totalCoeffs;
    res *= pow(abs(exp(-j*beta_0*this->distance)/this->distance),2); // exp term
    qDebug() << "getTotalCoeffs ray:" << res;
    return res;

    //qreal res = 0;
    //for (complex<qreal> coeff : this->coeffsList) {
    //    res*=pow(abs(coeff),2); // all coeffs
    //}
    //res*=pow(abs(exp(-j*beta_0*this->distance)/this->distance),2); // exp term
    //qDebug() << "computeAllCoeffs:" << res;
    //return res;
}

QList<QGraphicsLineItem*> Ray::getSegmentsGraphics(){
    // returns this ray's graphics: list of its segment's QGraphicsItems
    qDebug() << "Getting ray segments graphics";
    QPen ray_pen;
    ray_pen.setWidthF(0.1);
    // set ray graphics color depending on number of reflections
    qDebug() << "This ray has" << this->segments.length() << "segments, so" << this->segments.length()-1 << "reflections";
    this->num_reflections=this->segments.length()-1;
    switch (this->num_reflections){
    case 0: // no reflections: direct ray
        qDebug() << "This ray is direct";
        ray_pen.setColor(Qt::green);
        break;
    case 1: // 1 reflection
        qDebug() << "This ray has 1 reflection";
        ray_pen.setColor(Qt::red);
        break;
    case 2: // 2 reflection
        qDebug() << "This ray has 2 reflections";
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

qreal Ray::getTotalDistance() const {
    // returns the total distance of this ray (which is the either the sum of the distances of each segments,
    // or the distance between the last image and RX)

    // TODO: ?
    //qreal d = 0;
    //for (RaySegment* segment : this->segments) {
    //    d += segment->distance;
    //}
    //return d;
    qDebug() << "Ray getTotalDisctance:" << this->distance;

    return this->distance;
}
