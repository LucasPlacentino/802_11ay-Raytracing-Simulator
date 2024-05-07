#include "obstacle.h"

#include <QPen>
#include <QVector2D>
//#include "parameters.h"

/*
 * The floorplan is 9m x 15m (x=0,y=0 top left to x=15,y=-9 bottom right) :
 *  __________________________
 * |      |    |      |      |
 * |      |    |      |      |
 * |      |    |      |      |
 * |      |_  _|___  _|      |
 * |______                  /
 * |       _________       /
 * |      |##I     |      /
 * |      |##I     |     /
 * -----------     ------
 *
 * _ or | : wall
 * # : lift
 * I : metal door
 * / : glass window
 *
*/


/*
Obstacle::Obstacle(
    QPointF start_coordinates,
    QPointF end_coordinates,
    ObstacleType material,
    //qreal relative_permittivity,
    //qreal conductivity,
    int thickness_cm)
{
    qDebug() << "Creating Obstacle.";
    this->material = material;
    this->start_coordinates = start_coordinates;
    this->end_coordinates = end_coordinates;
    //this->properties.relative_permittivity = relative_permittivity; // set properties hard coded in the switch case below ?
    //this->properties.conductivity = conductivity; // set properties hard coded in the switch case below ?
    this->thickness_cm = thickness_cm;

    this->drawing_thickness = 2;

    QLineF line(this->start_coordinates,this->end_coordinates); // in func args ?
    setLine(line);
    QPen *pen = new QPen();
    pen->setWidth(this->drawing_thickness);

    switch (this->material) {
    case BrickWall:
        pen->setColor(Qt::darkRed);
        this->properties.relative_permittivity = 3.95;
        this->properties.conductivity = 0.073;
        break;
    case Window:
        pen->setColor(Qt::cyan);
        this->properties.relative_permittivity = 6.3919;
        this->properties.conductivity = 0.0107;
        break;
    case MetalWall:
        pen->setColor(Qt::gray);
        this->properties.relative_permittivity = 1;
        this->properties.conductivity = 10e7;
        break;
    case DryWall:
        pen->setColor(Qt::lightGray);
        this->properties.relative_permittivity = 2.7;
        this->properties.conductivity = 0.05349;
        break;
    case ConcreteWall:
        pen->setColor(Qt::green);
        this->properties.relative_permittivity = 6.4954;
        this->properties.conductivity = 1.43;
        break;
    default:
        pen->setColor(Qt::white);
        this->properties.relative_permittivity = 0;
        this->properties.conductivity = 0;
        break;
    }
    setPen(*pen);

    this->properties.epsilon = epsilon_0 * this->properties.relative_permittivity;
    this->properties.Z_m = sqrt(mu_0 / (this->properties.epsilon - j * this->properties.conductivity / omega));

    //setAcceptHoverEvents(true); // really needed ?
}
*/

Obstacle::Obstacle(QVector2D start, QVector2D end, ObstacleType material, qreal thickness)//, int id)
{
    // Wall object cosntructor
    qDebug("Creating wall...");
    this->material = material;
    //this->id = id;
    this->thickness = thickness; // in meters
    this->line = QLineF(start.x(),start.y(), end.x(), end.y());
    QLineF graphics_line = QLineF(start.x(),-start.y(), end.x(), -end.y());
    qDebug() << "Wall" << id << "line:" << this->line ;
    QLineF normal_line = this->line.normalVector();
    qDebug() << "Line" << id << "normal:" << normal_line;
    this->normal = QVector2D(normal_line.dx(),normal_line.dy()).normalized(); // ! normalized !
    qDebug() << "Wall" << id << "normal:" << this->normal;
    QLineF unit_line = this->line.unitVector();
    qDebug() << "Line" << id << "unitary:" << unit_line;
    this->unitary = QVector2D(unit_line.dx(),unit_line.dy()).normalized(); // ! normalized !
    qDebug() << "Wall" << id << "unitary:" << this->unitary;
    qDebug("Setting Wall graphics line...");
    this->graphics->setLine(graphics_line);

    QPen pen(Qt::gray);
    pen.setWidth(3);

    switch (this->material) {
    case BrickWall:
        pen.setColor(Qt::darkRed);
        this->properties.relative_permittivity = 3.95;
        this->properties.conductivity = 0.073;
        break;
    case Window:
        pen.setColor(Qt::cyan);
        this->properties.relative_permittivity = 6.3919;
        this->properties.conductivity = 0.0107;
        break;
    case MetalWall:
        pen.setColor(Qt::gray);
        this->properties.relative_permittivity = 1;
        this->properties.conductivity = 10e7;
        break;
    case DryWall:
        pen.setColor(Qt::lightGray);
        this->properties.relative_permittivity = 2.7;
        this->properties.conductivity = 0.05349;
        break;
    case ConcreteWall:
        pen.setColor(Qt::green);
        this->properties.relative_permittivity = 6.4954;
        this->properties.conductivity = 1.43;
        break;
    default:
        pen.setColor(Qt::white);
        this->properties.relative_permittivity = 0;
        this->properties.conductivity = 0;
        break;
    }
    this->graphics->setPen(pen);

    this->properties.epsilon = epsilon_0 * this->properties.relative_permittivity;
    this->properties.Z_m = sqrt(mu_0 / (this->properties.epsilon - j * this->properties.conductivity / omega));
    this->properties.alpha_m = 1;
    this->properties.beta_m = 1;
    this->properties.gamma_m = this->properties.alpha_m +j*this->properties.beta_m;

    qDebug("Wall created.");
}

ObstacleType Obstacle::getMaterial()
{
    return this->material;
}

/*
QPointF Obstacle::getStartCoordinates() const
{
    return this->start_coordinates;
}

QPointF Obstacle::getEndCoordinates() const
{
    return this->end_coordinates;
}

qreal Obstacle::getRelativePermittivity() const
{
    return this->properties.relative_permittivity;
}

qreal Obstacle::getConductivity() const
{
    return this->properties.conductivity;
}
*/
