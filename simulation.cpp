#include "simulation.h"
#include "utils.h"
//#include "parameters.h"

#include <QtMath>

Simulation::Simulation() {
    // constructor

    //this->baseStations = std::vector<Transmitter>();
    ////this->simulation_scene; // not needed, will be default-initialized when Simulation is initialized
    //this->scene = scene;

    createWalls();
}

void Simulation::createWalls()
{
    qDebug("Creating walls...");

    // Delete old obstacles vector objects (avoid memory leak)
    if(!this->obstacles.empty()) {
        // The vector is not empty
        for(auto ptr : this->obstacles) {
            delete ptr;
        }
        this->obstacles.clear();
    }

    // Add obstacles:
    QList<Obstacle*> concrete_walls;
    concrete_walls.append(new Obstacle(QVector2D(), QVector2D(), ConcreteWall, 30));
    concrete_walls.append(new Obstacle(QVector2D(), QVector2D(), ConcreteWall, 30));
    concrete_walls.append(new Obstacle(QVector2D(), QVector2D(), ConcreteWall, 30));
    concrete_walls.append(new Obstacle(QVector2D(), QVector2D(), ConcreteWall, 30));
    concrete_walls.append(new Obstacle(QVector2D(), QVector2D(), ConcreteWall, 30));
    concrete_walls.append(new Obstacle(QVector2D(), QVector2D(), ConcreteWall, 30));
    concrete_walls.append(new Obstacle(QVector2D(), QVector2D(), ConcreteWall, 30));
    concrete_walls.append(new Obstacle(QVector2D(), QVector2D(), ConcreteWall, 30));
    concrete_walls.append(new Obstacle(QVector2D(), QVector2D(), ConcreteWall, 30));

    QList<Obstacle*> drywall_walls;
    drywall_walls.append(new Obstacle(QVector2D(), QVector2D(), DryWall, 10));
    drywall_walls.append(new Obstacle(QVector2D(), QVector2D(), DryWall, 10));
    drywall_walls.append(new Obstacle(QVector2D(), QVector2D(), DryWall, 10));
    drywall_walls.append(new Obstacle(QVector2D(), QVector2D(), DryWall, 10));
    drywall_walls.append(new Obstacle(QVector2D(), QVector2D(), DryWall, 10));
    drywall_walls.append(new Obstacle(QVector2D(), QVector2D(), DryWall, 10));

    Obstacle* glass_window = new Obstacle(QVector2D(),QVector2D(), Window, 5); // this one is diagonal

    Obstacle* metal_lift_door = new Obstacle(QVector2D(),QVector2D(), MetalWall, 5);

    // /!\ The lift is only added to the obstacles if enabled
    QList<Obstacle*> all_obstacles;
    all_obstacles.append(concrete_walls);
    //all_obstacles.insert(all_obstacles.end(), concrete_walls.begin(), concrete_walls.end());
    all_obstacles.append(drywall_walls);
    //all_obstacles.insert(all_obstacles.end(), drywall_walls.begin(), drywall_walls.end());
    all_obstacles.append(glass_window);
    all_obstacles.append(metal_lift_door); // last one is the metal lift door

    this->obstacles = all_obstacles;
}

