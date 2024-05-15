#include "simulation.h"

#include <QDir>
#include <QtMath>

#include "parameters.h"

Simulation::Simulation() {
    // class constructor

    createWalls();
}

void Simulation::createWalls()
{
    //qDebug("Creating walls...");

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
    ObstacleType concrete = ConcreteWall;
    qreal thickness = 0.3; // 30cm
    concrete_walls.append(new Obstacle(QVector2D(0,0), QVector2D(15,0), concrete, thickness));
    concrete_walls.append(new Obstacle(QVector2D(15,0), QVector2D(15,4), concrete, thickness));
    concrete_walls.append(new Obstacle(QVector2D(7,0), QVector2D(7,4), concrete, thickness));
    concrete_walls.append(new Obstacle(QVector2D(0,0), QVector2D(0,8), concrete, thickness));
    concrete_walls.append(new Obstacle(QVector2D(0,8), QVector2D(6,8), concrete, thickness));
    concrete_walls.append(new Obstacle(QVector2D(4,6), QVector2D(9,6), concrete, thickness));
    concrete_walls.append(new Obstacle(QVector2D(4,6), QVector2D(4,8), concrete, thickness));
    concrete_walls.append(new Obstacle(QVector2D(9,6), QVector2D(9,8), concrete, thickness));
    concrete_walls.append(new Obstacle(QVector2D(9,8), QVector2D(12,8), concrete, thickness));

    QList<Obstacle*> drywall_walls;
    ObstacleType drywall = DryWall;
    thickness=0.1; // 10cm
    drywall_walls.append(new Obstacle(QVector2D(4,0), QVector2D(4,4), drywall, thickness));
    drywall_walls.append(new Obstacle(QVector2D(4,4), QVector2D(5,4), drywall, thickness));
    drywall_walls.append(new Obstacle(QVector2D(6,4), QVector2D(9,4), drywall, thickness));
    drywall_walls.append(new Obstacle(QVector2D(10,4), QVector2D(11,4), drywall, thickness));
    drywall_walls.append(new Obstacle(QVector2D(11,0), QVector2D(11,4), drywall, thickness));
    drywall_walls.append(new Obstacle(QVector2D(0,5), QVector2D(4,5), drywall, thickness));

    thickness=0.05; // 5cm
    Obstacle* glass_window = new Obstacle(QVector2D(12,8),QVector2D(15,4), Window, thickness); // this one is diagonal

    thickness=0.05; // 5cm
    Obstacle* metal_lift_door = new Obstacle(QVector2D(6,6),QVector2D(6,8), MetalWall, thickness);

    // /!\ The lift is only added to the obstacles if enabled
    QList<Obstacle*> all_obstacles;
    all_obstacles.append(concrete_walls);
    all_obstacles.append(drywall_walls);
    all_obstacles.append(glass_window);
    all_obstacles.append(metal_lift_door); // last one is the metal lift door

    this->obstacles = all_obstacles;
    qDebug() << "Walls created";
}

