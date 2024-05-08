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
*/

Transmitter::Transmitter(qreal x, qreal y, int selector_index, QString name){
    // Transmitter object constructor
    QBrush txBrush(Qt::white);
    QPen txPen(Qt::gray);
    txPen.setWidthF(6*0.07);

    this->selector_index = selector_index;
    this->name = name;

    this->setX(x);
    this->setY(y);
    this->graphics->setToolTip(QString("Test transmitter x=%1 y=%2").arg(this->x(),this->y()));
    this->graphics->setBrush(txBrush);
    this->graphics->setPen(txPen);
    //this->graphics->setRect(10*x-1.5,10*y-1.5,3,3);
    this->setGraphicsRect(x,y);
    this->graphics->setAcceptHoverEvents(true);
};

void Transmitter::setGraphicsRect(qreal x,qreal y)
{
    this->graphics->setRect(10*x-1.2,10*y-1.2,2.4,2.4);
}

int Transmitter::getPower_dBm() const
{
    return 10*std::log10(this->power*1000);
}

double Transmitter::getPower() const
{
    return this->power;
}

qreal Transmitter::getG_TXP_TX() const
{
    return this->G_TXP_TX;
}

void Transmitter::setPower_dBm(int power_dBm)
{
    // TODO: correct ?
    this->power = (10^(power_dBm/10))/1000; // dBm to Watts
    qDebug() << "setPower:" << this->power << "Watts";
}

/*
QPointF Transmitter::getCoordinates() const
{
    return this->coordinates;
}
*/

void Transmitter::changeCoordinates(QPointF new_coordinates)
{
    this->setX(new_coordinates.x());
    this->setY(new_coordinates.y());
    this->setGraphicsRect(this->x(),this->y());
    //this->coordinates = new_coordinates;
    // TODO: update the GraphicsScene to show base station at new coordinates ?
}

