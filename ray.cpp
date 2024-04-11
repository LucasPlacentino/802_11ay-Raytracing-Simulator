#include "ray.h"


/*
Ray::Ray(QPointF start_point, QPointF end_point)
{
    this->end_point = end_point;
    this->points.push_back(start_point);
}
*/

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
