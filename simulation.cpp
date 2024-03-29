#include "simulation.h"

Simulation::Simulation() {
    // constructor

    this->number_of_obstacles = std::size(this->obstacles);
    //this->baseStations = std::vector<Transmitter>();
}

// destructor
void Simulation::resetAll() {
    // TODO: reset all simualtion parameters

}

Transmitter Simulation::getBaseStation(int index) {
    if (index < 0 || index >= baseStations.size()) {
        qWarning("baseStations index out of range");
        throw std::out_of_range("baseStations index out of range");
    }
    return this->baseStations.at(index);
}

void Simulation::createBaseStation(Transmitter transmitter) {
    this->baseStations.push_back(transmitter);
}
