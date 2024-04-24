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
//#include <iostream>
#include <QVector2D>

// pour plus de simplicité
using namespace std;
//using namespace Qt; // ?

// constantes
constexpr double epsilon_0 = 8.8541878128e-12;
constexpr double mu_0 = 4 * M_PI * 1e-7;
constexpr double freq = 868.3e6; // EX 4.1, it is 868.3 MHz
constexpr double c = 299792458;

constexpr double G_TXP_TX = 1.64e-3; // TODO : régler la problématique de sa valeur?

// il n'est pas défini en constexpr car cela provoque une erreur,
// je ne sais pas pourquoi
const double Z_0 = sqrt(mu_0 / epsilon_0); // impédance du vide // vacuum impedance
constexpr double epsilon_r = 4.8;
constexpr double sigma = 0.018; // conductivité (S/m)
constexpr double thickness = 0.15; // épaisseur des murs, en mètres
constexpr double omega = 2 * M_PI * freq;
constexpr double lambda = c / freq;
constexpr complex<double> j(0, 1); // ! définition de j, useful

constexpr complex<double> epsilon = epsilon_0 * epsilon_r;
const complex<double> Z_m = sqrt(mu_0 / (epsilon - j * sigma / omega));
const complex<double> gamma_m = sqrt(j * omega * mu_0 * (sigma + j * omega * epsilon)); // gamma_m = alpha_m + j*beta_m
constexpr double Ra = 73; // résistance d'antenne en Ohm

////// positions des objets
////const QVector2D RX(47, 65);
////const QVector2D TX(32, 10);

// TODO : incorporer les positions des murs, bien que pour l'instant
//  ça ne serve pas directement dans le calcul
constexpr QVector2D normal(1, 0);
constexpr QVector2D unitary(0,1);

constexpr QVector2D normal_top(0,-1); // mur du haut
constexpr QVector2D unitary_top(1,0); // mur du haut

// Total Receiver power
double P_RX_TOTAL = 0;

QBrush rxBrush(Qt::blue);
QPen rxPen(Qt::darkBlue);
QBrush txBrush(Qt::black);
QPen txPen(Qt::darkGray);
QPen dVectorPen(Qt::green);
//dVectorPen.setWidth(1); // 2?
QPen wallPen(Qt::gray);
//wallPen.setWidth(4);
QPen imagePen(Qt::PenStyle::DotLine);

//0, -20, 130, -20
QVector2D wall1start(0, 20);
QVector2D wall1end(130, 20);
//0, 0, 0, -80
QVector2D wall2start(0,0);
QVector2D wall2end(0, 80);
//0, -80, 130, -80
QVector2D wall3start(0, 80);
QVector2D wall3end(130, 80);

void init() {
    // initialize program stuff
    dVectorPen.setWidth(1); // 2?
    wallPen.setWidth(3);
    imagePen.setColor(Qt::black);
}

