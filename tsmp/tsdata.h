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
#ifndef TSADATA_H
#define TSADATA_H

#include "tsheader.h"
#include "valarray"
#include <iostream>
#include <fstream>
#include <string>
#include "my_valarray.h"
#include "my_valarray_double.h"
#include <ctime>
#include "allinclude.h"
#include "string_utils.h"
#include "ats_consts.h"
#include "bt_utils.h"
#include "bfr_iter.h"
#include "mtx_iter.h"

#include "icalibration.h"


#ifdef myMySQL
  #include "mysql_simple_vector.h"
  #include </usr/include/mysql++/mysql++.h>
  using namespace mysqlpp;
#endif



/**
@author Bernhard Friedrichs
*/
class TSData : public TSHeader
{
public:

     TSData();
    ~TSData();



/**************************************************************************
*                                                                         *
*                                                                         *
*    defining the basic overloaded operators for the data valarray        *
*                                                                         *
*                                                                         *
*                                                                         *
**************************************************************************/

  /*!
      overlaoding operator *=
  */
  TSData& operator *= (const double& f) {
    ddata *= f;
    // force re-calculation if values are requested
    TSData::data_changed();
    return *this;
  }

  /*!
      overlaoding operator /=
  */
  TSData& operator /= (const double& f) {
    ddata /= f;
    // force re-calculation if values are requested
    TSData::data_changed();
    return *this;
  }

  /*!
      overlaoding operator +=
  */
  TSData& operator += (const double& f) {
    ddata += f;
    // force re-calculation if values are requested
    TSData::data_changed();
    return *this;
 }

  /*!
      overlaoding operator *=
  */
  TSData& operator -= (const double& f) {
    ddata -= f;
    // force re-calculation if values are requested
    TSData::data_changed();
    return *this;
  }


/**************************************************************************
*                                                                         *
*                                                                         *
*    defining the basic overloaded operators for the class                *
*                                                                         *
*                                                                         *
*                                                                         *
**************************************************************************/

/*!
overloading = operator for this class
*/


TSData& operator = (const TSData& M) {
  if (this == &M) return *this;
    
    this->TSHeader::operator=(M);
		cerr << "TSData::DATA COPY" << endl;
    // initialize and copy
    // here I could copy the position vectors as well but I don't
		// check whether double data is initialized or only integer
		if (M.ddata.size()) {
    	this->init_data(M.samples);
			this->ddata = M.ddata;
		}
		else {
		 	this->init_data(M.samples, true);
			this->idata = M.idata;
		}
		
    //cerr << M.samples << endl;
    
		
  return *this;
  }


/*!
overloading += operator for this class
*/
  TSData& operator += (const TSData& M) {
    if (this->samples != M.samples) {
        cerr << "\ndata vector length different, can not add values\n";
        exit(0);
    }
    else this->ddata += M.ddata;
    // force re-calculation if values are requested
    TSData::data_changed();
  return *this;
  }


/*!
overloading + operator for this class
*/
friend TSData operator + (const TSData& M, const TSData& N) {
    if (N.samples != M.samples) {
        cerr << "\ndata vector length different, can not add values\n";
        exit(0);
    }
    TSData P;
    P  = M;
    P.ddata += N.ddata;
    // force re-calculation if values are requested
    P.data_changed();

    return P;
  }


/*!
overloading -= operator for this class
*/
  TSData& operator -= (const TSData& M) {
    if (this->samples != M.samples) {
        cerr << "\ndata vector length different, can not subtract values\n";
        exit(0);
    }
    else this->ddata -= M.ddata;
    // force re-calculation if values are requested
    TSData::data_changed();

    return *this;
  }

/*!
overloading - operator for this class
*/

  friend TSData operator - (const TSData& M, const TSData& N) {
    if (N.samples != M.samples) {
        cerr << "\ndata vector length different, can not subtract values\n";
        exit(0);
    }
    TSData P;
    P = M;
    P.ddata -= N.ddata;
    // force re-calculation if values are requested
    P.data_changed();

    return P;
  }

/*!
overloading *= operator for this class
*/

