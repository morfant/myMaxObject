

#include "stdio.h"
#include "math.h"


double perlin_1d_noise(double v1) {

    double iptr;
    double f;
    f = modf(sin(v1) * 43758.5453123, &iptr);

    printf("f: %f\n", f);

    return f;
}


// http://libnoise.sourceforge.net/noisegen/index.html#linearcoherentnoise
double IntegerNoise (int n)
{
    n = (n >> 13) ^ n;
    int nn = (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
    return 1.0 - ((double)nn / 1073741824.0); // -1.0 ~ 1.0
}


double lerp(a, b, w) {
    // lerp(a, b, w) = a * (1 - w) + b * w
    return a + (b - a) * w;
}

double CoherentNoise (double x) {
    int intX = (int)floor(x);
    double n0 = IntegerNoise(intX);
    double n1 = IntegerNoise(intX + 1);
    double w = x - intX;
    double noise = lerp(n1, n1, w);
    return noise;
}



int main(int argc, char * argv[]) {

    size_t i = 0;
    while (i < 100) {
        // perlin_1d_noise(i);
        printf("Result of IntegerNoise(%ld): %f\n", i, IntegerNoise(i));
        i++;
    }


    return 0;
}