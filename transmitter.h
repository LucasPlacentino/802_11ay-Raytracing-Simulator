#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <QGraphicsEllipseItem>


class Transmitter// : public QGraphicsEllipseItem // QGraphicsEllipseItem?
{
public:
    Transmitter(
        int selector_index,
        QString name,
        int power_dBm,
        QPointF coordinates // QPoint ?
        );
    int getPower_dBm();
    double getPower();
    void setPower_dBm(int power_dBm);
    QPointF getCoordinates();
    void changeCoordinates(QPointF new_coordinates);
private:
    int selector_index;
    QString name;
    int power_dBm;
    QPointF coordinates;
    qulonglong frequency = 60*1e9; // 802.11ay spec. Compiler complains with 60e9
    qulonglong* f = &frequency;
    // qreal beta_0 = ...;
    // qreal h_e = ...;
};

#endif // TRANSMITTER_H
