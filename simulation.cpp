#include "simulation.h"

Simulation::Simulation() {
    // constructor

    this->number_of_obstacles = std::size(this->obstacles);
    //this->baseStations = std::vector<Transmitter>();
    ////this->simulation_scene; // not needed, will be default-initialized when Simulation is initialized
    //this->scene = scene;
}

void Simulation::run()
{
    // TODO: compute everything


}

void Simulation::resetAll() {
    // TODO: reset all simualtion parameters

}

Transmitter* Simulation::getBaseStation(int index) {
    if (index < 0 || index >= baseStations.size()) {
        qWarning("baseStations index out of range");
        throw std::out_of_range("baseStations index out of range");
    }
    return &this->baseStations.at(index);
}

void Simulation::createBaseStation(Transmitter transmitter) {
    this->baseStations.push_back(transmitter);
}

void Simulation::deleteBaseStation(int index) {
    if (index > 0 || index < baseStations.size())
    {
        this->baseStations.erase(baseStations.begin()+index);
    } else if (index == 0)
    {
        qDebug("Cannot delete Base Station 1");
    } else {
        qWarning("deleteBaseStation error: index out of range");
        throw std::out_of_range("deleteBaseStation error: index out of range");
    }
}

std::vector<Obstacle>* Simulation::getObstacles()
{
    return &this->obstacles;
}

