#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <QGraphicsEllipseItem>
#include <QVector2D>
#include <QPen>
#include <QBrush>

#include "parameters.h"

class Transmitter : public QVector2D// : public QGraphicsEllipseItem // QGraphicsEllipseItem?
{
public:

    /*
    Transmitter(
        int selector_index,
        QString name,
        int power_dBm,
        QPointF coordinates // QPoint ? // TODO: change to QVector2D ?
        );
    */
    int getPower_dBm() const;
    qreal getPower() const;
    qreal getG_TXP_TX() const;
    void setPower_dBm(int power_dBm);
    QPointF getCoordinates() const;
    void changeCoordinates(QPointF new_coordinates);

    //QVector2D get2DVector() const;

    //QGraphicsEllipseItem* graphics = new QGraphicsEllipseItem();

    int selector_index;
    QString name;

    Transmitter(qreal x, qreal y, int selector_index, QString name);
    /* // uses parent class implementation
    qreal x() const{
        return this->QPointF::x();
    }
    qreal y() const{
        return this->QPointF::y();
    }
    */
    QGraphicsEllipseItem* graphics = new QGraphicsEllipseItem(); // TX's QGraphicsItem
    qreal power = 0.1; // ! in Watts, 20dBm = 0.1W
    qreal power_dBm = 20;
    // TODO:
    //qreal gain = 1.64; // calculé dans le rapport
    qreal gain = G_TX; // 1.64
    //qreal G_TX; // ?
    //qreal gain = G_TX;

private:
    void setGraphicsRect(qreal x,qreal y);

    /*
    int selector_index;
    QString name;
    int power_dBm;
    qreal gain;
    QPointF coordinates; // TODO: change to QVector2D ?
    qulonglong frequency = 60*1e9; // 802.11ay spec. Compiler complains with 60e9
    qulonglong* f = &frequency;
    // qreal beta_0 = ...;
    // qreal h_e = ...;
    */
};

#endif // TRANSMITTER_H
