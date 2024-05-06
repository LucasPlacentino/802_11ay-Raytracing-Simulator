#include "transmitter.h"

/*
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

qreal Transmitter::getGain() const
{
    return this->gain;
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
*/

Transmitter::Transmitter(qreal x, qreal y){
    // Transmitter object constructor
    QBrush txBrush(Qt::white);
    QPen txPen(Qt::darkGray);

    this->setX(x);
    this->setY(y);
    this->graphics->setToolTip(QString("Test transmitter x=%1 y=%2").arg(this->x(),this->y()));
    this->graphics->setBrush(txBrush);
    this->graphics->setPen(txPen);
    this->graphics->setRect(x-3,-y-3,6,6);
    this->graphics->setAcceptHoverEvents(true);
};