void Simulation::run(QProgressBar* progress_bar)
{
    // TODO: compute everything
    this->timer.start();
    qDebug() << "Simulation::run() - single cell simulation: " << (this->showRaySingleCell); // still TODO: single cell simulation

    qDebug() << "P_TX:" << P_TX << "W," << P_TX_dBm << "dBm";
    qDebug() << "G_TX:" << G_TX;
    qDebug() << "beta:" << beta_0;
    qDebug() << "lambda:" << lambda << "m";
    qDebug() << "frequency:" << freq << "Hz";
    qDebug() << "omega:" << omega << "rad/s";

    //this->cells_matrix.clear();
    for (QList<Receiver*> cells_line : this->cells) {
        qDeleteAll(cells_line);
    }
    this->cells.clear();
    qDeleteAll(this->obstacles);
    this->obstacles.clear();

    createWalls();
    if (this->lift_is_on_floor) { // Adds the lift metal walls if set as present
        qDebug() << "Lift is on this floor.";

        QList<Obstacle*> lift_walls;
        qreal thickness = 0.05; // 5cm
        lift_walls.append(new Obstacle(QVector2D(4.25,6.25),QVector2D(5.75,6.25), MetalWall, thickness));
        lift_walls.append(new Obstacle(QVector2D(4.25,6.25),QVector2D(4.25,7.75), MetalWall, thickness));
        lift_walls.append(new Obstacle(QVector2D(5.75,6.25),QVector2D(5.75,7.75), MetalWall, thickness));
        lift_walls.append(new Obstacle(QVector2D(4.25,7.75),QVector2D(5.75,7.75), MetalWall, thickness));

        this->obstacles.append(lift_walls);
    }

    if (!this->showRaySingleCell) {
        createCellsMatrix();
    } else {
        // single cell simulation
        Receiver* rx = new Receiver(this->singleCellX,this->singleCellY,0.5, true);
        QPen singleCellPen = QPen(Qt::magenta);
        singleCellPen.setWidthF(10*0.03);
        rx->graphics->setPen(singleCellPen);
        this->cells = {{rx}};
    }

    Transmitter* base_station = this->baseStations[0]; // TODO: feature : multiple transmitters?

    //if (this->cells_matrix.isEmpty()) {
    if (this->cells.isEmpty()) {
        qWarning("no cells provided (simulation.cells matrix is empty)");
        throw std::exception();
    }

    int i=0;
    for (QList<Receiver*> cells_line : this->cells){
        // loops over each line
        for (Receiver* cell : cells_line) {
            computeDirect(cell, *base_station);
            computeReflections(cell, *base_station);
            progress_bar->setValue(i/(cells_line.length()*this->cells.length()));
            i++;
        }
    }
    qDebug() << this->cells.length()*this->cells[0].length() << "cells," << i << "computed";

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
        if (checkSameSideOfWall(wall->normal,_TX,*_RX)) {
            //same side of this wall, can make a reflection
            //qDebug() << "Same side of wall TX and RX:" << wall << _TX.toPointF() << _RX->toPointF() ;
            Ray* ray_1_reflection = new Ray(_TX.toPointF(), _RX->toPointF());

            QVector2D _imageTX = computeImage(_TX, wall);
            //qDebug() << "_image:" << _imageTX.x() << _imageTX.y();

            QVector2D _P_r = calculateReflectionPoint(_imageTX,*_RX,wall);

            // CHECK IF REFLECTION IS ON THE WALL AND NOT ITS EXTENSION:
            RaySegment* test_segment = new RaySegment(_imageTX.x(),_imageTX.y(),_RX->x(),_RX->y());
            if (!checkRaySegmentIntersectsWall(wall, test_segment)) {
                // RAY DOES NOT TRULY INTERSECT THE WALL (only the wall extension) ignore this one-reflection ray at this wall
                //qDebug() << "ignore";
                delete ray_1_reflection;
                delete test_segment;
                ////continue; // break out of this forloop instance for this wall
                goto second_reflection; // don't make a 1 reflection ray with this wall, proceed to two-reflections rays
            }
            delete test_segment;

            //qDebug() << "P_r" << _P_r;

            // create ray segments between points
            QList<RaySegment*> ray_segments;
            ray_segments.append(new RaySegment(_TX.x(),_TX.y(),_P_r.x(),_P_r.y())); // first segment
            ray_segments.append(new RaySegment(_P_r.x(),_P_r.y(),_RX->x(),_RX->y())); // last segment

            ray_1_reflection->segments = ray_segments;
            addReflection(ray_1_reflection,_imageTX,*_RX,wall);
            checkTransmissions(ray_1_reflection,{wall});

            if (this->showRaySingleCell) {
                this->singleCellSimReflectionPoints.append(_P_r);
            }

            //qDebug() << "ray_1_refl distance:" << QVector2D(*_RX - _imageTX).length();
            ray_1_reflection->distance = QVector2D(*_RX-_imageTX).length();
            //qDebug() << "Ray's (1refl) total coeffs:" << ray_1_reflection->getTotalCoeffs();
            _RX->all_rays.append(ray_1_reflection);
        }

        second_reflection:
        QVector2D _imageTX = computeImage(_TX, wall);
        //qDebug() << "_image:" << _imageTX.x() << _imageTX.y();
        // 2nd reflection
        for (Obstacle* wall_2 : this->obstacles) {
            // check that the second wall is not the same as the first wall and that imageTX and RX are at the same side of this second wall
            if (wall_2 != wall && checkSameSideOfWall(wall_2->normal,_imageTX,*_RX)) {
                //qDebug() << "Same side of wall imageTX and RX --- wall_2:" << wall_2->line.p1() << wall_2->line.p2() << ", imageTX:" << _imageTX.toPointF() << ", RX:" << _RX->toPointF() ;
                Ray* ray_2_reflection = new Ray(_TX.toPointF(),_RX->toPointF());

                QVector2D _image_imageTX = computeImage(_imageTX,wall_2);
                //qDebug() << "_image_image:" << _image_imageTX.x() << _image_imageTX.y();

                QVector2D _P_r_2_last = calculateReflectionPoint(_image_imageTX,*_RX,wall_2);
                QVector2D _P_r_2_first = calculateReflectionPoint(_imageTX,_P_r_2_last,wall);
                if (_P_r_2_last.x()==_P_r_2_first.x() && _P_r_2_last.y()==_P_r_2_first.y()) {
                    //qDebug() << "------> P_r_2_last = P_r_2_first !!!)";
                }

                RaySegment* test_segment_1 = new RaySegment(_image_imageTX.x(),_image_imageTX.y(),_RX->x(),_RX->y());
                RaySegment* test_segment_2 = new RaySegment(_imageTX.x(),_imageTX.y(),_P_r_2_last.x(),_P_r_2_last.y());
                if (!checkRaySegmentIntersectsWall(wall_2, test_segment_1) || !checkRaySegmentIntersectsWall(wall,test_segment_2)) {
                    //qDebug() << "ignore";
                    // RAY DOES NOT TRULY INTERSECT THE WALL (only the wall extension) ignore this two-reflections ray at this wall
                    delete ray_2_reflection;
                    delete test_segment_1;
                    delete test_segment_2;
                    continue; // break out of this forloop instance for this wall
                }
                delete test_segment_1;
                delete test_segment_2;

                //qDebug() << "P_r_2_first" << _P_r_2_first;
                //qDebug() << "P_r_2_last" << _P_r_2_last;

                QList<RaySegment*> ray_segments_2;
                ray_segments_2.append(new RaySegment(_TX.x(),_TX.y(),_P_r_2_first.x(),_P_r_2_first.y()));
                ray_segments_2.append(new RaySegment(_P_r_2_first.x(),_P_r_2_first.y(),_P_r_2_last.x(),_P_r_2_last.y()));
                ray_segments_2.append(new RaySegment(_P_r_2_last.x(),_P_r_2_last.y(),_RX->x(),_RX->y()));

                ray_2_reflection->segments = ray_segments_2;
                addReflection(ray_2_reflection,_imageTX,_P_r_2_last,wall);
                addReflection(ray_2_reflection,_image_imageTX,*_RX,wall_2);
                checkTransmissions(ray_2_reflection,{wall,wall_2});

                if (this->showRaySingleCell) {
                    this->singleCellSimReflectionPoints.append(_P_r_2_first);
                    this->singleCellSimReflectionPoints.append(_P_r_2_last);
                }

                //qDebug() << "ray_2_refl distance:" << QVector2D(*_RX - _image_imageTX).length();
                ray_2_reflection->distance = QVector2D(*_RX-_image_imageTX).length();
                //qDebug() << "Ray's (2refl) total coeffs:" << ray_2_reflection->getTotalCoeffs();
                _RX->all_rays.append(ray_2_reflection);
            }
        }
    }
}