void Simulation::run()
{
    // TODO: compute everything
    this->timer.start();
    qDebug() << "Simulation::run() - single cell simulation: " << (this->showRaySingleCell); // still TODO: single cell simulation

    //this->cells_matrix.clear();
    this->cells.clear();
    //this->rays_list.clear();
    this->obstacles.clear();

    createWalls();
    if (this->lift_is_on_floor) { // Adds the lift metal walls if set as present
        qDebug() << "Lift is on this floor.";
        // change the metal door of the lift to twice its thickness (because metal door + metal wall at same place)
        this->obstacles.last() = new Obstacle(QVector2D(),QVector2D(),MetalWall,10); // door+wall
        QList<Obstacle*> lift_walls;
        lift_walls.append(new Obstacle(QVector2D(),QVector2D(), MetalWall, 5));
        lift_walls.append(new Obstacle(QVector2D(),QVector2D(), MetalWall, 5));
        lift_walls.append(new Obstacle(QVector2D(),QVector2D(), MetalWall, 5));

        //this->obstacles.insert(this->obstacles.end(), lift_walls.begin(), lift_walls.end());
        this->obstacles.append(lift_walls);
    }

    if (!this->showRaySingleCell) {
        createCellsMatrix();
    } else {
        // single cell simulation
        // TODO:
        return;
    }

    Transmitter* base_station = this->baseStations[0];

    // ...
    /*
     * loop over each cell of cells_matrix,
     * trace rays recursively for each reflection,
     * do the same with multiple transmitters and only keep the strongest transmitter for this cell,
     * compute final power for this cell
    */
    //if (this->cells_matrix.isEmpty()) {
    if (this->cells.isEmpty()) {
        qWarning("no cells provided (simulation.cells matrix is empty)");
        throw std::exception();
    }

    //QPair<int, int> matrix_size = {this->cells_matrix[0].size(), this->cells_matrix.size()};
    //QPair<int, int> matrix_size = {this->cells[0].size(), this->cells.size()};
    for (QList<Receiver*> cells_line : this->cells){
    //for (int l = 0; l < matrix_size.first; l++) {
        // loops over each line
        //qDebug() << "Cells line " << l;

        for (Receiver* cell : cells_line) {
        //for (int c = 0; c < matrix_size.second; c++) {
            // loops over each column of each line
            //qDebug() << "Cell column " << c;

            // --- old ---
            //for (int reflections = 0; reflections <= this->max_ray_reflections; reflections++) {
            //    qDebug() << "Ray(s) with " << reflections << " reflections";
            //    //traceRaysToCell(QPair<int,int>(l,c),reflections);
            //}
            //  add power of every ray to this cell.
            // -----------

            // TODO : multiple transmitters ?
            computeDirect(cell, *base_station);
            computeReflections(cell, *base_station);
        }
    }

    //end of simulation
    this->simulation_time = this->timer.elapsed();
    qDebug() << "Simulation time:" << this->simulation_time << "ms";

    showView();
}

