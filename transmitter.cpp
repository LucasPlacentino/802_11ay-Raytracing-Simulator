#include "transmitter.h"

Transmitter::Transmitter(int selector_index, QString name, int power_dBm) {
    this->selector_index = selector_index;
    this->name = name;
    this->power_dBm = power_dBm;
}

int Transmitter::getPower_dBm()
{
    return this->power_dBm;
}

void Transmitter::setPower_dBm(int power_dBm)
{
    this->power_dBm = power_dBm;
}


