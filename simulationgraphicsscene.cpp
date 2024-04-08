#include "simulationgraphicsscene.h"


SimulationGraphicsScene::SimulationGraphicsScene(QObject *parent)
    : QGraphicsScene{parent}
{
    //this->addItem(this->gradient_legend);
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
