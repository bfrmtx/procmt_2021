/***************************************************************************
 *   Copyright (C) 2003 by Bernhard Friedrichs                             *
 *   bfriedr@isi2.metronix.de                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "coherency.h"


/***************************************************************************
 *                                                                         *
 *   coherency  file section                                               *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

// do not call with parameters so that class coherency can be initialized in a loop
coherency::coherency() {
  is_init = 0;
  is_quad_co_median =  is_coh_med = is_coh_med_range = 0;
  is_coh = is_quad_co = is_quad_co_mean = is_quad_co_median = is_quad_co_median_range = 0;
}

coherency::~coherency() {
}

int coherency::init(v_spectra &v) {
// details see class v_spectra
  wl = v.get_wl();
  pts = v.get_pts();
  stacks = v.get_stacks();
  size = stacks * pts;
  freq.resize(v.freq.size());
  freq = v.freq;
  sample_freq = v.get_sample_freq();
  
   is_quad_co_median =  is_coh_med = is_coh_med_range = 0;
  is_coh = is_quad_co = is_quad_co_mean = is_quad_co_median = is_quad_co_median_range = 0;

return is_init = 1;
}

// quadrature and co-spectrum
int coherency::quad_co(int stack_it,  int imedian, double range, const v_spectra &x, const v_spectra &y) {

// vergleichen, ob mit gleichen parametern gerechnet wurde in v_spectra

  if (!is_init) {
    cerr << "coherency::quad_co -> coherency is not initialisied! Use init()" << endl;
    exit(0);
  }


  if (!x.is_spec) {
    cerr << "coherency::quad_co -> basis complex spectra of x not calculated!" << endl;
    cerr << "use calc_sp" << endl;
    exit(0);
  }
   
  if (!y.is_spec) {
    cerr << "coherency::quad_co -> basis complex spectra of y not calculated!" << endl;
    cerr << "use calc_sp" << endl;
    exit(0);
  }
   
   unsigned int i, j;
  squad.resize(size); scosp.resize(size);
  smean_quad.resize(pts);
  smean_cosp.resize(pts);

// das muss weg wenn stacks an!! speicher platz

   if( !stack_it || stack_it) {            // always
     for(i = 0; i < stacks; i++) {
       for(j = 0; j < pts; j++) {
         // replace later with sclices
          scosp[j + i * pts] = real(x.spc[j + i * pts]) * real(y.spc[j + i * pts])
                             + imag(x.spc[j + i * pts]) * imag(y.spc[j + i * pts]);
          squad[j + i * pts] = imag(x.spc[j + i * pts]) * real(y.spc[j + i * pts])
                             - imag(y.spc[j + i * pts]) * real(x.spc[j + i * pts]);
       }
     }
     is_quad_co = 1;


     if (imedian) {
        valarray<double> one_f(stacks);
         smed_quad.resize(pts);
         smed_cosp.resize(pts);

         for(j = 0; j < pts; j++) {
           one_f = scosp[slice(j, stacks, pts)];
           smed_cosp[j] = my_valarray::median(one_f);
         }
         for(j = 0; j < pts; j++) {
           one_f = squad[slice(j, stacks, pts)];
           smed_quad[j] = my_valarray::median(one_f);
         }
         is_quad_co_median = 1;

        if (1000 * range) {
          smed_quad_range.resize(pts);
          smed_cosp_range.resize(pts);
          for(j = 0; j < pts; j++) {
            one_f = scosp[slice(j, stacks, pts)];
            smed_cosp_range[j] = my_valarray::median_range(one_f, range);
//            smed_cosp_range[j] = median(one_f);
          }
          
          for(j = 0; j < pts; j++) {
            one_f = squad[slice(j, stacks, pts)];
            smed_quad_range[j] = my_valarray::median_range(one_f, range);
//            smed_quad_range[j] = median(one_f);
          }

          is_quad_co_median_range = 1;
        }

     }  



    

// this gives better results than stacking all doubles without slicing
      for(i = 0; i < stacks; i++) {
        smean_quad += squad[slice(i * pts, pts, 1)];
        smean_cosp += scosp[slice(i * pts, pts, 1)];
        
      }
      smean_quad /= stacks;
      smean_cosp /= stacks;
       is_quad_co_mean = 1;
  }



  return 1;

}





/***************************************************************************
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *    COHERENCIES                                                        *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/




int coherency::coh(int stack_it, int imedian, double range, const v_spectra &x, const v_spectra &y) {

  if (!is_init) {
    cerr << "coherency::coh -> coherency is not initialisied! Use init()" << endl;
    exit(0);
  }


  if (!x.is_mean_pwr) {
    cerr << "coherency::coh -> stacked power spectra of x not calculated!" << endl;
    cerr << "use mean and power" << endl;
    exit(0);
  }
  if (!y.is_mean_pwr) {
    cerr << "coherency::coh -> stacked power spectra of y not calculated!" << endl;
    cerr << "use mean and power" << endl;
    exit(0);
  }



//  unsigned int i, j;
// koennte automatisiert werden!!
  if (stacks > 1) {
    if (!is_quad_co_mean) {      
      cerr << "coherency::coh -> calculating quadrature and cospectrum ";
      quad_co(stack_it, imedian, range, x, y);
      cerr << ".... done" << endl;
    }
     
     // this is: fist stack all values and then use the coherency formula
    sqrt_coh.resize(pts);
    sqrt_coh = (smean_cosp * smean_cosp + smean_quad * smean_quad) / (x.smean_pwr * y.smean_pwr);
    sqrt_coh = sqrt(sqrt_coh);
    is_coh = 1;
 
  }

  if (imedian) {
    if (!x.is_median_pwr) {
      cerr << "coherency::coh -> stacked median power spectra of x not calculated!" << endl;
      cerr << "use median and power" << endl;
      exit(0);
    }
    if (!y.is_median_pwr) {
      cerr << "coherency::coh -> stacked median power spectra of y not calculated!" << endl;
      cerr << "use median and power" << endl;
      exit(0);
    }
    if (!is_quad_co_mean) {
      quad_co(stack_it, imedian, range, x, y);
      cerr << "coherency::coh -> quadrature and cospectrum not calculated!" << endl;
      cerr << "use quad_co" << endl;
//      exit(0);
    }
    
    sqrt_coh_med.resize(pts);
    sqrt_coh_med = (smed_cosp * smed_cosp + smed_quad * smed_quad) / (x.smedian_pwr * y.smedian_pwr);
    sqrt_coh_med = sqrt(sqrt_coh_med);
    is_coh_med = 1;

    if (1000 * range) {
      if (!x.is_median_pwr_range) {
        cerr << "coherency::coh -> stacked median power spectra of x not calculated!" << endl;
        cerr << "use median range and power" << endl;
        exit(0);
      }
      if (!y.is_median_pwr_range) {
        cerr << "coherency::coh -> stacked median power spectra of y not calculated!" << endl;
        cerr << "use median range and power" << endl;
        exit(0);
      }
      sqrt_coh_med_range.resize(pts);
      sqrt_coh_med_range = (smed_cosp_range * smed_cosp_range + smed_quad_range * smed_quad_range) / (x.smedian_pwr_range * y.smedian_pwr_range);
//            sqrt_coh_med_range = (smed_cosp_range * smed_cosp_range + smed_quad_range * smed_quad_range) / (x.smedian_pwr * y.smedian_pwr);
      sqrt_coh_med_range = sqrt(sqrt_coh_med_range);
      is_coh_med_range = 1;
    }

  }

return is_coh;
}

int coherency::write(const char* type, int time_col, const unsigned int& ul, const unsigned int& ll, string& name){

 unsigned int i;
 ofstream ofs;

  if (!is_init) {
    cerr << "coherency::write -> coherency is not initialisied! Use init()" << endl;
    exit(0);
  }


 
 if (strstr(type, "coh")) {
    if (is_coh != 1) {
      cerr  << "coherency::write -> coherency stack type one not calculated" << endl;
      exit(0);
    }
     ofs.open(name.c_str());
    if (!ofs) {
      cerr << "coherency::write -> can not open " << name << " as output" << endl;
      exit(0);
    }

    if (time_col) {
      for (i = 1 + ll; i < pts - ul; i++) ofs <<  freq[i] << "\x9" << sqrt_coh[i] << endl;
    }
    
    else {
      for (i = 1 + ll; i < pts - ul; i++) ofs << sqrt_coh[i] << endl;
    }
    
    ofs.close();

 }


 if (!strcmp(type, "median")) {
    if (is_coh_med != 1) {
      cerr  << "coherency::write -> coherency stack type one not calculated" << endl;
      exit(0);
    }
     ofs.open(name.c_str());
    if (!ofs) {
      cerr << "coherency::write -> can not open " << name << " as output" << endl;
      exit(0);
    }
    if (time_col) {
      for (i = 1 + ll; i < pts - ul; i++) ofs <<  freq[i] << "\x9" << sqrt_coh_med[i] << endl;
    }
    
    else {
      for (i = 1 + ll; i < pts - ul; i++) ofs << sqrt_coh_med[i] << endl;
    }
    
    ofs.close();
 }

 if (!strcmp(type, "medianrange")) {
    if (is_coh_med_range != 1) {
      cerr  << "coherency::write -> coherency stack type one not calculated" << endl;
      exit(0);
    }
     ofs.open(name.c_str());
    if (!ofs) {
      cerr << "coherency::write -> can not open " << name << " as output" << endl;
      exit(0);
    }
    
    if (time_col) {
      for (i = 1 + ll; i < pts - ul; i++) ofs <<  freq[i] << "\x9" << sqrt_coh_med_range[i] << endl;
    }
    
    else {
      for (i = 1 + ll; i < pts - ul; i++) ofs << sqrt_coh_med_range[i] << endl;
    }
    
    ofs.close();
 }



return 1;
}


