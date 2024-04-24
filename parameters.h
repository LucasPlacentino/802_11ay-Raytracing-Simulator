#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "qtypes.h"
#include <complex>

using namespace std;

constexpr complex<double> j(0, 1); // ! définition de j, useful
constexpr qulonglong c = 3*1e8;
constexpr qulonglong frequency = 60*1e9;
const qreal wavelength = c/frequency;
constexpr double epsilon_0 = 8.854187817e-12;
constexpr double mu_0 = 1.256637e-6;//4 * M_PI * 1e-7; // 1.256637e-6; // ?
constexpr double freq = 60e9; // 60 GHz
constexpr double omega = 2 * M_PI * freq; // pulsation

constexpr qreal max_x = 15;
constexpr qreal min_x = 0;
constexpr qreal max_y = 0;
constexpr qreal min_y = -8;
qreal resolution = 0.5;


#endif //PARAMETERS_H
