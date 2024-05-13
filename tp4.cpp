// Des imports, je ne sais pas lesquels sont nécessaires
// et lesquels ne le sont pas, j'ai pas fait le tri
// de toute façon quand on mergera ça dans le vrai code
// on fera la distinction
#include <QCoreApplication>
#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QPen>
#include <QBrush>
#include <cmath>
#include <complex>
#include <QVector2D>
#include <QElapsedTimer>

// Defines for debugging:
#define DRAW_RAYS_TP4 1
//#define DRAW_IMAGES_TP4 1
#define DRAW_REFLECTION_POINTS_TP4 1

// pour plus de simplicité
using namespace std;

// constants
constexpr qreal epsilon_0 = 8.8541878128e-12;
constexpr qreal mu_0 = 4 * M_PI * 1e-7;
constexpr qreal freq = 868.3e6; // 868.3 MHz
constexpr qreal c = 299792458;

constexpr qreal G_TXP_TX = 1.64e-3; // TODO : régler la problématique de sa valeur?
constexpr qreal beta_0 =  2*M_PI*freq/c; // beta

// il n'est pas défini en constexpr car cela provoque une erreur, sans doute parce qu'il utilise une fonction sqrt()
const qreal Z_0 = sqrt(mu_0 / epsilon_0); // impédance du vide // vacuum impedance
//constexpr double epsilon_r = 4.8;
//constexpr double sigma = 0.018; // conductivité (S/m)
//constexpr double thickness = 0.15; // épaisseur des murs, en mètres
constexpr qreal omega = 2 * M_PI * freq;
constexpr qreal lambda = c / freq;
constexpr complex<qreal> j(0, 1); // ! définition de j, useful

//constexpr complex<double> epsilon = epsilon_0 * epsilon_r;
//const complex<double> Z_m = sqrt(mu_0 / (epsilon - j * sigma / omega));
//const complex<double> gamma_m = sqrt(j * omega * mu_0 * (sigma + j * omega * epsilon)); // gamma_m = alpha_m + j*beta_m
constexpr qreal Ra = 73; // résistance d'antenne en Ohm

////// positions des objets
////const QVector2D RX(47, 65);
////const QVector2D TX(32, 10);

//constexpr QVector2D normal(1, 0);
//constexpr QVector2D unitary(0,1);
//// they should be normalized!!!
//constexpr QVector2D normal_top(0,-1); // mur du haut
//constexpr QVector2D unitary_top(1,0); // mur du haut

// Total Receiver power
qreal P_RX_TOTAL = 0;

QBrush rxBrush(Qt::blue);
QPen rxPen(Qt::darkBlue);
QBrush txBrush(Qt::white);
QPen txPen(Qt::darkGray);
QPen dVectorPen(Qt::green);
//dVectorPen.setWidth(1); // 2?
QPen wallPen(Qt::gray);
//wallPen.setWidth(4);
QPen imagePen(Qt::PenStyle::DotLine);

void init() {
    // initialize program stuff
    dVectorPen.setWidth(1); // 2?
    wallPen.setWidth(3);
    imagePen.setWidth(1);
    imagePen.setColor(Qt::darkGray);
}

//0, -20, 130, -20
QVector2D wall1start(0, 20);
QVector2D wall1end(130, 20);
//0, 0, 0, -80
QVector2D wall2start(0,0);
QVector2D wall2end(0, 80);
//0, -80, 130, -80
QVector2D wall3start(0, 80);
QVector2D wall3end(130, 80);

struct Wall {
    QGraphicsLineItem* graphics = new QGraphicsLineItem(); // wall's QGraphicsItem
    QLineF line; // wall's QLineF, for intersects()
    QVector2D normal; // wall's normal vector ! normalized !
    QVector2D unitary; // wall's unitary vector ! normalized !
    int id; // only used for debugging
    qreal thickness; // in meters

    Wall(QVector2D start, QVector2D end, qreal thickness, int id){
        // Wall object cosntructor
        qDebug("Creating wall...");
        this->id = id;
        this->thickness = thickness;
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
        wallPen.setWidth(3);
        this->graphics->setPen(wallPen);
        qDebug("Wall created.");
    }

    qreal sigma = 0.018; // wall's conductivity (S/m)
    qreal epsilon_r = 4.8; // wall's relative permittivity
    qreal epsilon = epsilon_0 * epsilon_r; // wall's permittivity
    complex<qreal> gamma_m = sqrt(j * omega * mu_0 * (sigma + j * omega * epsilon)); // gamma_m = alpha_m + j*beta_m
    complex<qreal> Z_m = sqrt(mu_0 / (epsilon - j * sigma / omega)); // wall's impedance
    qreal beta_m = omega*sqrt((mu_0*this->epsilon)/2)*pow(sqrt(1+pow((this->sigma/(omega*this->epsilon)),2))+1,1/2);
    qreal alpha_m = omega*sqrt((mu_0*this->epsilon)/2)*pow(sqrt(1+pow((this->sigma/(omega*this->epsilon)),2))-1,1/2);
    //complex<qreal> gamma_m = this->alpha_m + j*this->beta_m;
};

QList<Wall*> wall_list = { // list containing all the created walls
    new Wall(wall1start,wall1end, 0.15, 1),
    new Wall(wall2start,wall2end, 0.15, 2),
    new Wall(wall3start,wall3end, 0.15, 3),
};

class RaySegmentTP4 : public QLineF { // Finite segment of a ray
public:
    qreal distance; //? not used ?
    QGraphicsLineItem* graphics = new QGraphicsLineItem(); // segment's QGraphicsItem
    RaySegmentTP4(qreal start_x, qreal start_y, qreal end_x, qreal end_y){
        // RaySegmentTP4 object constructor
        this->setLine(start_x, start_y, end_x, end_y);
        this->graphics->setLine(start_x, -start_y, end_x, -end_y);
        this->distance = this->length();
    }
};

