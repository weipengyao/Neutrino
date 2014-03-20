/*
 *
 *    Copyright (C) 2013 Alessandro Flacco, Tommaso Vinci All Rights Reserved
 * 
 *    This file is part of nPhysImage library.
 *
 *    nPhysImage is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    nPhysImage is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with neutrino.  If not, see <http://www.gnu.org/licenses/>.
 *
 *    Contact Information: 
 *	Alessandro Flacco <alessandro.flacco@polytechnique.edu>
 *	Tommaso Vinci <tommaso.vinci@polytechnique.edu>
 *
 */
// functions for wavelet analysis
#include <iostream>
#include <cstring>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_const_mksa.h>
#include <gsl/gsl_dht.h>
#include <gsl/gsl_sf_bessel.h>


#include <list>

#include "nPhysImageF.h"
#include "nPhysMaths.h"
#include "nPhysTools.h"
#include "mcomplex.h"
#include "bidimvec.h"

//using namespace std;

#ifndef __nPhysWave_h
#define __nPhysWave_h

enum unwrap_strategy {GOLDSTEIN, QUALITY, SIMPLE_HV, SIMPLE_VH, MIGUEL};

struct wavelet_params_str {
	wavelet_params_str()
	{ iter_ptr = &iter; }

	double init_angle;
	double end_angle;
	size_t n_angles;

	double init_lambda;
	double end_lambda;
	size_t n_lambdas;

	int thickness;
	double damp;
	
	int iter;
	int *iter_ptr;
};
typedef struct wavelet_params_str wavelet_params;



//struct convolution_result_str {
//public:
//	convolution_result_str()
//		: I(0)
//	{ } 
//	
//	convolution_result_str(int)
//		: I(0)
//	{ } 
//
//	// (cosa cazzo gli servono questi?? creeepy...
//	convolution_result_str operator+ (const convolution_result_str &oth)
//	{ return *this; }
//	convolution_result_str operator- (convolution_result_str &oth)
//	{ return *this; }
//	convolution_result_str operator* (convolution_result_str &oth)
//	{ return *this; }
//	
//	operator convolution_result_str & ()
//	{ return *this; }
//
//	double I;
//	double phi;
//	double lambda;
//	double angle;
//};
//typedef struct convolution_result_str convolution_result;

// calculate wavelet field

// std::list<nPhysImageF<double> *> phys_wavelet_field_2D(nPhysImageF<double> &, wavelet_params &);
std::list<nPhysImageF<double> *> phys_wavelet_field_2D_morlet(nPhysImageF<double> &, wavelet_params &);
bool cudaEnabled();
std::list<nPhysImageF<double> *> phys_wavelet_field_2D_morlet_cuda(nPhysImageF<double> &, wavelet_params &);

// unwrap methods
nPhysImageF<double> *phys_phase_unwrap(nPhysImageF<double> &, nPhysImageF<double> &, enum unwrap_strategy);

// carrier subtraction
void phys_subtract_carrier (nPhysImageF<double> &, double, double);

// create a synthetic interferogram from phase and quality
void phys_synthetic_interferogram (nPhysImageF<double> &, nPhysImageF<double> &, nPhysImageF<double> &);

bidimvec<double> phys_guess_carrier(nPhysImageF<double> &, double=1.0);

// integral inversions

// numerical approach for inversion
enum inversion_algo {ABEL = 10, ABEL_HF = 20};
enum inversion_physics { ABEL_GAS, ABEL_PLASMA, ABEL_NONE };
struct abel_params_str {
	abel_params_str()
		: iter_ptr(0)
	{ }

	std::vector<phys_point> iaxis;
	phys_direction idir;
	inversion_algo ialgo;
	inversion_physics iphysics;
	
	int iter;
	int *iter_ptr;
};
typedef struct abel_params_str abel_params;

// main inversion function
nPhysImageF<double> * phys_invert_abel(nPhysImageF<double> &, abel_params &);