void Simulation::computeReflections(Receiver* _RX, const QVector2D& _TX)
{
    // Makes the whole reflections computation, summary:
    // For each wall, check if TX (or its image, for the 2nd reflection) and RX are on the same
    // side of the wall, if so computes TX's (or its image's) image with this wall, then computes
    // the reflection point which is the intersection of the line between the image and RX and the
    // wall. Creates the segments from TX to each reflection point to finally RX and creates a new
    // Ray object made of these segments.

    // 1st reflection :
    //for (Wall* wall: wall_list) { // could use this instead
    for (int i=0; i<this->obstacles.length(); i++) {
        Obstacle* wall = this->obstacles[i];

        // check if same side of wall, if false, then no reflection only transmission
        if (checkSameSideOfWall(wall->normal,_TX,_RX)) {
            //same side of this wall, can make a reflection
            qDebug() << "Same side of wall TX and RX:" << wall << _TX.toPointF() << _RX->toPointF() ;
            Ray* ray_1_reflection = new Ray(_TX.toPointF(), _RX.toPointF());

            QVector2D _imageTX = computeImage(_TX, wall);
            qDebug() << "_image:" << _imageTX.x() << _imageTX.y();

            QVector2D _P_r = calculateReflectionPoint(_imageTX,_RX,wall);

            // CHECK IF REFLECTION IS ON THE WALL AND NOT ITS EXTENSION:
            RaySegment* test_segment = new RaySegment(_imageTX.x(),_imageTX.y(),_RX->x(),_RX->y());
            if (!checkRaySegmentIntersectsWall(wall, test_segment)) {
                // RAY DOES NOT TRULY INTERSECT THE WALL (only the wall extension) ignore this reflection at this wall
                qDebug() << "ignore";
                delete test_segment;
                continue; // break out of this forloop instance for this wall
            }
            delete test_segment;

            qDebug() << "P_r" << _P_r;
            //debug :
            //tx_images.append(new QGraphicsEllipseItem(_imageTX.x()-2, -_imageTX.y()-2, 4, 4));
            //reflection_points.append(new QGraphicsEllipseItem(_P_r.x()-1, -_P_r.y()-1, 2, 2));

            // create ray segments between points
            QList<RaySegment*> ray_segments;
            ray_segments.append(new RaySegment(_TX.x(),_TX.y(),_P_r.x(),_P_r.y())); // first segment
            ray_segments.append(new RaySegment(_P_r.x(),_P_r.y(),_RX->x(),_RX->y())); // last segment

            ray_1_reflection->segments = ray_segments;
            addReflection(ray_1_reflection,_imageTX,*_RX,wall);
            checkTransmissions(ray_1_reflection,{wall});

            qDebug() << "ray_1_refl distance:" << QVector2D(*_RX - _imageTX).length();
            ray_1_reflection->distance = QVector2D(*_RX-_imageTX).length();
            qDebug() << "Ray's (1refl) total coeffs:" << ray_1_reflection->getTotalCoeffs();
            _RX->all_rays.append(ray_1_reflection);

            // 2nd reflection
            for (Obstacle* wall_2 : this->obstacles) {
                // check that the second wall is not the same as the first wall and that imageTX and RX are at the same side of this second wall
                if (wall_2 != wall && checkSameSideOfWall(wall_2->normal,_imageTX,_RX)) {
                    qDebug() << "Same side of wall imageTX and RX --- wall_2:" << wall_2->line.p1() << wall_2->line.p2() << ", imageTX:" << _imageTX.toPointF() << ", RX:" << _RX->toPointF() ;
                    Ray* ray_2_reflection = new Ray(_TX.toPointF(),_RX->toPointF());

                    QVector2D _image_imageTX = computeImage(_imageTX,wall_2);
                    qDebug() << "_image_image:" << _image_imageTX.x() << _image_imageTX.y();

                    QVector2D _P_r_2_last = calculateReflectionPoint(_image_imageTX,_RX,wall_2);
                    QVector2D _P_r_2_first = calculateReflectionPoint(_imageTX,_P_r_2_last,wall);
                    if (_P_r_2_last.x()==_P_r_2_first.x() && _P_r_2_last.y()==_P_r_2_first.y()) {
                        qDebug() << "------> P_r_2_last = P_r_2_first !!!)";
                    }

                    RaySegment* test_segment_1 = new RaySegment(_image_imageTX.x(),_image_imageTX.y(),_RX->x(),_RX->y());
                    RaySegment* test_segment_2 = new RaySegment(_imageTX.x(),_imageTX.y(),_P_r_2_last.x(),_P_r_2_last.y());
                    if (!checkRaySegmentIntersectsWall(wall_2, test_segment_1) && !checkRaySegmentIntersectsWall(wall,test_segment_2)) {
                        qDebug() << "ignore";
                        // RAY DOES NOT TRULY INTERSECT THE WALL (only the wall extension) ignore this reflection at this wall
                        delete test_segment_1;
                        delete test_segment_2;
                        continue; // break out of this forloop instance for this wall
                    }
                    delete test_segment_1;
                    delete test_segment_2;

                    qDebug() << "P_r_2_first" << _P_r_2_first;
                    qDebug() << "P_r_2_last" << _P_r_2_last;
                    //debug :
                    //tx_images.append(new QGraphicsEllipseItem(_image_imageTX.x()-2, -_image_imageTX.y()-2, 4, 4));
                    //reflection_points.append(new QGraphicsEllipseItem(_P_r_2_last.x()-1, -_P_r_2_last.y()-1, 2, 2));
                    //reflection_points.append(new QGraphicsEllipseItem(_P_r_2_first.x()-1, -_P_r_2_first.y()-1, 2, 2));

                    QList<RaySegment*> ray_segments_2;
                    ray_segments_2.append(new RaySegment(_TX.x(),_TX.y(),_P_r_2_first.x(),_P_r_2_first.y()));
                    ray_segments_2.append(new RaySegment(_P_r_2_first.x(),_P_r_2_first.y(),_P_r_2_last.x(),_P_r_2_last.y()));
                    ray_segments_2.append(new RaySegment(_P_r_2_last.x(),_P_r_2_last.y(),_RX->x(),_RX->y()));

                    ray_2_reflection->segments = ray_segments_2;
                    addReflection(ray_2_reflection,_imageTX,_P_r_2_last,wall);
                    addReflection(ray_2_reflection,_image_imageTX,*_RX,wall_2);
                    checkTransmissions(ray_2_reflection,{wall,wall_2});

                    qDebug() << "ray_2_refl distance:" << QVector2D(*_RX - _image_imageTX).length();
                    ray_2_reflection->distance = QVector2D(*_RX-_image_imageTX).length();
                    qDebug() << "Ray's (2refl) total coeffs:" << ray_2_reflection->getTotalCoeffs();
                    _RX->all_rays.append(ray_2_reflection);
                }
            }
        }
    }
}

