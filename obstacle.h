#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <QGraphicsRectItem>
#include <complex>
#include <QVector2D>

using namespace std;

struct WallProperties {
    qreal relative_permittivity; // epsilon_r
    qreal conductivity; // sigma
    qreal epsilon; // permittivity = epsilon_0 * epsilon_r
    complex<qreal> Z_m; // impedance
    qreal alpha_m;
    qreal beta_m;
    complex<qreal> gamma_m; // gamma_m = alpha_m + j*beta_m
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
};

class Obstacle: public Wall
{
public:
    Obstacle(
        QVector2D start,
        QVector2D end,
        ObstacleType material,
        qreal thickness//,
        //int id
    );
    QGraphicsLineItem* graphics = new QGraphicsLineItem(); // wall's QGraphicsItem
    QLineF line; // wall's QLineF, for intersects()
    QVector2D normal; // wall's normal vector ! normalized !
    QVector2D unitary; // wall's unitary vector ! normalized !
    int id=0; // only used for debugging
    qreal thickness; // in meters

    ObstacleType getMaterial();
};

#endif // OBSTACLE_H
