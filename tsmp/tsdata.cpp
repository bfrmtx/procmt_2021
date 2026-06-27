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
#include "tsdata.h"



/*! constructor
  Initialize data and values with zeros

*/
TSData::TSData() {
    this->samples = this->skip_samples_read =
    this->app_lsb = 0;
    this->wl = 0;  this->stacks = 0; this->time_col = 0; this->relative_time = 0;

    this->ovr = 0;
    this->dmean = this->dmedian = this->dmin = this->dmax = this->dstddev = DBL_MAX;
    this->use_min_std_f = this->use_max_std_f = DBL_MAX;



}

/*!
destructor
*/
TSData::~TSData() {


  }
/*!
  init data vector; file can be read in small parts                      *
*/
  int TSData::init_data(size_t s, bool ints) {
    if (!s) {
      cerr << "\nTSData::init_data -> initialise with s > 0 please\n";
      exit(0);
    }

    // calling resize deletes existing data and creates a new valarray
    this->samples = s;
    if (!ints) ddata.resize(this->samples);
		else idata.resize(this->samples);
    this->samples_orig = this->samples;
    atm_select.resize(this->samples, 0);        // should be ZERO all over
    new_atm_select.resize(this->samples, 0);    // all selected for proc
    return s;
}





/*!
force re-calculation if one of these is required
max, min, mean, median, stddev
*/
void TSData::data_changed() {


    this->dmean = this->dmedian = this->dmin = this->dmax = this->dstddev = DBL_MAX;
    if (this->ddata.size() && wl) this->stacks = this->ddata.size() / this->wl;
    calc_lsb();
}





 /*!
  assuming the header with samples is read; while initializing the data we make sure
  that samples is not greater than samples
 */
  size_t TSData::set_samples(size_t samples) {

    if (samples > this->samples_orig) {
      cerr << "TSData::set_samples -> you can not read more samples than existing in the file" << endl;
      cerr << "total, try: " << samples << ", " << this->samples_orig  << endl;
      cerr << "EXIT" << endl;
      return EXIT_SUCCESS;
    }
    else {
      this->samples = samples;
      return this->samples;
    }
  }




/*!
  get a vector containing data for excluding parts from processing
*/
  size_t TSData::set_select(vector<bool>& select) {
    size_t i;
    for (i = 0; i < this->samples; i++) atm_select[i] = select[skip_samples_read + i];
    cerr << "TSData::set_select -> total "  << select.size() << " used " <<  atm_select.size() << " tags: " << TSData::tags() << endl;
    return atm_select.size();



/*
    if (select.size() != atm_select.size()) {
      cerr << "TSData::getselect -> boolean vector sizes are different!; nothing done" << endl;
      return 0;
    }
    else {
      atm_select = select;
      return atm_select.size();
    }
*/
  }


 /*!
   amx min analysis
 */
 size_t TSData::maxmin_select(size_t length, double max_min ) {

   cerr << "TSData::maxmin_select got length, max_min" << length << "  " << max_min << endl;
  my_valarray::maxmin_select(ddata, length, max_min, atm_select);
 return 0;
 }



/*!
 get a double at position i
*/
  void TSData::set_data(const size_t& i, const double &d) {
  this->ddata[i] = d;
  TSData::data_changed();
  }


/*!
 import an external valarray
*/
  void TSData::set_data(const valarray<double>&  d) {
  if (ddata.size() != d.size()){
      cerr << "TSData::set_data -> getting data vector; old size " << ddata.size() << " ";
     ddata.resize(d.size());
     atm_select.resize(d.size());
		 ddata = d;
     cerr << "differs from new; new size now: " << ddata.size() << "max/min: " << ddata.max() << " " << ddata.min() << endl;
  }
  ddata = d;
  this->samples = d.size();
  samples = d.size();;
  TSData::data_changed();
  }

/*!
 import an external valarray
*/
  void TSData::set_data_int(const valarray<int>&  di) {
  if (idata.size() != di.size()){
      cerr << "TSData::set_data -> getting data vector; old size " << idata.size() << " ";
     idata.resize(di.size());
     atm_select.resize(di.size());
		 idata = di;
     cerr << "differs from new; new size now: " << idata.size() << "max/min: " << idata.max() << " " << idata.min() << endl;
  }
  idata = di;
  this->samples = di.size();
  samples = di.size();;
  // no! TSData::data_changed();
  }



  int TSData::set_stddev_param(double& dmin, double& dmax, size_t& iovr){

    if (dmin == DBL_MAX || dmax == DBL_MAX || !iovr) {

      cerr << "TSData::set_stddev_param -> error in getting stddev parameter! " << endl;
      cerr << "min_std_factor, max_std_factor, overlay: " << dmin << "  " << dmax << "  " << iovr << endl;
      return EXIT_FAILURE;

    }
    else {
    use_min_std_f = dmin;
    use_max_std_f = dmax;
    ovr = iovr;
    return 1;
    }
  }


/*!
 needed for = operator; copy the time series header
*/
int TSData::cpheader(const TSData& M) {
    //tsheader::cpheader(M);
    TSHeader::get_lsb();
return 1;
}





/*!
 having a time series samples we calculate for every wl a standard deviation
 the result is stored inside the class in stddevs and the beginning of each wl in posv
 this enables later to use this results for ex - including segments from processing

*/

size_t TSData::stddevarray(size_t wl, size_t ovr, unsigned int do_atm) {

  int nwindows;
      nwindows =  my_valarray::stddev_array(ddata, stddevs, posv, wl, ovr);

      my_stddev_median = my_valarray::median(stddevs);
      my_stddev_stddev = my_valarray::stddev(stddevs);
      my_stddev_mean = stddevs.sum() / stddevs.size();

      cerr << "TSData::stddev_array -> stddev min: " << stddevs.min() << "\x9stddev max: " << stddevs.max() << endl;
      cerr << "TSData::stddev_array -> stddev mean: " << my_stddev_mean;
      cerr << "\x9stddev median: " << my_stddev_median << "\x9stddev: " << my_stddev_stddev << endl;


      // we will do a min max analysis
      if (use_min_std_f != DBL_MAX && use_max_std_f != DBL_MAX) {
        size_t i, j, sel = 0;

        new_atm_select.assign(new_atm_select.size(), 1);

        for (i = 0; i < posv.size(); i++) {
          if ( (stddevs[i] >  use_min_std_f * my_stddev_stddev + my_stddev_median) &&
              (stddevs[i] <  use_max_std_f * my_stddev_stddev + my_stddev_median)) {
                sel++;
                for (j = posv[i]; j < posv[i] + wl - ovr; j++) new_atm_select[j] = 0;

          }
        }
        if (do_atm == 2 || do_atm == 4) {
            cerr << "TSData::stddev_array -> merging selections, existing: " << check_true(atm_select) << "  ";
            merge_bool(atm_select, new_atm_select);
        }

        else atm_select = new_atm_select;

        cerr << check_true(atm_select) << "  " << check_true(new_atm_select) << endl;
        cerr << "TSData::stddev_array -> " << sel << " window tags selected FOR processing and "
            << check_true(atm_select)/wl << " NOT"  << endl;

      }

  return nwindows;
}