  TSData& operator *= (const TSData& M) {
    if (this->samples != M.samples) {
        cerr << "\ndata vector length different, can not multiply values\n";
        exit(0);
    }
    else this->ddata *= M.ddata;
    // force re-calculation if values are requested
    TSData::data_changed();

  return *this;
  }

/*!
overloading * operator for this class
*/

friend TSData operator * (const TSData& M, const TSData& N) {
    if (N.samples != M.samples) {
        cerr << "\ndata vector length different, can not multiply values\n";
        exit(0);
    }

    TSData P;
    P = M;
    P.ddata *= N.ddata;
    // force re-calculation if values are requested
    P.data_changed();

  return P;
}

/*!
overloading /= operator for this class
*/
  TSData& operator /= (const TSData& M) {
    if (this->samples != M.samples) {
        cerr << "\ndata vector length different, can not divide values\n";
        exit(0);
    }
    else this->ddata /= M.ddata;
    // force re-calculation if values are requested
    data_changed();

  return *this;
}

/*!
overloading / operator for this class
*/
friend TSData operator / (const TSData& M, const TSData& N) {
    if (N.samples != M.samples) {
        cerr << "\ndata vector length different, can not divide values\n";
        exit(0);
    }

    TSData P;
    P = M;
    P.ddata /= N.ddata;
    // force re-calculation if values are requested
    P.data_changed();

    return P;
  }





/**************************************************************************
*                                                                         *
*                                                                         *
*    implementation                                                       *
*                                                                         *
*                                                                         *
*                                                                         *
**************************************************************************/









  int time_col;                                       /*!  write time stamp in first column of ascii file */
  int relative_time;                                  /*!  use realtive time starting with 0 sec instead of absolute time */
  int app_lsb;                                        /*!  data divided by lsb -> original ints as doubles written */




  int init_data(size_t s, bool ints = false);

  size_t read(ifstream& ifs,  size_t skip_samples_read = 0,  size_t n_samples = 0, const size_t n_slice = 0);
	
    size_t read_int(ifstream& ifs,  size_t skip_samples_read = 0,  size_t n_samples = 0, const size_t n_slice = 0);

  int write_ascii(ofstream& ofs, int i_time_col, int i_relative_time,
                  long time_offset,  string dateformat, int i_app_lsb, string& ident, double extra_shift_sec = 0.0 );
#ifdef myMySQL
  int write_mysql(Query& query, int i_time_col, int i_relative_time,
                  long time_offset,  string dateformat, int i_app_lsb, string& ident);
#endif

  int write(ofstream& ofs, size_t nbits = 32);       //writes a binary datum to file
	int write_int(ofstream& ofs, size_t nbits = 32);       //writes a binary datum to file
  int writedouble(ofstream& ofs, size_t& sampleno);           //writes a binary datum to file
  int writefloat(ofstream& ofs, size_t& sampleno);      //writes a binary datum to file


  //  do_atm : 1 create /overwrite; 2 merge; 3 merge from a file .atm.cat

  size_t destep(size_t step_len, double step_height, size_t relaxation, double skewness,
                unsigned int do_atm);
  size_t despike(size_t shoulder_len, double spike_height, size_t relaxation,
                  double spike_skewness, unsigned int do_atm);

                  // window length and overlapping;

  size_t stddevarray(size_t wl, size_t ovr, unsigned int do_atm);

  size_t maxmin_select(size_t length, double max_min );


  int filter(unsigned int reduction);
  size_t resample(double new_sample_freq);

  int cpheader(const TSData& M);
  int export_slice(double* f, unsigned int dstart, unsigned int dlength);
  int import_slice(double* f, unsigned int dstart, unsigned int dlength);

  size_t read_ascii(string filename, size_t col);

  int catats(const TSData& M);
  int linconv(unsigned int flen, double tarset_f);
  int check_for_mtx_trf(const string& fname);

  size_t hardwired_trf(string sensor_type, int chopper);   // 0 off 1 on use a measured trf function
  size_t read_trfkt_mtx(const string& fname);
  size_t read_trfkt(const string& fname, const double ampl_factor,
                    const int normalized_by_f, const int degrees,
                    const int skip_lines, const string keyword, const string type);
  size_t interpol_trfkt();
  double calc_lsb();                                      // essential for estimating the LSB bit

