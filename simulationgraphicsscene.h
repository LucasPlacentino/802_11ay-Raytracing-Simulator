#ifndef SIMULATIONGRAPHICSSCENE_H
#define SIMULATIONGRAPHICSSCENE_H

#include <QGraphicsScene>

class SimulationGraphicsScene : public QGraphicsScene
{
public:
    //explicit SimulationGraphicsScene(QObject *parent = nullptr);
    SimulationGraphicsScene();

    void drawScene();

private:
    void drawRays(); // or drawRay() ?
    void drawTransmitter();
    void drawWalls(); // or drawWall() ?
    //void drawGrid(); // ?

    QLinearGradient* gradient; // power color gradient

};

#endif // SIMULATIONGRAPHICSSCENE_H
