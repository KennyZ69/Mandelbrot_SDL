/* mathutil.c */
// For utility math functions and types such as complex numbers or mapping real numbers into the complex plane points

 #include "in/mathutil.h"

Complex comp_add(Complex a, Complex b) {
	return (Complex){a.real + b.real, a.imag + b.imag};
}

Complex comp_mul(Complex a, Complex b) {
	return (Complex){a.real*b.real - a.imag*b.imag, a.real*b.imag + a.imag*b.real};
}

long double map_point_to_complex(double val, double in_max, double in_min, double out_max, double out_min) {
	return (long double)(out_min + (val - in_min) * (out_max - out_min) / (in_max - in_min));
}