/*!
 Algorithm to suppres spikes in time series
  (e.g. electric fences)

*/
size_t TSData::despike(size_t shoulder_len, double spike_height,
                  size_t relaxation, double spike_skewness, unsigned int do_atm){
// do_atm 1 = new/create; 2 = merge   4 = use
    size_t ii = 0, jj = 0, kk = 0, nn;
//     double offset = 0.0;
      valarray<double> sl(shoulder_len);
      double smean;
      // form my_valarray.h included the "find" algorithm
      ii = my_valarray::fnd_spk(ddata, spikes, shoulder_len, spike_height, relaxation, spike_skewness);
      cerr << "TSData::despike -> marked points " << ii << flush;
      if (spikes.size()) {
        // save orig. value for later analysis - this is not including the data manipulated inside the
        // relaxation area
        for (ii = 0; ii < spikes.size(); ii++)   spike_vals.push_back(ddata[spikes[ii]]);
        if (do_atm == 1) {    // clear old selection because we receive the do_atm == 1 == create/new
          for (ii = 0; ii < atm_select.size(); ii++) atm_select[ii] = 0;
        }

        // copy into the atm selection vector and de-select within the relaxation range
      for (ii = 0; ii < spikes.size(); ii++) {
          for (nn = spikes[ii] - relaxation; nn < spikes[ii] + relaxation; nn++)
            if(nn < ddata.size()) atm_select[nn] = 1;
        }

      // the very first values can not be corrected because of missing "left shoulder"
        for (ii = relaxation + 1; ii < ddata.size()  ; ii++) {
            if ( ii == spikes[jj]) {

                sl = ddata[slice(ii - shoulder_len - relaxation, shoulder_len, 1)];
                smean = sl.sum()/shoulder_len;
                // here we define WHAT to do with the marked points
                for (kk = ii - relaxation/2; kk < ii + relaxation/2; kk++) {
    //             atd[nm].ddata[kk]  *= (smean/atd[nm].ddata[kk]);    // take the left sided value
                  if (ddata[kk] > sl.max())  ddata[kk] =  sl.max() - (sl.max()- smean)/2.;
                  else if (ddata[kk] < sl.min()) ddata[kk] = sl.min() + (smean - sl.min())/2.;;
                }
                ii = kk;                            // update global index after we moved behind the relaxation
                jj++;                            //  check for next spike index
                if ( kk >= spikes[jj]) {                // if next index is inside the relaxation
                  do {                                     // we don't need him anymore
                    jj++;
                  } while ( kk >= spikes[jj] && spikes[jj]);
                }
            }
        }
      }                                         // end spikes.size()
  cerr << endl;
  // force re-calculation if requested
    TSData::data_changed();
  return spikes.size();
}


/*!
 Algorithm to eliminate steps in time series

*/
size_t TSData::destep(size_t step_len, double step_height, size_t relaxation, double skewness,
                        unsigned int do_atm){
      size_t ii = 0, jj = 0, k;
      double offset = 0.0;
// do_atm = 1 = new / create; do_atm = 2 merge

      ii = my_valarray::fnd_stp(ddata, steps, step_len, step_height, relaxation, skewness);

      cerr << "TSData::destep -> marked points " << steps.size() << endl;
      if (steps.size()) {

        // clear slection if do_atm was set to 1 == create/new
        if (do_atm == 1) for (ii = 0; ii < atm_select.size(); ii++) atm_select[ii] = 0;
        // tag the selection vector
        for (ii = 0; ii < steps.size(); ii++) {
          k = steps[ii];
          atm_select[k-1] = 1;
          atm_select[k] = 1;
          atm_select[k+1] = 1;
        }

        // save orig. value for later analysis
        for (ii = 0; ii < steps.size(); ii++)   step_vals.push_back(ddata[steps[ii]]);

        for (ii = 1; ii < ddata.size() - 1; ii++) {
            ddata[ii]  += offset;    // take the left sided value
            if ( ii == steps[jj]) {
              cerr << "TSData::destep -> step at " << ii << endl;
              offset =  ddata[ii] - ddata[ii + 1]; // new offset
/*             if ( (ii < ddata.size() - 3) && ((ii+1) == (steps[jj+1])) ) {
                cerr << "TSData::destep -> two sample step at " << ii << "offset: " << offset << endl;
                ii++; jj++;
                ddata[ii]  += offset;
                offset = ddata[ii] - ddata[ii + 2];   // prevent "shoot over"
              }
*/
              jj++;
            }
        }
      }
  // force re-calculation if requested
    TSData::data_changed();

return steps.size();
}

/*!
  applies filter with  32x, 4x decimation                               *
*/
int TSData::filter(unsigned int reduction) {

  if (reduction == 4 || reduction == 32 || reduction == 2|| reduction == 25 || reduction == 8) {
    valarray<double> tddata, coeff;
    if (reduction == 4) {
      coeff.resize(71);
      copy(&coeff4[0], &coeff4[70], &coeff[0]);
    }
    if (reduction == 32) {
      coeff.resize(471);
      copy(&coeff32[0], &coeff32[470], &coeff[0]);
    }
    if (reduction == 25) {
      coeff.resize(501);
      copy(&coeff25[0], &coeff25[500], &coeff[0]);
    }
    if (reduction == 2) {
      coeff.resize(35);
      copy(&coeff2[0], &coeff2[34], &coeff[0]);
    }

    if (reduction == 8) {
      coeff.resize(159);
      copy(&coeff8[0], &coeff8[158], &coeff[0]);
    }
    

    samples_orig = samples;
    cerr << "TSData::filter -> coeff -> " << coeff.size() << endl;
    cerr << "TSData::filter -> " << reduction << "x decimation ";

    this->samples = my_valarray_double::filx(this->ddata, tddata, reduction, coeff, 1.0);


    this->datetime += long(coeff.size()/(2. * this->samplefreq));
    this->samplefreq /= reduction;
    this->skip_samples_read = 0;
    cerr << "TSData::filter -> new sample freq: " << this->samplefreq << "Hz (" << 1/this->samplefreq << "s),  ";
    cerr << this->samples << " samples" << endl;
    cerr << "TSData::filter -> new start time: " << GMTtime(datetime) << endl;
    this->stoptime = this->datetime + long (this->samples/this->samplefreq);
    this->ddata.resize(samples);

    this->ddata = tddata;
    this->atm_select.resize(this->ddata.size());


  }
  else {
    cerr << "reduction 2, 4, 8, 32, 25 posssible; argument was : " << reduction << endl;
    exit(0);
  }
  // force re-calculation
      TSData::data_changed();

return 1;
}

/*!
    \fn TSData::fir_filter(fir_order, cutlow, cuthigh)
 */
