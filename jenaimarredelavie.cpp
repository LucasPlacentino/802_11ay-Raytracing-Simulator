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

using namespace std;

// Définition des constantes physiques
constexpr double epsilon_0 = 8.854187817e-12; // Permittivité du vide (F/m)
constexpr double mu_0 = 4 * M_PI * 1e-7;      // Perméabilité du vide (H/m)
constexpr double freq = 868.3e6;              // Fréquence utilisée pour le WiFi (Hz)
constexpr double c = 299792458;               // Vitesse de la lumière (m/s)
constexpr double G_TXP_TX = 1.64e-3;          // Gain de l'émetteur, simplifié pour les calculs

// Calcul de l'impédance du vide
double Z_0 = sqrt(mu_0 / epsilon_0);

// Paramètres des matériaux
constexpr double epsilon_r = 4.8;   // Permittivité relative
constexpr double sigma = 0.018;     // Conductivité (S/m)
constexpr double thickness = 0.15;  // Épaisseur du mur (m)

// Paramètres de calcul dérivés
constexpr double omega = 2 * M_PI * freq;       // Fréquence angulaire (rad/s)
constexpr double lambda = c / freq;             // Longueur d'onde (m)
const complex<double> j(0, 1);                  // Unité imaginaire

// Calculs préliminaires sur les matériaux
complex<double> epsilon = epsilon_0 * epsilon_r;
complex<double> Z_m = sqrt(mu_0 / (epsilon - complex<double>(0,1) * sigma / omega));
complex<double> gamma_m = sqrt(j * omega * mu_0 * (sigma + j * omega * epsilon));

// Position des antennes
const QPointF RX(47, 65);
const QPointF TX(32, 10);
const QPointF normal(1, 0); // Vecteur normal au mur
const QPointF unitary(0,1);

// Fonction pour calculer l'image de l'antenne émettrice par rapport à un obstacle
QPointF calculateImageAntenna(const QPointF& TX, const QPointF& normal) {
    double dotProduct = QPointF::dotProduct(TX, normal);
    QPointF r_image = TX - 2 * dotProduct * normal;
    return r_image;
}

// Fonction pour ajouter les composants graphiques de réflexion dans la scène
void addReflectionComponents(QGraphicsScene* scene, const QPointF& RX, const QPointF& TX, const QPointF& r_image) {
    QPointF d = RX - r_image;
    QPointF x0(0, 0);  // Origine
    double t = ((d.y() * (r_image.x() - x0.x())) - (d.x() * (r_image.y() - x0.y()))) / (unitary.x() * d.y() - unitary.y() * d.x());
    QPointF P_r = x0 + t * unitary;
    double d_norm = sqrt(pow(d.x(), 2) + pow(d.y(), 2));
    double cos_theta_i = QPointF::dotProduct(d/d_norm, normal);
    double sin_theta_i = sqrt(1 - pow(cos_theta_i, 2));
    double sin_theta_t = sin_theta_i / sqrt(epsilon_r);
    double cos_theta_t = sqrt(1 - pow(sin_theta_t, 2));
    double s = thickness / cos_theta_t;
    complex<double> Gamma_perpendicular = (Z_m * cos_theta_i - Z_0 * cos_theta_t) / (Z_m * cos_theta_i + Z_0 * cos_theta_t);
    complex<double> reflection_term = exp(-2.0 * gamma_m * s) * exp(j * 2.0 * omega * s * sin_theta_t * sin_theta_i);
    complex<double> Gamma_m = Gamma_perpendicular - (1.0 - pow(abs(Gamma_perpendicular), 2)) * Gamma_perpendicular * reflection_term / (1.0 - pow(abs(Gamma_perpendicular), 2) * reflection_term);
    QPen reflectionPen(Qt::red);
    reflectionPen.setWidth(2);
    scene->addLine(TX.x(), -TX.y(), P_r.x(), -P_r.y(), reflectionPen);
    scene->addLine(P_r.x(), -P_r.y(), RX.x(), -RX.y(), reflectionPen);
}

