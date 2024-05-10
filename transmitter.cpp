#include "transmitter.h"

Transmitter::Transmitter(int selector_index, QString name, int power_dBm, QPointF coordinates)
{
    //setAcceptHoverEvents(true);
    this->selector_index = selector_index;
    this->name = name;
    this->power_dBm = power_dBm;
    this->coordinates = coordinates;
}

int Transmitter::getPower_dBm() const
{
    return this->power_dBm;
}

double Transmitter::getPower() const
{
    return pow(10, this->power_dBm / 10);
}

void Transmitter::setPower_dBm(int power_dBm)
{
    this->power_dBm = power_dBm;
}

QPointF Transmitter::getCoordinates() const
{
    return this->coordinates;
}

void Transmitter::changeCoordinates(QPointF new_coordinates)
{
    this->coordinates = new_coordinates;
    // TODO: update the GraphicsScene to show base station at new coordinates.
}

QVector2D Transmitter::get2DVector() const
{
    return QVector2D(this->coordinates);
}