size_t TSData::fir_filter(size_t fir_order, double cutlow, double cuthigh, size_t firfil_reduct)
{
    valarray<double> tddata, coeff;


    coeff.resize(fir_order);
    samples_orig = samples;
    cerr << "TSData::fir_filter -> coeff -> " << coeff.size() << endl;
    if (cuthigh == DBL_MAX) cuthigh = this->samplefreq;

    if (!firfil_reduct) tddata.resize(this->ddata.size()- fir_order + 1);
    else  tddata.resize( (this->ddata.size()- fir_order + 1) / firfil_reduct);
    
    //fir_dsgn(T* first, T* last, const T samplefreq, T CutLow, T CutHigh)
    //mtx::detrend<double>(&coeff[0], &coeff[coeff.size()]);
    mtx::fir_dsgn<double>(&coeff[0], &coeff[coeff.size()], this->samplefreq, cutlow, cuthigh);
    if (!firfil_reduct) {
      if (!mtx::fil_fir<double>(&this->ddata[0], &this->ddata[this->ddata.size()], &tddata[0], &tddata[tddata.size()], &coeff[0], &coeff[coeff.size()])) {
       exit (EXIT_FAILURE);
     }
      
    
    }
    
    else {
      if (!mtx::fil_fir_reduce<double>(&this->ddata[0], &this->ddata[this->ddata.size()], &tddata[0], &tddata[tddata.size()], &coeff[0], &coeff[coeff.size()], firfil_reduct )) {
        exit (EXIT_FAILURE);
      }
 
   }
      
    
//     bfrtsmp::fir_dsgn(&coeff[0], &coeff[coeff.size()], this->samplefreq, cutlow, cuthigh);
//     if (!bfrtsmp::fil_fir(&this->ddata[0], &this->ddata[this->ddata.size()], &tddata[0], &tddata[tddata.size()], &coeff[0], &coeff[coeff.size()])) {
//       exit (EXIT_FAILURE);
//     }


    this->datetime += long((fir_order-1)/(2. * this->samplefreq));

    this->skip_samples_read = 0;
    cerr << "TSData::fir_filter -> new start time: " << GMTtime(datetime) << endl;
    this->stoptime = this->datetime + long (this->samples/this->samplefreq);

    this->ddata.resize(tddata.size());
    this->ddata = tddata;
    this->samples = this->ddata.size();
    this->atm_select.resize(this->ddata.size());

    return 1;
}


/*!
   uses akima spline for producing articial sampling rates
*/


size_t TSData::resample(double new_sample_freq) {

  cerr << "TSData::resample -> generating new sampling frequency from " << samplefreq  << "  ->  " << new_sample_freq << endl;
  valarray<double> tddata;
  samples = my_valarray_double::resample(ddata, tddata, samplefreq, new_sample_freq);
  init_data(samples);

  samplefreq = float(new_sample_freq);
  cerr << "TSData::resample -> done, " << samplefreq << ", "  << samples << endl;

  ddata = tddata;

  // force re-calculation if requested
  TSData::data_changed();


return 1;

}







/*!
  reads parts of the file or complete file  \n
  also modify datetime, calculate stop time \n
  skip and than the number of points (samples)
*/
size_t TSData::read(ifstream& ifs,  size_t skip_samples_read,  size_t samples, const size_t n_slice) {

    // test input file
    if (!ifs) {
      cerr << "\nTSData::read -> can not read ifstream";
      exit(0);
    }

    #ifdef use_shift_to_swap
      cerr << "TSData::read -> using byte swap (s);  " << endl;
    #endif
    #ifdef use_template_to_swap
      cerr << "TSData::read -> using byte swap (t);  " << endl;
    #endif


    size_t i;
    size_t skip_complete_channel_data = 0;
    size_t skip_slicesamples = 0;

    this->skip_samples_read = skip_samples_read;


    // handle incorrect values first
    if (this->skip_samples_read > this->samples_orig) {
      cerr << "\natsdata::read -> you skip more samples than existing in the file skip:" << this->skip_samples_read << "  exist:" << this->samples_orig << "\n";
      exit(0);
    }
    else if  ( samples + this->skip_samples_read == this->samples_orig) {
      this->samples = samples;
    }

    else if (!samples ||
        (this->skip_samples_read < samples && samples == this->samples_orig) ) {
      this->samples = samples - this->skip_samples_read;

    }

    else if (samples > this->samples_orig) {
      cerr  << "\natsdata::read samples is greater than total samples in file\n";
      exit(0);
    }




    if (this->skip_samples_read + this->samples > this->samples_orig) {
      cerr << "\natsdata::read -> you try to skip and read more samples than existing in the file\n";
      cerr << "skip: " << skip_samples_read << " samples: " << this->samples << " orig samples: " <<  this->samples_orig << endl;
      cerr << "if you want to filter data where you know that the start time is the same for all file try the option -sync2 -nogrid to avoid adjustment of start time " << endl;
      exit(0);
    }


    // include function header data into object

    init_data(this->samples);


    cerr << "TSData::read -> skipping " << skip_samples_read << " samples   ";


    if (headertype == "ats" || headertype == "bgrf1" || headertype == "emimt24") {

      if (ifs.tellg() != streampos(headerlength)) {
        cerr << "TSData::read -> wrong file position is: " <<  ifs.tellg() << " should be " << headerlength << "  ... correcting" << endl;
        ifs.seekg(headerlength);

      }

      if ( n_slice && this->tslices.size()) {
          // try to fetch the slice
          for (size_t jjj = 0; jjj < n_slice; ++jjj) {
              skip_slicesamples += this->tslices.at(jjj).iSamples;
          }

          cerr << "TSData::read -> skipping " << n_slice << " slices, new data pos: " << skip_slicesamples << endl;

      }

        // skip data is neccessary ************************************************************************
        if (skip_samples_read + skip_slicesamples || channel_no) {
          // init dummy vector
            if (channel_no) cerr << "test: reading channel no: " << channel_no << endl;
            //skip_complete_channel_data = channel_no * goe.channel_blocksize() / n_channels;
            if (storagebits == 32) {
              for ( i = 0; i < skip_samples_read + skip_slicesamples + skip_complete_channel_data; i++) {
                ifs.read(static_cast<char *> (static_cast<void *>(&idata32)), sizeof( int));
                if (ifs.eof()) {
                  cerr << endl << endl << "file ended unexpectetly at " << i << endl << endl;
                  exit(0);
                }
              }
            }
            if (storagebits == 16) {
              for ( i = 0; i < skip_samples_read + skip_slicesamples + skip_complete_channel_data; i++) {
                ifs.read(static_cast<char *> (static_cast<void *>(&idata16)), sizeof( int));
                if (ifs.eof()) {
                  cerr << endl << endl << "file ended unexpectetly at " << i << endl << endl;
                  exit(0);
                }
              }
            }

        }
				// end skip data

        //modify the datetime
        datetime = datetime + long (skip_samples_read / samplefreq);
        stoptime = datetime + long (this->samples / samplefreq);
        cerr << "TSData::read -> reading " << this->samples << " samples " << endl;
        cerr << "TSData::read -> starting at date: " << GMTtime(datetime) << "TSData::read -> stopping at date: " << GMTtime(stoptime);
        cerr << "TSData::read -> ADCOffset: " << adc_dcoffset << " LSBVal: " << lsbval << "  extra factor: " << adc_factor << endl;

        if (storagebits == 32) {
          for (i = 0; i < this->samples; i++ ) {
             
              ifs.read(static_cast<char *> (static_cast<void *>(&idata32)), sizeof( int));
              #ifdef use_shift_to_swap
                idata32 = bswap_32(idata32);
              #endif
              #ifdef use_template_to_swap
                tbswap(idata32);
              #endif

              ddata[i] = (double) (idata32 - adc_dcoffset) * lsbval * adc_factor;
  //            cerr << idata32 << "--" << ddata[i] << endl;
              if (ifs.eof()) {
                cerr << endl << endl << "file ended unexpectetly at " << i << endl << endl;
                exit(0);
              }

          }
        }
        if (storagebits == 16) {
          for (i = 0; i < this->samples; i++ ) {
              ifs.read(static_cast<char *> (static_cast<void *>(&idata16)), sizeof( short int));
              #ifdef use_shift_to_swap
                idata16 = bswap_16(idata16);
              #endif
              #ifdef use_template_to_swap
                tbswap(idata16);
              #endif
              ddata[i] = (double) (idata16 - adc_dcoffset) * lsbval * adc_factor;
              if (ifs.eof()) {
                cerr << endl << endl << "file ended unexpectetly at " << i << endl << endl;
                exit(0);
              }
          }
        }

    }


/*
    if (headertype == "goe_rap") {
      if (ifs.tellg() != streampos(goe.afdb)) {
        cerr << "TSData::read -> wrong file position is: " <<  ifs.tellg() << " should be afb: " << goe.afdb << endl;
      }
        short int *dumps;
        if (channel_no || skip_samples_read)  {          // if we do not read channel_no 0 we have to skip bytes
          i =  channel_no * goe.channel_blocksize() / n_channels + skip_samples_read;
          dumps = new short int [i];
          for (j = 0; j < i; j++)
            ifs.read(static_cast<char *> (static_cast<void *>(&dumps[j])), sizeof( short int));
          delete[] dumps;
        }



        datetime = datetime + long (skip_samples_read / samplefreq);
        cerr << "TSData::read -> reading " << this->samples << " samples   ";
        for (i = 0; i < this->samples; i++) {
          ifs.read(static_cast<char *> (static_cast<void *>(&sdata)), sizeof(short int));
          #ifdef use_shift_to_swap
            sdata = bswap_16(sdata);
          #endif
          #ifdef use_template_to_swap
            tbswap(sdata);
          #endif

//          cerr << sdata << "  ->" << ifs.tellg() << " ";
          sdata += SHRT_MIN;
          ddata[i] = double (sdata);
        }
        ddata *= lsbval;

    }
*/

    cerr << "TSData::read -> finished\n";
    return 1;
}
// end read 



