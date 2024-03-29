#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <QGraphicsEllipseItem>


class Transmitter//: QGraphicsEllipseItem // QGraphicsEllipseItem?
{
public:
    Transmitter(
        int selector_index,
        QString name,
        int power_dBm
        );
    int getPower_dBm();
    void setPower_dBm(int power_dBm);
private:
    int selector_index;
    QString name;
    int power_dBm;
};

#endif // TRANSMITTER_H