// fonction qui calcule la position de \vec r_image de l'antenne
//QPointF computeImageTX(const QPointF& TX, const QPointF& normal) {
QVector2D Simulation::computeImage(const QVector2D& _point, Obstacle* wall) {
    // returns the coordinates of _point's image with wall
    QVector2D new_origin = QVector2D(wall->line.p1()); // set origin to point1 of wall
    qDebug() << "new coords" << wall->line.p1();
    QVector2D _normal = wall->normal; // normal to the wall (is normalized so it is relative to any origin)
    qDebug() << "normal" << wall->normal;
    QVector2D new_point_coords = _point - new_origin; // initial point in new coordinates relative to point1 of wall
    double _dotProduct = QVector2D::dotProduct(new_point_coords, _normal);
    QVector2D _image_new_coords = new_point_coords - 2 * _dotProduct * _normal; // image point in new coordinates relative to point1 of wall
    QVector2D _image = new_origin + _image_new_coords; // image point in absolute coordinates
    //qDebug() << "image:" << _image.x() << _image.y();

    return _image;
}

// calculer le point de réflexion sur un mur, même chose que dans le tp
QVector2D Simulation::calculateReflectionPoint(const QVector2D& _start, const QVector2D& _end, Obstacle* wall) {
    // returns the intersection bewteen the line from _start to _end and the wall
    QVector2D d = _end-_start;
    //QVector2D x0(0,0); // TODO: always this ?
    QVector2D x0 = QVector2D(wall->line.p1()); // TODO: FIXME: correct ?
    qreal t = ((d.y()*(_start.x()-x0.x()))-(d.x()*(_start.y()-x0.y()))) / (wall->unitary.x()*d.y()-wall->unitary.y()*d.x());
    QVector2D P_r = x0 + t * wall->unitary;
    return P_r;
}

void Simulation::addReflection(Ray* _ray, const QVector2D& _p1, const QVector2D& _p2, Obstacle* wall){
    // computes the final |Gamma| coeff for the ray_segment's reflection with this wall, and adds it to this ray's coeffs list
    QVector2D _d = _p2-_p1;
    qreal _cos_theta_i = abs(QVector2D::dotProduct(_d.normalized(),wall->normal));
    qreal _sin_theta_i = abs(QVector2D::dotProduct(_d.normalized(),wall->unitary)); // sqrt(1 - pow(_cos_theta_i,2))
    qreal _sin_theta_t = _sin_theta_i / sqrt(wall->properties.relative_permittivity);
    qreal _cos_theta_t = sqrt(1 - pow(_sin_theta_t,2));
    qreal Gamma_coeff = abs(computeReflectionCoeff(_cos_theta_i,_sin_theta_i,_cos_theta_t,_sin_theta_i, wall));
    qDebug() << "addReflection, Gamma_coeff:" << Gamma_coeff;
    _ray->addCoeff(Gamma_coeff);
}