/*!
  write the complete file
*/
  int TSData::write(ofstream& ofs, size_t nbits) {

  // it should not be neccessary to calculate the LSBValue here!
  // should be done earlier
    if (!ofs) {
      cerr << "\natsdata::write -> can not write to ofstream";
      exit(0);
    }
    #ifdef use_shift_to_swap
      cerr << "TSData::write -> using byte swap (s);  " << endl;
    #endif
    #ifdef use_template_to_swap
      cerr << "TSData::write -> using byte swap (t);  " << endl;
    #endif


    size_t i;
    if (nbits == 32) {
      for (i = 0; i < this->samples; i++) {
        idata32 = int(ddata[i]/lsbval);
        //cerr << idata32 << " " << ddata[i] << " " << lsbval << endl;
        #ifdef use_shift_to_swap
          idata32 = bswap_32(idata32);
        #endif
        #ifdef use_template_to_swap
          tbswap(idata32);
        #endif
        ofs.write(static_cast<char *> (static_cast<void *>(&idata32)), sizeof( int));
      }
    }

    if (nbits == 16) {
      for (i = 0; i < this->samples; i++) {
        idata16 = (short int) (ddata[i]/lsbval);
        #ifdef use_shift_to_swap
          idata16 = bswap_16(idata16);
        #endif
        #ifdef use_template_to_swap
          tbswap(idata16);
        #endif
        ofs.write(static_cast<char *> (static_cast<void *>(&idata16)), sizeof( int));
      }
    }




    return 1;


  }


/*!
  write a single binary double datum  to stream
*/
int  TSData::writedouble(ofstream& ofs, size_t& sampleno) {

      dtemp_ddata = ddata[sampleno];
      #ifdef use_shift_to_swap
        bswap_32(dtemp_ddata);
      #endif
      #ifdef use_template_to_swap
        tbswap(dtemp_ddata);
      #endif
      ofs.write(static_cast<char *> (static_cast<void *>(&dtemp_ddata)), sizeof( double));
 return 1;

  }


/*!
  write a single binary float datum  to stream
*/

int  TSData::writefloat(ofstream& ofs, size_t& sampleno) {

      ftemp_ddata = float(ddata[sampleno]);
      #ifdef use_shift_to_swap
        bswap_32(ftemp_ddata);
      #endif
      #ifdef use_template_to_swap
        tbswap(ftemp_ddata);
      #endif
      ofs.write(static_cast<char *> (static_cast<void *>(&ftemp_ddata)), sizeof( float));
 return 1;

  }




 double TSData::time( const unsigned  int sampleno,
              const int i_relative_time, const long time_offset, const string dateformat,
              string& dateresult, double extra_shift_sec) {


     if (i_relative_time) {
       return   double(skip_samples_read + sampleno )/samplefreq + extra_shift_sec;
     }
     else {

       if (dateformat == "ISO") dateresult = GMTISOtime(datetime + time_offset, sampleno, samplefreq, extra_shift_sec);
       return   double(datetime + time_offset) + extra_shift_sec + double(sampleno) /samplefreq;


     }


 }

