#include "obstacle.h"

#include <QPen>

/*
 * The floorplan is 9m x 15m (x=0,y=0 top left to x=15,y=-9 bottom right) :
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
 * _ or | : wall
 * # : lift
 * I : metal door
 * / : glass window
 *
*/

Obstacle::Obstacle(
    QPointF start_coordinates,
    QPointF end_coordinates,
    ObstacleType material,
    //qreal relative_permittivity,
    //qreal conductivity,
    int thickness_cm)
{
    this->material = material;
    this->start_coordinates = start_coordinates;
    this->end_coordinates = end_coordinates;
    //this->properties.relative_permittivity = relative_permittivity; // set properties hard coded in the switch case below ?
    //this->properties.conductivity = conductivity; // set properties hard coded in the switch case below ?
    this->thickness_cm = thickness_cm;

    this->drawing_thickness = 2;

    QLineF line(this->start_coordinates,this->end_coordinates); // in func args ?
    setLine(line);
    QPen *pen = new QPen();
    pen->setWidth(this->drawing_thickness);

    switch (this->material) {
    case BrickWall:
        pen->setColor(Qt::darkRed);
        this->properties.relative_permittivity = 3.95;
        this->properties.conductivity = 0.073;
        break;
    case Window:
        pen->setColor(Qt::cyan);
        this->properties.relative_permittivity = 6.3919;
        this->properties.conductivity = 0.0107;
        break;
    case MetalWall:
        pen->setColor(Qt::gray);
        this->properties.relative_permittivity = 1;
        this->properties.conductivity = 10e7;
        break;
    case DryWall:
        pen->setColor(Qt::lightGray);
        this->properties.relative_permittivity = 2.7;
        this->properties.conductivity = 0.05349;
        break;
    case ConcreteWall:
        pen->setColor(Qt::green);
        this->properties.relative_permittivity = 6.4954;
        this->properties.conductivity = 1.43;
        break;
    default:
        pen->setColor(Qt::white);
        this->properties.relative_permittivity = 0;
        this->properties.conductivity = 0;
        break;
    }
    setPen(*pen);

    //setAcceptHoverEvents(true); // really needed ?
}

ObstacleType Obstacle::getMaterial()
{
    return this->material;
}

QPointF Obstacle::getStartCoordinates() const
{
    return this->start_coordinates;
}

QPointF Obstacle::getEndCoordinates() const
{
    return this->end_coordinates;
}

qreal Obstacle::getRelativePermittivity() const
{
    return this->properties.relative_permittivity;
}

qreal Obstacle::getConductivity() const
{
    return this->properties.conductivity;
}

