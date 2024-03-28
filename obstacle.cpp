#include "obstacle.h"

Obstacle::Obstacle(ObstacleType type, QPoint start_coordinates, QPoint end_coordinates, float relative_permittivity, float conductivity) {
    this->type = type;
    this->start_coordinates = start_coordinates;
    this->end_coordinates = end_coordinates;
    this->relative_permittivity = relative_permittivity;
    this->conductivity = conductivity;
}

ObstacleType Obstacle::getType() {
    return this->type;
}

QPoint Obstacle::getStartCoordinates() {
    return this->start_coordinates;
}

QPoint Obstacle::getEndCoordinates() {
    return this->end_coordinates;
}

float Obstacle::getRelativePermittivity() {
    return this->relative_permittivity;
}

float Obstacle::getConductivity() {
    return this->conductivity;
}