/*!
  write parts of the file
  if apply lsb is activated the double represents then the integer of
  the origina�digital time series
  if time col is 0, only data is written                                                                                                          *

*/
  int TSData::write_ascii(ofstream& ofs, int i_time_col, int i_relative_time,
                          long time_offset, string dateformat, int i_app_lsb, string& ident, double extra_shift_sec) {

  // test for output
  if (!ofs) {
    cerr << "\natsdata::write_ascii -> can not write to ofstream";
    exit(0);
  }
  ofs.setf(ios::scientific, ios::floatfield);
  ofs.precision(12);
  // include function header data into object


  size_t i, j = 0;
  double temp = 1.;
  if (i_app_lsb) temp = lsbval;

  if (i_time_col) {
    if (i_relative_time) {

      if (ident == "data") {
        for ( i = 0; i < this->samples; i++) {
          ofs << double(skip_samples_read + i)/samplefreq << "\x9"
              << ddata[i] / temp << endl;
        }
        cerr << "TSData::write_ascii -> time series data written " << endl;
      }



      if (ident == "steps") {
        for ( i = 0; i < steps.size(); i++) {
          ofs << double(skip_samples_read + steps[i])/samplefreq << "\x9"
              << step_vals[i] / temp << endl;
        }
        cerr << "TSData::write_ascii -> index of steps written" << endl;
      }


      if (ident == "spikes") {
        for ( i = 0; i < spikes.size(); i++) {
          ofs << double(skip_samples_read + spikes[i])/samplefreq << "\x9"
              << spike_vals[i] / temp << endl;
        }
        cerr << "TSData::write_ascii -> index of spikes written " << endl;
      }

      if (ident == "stddev") {
        for ( i = 0; i < posv.size(); i++) {
          ofs << double(skip_samples_read + posv[i])/samplefreq << "\x9"
              << stddevs[i] / temp << endl;
        }
        cerr << "TSData::write_ascii -> index of standard devations written " << endl;
      }

      if (ident == "stddev_used") {
        if (use_min_std_f == DBL_MAX || use_max_std_f == DBL_MAX) {
          cerr << "TSData::write_ascii -> can not write " << ident << endl;
          cerr << "min max factors were: " << use_min_std_f << "  " << use_max_std_f << endl;
          return 0;
        }
        for ( i = 0; i < posv.size(); i++) {
          if ( (stddevs[i] >  use_min_std_f * my_stddev_stddev + my_stddev_median) &&
              (stddevs[i] <  use_max_std_f * my_stddev_stddev + my_stddev_median)) {

              ofs << double(skip_samples_read + posv[i])/samplefreq << "\x9"
                  << stddevs[i] / temp << endl;
          }
        }
        cerr << "TSData::write_ascii -> index of standard devations for USE written " << endl;
      }

    }
      // absoulte time and date
    else {

      cerr << "TSData::write_ascii -> offset is date + offset: " << datetime << " + " << time_offset << endl;
      long newtime =  datetime + time_offset;
      cerr << "TSData::write_ascii -> approx! offsetts are: original, shift: " << endl;
      cerr << GMTtime(datetime);
      cerr << GMTtime(newtime) << "( not evaluated iso_s: " << extra_shift_sec << " )" <<endl;

      if (dateformat == "ISO") {
        cerr << "TSData::write_ascii -> using ISO 8601 date format " << endl;

        if (ident == "data") {
          for ( i = 0; i < this->samples; i++) {
            ofs << GMTISOtime(datetime, j++, samplefreq, extra_shift_sec) << "\x9"
                << ddata[i] / temp << endl;
          }
          cerr << "TSData::write_ascii -> time series data written " << endl;
        }

        if (ident == "steps") {
          for ( i = 0; i < steps.size(); i++) {
            ofs << GMTISOtime(datetime, steps[i], samplefreq, extra_shift_sec) << "\x9"
              << step_vals[i] / temp << endl;
          }
          cerr << "TSData::write_ascii -> index of steps written" << endl;
        }


        if (ident == "spikes") {
          for ( i = 0; i < spikes.size(); i++) {
            ofs << GMTISOtime(datetime, spikes[i], samplefreq, extra_shift_sec) << "\x9"
                << spike_vals[i] / temp << endl;
          }
          cerr << "TSData::write_ascii -> index of spikes written " << endl;
        }



        if (ident == "stddev") {
          for ( i = 0; i < posv.size(); i++) {
            ofs << GMTISOtime(datetime, posv[i], samplefreq, extra_shift_sec) << "\x9"
                << stddevs[i] / temp << endl;
          }
          cerr << "TSData::write_ascii -> index of standard devations written " << endl;
        }

        if (ident == "stddev_used") {
          if (use_min_std_f == DBL_MAX || use_max_std_f == DBL_MAX) {
            cerr << "TSData::write_ascii -> can not write " << ident << endl;
            cerr << "min max factors were: " << use_min_std_f << "  " << use_max_std_f << endl;
            return 0;
          }
          for ( i = 0; i < posv.size(); i++) {
            if ( (stddevs[i] >  use_min_std_f * my_stddev_stddev + my_stddev_median) &&
                (stddevs[i] <  use_max_std_f * my_stddev_stddev + my_stddev_median)) {

              ofs << GMTISOtime(datetime, posv[i], samplefreq, extra_shift_sec) << "\x9"
                    << stddevs[i] / temp << endl;
            }
          }
          cerr << "TSData::write_ascii -> index of standard devations for USE written " << endl;
        }


      }
      else {
        cerr << "TSData::write_ascii -> UNIX time_t + sample/sample_freq date format " << endl;


        if (ident == "data") {
          for ( i = 0; i < this->samples; i++) {


//              cout.setf(ios::scientific, ios::floatfield);
//              cout.precision(12);

//              cout << j << " " << double(j) / samplefreq << " " << double (datetime + time_offset) + double(j)/samplefreq << endl;
// NGRI

            ofs << double (datetime + time_offset) + double(j++)/samplefreq
                << "\x9" << ddata[i] / temp << endl;
          }
          cerr << "TSData::write_ascii -> time series data written " << endl;
        }


        if (ident == "steps") {
          for ( i = 0; i < steps.size(); i++) {
            ofs << double (datetime + time_offset) + double(steps[i])/samplefreq << "\x9"
                << step_vals[i] / temp << endl;
          }
          cerr << "TSData::write_ascii -> index of steps written" << endl;
        }


        if (ident == "spikes") {
          for ( i = 0; i < spikes.size(); i++) {
            ofs << double (datetime + time_offset)  + double(spikes[i])/samplefreq << "\x9"
                << spike_vals[i] / temp << endl;
          }
          cerr << "TSData::write_ascii -> index of spikes written " << endl;
        }

        if (ident == "stddev") {
          for ( i = 0; i < posv.size(); i++) {
            ofs << double (datetime + time_offset) + double(posv[i])/samplefreq << "\x9"
                << stddevs[i] / temp << endl;
          }
          cerr << "TSData::write_ascii -> index of standard devations written " << endl;
        }

        if (ident == "stddev_used") {
          if (use_min_std_f == DBL_MAX || use_max_std_f == DBL_MAX) {
            cerr << "TSData::write_ascii -> can not write " << ident << endl;
            cerr << "min max factors were: " << use_min_std_f << "  " << use_max_std_f << endl;
            return 0;
          }
          for ( i = 0; i < posv.size(); i++) {
            if ( (stddevs[i] >  use_min_std_f * my_stddev_stddev + my_stddev_median) &&
                (stddevs[i] <  use_max_std_f * my_stddev_stddev + my_stddev_median)) {

                ofs << double (datetime + time_offset)  + double(posv[i])/samplefreq << "\x9"
                    << stddevs[i] / temp << endl;
            }
          }
          cerr << "TSData::write_ascii -> index of standard devations for USE written " << endl;
        }

      } // UNIX
  } // absolute time

  } // time columne


  else {
      for ( i = 0; i < this->samples; i++) {
        ofs << ddata[i] / temp << endl;
      }
  }


  return 1;
}


/*!
force recalculation of lsb value
needed if integer values shall be written finally
*/


