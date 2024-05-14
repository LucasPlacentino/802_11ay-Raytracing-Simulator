#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <complex>
#include <QtTypes>

using namespace std;

constexpr complex<qreal> j(0, 1); // ! définition de j, useful
constexpr qulonglong c = 3*1e8;
//constexpr qulonglong frequency = 60*1e9;
constexpr qreal epsilon_0 = 8.854187817e-12;
constexpr qreal mu_0 = 1.256637e-6;//4 * M_PI * 1e-7; // 1.256637e-6; // ?
constexpr qreal freq = 60e9; // 60 GHz
constexpr qreal omega = 2 * M_PI * freq; // pulsation
constexpr qreal wavelength = c/freq;
constexpr qreal lambda = c / freq;
const qreal Z_0 = sqrt(mu_0 / epsilon_0); // impédance du vide // vacuum impedance

constexpr qreal max_x = 15;
constexpr qreal min_x = 0;
constexpr qreal max_y = 0;
constexpr qreal min_y = -8;
// resolution is set in simulation->resolution

constexpr qreal beta_0 = 2*M_PI*freq/c; // beta

constexpr qreal G_TX = 1.64; // transmitter antenna gain, 1.64 (or 1.7 ?)

#endif //PARAMETERS_H
