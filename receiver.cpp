#include "receiver.h"

double max_power_dBm = -40.0;
double min_power_dBm = -90.0;
qulonglong max_bitrate_Mbps = 40*1e3;
qulonglong min_bitrate_Mbps = 50;

Receiver::Receiver(double power_dBm)
{
    this->cell_color = Qt::black;

    if (power_dBm > max_power_dBm) {
        this->bitrate_Mbps = max_bitrate_Mbps; //in Mbps, 40 Gbps max from -40 dBm
        this->cell_color = Qt::red;
    } else if (power_dBm < min_power_dBm) { // 50 Mbps at -90 dBm
        this->bitrate_Mbps = 0; //in Mbps, no connection (0 Mbps)
        this->cell_color = Qt::black;
    } else {
        // TODO: conversion to bitrate (beware log scale)
        double max_power_mW = std::pow(10.0, max_power_dBm / 10.0);
        double min_power_mW = std::pow(10.0, min_power_dBm / 10.0);
        double power_mW = std::pow(10.0, power_dBm / 10.0);
        this->bitrate_Mbps = ((power_mW - min_power_mW) / (max_power_mW - min_power_mW)) * (max_bitrate_Mbps - min_bitrate_Mbps) + min_bitrate_Mbps;

        // Color gradient heatmap scale:
        // normalize power_dBm (or bitrate) to [0,360[
        int h = static_cast<int>((power_dBm - -90) * (360 - 0) / (-40 - -90) + 0) % 360; // modulo 360 because QColor::fromHsl() h is in [0,359]
        this->cell_color = QColor::fromHsl(h, 255, 128); // or QColor::fromHsv()
    }

    this->setToolTip(QString("Signal: %1dBm").arg(this->power_dBm)); // here ? or in a ::hoverMoveEvent() ?
    //setAcceptHoverEvents(true); // trigger function below on mouse hover events (i.e show a tooltip)
}

/*
void Receiver::hoverMoveEvent(QGraphicsSceneHoverEvent* event) // ?
{
    this->setToolTip(QString("Signal: %1dBm").arg(this->power_dBm)); // here ? or just in constructor ?
}
*/

double Receiver::getPower_dBm()
{
    return this->power_dBm;
}

qulonglong Receiver::getBitrateMbps()
{
    return this->bitrate_Mbps;
}

QColor Receiver::getCellColor()
{
    return this->cell_color;
}

