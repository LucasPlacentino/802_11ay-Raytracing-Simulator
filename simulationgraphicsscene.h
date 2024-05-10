// #ifndef SIMULATIONGRAPHICSSCENE_H
// #define SIMULATIONGRAPHICSSCENE_H

// #include "heatmapgradientlegend.h"

// #include <QGraphicsScene>

// class SimulationGraphicsScene : public QGraphicsScene
// {
// public:
//     SimulationGraphicsScene(QObject *parent = nullptr);
//     //SimulationGraphicsScene();

//     void drawScene();

// private:
//     void drawRays(); // or drawRay() ?
//     void drawTransmitters(); // Base Station(s)
//     void drawWalls(); // or drawWall() ?
//     void drawCells(); // "Receivers"
//     //void drawGrid(); // ?

//     void drawGradientLegend();
//     HeatmapGradientLegend *gradient_legend = new HeatmapGradientLegend();
//     //QLinearGradient* gradient; // power color gradient
//     // gradient is a QColor::fromHsl([value between 0 and 240, 0 being red and 240 darkblue], 255, 128 or 92)

// };

// #endif // SIMULATIONGRAPHICSSCENE_H