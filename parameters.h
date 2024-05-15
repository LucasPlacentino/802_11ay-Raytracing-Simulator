#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <complex>
#include <QtTypes>

using namespace std;

constexpr complex<qreal> j(0, 1); // useful
constexpr qulonglong c = 3*1e8;
constexpr qreal epsilon_0 = 8.854187817e-12;
constexpr qreal mu_0 = 1.256637e-6;//4 * M_PI * 1e-7; // 1.256637e-6; // ?
constexpr qreal freq = 60e9; // 60 GHz
constexpr qreal omega = 2 * M_PI * freq; // pulse
constexpr qreal wavelength = c/freq;
constexpr qreal lambda = wavelength;
const qreal Z_0 = sqrt(mu_0 / epsilon_0); // vacuum impedance

constexpr qreal max_x = 15;
constexpr qreal min_x = 0;
constexpr qreal min_y = 0;
constexpr qreal max_y = 8;
// resolution is set in simulation->resolution, user chosen at runtime

constexpr qreal beta_0 = 2*M_PI*freq/c; // beta

constexpr qreal G_TX = 1.7; // transmitter antenna gain, 1.64 or 1.7
constexpr qreal P_TX = 0.1; // transmitter power in Watts
constexpr qreal P_TX_dBm = 20; // transmitter power in dBm

#endif //PARAMETERS_H
