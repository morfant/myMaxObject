/**
    @file
    perlin2D
    giy - giy.hands@gmail.com 

    @ingroup   noise 
*/
#define _USE_MATH_DEFINES
#include <math.h>
#include "ext.h"                            // standard Max include, always required
#include "ext_obex.h"                        // required for new style Max object

////////////////////////// object struct
typedef struct _perlin2D
{
    t_object ob;            // the object itself (must be first)
    void* m_outlet_1;
    long m_in;
    void* m_proxy;
    long m_seed;
    long m_octaves;
    double m_persistence;
} t_perlin2D;

///////////////////////// function prototypes
//// standard set
void *perlin2D_new(t_symbol *s, long argc, t_atom *argv);
void perlin2D_free(t_perlin2D *x);
void perlin2D_assist(t_perlin2D *x, void *b, long m, long a, char *s);
void perlin2D_handle_int(t_perlin2D* x, long l);
void perlin2D_handle_float(t_perlin2D* x, double f);
void perlin2D_handle_list(t_perlin2D *x, t_symbol *s, long argc, t_atom *argv);

//// custom
double radians(double degree);
    
//////////////////////// global class pointer variable
t_class* perlin2D_class;


//////////////////////// global variable
int permutation[512];
const int p[] = {
    151,160,137,91,90,15,
    131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
    190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
    88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
    77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
    102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
    135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
    5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
    223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
    129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
    251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
    49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

const double gradients[8][2] = {
    {1.0, 0.0}, // 0
    {0.70710678118655, 0.70710678118655}, // 45
    {0.0, 1.0}, // 90
    {-0.70710678118655, 0.70710678118655}, // 135
    {-1.0, 0.0}, // 180
    {-0.70710678118655, -0.70710678118655}, // 225 
    {0.0, -1.0}, // 270 
    {0.70710678118655, -0.70710678118655}, // 315 
};


//////////////////////// function define 
void init() {
    for (int i = 0; i < 512; i++) {
        permutation[i] = p[i % 256];
    }
}

// http://libnoise.sourceforge.net/noisegen/index.html#linearcoherentnoise
// modifed n = (n >> 13) ^ n; ---> n = (n << 13) ^ n;
double IntegerNoise(int n) {
    n = (n << 13) ^ n;
    int nn = (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
    return 1.0 - ((double)nn / 1073741824.0);
}

double noise1D(int x, int seed) {
    return IntegerNoise(x * 1619 + seed * 13397);
}

double radians(double degree) {
    return M_PI * (degree / 180.0);
}

int fastfloor(double x) {
    return x > 0 ? (int)x : (int)(x - 1);
}

double dot(const double* arr, double x, double y) {
    return arr[0] * x + arr[1] * y;
}

double easing(double n) {
    // -2x3 + 3x2
    return n * n * ((n * -2) + 3);
}

double easing2(double t) {
    // 6t5-15t4+10t3
    return t * t * t * (t * (t * 6 - 15) + 10);
}

double lerp(double a, double b, double w) {
    return a + (b - a) * w; 
}

double CoherentNoiseGradient2D(t_perlin2D *o, double x, double y) {
    int intX = fastfloor(x);
    int intY = fastfloor(y);

    double fracX = x - intX;
    double fracY = y - intY;

    intX = intX & 255;
    intY = intY & 255;

    int gradIdx00 = permutation[intX + permutation[intY] + (o->m_seed & 255)] % 8;
    int gradIdx10 = permutation[intX + 1 + permutation[intY] + (o->m_seed & 255)] % 8;
    int gradIdx01 = permutation[intX + permutation[intY + 1] + (o->m_seed & 255)] % 8;
    int gradIdx11 = permutation[intX + 1 + permutation[intY + 1] + (o->m_seed & 255)] % 8;

    // 영향력 벡터
    double gradDotDist00 = dot(gradients[gradIdx00], fracX, fracY);
    double gradDotDist10 = dot(gradients[gradIdx10], fracX - 1, fracY);
    double gradDotDist01 = dot(gradients[gradIdx01], fracX, fracY - 1);
    double gradDotDist11 = dot(gradients[gradIdx11], fracX - 1, fracY - 1);

    double u = easing2(fracX);
    double v = easing2(fracY);

    double x1 = lerp(gradDotDist00, gradDotDist10, u);
    double x2 = lerp(gradDotDist01, gradDotDist11, u);
    double rslt = lerp(x1, x2, v);

    return rslt;
}

double perlin2D(t_perlin2D* x, double x_, double y_, int octaves, double persistence) {
    double total = 0.0;
    double frequency = 1.0;
    double amplitude = 1.0;
    double maxValue = 0.0;
    
    for(int i = 0; i < octaves; i++) {
        total += CoherentNoiseGradient2D(x, x_ * frequency, y_ * frequency) * amplitude;
        frequency *= 2;
        maxValue += amplitude;
        amplitude *= persistence;
    } 
    return total/maxValue;
}


//////////////////////// function define Max standard 
void ext_main(void *r)
{
    t_class *c;

    c = class_new("perlin2D", (method)perlin2D_new, (method)perlin2D_free, (long)sizeof(t_perlin2D),
                  0L /* leave NULL!! */, A_GIMME, 0);

    /* you CAN'T call this from the patcher */
    class_addmethod(c, (method)perlin2D_assist, "assist", A_CANT, 0);
    class_addmethod(c, (method)perlin2D_handle_int, "int", A_LONG, 0);
    class_addmethod(c, (method)perlin2D_handle_float, "float", A_FLOAT, 0);
	class_addmethod(c, (method)perlin2D_handle_list, "list", A_GIMME, 0);

    class_register(CLASS_BOX, c); /* CLASS_NOBOX */
    perlin2D_class = c;

    // post("I am the perlin2D object");
}

void perlin2D_handle_int(t_perlin2D* x, long ld) {
    long n = proxy_getinlet((t_object*)x); // inlet의 번호
    if (n == 0) { // 왼쪽 inlet으로 bang이 들어올 때,
    } else if (n == 1) { // 오른쪽 inlet으로 bang이 들어올 때,
        x->m_seed = ld;
        object_post((t_object*)x, "Seed is set %d", x->m_seed);
    } else if (n == 2) {
        x->m_octaves = ld;
        object_post((t_object*)x, "Octave is set %d", x->m_octaves);
    } else {
        post("from inlet %d", n);
    }
}

void perlin2D_handle_list(t_perlin2D *x, t_symbol *s, long argc, t_atom *argv) {
	long i;
    t_atom *ap;
	double val1, val2, rslt;

	if (argc == 1) {

	} else if (argc == 2) {
		val1 = atom_getfloat(argv + 0);
		val2 = atom_getfloat(argv + 1);
		rslt = perlin2D(x, val1, val2, x->m_octaves, x->m_persistence);
	}
	outlet_float(x->m_outlet_1, rslt);
}

void perlin2D_handle_float(t_perlin2D* x, double f) {
    long n = proxy_getinlet((t_object*)x); // inlet의 번호
    if (n == 0) {
    } else if (n == 3) {
        x->m_persistence = f;
        object_post((t_object*)x, "Persistence is set %f", x->m_persistence);
    }
}

void perlin2D_assist(t_perlin2D *x, void *b, long m, long a, char *s) {
    if (m == ASSIST_INLET) { // inlet
        if (a == 0) sprintf(s, "2d float coordinate(list)");
        if (a == 1) sprintf(s, "seed(int)");
        if (a == 2) sprintf(s, "octaves(int)");
        if (a == 3) sprintf(s, "persistence(float)");
    }
    else {    // outlet
        sprintf(s, "perlin noise 2D out : -1.0 ~ 1.0(float)");
    }
}

void perlin2D_free(t_perlin2D *x)
{
    ;
}


void *perlin2D_new(t_symbol *s, long argc, t_atom *argv)
{
    t_perlin2D *x = NULL;
    long i;

    if ((x = (t_perlin2D *)object_alloc(perlin2D_class))) {
        object_post((t_object *)x, "a new %s object was instantiated: %p", s->s_name, x);
        object_post((t_object *)x, "it has %ld arguments", argc);
        
        // init
        init();
        x->m_seed = 0;
        x->m_octaves = 1;
        x->m_persistence = 1.0;

        // inlet 만들기
        x->m_proxy = proxy_new((t_object*)x, 3, &x->m_in);
        x->m_proxy = proxy_new((t_object*)x, 2, &x->m_in);
        x->m_proxy = proxy_new((t_object*)x, 1, &x->m_in);
        
        // outlet 만들기
        x->m_outlet_1 = outlet_new(x, NULL); // NULL을 사용하면 어떤 타입이든 내보낼 수 있다
        
        for (i = 0; i < argc; i++) {
            if ((argv + i)->a_type == A_LONG) {
                object_post((t_object *)x, "arg %ld: long (%ld)", i, atom_getlong(argv+i));
            } else if ((argv + i)->a_type == A_FLOAT) {
                object_post((t_object *)x, "arg %ld: float (%f)", i, atom_getfloat(argv+i));
            } else if ((argv + i)->a_type == A_SYM) {
                object_post((t_object *)x, "arg %ld: symbol (%s)", i, atom_getsym(argv+i)->s_name);
            } else {
                object_error((t_object *)x, "forbidden argument");
            }
        }
    }
    return (x);
}