// compute the image position
QVector2D Simulation::computeImage(const QVector2D& _point, Obstacle* wall) {
    // returns the coordinates of _point's image with wall
    QVector2D new_origin = QVector2D(wall->line.p1()); // set origin to point1 of wall
    //qDebug() << "new coords" << wall->line.p1();
    QVector2D _normal = wall->normal; // normal to the wall (is normalized so it is relative to any origin)
    //qDebug() << "normal" << wall->normal;
    QVector2D new_point_coords = _point - new_origin; // initial point in new coordinates relative to point1 of wall
    double _dotProduct = QVector2D::dotProduct(new_point_coords, _normal);
    QVector2D _image_new_coords = new_point_coords - 2 * _dotProduct * _normal; // image point in new coordinates relative to point1 of wall
    QVector2D _image = new_origin + _image_new_coords; // image point in absolute coordinates
    //qDebug() << "image:" << _image.x() << _image.y();

    return _image;
}

// compute the reflection point on the wall
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
    // computes the final Gamma coeff for the ray_segment's reflection with this wall, and adds it to this ray's coeffs list
    QVector2D _d = _p2-_p1;
    qreal _cos_theta_i = abs(QVector2D::dotProduct(_d.normalized(),wall->normal));
    qreal _sin_theta_i = sqrt(1.0 - pow(_cos_theta_i,2));
    qreal _sin_theta_t = _sin_theta_i / sqrt(wall->properties.relative_permittivity);
    qreal _cos_theta_t = sqrt(1.0 - pow(_sin_theta_t,2));
    complex<qreal> Gamma_coeff = computeReflectionCoeff(_cos_theta_i,_sin_theta_i,_cos_theta_t,_sin_theta_t, wall);
    //qDebug() << "addReflection, Gamma_coeff:" << Gamma_coeff;
    if (Gamma_coeff.real() != Gamma_coeff.real() || Gamma_coeff.imag() != Gamma_coeff.imag()) {
        qDebug() << "Gamma_coeff = NaN";
        qDebug() << "_d" << _d;
        qDebug() << "_cos_theta_i" << _cos_theta_i;
        qDebug() << "_sin_theta_i" << _sin_theta_i;
        qDebug() << "_sin_theta_t" << _sin_theta_t;
        qDebug() << "_cos_theta_t" << _cos_theta_t;
    }
    _ray->addCoeff(Gamma_coeff);
}

