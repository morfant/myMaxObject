/**
	@file
	filter~: a simple audio object for Max
	original by: jeremy bernstein, jeremy@bootsquad.com
	@ingroup examples
*/

#include <vector>
#include <iostream>
#include "ext.h"			// standard Max include, always required (except in Jitter)
#include "ext_obex.h"		// required for "new" style objects
#include "z_dsp.h"			// required for MSP objects


// struct to represent the object's state
typedef struct _filter {
	t_pxobject		ob;			// the object itself (t_pxobject in MSP instead of t_object)
	double 			m_sample;
	std::vector<double> 			m_samples;
	int 			m_delayOffset;
} t_filter;


// method prototypes
void *filter_new(t_symbol *s, long argc, t_atom *argv);
void filter_free(t_filter *x);
void filter_assist(t_filter *x, void *b, long m, long a, char *s);
void filter_int(t_filter *x, long l);
void filter_dsp64(t_filter *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void filter_perform64(t_filter *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam);


// global class pointer variable
static t_class *filter_class = NULL;


//***********************************************************************************************

void ext_main(void *r)
{
	// object initialization, note the use of dsp_free for the freemethod, which is required
	// unless you need to free allocated memory, in which case you should call dsp_free from
	// your custom free function.

	t_class *c = class_new("filter~", (method)filter_new, (method)dsp_free, (long)sizeof(t_filter), 0L, A_GIMME, 0);

	class_addmethod(c, (method)filter_int,			"int",		A_LONG,	 0);
	class_addmethod(c, (method)filter_dsp64,		"dsp64",	A_CANT, 0);
	class_addmethod(c, (method)filter_assist,	"assist",	A_CANT, 0);

	class_dspinit(c);
	class_register(CLASS_BOX, c);
	filter_class = c;
}


void *filter_new(t_symbol *s, long argc, t_atom *argv)
{
	t_filter *x = (t_filter *)object_alloc(filter_class);

	if (x) {
		// 2 is number of inlet(s)
		dsp_setup((t_pxobject *)x, 2);	// MSP inlets: arg is # of inlets and is REQUIRED!
		// use 0 if you don't need inlets

		outlet_new(x, "signal"); 		// signal outlet (note "signal" rather than NULL)

		x->m_delayOffset = 1;

	}
	return (x);
}


// NOT CALLED!, we use dsp_free for a generic free function
void filter_free(t_filter *x)
{
	;
}


void filter_assist(t_filter *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET) { //inlet
		sprintf(s, "I am inlet %ld", a);
	}
	else {	// outlet
		sprintf(s, "I am outlet %ld", a);
	}
}

void filter_int(t_filter *x, long l)
{
	long in = proxy_getinlet((t_object *)x);
	post("filter_int()");

	if (in == 1) {
		x->m_delayOffset = l;
		post("x->m_delayOffset: %ld", x->m_delayOffset);
		int i = 0;
		x->m_samples.clear();
		while(i < x->m_delayOffset) {
			x->m_samples.push_back(0.0);
			post("x->m_samples(%d): %f", i, x->m_samples[i]);
			i++;
		}
	}
}

// registers a function for the signal chain in Max
void filter_dsp64(t_filter *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
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

	object_method(dsp64, gensym("dsp_add64"), x, filter_perform64, 0, NULL);

	x->m_sample = 0.0;


	int i = 0;
	x->m_samples.clear();
	while(i < x->m_delayOffset) {
		x->m_samples.push_back(0.0);
		post("x->m_samples(%d): %f", i, x->m_samples[i]);
		i++;
	}


}


// this is the 64-bit perform method audio vectors
void filter_perform64(t_filter *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam)
{
	t_double *inL = ins[0];		// we get audio for each inlet of the object from the **ins argument
	t_double *outL = outs[0];	// we get audio for each outlet of the object from the **outs argument
	int n = 0;
	int m = sampleframes;
	double sample = x->m_sample;


	// this perform method simply copies the input to the output, offsetting the value
	/*
	while (n < sampleframes) {
		double valL = *inL++;
		double samp;

		if (n >= x->m_delayOffset) {
			samp = ins[0][n - x->m_delayOffset];
		} else {
			samp = x->m_samples[n];
		}

		*outL++ = (valL + samp) * 0.5;

		if (n == sampleframes - 1) {

			for (size_t i = 0; i < x->m_delayOffset; i++) {
				x->m_samples[i] = ins[0][n - (x->m_delayOffset - 1 - i)];
			}
			// x->m_samples[0] = ins[0][n - 1];
			// x->m_samples[1] = ins[0][n];

		}
		n++;
	}
	*/

	while(m--) {
		double valL = *inL++;
		*outL++ = (valL + sample) * 0.5;
		sample = valL;
	}
	x->m_sample = sample;
}

