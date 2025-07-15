/*
Danger from the Deep - Open source submarine simulation
Copyright (C) 2003-2006  Thorsten Jordan, Luis Barrancos and others.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

// an ocean wave generator (C)+(W) 2003 by Thorsten Jordan
// realized algorithms of the paper "Simulating ocean water" by
// Jerry Tessendorf.

#ifndef OCEAN_WAVE_GENERATOR
#define OCEAN_WAVE_GENERATOR

#include <common/Vector.h>
#ifdef __DARWIN__
#include <fftw3.h>
#else
#include "fftw3.h"
#endif
#include <complex>
#include <vector>

#ifdef FFTW_USE_DOUBLE
#define FFT_COMPLEX_TYPE fftw_complex
#define FFT_REAL_TYPE double
#define FFT_PLAN_TYPE fftw_plan
#define FFT_CREATE_PLAN fftw_plan_dft_c2r_2d
#define FFT_DELETE_PLAN fftw_destroy_plan
#define FFT_EXECUTE_PLAN fftw_execute
#else
#define FFT_COMPLEX_TYPE fftwf_complex
#define FFT_REAL_TYPE float
#define FFT_PLAN_TYPE fftwf_plan
#define FFT_CREATE_PLAN fftwf_plan_dft_c2r_2d
#define FFT_DELETE_PLAN fftwf_destroy_plan
#define FFT_EXECUTE_PLAN fftwf_execute
#endif

#define GRAVITY 10.0f

#ifndef M_PI
#define M_PI 3.1415926535897932	// should be in math.h, but not for Windows. *sigh*
#endif

///\brief A generator class for ocean wave height data using a statistical model and the FFT.
template <class T>
class ocean_wave_generator
{
	int N;		// grid size
	Vector W;	// wind direction
	T v;		// wind speed m/s
	T a;		// wave height scalar
	T Lm;		// tile size in m
	T w0;		// cycle time
	std::vector<std::complex<T> > h0tilde;
	std::vector<std::complex<T> > htilde;	// holds values for one fix time.
	
	ocean_wave_generator& operator= (const ocean_wave_generator& );
	ocean_wave_generator(const ocean_wave_generator& );
	static T myrnd();
	static std::complex<T> gaussrand();
	T phillips(Vector& K) const;
	std::complex<T> h0_tilde(Vector& K) const;
	void compute_h0tilde();
	std::complex<T> h_tilde(Vector& K, int kx, int ky, T time) const;
	void compute_htilde(T time);
	
	FFT_COMPLEX_TYPE *fft_in, *fft_in2;	// can't be a vector, since the type is an array
	FFT_REAL_TYPE *fft_out, *fft_out2;	// for sake of uniformity
	FFT_PLAN_TYPE plan, plan2, plan3;
	
public:
	ocean_wave_generator(
		int gridsize,
		Vector& winddir,
		T windspeed = T(20.0),
		T waveheight = T(0.0001),	// fixme: compute that automatically from Lm, etc.?
		T tilesize = T(100.0),
		T cycletime = T(10.0)
	);
	// make a (smaller) copy, gridsize must be <= owg.gridsize,
	// clearlowfreq optionally clears lower frequencies, if it is < 0, it clears higher frequencies.
	ocean_wave_generator(const ocean_wave_generator<T>& owg, int gridsize,
			     int clearlowfreq = 0);
	void set_time(T time);	// call this before any compute_*() function
	void compute_heights(Water2Points& waveheights) const;
	// use this after height computation to avoid the overhead of fft normals
	//void compute_finite_normals(const std::vector<T>& heights, std::vector<Vector >& normals) const;
	//void compute_normals(std::vector<Vector >& wavenormals) const;
	// give negative values to use default factor, positive for displacement scaling
	void compute_displacements(const T& scalefac, Water2Points& wavedisplacements) const;
	~ocean_wave_generator();
};

template <class T>
T ocean_wave_generator<T>::myrnd()
{
	return T(rand())/RAND_MAX;
}

template <class T>
std::complex<T> ocean_wave_generator<T>::gaussrand()
{
	T x1, x2, w;
	do {
		x1 = T(2.0) * myrnd() - T(1.0);
		x2 = T(2.0) * myrnd() - T(1.0);
		w = x1 * x1 + x2 * x2;
	} while ( w >= T(1.0) );
	w = sqrt( (T(-2.0) * log( w ) ) / w );
	return std::complex<T>(x1 * w, x2 * w);
}

template <class T>
T ocean_wave_generator<T>::phillips(Vector& K) const
{
	T v2 = v * v;
	T k2 = K.MagnitudeSquared();
	if (k2 == T(0)) return T(0);
	T v4 = v2 * v2;
	T k4 = k2 * k2;
	T g2 = GRAVITY * GRAVITY;
	// note: Khat = K.normal() * W should be taken, but we use K
	// and divide |K * W|^2 by |K|^2 = k2 later.
	// note: a greater exponent could be used (e.g. 6) to align waves even more to the wind
	T KdotW = K.dotP(W);
	T KdotWhat = KdotW*KdotW/k2;
	T eterm = exp(-g2 / (k2 * v4)) / k4;
	T dampfac = T(1.0/100);
	T l2 = v4/g2 * dampfac*dampfac;	// damping of very small waves
	T result = a * eterm * KdotWhat * exp(-k2*l2);	// only here the term "a" (wave height scalar) is used
	if (KdotW < T(0))	// filter out waves moving against the wind
		result *= T(0.25);
	return result;
}

template <class T>
std::complex<T> ocean_wave_generator<T>::h0_tilde(Vector& K) const
{
	//in comparison to the water engine by ??? here some randomization is missing.
	//there this complex number is multiplied with a random sinus value
	//that means a random phase. But it doesn't seem to change the appearance much.
	// T f = sin(2*M_PI*T(rand())/RAND_MAX);
	std::complex<T> g = gaussrand();	// split in two expressions to make it compileable under VC6
	T p = sqrt(T(0.5) * phillips(K));
	return g * p;	// * f;
}

template <class T>
void ocean_wave_generator<T>::compute_h0tilde()
{
	const T pi2 = T(2.0*M_PI);
	// outer parts of arrays (x2,y2 away from zero) hold higher frequencies.
	// the significant frequencies are very close to zero, anything above
	// +-N/4 or so is only very high frequency
	for (int y = 0, y2 = -N/2; y <= N; ++y, ++y2) {
		T Ky = pi2*y2/Lm;
		for (int x = 0, x2 = -N/2; x <= N; ++x, ++x2) {
			T Kx = pi2*x2/Lm;
			Vector tmp(Kx, Ky);
			h0tilde[y*(N+1)+x] = h0_tilde(tmp);
		}
	}
}

template <class T>
std::complex<T> ocean_wave_generator<T>::h_tilde(Vector& K, int kx, int ky, T time) const
{
	std::complex<T> h0_tildeK = h0tilde[ky*(N+1)+kx];
	std::complex<T> h0_tildemKconj = conj(h0tilde[(N-ky)*(N+1)+(N-kx)]);
	// all frequencies should be multiples of one base frequency (see paper).
	T wK = sqrt(GRAVITY * K.Magnitude());
	T wK2 = floor(wK/w0)*w0;
	T xp = wK2 * time;
	T cxp = cos(xp);
	T sxp = sin(xp);
	return h0_tildeK * std::complex<T>(cxp, sxp) + h0_tildemKconj * std::complex<T>(cxp, -sxp);
}

template <class T>
void ocean_wave_generator<T>::compute_htilde(T time)
{
	const T pi2 = T(2.0*M_PI);
	for (int y = 0; y <= N/2; ++y) {
		for (int x = 0; x < N; ++x) {
			Vector K(pi2*(x-N/2)/Lm, pi2*(y-N/2)/Lm);
			htilde[y*N+x] = h_tilde(K, x, y, time);
		}
	}
}

template <class T>
ocean_wave_generator<T>::ocean_wave_generator(
		int gridsize,
		Vector& winddir,
		T windspeed,
		T waveheight,
		T tilesize,
		T cycletime )
	: N(int(gridsize)), W(winddir.Normalize()), v(windspeed), a(waveheight), Lm(tilesize),
	w0(T(2.0*M_PI)/cycletime)
{
	h0tilde.resize((N+1)*(N+1));
	compute_h0tilde();
	htilde.resize(N*(N/2+1));
	fft_in = new FFT_COMPLEX_TYPE[N*(N/2+1)];
	fft_in2 = new FFT_COMPLEX_TYPE[N*(N/2+1)];
	fft_out = new FFT_REAL_TYPE[N*N];
	fft_out2 = new FFT_REAL_TYPE[N*N];
	plan = FFT_CREATE_PLAN(N, N, fft_in, fft_out, 0);
	plan2 = FFT_CREATE_PLAN(N, N, fft_in2, fft_out2, 0);
}
/*
template <class T>
ocean_wave_generator<T>::ocean_wave_generator(const ocean_wave_generator<T>& owg, int gridsize,
					      int clearlowfreq)
	: N(gridsize <= owg.N ? gridsize : owg.N), W(owg.W), v(owg.v), a(owg.a), Lm(owg.Lm), w0(owg.w0)
{
	h0tilde.resize((N+1)*(N+1));
	//copy h0 tilde instead of computing it
	int offset = (owg.N - N) / 2;
	for (int y = 0; y <= N; ++y) {
		for (int x = 0; x <= N; ++x) {
			h0tilde[y*(N+1)+x] = owg.h0tilde[(y+offset)*(owg.N+1)+(x+offset)];
		}
	}
	bool clearhigh = false;
	if (clearlowfreq < 0) {
		clearhigh = true;
		clearlowfreq = -clearlowfreq;
	}
	// clear lower frequencies if requested
	clearlowfreq = std::min(clearlowfreq, N/2+1);
	//clearlowfreq = std::max(clearlowfreq, 0);
	if (clearhigh) {
		// clear high frequencies
		for (int y = 0; y < clearlowfreq; ++y) {
			for (int x = 0; x < clearlowfreq; ++x) {
				h0tilde[y*(N+1)+x] = 0;
				h0tilde[y*(N+1)+(N-x)] = 0;
				h0tilde[(N-y)*(N+1)+x] = 0;
				h0tilde[(N-y)*(N+1)+(N-x)] = 0;
			}
		}
	} else {
		// clear low frequencies
		for (int y = N/2+1-clearlowfreq; y <= N/2-1+clearlowfreq; ++y) {
			for (int x = N/2+1-clearlowfreq; x <= N/2-1+clearlowfreq; ++x) {
				h0tilde[y*(N+1)+x] = 0;
			}
		}
	}
	htilde.resize(N*(N/2+1));
	fft_in = new FFT_COMPLEX_TYPE[N*(N/2+1)];
	fft_in2 = new FFT_COMPLEX_TYPE[N*(N/2+1)];
	fft_out = new FFT_REAL_TYPE[N*N];
	fft_out2 = new FFT_REAL_TYPE[N*N];
	plan = FFT_CREATE_PLAN(N, N, fft_in, fft_out, 0);
	plan2 = FFT_CREATE_PLAN(N, N, fft_in2, fft_out2, 0);
}
*/

