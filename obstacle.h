#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <QGraphicsRectItem>
#include <QPoint> // TODO: this one?
#include <complex>
#include "parameters.h"


struct WallProperties {
    qreal relative_permittivity; // epsilon_r
    qreal conductivity; // sigma
    double epsilon; // permittivity
    std::complex<double> Z_m; // impedance
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
    //std::complex<qreal> gamma_m;
    //qreal alpha_m;
    //qreal beta_m;
    //qreal beta_0;
    //std::complex<qreal> Z;
    //qreal Z_0;
};

class Obstacle: Wall, public QGraphicsLineItem//: QGraphicsPolygonItem // QGraphicsRectItem? QGraphicsItem? QGraphicsPolygonItem? QGraphicsPathItem?
{
public:
    Obstacle(
        QPointF start_coordinates, // TODO: change to QVector2D ?
        QPointF end_coordinates, // TODO: change to QVector2D ?
        ObstacleType material,
        //qreal relative_permittivity, // $\epsilon_r$
        //qreal conductivity, // $\sigma$
        int thickness_cm
    ); // constructor
    //Obstacle(Obstacle&& other) noexcept; // move constructor

    ObstacleType getMaterial();
    QPointF getStartCoordinates() const;
    QPointF getEndCoordinates() const;
    qreal getRelativePermittivity() const; // really needed?
    qreal getConductivity() const; // really needed?
private:
    int drawing_thickness;
    int thickness_cm;
    QPointF start_coordinates;
    QPointF end_coordinates;
    /* // in Wall class
    ObstacleType type;
    float relative_permittivity;
    float conductivity;
    */
};

#endif // OBSTACLE_H