complex<qreal> Simulation::makeTransmission(RaySegment* ray_segment, Obstacle* wall) {
    // computes the final T coeff for the ray_segment's transmission with this wall, and adds it to this ray's coeffs list
    QVector2D _d = QVector2D(ray_segment->p1())-QVector2D(ray_segment->p2());
    qreal _cos_theta_i = abs(QVector2D::dotProduct(_d.normalized(),wall->normal));
    qreal _sin_theta_i = sqrt(1.0 - pow(_cos_theta_i,2));
    qreal _sin_theta_t = _sin_theta_i / sqrt(wall->properties.relative_permittivity);
    qreal _cos_theta_t = sqrt(1.0 - pow(_sin_theta_t,2));

    complex<qreal> T_coeff = computeTransmissionCoeff(_cos_theta_i,_sin_theta_i,_cos_theta_t,_sin_theta_t,wall);
    if (T_coeff.real() != T_coeff.real() || T_coeff.imag() != T_coeff.imag()) {
        qDebug() << "Gamma_coeff = NaN";
        qDebug() << "_d" << _d;
        qDebug() << "_cos_theta_i" << _cos_theta_i;
        qDebug() << "_sin_theta_i" << _sin_theta_i;
        qDebug() << "_sin_theta_t" << _sin_theta_t;
        qDebug() << "_cos_theta_t" << _cos_theta_t;
    }
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
                    complex<qreal> T_coeff = makeTransmission(ray_segment,wall);
                    //qDebug() << "checkTransmission, T_coeff:" << T_coeff;
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
        QPointF* intersection_point = nullptr; // not used
        if (checkRaySegmentIntersectsWall(wall, _direct_line, intersection_point)) {
            // transmission through this wall, compute the transmission coeff
            complex<qreal> T_coeff = makeTransmission(_direct_line,wall);
            direct_ray->addCoeff(T_coeff);
        }
        //else {
        //    continue;
        //}
    }
    //qDebug() << "ray_direct distance:" << QVector2D(*_RX - _TX).length();
    direct_ray->distance = QVector2D(*_RX-_TX).length();
    //qDebug() << "Ray's (direct) total coeffs:" << direct_ray->getTotalCoeffs();
    direct_ray->segments = {_direct_line};
    _RX->all_rays.append(direct_ray);
}

