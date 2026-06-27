#ifndef MFS07CAL_H
#define MFS07CAL_H

#include "icalibration.h"
#include "my_valarray.h"
#include "my_valarray_double.h"
#include <string>
#include "string_utils.h"
#include <algorithm>
#include "allinclude.h"

using namespace std;

/**
@author Bernhard Friedrichs
 */
// mfs07

class MFS07Cal : public ICalibration {
  public:
    MFS07Cal();

    ~MFS07Cal();

    virtual size_t read(const valarray<double>& newfreq, string chopper, const double& samplefreq, const double& gain,
                        const double& factor, const double& offset, string adc, string tags);
    virtual size_t dump(const string& fname);

    virtual void set_filename(const string& fname) {
      this->filename = fname;
    }
    /*!
    return frequencies
     */
    virtual valarray<double>& freq() {
      return this->freqs;
    }

    /*!
    return complex transfer function
     */
    virtual valarray<complex<double> >& trfkt() {
      return this->trf;
    }



    /*!
    interpolate to a given list
     */
    virtual size_t interpolate(const valarray<double>& newfreq);

    /*!
    interpolate and extrapolate
     */
    virtual size_t interpolate_extend_theoretical(const valarray<double>& newfreq, string chopper, const double& samplefreq, const double& gain,
        const double& factor,  const double& offset, string adc, string tags);

    /*!
    use a build in transfer function
     */
    virtual size_t hardwired(const valarray<double>& newfreq, string chopper, const double& samplefreq, const double& gain, const double& factor,
                             const double& offset, string adc, string tags);

    /*!
    use a theoretical function
     */
    virtual size_t theoretical(const valarray<double>& newfreq, string chopper, const double& samplefreq, const double& gain, const double& factor,
                               const double& offset, string adc, string tags);



    virtual size_t set_calib(const valarray<double>& f, const valarray<complex<double> >& trfn);
    virtual size_t set_calib(const valarray<double>& f, const valarray<double>& ampl, const valarray<double>& ph_deg);

    virtual string my_name() {  return name;}

  private:
    valarray<double> freqs;
    valarray<complex<double> > trf;
    string filename;
    string name;

};

#endif