double TSData::calc_lsb() {

// diese routine muss noch einmal ueberdacht werden!
// es sei denn, die used bits drfen gr�ser werden ???

//double is_range = ddata.max() - ddata.min();

//    lsbval = double(is_range/pow(2.,24));
      cerr << "LSB: " << lsbval;
      
      if (ddata.max() > 0) lsbval = 2 * abs(ddata.max()) / (INT_MAX);
      else   lsbval = 2 * abs(ddata.min()) / (INT_MAX);
      cerr << "  -> " << lsbval << endl;
// +- 10V is ADC range; smallest step can be 10/2^24 = 6E-7 V
//cerr << ddata.min() << "  " << ddata.max() << "  " << lsbval << endl;

  if (lsbval < 6.E-7 && ( return_lower(channel_type) == "hx" || return_lower(channel_type) == "hy" || return_lower(channel_type) == "hz") ){
    cerr << "TSData::calc_lsb: lsbval is: " << lsbval << ", adjusting to 10/2^24 = 6E-7 V LSB (could be removed)" << endl;
    lsbval = 6.E-7;
  }

  if (lsbval < 3.E-7 && ( return_lower(channel_type) == "ex" || return_lower(channel_type) == "ey" || return_lower(channel_type) == "ez") ){
    cerr << "TSData::calc_lsb: lsbval is: " << lsbval << ", adjusting to 5/2^24 = 3E-7 V LSB (could be removed)" << endl;
    lsbval = 3.E-7;
  }

  if (abs(ddata.max() / lsbval) >= INT_MAX ) {
    lsbval *= 100;
    cerr << "TSData::calc_lsb: adjusting to bigger LSB: " << lsbval << endl;

  }

return lsbval;

}


//**************************************************************
// **************** interface to doubles ***********************
// neccessary for old C/C++ routines who only work with *double
/***************************************************************************
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
***************************************************************************/

int TSData::export_slice(double* f, unsigned int dstart, unsigned int dlength) {

    size_t i, j = 0;
    if ((dstart + dlength) > this->samples) {
      cerr << "\ntsdata::export_slice -> range check error!\n";
      exit(0);
    }
    for (i = dstart; i < (dstart + dlength); i++, j++) {
      f[j] = ddata[i];
    }
    return 1;
}

// getting data back into data valarray of the ats file
// it is not neccessary to read the complete ddata ... it can be a slice!
/***************************************************************************
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
***************************************************************************/

  int TSData::import_slice(double* f, unsigned int dstart, unsigned int dlength) {

    size_t i, j = 0;
    if ((dstart + dlength) > this->samples) {
      cerr << "\ntsdata::import_slice -> range check error!\n";
      exit(0);
    }
    for (i = dstart; i < (dstart + dlength); i++, j++) {
      ddata[i] = f[j];
    }
  // force re-calculation if requested
    TSData::data_changed();

    return 1;
  }

/***************************************************************************
*         concatunates two atsfiles                                       *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
***************************************************************************/

int TSData::catats(const TSData& M){
  size_t i, j = 0, fill, x_samples;

  // for linear interpolation between the data files
  // we start at first, which is the last sample of the first ats file
  // and end with last, which is the first sample of the second file
  double last = M.ddata[1], first = this->ddata[ddata.size()-1];
  last -= first;

  // do some basic checks to find whether it make sense what we are doing
  if (this->datetime > M.datetime) {
    cerr << "TSData::catats -> Date time of second ats file is in the past!" << endl;
    exit(1);
  }

  if (this->datetime == M.datetime) {
    cerr << "TSData::catats -> Date time of second ats file equal!" << endl;
    exit(1);
  }


  if (this->samplefreq != M.samplefreq) {
    cerr <<  "TSData::catats -> Sample Frequencies are different!" << endl;
    exit(1);
  }
  if (this->samplefreq < 2) {
  cerr <<  "TSData::catats -> use a sample frequency (band) with 1 Hz or higher" << endl;
  exit(0);
  }

  if (this->samples != unsigned (samples)) {
    cerr <<  "TSData::catats -> please use the COMPLETE files" << endl;
    exit(0);
  }
  if (this->sensor_type != M.sensor_type) {
    cerr <<  "TSData::catats -> WARNING sensor types are different: "
          << sensor_type << "  " << M.sensor_type << endl;
  }
  if (this->sensor_id != M.sensor_id) {
    cerr <<  "TSData::catats -> WARNING sensor serial numbers are different: "
          << this->sensor_id << "  " << M.sensor_id << endl;
  }

  if (this->dipole_length() != M.dipole_length()) {
    cerr << "TSData::catats -> WARNING different dipole length: "
        << this->dipole_length() << "  " << M.dipole_length() << endl;
  }

  cerr <<  "TSData::catats -> Dates:" << endl;
  cerr << "datetime: " << GMTtime(this->datetime);
  cerr << "stoptime: " << GMTtime(this->stoptime) <<  endl;
  cerr << "datetime: " << GMTtime(M.datetime);
  cerr << "stoptime: " << GMTtime(M.stoptime) <<  endl;

  // copy the old samples and calculate the gap between both files
  x_samples = this->samples;
  fill = (M.datetime - (this->datetime + this->samples /(int (this->samplefreq))) ) * int (this->samplefreq);

  cerr <<  "TSData::catats -> cat 1st " << this->samples << "\x9 fill: " << fill << "\x9 2nd " << M.samples;
  this->samples += fill;
  this->samples += M.samples;
  cerr << "\x9together " << this->samples << endl;

  // init the new data with the correct new size and copy the first file into it
  // xsampels are the old ssamples
  valarray<double> tddata(samples);
  for ( i = 0; i < x_samples; i++) {
    tddata[i] = ddata[i];
  }

  // now init the NEW ats file and copy the data; in-efficient but fust enough
  init_data(this->samples);
  this->ddata = tddata;

  //interpolate between the files
  last /= fill;
  for (i = x_samples; i < x_samples + fill; i++)  {
    this->ddata[i] = first + last * (i - x_samples);
  }
  // and copy the rest
  for(i = x_samples + fill; i < unsigned (this->samples); i++) {
    this->ddata[i] = M.ddata[j++];
    this->atm_select[i] = 1;  // exclude this part from processing
  }

  cerr <<  "TSData::catats -> ready" << endl;
  TSData::data_changed();
  return this->samples;
}



/* Linear Convolution*************************************************************************/
/***************************************************************************
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
***************************************************************************/

