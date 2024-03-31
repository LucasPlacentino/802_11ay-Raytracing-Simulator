#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <QGraphicsRectItem>
#include <QPoint> // TODO: this one?
#include <complex>


struct WallProperties {
    qreal relative_permittivity;
    qreal conductivity;
};

enum ObstacleType {
    BrickWall,
    Window,
    MetalWall,
    DryWall,
    ConcreteWall
};

struct Wall {
    ObstacleType material;
    WallProperties properties;
    //std::complex<qreal> todo;
};

class Obstacle: Wall, QGraphicsLineItem//: QGraphicsPolygonItem // QGraphicsRectItem? QGraphicsItem? QGraphicsPolygonItem? QGraphicsPathItem?
{
public:
    Obstacle(
        QPoint start_coordinates,
        QPoint end_coordinates,
        ObstacleType material,
        qreal relative_permittivity, // $\epsilon_r$
        qreal conductivity, // $\sigma$
        int thickness
    ); // constructor

    ObstacleType getMaterial();
    QPoint getStartCoordinates();
    QPoint getEndCoordinates();
    qreal getRelativePermittivity(); // really needed?
    qreal getConductivity(); // really needed?
private:
    int thickness;
    QPoint start_coordinates;
    QPoint end_coordinates;
    /* // in Wall class
    ObstacleType type;
    float relative_permittivity;
    float conductivity;
    */
};

#endif // OBSTACLE_H