  double min();
  double max();
  double mean();
  double median();
  double usedbits();
  double scale();                    // scales Ex and Ey to mv/km
                                     // returns the time
  double time( const unsigned  int sampleno,  const int i_relative_time,
               const long time_offset, const string dateformat,
               string& dateresult, double extra_shift_sec = 0.0);
  int detrend();                     // detrends the complete time series
  size_t detrend_w();                // detrends every window wl
  int set_window_length(size_t iwl, size_t iovr); // get window length of FFT or other


  size_t get_window_length() const { return wl; }        // returns window length
  size_t get_stacks() const { return stacks; }               // returns amount of possible stack with windows of wl
                                     // stacks are calculated
  double stddev();




/**************************************************************************
*                                                                         *
*                                                                         *
*    some inline functions for simple data exchange                       *
*                                                                         *
*                                                                         *
*                                                                         *
**************************************************************************/


  /*!
    returns a single double
  */
  double data(const size_t& i){return ddata[i];}

  /*!
   returns the comlete time series data<br>
   can be used as a valarray t.data() *= 3.;
  */
  valarray<double>& data(){return ddata;}

  /*!
   returns the comlete time series data as INTEGER<br>
   can be used as a valarray t.data() *= 3.;
  */
  valarray<int>& int_data(){return idata;}

  /*!
  returns the selection vector ...0 is selected for processing<br>
  1 is de-selected, excluded from processing
  */
  vector<bool>& get_select(){return atm_select;}


  /*!
  returns the number of excluded points from processing
  */
  size_t tags(){return check_true(atm_select);}


  void set_data(const size_t& i, const double &d);  // gets a double
  void set_data(const valarray<double>&  d);        // gets a data vector
	void set_data_int(const valarray<int>&  di);        // gets a data vector
  void kill_data() {ddata.resize(0);}


  size_t set_select(vector<bool>& select);          // gets a selection vector
  size_t set_samples(size_t samples);             // gets the amount of samples to use;


  void data_changed();

  int set_stddev_param(double& dmin, double& dmax, size_t& iovr);
  size_t fir_filter(size_t fir_order, double cutlow, double cuthigh, size_t firfil_reduct);









  private:


  size_t stacks;  /*! possible amount of stacks for fft */
  size_t wl;      /*! wl is window length of fft and detrend of fft or detrend_w used later */
  size_t ovr;     /*! overlapping of each window wl in points */



  size_t skip_samples_read;                  /*! samples to be skipped at the beginning -> datetime will change*/



  valarray<double> ddata;                    /*! "real" data in [mV] */
  valarray<int> idata;				         /*! 32 bit integer - used for cut/truncate only*/
  double dmax;                               /*! max of time series */
  double dmin;                               /*! min of time series */
  double dmean;                              /*! mean of time series */
  double dmedian;                            /*! median of time series */
  double dstddev;                            /*! standard deviation of time series */
  int idata32;                               /*! used for reading and byte swapping */
  short int idata16;                         /*! for reading and byte swapping 16 bit data */

  double dtemp_ddata;                        /*! used for byte swapping and writing a single double */
  float  ftemp_ddata;                        /*! used for byte swapping and writing a single float */

  double my_stddev_stddev;                   /*! standard deviation of all standard deviation windows */
  double my_stddev_median;                   /*! median of all standard deviation windows */
  double my_stddev_mean;                     /*! mean of all standard deviation windows */
  double use_min_std_f;                      /*! get form command line the range of min*median of stddevs for usage */
  double use_max_std_f;                       /*! get form command line the range of max*median of stddevs for usage */




  vector<bool> new_atm_select;               /*! vector for writing atm files 0 = false = selcected for processing */

  vector<bool> atm_select;                   /*! vector for writing atm files 0 = false = selcected for processing */


  valarray<size_t> posv;                     /*! vector with positions of tagged samples */

  vector<size_t> spikes;                     /*! index vectors for stpikes in time series */
  vector<size_t> steps;                      /*! index vectors for steps in time series */
  vector<double> spike_vals;                 /*! corresponding values for removed spikes */
  vector<double> step_vals;                  /*! corresponding values for removed steps */

  valarray<double> stddevs;                  /*! vector of standard deviations */


};

#endif
