/**
    @file
    perlin1D1D
    giy - giy.hands@gmail.com 

    @ingroup   noise 
*/
#define _USE_MATH_DEFINES
#include <math.h>
#include "ext.h"                            // standard Max include, always required
#include "ext_obex.h"                        // required for new style Max object

/*
1. 1d noise function - pseudo random: 같은 값이 들어가면 같은 결과


*/


////////////////////////// object struct
typedef struct _perlin1D
{
    t_object ob;            // the object itself (must be first)
    void* outlet_1;
    void* outlet_2;
    void* outlet_3;
    void* outlet_4;
    void* outlet_5;
    void* outlet_6;
    long value_prev;
    long value_current;
    long m_in;
    void* m_proxy;
    long m_seed;
    long m_octaves;
    double m_persistence;
} t_perlin1D;

///////////////////////// function prototypes
//// standard set
void *perlin1D_new(t_symbol *s, long argc, t_atom *argv);
void perlin1D_free(t_perlin1D *x);
void perlin1D_assist(t_perlin1D *x, void *b, long m, long a, char *s);
void perlin1D_handle_int(t_perlin1D* x, long l);
void perlin1D_handle_float(t_perlin1D* x, double f);
void perlin1D_handle_bang(t_perlin1D* x);
void perlin1D_handle_hello(t_perlin1D* x);
double PerlinNoise1D(t_perlin1D* x, double v1);
    
//////////////////////// global class pointer variable
t_class* perlin1D_class;
double gradients[64];

void ext_main(void *r)
{
    t_class *c;

    c = class_new("perlin1D", (method)perlin1D_new, (method)perlin1D_free, (long)sizeof(t_perlin1D),
                  0L /* leave NULL!! */, A_GIMME, 0);

    /* you CAN'T call this from the patcher */
    class_addmethod(c, (method)perlin1D_assist, "assist", A_CANT, 0);
    class_addmethod(c, (method)perlin1D_handle_int, "int", A_LONG, 0);
    class_addmethod(c, (method)perlin1D_handle_float, "float", A_FLOAT, 0);
    class_addmethod(c, (method)perlin1D_handle_bang, "bang", 0);
    class_addmethod(c, (method)perlin1D_handle_hello, "hello", 0);

    class_register(CLASS_BOX, c); /* CLASS_NOBOX */
    perlin1D_class = c;

    post("I am the perlin1D object");
}

void perlin1D_handle_hello(t_perlin1D* x) {
    object_post((t_object*)x, "Hello!!!");
}

