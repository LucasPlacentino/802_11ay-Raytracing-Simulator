#ifndef RECEIVER_H
#define RECEIVER_H

#include "ray.h"
#include "simulation.h"

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

    Receiver(qreal x, qreal y);
    QGraphicsRectItem* graphics = new QGraphicsRectItem(); // RX's QGraphicsItem
    qreal power; // ! in Watts

    qreal Ra = 73;

    QList<Ray*> all_rays;

    qreal computeTotalPower(Simulation* simulation); // returns final total power computation for this RX

protected:
    //void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
private:
    double power_dBm;
    qulonglong bitrate_Mbps;
    QColor cell_color;
    QPointF center_coordinates; // TODO: change to QVector2D ?
};

#endif // RECEIVER_H