class RayTP4 { // RayTP4, made of one or multiple ray segments
public:
    RayTP4(QPointF start, QPointF end) {
        // RayTP4 object constructor
        this->start=start;
        this->end=end;
    }
    QList<RaySegmentTP4*> segments; // list of this ray's segment(s)
    int num_reflections = 0;
    QPointF start; // ray's starting point (TX)
    QPointF end; // ray's end point (RX)
    QList<complex<qreal>> coeffsList; // list of ray's transmission and reflection coefficients
    qreal totalCoeffs=1; // product of all of the ray's coefficients (|T_1|^2 * |G_1|^2 * ...)
    qreal distance=1e10; //? not used ? // total ray's distance (default is 1e10 instead of 0 to avoid inf power)

    void addCoeff(qreal coeff_module) {
        // add a Transmission or Reflection coefficient to this ray's list of coeffs
        qDebug() << "Adding coeff to ray:" << coeff_module;
        // which one to use ? :
        this->coeffsList.append(coeff_module);
        this->totalCoeffs*=pow(coeff_module,2);
    }

    qreal getTotalCoeffs() {
        // returns the total product of all of the ray's coefficients multiplied by the exponent term
        qreal res = totalCoeffs;
        res *= pow(abs(exp(-j*beta_0*this->distance)/this->distance),2); // exp term
        qDebug() << "getTotalCoeffs ray:" << res;
        return res;

        //qreal res = 0;
        //for (complex<qreal> coeff : this->coeffsList) {
        //    res*=pow(abs(coeff),2); // all coeffs
        //}
        //res*=pow(abs(exp(-j*beta_0*this->distance)/this->distance),2); // exp term
        //qDebug() << "computeAllCoeffs:" << res;
        //return res;

    }

    QList<QGraphicsLineItem*> getSegmentsGraphics(){
        // returns this ray's graphics: list of its segment's QGraphicsItems
        qDebug() << "Getting ray segments graphics";
        QPen ray_pen;
        ray_pen.setWidth(1);
        // set ray graphics color depending on number of reflections
        qDebug() << "This ray has" << this->segments.length() << "segments, so" << this->segments.length()-1 << "reflections";
        this->num_reflections=this->segments.length()-1;
        switch (this->num_reflections){
        case 0: // no reflections: direct ray
            qDebug() << "This ray is direct";
            ray_pen.setColor(Qt::green);
            break;
        case 1: // 1 reflection
            qDebug() << "This ray has 1 reflection";
            ray_pen.setColor(Qt::red);
            break;
        case 2: // 2 reflection
            qDebug() << "This ray has 2 reflections";
            ray_pen.setColor(Qt::yellow);
            break;
        }
        QList<QGraphicsLineItem*> ray_graphics;
        for (RaySegmentTP4* ray_segment: this->segments) {
        //for (int i=0; i<this->segments.length(); i++) {
            //qDebug() << "Adding this segment to list ray_graphics";
            ray_segment->graphics->setPen(ray_pen);
            //qDebug() << "before" << ray_segment->graphics;
            ray_graphics.append(ray_segment->graphics);
            //qDebug() << "after:" << ray_graphics;
        }
        return ray_graphics;
    }

    qreal getTotalDistance() const {
        // returns the total distance of this ray (which is the either the sum of the distances of each segments,
        // or the distance between the last image and RX)

        // TODO: ?
        //qreal d = 0;
        //for (RaySegment* segment : this->segments) {
        //    d += segment->distance;
        //}
        //return d;
        qDebug() << "Ray getTotalDisctance:" << this->distance;

        return this->distance;
    }
};
QList<RayTP4*> all_rays; // list in which all computed rays will be added

class TransmitterTP4 : public QVector2D { // TX class
public:
    TransmitterTP4(qreal x, qreal y){
        // TransmitterTP4 object constructor
        this->setX(x);
        this->setY(y);
        this->graphics->setToolTip(QString("Test transmitter x=%1 y=%2").arg(this->x(),this->y()));
        this->graphics->setBrush(txBrush);
        this->graphics->setPen(txPen);
        this->graphics->setRect(x-3,-y-3,6,6);
        this->graphics->setAcceptHoverEvents(true);
    };
    /* // uses parent class implementation
    qreal x() const{
        return this->QPointF::x();
    }
    qreal y() const{
        return this->QPointF::y();
    }
    */
    QGraphicsEllipseItem* graphics = new QGraphicsEllipseItem(); // TX's QGraphicsItem
    qreal power; // ! in Watts
};
class ReceiverTP4 : public QVector2D { // RX class
public:
    ReceiverTP4(qreal x, qreal y) {
        // ReceiverTP4 object constructor
        this->setX(x);
        this->setY(y);
        this->graphics->setToolTip(QString("Test receiver x=%1 y=%2").arg(this->x(),this->y()));
        this->graphics->setBrush(rxBrush);
        this->graphics->setPen(rxPen);
        this->graphics->setRect(x-3,-y-3,6,6);
        this->graphics->setAcceptHoverEvents(true);
    }
    QGraphicsRectItem* graphics = new QGraphicsRectItem(); // RX's QGraphicsItem
    qreal power; // ! in Watts
};
TransmitterTP4 TX(32,10); // initialized global TX
ReceiverTP4 RX(47, 65); // initialized global RX
// Debug TEST, reverse positions :
//ReceiverTP4 RX(32,10); // initialized global TX
//TransmitterTP4 TX(47, 65); // initialized global RX

QList<QGraphicsEllipseItem*> tx_images; // used for debugging: list of images graphics
QList<QGraphicsEllipseItem*> reflection_points; // used for debugging: list of reflection points graphics

// for debug:
QGraphicsEllipseItem* tx_image_graphics = new QGraphicsEllipseItem(); // used for debugging - old
QGraphicsEllipseItem* tx_image_image_graphics = new QGraphicsEllipseItem(); // used for debugging - old

qreal computeTotalPower() // returns final total power computation for this RX
{
    qreal res = 0;
    for (RayTP4* ray : all_rays) {
        res+=ray->getTotalCoeffs(); // sum of all the rays' total coefficients and exp term
    }
    // multiply by the term before the sum:
    res *= (60*pow(lambda,2))/(8*pow(M_PI,2)*Ra)*G_TXP_TX; // TODO: *transmitter->gain*transmitter->power plutot que *G_TXP_TX

    qDebug() << "computeTotalPower:" << res;
    return res;
}

