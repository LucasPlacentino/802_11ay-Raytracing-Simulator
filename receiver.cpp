#include "receiver.h"

#include <QBrush>
#include <QPen>
#include "parameters.h"

static constexpr qreal max_power_dBm = -40.0;
static constexpr qreal min_power_dBm = -90.0;
static constexpr qlonglong max_bitrate_Mbps = 40000;
static constexpr qlonglong min_bitrate_Mbps = 50;

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

Receiver::Receiver(qreal x, qreal y, qreal resolution, bool showOutline) {
    // Receiver object constructor
    QBrush rxBrush(Qt::black);
    QPen rxPen;
    if (showOutline){
        rxPen.setColor(Qt::black);
    } else {
        rxPen.setColor(Qt::transparent);
    }
    rxPen.setWidthF(10*0.01);

    this->setX(x);
    this->setY(y);
    //this->graphics->setToolTip(QString("Test receiver x=%1 y=%2").arg(this->x(),this->y()));
    this->graphics->setBrush(rxBrush);
    this->graphics->setPen(rxPen);
    this->graphics->setRect(10*(x-resolution/2),10*(y-resolution/2),10*resolution,10*resolution);
    this->graphics->setAcceptHoverEvents(true);
}
void Receiver::updateBitrateAndColor()
{
    qlonglong bitrate;
    qreal power_dBm = 10*std::log10(this->power*1000);
    if (this->power != this->power) {
        bitrate = 9999999999999999;
        this->cell_color = QColor::fromRgb(255,192,203); // pink
        qDebug() << "--- ! ERROR: Cell has NaN power ! ---";
    } else if (power_dBm > max_power_dBm) {
        bitrate = max_bitrate_Mbps; //in Mbps, 40 Gbps max from -40 dBm
        this->cell_color = QColor(255,0,0);
    } else if (power_dBm < min_power_dBm) { // 50 Mbps at -90 dBm
        bitrate = 0; //in Mbps, no connection (0 Mbps)
        this->cell_color = Qt::darkBlue; // Qt::transparent or Qt::black ?
    } else {
        // Conversion to bitrate (beware log scale)
        qreal max_power_mW = std::pow(10.0, max_power_dBm / 10.0);
        qreal min_power_mW = std::pow(10.0, min_power_dBm / 10.0);

        bitrate = min_bitrate_Mbps + (((this->power - min_power_mW/1000) / (max_power_mW/1000 - min_power_mW/1000)) * (max_bitrate_Mbps - min_bitrate_Mbps));
        // OR ?
        //bitrate = min_bitrate_Mbps + (((power_dBm - min_power_dBm) / (max_power_dBm - min_power_dBm)) * (max_bitrate_Mbps - min_bitrate_Mbps));
        //qDebug() << "bitrate (Mbps):" << bitrate;

        //qreal value_normalized = (power_dBm - min_power_dBm) / (max_power_dBm - min_power_dBm);
        // or
        //qreal value_normalized = (this->power*1000 - min_power_mW) / (max_power_mW - min_power_mW);
        // or
        qreal value_normalized = (qreal(bitrate) - qreal(min_bitrate_Mbps)) / (qreal(max_bitrate_Mbps) - qreal(min_bitrate_Mbps));
        //qDebug() << "value_normalized:" << value_normalized;

        QColor color = computeColor(value_normalized);
        // testing in monochrome :
        //QColor color = QColor::fromRgbF(value_normalized,value_normalized,value_normalized);
        //qDebug() << "cell color:" << color;

        this->cell_color = color;
    }
    this->bitrate_Mbps = bitrate;
}

QColor Receiver::computeColor(qreal value)
{
    // Maps a normalized value to a 5 color gradient
    //// Ensure value is within the range [0, 1]
    ////value = qBound(0.0, value, 1.0);

    // Define colors for the heatmap gradient
    QColor colors[] = {
        QColor(0, 0, 255),   // Blue
        QColor(0, 255, 255), // Cyan
        QColor(0, 255, 0),   // Green
        QColor(255, 255, 0), // Yellow
        QColor(255, 0, 0)    // Red
    };

    // Determine which two colors to interpolate between
    int index1 = value * 4;
    int index2 = index1 + 1;

    // Calculate interpolation factor
    qreal factor = (value * 4) - index1;

    // Interpolate between the two colors
    QColor color = colors[index1].toRgb();
    QColor next_color = colors[index2].toRgb();

    int red = color.red() + factor * (next_color.red() - color.red());
    int green = color.green() + factor * (next_color.green() - color.green());
    int blue = color.blue() + factor * (next_color.blue() - color.blue());

    return QColor(red, green, blue);
}


qreal Receiver::computeTotalPower(Transmitter* transmitter) // returns final total power computation for this RX
{
    qreal res = 0;
    for (Ray* ray : this->all_rays) {
        res+=ray->getTotalCoeffs(); // sum of all the rays' total coefficients and exp term, returns |T|²|G|²|exp|²
    }
    //qDebug() << "computeTotalPower res+=ray->getTotalCoeffs" << res;
    // multiply by the term before the sum:
    res *= (60*pow(lambda,2))/(8*pow(M_PI,2)*Ra)*transmitter->G_TXP_TX*transmitter->power_dBm; // TODO: *transmitter->gain*transmitter->power plutot que *G_TXP_TX

    if (res != res) {
        qDebug() << "computeTotalPower: NaN !!!";
    }

    //qDebug() << "computeTotalPower:" << res;
    return res;
}



