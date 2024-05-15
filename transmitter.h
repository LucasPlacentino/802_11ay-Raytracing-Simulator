#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <QGraphicsEllipseItem>
#include <QVector2D>
#include <QPen>
#include <QBrush>

#include "parameters.h"

class Transmitter : public QVector2D
{
public:
    int getPower_dBm() const;
    qreal getPower() const;
    qreal getG_TXP_TX() const;
    void setPower_dBm(int power_dBm);
    QPointF getCoordinates() const;
    void changeCoordinates(QPointF new_coordinates);

    int selector_index;
    QString name;

    Transmitter(qreal x, qreal y, int selector_index, QString name);

    QGraphicsEllipseItem* graphics = new QGraphicsEllipseItem(); // TX's QGraphicsItem
    qreal power = P_TX; // ! in Watts, 20dBm = 0.1W
    qreal power_dBm = P_TX_dBm; // 20dBm
    qreal gain = G_TX; // 1.64 or 1.7 ?

private:
    void setGraphicsRect(qreal x,qreal y);
};

#endif // TRANSMITTER_H
