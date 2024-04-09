#include "obstacle.h"

#include <QPen>

Obstacle::Obstacle(
    QPoint start_coordinates,
    QPoint end_coordinates,
    ObstacleType material,
    qreal relative_permittivity,
    qreal conductivity,
    int thickness_cm)
{
    this->material = material;
    this->start_coordinates = start_coordinates;
    this->end_coordinates = end_coordinates;
    this->properties.relative_permittivity = relative_permittivity; // set properties hard coded in the switch case below ?
    this->properties.conductivity = conductivity; // set properties hard coded in the switch case below ?
    this->thickness_cm = thickness_cm;

    this->drawing_thickness = 2;

    QLineF line(this->start_coordinates,this->end_coordinates); // in func args ?
    setLine(line);
    QPen *pen = new QPen();
    pen->setWidth(this->drawing_thickness);

    switch (this->material) {
    case BrickWall:
        pen->setColor(Qt::darkRed);
        break;
    case Window:
        pen->setColor(Qt::cyan);
        break;
    case MetalWall:
        pen->setColor(Qt::gray);
        break;
    case DryWall:
        pen->setColor(Qt::lightGray);
        break;
    case ConcreteWall:
        pen->setColor(Qt::green);
        break;
    default:
        pen->setColor(Qt::white);
        break;
    }
    setPen(*pen);

    //setAcceptHoverEvents(true); // really needed ?
}

ObstacleType Obstacle::getMaterial() {
    return this->material;
}

QPoint Obstacle::getStartCoordinates() {
    return this->start_coordinates;
}

QPoint Obstacle::getEndCoordinates() {
    return this->end_coordinates;
}

qreal Obstacle::getRelativePermittivity() {
    return this->properties.relative_permittivity;
}

qreal Obstacle::getConductivity() {
    return this->properties.conductivity;
}

