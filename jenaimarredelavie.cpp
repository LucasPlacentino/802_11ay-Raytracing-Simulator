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
#include <iostream>

// pour plus de simplicité
using namespace std;

// constantes
constexpr double epsilon_0 = 8.854187817e-12;
constexpr double mu_0 = 4 * M_PI * 1e-7;
constexpr double freq = 868.3e6;
constexpr double c = 299792458;
constexpr double G_TXP_TX = 1.64e-3; // TODO : régler la problématique de sa valeur?
// il n'est pas défini en constexpr car cela provoque une erreur,
// je ne sais pas pourquoi
double Z_0 = sqrt(mu_0 / epsilon_0); // impédance du vide
constexpr double epsilon_r = 4.8;
constexpr double sigma = 0.018; // conductivité (S/m)
constexpr double thickness = 0.15; // épaisseur des murs
constexpr double omega = 2 * M_PI * freq;
constexpr double lambda = c / freq;
const complex<double> j(0, 1); // définition de j

complex<double> epsilon = epsilon_0 * epsilon_r;
complex<double> Z_m = sqrt(mu_0 / (epsilon - j * sigma / omega));
complex<double> gamma_m = sqrt(j * omega * mu_0 * (sigma + j * omega * epsilon));

// positions des objets
const QPointF RX(47, 65);
const QPointF TX(32, 10);
// TODO : incorporer les positions des murs, bien que pour l'instant
//  ça ne serve pas directement dans le calcul
const QPointF normal(1, 0);
const QPointF unitary(0,1);

// fonction qui calcule la position de \vec r_image de l'antenne
QPointF calculateImageAntenna(const QPointF& TX, const QPointF& normal) {
    double dotProduct = QPointF::dotProduct(TX, normal);
    QPointF r_image = TX - 2 * dotProduct * normal;
    return r_image;
}

// calcul des composants issu d'une réflexion
void addReflectionComponents(QGraphicsScene* scene, const QPointF& RX, const QPointF& TX, const QPointF& r_image) {
    // vecteur d entre l'image et la RX
    QPointF d = RX - r_image;
    // vecteur origine TODO : est-ce qu'il est tjr à (0,0) ?
    QPointF x0(0, 0);  // Origine
    // calcul du t, forme générale
    double t = ((d.y() * (r_image.x() - x0.x())) - (d.x() * (r_image.y() - x0.y()))) / (unitary.x() * d.y() - unitary.y() * d.x());
    QPointF P_r = x0 + t * unitary;
    double d_norm = sqrt(pow(d.x(), 2) + pow(d.y(), 2));

    // paramètres de la réflexion
    double cos_theta_i = QPointF::dotProduct(d/d_norm, normal);
    double sin_theta_i = sqrt(1 - pow(cos_theta_i, 2));
    double sin_theta_t = sin_theta_i / sqrt(epsilon_r);
    double cos_theta_t = sqrt(1 - pow(sin_theta_t, 2));
    double s = thickness / cos_theta_t;
    complex<double> Gamma_perpendicular = (Z_m * cos_theta_i - Z_0 * cos_theta_t) / (Z_m * cos_theta_i + Z_0 * cos_theta_t);
    // TODO : valeur un petit peu différente de la valeur attendue, pourquoi ?
    complex<double> reflection_term = exp(-2.0 * real(gamma_m) * s) * exp(j * 2.0 * (gamma_m) * s * sin_theta_t * sin_theta_i);
    complex<double> Gamma_m = Gamma_perpendicular - (1.0 - pow((Gamma_perpendicular), 2)) * Gamma_perpendicular * reflection_term / (1.0 - pow((Gamma_perpendicular), 2) * reflection_term);
    // interface graphique, rien de fou, merci gpt pour la syntaxe ici
    QPen reflectionPen(Qt::red);
    reflectionPen.setWidth(2);
    scene->addLine(TX.x(), -TX.y(), P_r.x(), -P_r.y(), reflectionPen);
    scene->addLine(P_r.x(), -P_r.y(), RX.x(), -RX.y(), reflectionPen);
}
// calculer le point de réflexion sur un mur, même chose que dans le tp
QPointF calculateReflectionPoint(const QPointF& r_image, const QPointF& RX, const QPointF& TX) {
    QPointF d = RX-r_image;
    QPointF x0(0,0);
    double t = ((d.y()*(r_image.x()-x0.x()))-(d.x()*(r_image.y()-x0.y()))) / (unitary.x()*d.y()-unitary.y()*d.x());
    QPointF P_r = x0 + t * unitary;
    return P_r;
}

// scène graphique, encore une fois merci gpt pour la syntaxe
QGraphicsScene* createGraphicsScene(const QPointF& RX, const QPointF& TX) {
    auto* scene = new QGraphicsScene();

    // Définir les brosses et les stylos
    QBrush rxBrush(Qt::blue);
    QPen rxPen(Qt::black);
    QBrush txBrush(Qt::black);
    QPen txPen(Qt::black);
    QPen dVectorPen(Qt::green);
    dVectorPen.setWidth(2);
    QPen wallPen(Qt::gray);
    wallPen.setWidth(4);

    // Dessiner RX et TX
    scene->addEllipse(RX.x() - 5, -RX.y() - 5, 10, 10, rxPen, rxBrush);
    scene->addEllipse(TX.x() - 5, -TX.y() - 5, 10, 10, txPen, txBrush);

    // Dessiner le vecteur d
    scene->addLine(TX.x(), -TX.y(), RX.x(), -RX.y(), dVectorPen);

    // Dessiner les murs
    scene->addLine(0, -20, 130, -20, wallPen);
    scene->addLine(0, 0, 0, -80, wallPen);
    scene->addLine(0, -80, 130, -80, wallPen);

    return scene;
}



