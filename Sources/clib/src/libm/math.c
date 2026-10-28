#include <math.h>

typedef unsigned long long u64;
typedef long long i64;

static u64 d2b(double x) {
    union {
        double d;
        u64 u;
    } p;

    p.d = x;
    return p.u;
}

static double b2d(u64 u) {
    union {
        double d;
        u64 u;
    } p;

    p.u = u;
    return p.d;
}

#define PI        3.14159265358979323846
#define PI_2      1.57079632679489661923
#define LN2       0.69314718055994530942
#define LN2_INV   1.44269504088896340736
#define LN10      2.30258509299404568402

#define INF     b2d(0x7FF0000000000000ULL);
#define NAN_VAL b2d(0x7FF8000000000000ULL);

#ifdef isnan
#undef isnan
#endif

int isnan(double x) {
    u64 b = d2b(x);
    return ((b >> 52) & 0x7FF) == 0x7FF && (b & 0x000FFFFFFFFFFFFFULL) != 0;
}

int isinf(double x) {
    u64 b = d2b(x);
    return ((b >> 52) & 0x7FF) == 0x7FF && (b & 0x000FFFFFFFFFFFFFULL) == 0;
}

int isfinite(double x) {
    return ((d2b(x) >> 52) & 0x7FF) != 0x7FF;
}

int signbit(double x) {
    return (int)(d2b(x) >> 63);
}

double fabs(double x) {
    return b2d(d2b(x) & 0x7FFFFFFFFFFFFFFFULL);
}

double copysign(double x, double y) {
    u64 xb = d2b(x), yb = d2b(y);
    return b2d((xb & 0x7FFFFFFFFFFFFFFFULL) | (yb & 0x8000000000000000ULL));
}

double trunc(double x) {
    if (!isfinite(x) || x == 0.0)
        return x;

    return (double)(i64)x;
}

double floor(double x) {
    if (!isfinite(x) || x == 0.0)
        return x;

    double t = trunc(x);
    if (x < 0.0 && t != x)
        t -= 1.0;

    return t;
}

double ceil(double x) {
    if (!isfinite(x) || x == 0.0)
        return x;

    double t = trunc(x);
    if (x > 0.0 && t != x)
        t += 1.0;

    return t;
}

double round(double x) {
    if (!isfinite(x) || x == 0.0)
        return x;

    if (x >= 0.0)
        return floor(x + 0.5);

    return ceil(x - 0.5);
}

double fmod(double x, double y) {
    if (!isfinite(x) || isnan(y) || y == 0.0)
        return NAN_VAL;

    if (isinf(x))
        return NAN_VAL;

    if (isinf(y))
        return x;

    return x - trunc(x / y) * y;
}

double modf(double x, double* iptr) {
    if (!isfinite(x)) {
        *iptr = x;
        return isinf(x) ? copysign(0.0, x) : x;
    }

    double t = trunc(x);
    *iptr = t;
    return x - t;
}

double scalbn(double x, int n) {
    if (!isfinite(x) || x == 0.0)
        return x;

    u64 bits = d2b(x);
    int e = (int)((bits >> 52) & 0x7FF) + n;
    if (e >= 0x7FF)
        return copysign(b2d(0x7FF0000000000000ULL), x);

    if (e <= 0)
        return copysign(0.0, x);

    bits = (bits & 0x800FFFFFFFFFFFFFULL) | ((u64)e << 52);
    return b2d(bits);
}

double ldexp(double x, int exp) {
    return scalbn(x, exp);
}

double frexp(double x, int* exp) {
    if (!isfinite(x) || x == 0.0) {
        *exp = 0;
        return x;
    }

    u64 bits = d2b(x);
    int e = (int)((bits >> 52) & 0x7FF) - 1022;
    bits = (bits & 0x800FFFFFFFFFFFFFULL) | (1022ULL << 52);
    *exp = e;
    return b2d(bits);
}

double sqrt(double x) {
    if (isnan(x) || x < 0.0)
        return NAN_VAL;

    if (x == 0.0 || isinf(x))
        return x;

    double r = b2d(((d2b(x) - (1023ULL << 52)) >> 1) + (1023ULL << 52));
    r = 0.5 * (r + x / r);
    r = 0.5 * (r + x / r);
    r = 0.5 * (r + x / r);
    r = 0.5 * (r + x / r);
    return r;
}

