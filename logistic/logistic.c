/**
    @file
    logistic - a max object shell
    jeremy bernstein - jeremy@bootsquad.com

    @ingroup    examples
*/

#include "ext.h"                            // standard Max include, always required
#include "ext_obex.h"                        // required for new style Max object
#define INIT_P_VALUE 0.7
#define INIT_R_VALUE 2.4
#define MAX_R_VALUE 4.0

////////////////////////// object struct
typedef struct _logistic
{
    t_object ob;            // the object itself (must be first)
    void* m_outlet_1;
    float m_r, m_step_r, m_p;
    short m_year_max, m_count;
    short m_width, m_height;
    float m_rad;
} t_logistic;

///////////////////////// function prototypes
//// standard set
void *logistic_new(t_symbol *s, long argc, t_atom *argv);
void logistic_free(t_logistic *x);
void logistic_assist(t_logistic *x, void *b, long m, long a, char *s);
void logistic_handle_int(t_logistic* x, long l); // setYearMax
void logistic_handle_bang(t_logistic* x);
void logistic_generate(t_logistic* x); // generate
void logistic_handle_reset(t_logistic* x);
    
//////////////////////// global class pointer variable
void *logistic_class;


void ext_main(void *r)
{
    t_class *c;

    c = class_new("logistic", (method)logistic_new, (method)logistic_free, (long)sizeof(t_logistic),
                  0L /* leave NULL!! */, A_GIMME, 0);

    /* you CAN'T call this from the patcher */
    class_addmethod(c, (method)logistic_assist, "assist", A_CANT, 0);
    class_addmethod(c, (method)logistic_handle_int, "int", A_LONG, 0);
    class_addmethod(c, (method)logistic_handle_bang, "bang", 0);
    class_addmethod(c, (method)logistic_handle_reset, "reset", 0);

    class_register(CLASS_BOX, c); /* CLASS_NOBOX */
    logistic_class = c;

    post("I am the logistic object");
}

void logistic_handle_bang(t_logistic* x) {
    
//    long n = proxy_getinlet((t_object*)x); // inlet의 번호
//    outlet_bang(x->outlet_1);
    
//    if (n == 0) { // 왼쪽 inlet으로 bang이 들어올 때,
        logistic_generate(x);
//        logistic_reset(x);
//    } else if (n == 1) { // 오른쪽 inlet으로 bang이 들어올 때,
        
//    } else {
//        post("from inlet %d", n);
//    }
    
}

void logistic_handle_int(t_logistic* x, long ld) {
    x->m_year_max = ld;
}

void logistic_handle_reset(t_logistic* x) {
    x->m_count = 0;
    x->m_r = INIT_R_VALUE;
    x->m_p = INIT_P_VALUE;
}

void logistic_generate(t_logistic* x) {
    
    if (x->m_year_max > 0) {
        
        float pos_x, pos_y;
        for (int i = 0; i < x->m_year_max; i++) {
            x->m_p = x->m_p * x->m_r * (1.0 - x->m_p);
            
            pos_x = x->m_count + 1;
            pos_y = x->m_height * (1.0 - x->m_p);
            
            t_atom msg[8]; // paintoval l t r b r g b
            float left, top, right, bottom;
            int r, g, b;
            left = pos_x - x->m_rad;
            top = pos_y - x->m_rad;
            right = pos_x + x->m_rad;
            bottom = pos_y + x->m_rad;
            
            r = 0; g = 0; b = 0;

            atom_setsym(msg, gensym("paintoval"));
            atom_setlong(msg + 1, left);
            atom_setlong(msg + 2, top);
            atom_setlong(msg + 3, right);
            atom_setlong(msg + 4, bottom);
            atom_setlong(msg + 5, r);
            atom_setlong(msg + 6, g);
            atom_setlong(msg + 7, b);

            outlet_list(x->m_outlet_1, NULL, 8, msg);
            
        }
        
        x->m_count++;
        x->m_r = x->m_r + x->m_step_r;
        x->m_p = INIT_P_VALUE;
    }
    
    
}


void logistic_assist(t_logistic *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET) { // inlet
        sprintf(s, "I am inlet %ld", a);
    }
    else {    // outlet
        sprintf(s, "I am outlet %ld", a);
    }
}

void logistic_free(t_logistic *x)
{
    ;
}


void *logistic_new(t_symbol *s, long argc, t_atom *argv)
{
    t_logistic *x = NULL;
    long i;

    if ((x = (t_logistic *)object_alloc(logistic_class))) {
        object_post((t_object *)x, "a new %s object was instantiated: %p", s->s_name, x);
        object_post((t_object *)x, "it has %ld arguments", argc);
        
        // inlet 만들기
        
        // outlet 만들기
        x->m_outlet_1 = outlet_new(x, NULL); // NULL을 사용하면 어떤 타입이든 내보낼 수 있다
        
        // 변수 초기화
        x->m_count = 0;
        x->m_r = INIT_R_VALUE;
        x->m_step_r = 0;
        x->m_p = INIT_P_VALUE;
        x->m_year_max = 1;
        x->m_rad = 0.5f;

        
        // 아규먼트 처리
        if (argc != 2) {
            object_error((t_object*)x, "2 integer arguments (width, height) is required");
            return false; // max에서 오브젝트 생성이 안되도록 함
        } else if (argc == 2) {
            for (int i = 0; i < 2; i++) {
                if (argv[i].a_type != A_LONG) {
                    object_error((t_object*)x, "2 integer arguments (width, height) is required");
                    return false;
                }
            }

            x->m_width = atom_getlong(&argv[0]);
            x->m_height = atom_getlong(&argv[1]);
            x->m_step_r = (MAX_R_VALUE - INIT_R_VALUE) / x->m_width;
        }
        
    }
    return (x);
}
