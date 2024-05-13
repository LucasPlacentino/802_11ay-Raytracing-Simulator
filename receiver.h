#ifndef RECEIVER_H
#define RECEIVER_H

#include "ray.h"
#include "transmitter.h"

#include <QGraphicsRectItem>
#include <QVector2D>

class Receiver : public QVector2D// : public QGraphicsRectItem // each cell of the simulation grid acts like a receiver
{
public:
    /*
    Receiver(double power_dBm, const QPointF center_coordinates); // TODO: change to QVector2D ?

    QGraphicsRectItem* graphics = new QGraphicsRectItem();

    double getPower_dBm() const;
    qulonglong getBitrateMbps() const;
    QColor getCellColor();
    QPointF getCenterCoordinates() const;

    QVector2D get2DVector() const;
    */

    Receiver(qreal x, qreal y, qreal resolution, bool showOutline);

    QGraphicsRectItem* graphics = new QGraphicsRectItem(); // RX's QGraphicsItem
    qreal power; // ! in Watts
    qulonglong bitrate_Mbps; // bitrate in Mbps
    qreal Ra = 73; // antenna resistance
    QColor cell_color = QColor(Qt::transparent); // receiver cell color, based on power/bitrate

    QList<Ray*> all_rays; // list of all rays that go to this receiver

    qreal computeTotalPower(Transmitter* transmitter); // returns final total power computation for this RX

    void updateBitrateAndColor(); // update this receiver's bitrate and cell color
protected:
    //void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
private:
    /*
    double power_dBm;
    qulonglong bitrate_Mbps;
    QColor cell_color;
    QPointF center_coordinates; // TODO: change to QVector2D ?
    */
    QColor computeColor(qreal value_noramlized);
};

#endif // RECEIVER_H