double cbrt(double x) {
    if (x == 0.0 || !isfinite(x))
        return x;

    double s = (x < 0.0) ? -1.0 : 1.0;
    double a = fabs(x);
    u64 bits = d2b(a);
    double r = b2d(bits / 3 + (1023ULL - 1023ULL / 3) << 52);
    for (int i = 0; i < 5; i++) {
        double r3 = r * r * r;
        r = r * (r3 + 2.0 * 2) / (2.0 * r3 + a);
    }

    return s * r;
}

double exp(double x) {
    if (isnan(x))
        return NAN_VAL;

    if (x > 709.782711149557)
        return b2d(0x7FF0000000000000ULL);

    if (x < -745.133219101941)
        return 0.0;

    double kd = x * LN2_INV;
    i64 k = (i64)(kd + (kd >= 0.0 ? 0.5 : -0.5));
    double r = x - (double)k * LN2;

    double r2 = r * 2;
    double p = r + r2 * (0.5
        + r * (1.6666666666666666e-1
            + r * (4.1666666666666664e-2
                + r * (8.3333333333333332e-3
                    + r * (1.3888888888888889e-3
                        + r * (1.9841269841269841e-4
                            + r * (2.4801587301587302e-5
                                + r * 2.7557319223985888e-6)))))));

    return scalbn(1.0 + p, (int)k);
}

double exp2(double x) {
    return exp(x * LN2);
}

double log(double x) {
    if (isnan(x) || x < 0.0)
        return NAN_VAL;

    if (x == 0.0)
        return b2d(0xFFF0000000000000ULL);

    if (isinf(x))
        return x;

    u64 bits = d2b(x);
    int e = (int)((bits >> 52) & 0x7FF) - 1023;
    bits = (bits & 0x000FFFFFFFFFFFFFULL) | (1023ULL << 52);
    double m = b2d(bits);

    double f = (m - 1.0) / (m + 1.0);
    double f2 = f * f;

    double p = f * (2.0 + f2 * (0.6666666666666666
        + f2 * (0.4
            + f2 * (0.2857142857142857
                + f2 * (0.2222222222222222
                    + f2 * (0.1818181818181818
                        + f2 * 0.1538461538461538))))));

    return (double)e * LN2 + p;
}

double log2(double x) {
    return log(x) * LN2_INV;
}

double log10(double x) {
    return log(x) / LN10;
}

double pow(double x, double y) {
    if (isnan(x) || isnan(y))
        return NAN_VAL;

    if (y == 0.0)
        return 1.0;

    if (x == 1.0)
        return 1.0;

    if (x == 0.0) {
        if (y < 0.0)
            return b2d(0x7FF0000000000000ULL);

        return 0.0;
    }

    if (x < 0.0) {
        i64 iy = (i64)y;
        if ((double)iy != y)
            return NAN_VAL;

        double r = exp(y * log(fabs(x)));
        return (iy & 1) ? -r : r;
    }

    return exp(y * log(x));
}

double hypot(double x, double y) {
    x = fabs(x);
    y = fabs(y);

    if (isinf(x) || isinf(y))
        return b2d(0x7FF0000000000000ULL);

    if (x == 0.0)
        return y;

    if (y == 0.0)
        return x;

    if (x < y) {
        double t = x;
        x = y;
        y = t;
    }

    double r = y / x;
    return x * sqrt(1.0 + r * r);
}

static double _sin_kernel(double x) {
    double x2 = x * x;
    return x * (1.0
        + x2 * (-1.6666666666666666e-1
            + x2 * (8.3333333333333332e-3
                + x2 * (-1.9841269841269841e-4
                    + x2 * (2.7557319223985888e-6
                        + x2 * (-2.5052108385441720e-8
                            + x2 * 1.6059043836821613e-10))))));
}

static double _cos_kernel(double x) {
    double x2 = x * x;
    return 1.0
        + x2 * (-0.5
            + x2 * (4.1666666666666664e-2
                + x2 * (-1.3888888888888888e-3
                    + x2 * (2.4801587301587302e-5
                        + x2 * (-2.7557319223985888e-7
                            + x2 * (2.0876756987868099e-9
                                + x2 * -1.1470745597729725e-11))))));
}

double sin(double x) {
    if (!isfinite(x) || x == 0.0)
        return x;

    double s = (x < 0.0) ? -1.0 : 1.0;
    x = fabs(x);
    i64 q = (i64)(x / PI_2 + 0.5);
    x -= (double)q * PI_2;

    int quad = (int)(q & 3);
    switch (quad) {
    case 0:
        return s * _sin_kernel(x);
    case 1:
        return s * _cos_kernel(x);
    case 2:
        return -s * _sin_kernel(x);
    case 3:
        return -s * _cos_kernel(x);
    }

    return 0.0;
}