complex<qreal> Simulation::computePerpendicularGamma(qreal _cos_theta_i, qreal _cos_theta_t, Obstacle* wall)
{
    // returns Gamma_perpendicular
    complex<qreal> left = wall->properties.Z_m * _cos_theta_i;
    //qDebug() << "left perpGamma:" << left.real() << "+j" << left.imag();
    complex<qreal> right = Z_0 * _cos_theta_t;
    //qDebug() << "right perpGamma:" << right.real() << "+j" << right.imag();
    //complex<qreal> Gamma_perp = (wall->properties.Z_m*_cos_theta_i-Z_0*_cos_theta_t)/(wall->properties.Z_m*_cos_theta_i+Z_0*_cos_theta_t);
    complex<qreal> Gamma_perp = (left - right) / (left + right);

    //qDebug() << "Gamma_perp=" << QString::number(Gamma_perp.real()) << "+j" << QString::number(Gamma_perp.imag());
    return Gamma_perp;
}

complex<qreal> Simulation::computeReflectionCoeff(qreal _cos_theta_i, qreal _sin_theta_i, qreal _cos_theta_t, qreal _sin_theta_t, Obstacle* wall)
{
    // returns the reflection coefficient Gamma_m
    qreal s = wall->thickness/_cos_theta_t;
    complex<qreal> Gamma_perpendicular = computePerpendicularGamma(_cos_theta_i, _cos_theta_t, wall);
    complex<qreal> reflection_term = exp(-2.0 * wall->properties.gamma_m * s) * exp(j * 2.0 * beta_0 * s * _sin_theta_t * _sin_theta_i);
    //qDebug() << "reflection_term:" << QString::number(reflection_term.real()) << "+ j" << QString::number(reflection_term.imag());
    complex<qreal> Gamma_m = Gamma_perpendicular - (1.0 - pow((Gamma_perpendicular), 2)) * (Gamma_perpendicular * reflection_term) / (1.0 - pow((Gamma_perpendicular), 2) * reflection_term);
    //qDebug() << "Gamma_m:" << QString::number(Gamma_m.real()) << "+ j" << QString::number(Gamma_m.imag());

    return Gamma_m;
}

complex<qreal> Simulation::computeTransmissionCoeff(qreal _cos_theta_i, qreal _sin_theta_i, qreal _cos_theta_t, qreal _sin_theta_t, Obstacle* wall)
{
    // returns the transmission coefficient T_m
    //qDebug() << "_cos_theta_i" << _cos_theta_i;
    //qDebug() << "_sin_theta_i" << _sin_theta_i;
    //qDebug() << "_sin_theta_t" << _sin_theta_t;
    //qDebug() << "_cos_theta_t" << _cos_theta_t;
    qreal s = wall->thickness/_cos_theta_t;
    //qDebug() << "s" << s;
    complex<qreal> perpGamma = computePerpendicularGamma(_cos_theta_i, _cos_theta_t, wall);
    //qDebug() << "perpGamma" << perpGamma.real() << "+j" << perpGamma.imag();
    complex<qreal> T_m = ((1.0-pow(perpGamma,2))*exp(-(wall->properties.gamma_m)*s))/(1.0-(pow(perpGamma,2)*exp(-2.0*(wall->properties.gamma_m)*s)*exp(j*beta_0*2.0*s*_sin_theta_t*_sin_theta_i)));
    //qDebug() << "TransmissionCoeff=" << QString::number(T_m.real()) << "+j" << QString::number(T_m.imag());
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
    this->scene = sim_scene;
    this->view = new QGraphicsView(sim_scene); // create user's view showing the graphics scene

    this->view->setAttribute(Qt::WA_AlwaysShowToolTips); //? maybe necessary ?

    this->view->setFixedSize(990, 720);
    this->view->scale(6, 6);
    qDebug() << "Showing graphics view";
    //QIcon _icon = QIcon(QDir::currentPath()+"/icon.png");
    //view->setWindowIcon(QIcon("./assets/icon.png"));
    view->setWindowIcon(QIcon(":/assets/icon.png"));
    this->view->show(); // shows the graphics scene to the user
}

