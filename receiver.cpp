#include "receiver.h"

#include <QBrush>
#include <QPen>

double max_power_dBm = -40.0;
double min_power_dBm = -90.0;
qulonglong max_bitrate_Mbps = 40*1e3;
qulonglong min_bitrate_Mbps = 50;

/*
Receiver::Receiver(double power_dBm, const QPointF center_coordinates)
{
    // TODO: dont take power_dBm in constructor

    this->center_coordinates = center_coordinates;
    this->power_dBm = power_dBm;
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
        // normalize power_dBm (or bitrate) to [0,360[ or more like [0,240] so that we have between red and dark blue
        int h = static_cast<int>((power_dBm - -90) * (0 - 240) / (-40 - -90) + 240); // ? modulo 360 because QColor::fromHsl() h is in [0,359]
        this->cell_color = QColor::fromHsl(h, 255, 92); // or QColor::fromHsv(), h, 255 saturation, 128 or 92 lightness
    }

    this->graphics->setToolTip(QString("Signal: %1 dBm\nBitrate: %2 Mbps").arg(this->power_dBm, this->bitrate_Mbps)); // here ? or in a ::hoverMoveEvent() ?
    //setAcceptHoverEvents(true); // trigger function below on mouse hover events (i.e show a tooltip)
}


//void Receiver::hoverMoveEvent(QGraphicsSceneHoverEvent* event) // ?
//{
//    this->setToolTip(QString("Signal: %1 dBm\nBitrate: %2 Mbps").arg(this->power_dBm, this->bitrate_Mbps)); // here ? or just in constructor ?
//}


double Receiver::getPower_dBm() const
{
    return this->power_dBm;
}

qulonglong Receiver::getBitrateMbps() const
{
    return this->bitrate_Mbps;
}

QColor Receiver::getCellColor()
{
    return this->cell_color;
}

QPointF Receiver::getCenterCoordinates() const
{
    return this->center_coordinates;
}

QVector2D Receiver::get2DVector() const
{
    return QVector2D(this->center_coordinates); // but we use the mean power of the local zone (50cm x 50cm)
}
*/

Receiver::Receiver(qreal x, qreal y) {
    // Receiver object constructor
    QBrush rxBrush(Qt::blue);
    QPen rxPen(Qt::darkBlue);
    this->setX(x);
    this->setY(y);
    this->graphics->setToolTip(QString("Test receiver x=%1 y=%2").arg(this->x(),this->y()));
    this->graphics->setBrush(rxBrush);
    this->graphics->setPen(rxPen);
    this->graphics->setRect(x-3,-y-3,6,6);
    this->graphics->setAcceptHoverEvents(true);
}

qreal Receiver::computeTotalPower() // returns final total power computation for this RX
{
    qreal res = 0;
    for (Ray* ray : this->all_rays) {
        res+=ray->getTotalCoeffs(); // sum of all the rays' total coefficients and exp term
    }
    // multiply by the term before the sum:
    res *= (60*pow(lambda,2))/(8*pow(M_PI,2)*Ra)*G_TXP_TX; // TODO: *transmitter->gain*transmitter->power plutot que *G_TXP_TX

    qDebug() << "computeTotalPower:" << res;
    return res;
}
