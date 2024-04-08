#include "receiver.h"

Receiver::Receiver(int power_dBm)
{
    this->cell_color = Qt::black;

    if (power_dBm > -40) {
        this->bitrate = 40*1e9;
        this->cell_color = Qt::red;
    } else if (power_dBm < -90) {
        this->bitrate = 0;
        this->cell_color = Qt::black;
    } else {
        // TODO: conversion to bitrate (beware log scale)

        double value = 0.5; // TODO: normalize power_dBm (or bitrate) to [0,1]
        double h = (1 - value) * 100;
        double s = 100;
        double l = value * 50;
        this->cell_color = QColor::fromHsl(h, s, l); // or QColor::fromHsv()
    }

    setAcceptHoverEvents(true); // show details on mouse hover
}

int Receiver::getPower_dBm()
{
    return this->power_dBm;
}

qulonglong Receiver::getBitrate()
{
    return this->bitrate;
}

QColor Receiver::getCellColor()
{
    return this->cell_color;
}