int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QPointF r_image = calculateImageAntenna(TX, normal);
    // TODO : foutre ça dans une fonction dédiée, MAIS c'est pas urgent, ça se fera
    // naturellement quand on mettre ça dans l'interface
    // calculs de la transmission directe
    QPointF d = RX - TX;
    double cos_theta_i = d.y() / sqrt(pow(d.x(), 2) + pow(d.y(), 2));
    double sin_theta_i = sqrt(1 - pow(cos_theta_i, 2));
    double sin_theta_t = sin_theta_i / sqrt(epsilon_r);
    double cos_theta_t = sqrt(1 - pow(sin_theta_t, 2));
    double s = thickness / cos_theta_t;
    complex<double> Gamma_perpendicular = (Z_m * cos_theta_i - Z_0 * cos_theta_t) / (Z_m * cos_theta_i + Z_0 * cos_theta_t);
    // découpe le calcul de T_m en numérateur et dénominateur parce qu'il est
    //  tarpin long, sa valeur est celle attendue !
    complex<double> numerator = (1.0 - pow(Gamma_perpendicular, 2)) * exp(-gamma_m * s);
    complex<double> denominator = 1.0 - pow(Gamma_perpendicular, 2) * exp(-2.0 * gamma_m * s) * exp(j * 2.0 * real(gamma_m)* sin_theta_t * sin_theta_i);
    complex<double> T_m = numerator / denominator;
    double d1 = sqrt(pow(d.x(), 2) + pow(d.y(), 2)); // TODO foutre ça
                                                     //quelque part plus haut ptet ?
    // valeur de exp term non calculée solo dans le tp donc je sais pas quelle est sa valeur
    // mais E_n a une valeur un peu différente d'attendu TODO : découvrir pourquoi et corriger
    complex<double> exp_term = exp(-j * real(gamma_m) * d1); // pr simplifier expression en dessous
    complex<double> E_n = T_m * sqrt(60 * G_TXP_TX) * exp_term/ d1; // Convertir P_TX de dBm en Watts
    // valeur différente de celle attendue, logique car elle dépend de G_TXP_TX dont je suis
    //  pas sûr de la valeur et de E_n dont la valeur est différente de celle attendue
    //  TODO : malgré cela, est-ce que la fonction est bien celle ci ou y a t il une erreur ?
    //         j'ai pas investigué outre mesure car on peut pas le déterminer numériquement
    //         vu que E_n a déjà pas la bonne valeur
    double P_RX = (60 * pow(lambda, 2)) / (8 * M_PI) * G_TXP_TX * pow(abs(E_n), 2);

    // petit affichage graphique, syntaxe made in gpt
    QGraphicsScene* scene = createGraphicsScene(RX, TX);
    addReflectionComponents(scene, RX, TX, r_image);
    QGraphicsView* view = new QGraphicsView(scene);

    // calculs de paramètres pour la réflexion
    QPointF P_r = calculateReflectionPoint(r_image, RX, TX);
    QPointF eta = P_r - TX; // vecteur de P_r à RX, notation issue du tp
    double eta_norm = sqrt(pow(eta.x(), 2) + pow(eta.y(), 2));
    double cos_theta_i_reflected = abs(QPointF::dotProduct(eta / eta_norm, unitary));
    double sin_theta_i_reflected = sqrt(1 - pow(cos_theta_i_reflected, 2));
    double sin_theta_t_reflected = sin_theta_i_reflected / sqrt(epsilon_r);
    double cos_theta_t_reflected = sqrt(1 - pow(sin_theta_t_reflected, 2));
    double s_reflected = thickness / cos_theta_t_reflected;
    complex<double> Gamma_perpendicular_reflected = (Z_m * cos_theta_i_reflected - Z_0 * cos_theta_t_reflected) /
                                                    (Z_m * cos_theta_i_reflected + Z_0 * cos_theta_t_reflected);
    // encore une fois un coefficient slightly pas le même pélo TODO : nsm wsh
    // en tout cas la discussion est la même que pour le cas à transmission, voir le todo plus haut
    complex<double> T_m_r = ((1.0 - pow(Gamma_perpendicular_reflected, 2)) * exp(-gamma_m * s)) / (1.0 - pow(Gamma_perpendicular, 2) * exp(-2.0 * gamma_m * s) * exp(j * 2.0 * real(gamma_m)* sin_theta_t * sin_theta_i));
    complex<double> E_2 = T_m_r * sqrt(60 * G_TXP_TX) * exp(-j * imag(gamma_m) * eta_norm) / eta_norm;
    double P_RX_reflected = (60 * pow(lambda, 2)) / (8 * M_PI) * G_TXP_TX * pow(abs(E_2), 2);
    // une view, TODO pour quand on implémente, faire en sorte que les ellipses de RX et TX
    //  soient plus petites, parce que j'ai fait un scale x2 juste pour que ça soit moins minuscule
    view->setFixedSize(800, 600);
    view->scale(2.0, 2.0);
    view->show();
    return app.exec();
}