void Simulation::createCellsMatrix()
{
    int max_x_count = ceil(max_x/this->resolution); // -1 ?
    //qDebug() << "Max count of cells X:" << max_x_count;
    int max_y_count = ceil(-min_y/this->resolution); // -1 ?
    //qDebug() << "Max count of cells Y:" << max_y_count;

    qInfo() << "Creating cells matrix" << max_x_count << "x" << max_y_count << "...";
    //qDebug() << "cells matrix initial size:" << this->cells.size();
    for (int x_count=0; x_count < max_x_count; x_count++) {
        //qDebug() << "Creating new line of cells_matrix...";
        qreal x = this->resolution/2+(this->resolution*x_count);
        QList<Receiver*> temp_list;
        for (int y_count=0; y_count < max_y_count; y_count++) {
            qreal y = this->resolution/2+(this->resolution*y_count);
            temp_list.append(new Receiver(x,y,this->resolution, this->show_cell_outline));
            //qDebug() << "cells_matrix line"<< x_count << "size:" << temp_list.size();
        }
        this->cells.append(temp_list);
        //qDebug() << "cells_matrix size:" << this->cells.size();
    }
    qDebug() << "cells_matrix created";
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

QList<Obstacle*>* Simulation::getObstacles()
{
    return &this->obstacles;
}

unsigned int Simulation::getNumberOfObstacles()
{
    return this->obstacles.size();
}

unsigned int Simulation::getNumberOfBaseStations()
{
    return this->baseStations.size();
}

qint64 Simulation::getSimulationTime() const
{
    return this->simulation_time;
}

QGraphicsScene *Simulation::createGraphicsScene()//std::vector<Transmitter>* TX)
{
    // creates the QGraphicsScene (to give to QGraphicsView) and adds all graphics to it
    qDebug() << "Creating graphics scene...";

    QGraphicsScene* scene = new QGraphicsScene();

    Transmitter* TX = this->baseStations[0];

    for (QList<Receiver*> cells_line : this->cells) {
        for (Receiver* RX : cells_line) {
            // compute total power and set it in RX
            qreal _rx_power = RX->computeTotalPower(TX);
            RX->power = _rx_power;

            RX->updateBitrateAndColor();
            QBrush _rxBrush = RX->graphics->brush();
            if (this->showRaySingleCell) {
                _rxBrush.setColor(Qt::black);
            } else {
                _rxBrush.setColor(RX->cell_color);
            }
            RX->graphics->setBrush(_rxBrush);

            // Draw RX and add its tooltip
            float _rx_power_dBm = 10*std::log10(RX->power*1000); // TODO: correct ? *1000 because is in Watts and need in mW :
            //qDebug() << "RX power:" << _rx_power << "W," << _rx_power_dBm << "dBm";
            //qDebug() << "RX bitrate:" << RX->bitrate_Mbps << "Mbps";

            qreal bitrate_Mbps = RX->bitrate_Mbps;
            if (bitrate_Mbps >= 1000) {
                qreal bitrate_Gbps = bitrate_Mbps/1000;
                RX->graphics->setToolTip(
                    //QString("Receiver cell:\nx=%1 y=%2\nPower: %3 mW | %4 dBm\nBitrate: %5 Gbps\nDirect ray (%7 segments) coeffs list length: %6").arg(
                    QString("Receiver cell:\nx=%1 y=%2\nPower: %3 mW | %4 dBm\nBitrate: %5 Gbps").arg(
                        QString::number(RX->x()),
                        QString::number(RX->y()),
                        QString::number(_rx_power*1000),
                        QString::number(_rx_power_dBm,'f',2),
                        QString::number(bitrate_Gbps,'f',2)
                        ////QString::number(RX->all_rays.first()->coeffsList.length()),
                        ////QString::number(RX->all_rays.first()->segments.length())
                    ));
            } else {
                RX->graphics->setToolTip(
                    //QString("Receiver cell:\nx=%1 y=%2\nPower: %3 mW | %4 dBm\nBitrate: %5 Mbps\nDirect ray (%7 segments) coeffs list length: %6").arg(
                    QString("Receiver cell:\nx=%1 y=%2\nPower: %3 mW | %4 dBm\nBitrate: %5 Mbps").arg(
                        QString::number(RX->x()),
                        QString::number(RX->y()),
                        QString::number(_rx_power*1000),
                        QString::number(_rx_power_dBm,'f',2),
                        QString::number(bitrate_Mbps)
                        //QString::number(RX->all_rays.first()->coeffsList.length()),
                        //QString::number(RX->all_rays.first()->segments.length())
                    ));
            }
            scene->addItem(RX->graphics);
            //qDebug() << "RX.graphics:" << RX->graphics->rect();
        }
    }
    qDebug() << "All receiver cells added to scene.";

    // Draw all walls in wall_list
    for (Obstacle* wall : this->obstacles){
        //qDebug() << "Adding wall to scene...";
        scene->addItem(wall->graphics);
    }
    qDebug() << "All walls added to scene.";

    // TODO: feature : multiple transmitters ?
    //for (Transmitter* TX : this->baseStations) {
    //    // Draw TX and add its tooltip
    //    TX->graphics->setToolTip(QString("Transmitter\nx=%1 y=%2\nG_TX*P_TX=%3").arg(QString::number(TX->x()),QString::number(TX->y()),QString::number(TX->gain*TX->power)));
    //    scene->addItem(TX->graphics);
    //    //qDebug() << "TX.graphics:" << TX->graphics->rect();
    //}
    TX->graphics->setToolTip(QString("Transmitter:\nx=%1 y=%2\nGain: %3\nPower: %4W").arg(QString::number(TX->x()),QString::number(TX->y()),QString::number(TX->gain),QString::number(TX->power)));
    //qDebug() << "TX.graphics:" << TX->graphics->rect();
    scene->addItem(TX->graphics);
    qDebug() << "Transmitter added to scene.";

    if (this->showRaySingleCell) {
        // Draw all rays (their segments) from the all_rays list

        for (Ray* ray :this->cells[0][0]->all_rays) {
            qDebug() << "Drawing ray";
            for (QGraphicsLineItem* segment_graphics : ray->getSegmentsGraphics()) {
                scene->addItem(segment_graphics);
            }
        }

        //// --- TEST only direct ---
        //#ifdef DEBUG_SINGLE_CELL_DIRECT_RAY
        //for (QGraphicsLineItem* gra : this->cells[0][0]->all_rays.first()->getSegmentsGraphics()) {
        //    scene->addItem(gra);
        //}
        //qDebug() << "number of coeffs:" << this->cells[0][0]->all_rays.first()->coeffsList.length();
        //qDebug() << "T_m=" << this->cells[0][0]->all_rays.first()->coeffsList.first().real() << "+j" << this->cells[0][0]->all_rays.first()->coeffsList.first().imag();
        //#endif
        //// ---

        qDebug() << "All rays added to scene.";

        for (QVector2D point : this->singleCellSimReflectionPoints) {
            qreal width = 0.04;
            QGraphicsEllipseItem* reflection_graphics = new QGraphicsEllipseItem(10*(point.x()-width),10*(point.y()-width),2*width*10,2*width*10);
            reflection_graphics->setPen(QPen(Qt::green));
            reflection_graphics->setBrush(QBrush(Qt::green));
            reflection_graphics->setToolTip("Reflection point");
            scene->addItem(reflection_graphics);
        }

        qDebug() << "All reflection points added to scene.";
    }

    addLegend(scene);

    scene->setBackgroundBrush(QBrush(Qt::black)); // black background

    qDebug() << "Scene created.";

    return scene;
}

void Simulation::addLegend(QGraphicsScene* scene)
{
    qDebug() << "Adding legend to scene";

    QPen legendPen(Qt::white);
    legendPen.setWidthF(0.3);

    if (!this->showRaySingleCell) {

        QLinearGradient gradient;
        QRectF rect(6,85,138,10); // gradient rectangle scene coordinates
        QGraphicsRectItem* gradient_graphics = new QGraphicsRectItem(rect);

        gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
        gradient.setColorAt(0.0, QColor::fromRgb(255,0,0)); // blue
        gradient.setColorAt(0.25, QColor::fromRgb(255,255,0)); // cyan
        gradient.setColorAt(0.5, QColor::fromRgb(0,255,0)); // green
        gradient.setColorAt(0.75, QColor::fromRgb(0,255,255)); // yellow
        gradient.setColorAt(1.0, QColor::fromRgb(0,0,255)); // red
        gradient.setStart(1.0, 0.0); // start left
        gradient.setFinalStop(0.0, 0.0); // end right

        QBrush gradientBrush(gradient);

        gradient_graphics->setPen(legendPen);
        gradient_graphics->setBrush(gradientBrush);
        scene->addItem(gradient_graphics); // draw gradient rectangle
        qreal rect_width = rect.width();
        for (int i=0; i<5; i++) {
            QGraphicsLineItem* small_line = new QGraphicsLineItem(rect.bottomLeft().x()+0.25*i*rect_width,rect.bottomLeft().y(),rect.bottomLeft().x()+0.25*i*rect_width,rect.bottomLeft().y()+2.0);
            small_line->setPen(legendPen);
            scene->addItem(small_line);
        }

        // text for minimum of gradient
        QGraphicsTextItem* min_text = new QGraphicsTextItem("50Mbps\nat -90dBm");
        min_text->setPos(rect.bottomLeft().x()-6,rect.bottomLeft().y()+1);
        min_text->setDefaultTextColor(Qt::white);
        min_text->setScale(0.25);
        // text for maximum of gradient
        QGraphicsTextItem* max_text = new QGraphicsTextItem("40Gbps\nat -40dBm");
        max_text->setPos(rect.bottomRight().x()-6,rect.bottomRight().y()+1);
        max_text->setDefaultTextColor(Qt::white);
        max_text->setScale(0.25);

        scene->addItem(min_text);
        scene->addItem(max_text);
    } else {
        QGraphicsTextItem* ray_colors = new QGraphicsTextItem("Green line: Direct ray\nRed line: One-reflection ray\nYellow line: Two-reflections ray");
        ray_colors->setPos(50,85);
        ray_colors->setScale(0.2);
        ray_colors->setDefaultTextColor(Qt::white);
        scene->addItem(ray_colors);
    }

    // axes legends :
    QGraphicsLineItem* x_line = new QGraphicsLineItem(0,-5,15*10,-5);
    QGraphicsLineItem* y_line = new QGraphicsLineItem(-5,0,-5,8*10);
    x_line->setPen(legendPen);
    y_line->setPen(legendPen);
    scene->addItem(x_line);
    scene->addItem(y_line);
    QGraphicsTextItem* x_label = new QGraphicsTextItem("x");
    x_label->setPos(-2.5,-6.8);
    x_label->setScale(0.15);
    x_label->setDefaultTextColor(Qt::white);
    scene->addItem(x_label);
    QGraphicsTextItem* y_label = new QGraphicsTextItem("y");
    y_label->setPos(-6,-3.7);
    y_label->setScale(0.15);
    y_label->setDefaultTextColor(Qt::white);
    scene->addItem(y_label);
    for (int x=0; x<=15; x++) {
        QGraphicsLineItem* small_line = new QGraphicsLineItem(0+(x*10),-5,0+(x*10),-6);
        small_line->setPen(legendPen);
        QGraphicsTextItem* x_index = new QGraphicsTextItem(QString::number(x));
        x_index->setPos(small_line->line().x2()-1.2,small_line->line().y2()-3.5);
        x_index->setScale(0.15);
        x_index->setDefaultTextColor(Qt::white);
        scene->addItem(small_line);
        scene->addItem(x_index);
    }
    QGraphicsTextItem* x_unit = new QGraphicsTextItem("m");
    x_unit->setPos(-3.6,-6-3.5);
    x_unit->setScale(0.15);
    x_unit->setDefaultTextColor(Qt::white);
    scene->addItem(x_unit);
    for (int y=0; y<=8; y++) {
        QGraphicsLineItem* small_line = new QGraphicsLineItem(-5,0+(y*10),-6,0+(y*10));
        small_line->setPen(legendPen);
        QGraphicsTextItem* y_index = new QGraphicsTextItem(QString::number(y));
        y_index->setPos(small_line->line().x2()-2.3,small_line->line().y2()-2);
        y_index->setScale(0.15);
        y_index->setDefaultTextColor(Qt::white);
        scene->addItem(small_line);
        scene->addItem(y_index);
    }
    QGraphicsTextItem* y_unit = new QGraphicsTextItem("m");
    y_unit->setPos(-6-2.5,-4.4);
    y_unit->setScale(0.15);
    y_unit->setDefaultTextColor(Qt::white);
    scene->addItem(y_unit);

}