// fonction qui calcule la position de \vec r_image de l'antenne
//QPointF computeImageTX(const QPointF& TX, const QPointF& normal) {
QVector2D computeImage(const QVector2D& _point, Wall* wall) {
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

// calcul des composants issu d'une réflexion
void addReflectionComponents(QGraphicsScene* scene, const QVector2D& RX, const QVector2D& TX, const QVector2D& r_image, const QVector2D& r_image_image) {
    //? FIXME: not used anymore?
    Wall* wall = wall_list[1];
    // vecteur d entre l'image et la RX
    QVector2D d = RX - r_image;
    // vecteur origine TODO : est-ce qu'il est tjr à (0,0) ?
    QVector2D x0(0, 0);  // Origine
    // calcul du t, forme générale
    double t = ((d.y() * (r_image.x() - x0.x())) - (d.x() * (r_image.y() - x0.y()))) / (wall->unitary.x() * d.y() - wall->unitary.y() * d.x());
    QVector2D P_r = x0 + t * wall->unitary;
    double d_norm = sqrt(pow(d.x(), 2) + pow(d.y(), 2));

    // paramètres de la réflexion
    double cos_theta_i = QVector2D::dotProduct(d/d_norm, wall->normal);
    qDebug() << "cos_theta_i" << cos_theta_i;
    double sin_theta_i = sqrt(1 - pow(cos_theta_i, 2));
    qDebug() << "sin_theta_i" << sin_theta_i;
    double sin_theta_t = sin_theta_i / sqrt(wall->epsilon_r);
    qDebug() << "sin_theta_t" << sin_theta_t;
    double cos_theta_t = sqrt(1 - pow(sin_theta_t, 2));
    qDebug() << "cos_theta_t" << cos_theta_t;
    double s = wall->thickness / cos_theta_t;
    qDebug() << "s" << s;
    complex<double> Gamma_perpendicular = (wall->Z_m * cos_theta_i - Z_0 * cos_theta_t) / (wall->Z_m * cos_theta_i + Z_0 * cos_theta_t);
    qDebug() << "Gamma_perpendicular:" << QString::number(Gamma_perpendicular.real()) << "+ j" << QString::number(Gamma_perpendicular.imag());
    // TODO : valeur un petit peu différente de la valeur attendue, pourquoi ?
    complex<double> reflection_term = exp(-2.0 * wall->gamma_m * s) * exp(j * 2.0 * beta_0 * s * sin_theta_t * sin_theta_i);
    qDebug() << "reflection_term:" << QString::number(reflection_term.real()) << "+ j" << QString::number(reflection_term.imag());
    complex<double> Gamma_m = Gamma_perpendicular - (1.0 - pow((Gamma_perpendicular), 2)) * Gamma_perpendicular * reflection_term / (1.0 - pow((Gamma_perpendicular), 2) * reflection_term);
    qDebug() << "Gamma_m:" << QString::number(Gamma_m.real()) << "+ j" << QString::number(Gamma_m.imag());
    // interface graphique, rien de fou, merci gpt pour la syntaxe ici
    QPen reflectionPen(Qt::red);
    reflectionPen.setWidth(1); // 2?
    scene->addLine(TX.x(), -TX.y(), P_r.x(), -P_r.y(), reflectionPen);
    scene->addLine(P_r.x(), -P_r.y(), RX.x(), -RX.y(), reflectionPen);


    // ---- TEST: deuxieme reflection ----

    Wall* wall_2 = wall_list[2];
    // vecteur d entre l'image et la RX
    QVector2D d_2 = RX - r_image_image;
    // vecteur origine TODO : est-ce qu'il est tjr à (0,0) ?
    //x0(0, -80);  // Nouvelle origine
    // calcul du t, forme générale
    double t_2 = ((d_2.y() * (r_image_image.x() - x0.x())) - (d_2.x() * (r_image_image.y() - x0.y()))) / (wall_2->unitary.x() * d_2.y() - wall_2->unitary.y() * d_2.x());
    QVector2D P_r_2 = x0 + t_2 * wall_2->unitary;
    double d_norm_2 = sqrt(pow(d_2.x(), 2) + pow(d_2.y(), 2));
    // paramètres de la réflexion
    double cos_theta_i_2 = QVector2D::dotProduct(d_2/d_norm_2, wall_2->normal);
    double sin_theta_i_2 = sqrt(1 - pow(cos_theta_i_2, 2));
    double sin_theta_t_2 = sin_theta_i_2 / sqrt(wall_2->epsilon_r);
    double cos_theta_t_2 = sqrt(1 - pow(sin_theta_t_2, 2));
    double s_2 = wall_2->thickness / cos_theta_t_2;
    complex<double> Gamma_perpendicular_2 = (wall_2->Z_m * cos_theta_i_2 - Z_0 * cos_theta_t_2) / (wall_2->Z_m * cos_theta_i_2 + Z_0 * cos_theta_t_2);
    // TODO : valeur un petit peu différente de la valeur attendue, pourquoi ?
    complex<double> reflection_term_2 = exp(-2.0 * wall_2->gamma_m * s_2) * exp(j * 2.0 * beta_0 * s_2 * sin_theta_t_2 * sin_theta_i_2);
    complex<double> Gamma_m_2 = Gamma_perpendicular_2 - (1.0 - pow((Gamma_perpendicular_2), 2)) * Gamma_perpendicular_2 * reflection_term_2 / (1.0 - pow((Gamma_perpendicular_2), 2) * reflection_term_2);
    // interface graphique, rien de fou, merci gpt pour la syntaxe ici
    QPen reflectionPen_2(Qt::yellow);
    reflectionPen_2.setWidth(1); // 2?
    scene->addLine(TX.x(), -TX.y(), P_r_2.x(), -P_r_2.y(), reflectionPen_2);
    scene->addLine(P_r_2.x(), -P_r_2.y(), RX.x(), -RX.y(), reflectionPen_2);

    // -----------------------------------


    // to debug:
    scene->addLine(TX.x(), -TX.y(), r_image.x(), -r_image.y(), imagePen);
    scene->addLine(r_image.x(), -r_image.y(), P_r.x(), -P_r.y(), imagePen);

}
// calculer le point de réflexion sur un mur, même chose que dans le tp
QVector2D calculateReflectionPoint(const QVector2D& _start, const QVector2D& _end, Wall* wall) {
    // returns the intersection bewteen the line from _start to _end and the wall
    QVector2D d = _end-_start;
    //QVector2D x0(0,0); // TODO: always this ?
    QVector2D x0 = QVector2D(wall->line.p1()); // TODO: FIXME: correct ?
    qreal t = ((d.y()*(_start.x()-x0.x()))-(d.x()*(_start.y()-x0.y()))) / (wall->unitary.x()*d.y()-wall->unitary.y()*d.x());
    QVector2D P_r = x0 + t * wall->unitary;
    return P_r;
}

void drawAllRays(QGraphicsScene* scene) {
    // Adds all rays in all_rays QList to the scene
    for (RayTP4* ray : all_rays) {
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

// scène graphique, encore une fois merci gpt pour la syntaxe
//QGraphicsScene* createGraphicsScene(const QVector2D& RX, const QVector2D& TX) {
QGraphicsScene* createGraphicsScene(ReceiverTP4& RX, TransmitterTP4& TX) {
    // creates the QGraphicsScene (to give to QGraphicsView) and adds all graphics to it

    auto* scene = new QGraphicsScene();

    // Draw all walls in wall_list
    for (Wall* wall : wall_list){
        qDebug() << "Adding wall to scene...";
        scene->addItem(wall->graphics);
    }
    qDebug() << "All walls added to scene.";

#ifdef DRAW_RAYS_TP4
    // Draw all rays (their segments) from the all_rays list
    drawAllRays(scene);
#endif

    // compute total power and set it in RX
    qreal totalPower = computeTotalPower();
    RX.power = totalPower;

    // Draw RX and add its tooltip
    float _rx_power_dBm = 10*std::log10(RX.power*1000); // TODO: correct ? *1000 because is in Watts and need in mW :
    RX.graphics->setToolTip(QString("Test receiver\nx=%1 y=%2\nPower: %3 mW | %4 dBm").arg(QString::number(RX.x()),QString::number(RX.y()),QString::number(RX.power*1000),QString::number(_rx_power_dBm,'f',2)));
    scene->addItem(RX.graphics);
    qDebug() << "RX.graphics:" << RX.graphics->rect();

    // Draw TX and add its tooltip
    TX.graphics->setToolTip(QString("Test transmitter\nx=%1 y=%2\nG_TX*P_TX=%3").arg(QString::number(TX.x()),QString::number(TX.y()),QString::number(G_TXP_TX)));
    scene->addItem(TX.graphics);
    qDebug() << "TX.graphics:" << TX.graphics->rect();

#ifdef DRAW_IMAGES_TP4
    // For debugging: draw all images from the tx_images list
    for (QGraphicsEllipseItem* image_graphics : tx_images) {
        image_graphics->setPen(QPen(Qt::darkYellow));
        image_graphics->setBrush(QBrush(Qt::darkYellow));
        image_graphics->setToolTip(QString("image\nx=%1 y=%2").arg(QString::number(image_graphics->rect().x()+image_graphics->rect().width()/2),QString::number(-image_graphics->rect().y()-image_graphics->rect().height()/2)));
        scene->addItem(image_graphics);
    }
#endif

#ifdef DRAW_REFLECTION_POINTS_TP4
    // For debugging: draw all reflection points from the reflection_points list
    for (QGraphicsEllipseItem* reflection_graphics : reflection_points) {
        reflection_graphics->setPen(QPen(Qt::magenta));
        reflection_graphics->setBrush(QBrush(Qt::magenta));
        reflection_graphics->setToolTip(QString("reflection\nx=%1 y=%2").arg(QString::number(reflection_graphics->rect().x()+reflection_graphics->rect().width()/2),QString::number(-reflection_graphics->rect().y()-reflection_graphics->rect().height()/2)));
        scene->addItem(reflection_graphics);
    }
#endif

    scene->setBackgroundBrush(QBrush(Qt::black)); // black background

    return scene;
}

complex<qreal> computePerpendicularGamma(qreal _cos_theta_i, qreal _cos_theta_t, Wall* wall)
{
    // returns Gamma_perpendicular
    complex<qreal> Gamma_perp = (wall->Z_m*_cos_theta_i-Z_0*_cos_theta_t)/(wall->Z_m*_cos_theta_i+Z_0*_cos_theta_t);

    qDebug() << "Gamma_perp=" << QString::number(Gamma_perp.real()) << "+j" << QString::number(Gamma_perp.imag());
    return Gamma_perp;
}

complex<qreal> computeReflectionCoeff(qreal _cos_theta_i, qreal _sin_theta_i, qreal _cos_theta_t, qreal _sin_theta_t, Wall* wall)
{
    // returns the reflection coefficient Gamma_m
    qreal s = wall->thickness/_cos_theta_t;
    complex<qreal> Gamma_perpendicular = computePerpendicularGamma(_cos_theta_i, _cos_theta_t, wall);
    complex<qreal> reflection_term = exp(-2.0 * wall->gamma_m * s) * exp(j * 2.0 * beta_0 * s * _sin_theta_t * _sin_theta_i);
    qDebug() << "reflection_term:" << QString::number(reflection_term.real()) << "+ j" << QString::number(reflection_term.imag());
    complex<qreal> Gamma_m = Gamma_perpendicular - (1.0 - pow((Gamma_perpendicular), 2)) * Gamma_perpendicular * reflection_term / (1.0 - pow((Gamma_perpendicular), 2) * reflection_term);
    qDebug() << "Gamma_m:" << QString::number(Gamma_m.real()) << "+ j" << QString::number(Gamma_m.imag());

    return Gamma_m;
}

complex<qreal> computeTransmissionCoeff(qreal _cos_theta_i, qreal _sin_theta_i, qreal _cos_theta_t, qreal _sin_theta_t, Wall* wall)
{
    // returns the transmission coefficient T_m
    qreal s = wall->thickness/_cos_theta_t;
    complex<qreal> perpGamma = computePerpendicularGamma(_cos_theta_i, _cos_theta_t, wall);
    complex<qreal> T_m = ((1.0-pow(perpGamma,2))*exp(-(wall->gamma_m)*s))/(1.0-(pow(perpGamma,2)*exp(-2.0*(wall->gamma_m)*s)*exp(j*beta_0*2.0*s*_sin_theta_t*_sin_theta_i)));

    qDebug() << "TransmissionCoeff=" << QString::number(T_m.real()) << "+j" << QString::number(T_m.imag());
    return T_m;
}

bool checkSameSideOfWall(const QVector2D& _normal, const QVector2D& _TX, const QVector2D& _RX) {
    // returns true if _TX and _RX are on the same side of the wall (using the wall's normal vector)
    // must be same sign to be true:
    bool res = (QVector2D::dotProduct(_normal,_RX)>0 == QVector2D::dotProduct(_normal,_TX)>0);
    return res;
}

bool checkRaySegmentIntersectsWall(const Wall* wall, RaySegmentTP4* ray_segment, QPointF* intersection_point=nullptr) {
    // returns true if ray_segment intersects wall
    // the intersection_point pointer's value is set wit hthe intersection point coordinates if they intersect
    int _intersection_type = ray_segment->intersects(wall->line, intersection_point); // also writes to intersection pointer the QPointF
    bool intersects_wall = _intersection_type==1 ? true: false; //0: no intersection (parallel), 1: intersects directly the line segment, 2: intersects the infinite extension of the line
    return intersects_wall;
}

qreal makeTransmission(RaySegmentTP4* ray_segment, Wall* wall) {
    // computes the final |T| coeff for the ray_segment's transmission with this wall
    QVector2D _eta = QVector2D(ray_segment->p1())-QVector2D(ray_segment->p2());
    qreal _cos_theta_i = abs(QVector2D::dotProduct(_eta.normalized(),wall->unitary));
    qreal _sin_theta_i = abs(QVector2D::dotProduct(_eta.normalized(),wall->normal));
    qreal _sin_theta_t = _sin_theta_i / sqrt(wall->epsilon_r);
    qreal _cos_theta_t = sqrt(1 - pow(_sin_theta_t,2));
    //qreal s = wall->thickness/_cos_theta_t;
    qreal T_coeff = abs(computeTransmissionCoeff(_cos_theta_i,_sin_theta_i,_cos_theta_t,_sin_theta_t,wall));
    return T_coeff;
}

void checkTransmissions(RayTP4* _ray, QList<Wall*> _reflection_walls) {
    // checks for every segment in this ray if they intersect a wall (which isn't a wall already used for a reflection by this ray)
    // if so: adds the Transmission coefficient to this ray's coeffs list
    for (RaySegmentTP4* ray_segment : _ray->segments) {
        for (Wall* wall : wall_list) {
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

void addReflection(RayTP4* _ray, const QVector2D& _p1, const QVector2D& _p2, Wall* wall){
    // computes the final |Gamma| coeff for the ray_segment's reflection with this wall, and adds it to this ray's coeffs list
    QVector2D _d = _p2-_p1;
    qreal _cos_theta_i = abs(QVector2D::dotProduct(_d.normalized(),wall->normal));
    qreal _sin_theta_i = abs(QVector2D::dotProduct(_d.normalized(),wall->unitary)); // sqrt(1 - pow(_cos_theta_i,2))
    qreal _sin_theta_t = _sin_theta_i / sqrt(wall->epsilon_r);
    qreal _cos_theta_t = sqrt(1 - pow(_sin_theta_t,2));
    qreal Gamma_coeff = abs(computeReflectionCoeff(_cos_theta_i,_sin_theta_i,_cos_theta_t,_sin_theta_i, wall));
    qDebug() << "addReflection, Gamma_coeff:" << Gamma_coeff;
    _ray->addCoeff(Gamma_coeff);
}

void computeReflections(const QVector2D& _RX, const QVector2D& _TX)
{
    // Makes the whole reflections computation, summary:
    // For each wall, check if TX (or its image, for the 2nd reflection) and RX are on the same
    // side of the wall, if so computes TX's (or its image's) image with this wall, then computes
    // the reflection point which is the intersection of the line between the image and RX and the
    // wall. Creates the segments from TX to each reflection point to finally RX and creates a new
    // Ray object made of these segments.

    // 1st reflection :
    //for (Wall* wall: wall_list) { // could use this instead
    for (int i=0; i<wall_list.length(); i++) {
        Wall* wall = wall_list[i];

        // check if same side of wall, if false, then no reflection only transmission
        if (checkSameSideOfWall(wall->normal,_TX,_RX)) {
            //same side of this wall, can make a reflection
            qDebug() << "Same side of wall TX and RX:" << wall << _TX.toPointF() << _RX.toPointF() ;
            RayTP4* ray_1_reflection = new RayTP4(_TX.toPointF(), _RX.toPointF());

            QVector2D _imageTX = computeImage(_TX, wall);
            qDebug() << "_image:" << _imageTX.x() << _imageTX.y();

            QVector2D _P_r = calculateReflectionPoint(_imageTX,_RX,wall);

            // CHECK IF REFLECTION IS ON THE WALL AND NOT ITS EXTENSION:
            RaySegmentTP4* test_segment = new RaySegmentTP4(_imageTX.x(),_imageTX.y(),_RX.x(),_RX.y());
            if (!checkRaySegmentIntersectsWall(wall, test_segment)) {
                // RAY DOES NOT TRULY INTERSECT THE WALL (only the wall extension) ignore this reflection at this wall
                qDebug() << "ignore";
                delete ray_1_reflection;
                delete test_segment;
                continue; // break out of this forloop instance for this wall
            }
            delete test_segment;

            qDebug() << "P_r" << _P_r;
            tx_images.append(new QGraphicsEllipseItem(_imageTX.x()-2, -_imageTX.y()-2, 4, 4));
            reflection_points.append(new QGraphicsEllipseItem(_P_r.x()-1, -_P_r.y()-1, 2, 2));

            // create ray segments between points
            QList<RaySegmentTP4*> ray_segments;
            ray_segments.append(new RaySegmentTP4(_TX.x(),_TX.y(),_P_r.x(),_P_r.y())); // first segment
            ray_segments.append(new RaySegmentTP4(_P_r.x(),_P_r.y(),_RX.x(),_RX.y())); // last segment

            ray_1_reflection->segments = ray_segments;
            addReflection(ray_1_reflection,_imageTX,_RX,wall);
            checkTransmissions(ray_1_reflection,{wall});

            qDebug() << "ray_1_refl distance:" << QVector2D(_RX - _imageTX).length();
            ray_1_reflection->distance = QVector2D(_RX-_imageTX).length();
            qDebug() << "Ray's (1refl) total coeffs:" << ray_1_reflection->getTotalCoeffs();
            all_rays.append(ray_1_reflection);

            // 2nd reflection
            for (Wall* wall_2 : wall_list) {
                // check that the second wall is not the same as the first wall and that imageTX and RX are at the same side of this second wall
                if (wall_2 != wall && checkSameSideOfWall(wall_2->normal,_imageTX,_RX)) {
                    qDebug() << "Same side of wall imageTX and RX --- wall_2:" << wall_2->line.p1() << wall_2->line.p2() << ", imageTX:" << _imageTX.toPointF() << ", RX:" << _RX.toPointF() ;
                    RayTP4* ray_2_reflection = new RayTP4(_TX.toPointF(),_RX.toPointF());

                    QVector2D _image_imageTX = computeImage(_imageTX,wall_2);
                    qDebug() << "_image_image:" << _image_imageTX.x() << _image_imageTX.y();

                    QVector2D _P_r_2_last = calculateReflectionPoint(_image_imageTX,_RX,wall_2);
                    QVector2D _P_r_2_first = calculateReflectionPoint(_imageTX,_P_r_2_last,wall);
                    if (_P_r_2_last.x()==_P_r_2_first.x() && _P_r_2_last.y()==_P_r_2_first.y()) {
                        qDebug() << "------> P_r_2_last = P_r_2_first !!!)";
                    }

                    RaySegmentTP4* test_segment_1 = new RaySegmentTP4(_image_imageTX.x(),_image_imageTX.y(),_RX.x(),_RX.y());
                    RaySegmentTP4* test_segment_2 = new RaySegmentTP4(_imageTX.x(),_imageTX.y(),_P_r_2_last.x(),_P_r_2_last.y());
                    if (!checkRaySegmentIntersectsWall(wall_2, test_segment_1) || !checkRaySegmentIntersectsWall(wall,test_segment_2)) {
                        qDebug() << "ignore";
                        delete ray_2_reflection;
                        // RAY DOES NOT TRULY INTERSECT THE WALL (only the wall extension) ignore this reflection at this wall
                        delete test_segment_1;
                        delete test_segment_2;
                        continue; // break out of this forloop instance for this wall
                    }
                    delete test_segment_1;
                    delete test_segment_2;

                    qDebug() << "P_r_2_first" << _P_r_2_first;
                    qDebug() << "P_r_2_last" << _P_r_2_last;
                    tx_images.append(new QGraphicsEllipseItem(_image_imageTX.x()-2, -_image_imageTX.y()-2, 4, 4));
                    reflection_points.append(new QGraphicsEllipseItem(_P_r_2_last.x()-1, -_P_r_2_last.y()-1, 2, 2));
                    reflection_points.append(new QGraphicsEllipseItem(_P_r_2_first.x()-1, -_P_r_2_first.y()-1, 2, 2));

                    QList<RaySegmentTP4*> ray_segments_2;
                    ray_segments_2.append(new RaySegmentTP4(_TX.x(),_TX.y(),_P_r_2_first.x(),_P_r_2_first.y()));
                    ray_segments_2.append(new RaySegmentTP4(_P_r_2_first.x(),_P_r_2_first.y(),_P_r_2_last.x(),_P_r_2_last.y()));
                    ray_segments_2.append(new RaySegmentTP4(_P_r_2_last.x(),_P_r_2_last.y(),_RX.x(),_RX.y()));

                    ray_2_reflection->segments = ray_segments_2;
                    addReflection(ray_2_reflection,_imageTX,_P_r_2_last,wall);
                    addReflection(ray_2_reflection,_image_imageTX,_RX,wall_2);
                    checkTransmissions(ray_2_reflection,{wall,wall_2});

                    qDebug() << "ray_2_refl distance:" << QVector2D(_RX - _image_imageTX).length();
                    ray_2_reflection->distance = QVector2D(_RX-_image_imageTX).length();
                    qDebug() << "Ray's (2refl) total coeffs:" << ray_2_reflection->getTotalCoeffs();
                    all_rays.append(ray_2_reflection);
                }
            }
        }
    }
}

void computeDirect(const QVector2D& _RX, const QVector2D& _TX)
{
    // Computes the direct ray: checks all walls between RX and TX and adds
    // their computed transmission coefficients to the direct ray list of coeffs
    RayTP4* direct_ray = new RayTP4(_TX.toPointF(), _RX.toPointF());
    RaySegmentTP4* _direct_line = new RaySegmentTP4(_RX.x(), _RX.y(), _TX.x(), _TX.y());
    for (Wall* wall : wall_list) {
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
    qDebug() << "ray_direct distance:" << QVector2D(_RX - _TX).length();
    direct_ray->distance = QVector2D(_RX-_TX).length();
    qDebug() << "Ray's (direct) total coeffs:" << direct_ray->getTotalCoeffs();
    direct_ray->segments = {_direct_line};
    all_rays.append(direct_ray);
}

/*
// --- not used ? ---
qreal computeCosTheta_i(const QVector2D& _unitary, const QVector2D& _d)
{
    qreal res = QVector2D::dotProduct(_unitary, _d.normalized());
    qDebug() << "cos(theta_i)=" << res;
    return res;
}
qreal computeSinTheta_i(const QVector2D& _normal, const QVector2D& _d)
{
    qreal res = QVector2D::dotProduct(_normal, _d.normalized());
    qDebug() << "sin(theta_i)=" << res;
    return res;
}
qreal computeSinTheta_t(qreal _sin_theta_i, qreal _epsilon_r)
{
    qreal res = _sin_theta_i / sqrt(_epsilon_r);
    qDebug() << "sin(theta_t)=" << res;
    return res;
}
qreal computeCosTheta_t(qreal _sin_theta_t)
{
    qreal res = sqrt(1 - pow(_sin_theta_t, 2));
    qDebug() << "cos(theta_t)=" << res;
    return res;
}
// ------------------
*/

void runTestOui1(QGraphicsScene* scene) { // ! TODO: old code when testing, remove ?
    QVector2D r_image = computeImage(TX, wall_list[1]);
    // test:
    tx_image_graphics->setRect(r_image.x()-3,-r_image.y()-3,6,6);

    // TODO : foutre ça dans une fonction dédiée, MAIS c'est pas urgent, ça se fera
    // naturellement quand on mettre ça dans l'interface
    // calculs de la transmission directe
    Wall* wall_d = wall_list[0];
    QVector2D d = RX - TX;

    // TODO: or use dot products? :
    //double cos_theta_i = abs(QVector2D::dotProduct(QVector2D(unitary), QVector2D(d).normalized()));
    double cos_theta_i = d.y() / sqrt(pow(d.x(), 2) + pow(d.y(), 2));
    qDebug() << "cos_theta_i" << cos_theta_i;
    // TODO: or use do products? :
    //double sin_theta_i = abs(QVector2D::dotProduct(QVector2D(normal),QVector2D(d).normalized()));
    double sin_theta_i = sqrt(1 - pow(cos_theta_i, 2));
    qDebug() << "sin_theta_i" << sin_theta_i;

    double sin_theta_t = sin_theta_i / sqrt(wall_d->epsilon_r);
    qDebug() << "sin_theta_t" << sin_theta_t;
    double cos_theta_t = sqrt(1 - pow(sin_theta_t, 2));
    qDebug() << "cos_theta_t" << cos_theta_t;

    double s = wall_d->thickness / cos_theta_t;

    complex<double> Gamma_perpendicular = (wall_d->Z_m * cos_theta_i - Z_0 * cos_theta_t) / (wall_d->Z_m * cos_theta_i + Z_0 * cos_theta_t);
    qDebug() << "Gamma_perp=" << Gamma_perpendicular.real() << "+j" << Gamma_perpendicular.imag();
    // découpe le calcul de T_m en numérateur et dénominateur parce qu'il est
    //  tarpin long, sa valeur est celle attendue yay !
    complex<double> numerator = (1.0 - pow(Gamma_perpendicular, 2)) * exp(-wall_d->gamma_m * s);
    complex<double> denominator = 1.0 - pow(Gamma_perpendicular, 2) * exp(-2.0 * wall_d->gamma_m * s) * exp(j * beta_0 * 2.0 * s * sin_theta_t * sin_theta_i);
    complex<double> T_m = numerator / denominator;
    double d1 = sqrt(pow(d.x(), 2) + pow(d.y(), 2)); // TODO foutre ça // TODO: d1 = d.length() comme c'est un QVector2D mtn
        //quelque part plus haut ptet ?
    // valeur de exp term non calculée solo dans le tp donc je sais pas quelle est sa valeur
    // mais E_1 a une valeur un peu différente d'attendu TODO : découvrir pourquoi et corriger
    qDebug() << "----T_m" << QString::number(T_m.real()) << "+ j" << QString::number(T_m.imag());
    qDebug() << "beta:" << beta_0;
    complex<double> exp_term = exp(-j * beta_0 * d1); // pr simplifier expression en dessous
    complex<double> E_1 = T_m * sqrt(60 * G_TXP_TX) * exp_term/ d1; // Convertir P_TX de dBm en Watts
    qDebug() << "|E_1|=" << abs(E_1);//sqrt(pow(E_1.real(),2)+pow(E_1.imag(),2));
    // valeur différente de celle attendue, logique car elle dépend de G_TXP_TX dont je suis
    //  pas sûr de la valeur et de E_1 dont la valeur est différente de celle attendue
    //  TODO : malgré cela, est-ce que la fonction est bien celle ci ou y a t il une erreur ?
    //         j'ai pas investigué outre mesure car on peut pas le déterminer numériquement
    //         vu que E_1 a déjà pas la bonne valeur
    double P_RX = (60 * pow(lambda, 2)) / (8 * pow(M_PI,2)*Ra) * G_TXP_TX * pow(abs(T_m*exp_term/d1), 2);
    qDebug() << "       P_RX" << P_RX;
    qDebug() << "       E_1" << real(E_1) << "+" << imag(E_1) <<"j";
    qDebug() << "       exp_term" << real(exp_term) << "+" << imag(exp_term) <<"j";

    // petit affichage graphique, syntaxe made in gpt

    //--- on a foutu ça en dernier dans main() ---
    ////
    ////QGraphicsScene* scene = createGraphicsScene(RX, TX);
    //////QGraphicsScene* scene = createGraphicsScene(RX);
    ////addReflectionComponents(scene, RX, TX, r_image);
    ////QGraphicsView* view = new QGraphicsView(scene);
    ////// ? :
    ////view->setAttribute( Qt::WA_AlwaysShowToolTips);
    ////
    //--------------------------------------------

    // calculs de paramètres pour la réflexion
    Wall* wall_r = wall_list[1];
    QVector2D P_r = calculateReflectionPoint(r_image, RX, wall_r); // reflection point P_r
    QVector2D eta = P_r - TX; // vecteur de P_r à RX, notation issue du tp
    double eta_norm = sqrt(pow(eta.x(), 2) + pow(eta.y(), 2));
    double cos_theta_i_reflected = abs(QVector2D::dotProduct(eta.normalized(), wall_r->unitary)); // or eta/eta_norm
    double sin_theta_i_reflected = sqrt(1 - pow(cos_theta_i_reflected, 2));
    double sin_theta_t_reflected = sin_theta_i_reflected / sqrt(wall_r->epsilon_r);
    double cos_theta_t_reflected = sqrt(1 - pow(sin_theta_t_reflected, 2));
    double s_reflected = wall_r->thickness / cos_theta_t_reflected;
    complex<double> Gamma_perpendicular_reflected = (wall_r->Z_m * cos_theta_i_reflected - Z_0 * cos_theta_t_reflected) /
                                                    (wall_r->Z_m * cos_theta_i_reflected + Z_0 * cos_theta_t_reflected);
    // encore une fois un coefficient slightly pas le même pélo TODO : nsm wsh
    // en tout cas la discussion est la même que pour le cas à transmission, voir le todo plus haut

    // TODO: ici en dessous on devrait pas utiliser s_reflected plutot que s et Gamma_perpendicular_reflected plutot que Gamma_perpendicular ?
    complex<double> T_m_r = ((1.0 - pow(Gamma_perpendicular_reflected, 2)) * exp(-wall_r->gamma_m * s_reflected)) / (1.0 - pow(Gamma_perpendicular_reflected, 2) * exp(-2.0 * wall_r->gamma_m * s_reflected) * exp(j * 2.0 * beta_0 * sin_theta_t_reflected * sin_theta_i_reflected));
    complex<double> E_2 = T_m_r * sqrt(60 * G_TXP_TX) * exp(-j * imag(wall_r->gamma_m) * eta_norm) / eta_norm; // this reflection coefficient ?
    double P_RX_reflected = (60 * pow(lambda, 2)) / (8 * M_PI) * G_TXP_TX * pow(abs(E_2), 2); // is this in mW ?
    qDebug() << "P_RX_reflected" << P_RX_reflected;


    // ---- TEST deuxieme reflection ---- // TODO: test
    Wall* wall_rr = wall_list[2];
    QVector2D r_image_image = computeImage(r_image, wall_rr);
    // test:
    tx_image_image_graphics->setRect(r_image_image.x()-3,-r_image_image.y()-3,6,6);

    //QVector2D P_r_2 = calculateReflectionPoint(r_image_image, RX, wall_rr->unitary); // second reflection point P_r_2
    //QVector2D eta_2 = P_r_2 - r_image;
    //double eta_2_norm = sqrt(pow(eta_2.x(),2) + pow(eta_2.y(),2));
    //double cos_theta_i_reflected_2 = abs(QVector2D::dotProduct(eta_2.normalized(), wall_rr->unitary)); // or eta/eta_norm
    //double sin_theta_i_reflected_2 = sqrt(1 - pow(cos_theta_i_reflected_2, 2));
    //double sin_theta_t_reflected_2 = sin_theta_i_reflected_2 / sqrt(wall_rr->epsilon_r);
    //double cos_theta_t_reflected_2 = sqrt(1 - pow(sin_theta_t_reflected_2, 2));
    //double s_reflected_2 = wall_rr->thickness / cos_theta_t_reflected_2;
    //complex<double> Gamma_perpendicular_reflected_2 = (wall_rr->Z_m * cos_theta_i_reflected_2 - Z_0 * cos_theta_t_reflected_2) /
    //                                                  (wall_rr->Z_m * cos_theta_i_reflected_2 + Z_0 * cos_theta_t_reflected_2);
    //// encore une fois un coefficient slightly pas le même pélo TODO : nsm wsh
    //// en tout cas la discussion est la même que pour le cas à transmission, voir le todo plus haut

    //// TODO: ici en dessous on devrait pas utiliser s_reflected plutot que s et Gamma_perpendicular_reflected plutot que Gamma_perpendicular ?
    //complex<double> T_m_r_2 = ((1.0 - pow(Gamma_perpendicular_reflected_2, 2)) * exp(-wall_rr->gamma_m * s_reflected_2)) / (1.0 - pow(Gamma_perpendicular_reflected_2, 2) * exp(-2.0 * wall_rr->gamma_m * s_reflected_2) * exp(j * 2.0 * beta_0 * sin_theta_t_reflected_2 * sin_theta_i_reflected_2));
    //complex<double> E_2_2 = T_m_r * sqrt(60 * G_TXP_TX) * exp(-j * imag(wall_rr->gamma_m) * eta_2_norm) / eta_2_norm; // this reflection coefficient ?
    //double P_RX_reflected_2 = (60 * pow(lambda, 2)) / (8 * M_PI) * G_TXP_TX * pow(abs(E_2_2), 2); // is this in mW ?
    //qDebug() << "P_RX_reflected_2" << P_RX_reflected_2;

    // -----------------------------


    P_RX_TOTAL = P_RX + P_RX_reflected; // TODO: correct ?
    qDebug() << "PX_RX_TOTAL ??" << P_RX_TOTAL;
    //RX.power = P_RX_TOTAL;

    addReflectionComponents(scene, RX, TX, r_image, r_image_image);
}


//int main(int argc, char *argv[]) {
QGraphicsView* runTP4(){
    //QApplication app(argc, argv);

    QElapsedTimer timer;
    timer.start(); // timer for both computation and drawing

    qDebug() << "RX: x" << QString::number(RX.x()) << " y" << QString::number(RX.y());
    qInfo() << "TX: x" << QString::number(TX.x()) << " y" << QString::number(TX.y());

    init(); // init function: set the QPens propterties, etc

    computeReflections(RX, TX); // makes the whole reflections rays computation and creation
    computeDirect(RX, TX); // makes the direct ray computation and creation

    QGraphicsScene* scene = createGraphicsScene(RX, TX); // create graphics scene and draws on it

    //runTestOui1(scene); // old code from Salman, has now been dispatched in multiple functions

    QGraphicsView* view = new QGraphicsView(scene); // create user's view showing the graphics scene

    view->setAttribute(Qt::WA_AlwaysShowToolTips); //? maybe necessary ?

    // TODO: find good values
#ifdef DRAW_IMAGES_TP4
    view->setFixedSize(500, 750);
    view->scale(2.6, 2.6);
#else
    view->setFixedSize(600, 400);
    view->scale(4, 4);
#endif
    view->setWindowIcon(QIcon(":/assets/icon.png"));
    view->show(); // shows the graphics scene to the user
    qDebug() << "Time elapsed:" << timer.elapsed() << "ms";
    qDebug() << "Total number of rays:" << QString::number(all_rays.length());
    qDebug() << "The receiver threshold is at -65 dBm";
    // RX.power *1000 because it's in Watts and we need in mW :
    qDebug() << QString((10*std::log10(RX.power*1000) >= -65) ? "-> Enough power:" : "-> Not enough power:") << 10*std::log10(RX.power*1000) << "dBm";

    // prints RX power values only for each ray
    for (RayTP4* ray : all_rays) {
        qreal coeffs = ray->getTotalCoeffs();
        qreal power = coeffs*(60*pow(lambda,2))/(8*pow(M_PI,2)*Ra)*G_TXP_TX;
        // power *1000 because it's in Watts and we need in mW :
        qDebug() << "Ray" << ray->num_reflections << "reflections, power:" << power*1000 << "mW " << 10*std::log10(power*1000/(1e-3)) << "dBm.";
    }

    ////? In real app:
    ////delete scene;
    ////delete view;

    return view;
    //return app.exec();
}
