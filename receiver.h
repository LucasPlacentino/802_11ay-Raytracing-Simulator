#ifndef RECEIVER_H
#define RECEIVER_H

#include <QGraphicsRectItem>

class Receiver : public QGraphicsRectItem // each cell of the simulation grid acts like a receiver
{
public:
    Receiver(double power_dBm);

    double getPower_dBm();
    qulonglong getBitrate();
    QColor getCellColor();

private:
    double power_dBm;
    qulonglong bitrate;
    QColor cell_color;
};

#endif // RECEIVER_H