template <class T>
ocean_wave_generator<T>::~ocean_wave_generator()
{
	FFT_DELETE_PLAN(plan);
	FFT_DELETE_PLAN(plan2);
	delete[] fft_in;
	delete[] fft_in2;
	delete[] fft_out;
	delete[] fft_out2;
}

template <class T>
void ocean_wave_generator<T>::set_time(T time)
{
	compute_htilde(time);
}

template <class T>
void ocean_wave_generator<T>::compute_heights(Water2Points& waveheights) const
{
	// this loop is a bit overhead, we could store htilde already in a fft_complex array
	// then we must transpose it, fftw has x*(N/2+1)+y, we use y*N+x
	// this overhead shouldn't matter.
	for (int y = 0; y <= N/2; ++y) {
		for (int x = 0; x < N; ++x) {
			const std::complex<T>& c = htilde[y*N+x];
			int ptr = x*(N/2+1)+y;
			fft_in[ptr][0] = c.real();
			fft_in[ptr][1] = c.imag();
		}
	}

	FFT_EXECUTE_PLAN(plan);
	
	// our kx,ky are in {-N/2...N/2}, but fft goes from {0...N-1}
	// so we have to add N/2 in the formulas, a term that can be seperated as exponential
	// term: exp(I*pi*(x+y)) that is equal to (-1)^(x+y), so we have to adjust
	// the sign at every second element in checkerboard form.
	// we copy the result to the output array in parallel.
	T signs[2] = { T(1), T(-1) };
	for (int y = 0; y < N; ++y)
		for (int x = 0; x < N; ++x)
		{
			Vector &point = waveheights.getPoint(x, y);
			point[2] = (T) fft_out[y*N+x] * signs[(x + y) & 1];
		}
}

