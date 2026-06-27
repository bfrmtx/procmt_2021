/***************************************************************************
                          mtstd.h  -  description
                             -------------------
    begin                : Thu Feb 22 2001
    copyright            : (C) 2001 by B. Friedrichs
    email                : bfr@metronix.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _MTSTD_H_
#define _MTSTD_H_

#include <valarray>
#include "my_valarray.h"
#include "spectra.h"
#include "allinclude.h"
#include "bfr_iter.h"

using namespace std;


/*!  \brief Class for basic MT equations

     class calculates the bivariate equations for 5 channel MT<br>
     \f[
      \frac{Gm_2}{r^2}
     \f]
      
   
     <br>
      
     \author Dr. Bernhard Friedrichs
     \date 19.05.2004
     \version 1.0
*/

class mtstd {
public:
	mtstd();
	int init(v_spectra &x, valarray<double>& inflist, const double parzen);
	int z( v_spectra &ex,  v_spectra &ey, v_spectra &hx,  v_spectra &hy,
				 v_spectra &hz);

  size_t nominator(v_spectra &ex, v_spectra &ey,  v_spectra &hx, v_spectra &hy);
  size_t denominator(v_spectra &hx, v_spectra &hy);

	~mtstd();
	valarray<double> freq;
  valarray<double> flist; //target frequencies

	valarray<complex<double> > znxx, znxy, znyx, znyy, znd, zsxx, zsxy, zsyx, zsyy, tsx, tsy;

  valarray<complex<double> > EXEX, EYEY, EXHX, EXHY, EYHX, EYHY, HXHX, HYHY, HXHY, HYHX, HZHZ;

 size_t rhoa_phi(valarray<double> &rhoa, valarray<double> &phi, int rad, int mirror, const char *type);
 
 double parzen;            // parzen
 double ovr;               // overlay 0 ... < wl; shouldn't be greater than wl/2
 size_t mt_pts, mstart, mstop, f_begin, f_end;
	
 unsigned int pts; 				// length of the FFT; is halft of window length of the fft algorithm
 unsigned int stacks;			// total amount of spectra, each of length pts from fft
 unsigned int count;   		// counter for 0 .. stacks
 unsigned int wl;					// window length of fft algoritm = 2* pts of resulting spectra
 double sample_freq; 			// sampling frequency of the signal
 
 int is_init, is_z;
 
 	
};


#endif
