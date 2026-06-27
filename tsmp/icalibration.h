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
#ifndef ICALIBRATION_H
#define ICALIBRATION_H

#include <valarray>
#include <vector>
#include <cstdlib>
#include <complex>
#include "allinclude.h"

using namespace std;

/**
@author Bernhard Friedrichs
*/
class ICalibration {
public:
    ICalibration();

    ~ICalibration();
    virtual size_t read(const valarray<double>& newfreq, string chopper = "on", const double& samplefreq = 0, const double& gain = 0,
                        const double& factor = 0,  const double& offset = 0, string adc = "", string tags = "") = 0;
    virtual size_t dump(const string& fname) = 0;

    /*
    in order to use always the same parameters for read and theoretical etc. get filename is a extr routine
    */
    virtual void set_filename(const string& fname) = 0;
    /*!
    return frequencies
    make sure that freqs[0] is lowest frequency or DC and freqs[freqs.size()-1] the highest!!!
    */
    virtual valarray<double>& freq() = 0;

    /*!
    return complex transfer function; in main use simply spectra/xxx->trfkt() to apply the transfer function
    */
    virtual valarray<complex<double> >&  trfkt()  = 0;


    /*!
    interpolate to a given list
    */
    virtual size_t interpolate(const valarray<double>& newfreq) = 0;

    /*!
    interpolate and extrapolate
    */
    virtual size_t interpolate_extend_theoretical(const valarray<double>& newfreq, string chopper, const double& samplefreq, const double& gain,
            const double& factor,  const double& offset, string adc, string tags) = 0;

    /*!
    use a build in transfer function
    */
    virtual size_t hardwired(const valarray<double>& newfreq, string chopper, const double& samplefreq, const double& gain, const double& factor,
                             const double& offset, string adc, string tags) = 0;

    /*!
    use a theoretical function
    */
    virtual size_t theoretical(const valarray<double>& newfreq, string chopper, const double& samplefreq, const double& gain, const double& factor,
                               const double& offset, string adc, string tags) = 0;


    virtual size_t set_calib(const valarray<double>& f, const valarray<complex<double> >& trfn) = 0;
    virtual size_t set_calib(const valarray<double>& f, const valarray<double>& ampl, const valarray<double>& ph_deg) = 0;


    virtual string my_name() = 0;


};



#endif
