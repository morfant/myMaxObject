/**
	@file
	Resonator~ a simple Resonator
	giy (giy.hands@gmail.com)
	@ingroup filter
*/

#include <vector>
#include <iostream>
#include "ext.h"			// standard Max include, always required (except in Jitter)
#include "ext_obex.h"		// required for "new" style objects
#include "z_dsp.h"			// required for MSP objects

#define TWO_PI 2 * M_PI

// struct to represent the object's state
typedef struct _Resonator {
	t_pxobject		ob;			// the object itself (t_pxobject in MSP instead of t_object)
	double m_sampleRate;
	double m_fc;
	double m_bw;
	double m_delayed[2];
	double m_delayedIn[2];
} t_Resonator;


// method prototypes
void *Resonator_new(t_symbol *s, long argc, t_atom *argv);
void Resonator_free(t_Resonator *x);
void Resonator_assist(t_Resonator *x, void *b, long m, long a, char *s);
void Resonator_float(t_Resonator *x, double f);
void Resonator_dsp64(t_Resonator *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void Resonator_perform64(t_Resonator *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam);


// global class pointer variable
static t_class *Resonator_class = NULL;


//***********************************************************************************************

void ext_main(void *r)
{
	// object initialization, note the use of dsp_free for the freemethod, which is required
	// unless you need to free allocated memory, in which case you should call dsp_free from
	// your custom free function.

	t_class *c = class_new("Resonator~", (method)Resonator_new, (method)dsp_free, (long)sizeof(t_Resonator), 0L, A_GIMME, 0);

	class_addmethod(c, (method)Resonator_float,		"float",	A_FLOAT, 0);
	class_addmethod(c, (method)Resonator_dsp64,		"dsp64",	A_CANT, 0);
	class_addmethod(c, (method)Resonator_assist,	"assist",	A_CANT, 0);

	class_dspinit(c);
	class_register(CLASS_BOX, c);
	Resonator_class = c;
}


void *Resonator_new(t_symbol *s, long argc, t_atom *argv)
{
	t_Resonator *x = (t_Resonator *)object_alloc(Resonator_class);

	if (x) {
		// 2 is number of inlet(s)
		dsp_setup((t_pxobject *)x, 3);	// MSP inlets: arg is # of inlets and is REQUIRED!
		// use 0 if you don't need inlets

		outlet_new(x, "signal"); 		// signal outlet (note "signal" rather than NULL)

		// x->m_delayOffset = 1;

	}
	return (x);
}


// NOT CALLED!, we use dsp_free for a generic free function
void Resonator_free(t_Resonator *x)
{
	;
}


void Resonator_assist(t_Resonator *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET) { //inlet
		sprintf(s, "I am inlet %ld", a);
	}
	else {	// outlet
		sprintf(s, "I am outlet %ld", a);
	}
}

void Resonator_float(t_Resonator *x, double f)
{
	long in = proxy_getinlet((t_object *)x);
	// post("Resonator_float()");

	if (in == 1) {
		x->m_fc = f;
	} else if (in == 2) {
		x->m_bw = f;
	}
}

// registers a function for the signal chain in Max
void Resonator_dsp64(t_Resonator *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
	post("my sample rate is: %f", samplerate);
	post("my sample vectorsize is: %d", maxvectorsize);
	post("count: (intlet 0) %d, (intlet 1) %d, (outlet 0) %d ", count[0], count[1], count[2]);

	// instead of calling dsp_add(), we send the "dsp_add64" message to the object representing the dsp chain
	// the arguments passed are:
	// 1: the dsp64 object passed-in by the calling function
	// 2: the symbol of the "dsp_add64" message we are sending
	// 3: a pointer to your object
	// 4: a pointer to your 64-bit perform method
	// 5: flags to alter how the signal chain handles your object -- just pass 0
	// 6: a generic pointer that you can use to pass any additional data to your perform method

	object_method(dsp64, gensym("dsp_add64"), x, Resonator_perform64, 0, NULL);

	x->m_sampleRate = samplerate;

	// for (size_t i = 0; i < 2; i++) {
	// 	x->m_delIn[i] = 0.0;
	// 	x->m_delOut[i] = 0.0;
	// }

	// for (size_t i = 0; i < 2; i++) {
	// 	post("%f", x->m_delIn[i]);
	// 	post("%f", x->m_delOut[i]);
	// }

	x->m_bw = 700;
	x->m_fc = 2000;


	// int i = 0;
	// x->m_samples.clear();
	// while(i < x->m_delayOffset) {
	// 	x->m_samples.push_back(0.0);
	// 	post("x->m_samples(%d): %f", i, x->m_samples[i]);
	// 	i++;
	// }


}


