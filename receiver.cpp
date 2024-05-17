#include "receiver.h"

#include <QBrush>
#include <QPen>
#include "parameters.h"

static constexpr qreal max_power_dBm = -40.0;
static constexpr qreal min_power_dBm = -90.0;
static constexpr qreal max_bitrate_Mbps = 40000;
static constexpr qreal min_bitrate_Mbps = 50;

Receiver::Receiver(qreal x, qreal y, qreal resolution, bool showOutline) {
    // Receiver object constructor
    QBrush rxBrush(Qt::black);
    QPen rxPen;
    if (showOutline){
        rxPen.setColor(Qt::black);
        rxPen.setWidthF(10*0.01);
    } else {
        rxPen.setColor(Qt::transparent);
        rxPen.setWidth(0);
    }

    this->setX(x);
    this->setY(y);
    this->graphics->setToolTip(QString("Receiver x=%1 y=%2").arg(this->x(),this->y()));
    this->graphics->setBrush(rxBrush);
    this->graphics->setPen(rxPen);
    this->graphics->setRect(10*(x-resolution/2),10*(y-resolution/2),10*resolution,10*resolution);
    this->graphics->setAcceptHoverEvents(true);
}
void Receiver::updateBitrateAndColor()
{
    qreal bitrate;
    qreal power_dBm = 10*std::log10(this->power*1000);
    if (this->power != this->power) {
        bitrate = 9999999999999999.9;
        this->cell_color = QColor::fromRgb(255,192,203); // pink
        qDebug() << "--- ! ERROR: Cell has NaN power ! ---";
    } else if (power_dBm > max_power_dBm) {
        bitrate = max_bitrate_Mbps; //in Mbps, 40 Gbps max from -40 dBm
        this->cell_color = QColor::fromRgb(255,0,0); // red
    } else if (power_dBm < min_power_dBm) { // 50 Mbps at -90 dBm
        bitrate = 0; //in Mbps, no connection (0 Mbps)
        this->cell_color = Qt::black; // Qt::transparent or Qt::black or Qt::darkBlue ?
    } else {
        // Conversion to bitrate (beware log scale)
        qreal max_power_mW = std::pow(10.0, max_power_dBm / 10.0);
        qreal min_power_mW = std::pow(10.0, min_power_dBm / 10.0);

        //bitrate = min_bitrate_Mbps + (((this->power - min_power_mW/1000) / (max_power_mW/1000 - min_power_mW/1000)) * (max_bitrate_Mbps - min_bitrate_Mbps));
        // OR ?
        qreal max_bitrate_dB = 10*log10(max_bitrate_Mbps);
        qreal min_bitrate_dB = 10*log10(min_bitrate_Mbps);
        qreal bitrate_dB = min_bitrate_dB + (((power_dBm - min_power_dBm) / (max_power_dBm - min_power_dBm)) * (max_bitrate_dB - min_bitrate_dB));
        //qDebug() << "bitrate dB:" << bitrate_dB;
        bitrate = pow(10.0, bitrate_dB / 10.0);
        //qDebug() << "bitrate (Mbps):" << bitrate;

        //qreal value_normalized = (power_dBm - min_power_dBm) / (max_power_dBm - min_power_dBm);
        // or
        ////qreal value_normalized = (this->power*1000 - min_power_mW) / (max_power_mW - min_power_mW);
        // or
        //qreal value_normalized = (qreal(bitrate) - qreal(min_bitrate_Mbps)) / (qreal(max_bitrate_Mbps) - qreal(min_bitrate_Mbps));
        // or
        qreal value_normalized = (bitrate_dB - min_bitrate_dB) / (max_bitrate_dB - min_bitrate_dB);
        //qDebug() << "value_normalized:" << value_normalized;

        QColor color = computeColor(value_normalized);
        // testing in monochrome :
        //QColor color = QColor::fromRgbF(value_normalized,value_normalized,value_normalized);
        //qDebug() << "cell color:" << color;

        this->cell_color = color;
    }
    //// DEBUG:
    ////this->cell_color = Qt::black; // set ALL cells to black

    this->bitrate_Mbps = bitrate;
}

QColor Receiver::computeColor(qreal value)
{
    // Maps a normalized value to a 5 color gradient

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
        res+=ray->getTotalCoeffs(); // sum of all the rays' total coefficients and exp term, returns |G*G*T|^2 *|exp|^2
    }
    //qDebug() << "computeTotalPower res+=ray->getTotalCoeffs" << res;
    // multiply by the term before the sum:
    res *= (60*pow(lambda,2))/(8*pow(M_PI,2)*Ra)*transmitter->gain*transmitter->power;

    if (res != res) {
        qDebug() << "computeTotalPower: NaN !!!";
    }

    //qDebug() << "computeTotalPower:" << res;
    return res;
}



