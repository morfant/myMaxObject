/**
	@file
	Pulse~: a simple audio object for Max
	original by: jeremy bernstein, jeremy@bootsquad.com
	@ingroup examples
*/

#include "ext.h"			// standard Max include, always required (except in Jitter)
#include "ext_obex.h"		// required for "new" style objects
#include "z_dsp.h"			// required for MSP objects
#include <algorithm>


// struct to represent the object's state
typedef struct _Pulse {
	t_pxobject		ob;			// the object itself (t_pxobject in MSP instead of t_object)
	long            m_freq; 	// the value of a property of our object
    double          m_phase;
    double          m_incr;
    double          m_duty;
    double          m_cur_duty;
    double          m_prev_duty;
    bool            m_is_freq_changed;
    bool            m_is_duty_changed;
} t_Pulse;


// method prototypes
void *Pulse_new(t_symbol *s, long argc, t_atom *argv);
void Pulse_free(t_Pulse *x);
void Pulse_assist(t_Pulse *x, void *b, long m, long a, char *s);
void Pulse_float(t_Pulse *x, double f);
void Pulse_int(t_Pulse *x, long i);
void Pulse_dsp64(t_Pulse *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void Pulse_perform64(t_Pulse *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam);

void init();

// global class pointer variable
static t_class *Pulse_class = NULL;
double g_sr;
//const int TABLELEN = 8192;
const int TABLELEN = 128;
double table[TABLELEN];


//***********************************************************************************************

void ext_main(void *r)
{
	// object initialization, note the use of dsp_free for the freemethod, which is required
	// unless you need to free allocated memory, in which case you should call dsp_free from
	// your custom free function.

	t_class *c = class_new("Pulse~", (method)Pulse_new, (method)dsp_free, (long)sizeof(t_Pulse), 0L, A_GIMME, 0);

	class_addmethod(c, (method)Pulse_float,		"float",	A_FLOAT, 0);
    class_addmethod(c, (method)Pulse_int,       "int",      A_LONG, 0);
	class_addmethod(c, (method)Pulse_dsp64,		"dsp64",	A_CANT, 0);
	class_addmethod(c, (method)Pulse_assist,	"assist",	A_CANT, 0);

	class_dspinit(c);
	class_register(CLASS_BOX, c);
	Pulse_class = c;
}


void *Pulse_new(t_symbol *s, long argc, t_atom *argv)
{
	t_Pulse *x = (t_Pulse *)object_alloc(Pulse_class);

	if (x) {
		dsp_setup((t_pxobject *)x, 2);	// MSP inlets: arg is # of inlets and is REQUIRED!
        
		// use 0 if you don't need inlets
		outlet_new(x, "signal"); 		// signal outlet (note "signal" rather than NULL)
        
        // inlet
        x->m_freq = 440;
        x->m_duty = PI;
        x->m_cur_duty = PI;
        x->m_prev_duty = x->m_cur_duty;
        
        x->m_is_freq_changed = false;
        
        init();
        
	}
	return (x);
}


// NOT CALLED!, we use dsp_free for a generic free function
void Pulse_free(t_Pulse *x)
{
	;
}


void Pulse_assist(t_Pulse *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET) { //inlet
		sprintf(s, "I am inlet %ld", a);
	}
	else {	// outlet
		sprintf(s, "I am outlet %ld", a);
	}
}


void Pulse_float(t_Pulse *x, double f)
{
    int in = proxy_getinlet((t_object*)x);
    
    switch (in) {
        case 0:
            x->m_duty = f;
           break;
            
        default:
            break;
    }
}

void Pulse_int(t_Pulse *x, long i)
{
    int in = proxy_getinlet((t_object*)x);
    
    switch (in) {
        case 1:
            if (i != x->m_freq) {
                x->m_freq = i;
                x->m_is_freq_changed = true;
            }
//            x->m_incr = (TWOPI / g_sr) * x->m_freq;
            break;
            
        default:
            break;
    }
}

void init() {
    
    // fill table
    double step = TWOPI / TABLELEN;
    for (int i = 0; i < TABLELEN; i++) {
        table[i] = sin(step * i);
    }
    
    for (int i = 0; i < TABLELEN; i++) {
        post("table %d : %f\n", i, table[i]);
    }
    
}

