/**
	@file
	HPF~ a simple High Pass Filter
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
typedef struct _HPF {
	t_pxobject		ob;			// the object itself (t_pxobject in MSP instead of t_object)
	double 			m_delayed;
	double 			m_fc; 		// cut off frequency
} t_HPF;


// method prototypes
void *HPF_new(t_symbol *s, long argc, t_atom *argv);
void HPF_free(t_HPF *x);
void HPF_assist(t_HPF *x, void *b, long m, long a, char *s);
void HPF_int(t_HPF *x, long l);
void HPF_dsp64(t_HPF *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void HPF_perform64(t_HPF *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam);


// global class pointer variable
static t_class *HPF_class = NULL;
double g_sr; // samplerate


//***********************************************************************************************
void ext_main(void *r)
{
	// object initialization, note the use of dsp_free for the freemethod, which is required
	// unless you need to free allocated memory, in which case you should call dsp_free from
	// your custom free function.

	t_class *c = class_new("HPF~", (method)HPF_new, (method)dsp_free, (long)sizeof(t_HPF), 0L, A_GIMME, 0);

	class_addmethod(c, (method)HPF_int,			"int",		A_LONG,	 0);
	class_addmethod(c, (method)HPF_dsp64,		"dsp64",	A_CANT, 0);
	class_addmethod(c, (method)HPF_assist,	"assist",	A_CANT, 0);

	class_dspinit(c);
	class_register(CLASS_BOX, c);
	HPF_class = c;
}


void *HPF_new(t_symbol *s, long argc, t_atom *argv)
{
	t_HPF *x = (t_HPF *)object_alloc(HPF_class);

	if (x) {
		// 2 is number of inlet(s)
		dsp_setup((t_pxobject *)x, 2);	// MSP inlets: arg is # of inlets and is REQUIRED!
		// use 0 if you don't need inlets

		outlet_new(x, "signal"); 		// signal outlet (note "signal" rather than NULL)
	}
	return (x);
}


// NOT CALLED!, we use dsp_free for a generic free function
void HPF_free(t_HPF *x)
{
	;
}


void HPF_assist(t_HPF *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET) { //inlet
		sprintf(s, "I am inlet %ld", a);
	}
	else {	// outlet
		sprintf(s, "I am outlet %ld", a);
	}
}


void HPF_int(t_HPF *x, long l)
{
	long in = proxy_getinlet((t_object *)x);
	if (in == 1) {
		x->m_fc = l;
	}
}


// registers a function for the signal chain in Max
void HPF_dsp64(t_HPF *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
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

	object_method(dsp64, gensym("dsp_add64"), x, HPF_perform64, 0, NULL);

	g_sr = samplerate;
	x->m_delayed = 0.0;
	x->m_fc = 1000;
}


// this is the 64-bit perform method audio vectors
void HPF_perform64(t_HPF *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam)
{
	t_double *inL = ins[0];		// we get audio for each inlet of the object from the **ins argument
	t_double *outL = outs[0];	// we get audio for each outlet of the object from the **outs argument
	int n = sampleframes;
	const double sr = g_sr;

	double c = 2 + cos(TWO_PI * x->m_fc/sr);
	double b = c - sqrt(c * c - 1);
	double a = 1 - b;
	double delayed = x->m_delayed;

	while (n--) {
		*outL = (a * (*inL++)) - (b * delayed);
		delayed = *outL++;
	}

	x->m_delayed = delayed;


	// double * delayed = &(x->m_delayed);

	// y(n) = ax(n) - by(n - 1)

	// HPF
	// a = 1 + b
	// b = 2 + cos(2*M_PI*freq/sr) - sqrt((2 + cos(2*M_PI*freq/sr))^2 - 1) 
	// double costh = 2 + cos(2*M_PI*g_freq/sr);
	// double b = costh - sqrt(costh*costh - 1);
	// double a = 1 - b;

	// HPF
	// a = 1 + b
	// b = sqrt((2 - cos(2*M_PI*freq/sr))^2 - 1) - 2 + cos(2*M_PI*freq/sr)
	// double costh = 2 - cos(2*M_PI*g_freq/sr);
	// double b = sqrt(costh*costh - 1) - costh;
	// double a = 1 + b;


	// Resonator
	// y(n) = x(n) - a2x(n - 2) - b1y(n - 1) - b2y(n - 2)
	// r = 1 - M_PI(x->m_bw/sr);
	// b1 = -(4r^2/(1+r^2))*cos(2*M_PI*g_freq/sr)
	// b2 = r^2
	// scaling = (1 - r^2)*sin(2*M_PI*g_freq/sr)
	// double freq = x->m_fc;
	// double bw = x->m_bw;
	// freq = 2000;
	// bw = 700;
	// double r = 1.0 - M_PI * (bw/sr);
	// double a2 = r;
	// double b1 = -1 * ((4*r*r)/(1 + (r*r))) * cos(2*M_PI*freq/sr);
	// double b2 = r * r;
	// double * delIn = x->m_delIn;
	// double * delOut = x->m_delOut;

	// while(n--) {
	// 	*outL = (scal * (*inL)) - (a2 * delIn[1]) + (b1 * delOut[0]) - (b2 * delOut[1]);
	// 	delIn[1] = delIn[0];
	// 	delIn[0] = *inL++;
	// 	delOut[1] = delOut[0];
	// 	delOut[0] = *outL++;
	// }


	// double rr = 2*r;
	// double rsq = r*r;
	// double costh = (rr/ (1 + rsq)) * cos(2*M_PI*freq/sr);
	// double scal = (1 - rsq) * sin(acos(costh));
	// double w;
	// double del[2] = {0.0, 0.0};

	// while(n--) {
	// 	*outL = *inL++ * scal + rr*costh*del[0] - rsq*del[1];
	// 	// *outL = w - r*del[1];
	// 	del[1] = del[0];
	// 	del[0] = *outL++;
	// }

	// for (size_t i = 0; i < 2; i++) {
	// 	x->m_delIn[i] = *delIn++;
	// 	x->m_delOut[i] = *delOut++;
	// }

	// x->m_delayed = *delayed;


	// while(m--) {
	// 	double valL = *inL++;
	// 	*outL++ = (valL + sample) * 0.5;
	// 	sample = valL;
	// }
	// x->m_sample = sample;

}

