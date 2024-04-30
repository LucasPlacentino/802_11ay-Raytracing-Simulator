#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <QGraphicsEllipseItem>
#include <QVector2D>


class Transmitter// : public QGraphicsEllipseItem // QGraphicsEllipseItem?
{
public:
    Transmitter(
        int selector_index,
        QString name,
        int power_dBm,
        QPointF coordinates // QPoint ? // TODO: change to QVector2D ?
        );
    int getPower_dBm() const;
    qreal getPower() const;
    qreal getGain() const;
    void setPower_dBm(int power_dBm);
    QPointF getCoordinates() const;
    void changeCoordinates(QPointF new_coordinates);

    QVector2D get2DVector() const;

    QGraphicsEllipseItem* graphics = new QGraphicsEllipseItem();
private:
    int selector_index;
    QString name;
    int power_dBm;
    qreal gain;
    QPointF coordinates; // TODO: change to QVector2D ?
    qulonglong frequency = 60*1e9; // 802.11ay spec. Compiler complains with 60e9
    qulonglong* f = &frequency;
    // qreal beta_0 = ...;
    // qreal h_e = ...;
};

#endif // TRANSMITTER_H