int TSData::linconv(unsigned int flen, double tarset_f) {

if (ddata.size() != this->samples) {
    cerr << "\natsdata::linconv -> different length of data and filter, exit" << endl;
    exit(0);
}
if (this->samples % flen) {
    cerr << "\natsdata::linconv -> samples divided by filter length is: " << (double) this->samples/flen << endl;
    cerr << "must be an integer!" << endl;
    exit(0);
}

unsigned int i, j;
unsigned int loop = this->samples / flen;
valarray<double> sinsig(flen), cossig(flen);
double ssum, csum, cabs, cphase;


//    ofstream ctr;
//    ctr.open("zz.dat");
cerr << "linear convolution start\n";
for ( j = 0; j < loop; j++) {
    cerr <<".";
    // folding the data
    for (i = 0; i < sinsig.size(); i++) {
          sinsig[i] = sin( (i + j * flen) / samplefreq * tarset_f * 2 * M_PI) * ddata[i + j * flen];
        cossig[i] = cos( (i + j * flen) / samplefreq * tarset_f * 2 * M_PI) * ddata[i + j * flen];

    }

    ssum = sinsig.sum();
    csum = cossig.sum();

    cabs = sqrt( ssum * ssum + csum * csum) * 2 / flen;
/*    cerr << "loop: " << j << "  ";
    if (csum <= 0) cerr << "-csum ";
    else cerr << "+csum ";

    if (ssum <= 0) cerr << "-ssum ";
    else cerr << "+ssum ";
*/
    if      ( csum > 0 && ssum < 0) cphase = M_PI + atan(csum/ssum);
    else if ( csum < 0 && ssum < 0) cphase = M_PI + atan(csum/ssum);
    else if ( csum < 0 && ssum > 0) cphase = atan(csum/ssum);

    else cphase = atan(csum/ssum);
//     cerr << cphase;
//     cerr << endl;
//cphase = -atan(csum/ssum);
// hier scheint eine phasenumkehr mgl -> signal verst�kung!
//     cerr << cphase << " " << j << "  ";
    // use sinsig as inverse
    for (i = 0; i < flen; i++) {
        sinsig[i] = sin((i + j * flen) / samplefreq * tarset_f * 2 * M_PI + cphase) * cabs;
      }

    for (i = 0; i < flen; i++) {
//       ctr << i + j * flen << "\x9" << i << "\x9" << ddata[i + j * flen] << "\x9" << sinsig[i] << "\x9" << ddata[i + j * flen] - sinsig[i] << endl;
      ddata[i + j * flen] -= sinsig[i]  ;
    }
  }
  cerr << " finished" << endl;
  TSData::data_changed();
return 1;
}


/***************************************************************************
*                                                                         *
*                                                                         *
*  read ascii values into data section                                    *
*                                                                         *
*                                                                         *
*                                                                         *
***************************************************************************/



size_t TSData::read_ascii(string filename, size_t col) {

int rows = 0;
ifstream ifs;
    ifs.open(filename.c_str());
    if (!ifs) {
      cerr << "\natsdata::read_ascii-> can not open " << filename << " for input, exit\n";
      exit(1);
    }
    samples = this->samples = my_valarray::read_nth_col(ifs, col, ddata);

    ifs.close();
    TSData::data_changed();
    cerr << "TSData::read_ascii -> " << samples << " samples read from file " << filename << ", LSB: " << lsbval << endl;


return rows;
}



/***************************************************************************
*                                                                         *
*                                                                         *
*  basic statistical values of the time series                            *
*                                                                         *
*  they are constructed that they are not calculated twice                *
*                                                                         *
***************************************************************************/

double TSData::max() {
  if (dmax != DBL_MAX) return dmax;
  else {
    dmax = ddata.max();
    return dmax;
  }
}

double TSData::min() {
  if (dmin != DBL_MAX) return dmin;
  else {
    dmin = ddata.min();
    return dmin;
  }
}

double TSData::mean() {
  if (dmean != DBL_MAX) return dmean;
  else {
    dmean = ddata.sum()/ddata.size();
    return dmean;
  }
}

double TSData::median() {
  if (dmedian != DBL_MAX) return dmedian;
  else {
    dmedian = my_valarray::median(ddata);
    return dmedian;
  }
}


double TSData::usedbits() {

  return log10( (ddata.max() - ddata.min()) / lsbval) / log10(2.0);


}

double TSData::scale() {


    // get_ E field to mV/km; mV is ADU unit; m should be inside atsheader
     if (return_lower(channel_type) == "ex" || return_lower(channel_type) == "ey") {

      valarray<double> senspos(DBL_MAX, 6);

      ddata /= (dipole_length()/1000.);
      cerr << "E Field scaled to mv/km; Dipole Length was " << dipole_length() << " metres "  << endl;

     // re-arrange the e-field if you want to re-write the file later

      diptopos(1000., e_angle());

      TSData::data_changed();

      return dipole_length();
     }

     else return 0;

}



int TSData::detrend() {
  my_valarray::detrend(ddata);
  TSData::data_changed();
  return 1;
}

size_t TSData::detrend_w(){                          // detrends every window wl

   valarray<double> time_slice(wl);

   // no idea yet to use detrend together with slice_arrays
   for (size_t i = 0; i < stacks; i++) {

       time_slice = ddata[slice(i*wl, wl, 1)];
       my_valarray::detrend(time_slice);
       ddata[slice(i*wl, wl, 1)] = time_slice;

   }

   TSData::data_changed();

return stacks;
}

int TSData::set_window_length(size_t iwl, size_t iovr) {          // get window length and overlay of FFT or other


  if (iwl == 1) this->wl = this->samples;
  if (iwl <= this->samples) this->wl = iwl;

  else {
    cerr << "TSData::set_window_length -> window greater than data size " << this->wl << endl;
    exit(0);
  }
  this->ovr = iovr;
   if (this->ovr > this->wl/2) {
      cerr << "TSData::set_window_length  -> overlay shouldn't be greater than window/2, ovr, wl " << this->ovr << ",  " << this->wl << endl;
   }
  this->stacks =  1 + ( (this->samples - this->wl) / (this->wl - this->ovr));
  cerr << "TSData::set_window_length -> wl: " << this->wl << "  " << "ovr: " << this->ovr << "  " << "stacks: " << this->stacks << endl;

return 1;
}



double TSData::stddev() {
  if (dstddev != DBL_MAX) return dstddev;
  else {
    dstddev = my_valarray::stddev(ddata);
    return dstddev;
  }
}












#ifdef myMySQL

	/*!
		write parts of the file
		if apply lsb is activated the double represents then the integer of
		the origina�digital time series
		if time col is 0, only data is written                                                                                                          *
	
	*/
		int TSData::write_mysql(Query& query, int i_time_col, int i_relative_time,
										long time_offset, string dateformat, int i_app_lsb, string& ident ) {
	
		// test for output
		size_t i, j = 0;
		double temp = 1.;
		string comm;
	
		if (i_app_lsb) temp = lsbval;
	
	
	
				cerr << "TSData::write_mysql -> offset is date + offset: " << datetime << " + " << time_offset << endl;
				long newtime =  datetime + time_offset;
				cerr << "TSData::write_mysql -> approx! offsetts are: original, shift: " << endl;
				cerr << GMTtime(datetime);
				cerr << GMTtime(newtime);
	
				mysql_simple_vector::insert_all(query, return_lower(channel_type), return_lower(channel_type), &ddata[0], &ddata[ddata.size()]);
	
				cerr << "TSData::write_mysql -> time series data written " << endl;
	
				if (i_time_col) {
					if (dateformat == "ISO") {
						cerr << "TSData::write_mysql -> using ISO 8601 date format " << endl;
						query << "insert into  iso8601 " << " values ";
	
						query.precision(2);
						query.setf(ios_base::scientific);
						for ( i = 0; i < this->samples-1; i++) {
							query << "('" <<   GMTISOtime(datetime, j++, samplefreq) << "'), ";
	
						}
						query << "('" <<   GMTISOtime(datetime, j++, samplefreq) << "'); ";
	
					}
					else {
	
						cerr << "TSData::write_mysql -> using UNIX time stamp date format " << endl;
						query << "insert into  unix " << " values ";
	
						for ( i = 0; i < this->samples-1; i++) {
									query << "(" <<  (datetime + time_offset) + (long) (double(j++)/samplefreq) << "), ";
						}
						query << "(" <<  (datetime + time_offset) + (long) (double(j++)/samplefreq) << "); ";
					}
					//cerr << "Query " << query.preview() << endl;
					query.execute();
					
					query.reset();
				}
	
	
		return 1;
	}
	// endif mysql 