/*

template <class T>
void ocean_wave_generator<T>::compute_finite_normals(const std::vector<T>& heights,
						     std::vector<Vector >& normals) const
{
	if (normals.size() != N*N)
		normals.resize(N*N);

	typename std::vector<Vector >::iterator it = normals.begin();
	T sf = Lm/T(N);
	for (int y = 0; y < N; ++y) {
		int y1 = (y+N-1)%N;
		int y2 = (y+1)%N;
		for (int x = 0; x < N; ++x) {
			int x1 = (x+N-1)%N;
			int x2 = (x+1)%N;
			Vector a((x-1)*sf, (y+1)*sf, heights[y2*N+x1]);
			Vector b((x-1)*sf, (y  )*sf, heights[y *N+x1]);
			Vector c((x-1)*sf, (y-1)*sf, heights[y1*N+x1]);
			Vector d((x  )*sf, (y-1)*sf, heights[y1*N+x ]);
			Vector e((x+1)*sf, (y-1)*sf, heights[y1*N+x2]);
			Vector f((x+1)*sf, (y  )*sf, heights[y *N+x2]);
			Vector g((x+1)*sf, (y+1)*sf, heights[y2*N+x2]);
			Vector h((x  )*sf, (y+1)*sf, heights[y2*N+x ]);
			Vector ortho = (a.cross(b) + b.cross(c) + c.cross(d) + d.cross(e) + e.cross(f) + f.cross(g) + g.cross(h) + h.cross(a));
			*it++ = ortho.normal();
		}
	}
}

template <class T>
void ocean_wave_generator<T>::compute_normals(std::vector<Vector >& wavenormals) const
{
	const T pi2 = T(2.0)*T(M_PI);
	for (int y = 0; y <= N/2; ++y) {
		for (int x = 0; x < N; ++x) {
			const std::complex<T>& c = htilde[y*N+x];
			Vector K(pi2*(x-N/2)/Lm, pi2*(y-N/2)/Lm);
			int ptr = x*(N/2+1)+y;
			fft_in[ptr][0] = -c.imag() * K.x;
			fft_in[ptr][1] =  c.real() * K.x;
			fft_in2[ptr][0] = -c.imag() * K.y;
			fft_in2[ptr][1] =  c.real() * K.y;
		}
	}

	FFT_EXECUTE_PLAN(plan);
	FFT_EXECUTE_PLAN(plan2);
	
	if (wavenormals.size() != N*N)
		wavenormals.resize(N*N);
	T signs[2] = { T(1), T(-1) };
	unsigned ptr = 0;
	for (int y = 0; y < N; ++y) {
		for (int x = 0; x < N; ++x) {
			T s = signs[(x + y) & 1];
			wavenormals[ptr] = Vector(-fft_out[ptr] * s, -fft_out2[ptr] * s, T(1)).normal();
			++ptr;
		}
	}
}
*/