qreal Simulation::makeTransmission(RaySegment* ray_segment, Obstacle* wall) {
    // computes the final |T| coeff for the ray_segment's transmission with this wall
    QVector2D _eta = QVector2D(ray_segment->p1())-QVector2D(ray_segment->p2());
    qreal _cos_theta_i = abs(QVector2D::dotProduct(_eta.normalized(),wall->unitary));
    qreal _sin_theta_i = abs(QVector2D::dotProduct(_eta.normalized(),wall->normal));
    qreal _sin_theta_t = _sin_theta_i / sqrt(wall->properties.relative_permittivity);
    qreal _cos_theta_t = sqrt(1 - pow(_sin_theta_t,2));
    //qreal s = wall->thickness/_cos_theta_t;
    qreal T_coeff = abs(computeTransmissionCoeff(_cos_theta_i,_sin_theta_i,_cos_theta_t,_sin_theta_t,wall));
    return T_coeff;
}
void Simulation::checkTransmissions(Ray* _ray, QList<Obstacle*> _reflection_walls) {
    // checks for every segment in this ray if they intersect a wall (which isn't a wall already used for a reflection by this ray)
    // if so: adds the Transmission coefficient to this ray's coeffs list
    for (RaySegment* ray_segment : _ray->segments) {
        for (Obstacle* wall : this->obstacles) {
            //qDebug() << "pwall" << &wall;
            if (!_reflection_walls.contains(wall)) { // is NOT reflection wall
                if (checkRaySegmentIntersectsWall(wall, ray_segment,nullptr)) {
                    ////QVector2D _eta = QVector2D(ray_segment->p1())-QVector2D(ray_segment->p2());
                    ////qreal _cos_theta_i = abs(QVector2D::dotProduct(_eta.normalized(),wall->unitary));
                    ////qreal _sin_theta_i = abs(QVector2D::dotProduct(_eta.normalized(),wall->normal));
                    ////qreal _sin_theta_t = _sin_theta_i / sqrt(wall->epsilon_r);
                    ////qreal _cos_theta_t = sqrt(1 - pow(_sin_theta_t,2));
                    //////qreal s = wall->thickness/_cos_theta_t;
                    ////qreal T_coeff = abs(computeTransmissionCoeff(_cos_theta_i,_sin_theta_i,_cos_theta_t,_sin_theta_t,wall));
                    qreal T_coeff = abs(makeTransmission(ray_segment,wall));
                    qDebug() << "checkTransmission, T_coeff:" << T_coeff;
                    _ray->addCoeff(T_coeff);
                }
            }
        }
    }
}

void Simulation::computeDirect(Receiver* _RX, const QVector2D& _TX)
{
    // Computes the direct ray: checks all walls between RX and TX and adds
    // their computed transmission coefficients to the direct ray list of coeffs
    Ray* direct_ray = new Ray(_TX.toPointF(), _RX->toPointF());
    RaySegment* _direct_line = new RaySegment(_RX->x(), _RX->y(), _TX.x(), _TX.y());
    for (Obstacle* wall : this->obstacles) {
        //for (int i=0; i<wall_list.length(); i++) {
        //Wall* wall = wall_list[i];
        QPointF* intersection_point = nullptr; // not used
        if (checkRaySegmentIntersectsWall(wall, _direct_line, intersection_point)) {
            // transmission through this wall, compute the transmission coeff
            //qreal T_coeff = computeTransmissionCoeff();
            // add coeff to power computing
            //addCoeff(T_coeff);

            qreal T_coeff_module = abs(makeTransmission(_direct_line,wall));
            direct_ray->addCoeff(T_coeff_module);
        } else {
            continue;
        }
    }
    qDebug() << "ray_direct distance:" << QVector2D(*_RX - _TX).length();
    direct_ray->distance = QVector2D(*_RX-_TX).length();
    qDebug() << "Ray's (direct) total coeffs:" << direct_ray->getTotalCoeffs();
    direct_ray->segments = {_direct_line};
    _RX->all_rays.append(direct_ray);
}

