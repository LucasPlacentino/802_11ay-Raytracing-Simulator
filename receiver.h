#ifndef RECEIVER_H
#define RECEIVER_H

#include <QGraphicsRectItem>

class Receiver : public QGraphicsRectItem // each cell of the simulation grid acts like a receiver
{
public:
    Receiver(int power_dBm);

    int getPower_dBm();
    qulonglong getBitrate();
    QColor getCellColor();

private:
    int power_dBm;
    qulonglong bitrate;
    QColor cell_color;
};

#endif // RECEIVER_H