template <class T>
void ocean_wave_generator<T>::compute_displacements(const T& scalefac,
						    Water2Points& wavedisplacements) const
{
	for (int y = 0; y <= N/2; ++y) {
		for (int x = 0; x < N; ++x) {
			const std::complex<T>& c = htilde[y*N+x];
			Vector K((x-N/2), (y-N/2));	// the factor 2*PI/Lm gets divided out later, so we don't need to multiply it.
			T k = K.Magnitude();
			Vector Kh;
			if (k != 0)
				Kh = K * (T(1)/k);
			else
				Kh[0] = Kh[1] = T(0);
			int ptr = x*(N/2+1)+y;
			fft_in[ptr][0] =  c.imag() * Kh[0];
			fft_in[ptr][1] = -c.real() * Kh[0];
			fft_in2[ptr][0] =  c.imag() * Kh[1];
			fft_in2[ptr][1] = -c.real() * Kh[1];
		}
	}

	FFT_EXECUTE_PLAN(plan);
	FFT_EXECUTE_PLAN(plan2);
	
	T signs[2] = { T(1), T(-1) };
	unsigned ptr = 0;
	for (int y = 0; y < N; ++y) {
		for (int x = 0; x < N; ++x) {
			T s = signs[(x + y) & 1];

			Vector &point = wavedisplacements.getPoint(x, y);
			point[0] = (T) fft_out[ptr] * s * scalefac;
			point[1] = (T) fft_out2[ptr] * s * scalefac;
			++ptr;
		}
	}
}

#endif