// this is the 64-bit perform method audio vectors
void Resonator_perform64(t_Resonator *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam)
{
	t_double *inL = ins[0];		// we get audio for each inlet of the object from the **ins argument
	t_double *outL = outs[0];	// we get audio for each outlet of the object from the **outs argument
	int n = sampleframes;
	const double sr = x->m_sampleRate;

	// steiglitz
	// y = x + b1y[n-1] - b2y[n-2]
	// double freq = x->m_fc;
	// double bw = x->m_bw;
	// double r = 1 - TWO_PI * (x->m_bw/sr);
	// double b2 = r*r;
	// double theta = TWO_PI * (x->m_fc/sr);
	// double b1 = 2 * r * cos(theta);
	// double a = (1 - (r*r))*sin(TWO_PI*x->m_fc/sr); // Audio programming book(p.483)
	// double delayed[2] = {0.0, 0.0};


	// Audio programming book (p.487)
	// y = ax - b1y[n-1] - b2y[n-2]
	// double r, rsq, rr, costh, scal, a, b1, b2;
	// r = 1 - M_PI * (x->m_bw / sr);
	// rr = 2*r;
	// rsq = r*r;
	// costh = (rr/(1 + rsq))*cos(TWO_PI*x->m_fc/sr);
	// scal = (1 - rsq) * sin(TWO_PI*x->m_fc/sr);
	// a = scal;
	// b1 = rr*costh;
	// b2 = rsq;
	// double delayed[2] = {0.0, 0.0};

	// while(n--) {
	// 	*outL = a * (*inL++) + (b1 * delayed[0]) - (b2 * delayed[1]);
	// 	delayed[1] = delayed[0];
	// 	delayed[0] = *outL++;
	// }



	// Audio programming book (p.487 bottom)
	// y = a1x - a2x[n-2] - b1y[n-1] - b2y[n-2]
	double r, rsq, rr, costh, scal, a1, a2, b1, b2, w;
	r = 1 - M_PI * (x->m_bw / sr);
	rr = 2*r;
	rsq = r*r;
	costh = (rr/(1 + rsq))*cos(TWO_PI*x->m_fc/sr);
	scal = (1 - r);
	a1 = scal;
	a2 = r;
	b1 = rr*costh;
	b2 = rsq;
	double delayed[2] = {0.0, 0.0};
	double delayedIn[2] = {0.0, 0.0};

	// my opinion - not working
	// while(n--) {
	// 	*outL = a1 * (*inL) - (a2 * delayedIn[1]) + (b1 * delayed[0]) - (b2 * delayed[1]);
	// 	delayed[1] = delayed[0];
	// 	delayed[0] = *outL++;
	// 	delayedIn[1] = delayedIn[0];
	// 	delayedIn[0] = *inL++;
	// }

	// book code(p.487 bottom) - working
	while(n--) {
		w = a1 * (*inL++) + (b1*delayed[0]) - (b2*delayed[1]);
		*outL++ = w - (a2*delayed[1]);
		delayed[1] = delayed[0];
		delayed[0] = w;
	}

	x->m_delayed[0] = delayed[0];
	x->m_delayed[1] = delayed[1];

	x->m_delayedIn[0] = delayedIn[0];
	x->m_delayedIn[1] = delayedIn[1];
}

