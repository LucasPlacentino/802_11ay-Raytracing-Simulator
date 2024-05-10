#include "simulation.h"

Simulation::Simulation() {
    // constructor

    //this->baseStations = std::vector<Transmitter>();
    ////this->simulation_scene; // not needed, will be default-initialized when Simulation is initialized
    //this->scene = scene;

    // Delete old obstacles vector objects (avoid memory leak)
    if(!this->obstacles.empty()) {
        // The vector is not empty
        for(auto ptr : this->obstacles) {
            delete ptr;
        }
        this->obstacles.clear();
    }

    // Add obstacles:
    std::vector<Obstacle*> concrete_walls;
    concrete_walls.push_back(new Obstacle(QPointF(), QPointF(), ConcreteWall, 30));
    concrete_walls.push_back(new Obstacle(QPointF(), QPointF(), ConcreteWall, 30));
    concrete_walls.push_back(new Obstacle(QPointF(), QPointF(), ConcreteWall, 30));
    concrete_walls.push_back(new Obstacle(QPointF(), QPointF(), ConcreteWall, 30));
    concrete_walls.push_back(new Obstacle(QPointF(), QPointF(), ConcreteWall, 30));
    concrete_walls.push_back(new Obstacle(QPointF(), QPointF(), ConcreteWall, 30));
    concrete_walls.push_back(new Obstacle(QPointF(), QPointF(), ConcreteWall, 30));
    concrete_walls.push_back(new Obstacle(QPointF(), QPointF(), ConcreteWall, 30));
    concrete_walls.push_back(new Obstacle(QPointF(), QPointF(), ConcreteWall, 30));

    std::vector<Obstacle*> drywall_walls;
    drywall_walls.push_back(new Obstacle(QPointF(), QPointF(), DryWall, 10));
    drywall_walls.push_back(new Obstacle(QPointF(), QPointF(), DryWall, 10));
    drywall_walls.push_back(new Obstacle(QPointF(), QPointF(), DryWall, 10));
    drywall_walls.push_back(new Obstacle(QPointF(), QPointF(), DryWall, 10));
    drywall_walls.push_back(new Obstacle(QPointF(), QPointF(), DryWall, 10));
    drywall_walls.push_back(new Obstacle(QPointF(), QPointF(), DryWall, 10));

    Obstacle* glass_window = new Obstacle(QPointF(),QPointF(), Window, 5); // this one is diagonal

    Obstacle* metal_lift_door = new Obstacle(QPointF(),QPointF(), MetalWall, 5);

    // /!\ The lift is only added to the obstacles if enabled
    std::vector<Obstacle*> all_obstacles;
    all_obstacles.insert(all_obstacles.end(), concrete_walls.begin(), concrete_walls.end());
    all_obstacles.insert(all_obstacles.end(), drywall_walls.begin(), drywall_walls.end());
    all_obstacles.push_back(glass_window);
    all_obstacles.push_back(metal_lift_door);

    this->obstacles = all_obstacles;
}

void Simulation::run()
{
    // TODO: compute everything
    qDebug() << "Simulation::run() - single cell simulation: " << (this->showRaySingleCell);

    if (this->lift_is_on_floor) { // Adds the lift metal walls if set as present
        qDebug() << "Lift is on this floor.";
        std::vector<Obstacle*> lift_walls;
        lift_walls.push_back(new Obstacle(QPointF(),QPointF(), MetalWall, 5));
        lift_walls.push_back(new Obstacle(QPointF(),QPointF(), MetalWall, 5));
        lift_walls.push_back(new Obstacle(QPointF(),QPointF(), MetalWall, 5));
        lift_walls.push_back(new Obstacle(QPointF(),QPointF(), MetalWall, 5));

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
                traceRaysToCell(QPair<int,int>(l,c),reflections);
            }

            // TODO: add power of every ray to this cell.
        }
    }

}

void Simulation::traceRaysToCell(QPair<int,int> cell_index,int num_reflections)
{
    // ! TODO: but we can have multiple possible rays for a set number of reflections ?
    // TODO: find how many rays we can compute for num_reflections
    traceRay(QSharedPointer<Ray>(new Ray(getBaseStation(0)->getCoordinates(), this->cells_matrix[cell_index.first][cell_index.second])), num_reflections); // Careful about memory leak if using raw pointer
}

void Simulation::traceRay(QSharedPointer<Ray> ray, int reflections)
{
    if (ray->num_reflections < reflections) {
        QPointF intersection_point;
        //... compute

        // vector maths

        ray->addPoint(intersection_point);

        // TODO: create sub-ray ?

        // recursive reflections
        //traceRay(ray, num_reflections);
    } else {
        // This ray has reached its maximum number of reflections, its computation is finished.

        if (!this->showRaySingleCell) {
            // Do does not keep the ray once everything is computed, only for the coverage area heatmap simulation

            // if using smart pointer (like unique_ptr or shared_ptr, does not need to be manually freed)
            //delete ray; // frees the Ray object from memory once the computation is done with this ray.
        } else {
            // Keeps the rays so they can be drawn after being computed, only for the rays to single cell simulation

            this->rays_list.push_back(ray); // delete later if using raw pointers
        }
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

std::vector<Obstacle*>* Simulation::getObstacles()
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

