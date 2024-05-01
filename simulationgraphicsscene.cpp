// #include "simulation.h"
// #include "simulationgraphicsscene.h"

// extern Simulation simulation;

// SimulationGraphicsScene::SimulationGraphicsScene(QObject *parent)
//     : QGraphicsScene{parent}
// {
//     //this->addItem(this->gradient_legend);
// }

// void SimulationGraphicsScene::drawRays()
// {
//     // Only when doing the simulation for rays to single cell

// }

// void SimulationGraphicsScene::drawTransmitters()
// {
//     for (int i = 0; i != simulation.getNumberOfBaseStations(); i++)
//     {
//         qDebug() << "Drawing base station " << i;
//         Transmitter* base_station = simulation.getBaseStation(i);
//         this->addItem(base_station->graphics);
//     }
// }

// void SimulationGraphicsScene::drawCells()
// {
//     // TODO: iterate over whole grid and draw each cell with its cell_color

// }

// void SimulationGraphicsScene::drawGradientLegend()
// {
//     this->addItem(this->gradient_legend);
// }

// /*
// SimulationGraphicsScene::SimulationGraphicsScene()
// {

// }
// */

// void SimulationGraphicsScene::drawWalls()
// {
//     // TODO: get floorplan
//     std::vector<Obstacle*>* walls = simulation.getObstacles();

//     for (auto it = walls->begin(); it != walls->end(); it++)
//     {
//         qDebug("Drawing wall");
//         // iterates through each obstacle

//         // draw
//         this->addItem(*it);
//     }
// }