class TransmitterTest : public QVector2D {
public:
    TransmitterTest(qreal x, qreal y){
        this->setX(x);
        this->setY(y);
        this->graphics->setToolTip(QString("Test transmitter x=%1 y=%2").arg(this->x(),this->y()));
        this->graphics->setBrush(QBrush(Qt::black));
        this->graphics->setPen(QPen(Qt::darkGray));
        this->graphics->setRect(x-5,-y-5,10,10);
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
    QGraphicsEllipseItem* graphics = new QGraphicsEllipseItem();
    double power;
};
class ReceiverTest : public QVector2D {
public:
    ReceiverTest(qreal x, qreal y) {
        this->setX(x);
        this->setY(y);
        this->graphics->setToolTip(QString("Test receiver x=%1 y=%2").arg(this->x(),this->y()));
        this->graphics->setBrush(QBrush(Qt::blue));
        this->graphics->setPen(QPen(Qt::darkBlue));
        this->graphics->setRect(x-5,-y-5,10,10);
        this->graphics->setAcceptHoverEvents(true);
    }
    QGraphicsRectItem* graphics = new QGraphicsRectItem();//(this->x()) - 5, - (this->y()) - 5, 10, 10);
    double power;
};
TransmitterTest TX(32,10);
ReceiverTest RX(47, 65);

QGraphicsEllipseItem* tx_image_graphics = new QGraphicsEllipseItem();
QGraphicsEllipseItem* tx_image_image_graphics = new QGraphicsEllipseItem();


// fonction qui calcule la position de \vec r_image de l'antenne
//QPointF calculateImageAntenna(const QPointF& TX, const QPointF& normal) {
QVector2D calculateImageAntenna(const QVector2D& TX, const QVector2D& normal) {
    double _dotProduct = QVector2D::dotProduct(TX, normal);
    QVector2D r_image = TX - 2 * _dotProduct * normal;
    qDebug() << "r_image:" << r_image.x() << r_image.y();

    return r_image;
}

// calcul des composants issu d'une réflexion
void addReflectionComponents(QGraphicsScene* scene, const QVector2D& RX, const QVector2D& TX, const QVector2D& r_image, const QVector2D& r_image_image) {
    // vecteur d entre l'image et la RX
    QVector2D d = RX - r_image;
    // vecteur origine TODO : est-ce qu'il est tjr à (0,0) ?
    QVector2D x0(0, 0);  // Origine
    // calcul du t, forme générale
    double t = ((d.y() * (r_image.x() - x0.x())) - (d.x() * (r_image.y() - x0.y()))) / (unitary.x() * d.y() - unitary.y() * d.x());
    QVector2D P_r = x0 + t * unitary;
    double d_norm = sqrt(pow(d.x(), 2) + pow(d.y(), 2));

    // paramètres de la réflexion
    double cos_theta_i = QVector2D::dotProduct(d/d_norm, normal);
    qDebug() << "cos_theta_i" << cos_theta_i;
    double sin_theta_i = sqrt(1 - pow(cos_theta_i, 2));
    qDebug() << "sin_theta_i" << sin_theta_i;
    double sin_theta_t = sin_theta_i / sqrt(epsilon_r);
    qDebug() << "sin_theta_t" << sin_theta_t;
    double cos_theta_t = sqrt(1 - pow(sin_theta_t, 2));
    qDebug() << "cos_theta_t" << cos_theta_t;
    double s = thickness / cos_theta_t;
    qDebug() << "s" << s;
    complex<double> Gamma_perpendicular = (Z_m * cos_theta_i - Z_0 * cos_theta_t) / (Z_m * cos_theta_i + Z_0 * cos_theta_t);
    qDebug() << "Gamma_perpendicular:" << QString::number(Gamma_perpendicular.real()) << "+ j" << QString::number(Gamma_perpendicular.imag());
    // TODO : valeur un petit peu différente de la valeur attendue, pourquoi ?
    complex<double> reflection_term = exp(-2.0 * real(gamma_m) * s) * exp(j * 2.0 * (gamma_m) * s * sin_theta_t * sin_theta_i);
    qDebug() << "reflection_term:" << QString::number(reflection_term.real()) << "+ j" << QString::number(reflection_term.imag());
    complex<double> Gamma_m = Gamma_perpendicular - (1.0 - pow((Gamma_perpendicular), 2)) * Gamma_perpendicular * reflection_term / (1.0 - pow((Gamma_perpendicular), 2) * reflection_term);
    qDebug() << "Gamma_m:" << QString::number(Gamma_m.real()) << "+ j" << QString::number(Gamma_m.imag());
    // interface graphique, rien de fou, merci gpt pour la syntaxe ici
    QPen reflectionPen(Qt::red);
    reflectionPen.setWidth(1); // 2?
    scene->addLine(TX.x(), -TX.y(), P_r.x(), -P_r.y(), reflectionPen);
    scene->addLine(P_r.x(), -P_r.y(), RX.x(), -RX.y(), reflectionPen);


    // ---- TEST: deuxieme reflection ----

    // vecteur d entre l'image et la RX
    QVector2D d_2 = RX - r_image_image;
    // vecteur origine TODO : est-ce qu'il est tjr à (0,0) ?
    //x0(0, -80);  // Nouvelle origine
    // calcul du t, forme générale
    double t_2 = ((d_2.y() * (r_image_image.x() - x0.x())) - (d_2.x() * (r_image_image.y() - x0.y()))) / (unitary_top.x() * d_2.y() - unitary_top.y() * d_2.x());
    QVector2D P_r_2 = x0 + t_2 * unitary_top;
    double d_norm_2 = sqrt(pow(d_2.x(), 2) + pow(d_2.y(), 2));
    // paramètres de la réflexion
    double cos_theta_i_2 = QVector2D::dotProduct(d_2/d_norm_2, normal_top);
    double sin_theta_i_2 = sqrt(1 - pow(cos_theta_i_2, 2));
    double sin_theta_t_2 = sin_theta_i_2 / sqrt(epsilon_r);
    double cos_theta_t_2 = sqrt(1 - pow(sin_theta_t_2, 2));
    double s_2 = thickness / cos_theta_t_2;
    complex<double> Gamma_perpendicular_2 = (Z_m * cos_theta_i_2 - Z_0 * cos_theta_t_2) / (Z_m * cos_theta_i_2 + Z_0 * cos_theta_t_2);
    // TODO : valeur un petit peu différente de la valeur attendue, pourquoi ?
    complex<double> reflection_term_2 = exp(-2.0 * real(gamma_m) * s_2) * exp(j * 2.0 * (gamma_m) * s_2 * sin_theta_t_2 * sin_theta_i_2);
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
QVector2D calculateReflectionPoint(const QVector2D& r_image, const QVector2D& RX, const QVector2D& TX) {
    QVector2D d = RX-r_image;
    QVector2D x0(0,0);
    double t = ((d.y()*(r_image.x()-x0.x()))-(d.x()*(r_image.y()-x0.y()))) / (unitary.x()*d.y()-unitary.y()*d.x());
    QVector2D P_r = x0 + t * unitary;
    return P_r;
}


// scène graphique, encore une fois merci gpt pour la syntaxe
//QGraphicsScene* createGraphicsScene(const QVector2D& RX, const QVector2D& TX) {
QGraphicsScene* createGraphicsScene(ReceiverTest& RX, TransmitterTest& TX) {
    //QGraphicsScene* createGraphicsScene(const QPointF& RX) {
    auto* scene = new QGraphicsScene();

    // Définir les QBrush et les QPen
    //QBrush rxBrush(Qt::blue);
    //QPen rxPen(Qt::darkBlue);
    //QBrush txBrush(Qt::black);
    //QPen txPen(Qt::darkGray);
    //QPen dVectorPen(Qt::green);
    //dVectorPen.setWidth(1); // 2?
    //QPen wallPen(Qt::gray);
    //wallPen.setWidth(4);

    // Dessiner RX et TX
    //create RectItem for the RX to put a toolTip on it
    //QGraphicsRectItem* RXgraphics = new QGraphicsRectItem(RX.x() - 5, -RX.y() - 5, 10, 10);
    //RXgraphics->setBrush(rxBrush);
    //RXgraphics->setPen(rxPen);
    //float rx_power_dBm = 10*std::log10(P_RX_TOTAL); // TODO: correct ?
    //RXgraphics->setToolTip(QString("Test receiver\nx=%1 y=%2\nPower: %3 mW | %4 dBm").arg(QString::number(RX.x()),QString::number(RX.y()),QString::number(P_RX_TOTAL),QString::number(rx_power_dBm,'f',2)));
    ////scene->addRect(RX.x() - 5, -RX.y() - 5, 10, 10, rxPen, rxBrush);
    ////scene->addEllipse(RX.x() - 5, -RX.y() - 5, 10, 10, rxPen, rxBrush);
    //scene->addItem(RXgraphics);
    //qDebug() << "RXgraphics:" << RXgraphics->rect();

    float _rx_power_dBm = 10*std::log10(RX.power); // TODO: correct ?
    RX.graphics->setToolTip(QString("Test receiver\nx=%1 y=%2\nPower: %3 mW | %4 dBm").arg(QString::number(RX.x()),QString::number(RX.y()),QString::number(RX.power),QString::number(_rx_power_dBm,'f',2)));
    scene->addItem(RX.graphics);
    qDebug() << "RX.graphics:" << RX.graphics->rect();

    //create EllipseItem for the TX to put a toolTip on it
    //QGraphicsEllipseItem* TXgraphics = new QGraphicsEllipseItem(TX.x() - 5, -TX.y() - 5, 10, 10);
    //TXgraphics->setBrush(txBrush);
    //TXgraphics->setPen(txPen);
    //TXgraphics->setToolTip(QString("Test transmitter\nx=%1 y=%2\nG_TX*P_TX=%3").arg(QString::number(TX.x()),QString::number(TX.y()),QString::number(G_TXP_TX)));
    ////scene->addEllipse(TX.x() - 5, -TX.y() - 5, 10, 10, txPen, txBrush);
    ////scene->addItem(&TX);
    //scene->addItem(TXgraphics);

    TX.graphics->setToolTip(QString("Test transmitter\nx=%1 y=%2\nG_TX*P_TX=%3").arg(QString::number(TX.x()),QString::number(TX.y()),QString::number(G_TXP_TX)));
    scene->addItem(TX.graphics);
    qDebug() << "TX.graphics:" << TX.graphics->rect();

    // Dessiner le vecteur d
    scene->addLine(TX.x(), -TX.y(), RX.x(), -RX.y(), dVectorPen);

    // Dessiner les murs
    scene->addLine(wall1start.x(), -wall1start.y(), wall1end.x(), -wall1end.y(), wallPen);
    scene->addLine(wall2start.x(), -wall2start.y(), wall2end.x(), -wall2end.y(), wallPen);
    scene->addLine(wall3start.x(), -wall3start.y(), wall3end.x(), -wall3end.y(), wallPen);

    // test:
    tx_image_graphics->setBrush(QBrush(Qt::darkYellow));
    tx_image_graphics->setPen(QPen(Qt::darkYellow));
    tx_image_graphics->setToolTip(QString("TX image\nx=%1 y=%2").arg(QString::number(tx_image_graphics->rect().x()),QString::number(-tx_image_graphics->rect().y())));
    scene->addItem(tx_image_graphics);

    tx_image_image_graphics->setBrush(QBrush(Qt::darkYellow));
    tx_image_image_graphics->setPen(QPen(Qt::darkYellow));
    tx_image_image_graphics->setToolTip(QString("TX image image\nx=%1 y=%2").arg(QString::number(tx_image_image_graphics->rect().x()),QString::number(-tx_image_image_graphics->rect().y())));
    scene->addItem(tx_image_image_graphics);

    return scene;
}


qreal computeReflectionCoeff() {

}

qreal computeTransmissionCoeff() {

}

complex<qreal> computePerpendicularGamma() {

}

void computeReflections() {
    // calls to 1reflection and 2reflection

    // TODO: third reflection ?
}

void computeDirect() {

}


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    qDebug() << "RX: x" << QString::number(RX.x()) << " y" << QString::number(RX.y());
    qDebug() << "TX: x" << QString::number(TX.x()) << " y" << QString::number(TX.y());
    //qDebug() << "RX: x" << QString::number(RX.x()) << " y" << QString::number(RX.y());
    init();

    QVector2D r_image = calculateImageAntenna(TX, normal);
    // test:
    tx_image_graphics->setRect(r_image.x()-3,-r_image.y()-3,6,6);

    // TODO : foutre ça dans une fonction dédiée, MAIS c'est pas urgent, ça se fera
    // naturellement quand on mettre ça dans l'interface
    // calculs de la transmission directe
    QVector2D d = RX - TX;

    // TODO: or use dot products? :
    //double cos_theta_i = abs(QVector2D::dotProduct(QVector2D(unitary), QVector2D(d).normalized()));
    double cos_theta_i = d.y() / sqrt(pow(d.x(), 2) + pow(d.y(), 2));
    qDebug() << "cos_theta_i" << cos_theta_i;
    // TODO: or use do products? :
    //double sin_theta_i = abs(QVector2D::dotProduct(QVector2D(normal),QVector2D(d).normalized()));
    double sin_theta_i = sqrt(1 - pow(cos_theta_i, 2));
    qDebug() << "sin_theta_i" << sin_theta_i;

    double sin_theta_t = sin_theta_i / sqrt(epsilon_r);
    qDebug() << "sin_theta_t" << sin_theta_t;
    double cos_theta_t = sqrt(1 - pow(sin_theta_t, 2));
    qDebug() << "cos_theta_t" << cos_theta_t;

    double s = thickness / cos_theta_t;

    complex<double> Gamma_perpendicular = (Z_m * cos_theta_i - Z_0 * cos_theta_t) / (Z_m * cos_theta_i + Z_0 * cos_theta_t);
    // découpe le calcul de T_m en numérateur et dénominateur parce qu'il est
    //  tarpin long, sa valeur est celle attendue yay !
    complex<double> numerator = (1.0 - pow(Gamma_perpendicular, 2)) * exp(-gamma_m * s);
    complex<double> denominator = 1.0 - pow(Gamma_perpendicular, 2) * exp(-2.0 * gamma_m * s) * exp(j * 2.0 * real(gamma_m)* sin_theta_t * sin_theta_i);
    complex<double> T_m = numerator / denominator;
    double d1 = sqrt(pow(d.x(), 2) + pow(d.y(), 2)); // TODO foutre ça // TODO: d1 = d.length() comme c'est un QVector2D mtn
        //quelque part plus haut ptet ?
    // valeur de exp term non calculée solo dans le tp donc je sais pas quelle est sa valeur
    // mais E_1 a une valeur un peu différente d'attendu TODO : découvrir pourquoi et corriger
    qDebug() << "----T_m" << QString::number(T_m.real()) << "+ j" << QString::number(T_m.imag());
    qDebug() << "gamma_m imag:" << imag(gamma_m);
    complex<double> exp_term = exp(-j * imag(gamma_m) * d1); // pr simplifier expression en dessous
    complex<double> E_1 = T_m * sqrt(60 * G_TXP_TX) * exp_term/ d1; // Convertir P_TX de dBm en Watts
    // valeur différente de celle attendue, logique car elle dépend de G_TXP_TX dont je suis
    //  pas sûr de la valeur et de E_1 dont la valeur est différente de celle attendue
    //  TODO : malgré cela, est-ce que la fonction est bien celle ci ou y a t il une erreur ?
    //         j'ai pas investigué outre mesure car on peut pas le déterminer numériquement
    //         vu que E_1 a déjà pas la bonne valeur
    double P_RX = (60 * pow(lambda, 2)) / (8 * pow(M_PI,2)*Ra) * G_TXP_TX * pow(abs(T_m*exp_term/d1), 2);
    qDebug() << "           P_RX" << P_RX;
    qDebug() << "           E_1" << real(E_1) << "+" << imag(E_1) <<"j";
    qDebug() << "          exp_term" << real(exp_term) << "+" << imag(exp_term) <<"j";

    // petit affichage graphique, syntaxe made in gpt

    //--- on a foutu ça en dernier dans main() ---
    /*
    QGraphicsScene* scene = createGraphicsScene(RX, TX);
    //QGraphicsScene* scene = createGraphicsScene(RX);
    addReflectionComponents(scene, RX, TX, r_image);
    QGraphicsView* view = new QGraphicsView(scene);
    // ? :
    view->setAttribute( Qt::WA_AlwaysShowToolTips);
    */
    //--------------------------------------------

    // calculs de paramètres pour la réflexion
    QVector2D P_r = calculateReflectionPoint(r_image, RX, TX); // reflection point P_r
    QVector2D eta = P_r - TX; // vecteur de P_r à RX, notation issue du tp
    double eta_norm = sqrt(pow(eta.x(), 2) + pow(eta.y(), 2));
    double cos_theta_i_reflected = abs(QVector2D::dotProduct(eta.normalized(), unitary)); // or eta/eta_norm
    double sin_theta_i_reflected = sqrt(1 - pow(cos_theta_i_reflected, 2));
    double sin_theta_t_reflected = sin_theta_i_reflected / sqrt(epsilon_r);
    double cos_theta_t_reflected = sqrt(1 - pow(sin_theta_t_reflected, 2));
    double s_reflected = thickness / cos_theta_t_reflected;
    complex<double> Gamma_perpendicular_reflected = (Z_m * cos_theta_i_reflected - Z_0 * cos_theta_t_reflected) /
                                                    (Z_m * cos_theta_i_reflected + Z_0 * cos_theta_t_reflected);
    // encore une fois un coefficient slightly pas le même pélo TODO : nsm wsh
    // en tout cas la discussion est la même que pour le cas à transmission, voir le todo plus haut

    // TODO: ici en dessous on devrait pas utiliser s_reflected plutot que s et Gamma_perpendicular_reflected plutot que Gamma_perpendicular ?
    complex<double> T_m_r = ((1.0 - pow(Gamma_perpendicular_reflected, 2)) * exp(-gamma_m * s_reflected)) / (1.0 - pow(Gamma_perpendicular_reflected, 2) * exp(-2.0 * gamma_m * s_reflected) * exp(j * 2.0 * real(gamma_m)* sin_theta_t_reflected * sin_theta_i_reflected));
    complex<double> E_2 = T_m_r * sqrt(60 * G_TXP_TX) * exp(-j * imag(gamma_m) * eta_norm) / eta_norm; // this reflection coefficient ?
    double P_RX_reflected = (60 * pow(lambda, 2)) / (8 * M_PI) * G_TXP_TX * pow(abs(E_2), 2); // is this in mW ?
    qDebug() << "P_RX_reflected" << P_RX_reflected;


    // ---- TEST deuxieme reflection ---- // TODO: test

    QVector2D r_image_image = calculateImageAntenna(r_image, normal_top);
    // test:
    tx_image_image_graphics->setRect(r_image_image.x()-3,-r_image_image.y()-3,6,6);

    QVector2D P_r_2 = calculateReflectionPoint(r_image_image, RX, r_image); // second reflection point P_r_2
    QVector2D eta_2 = P_r_2 - r_image;
    double eta_2_norm = sqrt(pow(eta_2.x(),2) + pow(eta_2.y(),2));
    double cos_theta_i_reflected_2 = abs(QVector2D::dotProduct(eta_2.normalized(), unitary_top)); // or eta/eta_norm
    double sin_theta_i_reflected_2 = sqrt(1 - pow(cos_theta_i_reflected_2, 2));
    double sin_theta_t_reflected_2 = sin_theta_i_reflected_2 / sqrt(epsilon_r);
    double cos_theta_t_reflected_2 = sqrt(1 - pow(sin_theta_t_reflected_2, 2));
    double s_reflected_2 = thickness / cos_theta_t_reflected_2;
    complex<double> Gamma_perpendicular_reflected_2 = (Z_m * cos_theta_i_reflected_2 - Z_0 * cos_theta_t_reflected_2) /
                                                    (Z_m * cos_theta_i_reflected_2 + Z_0 * cos_theta_t_reflected_2);
    // encore une fois un coefficient slightly pas le même pélo TODO : nsm wsh
    // en tout cas la discussion est la même que pour le cas à transmission, voir le todo plus haut

    // TODO: ici en dessous on devrait pas utiliser s_reflected plutot que s et Gamma_perpendicular_reflected plutot que Gamma_perpendicular ?
    complex<double> T_m_r_2 = ((1.0 - pow(Gamma_perpendicular_reflected_2, 2)) * exp(-gamma_m * s_reflected_2)) / (1.0 - pow(Gamma_perpendicular_reflected_2, 2) * exp(-2.0 * gamma_m * s_reflected_2) * exp(j * 2.0 * real(gamma_m)* sin_theta_t_reflected_2 * sin_theta_i_reflected_2));
    complex<double> E_2_2 = T_m_r * sqrt(60 * G_TXP_TX) * exp(-j * imag(gamma_m) * eta_2_norm) / eta_2_norm; // this reflection coefficient ?
    double P_RX_reflected_2 = (60 * pow(lambda, 2)) / (8 * M_PI) * G_TXP_TX * pow(abs(E_2_2), 2); // is this in mW ?
    qDebug() << "P_RX_reflected_2" << P_RX_reflected_2;

    // -----------------------------


    P_RX_TOTAL = P_RX + P_RX_reflected; // TODO: correct ?
    qDebug() << "PX_RX_TOTAL ??" << P_RX_TOTAL;
    RX.power = P_RX_TOTAL;

    QGraphicsScene* scene = createGraphicsScene(RX, TX);
    //QGraphicsScene* scene = createGraphicsScene(RX);

    addReflectionComponents(scene, RX, TX, r_image, r_image_image);
    QGraphicsView* view = new QGraphicsView(scene);

    // necessary ? :
    view->setAttribute( Qt::WA_AlwaysShowToolTips);

    // une view, TODO pour quand on implémente, faire en sorte que les ellipses de RX et TX
    //  soient plus petites, parce que j'ai fait un scale x2 juste pour que ça soit moins minuscule
    view->setFixedSize(600, 400);
    view->scale(2.0, 2.0); // TODO: not use scale?
    view->show();
    return app.exec();
}
