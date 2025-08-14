/* mathutil.h */

#pragma once

typedef struct {
	double real;
	double imag;
} Complex;

Complex comp_add(Complex a, Complex b);
Complex comp_mul(Complex a, Complex b);

// Returns the position of given number on given axis and its boundaries on the complex plane
long double map_point_to_complex(double val, double in_max, double in_min, double out_max, double out_min);