#endif


size_t TSData::read_int(ifstream& ifs,  size_t skip_samples_read,  size_t n_samples, const size_t n_slice) {

    // test input file
    if (!ifs) {
      cerr << "\nTSData::read_int -> can not read ifstream";
      exit(0);
    }

    #ifdef use_shift_to_swap
      cerr << "TSData::read_int -> using byte swap (s);  " << endl;
    #endif
    #ifdef use_template_to_swap
      cerr << "TSData::read_int -> using byte swap (t);  " << endl;
    #endif


    size_t i, j;
    size_t skip_complete_channel_data = 0, skip_slicesamples = 0;

    this->skip_samples_read = skip_samples_read;


    // handle incorrect values first
    if (this->skip_samples_read > this->samples_orig) {
      cerr << "\natsdata::read -> you skip more samples than existing in the file skip:" << this->skip_samples_read << "  exist:" << this->samples_orig << "\n";
      exit(0);
    }
    else if  ( samples + this->skip_samples_read == this->samples_orig) {
      this->samples = samples;
    }

    else if (!samples ||
        (this->skip_samples_read < samples && samples == this->samples_orig) ) {
      this->samples = samples - this->skip_samples_read;

    }

    else if (samples > this->samples_orig) {
      cerr  << "\natsdata::read samples is greater than total samples in file\n";
      exit(0);
    }




    if (this->skip_samples_read + this->samples > this->samples_orig) {
      cerr << "\natsdata::read -> you try to skip and read more samples than existing in the file\n";
      cerr << "skip: " << skip_samples_read << " samples: " << this->samples << " orig samples: " <<  this->samples_orig << endl;
      cerr << "if you want to filter data where you know that the start time is the same for all file try the option -sync2 -nogrid to avoid adjustment of start time " << endl;
      exit(0);
    }


    // include function header data into object

    init_data(this->samples, true);


    cerr << "TSData::read -> skipping " << skip_samples_read << " samples   ";


    if (headertype == "ats" || headertype == "bgrf1" || headertype == "emimt24") {

      if (ifs.tellg() != streampos(headerlength)) {
        cerr << "TSData::read -> wrong file position is: " <<  ifs.tellg() << " should be " << headerlength << "  ... correcting" << endl;
        ifs.seekg(headerlength);

      }

      if ( n_slice && this->tslices.size()) {
          // try to fetch the slice
          for (size_t jjj = 0; jjj < n_slice; ++jjj) {
              skip_slicesamples += this->tslices.at(jjj).iSamples;
          }

          cerr << "TSData::read -> skipping " << n_slice << " slices, new data pos: " << skip_slicesamples << endl;

      }

        // skip data is neccessary ************************************************************************
        if (skip_samples_read + skip_slicesamples || channel_no) {
          // init dummy vector
            if (channel_no) cerr << "test: reading channel no: " << channel_no << endl;
            //skip_complete_channel_data = channel_no * goe.channel_blocksize() / n_channels;
            if (storagebits == 32) {
              for ( i = 0; i < skip_samples_read + skip_slicesamples + skip_complete_channel_data; i++) {
                ifs.read(static_cast<char *> (static_cast<void *>(&idata32)), sizeof( int));
              }
            }
            if (storagebits == 16) {
              for ( i = 0; i < skip_samples_read + skip_slicesamples + skip_complete_channel_data; i++) {
                ifs.read(static_cast<char *> (static_cast<void *>(&idata16)), sizeof( int));
              }
            }

        }
				// end skip data

        //modify the datetime
        datetime = datetime + long (skip_samples_read / samplefreq);
        stoptime = datetime + long (this->samples / samplefreq);
        cerr << "TSData::read -> reading " << this->samples << " samples " << endl;
        cerr << "TSData::read -> starting at date: " << GMTtime(datetime) << "TSData::read -> stopping at date: " << GMTtime(stoptime);
        cerr << "TSData::read -> ADCOffset: " << adc_dcoffset << " LSBVal: " << lsbval << "  extra factor: " << adc_factor << endl;

        if (storagebits == 32) {
          for (i = 0; i < this->samples; i++ ) {
              ifs.read(static_cast<char *> (static_cast<void *>(&idata32)), sizeof( int));
              #ifdef use_shift_to_swap
                idata32 = bswap_32(idata32);
              #endif
              #ifdef use_template_to_swap
                tbswap(idata32);
              #endif

              idata[i] = idata32;


          }
        }
        if (storagebits == 16) {
          for (i = 0; i < this->samples; i++ ) {
              ifs.read(static_cast<char *> (static_cast<void *>(&idata16)), sizeof( short int));
              #ifdef use_shift_to_swap
                idata16 = bswap_16(idata16);
              #endif
              #ifdef use_template_to_swap
                tbswap(idata16);
              #endif
              ddata[i] = (int) (idata16);
          }
        }

    }



    cerr << "TSData::read_int -> finished\n";
    return 1;
}

//end read_int









/*!
  write the complete file
*/
  int TSData::write_int(ofstream& ofs, size_t nbits) {

  // it should not be neccessary to calculate the LSBValue here!
  // should be done earlier
    if (!ofs) {
      cerr << "\natsdata::write -> can not write to ofstream";
      exit(0);
    }
    #ifdef use_shift_to_swap
      cerr << "TSData::write -> using byte swap (s);  " << endl;
    #endif
    #ifdef use_template_to_swap
      cerr << "TSData::write -> using byte swap (t);  " << endl;
    #endif


    size_t i;
    if (nbits == 32) {
      for (i = 0; i < this->samples; i++) {
        idata32 = idata[i];
        #ifdef use_shift_to_swap
          idata32 = bswap_32(idata32);
        #endif
        #ifdef use_template_to_swap
          tbswap(idata32);
        #endif
        ofs.write(static_cast<char *> (static_cast<void *>(&idata32)), sizeof( int));
      }
    }

    if (nbits == 16) {
      for (i = 0; i < this->samples; i++) {
        idata16 = (short int) (idata[i]);
        #ifdef use_shift_to_swap
          idata16 = bswap_16(idata16);
        #endif
        #ifdef use_template_to_swap
          tbswap(idata16);
        #endif
        ofs.write(static_cast<char *> (static_cast<void *>(&idata16)), sizeof( int));
      }
    }




    return 1;


  }










