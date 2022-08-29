/**
    @file
    perlin
    gy - giy.hands@gmail.com 

    @ingroup   noise 
*/

#include "ext.h"                            // standard Max include, always required
#include "ext_obex.h"                        // required for new style Max object

////////////////////////// object struct
typedef struct _perlin
{
    t_object ob;            // the object itself (must be first)
    void* outlet_1;
    long value_prev;
    long value_current;
    long m_in;
    void* m_proxy;
} t_perlin;

///////////////////////// function prototypes
//// standard set
void *perlin_new(t_symbol *s, long argc, t_atom *argv);
void perlin_free(t_perlin *x);
void perlin_assist(t_perlin *x, void *b, long m, long a, char *s);
void perlin_handle_int(t_perlin* x, long l);
void perlin_handle_bang(t_perlin* x);
void perlin_handle_hello(t_perlin* x);
    
//////////////////////// global class pointer variable
void *perlin_class;


void ext_main(void *r)
{
    t_class *c;

    c = class_new("perlin", (method)perlin_new, (method)perlin_free, (long)sizeof(t_perlin),
                  0L /* leave NULL!! */, A_GIMME, 0);

    /* you CAN'T call this from the patcher */
    class_addmethod(c, (method)perlin_assist, "assist", A_CANT, 0);
    class_addmethod(c, (method)perlin_handle_int, "int", A_LONG, 0);
    class_addmethod(c, (method)perlin_handle_bang, "bang", 0);
    class_addmethod(c, (method)perlin_handle_hello, "hello", 0);

    class_register(CLASS_BOX, c); /* CLASS_NOBOX */
    perlin_class = c;

    post("I am the perlin object");
}

void perlin_handle_hello(t_perlin* x) {
    object_post((t_object*)x, "Hello!!!");
}

void perlin_handle_bang(t_perlin* x) {
    
    long n = proxy_getinlet((t_object*)x); // inlet의 번호
//    outlet_bang(x->outlet_1);
    
    if (n == 0) { // 왼쪽 inlet으로 bang이 들어올 때,
        
        long result = x->value_prev + x->value_current;
        
        if (result > 4660046610375530309) {
            x->value_prev = 0; // reset
            x->value_current = 1;
            object_post((t_object*)x, "Reset!");
            result = x->value_prev + x->value_current;
        }
        outlet_int(x->outlet_1, result);
        
        x->value_prev = x->value_current;
        x->value_current = result;
        
    } else if (n == 1) { // 오른쪽 inlet으로 bang이 들어올 때,
        x->value_prev = 0; // reset
        x->value_current = 1;
    } else {
        post("from inlet %d", n);
    }
    
}

void perlin_handle_int(t_perlin* x, long ld) {
    object_post((t_object*)x, "I got alksdjflkasj %ld", ld);
    outlet_bang(x->outlet_1);
}

void perlin_assist(t_perlin *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET) { // inlet
        sprintf(s, "I am inlet %ld", a);
    }
    else {    // outlet
        sprintf(s, "I am outlet %ld", a);
    }
}

void perlin_free(t_perlin *x)
{
    ;
}


void *perlin_new(t_symbol *s, long argc, t_atom *argv)
{
    t_perlin *x = NULL;
    long i;

    if ((x = (t_perlin *)object_alloc(perlin_class))) {
        object_post((t_object *)x, "a new %s object was instantiated: %p", s->s_name, x);
        object_post((t_object *)x, "it has %ld arguments", argc);
        
        // inlet 만들기
        x->m_proxy = proxy_new((t_object*)x, 1, &x->m_in);
        
        // outlet 만들기
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

//argv+0 ===> argv[0]
//argv+1 ===> argv[1]
//
//
//var freq = 200; // SC, 데이터 타입 구분 없음
//var arr = [1,2,3];
//arr[1]; ==> 2
//
//
//int x = 100; // 데이터 타입을 명확히 구분
//int x[3] = {1, 100, 23}
//x[2]; // 23
//