complex<qreal> Simulation::computePerpendicularGamma(qreal _cos_theta_i, qreal _cos_theta_t, Obstacle* wall)
{
    // returns Gamma_perpendicular
    complex<qreal> Gamma_perp = (wall->properties.Z_m*_cos_theta_i-Z_0*_cos_theta_t)/(wall->properties.Z_m*_cos_theta_i+Z_0*_cos_theta_t);

    qDebug() << "Gamma_perp=" << QString::number(Gamma_perp.real()) << "+j" << QString::number(Gamma_perp.imag());
    return Gamma_perp;
}

complex<qreal> Simulation::computeReflectionCoeff(qreal _cos_theta_i, qreal _sin_theta_i, qreal _cos_theta_t, qreal _sin_theta_t, Obstacle* wall)
{
    // returns the reflection coefficient Gamma_m
    qreal s = wall->thickness/_cos_theta_t;
    complex<qreal> Gamma_perpendicular = computePerpendicularGamma(_cos_theta_i, _cos_theta_t, wall);
    complex<qreal> reflection_term = exp(-2.0 * wall->properties.gamma_m * s) * exp(j * 2.0 * beta_0 * s * _sin_theta_t * _sin_theta_i);
    qDebug() << "reflection_term:" << QString::number(reflection_term.real()) << "+ j" << QString::number(reflection_term.imag());
    complex<qreal> Gamma_m = Gamma_perpendicular - (1.0 - pow((Gamma_perpendicular), 2)) * Gamma_perpendicular * reflection_term / (1.0 - pow((Gamma_perpendicular), 2) * reflection_term);
    qDebug() << "Gamma_m:" << QString::number(Gamma_m.real()) << "+ j" << QString::number(Gamma_m.imag());

    return Gamma_m;
}

complex<qreal> Simulation::computeTransmissionCoeff(qreal _cos_theta_i, qreal _sin_theta_i, qreal _cos_theta_t, qreal _sin_theta_t, Obstacle* wall)
{
    // returns the transmission coefficient T_m
    qreal s = wall->thickness/_cos_theta_t;
    complex<qreal> perpGamma = computePerpendicularGamma(_cos_theta_i, _cos_theta_t, wall);
    complex<qreal> T_m = ((1.0-pow(perpGamma,2))*exp(-(wall->properties.gamma_m)*s))/(1.0-(pow(perpGamma,2)*exp(-2.0*(wall->properties.gamma_m)*s)*exp(j*beta_0*2.0*s*_sin_theta_t*_sin_theta_i)));

    qDebug() << "TransmissionCoeff=" << QString::number(T_m.real()) << "+j" << QString::number(T_m.imag());
    return T_m;
}

bool Simulation::checkSameSideOfWall(const QVector2D& _normal, const QVector2D& _TX, const QVector2D& _RX) {
    // returns true if _TX and _RX are on the same side of the wall (using the wall's normal vector)
    // must be same sign to be true:
    bool res = (QVector2D::dotProduct(_normal,_RX)>0 == QVector2D::dotProduct(_normal,_TX)>0);
    return res;
}

bool Simulation::checkRaySegmentIntersectsWall(const Obstacle* wall, RaySegment* ray_segment, QPointF* intersection_point) {
    // returns true if ray_segment intersects wall
    // the intersection_point pointer's value is set wit hthe intersection point coordinates if they intersect
    int _intersection_type = ray_segment->intersects(wall->line, intersection_point); // also writes to intersection pointer the QPointF
    bool intersects_wall = _intersection_type==1 ? true: false; //0: no intersection (parallel), 1: intersects directly the line segment, 2: intersects the infinite extension of the line
    return intersects_wall;
}