// Création de la scène graphique initiale
QGraphicsScene* createGraphicsScene(const QPointF& RX, const QPointF& TX) {
    auto* scene = new QGraphicsScene();
    QBrush rxBrush(Qt::blue);
    QPen rxPen(Qt::black);
    QBrush txBrush(Qt::black);
    QPen txPen(Qt::black);
    QPen dVectorPen(Qt::green);
    dVectorPen.setWidth(2);
    QPen wallPen(Qt::gray);
    wallPen.setWidth(4);
    scene->addEllipse(RX.x() - 5, -RX.y() - 5, 10, 10, rxPen, rxBrush);
    scene->addEllipse(TX.x() - 5, -TX.y() - 5, 10, 10, txPen, txBrush);
    scene->addLine(TX.x(), -TX.y(), RX.x(), -RX.y(), dVectorPen);
    scene->addLine(0, -20, 130, -20, wallPen);
    scene->addLine(0, 0, 0, -80, wallPen);
    scene->addLine(0, -80, 130, -80, wallPen);
    return scene;
}
pair<complex<double>, complex<double>> calculateImpedanceAndPropagation(double epsilon_r, double sigma, double omega) {
    complex<double> epsilon = epsilon_0 * epsilon_r;
    complex<double> Z_m = sqrt(mu_0 / (epsilon - complex<double>(0,1) * sigma / omega));
    complex<double> gamma_m = sqrt(j * omega * mu_0 * (sigma + j * omega * epsilon));
    return {Z_m, gamma_m};
}
QPointF calculateReflectionPoint(const QPointF& r_image, const QPointF& RX, const QPointF& TX) {
    QPointF d = RX-r_image; // Vecteur directeur
    QPointF x0(0,0); // Vecteur directeur
    cout << d.x() << " " << d.y() << " " << x0.x()<< " " <<x0.y()<< " " <<r_image.x()<< " " <<r_image.y()<< " " << endl;
    double t = ((d.y()*(r_image.x()-x0.x()))-(d.x()*(r_image.y()-x0.y()))) / (unitary.x()*d.y()-unitary.y()*d.x());
    cout << "----dededede---" << "t:"<< t << endl;
    QPointF P_r = x0 + t * unitary;
    cout << "----dededede---" << "P_r:"<< P_r.x() << "P_r:"<< P_r.y() <<endl;
    return P_r;
}
// Fonction principale
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    auto [Z_m, gamma_m] = calculateImpedanceAndPropagation(epsilon_r, sigma, omega);
    QPointF r_image = calculateImageAntenna(TX, normal);
    QPointF d = RX - TX;
    double cos_theta_i = d.y() / sqrt(pow(d.x(), 2) + pow(d.y(), 2));
    double sin_theta_i = sqrt(1 - pow(cos_theta_i, 2));
    double sin_theta_t = sin_theta_i / sqrt(epsilon_r);
    double cos_theta_t = sqrt(1 - pow(sin_theta_t, 2));
    double s = thickness / cos_theta_t;
    complex<double> Gamma_perpendicular = (Z_m * cos_theta_i - Z_0 * cos_theta_t) / (Z_m * cos_theta_i + Z_0 * cos_theta_t);
    complex<double> T_m = (1.0 - std::norm(Gamma_perpendicular)) * std::exp(-gamma_m * s) * exp(j * imag(gamma_m) * 2.0 * sin_theta_t * sin_theta_i);
    double d1 = sqrt(pow(d.x(), 2) + pow(d.y(), 2));
    complex<double> exp_term = exp(-j * imag(gamma_m) * d1);
    complex<double> E_n = T_m * sqrt(60 * G_TXP_TX) * exp_term/ d1; // Conversion de P_TX de dBm en Watts
    double P_RX = (60 * pow(lambda, 2)) / (8 * M_PI) * G_TXP_TX * pow(abs(E_n), 2);
    QGraphicsScene* scene = createGraphicsScene(RX, TX);
    addReflectionComponents(scene, RX, TX, r_image);
    QGraphicsView* view = new QGraphicsView(scene);
    QPointF P_r = calculateReflectionPoint(r_image, RX, TX);
    QPointF eta = P_r - TX; // Vecteur de P_r à RX
    double eta_norm = sqrt(pow(eta.x(), 2) + pow(eta.y(), 2));
    double cos_theta_i_reflected = eta.y() / eta_norm;
    double sin_theta_i_reflected = sqrt(1 - pow(cos_theta_i_reflected, 2));
    double sin_theta_t_reflected = sin_theta_i_reflected / sqrt(epsilon_r);
    double cos_theta_t_reflected = sqrt(1 - pow(sin_theta_t_reflected, 2));
    double s_reflected = thickness / cos_theta_t_reflected;
    complex<double> Gamma_perpendicular_reflected = (Z_m * cos_theta_i_reflected - Z_0 * cos_theta_t_reflected) /
                                                    (Z_m * cos_theta_i_reflected + Z_0 * cos_theta_t_reflected);
    complex<double> T_m_reflected = (1.0 - std::norm(Gamma_perpendicular_reflected)) * std::exp(-gamma_m * s) * exp(j * imag(gamma_m) * 2.0 * sin_theta_t * sin_theta_i);
    complex<double> E_2 = T_m_reflected * sqrt(60 * G_TXP_TX) * exp(-j * imag(gamma_m) * eta_norm) / eta_norm;
    double P_RX_reflected = (60 * pow(lambda, 2)) / (8 * M_PI) * G_TXP_TX * pow(abs(E_2), 2);
    view->setFixedSize(800, 600);
    view->scale(2.0, 2.0);
    view->show();
    return app.exec();
}
