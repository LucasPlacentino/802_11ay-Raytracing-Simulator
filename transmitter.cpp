#include "transmitter.h"

Transmitter::Transmitter(int selector_index, QString name, int power_dBm, QPoint coordinates)
{
    this->selector_index = selector_index;
    this->name = name;
    this->power_dBm = power_dBm;
    this->coordinates = coordinates;
}

int Transmitter::getPower_dBm()
{
    return this->power_dBm;
}

double Transmitter::getPower()
{
    return pow(10, this->power_dBm / 10);
}

void Transmitter::setPower_dBm(int power_dBm)
{
    this->power_dBm = power_dBm;
}

QPoint Transmitter::getCoordinates()
{
    return this->coordinates;
}

void Transmitter::changeCoordinates(QPoint new_coordinates)
{
    this->coordinates = new_coordinates;
}


