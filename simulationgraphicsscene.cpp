#include "simulation.h"
#include "simulationgraphicsscene.h"

extern Simulation simulation;

SimulationGraphicsScene::SimulationGraphicsScene(QObject *parent)
    : QGraphicsScene{parent}
{
    //this->addItem(this->gradient_legend);
}

void SimulationGraphicsScene::drawRays()
{

}

void SimulationGraphicsScene::drawTransmitter()
{

}

void SimulationGraphicsScene::drawGradientLegend()
{
    this->addItem(this->gradient_legend);
}

/*
SimulationGraphicsScene::SimulationGraphicsScene()
{

}
*/

void SimulationGraphicsScene::drawWalls()
{
    // TODO: get floorplan
    std::vector<Obstacle>* walls = simulation.getObstacles();

    for (auto it = walls->begin(); it != walls->end(); it++)
    {
        // iterates through each obstacle

        // draw
        this->addItem(&(*it));
    }
}

