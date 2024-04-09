#include "simulation.h"

Simulation::Simulation() {
    // constructor

    //this->baseStations = std::vector<Transmitter>();
    ////this->simulation_scene; // not needed, will be default-initialized when Simulation is initialized
    //this->scene = scene;


    // Add obstacles:
    std::vector<Obstacle> concrete_walls;
    concrete_walls.push_back(Obstacle(QPointF(), QPointF(), ConcreteWall, 30));
    concrete_walls.push_back(Obstacle(QPointF(), QPointF(), ConcreteWall, 30));
    concrete_walls.push_back(Obstacle(QPointF(), QPointF(), ConcreteWall, 30));
    concrete_walls.push_back(Obstacle(QPointF(), QPointF(), ConcreteWall, 30));
    concrete_walls.push_back(Obstacle(QPointF(), QPointF(), ConcreteWall, 30));
    concrete_walls.push_back(Obstacle(QPointF(), QPointF(), ConcreteWall, 30));
    concrete_walls.push_back(Obstacle(QPointF(), QPointF(), ConcreteWall, 30));
    concrete_walls.push_back(Obstacle(QPointF(), QPointF(), ConcreteWall, 30));
    concrete_walls.push_back(Obstacle(QPointF(), QPointF(), ConcreteWall, 30));

    std::vector<Obstacle> drywall_walls;
    drywall_walls.push_back(Obstacle(QPointF(), QPointF(), DryWall, 10));
    drywall_walls.push_back(Obstacle(QPointF(), QPointF(), DryWall, 10));
    drywall_walls.push_back(Obstacle(QPointF(), QPointF(), DryWall, 10));
    drywall_walls.push_back(Obstacle(QPointF(), QPointF(), DryWall, 10));
    drywall_walls.push_back(Obstacle(QPointF(), QPointF(), DryWall, 10));
    drywall_walls.push_back(Obstacle(QPointF(), QPointF(), DryWall, 10));

    Obstacle glass_window = Obstacle(QPointF(),QPointF(), Window, 5); // this one is diagonal

    Obstacle metal_lift_door = Obstacle(QPointF(),QPointF(), MetalWall, 5);

    // /!\ The lift is only added to the obstacles if enabled
    this->obstacles.insert(this->obstacles.end(), concrete_walls.begin(), concrete_walls.end());
    this->obstacles.insert(this->obstacles.end(), drywall_walls.begin(), drywall_walls.end());
    this->obstacles.insert(this->obstacles.end(), glass_window);
    this->obstacles.insert(this->obstacles.end(), metal_lift_door);
}

void Simulation::run()
{
    // TODO: compute everything

    if (this->lift_is_on_floor) { // Adds the lift metal walls if set as present
        std::vector<Obstacle> lift_walls;
        lift_walls.push_back(Obstacle(QPointF(),QPointF(), MetalWall, 5));
        lift_walls.push_back(Obstacle(QPointF(),QPointF(), MetalWall, 5));
        lift_walls.push_back(Obstacle(QPointF(),QPointF(), MetalWall, 5));
        lift_walls.push_back(Obstacle(QPointF(),QPointF(), MetalWall, 5));

        this->obstacles.insert(this->obstacles.end(), lift_walls.begin(), lift_walls.end());
    }


    // ...
    /*
     * loop over each cell of cells_matrix,
     * trace rays recursively for each reflection,
     * do the same with multiple transmitters and only keep the strongest transmitter for this cell,
     * compute final power for this cell
    */

    QPair<int, int> matrix_size = {this->cells_matrix[0].size(), this->cells_matrix.size()};
    for (int l = 0; l < matrix_size.first; l++) {
        // loops over each line
        qDebug() << "Cells line " << l;

        for (int c = 0; c < matrix_size.second; c++) {
            // loops over each column of each line
            qDebug() << "Cell column " << c;

            for (int reflections = 0; reflections < this->max_ray_reflections; reflections++) {
                qDebug() << "Ray(s) with " << reflections << " reflections";
                traceRays(reflections);
            }

            // add power of every ray to this cell.
        }
    }

}

void Simulation::traceRays(int num_reflections)
{
    // ! TODO: but we can have multiple possible rays for a set number of reflections ?
    // TODO: find how many rays we can compute for num_reflections
    traceRay(Ray(getBaseStation(0)->getCoordinates(), this->cells_matrix), num_reflections);
}

void Simulation::traceRay(Ray* ray, int reflections)
{
    if (ray->num_reflections < reflections) {
        QPointF intersection_point;
        //... compute

        ray->points.push_back(intersection_point);

        // recursive reflections
        //traceRay(ray, num_reflections);
    } else {

        free(ray);
        return;
    }
}

void Simulation::resetAll()
{
    // TODO: reset all simualtion parameters
    // ? needed ?
}

Transmitter* Simulation::getBaseStation(int index)
{
    if (index < 0 || index >= baseStations.size()) {
        qWarning("baseStations index out of range");
        throw std::out_of_range("baseStations index out of range");
    }
    return &this->baseStations.at(index);
}

void Simulation::createBaseStation(Transmitter transmitter)
{
    this->baseStations.push_back(transmitter);
}

void Simulation::deleteBaseStation(int index)
{
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

unsigned int Simulation::getNumberOfObstacles()
{
    return this->obstacles.size();
    // return std::size(this->obstacles);
}

int Simulation::getNumberOfBaseStations()
{
    return this->baseStations.size();
}

void Simulation::computeCell(Receiver cell)
{
    // TODO: recursive function for each ray bounce from the transmitter

}

