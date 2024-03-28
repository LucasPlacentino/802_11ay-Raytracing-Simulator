#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <QGraphicsRectItem>
#include <QPoint> // TODO: this one?


enum ObstacleType {
    BrickWall,
    Window,
    MetalWall,
    DryWall,
    ConcreteWall
};

class Obstacle//: QGraphicsPolygonItem // QGraphicsRectItem? QGraphicsItem? QGraphicsPolygonItem? QGraphicsPathItem?
{
public:
    Obstacle(
        ObstacleType type,
        QPoint start_coordinates,
        QPoint end_coordinates,
        float relative_permittivity,
        float conductivity
    ); // constructor

    ObstacleType getType();
    QPoint getStartCoordinates();
    QPoint getEndCoordinates();
    float getRelativePermittivity(); // really needed?
    float getConductivity(); // really needed?
private:
    ObstacleType type;
    QPoint start_coordinates;
    QPoint end_coordinates;
    float relative_permittivity;
    float conductivity;
};

#endif // OBSTACLE_H