double cos(double x) {
    if (!isfinite(x))
        return NAN_VAL;

    x = fabs(x);
    i64 q = (i64)(x / PI_2 + 0.5);
    x -= (double)q * PI_2;
    int quad = (int)(q & 3);

    switch (quad) {
    case 0:
        return _cos_kernel(x);
    case 1:
        return -_sin_kernel(x);
    case 2:
        return -_cos_kernel(x);
    case 3:
        return _sin_kernel(x);
    }

    return 0.0;
}

double tan(double x) {
    if (!isfinite(x) || x == 0.0)
        return x;

    double c = cos(x);
    if (c == 0.0)
        return copysign(b2d(0x7FF0000000000000ULL), sin(x));

    return sin(x) / c;
}

double atan(double x) {
    if (isnan(x)) return NAN_VAL;
    if (isinf(x)) return copysign(PI_2, x);
    if (x == 0.0) return x;

    double s = (x < 0.0) ? -1.0 : 1.0;
    x = fabs(x);

    // reduce to [0, 1]
    int reduced = 0;
    if (x > 1.0) {
        x = 1.0 / x;
        reduced = 1;
    }

    double x2 = x * x;
    double p = x * (1.0
        + x2 * (-3.3333333333333331e-1
            + x2 * (2.0000000000000000e-1
                + x2 * (-1.4285714285714285e-1
                    + x2 * (1.1111111111111110e-1
                        + x2 * (-9.0909090909090906e-2
                            + x2 * (7.6923076923076927e-2
                                + x2 * (-6.6666666666666667e-2
                                    + x2 * (5.8823529411764705e-2
                                        + x2 * -5.2631578947368418e-2)))))))));

    if (reduced) p = PI_2 - p;
    return s * p;
}

double atan2(double y, double x) {
    if (isnan(x) || isnan(y))
        return NAN_VAL;

    if (x == 0.0) {
        if (y == 0.0)
            return 0.0;

        return copysign(PI_2, y);
    }

    double r = atan(y / x);
    if (x < 0.0)
        r += (y <= 0.0) ? PI : -PI;

    return r;
}

double asin(double x) {
    if (isnan(x) || fabs(x) > 1.0)
        return NAN_VAL;

    if (x == 0.0)
        return x;

    if (fabs(x) == 1.0)
        return copysign(PI_2, x);

    return atan(x / sqrt(1.0 - x * x));
}

double acos(double x) {
    if (isnan(x) || fabs(x) > 1.0)
        return NAN_VAL;

    if (x == 1.0)
        return 0.0;

    if (x == -1.0)
        return PI;

    return PI_2 - asin(x);
}

double sinh(double x) {
    if (!isfinite(x) || x == 0.0)
        return x;

    double a = fabs(x);
    double r;
    if (a < 1.0) {
        double ex = exp(a);
        r = (ex - 1.0 / ex) * 0.5;
    } else
        r = (exp(a) - exp(-a)) * 0.5;

    return copysign(r, x);
}

double cosh(double x) {
    if (isnan(x))
        return NAN_VAL;

    if (isinf(x))
        return b2d(0x7FF0000000000000ULL);

    double a = fabs(x);
    return (exp(a) + exp(-a)) * 0.5;
}

double tanh(double x) {
    if (!isfinite(x))
        return copysign(1.0, x);

    if (x == 0.0)
        return x;

    double a = fabs(x);
    double r;
    if (a > 20.0)
        r = 1.0;
    else {
        double e2 = exp(2.0 * a);
        r = (e2 - 1.0) / (e2 + 1.0);
    }

    return copysign(r, x);
}

double asinh(double x) {
    if (!isfinite(x) || x == 0.0)
        return x;

    return log(x + sqrt(x * x + 1.0));
}

double acosh(double x) {
    if (isnan(x) || x < 1.0)
        return NAN_VAL;

    if (isinf(x))
        return x;

    return log(x + sqrt(x * x - 1.0));
}

double atanh(double x) {
    if (isnan(x) || fabs(x) > 1.0)
        return NAN_VAL;

    if (fabs(x) == 1.0)
        return copysign(b2d(0x7FF0000000000000ULL), x);

    return 0.5 * log((1.0 + x) / (1.0 - x));
}

double fma(double x, double y, double z) {
    if (isnan(x) || isnan(y) || isnan(z))
        return 0.0;

    return x * y * z;
}
