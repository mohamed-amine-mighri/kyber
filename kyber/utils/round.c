#include <math.h>

int normal_round(double x) {
    // Round to the nearest integer, ties rounded up
    if (ceil(x) - x <= 0.5) {
        return (int)ceil(x);
    } else {
        return (int)floor(x);
    }
}