// inline inversion maths
inline void phys_invert_abel_1D(double *ivec, double *ovec, size_t size)
{
	// FIRST element of *ptr MUST BE on symmetry axis
// 	double integral;
	int size_dy = size-1;

	if (ivec==NULL || ovec==NULL || size==0)
		return;

	// 0. init 
	//memset(ovec, 0, size*sizeof(sizeof(double))); ??????
	memset(ovec, 0, size*sizeof(double));


	// 1. dy - mind that this shifts the function to semi-integer indexes
	double dy[size_dy];
	double func_x[size_dy];
	for (int i=0; i<size_dy; i++)
		dy[i] = (ivec[i+1] - ivec[i]);
		//dy[i] = 2.*__ifg_pi*(y[i+1] - y[i]); taken care of elsewhere

	// 2. integral
	int R0_idx = size-1; 			// considering R=0 for the first index
	double i_dy;
	for (int R_idx=0; R_idx<R0_idx; R_idx++) {		// looping on function
		memset(func_x, 0, size_dy*sizeof(double));
		for (int x_idx = (R_idx)+1; x_idx < R0_idx; x_idx++) {
			//double x = (double)x_idx;
			double x = (double)x_idx-0.5;
			double R = (double)R_idx;
			i_dy = .5*(dy[x_idx]+dy[x_idx-1]);

			func_x[R_idx] = i_dy/sqrt(x*x - R*R);
			ovec[R_idx] += func_x[R_idx];
		}
		// first index is R_idx, last index is (R0_idx-2): trapezioid integral correction

		ovec[R_idx] -= 0.5*(func_x[R_idx]+func_x[R0_idx-1]);
		ovec[R_idx] /= 3.1415;
		ovec[R_idx] *= -1.;

	}

	// shift of half index on the right
	double prev_p = ovec[0];
	for (register size_t ii=1; ii<size; ii++) {
		ovec[ii] = .5*(prev_p+ovec[ii]);
		prev_p = ovec[ii];
	}
	
	// zero axis
	ovec[0] = 0;

	return;
}

// bessel LUT
struct bessel_str {
public:
	bessel_str()
		: N(0), lut(0), cos_lut(0)
	{ }

	int N;
	double *lut;
	double *cos_lut;
};
typedef struct bessel_str bessel_alloc_t;


bessel_alloc_t *bessel_allocate();
void bessel_free(bessel_alloc_t *);


inline void phys_invert_abelHF_1D(double *ivec, double *ovec, size_t size, bessel_alloc_t *lut)
{
	if (ivec==NULL || ovec==NULL || size==0)
		return;

	// 0. init 
	memset(ovec, 0, size*sizeof(double));

	// check Bessel LUT
	int N=size;
	if (N != lut->N) {
		// recalculate lut
		DEBUG(10,"recalculate Bessel LUT for N="<<N<<"...");
		if (lut->lut != NULL)
			delete lut->lut;

		lut->N = N;
		lut->lut = new double[N*N];

		for (int j=0; j<N; j++) {
			for (int k=0; k<j+1; k++) {
				// direct cosine
				//lut->lut[k*j] = gsl_sf_bessel_J0(j*k/(2.*N+1.));

				// DCT 
				// (change in integration variable to respect FFTW-DCT definition)
				lut->lut[k*j] = gsl_sf_bessel_J0(M_PI*j*k/N);
			}
		}
		DEBUG(10,"done");

	}

	// sara' da trovare una soluzione rapida per il plan
	double Fivec[size];
	std::memset(Fivec, 0, size*sizeof(double));

	fftw_plan r2rplan = fftw_plan_r2r_1d(size, ivec, (double *)Fivec, FFTW_REDFT00, FFTW_ESTIMATE);
	
	fftw_execute(r2rplan);

	for (size_t j=0; j<size; j++) {
	
	//	for (int i=-N; i<N; i++) { // this was for Cosinus transform
			for (int k=0; k<N; k++) {
				// direct cosinus
				//inv_y[j] += 2 * in[i] * cos(i*k/(2.*N+1.)) * k * gsl_sf_bessel_J0(j*k/(2.*N+1.));
				//ovec[j] += ivec[abs(i)] * cos(i*k/(2.*N+1.)) * k * lut->lut[j*k];
				//ovec[j] += ivec[abs(i)] * lut->cos_lut[abs(i)*k] * k * lut->lut[j*k];

				// fast DCT
				//ovec[j] += Fivec[k] * k * lut->lut[j*k];
				ovec[j] += Fivec[k] * (M_PI/N) * k * lut->lut[j*k];
			}
	//	}
		//ovec[j] *= (1/(2*3.14*pow(2.*N+1.,2)));
		ovec[j] *= 1./(2.*N);
	}


/*	for (size_t j=0; j<size; j++) {
	
		for (int i=-N; i<N; i++) {
			for (int k=0; k<N; k++) {
				//inv_y[j] += 2 * in[i] * cos(i*k/(2.*N+1.)) * k * gsl_sf_bessel_J0(j*k/(2.*N+1.));
				//ovec[j] += ivec[abs(i)] * cos(i*k/(2.*N+1.)) * k * lut->lut[j*k];
				ovec[j] += ivec[abs(i)] * lut->cos_lut[abs(i)*k] * k * lut->lut[j*k];
			}
		}
		ovec[j] *= (1/(2*3.14*pow(2.*N+1.,2)));
	}*/

	return;
}


// inversion physics
void phys_apply_inversion_plasma(nPhysImageF<double> &, double, double);
void phys_apply_inversion_gas(nPhysImageF<double> &, double, double, double);

#endif