void perlin1D_handle_bang(t_perlin1D* x) {
    
    long n = proxy_getinlet((t_object*)x); // inlet의 번호
//    outlet_bang(x->outlet_1);
    if (n == 0) { // 왼쪽 inlet으로 bang이 들어올 때,
    } else if (n == 1) { // 오른쪽 inlet으로 bang이 들어올 때,
    } else {
        post("from inlet %d", n);
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

double easing(double n) {
    // -2x3 + 3x2
    // return (n * n * n * -2) + (n * n * 3);
    return n * n * ((n * -2) + 3);
}

double easing2(double t) {
    // 6t5-15t4+10t3
    return t * t * t * (t * (t * 6 - 15) + 10);
}

double lerp(double a, double b, double w) {
    return a + (b - a) * w;
}

double CoherentNoise(double x) {
    int intX = (int)floor(x);
    double fracX = x - intX;
    double n0 = IntegerNoise(intX);
    double n1 = IntegerNoise(intX + 1);
    return lerp(n0, n1, fracX);
}

double CoherentNoiseEasing(double x) {
    int intX = (int)floor(x);
    double fracX = x - intX;
    double n0 = IntegerNoise(intX);
    double n1 = IntegerNoise(intX + 1);
    return lerp(n0, n1, easing(fracX));
}

double CoherentNoiseEasing2(double x) {
    int intX = (int)floor(x);
    double fracX = x - intX;
    double n0 = IntegerNoise(intX);
    double n1 = IntegerNoise(intX + 1);
    return lerp(n0, n1, easing2(fracX));
}

void initGradients(t_perlin1D* x, int seed) {
    x->m_seed = seed;
    for (int i = 0; i < 64; i++) {
        gradients[i] = noise1D(i, x->m_seed);
    }
} 

double CoherentNoiseGradient(double x) {
    int intX = (int)floor(x);
    double dist = x - intX;
    double pos0 = gradients[intX] * dist; // 기울기 * 거리 (y0 = ax)
    double pos1 = -1 * gradients[intX + 1] * (1 - dist); // (y1 = -1 * a1 * (1 - 거리))
    return lerp(pos0, pos1, easing2(dist));
}


double perlin1D(t_perlin1D* x, double f, int octaves, double persistence) {
    double total = 0.0;
    double frequency = 1.0;
    double amplitude = 1.0;
    double maxValue = 0.0;
    
    for(int i = 0; i < octaves; i++) {
        total += CoherentNoiseGradient(f * frequency) * amplitude;
        frequency *= 2;
        maxValue += amplitude;
        amplitude *= persistence;
    } 
    return total/maxValue;
}

void perlin1D_handle_int(t_perlin1D* x, long ld) {
 
    long n = proxy_getinlet((t_object*)x); // inlet의 번호
    if (n == 0) { // 왼쪽 inlet으로 bang이 들어올 때,
        double r = IntegerNoise((int)ld);
        outlet_float(x->outlet_1, r);
    } else if (n == 1) { // 오른쪽 inlet으로 bang이 들어올 때,
        x->m_seed = ld;
        initGradients(x, x->m_seed);
        object_post((t_object*)x, "Seed reset!");
    } else if (n == 2) {
        x->m_octaves = ld;
        object_post((t_object*)x, "Octave is set %d", x->m_octaves);
    } else {
        post("from inlet %d", n);
    }
}

void perlin1D_handle_float(t_perlin1D* x, double f) {

    long n = proxy_getinlet((t_object*)x); // inlet의 번호
    if (n == 0) {

        double r = CoherentNoise(f);
        outlet_float(x->outlet_2, r);

        double r2 = CoherentNoiseEasing(f);
        outlet_float(x->outlet_3, r2);

        double r3 = CoherentNoiseEasing2(f);
        outlet_float(x->outlet_4, r3);

        initGradients(x, x->m_seed);
        double r4 = CoherentNoiseGradient(f);
        outlet_float(x->outlet_5, r4);

        double r5 = perlin1D(x, f, x->m_octaves, x->m_persistence);
        outlet_float(x->outlet_6, r5);

    } else if (n == 3) {
        x->m_persistence = f;
        object_post((t_object*)x, "Persistence is set %f", x->m_persistence);
    }

}

void perlin1D_assist(t_perlin1D *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET) { // inlet
        sprintf(s, "I am inlet %ld", a);
    }
    else {    // outlet
        sprintf(s, "I am outlet %ld", a);
    }
}

void perlin1D_free(t_perlin1D *x)
{
    ;
}

void *perlin1D_new(t_symbol *s, long argc, t_atom *argv)
{
    t_perlin1D *x = NULL;
    long i;

    if ((x = (t_perlin1D *)object_alloc(perlin1D_class))) {
        object_post((t_object *)x, "a new %s object was instantiated: %p", s->s_name, x);
        object_post((t_object *)x, "it has %ld arguments", argc);
        
        // init
        x->m_seed = 0;
        x->m_octaves = 2;
        x->m_persistence = 0.6;

        // inlet 만들기
        x->m_proxy = proxy_new((t_object*)x, 3, &x->m_in);
        x->m_proxy = proxy_new((t_object*)x, 2, &x->m_in);
        x->m_proxy = proxy_new((t_object*)x, 1, &x->m_in);
        
        // outlet 만들기
        x->outlet_6 = outlet_new(x, NULL); // NULL을 사용하면 어떤 타입이든 내보낼 수 있다
        x->outlet_5 = outlet_new(x, NULL); // NULL을 사용하면 어떤 타입이든 내보낼 수 있다
        x->outlet_4 = outlet_new(x, NULL); // NULL을 사용하면 어떤 타입이든 내보낼 수 있다
        x->outlet_3 = outlet_new(x, NULL); // NULL을 사용하면 어떤 타입이든 내보낼 수 있다
        x->outlet_2 = outlet_new(x, NULL); // NULL을 사용하면 어떤 타입이든 내보낼 수 있다
        x->outlet_1 = outlet_new(x, NULL); // NULL을 사용하면 어떤 타입이든 내보낼 수 있다
        
        // 변수 초기화
        x->value_prev = 0;
        x->value_current = 1;

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

//argv+0 ===> &argv[0]
//argv+1 ===> &argv[1]