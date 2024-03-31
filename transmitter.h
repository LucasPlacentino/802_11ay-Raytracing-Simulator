#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <QGraphicsEllipseItem>


class Transmitter//: QGraphicsEllipseItem // QGraphicsEllipseItem?
{
public:
    Transmitter(
        int selector_index,
        QString name,
        int power_dBm,
        QPoint coordinates // QPoint ?
        );
    int getPower_dBm();
    double getPower();
    void setPower_dBm(int power_dBm);
    QPoint getCoordinates();
    void changeCoordinates(QPoint new_coordinates);
private:
    int selector_index;
    QString name;
    int power_dBm;
    QPoint coordinates;
    qulonglong frequency = 60*1e9; // 802.11ay spec. Compiler complains with 60e9
    qulonglong* f = &frequency;
};

#endif // TRANSMITTER_H
