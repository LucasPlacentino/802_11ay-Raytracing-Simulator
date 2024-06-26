#include "obstacle.h"

#include <QPen>
#include <QVector2D>
#include "parameters.h"

/*
 * The floorplan is 8m x 15m (x=0,y=0 top left to x=15,y=8 bottom right) :
 *  __________________________
 * |      |    |      |      |
 * |      |    |      |      |
 * |      |    |      |      |
 * |      |_  _|___  _|      |
 * |______                  /
 * |       _________       /
 * |      |##I     |      /
 * |      |##I     |     /
 * -----------     ------
 *
 * _ or | : wall (concrete or drywall)
 * # : lift (metal walls)
 * I : metal door
 * / : glass window
 *
*/

Obstacle::Obstacle(QVector2D start, QVector2D end, ObstacleType material, qreal thickness)//, int id)
{
    // Wall object cosntructor
    //qDebug("Creating wall...");
    this->material = material;
    //this->id = id;
    this->thickness = thickness; // in meters
    this->line = QLineF(start.x(),start.y(), end.x(), end.y());
    QLineF graphics_line = QLineF(10*start.x(), 10*start.y(), 10*end.x(), 10*end.y());
    //qDebug() << "Wall" << id << "line:" << this->line ;
    QLineF normal_line = this->line.normalVector();
    //qDebug() << "Line" << id << "normal:" << normal_line;
    this->normal = QVector2D(normal_line.dx(),normal_line.dy()).normalized(); // ! normalized !
    //qDebug() << "Wall" << id << "normal:" << this->normal;
    QLineF unit_line = this->line.unitVector();
    //qDebug() << "Line" << id << "unitary:" << unit_line;
    this->unitary = QVector2D(unit_line.dx(),unit_line.dy()).normalized(); // ! normalized !
    //qDebug() << "Wall" << id << "unitary:" << this->unitary;
    //qDebug("Setting Wall graphics line...");
    this->graphics->setLine(graphics_line);

    QPen pen(Qt::gray);
    //pen.setWidthF(10*0.2);

    switch (this->material) {
    case BrickWall:
        pen.setColor(Qt::darkRed);
        pen.setWidthF(10*0.18);
        this->properties.relative_permittivity = 3.95;
        this->properties.conductivity = 0.073;
        break;
    case Window:
        pen.setColor(Qt::white);
        pen.setWidthF(10*0.08);
        this->properties.relative_permittivity = 6.3919;
        this->properties.conductivity = 0.00107;
        break;
    case MetalWall:
        pen.setColor(Qt::gray);
        pen.setWidthF(10*0.1);
        this->properties.relative_permittivity = 1;
        this->properties.conductivity = 1e7;
        break;
    case DryWall:
        pen.setColor(Qt::lightGray);
        pen.setWidthF(10*0.12);
        this->properties.relative_permittivity = 2.7;
        this->properties.conductivity = 0.05349;
        break;
    case ConcreteWall:
        pen.setColor(Qt::darkMagenta);
        pen.setWidthF(10*0.2);
        this->properties.relative_permittivity = 6.4954;
        this->properties.conductivity = 1.43;
        break;
    default:
        pen.setColor(Qt::white);
        pen.setWidthF(10*0.12);
        this->properties.relative_permittivity = 1e-10;
        this->properties.conductivity = 1e-10;
        break;
    }
    this->graphics->setPen(pen);

    this->properties.epsilon = epsilon_0 * this->properties.relative_permittivity;
    complex<qreal> epsilon_tilde = this->properties.epsilon - j *(this->properties.conductivity / omega);
    this->properties.Z_m = sqrt(mu_0 / epsilon_tilde);

    this->properties.gamma_m = j * omega * sqrt(mu_0 * epsilon_tilde);

    //qDebug("Wall created.");
    //qDebug() << this->material; // 1: glass, 4: concrete, 2: metal, 3: drywall, 0: brick
    //qDebug() << "Z_m" << this->properties.Z_m.real() << "+j" << this->properties.Z_m.imag();
    //qDebug() << "gamma_m" << this->properties.gamma_m.real() << "+j" << this->properties.gamma_m.imag();
}

ObstacleType Obstacle::getMaterial()
{
    return this->material;
}