// registers a function for the signal chain in Max
void Pulse_dsp64(t_Pulse *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
	post("my sample rate is: %f", samplerate);
    post("my vectorsize is: %d", maxvectorsize);

	// instead of calling dsp_add(), we send the "dsp_add64" message to the object representing the dsp chain
	// the arguments passed are:
	// 1: the dsp64 object passed-in by the calling function
	// 2: the symbol of the "dsp_add64" message we are sending
	// 3: a pointer to your object
	// 4: a pointer to your 64-bit perform method
	// 5: flags to alter how the signal chain handles your object -- just pass 0
	// 6: a generic pointer that you can use to pass any additional data to your perform method
    g_sr = samplerate;
    x->m_phase = 0;
    x->m_incr = (TWOPI / g_sr) * x->m_freq;
//    x->m_incr = (TABLELEN / g_sr) * x->m_freq;

	object_method(dsp64, gensym("dsp_add64"), x, Pulse_perform64, 0, NULL);
}

double lerp(double a, double b, double w) {
    return a + (b - a) * w;
}


// this is the 64-bit perform method audio vectors
void Pulse_perform64(t_Pulse *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam)
{
	t_double *inL = ins[0];		// we get audio for each inlet of the object from the **ins argument
	t_double *outL = outs[0];	// we get audio for each outlet of the object from the **outs argument
	int n = sampleframes;
    
    double target_duty = x->m_duty;
    double slopeA, slopeB;
    
//    x->m_incr = (TABLELEN / g_sr) * x->m_freq; // for table lookup osc
    x->m_incr = (TWOPI / g_sr) * x->m_freq;

    for (int i = 0; i < n; i++) {
        // VarSaw (Triangle with var duty)
        if (x->m_phase >= TWOPI) {
            x->m_phase -= TWOPI;
            x->m_incr = (TWOPI / g_sr) * x->m_freq;
        }
         
        double duty_step = (target_duty - x->m_prev_duty) / 64;
        x->m_cur_duty = x->m_cur_duty + duty_step;
        x->m_prev_duty = x->m_cur_duty;
        std::clamp(x->m_cur_duty, 0.001, TWOPI - 0.001);
        slopeA = 2.0 / x->m_cur_duty;
        slopeB = 2.0 / (TWOPI - x->m_cur_duty);
            
        float val = x->m_phase < x->m_cur_duty ? x->m_phase * slopeA : (TWOPI - x->m_phase) * slopeB;
        x->m_phase += x->m_incr;
//        *(outL + i) = val - 1;
//        *outL++ = val - 1;
        outs[0][i] = val - 1;
        
    }
    
    /*
	// this perform method simply copies the input to the output, offsetting the value
    while (n--) {
//        *outL++ = x->m_phase <= PI ? 1 : -1; // Pulse
//        *outL++ = 2.0 * (x->m_phase * (1.0/TWOPI)) - 1; // Upward saw
//        *outL++ = 2.0 * (x->m_phase * (-1.0/TWOPI)) + 1; // Downward saw
        
        // Triangle
//        double val = 2.0 * (x->m_phase * (1.0/TWOPI)) - 1;
//        val = val < 0.0 ? -val : val;
//        *outL++ = val * 2 - 1; // Triangle
        
        // Sine
//        *outL++ = sin(x->m_phase); // sine
        
        // VarSaw (Triangle with var duty)
        if (x->m_phase >= TWOPI) {
            x->m_phase -= TWOPI;
            x->m_incr = (TWOPI / g_sr) * x->m_freq;
        }
         
        double duty_step = (target_duty - x->m_prev_duty) / 10;
        x->m_cur_duty = x->m_cur_duty + duty_step;
        x->m_prev_duty = x->m_cur_duty;
        std::clamp(x->m_cur_duty, 0.001, TWOPI - 0.001);
        slopeA = 2.0 / x->m_cur_duty;
        slopeB = 2.0 / (TWOPI - x->m_cur_duty);
            
        float val = x->m_phase < x->m_cur_duty ? x->m_phase * slopeA : (TWOPI - x->m_phase) * slopeB;
        x->m_phase += x->m_incr;
        *outL++ = val - 1;
        
//        if (x->m_is_freq_changed) {
//            if (x->m_phase >= TWOPI) {
//                x->m_incr = (TWOPI / g_sr) * freq;
//                x->m_is_freq_changed = false;
//            }
//        }
        
//        x->m_phase = fmod(x->m_phase, TWOPI);
        
//        if (x->m_phase >= TWOPI) x->m_phase -= TWOPI;
//        if (x->m_phase < 0.0) x->m_phase += TWOPI;
        
        
        // Table lookup sine osc
//        int idx = (int)x->m_phase;
//        int idxNext = (idx + 1) % TABLELEN;
//        float frac = x->m_phase - idx;
//        *outL++ = lerp(table[idx], table[idxNext], frac); // interpolation
////        *outL++ = table[idx]; // non interpolation
//        x->m_phase += x->m_incr;
//        if ((int)x->m_phase >= TABLELEN) {
//            x->m_phase -= TABLELEN;
//        }
        
    }
    */
        
}

