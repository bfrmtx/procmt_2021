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
#ifndef MTXCAL_H
#define MTXCAL_H

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
// mfs05, mfs06, mfs06u, kim877

class MTXCal : public ICalibration {
public:
    MTXCal();

    ~MTXCal();

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
