/***************************************************************************
                          mtstd.cpp  -  description
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

#include "mtstd.h"

mtstd::mtstd(){
	pts = stacks = count = wl = 0;
	sample_freq = 0.; f_begin = f_end = 0;
}
mtstd::~mtstd(){
}


int mtstd::init(v_spectra &x, valarray<double>& inflist, const double parzen) {

  
  if(!x.spc.size()) {
		cerr << "mtstd::mtstd -> init with empty spectra! BYE" << endl;
		exit(0);
	}

  
  sample_freq = x.get_sample_freq();
	is_z = 0;
	
  freq.resize(x.freq.size());
  freq = x.freq;
  ovr = x.get_ovr();
  
  
  
  // make a doublette check
  
  // sort in if neccessary
  if (inflist[1] > inflist[2]) reverse(&inflist[0], &inflist[inflist.size()]);
   
  
  // estimate start stop and size 
  mt_pts = my_valarray::parzen_length(freq, inflist, parzen, mstart, mstop);
  
  // all what we need
  flist.resize(mt_pts);
  copy(&inflist[mstart], &inflist[mstop], &flist[0]);
  wl = x.get_wl();
	pts = x.get_pts();
	stacks = x.get_stacks();
  this->parzen = parzen;
  	
	
  
  
  
  
  cerr << mstart << mstop << endl;
  cerr << "mtsdt::init MT spectra size is " << mt_pts << ", parzen: " << parzen << endl;
  cerr << "input size " << x.spc.size() << " frequency range (from list): " << flist[0] << " - " << flist[flist.size()-1] <<  endl;
  my_valarray::possible_parzening(freq, parzen, f_begin, f_end);
  cerr << "input freqs: "  << freq[0] << " - " << freq[freq.size()-1] << " ppr: " << freq[f_begin] << " - " << freq[f_end-1] << endl;
  
  EXEX.resize(mt_pts * stacks);
  EYEY.resize(mt_pts * stacks);
  
  EXHX.resize(mt_pts * stacks);
  EXHY.resize(mt_pts * stacks);
  
  EYHX.resize(mt_pts * stacks);
  EYHY.resize(mt_pts * stacks);
  
  HXHX.resize(mt_pts * stacks);
  HYHY.resize(mt_pts * stacks);
  
  HXHY.resize(mt_pts * stacks);
  HYHX.resize(mt_pts * stacks);
  
  
  HZHZ.resize(mt_pts * stacks);

 
  
 
 return is_init = 1;
}



int mtstd::z( v_spectra &ex,  v_spectra &ey, v_spectra &hx,  v_spectra &hy,
				 v_spectra &hz) {

	if (!ex.is_spec) {
			cerr << "mtstd::z -> raw spectrum of Ex not calculated" << endl;
			exit(0);
	}
	
  
  
	if (!ey.is_spec) {
			cerr << "mtstd::z -> raw spectrum of Ey not calculated" << endl;
			exit(0);
	}
	
  if (!hx.is_spec) {
			cerr << "mtstd::z -> raw spectrum of Hx not calculated" << endl;
			exit(0);
	}
	if (!hx.is_conj) {
			cerr << "mtstd::z -> conjugated raw spectrum of Hx not calculated";
			hx.single("conj");
			cerr << "... calculated" << endl;
	}

	if (!hy.is_spec) {
			cerr << "mtstd::z -> raw spectrum of Hy not calculated" << endl;
			exit(0);
	}

	if (!hy.is_conj) {
			cerr << "mtstd::z -> conjugated raw spectrum of Hy not calculated";
			hy.single("conj");
			cerr << "... calculated" << endl;
	}

	if (!hz.is_spec) {
			cerr << "mtstd::z -> raw spectrum of Hz not calculated" << endl;
			exit(0);
	}
	if (!hz.is_conj) {
			cerr << "mtstd::z -> conjugated raw spectrum of Hz not calculated";
			hz.single("conj");
			cerr << "... calculated" << endl;
	}

  size_t i, j, tmp_stack;;
  
  
   mtstd::denominator(hx, hy);
   mtstd::nominator(ex, ey, hx, hy);
   
  
  
  valarray<complex<double> > unstacked(mt_pts*stacks),  temps(mt_pts);
  valarray<double> unstacked2(mt_pts*stacks);
  valarray<double> one_f(stacks), s3(mt_pts), median_mt(mt_pts);
  
  double median_rg = 30;
 
  valarray<double> flist2(mt_pts);
	flist2 = 1./flist;
  
  zsxx.resize(mt_pts);
  zsxy.resize(mt_pts);
  zsyx.resize(mt_pts);
  zsyy.resize(mt_pts);
  
  my_valarray::stack(temps, HXHX, temps.size(), 1, 0, tmp_stack);
 
  unstacked = znxx / znd;
  my_valarray::stack(zsxx, unstacked , zsxy.size(), 1, 0, tmp_stack);  
  
	
	
	
	
  unstacked = znxy / znd;
  my_valarray::stack(zsxy, unstacked , zsxy.size(), 1, 0, tmp_stack);  
  
	
  for (i = 0; i < unstacked.size(); i++) unstacked2[i] = abs(unstacked[i]);
	 my_valarray::vwrite_st_ust(flist, unstacked2, "zxy_raw.dat",1,0,0);
	
	 for(j = 0; j < mt_pts; j++) {
  	one_f = unstacked2[slice(j, stacks, mt_pts)];
    median_mt[j] =  my_valarray::median_range(one_f, median_rg);
  }
	for (i = 0; i < median_mt.size(); i++)  median_mt[i] = 1000000.  *(mue0 / (2 * M_PI * flist[i])) * median_mt[i];
	my_valarray::vwrite(flist2, median_mt, "zxymed.dat",0, 0, 0);
	
	
	
	
	
	
	
  unstacked = znyx / znd;
  my_valarray::stack(zsyx, unstacked , zsxy.size(), 1, 0, tmp_stack);  
  
	for (i = 0; i < unstacked.size(); i++) unstacked2[i] = abs(unstacked[i]);
	
	for(j = 0; j < mt_pts; j++) {
  	one_f = unstacked2[slice(j, stacks, mt_pts)];
    median_mt[j] =  my_valarray::median_range(one_f, median_rg);
  }
	for (i = 0; i < median_mt.size(); i++)  median_mt[i] = 1000000.  *(mue0 / (2 * M_PI * flist[i])) * median_mt[i];
	my_valarray::vwrite(flist2, median_mt, "zyxmed.dat",0, 0, 0);
		
	
	
	
  unstacked = znyy / znd;
  my_valarray::stack(zsyy, unstacked , zsxy.size(), 1, 0, tmp_stack);  
  
  for (i = 0; i < s3.size(); i++) s3[i] = real(temps[i]);
  my_valarray::vwrite(flist, s3, "HX.dat",0, 0, 0);
  
  
  
  
  
  

return is_z = 1;
}

size_t mtstd::nominator(v_spectra &ex, v_spectra &ey,  v_spectra &hx, v_spectra &hy) {

   cerr << "mtstd::nominator ... ";
   size_t i;
   znxx.resize(mt_pts * stacks);
   znxy.resize(mt_pts * stacks);
   znyx.resize(mt_pts * stacks);
   znyy.resize(mt_pts * stacks);
   
   for (i = 0; i < stacks; i++) {
    
    
    bfrtsmp::parzening(&ex.spc[i*pts], &ex.spc[i*pts+pts], 
                           &hx.spc_conj[i*pts], &hx.spc_conj[i*pts+pts], 
                           &EXHX[i*mt_pts], &EXHX[i*mt_pts+mt_pts], 
                           &freq[0], &freq[f_begin], &freq[f_end],
                           &flist[0], &flist[flist.size()], 
                            parzen);
    
    bfrtsmp::parzening(&ex.spc[i*pts], &ex.spc[i*pts+pts], 
                           &hy.spc_conj[i*pts], &hy.spc_conj[i*pts+pts], 
                           &EXHY[i*mt_pts], &EXHY[i*mt_pts+mt_pts], 
                           &freq[0], &freq[f_begin], &freq[f_end],
                           &flist[0], &flist[flist.size()], 
                            parzen);
    
    
    bfrtsmp::parzening(&ey.spc[i*pts], &ey.spc[i*pts+pts], 
                           &hx.spc_conj[i*pts], &hx.spc_conj[i*pts+pts], 
                           &EYHX[i*mt_pts], &EYHX[i*mt_pts+mt_pts], 
                           &freq[0], &freq[f_begin], &freq[f_end],
                           &flist[0], &flist[flist.size()], 
                            parzen);
    
    bfrtsmp::parzening(&ey.spc[i*pts], &ey.spc[i*pts+pts], 
                           &hy.spc_conj[i*pts], &hy.spc_conj[i*pts+pts], 
                           &EYHY[i*mt_pts], &EYHY[i*mt_pts+mt_pts], 
                           &freq[0], &freq[f_begin], &freq[f_end],
                           &flist[0], &flist[flist.size()], 
                            parzen);

    
    
    }
   
   
   
   znxy = (HXHX * EXHY) + (HXHY * EXHX);
   znyx = (HYHY * EYHY) + (HYHX * EYHY);
   
   znyy = (HXHX * EYHY) + (HXHY * EYHX);
   znxx = (HYHY * EXHY) + (HYHX * EXHY);
   
   
   
//   zn = ((h1.spc * h1.spc_conj) * (e.spc * h2.spc_conj))
//      + ((h1.spc * h2.spc_conj) * (e.spc * h1.spc_conj)); // should be -
   cerr << " .. done" << endl;

   
  //  my_valarray::vwrite(zn, "zn.dat", 1, 0);
return znxy.size();
}


size_t mtstd::denominator(v_spectra &hx, v_spectra &hy) {


 cerr << "mtstd::denominator ... ";
 znd.resize(mt_pts * stacks);
 size_t i;
  for (i = 0; i < stacks; i++) {
    
    
    bfrtsmp::parzening(&hx.spc[i*pts], &hx.spc[i*pts+pts], 
                           &hx.spc_conj[i*pts], &hx.spc_conj[i*pts+pts], 
                           &HXHX[i*mt_pts], &HXHX[i*mt_pts+mt_pts], 
                           &freq[0], &freq[f_begin], &freq[f_end],
                           &flist[0], &flist[flist.size()], 
                            parzen);
    
    bfrtsmp::parzening(&hx.spc[i*pts], &hx.spc[i*pts+pts], 
                           &hy.spc_conj[i*pts], &hy.spc_conj[i*pts+pts], 
                           &HXHY[i*mt_pts], &HXHY[i*mt_pts+mt_pts], 
                           &freq[0], &freq[f_begin], &freq[f_end],
                           &flist[0], &flist[flist.size()], 
                            parzen);
    
    bfrtsmp::parzening(&hy.spc[i*pts], &hy.spc[i*pts+pts], 
                           &hy.spc_conj[i*pts], &hy.spc_conj[i*pts+pts], 
                           &HYHY[i*mt_pts], &HYHY[i*mt_pts+mt_pts], 
                           &freq[0], &freq[f_begin], &freq[f_end],
                           &flist[0], &flist[flist.size()], 
                            parzen);
  
    bfrtsmp::parzening(&hy.spc[i*pts], &hy.spc[i*pts+pts], 
                           &hx.spc_conj[i*pts], &hx.spc_conj[i*pts+pts], 
                           &HYHX[i*mt_pts], &HYHX[i*mt_pts+mt_pts], 
                           &freq[0], &freq[f_begin], &freq[f_end],
                           &flist[0], &flist[flist.size()], 
                            parzen);
    
   }
   
   znd = (HXHX * HYHY) + (HXHY * HYHX);
   cerr << " .. done" << endl;
 
return znd.size();
}



size_t mtstd::rhoa_phi(valarray<double> &rhoa, valarray<double> &phi, int rad, int mirror, const char *type) {

 size_t i, j;

 if (!strcmp(type, "zxy")) {
   cerr << "mtstd::rho_a_phi -> " << type << flush;
  rhoa.resize(mt_pts);
  phi.resize(mt_pts);

  //for (i = 0; i < rhoa.size(); i++) rhoa[i] = (mue0 / (2 * M_PI * flist[i] * flist[i])) * abs(zsxy[i]);
  for (i = 0; i < rhoa.size(); i++) rhoa[i] = (mue0 / (2 * M_PI * flist[i])) * abs(zsxy[i]);
  for (i = 0; i < phi.size();  i++) phi[i]  = arg(zsxy[i]);
  if (!rad) for (i = 0; i < phi.size(); i++) phi[i] *= 180. / M_PI;
  /*
    for (i = 0; i < stacks; i++){
     for (j = 1; j < pts; j++) {
       rhoa[i+j] = (mue0 / (2 * M_PI * freq[j])) * abs(zsxy[i+j]);
     }
    }
   for (i = 1; i < phi.size(); i++) phi[i] = arg(zsxy[i]);
   if (!rad) for (i = 1; i < phi.size(); i++) phi[i] *= 180. / M_PI;
   
   */
   cerr << " ... done" << flush << endl;
 }
if (!strcmp(type, "zyx")) {
   cerr << "mtstd::rho_a_phi -> " << type << flush;
  rhoa.resize(mt_pts);
  phi.resize(mt_pts);

  //for (i = 0; i < rhoa.size(); i++) rhoa[i] = (mue0 / (2 * M_PI * flist[i] * flist[i])) * abs(zsxy[i]);
  for (i = 0; i < rhoa.size(); i++) rhoa[i] = (mue0 / (2 * M_PI * flist[i])) * abs(zsyx[i]);
  for (i = 0; i < phi.size();  i++) phi[i]  = arg(zsyx[i]);
  if (!rad) for (i = 0; i < phi.size(); i++) phi[i] *= 180. / M_PI;
  /*
    for (i = 0; i < stacks; i++){
     for (j = 1; j < pts; j++) {
       rhoa[i+j] = (mue0 / (2 * M_PI * freq[j])) * abs(zsxy[i+j]);
     }
    }
   for (i = 1; i < phi.size(); i++) phi[i] = arg(zsxy[i]);
   if (!rad) for (i = 1; i < phi.size(); i++) phi[i] *= 180. / M_PI;
   
   */
   cerr << " ... done" << flush << endl;
 }

return rhoa.size();

}



