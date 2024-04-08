#include "receiver.h"

Receiver::Receiver(double power_dBm)
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

        // Color gradient heatmap scale:
        // normalize power_dBm (or bitrate) to [0,360]
        int h = static_cast<int>((power_dBm - -90) * (360 - 0) / (-40 - -90) + 0) % 360; // modulo 360 because QColor::fromHsl() h is in [0,359]
        this->cell_color = QColor::fromHsl(h, 255, 128); // or QColor::fromHsv()
    }

    setAcceptHoverEvents(true); // show details on mouse hover
}

double Receiver::getPower_dBm()
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

