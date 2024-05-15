#ifndef RECEIVER_H
#define RECEIVER_H

#include "ray.h"
#include "transmitter.h"

#include <QGraphicsRectItem>
#include <QVector2D>

class Receiver : public QVector2D // each cell of the simulation grid acts like a receiver
{
public:

    Receiver(qreal x, qreal y, qreal resolution, bool showOutline = false);

    QGraphicsRectItem* graphics = new QGraphicsRectItem(); // RX's QGraphicsItem
    qreal power; // ! in Watts
    qulonglong bitrate_Mbps; // bitrate in Mbps
    qreal Ra = 73; // antenna resistance
    QColor cell_color = QColor(Qt::transparent); // receiver cell color, based on power/bitrate

    QList<Ray*> all_rays; // list of all rays that go to this receiver

    qreal computeTotalPower(Transmitter* transmitter); // returns final total power computation for this RX

    void updateBitrateAndColor(); // update this receiver's bitrate and cell color

    QColor computeColor(qreal value_normalized);
};

#endif // RECEIVER_H