void Simulation::showView()
{
    qDebug() << "Creating graphics view...";
    QGraphicsScene* sim_scene = createGraphicsScene();
    this->view = new QGraphicsView(sim_scene); // create user's view showing the graphics scene

    this->view->setAttribute(Qt::WA_AlwaysShowToolTips); //? maybe necessary ?

    // TODO: test some view parameters
    this->view->setFixedSize(1000, 900);
    this->view->scale(2, 2);
    qDebug() << "Showing graphics view";
    this->view->show(); // shows the graphics scene to the user
}


// --- old : ---
/*
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
*/

void Simulation::createCellsMatrix()
{
    //TODO:
    qDebug() << "cells matrix initial size:" << this->cells.size();
    int max_x_count = ceil(max_x/this->resolution); // -1 ?
    qDebug() << "Max count of cells X:" << max_x_count;
    int max_y_count = ceil(-min_y/this->resolution); // -1 ?
    qDebug() << "Max count of cells Y:" << max_y_count;
    for (int x_count=0; x_count < max_x_count; x_count++) {
        qDebug() << "Creating new line of cells_matrix...";
        qreal x = 0+(this->resolution*x_count);
        //QList<QSharedPointer<Receiver>> temp_list;
        QList<Receiver*> temp_list;
        for (int y_count=0; y_count < max_y_count; y_count++) {
            qreal y = 0-(this->resolution*y_count);
            //temp_list.push_back(QSharedPointer<Receiver>(new Receiver(0,QPointF(x,y))));
            //temp_list.push_back(QSharedPointer<Receiver>(new Receiver(x,y)));
            temp_list.append(new Receiver(x,y));
            qDebug() << "cells_matrix line"<< x_count << "size:" << temp_list.size();
        }
        this->cells.append(temp_list);
        qDebug() << "cells_matrix size:" << this->cells.size();
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
    if (index < 0 || index >= this->baseStations.length()) {
        qWarning("baseStations index out of range");
        throw std::out_of_range("baseStations index out of range");
    }
    //return &this->baseStations.at(index);
    return this->baseStations.at(index);
}

void Simulation::createBaseStation(Transmitter* transmitter)
{
    //TODO: needed ?
    this->baseStations.append(transmitter);
}

void Simulation::deleteBaseStation(int index)
{
    if (index > 0 || index < this->baseStations.size())
    {
        this->baseStations.erase(this->baseStations.begin()+index);
    } else if (index == 0)
    {
        qDebug("Cannot delete Base Station 1");
    } else {
        qWarning("deleteBaseStation error: index out of range");
        throw std::out_of_range("deleteBaseStation error: index out of range");
    }
}

// TODO:
//std::vector<Obstacle*>* Simulation::getObstacles()
//{
//    return &this->obstacles;
//}
QList<Obstacle*>* Simulation::getObstacles()
{
    return &this->obstacles;
}

unsigned int Simulation::getNumberOfObstacles()
{
    return this->obstacles.size();
    // return std::size(this->obstacles);
}

unsigned int Simulation::getNumberOfBaseStations()
{
    return this->baseStations.size();
}

// TODO: use new
/*
void Simulation::computeCell(QSharedPointer<Receiver> cell)
{
    // TODO: recursive function for each ray bounce from the transmitter

    qreal Ra = 1; // ????
    qulonglong rx_power = ((60*pow(wavelength,2))/(8*M_PI*Ra))*this->getBaseStation(0)->getPower()*this->getBaseStation(0)->getGain();

    //for (qreal Gamma; todo) {
    //    rx_power *= pow(qAbs(Gamma),2);
    //}
    //for (qreal T; todo) {
    //    rx_power *= pow(qAbs(T),2);
    //}

}
*/

qint64 Simulation::getSimulationTime() const
{
    return this->simulation_time;
}

// test:
/*
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
*/

#ifdef DRAW_RAYS
void Simulation::drawAllRays(QGraphicsScene* scene, Receiver* _RX) {
    // Adds all rays in all_rays QList to the scene
    for (Ray* ray : _RX->all_rays) {
        //for (int i=0; i<all_rays.length(); i++) {
        qDebug() << "Adding ray to scene";
        //QList<QGraphicsLineItem*> segments_graphics = all_rays.at(i)->getSegmentsGraphics();
        for (QGraphicsLineItem* segment_graphics : ray->getSegmentsGraphics()) {
            //for (int p=0; p<segments_graphics.length(); p++) {
            qDebug() << "->Adding ray segment to scene...";
            QGraphicsLineItem* line = segment_graphics;
            scene->addItem(line);
        }
    }
}
#endif

QGraphicsScene *Simulation::createGraphicsScene()//std::vector<Transmitter>* TX)
{
    // creates the QGraphicsScene (to give to QGraphicsView) and adds all graphics to it
    qDebug() << "Creating graphics scene...";

    QGraphicsScene* scene = new QGraphicsScene();

    // Draw all walls in wall_list
    for (Obstacle* wall : this->obstacles){
        qDebug() << "Adding wall to scene...";
        scene->addItem(wall->graphics);
    }
    qDebug() << "All walls added to scene.";

#ifdef DRAW_RAYS
    // Draw all rays (their segments) from the all_rays list
    drawAllRays(scene);
#endif

    Transmitter* TX = this->baseStations[0];
    // TODO: multiple transmitters ?
    //for (Transmitter* TX : this->baseStations) {
    //    // Draw TX and add its tooltip
    //    TX->graphics->setToolTip(QString("Test transmitter\nx=%1 y=%2\nG_TX*P_TX=%3").arg(QString::number(TX->x()),QString::number(TX->y()),QString::number(TX->gain*TX->power)));
    //    scene->addItem(TX->graphics);
    //    qDebug() << "TX.graphics:" << TX->graphics->rect();
    //}

    for (QList<Receiver*> cells_line : this->cells) {
        for (Receiver* RX : cells_line) {
            // compute total power and set it in RX
            qreal totalPower = RX->computeTotalPower(TX);
            RX->power = totalPower;

            // Draw RX and add its tooltip
            float _rx_power_dBm = 10*std::log10(RX->power*1000); // TODO: correct ? *1000 because is in Watts and need in mW :
            RX->graphics->setToolTip(QString("Test receiver\nx=%1 y=%2\nPower: %3 mW | %4 dBm").arg(QString::number(RX->x()),QString::number(RX->y()),QString::number(RX->power*1000),QString::number(_rx_power_dBm,'f',2)));
            scene->addItem(RX->graphics);
            qDebug() << "RX.graphics:" << RX->graphics->rect();
        }
    }

#ifdef DRAW_IMAGES
    // For debugging: draw all images from the tx_images list
    for (QGraphicsEllipseItem* image_graphics : tx_images) {
        image_graphics->setPen(QPen(Qt::darkYellow));
        image_graphics->setBrush(QBrush(Qt::darkYellow));
        image_graphics->setToolTip(QString("image\nx=%1 y=%2").arg(QString::number(image_graphics->rect().x()+image_graphics->rect().width()/2),QString::number(-image_graphics->rect().y()-image_graphics->rect().height()/2)));
        scene->addItem(image_graphics);
    }
#endif

#ifdef DRAW_REFLECTION_POINTS
    // For debugging: draw all reflection points from the reflection_points list
    for (QGraphicsEllipseItem* reflection_graphics : reflection_points) {
        reflection_graphics->setPen(QPen(Qt::darkGreen));
        reflection_graphics->setBrush(QBrush(Qt::darkGreen));
        reflection_graphics->setToolTip(QString("reflection\nx=%1 y=%2").arg(QString::number(reflection_graphics->rect().x()+reflection_graphics->rect().width()/2),QString::number(-reflection_graphics->rect().y()-reflection_graphics->rect().height()/2)));
        scene->addItem(reflection_graphics);
    }
#endif

    scene->setBackgroundBrush(QBrush(Qt::black)); // black background

    return scene;
}
