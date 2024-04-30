#include "simulation.h"
#include "utils.h"
//#include "parameters.h"

#include <QtMath>

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
    this->timer.start();
    qDebug() << "Simulation::run() - single cell simulation: " << (this->showRaySingleCell); // still TODO: single cell simulation

    this->cells_matrix.clear();
    this->rays_list.clear();
    this->obstacles.clear();

    if (this->lift_is_on_floor) { // Adds the lift metal walls if set as present
        qDebug() << "Lift is on this floor.";
        std::vector<Obstacle*> lift_walls;
        lift_walls.push_back(new Obstacle(QPointF(),QPointF(), MetalWall, 5));
        lift_walls.push_back(new Obstacle(QPointF(),QPointF(), MetalWall, 5));
        lift_walls.push_back(new Obstacle(QPointF(),QPointF(), MetalWall, 5));
        lift_walls.push_back(new Obstacle(QPointF(),QPointF(), MetalWall, 5));

        this->obstacles.insert(this->obstacles.end(), lift_walls.begin(), lift_walls.end());
    }

    if (!this->showRaySingleCell) {
        createCellsMatrix();
    } else {
        // single cell simulation
        // TODO:
        return;
    }

    // ...
    /*
     * loop over each cell of cells_matrix,
     * trace rays recursively for each reflection,
     * do the same with multiple transmitters and only keep the strongest transmitter for this cell,
     * compute final power for this cell
    */
    if (this->cells_matrix.isEmpty()) {
        qWarning("no cells provided (simulation.cells_matrix is empty)");
        throw std::exception();
    }
    QPair<int, int> matrix_size = {this->cells_matrix[0].size(), this->cells_matrix.size()};
    for (int l = 0; l < matrix_size.first; l++) {
        // loops over each line
        qDebug() << "Cells line " << l;

        for (int c = 0; c < matrix_size.second; c++) {
            // loops over each column of each line
            qDebug() << "Cell column " << c;

            for (int reflections = 0; reflections <= this->max_ray_reflections; reflections++) {
                qDebug() << "Ray(s) with " << reflections << " reflections";
                traceRaysToCell(QPair<int,int>(l,c),reflections);
            }

            // TODO: add power of every ray to this cell.
        }
    }

    //end
    this->simulation_time = this->timer.elapsed();
    qDebug() << "Simulation time:" << this->simulation_time << "ms";
}

void Simulation::traceRaysToCell(QPair<int,int> cell_index,int num_reflections)
{
    qDebug() << "Tracing Ray(s) with" << num_reflections << "reflections to cell ("<<cell_index.first<<","<<cell_index.second<<")";
    // ! TODO: but we can have multiple possible rays for a set number of reflections ?
    // TODO: find how many rays we can compute for num_reflections
    traceRay(QSharedPointer<Ray>(new Ray(getBaseStation(0)->getCoordinates(), this->cells_matrix[cell_index.second][cell_index.first])), num_reflections); // Careful about memory leak if using raw pointer
}

void Simulation::traceRay(QSharedPointer<Ray> ray, int reflections)
{
    qDebug() << "Tracing ray";
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

void Simulation::createCellsMatrix()
{
    qDebug() << "cells_matrix initial size:" << this->cells_matrix.size();
    int max_x_count = ceil(max_x/this->resolution); // -1 ?
    qDebug() << "Max count of cells X:" << max_x_count;
    int max_y_count = ceil(-min_y/this->resolution); // -1 ?
    qDebug() << "Max count of cells Y:" << max_y_count;
    for (int x_count=0; x_count < max_x_count; x_count++) {
        qDebug() << "Creating new line of cells_matrix...";
        qreal x = 0+(this->resolution*x_count);
        QList<QSharedPointer<Receiver>> temp_list;
        for (int y_count=0; y_count < max_y_count; y_count++) {
            qreal y = 0-(this->resolution*y_count);
            temp_list.push_back(QSharedPointer<Receiver>(new Receiver(0,QPointF(x,y))));
            qDebug() << "cells_matrix line"<< x_count << "size:" << temp_list.size();
        }
        this->cells_matrix.push_back(temp_list);
        qDebug() << "cells_matrix size:" << this->cells_matrix.size();
    }
    qDebug() << "cells_matrix created";
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

void Simulation::computeCell(QSharedPointer<Receiver> cell)
{
    // TODO: recursive function for each ray bounce from the transmitter

    qreal Ra = 1; // ????
    qulonglong rx_power = ((60*pow(wavelength,2))/(8*M_PI*Ra))*this->getBaseStation(0)->getPower()*this->getBaseStation(0)->getGain();
    /*
    for (qreal Gamma; todo) {
        rx_power *= pow(qAbs(Gamma),2);
    }
    for (qreal T; todo) {
        rx_power *= pow(qAbs(T),2);
    }
    */
}

qint64 Simulation::getSimulationTime() const
{
    return this->simulation_time;
}

void Simulation::test()
{
    // math needed :
    QVector2D p; // intersection between ray and wall, reflection point

    QVector2D wall_start = QVector2D(2,-3); // r_0 // used so it does not need to be at (0,0)
    QVector2D wall_end = QVector2D(0,5); // r_1
    QVector2D u = (wall_end-wall_start).normalized(); // normalized tangent vector to this wall
    qDebug() << "u norm =" << u.length();

    bool dir = true; // TODO: determine
    QVector2D n = transpose2DVector(&u, dir).normalized(); // normalized normal vector to this wall
    qDebug() << "n norm =" << n.length();

    qDebug() << "Check <u,n>?=0 :" << QVector2D::dotProduct(u,n);

    QVector2D rx = QVector2D(4,0.5); // r_RX
    QVector2D tx = QVector2D(5,3.5); // r_TX or s

    if (!checkSameSideOfWall(n,rx,tx)) {
        // has to be transmission through this wall then.
        // p point will need to be determined from the next point (other p, or rx if no more reflection or transmission)
        return;
    } else {
        QVector2D tx_image = rx - 2*n*QVector2D::dotProduct(tx,n); // image of tx with this wall

        QVector2D d = rx-tx_image; // distance vector (between the tx image and rx but its norm is also the distance the ray has travelled
        float ray_length = d.length(); // used ?
        float dx = d.x();
        float dy = d.y();

        float t = (dy*(wall_start.x()*rx.x())-dx*(wall_start.y()-rx.y()))/(dx*u.y()-dy*u.x());
        p = wall_start + t*u; // the intersection point P vector
    }

    // do it again for another wall (next reflection) ?
    // tx_image will be the new tx, and we use a new wall
    // we need to check if the new wall can in fact reflect from the new tx
    // => check if sign(dotProduct(n, rx))==sign(dotProduct(n, tx))
}

QGraphicsScene *Simulation::createGraphicsScene(std::vector<Transmitter>* TX)
{
    return new QGraphicsScene;
}
