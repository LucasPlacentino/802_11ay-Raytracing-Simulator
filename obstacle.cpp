#include "obstacle.h"

Obstacle::Obstacle(QPoint start_coordinates, QPoint end_coordinates, ObstacleType material, qreal relative_permittivity, qreal conductivity, int thickness) {
    this->material = material;
    this->start_coordinates = start_coordinates;
    this->end_coordinates = end_coordinates;
    this->properties.relative_permittivity = relative_permittivity;
    this->properties.conductivity = conductivity;
    this->thickness = thickness;
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

