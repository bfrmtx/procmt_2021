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
#include "adu06cal.h"

ADU06cal::ADU06cal()
        : ICalibration() {
 name = "adu06cal";				
}


ADU06cal::~ADU06cal() {}


/*!
    \fn ADU06cal::read(string filename, string chopper)
     reads from a file; chopper: on, off, auto, none
     simple check for double entry,
     makes sure that f[0] < f[1]
 */
size_t ADU06cal::read(const valarray<double>& newfreq, string chopper = "on", const double& samplefreq = 0, const double& gain = 0,
                      const double& factor = 0,  const double& offset = 0, string adc = "", string tags = "") {
    if (!filename.size())
        return 0;
    ifstream ifs;

    ifs.open(filename.c_str());
    string share_dir = "/usr/local/share/";
    if (!ifs) {
        ifs.close();
        ifs.clear();
        share_dir = share_dir + filename;
        ifs.open(share_dir.c_str());
        if (!ifs) {
            cerr << "ADU06cal::read -> can nor open file " << filename << " neither " << share_dir << " exit" << endl;
            exit(0);
        } else
            cerr << "ADU06cal::read -> local file not found, taking " << share_dir << " instead" << endl;
    }

    vector<double> ix;
    vector<complex<double> > iz;
    double dx, dy, dz;
    complex<double> z;
    string line;

    

    if ( ( return_lower(chopper) == "on") || (return_lower(chopper) == "off") || (return_lower(chopper) == "none")) {
        // metronix uses chopper on or chopper off or chopper none
        chopper = "chopper " + chopper;    // is "chopper on" now for example
    }

    if ( (chopper == "auto") && (samplefreq > 512) )
        chopper = "chopper off";
    else if ( (chopper == "auto") && (samplefreq <= 512) )
        chopper = "chopper on";

    do {
        getline(ifs, line, '\n');
        lower(line);
        if ( (line.find(chopper) != line.npos))
            break;
    } while (ifs);

    if (ifs.eof()) {
        cerr << "ADU06cal::read -> can not find key words chopper on or chopper off; I try a file with numbers only now.... check!!!" << endl;

        ifs.clear();
        ifs.seekg(0);
        chopper = "no chopper info, blank file, freq[hz], ampl [V/(nT*Hz)], phase[deg]";
    }


    cerr << "ADU06cal::read -> using " << chopper << endl;
    while(ifs) {
        ifs >> dx >> dy >> dz;							// read temp variables
        dy *=  (dx * 1000.);                // mtx data is normalized by frequency and we do NOT keep that!!!!!!!!!

        // file should have V/(nT *Hz) as units; so multiply
        // with 1000 to get mV as (stored in ats files)


        z = polar(dy ,(M_PI/180.) * dz);
        // if last operation was successful
        if(ifs) {
            ix.push_back(dx);
            iz.push_back(z);

        }

    }

    freqs.resize(ix.size());
    trf.resize(ix.size());

    // copy data into valarrays real and complex
    copy(&ix[0], &ix[ix.size()], &freqs[0]);
    copy(&iz[0], &iz[iz.size()], &trf[0]);

    // some checks
    size_t i = size_t ( adjacent_find(&freqs[0], &freqs[freqs.size()]) - &freqs[0]);

    if (i != freqs.size()) {
        cerr << "ADU06cal::read -> at least on frequency with double entry!!! " << endl;
        exit(0);
    }

    // sort frequency if neccessaray
    if (freqs[1] > freqs[2]) {
        reverse(&freqs[0], &freqs[freqs.size()]);
        reverse(&trf[0], &trf[trf.size()]);
    }



    return freqs.size();



}

/*!
    \fn ADU06cal::dump(string filename)
     debug function; just dump values to file
 */

size_t ADU06cal::dump(const string& filename) {

    ofstream ofs;
    size_t i;
    if (!filename.size()) {
        cerr << "ADU06cal::dump -> no filename given" << endl;
        return 0;
    }
    ofs.open(filename.c_str());
    if (!ofs) {
        cerr << "ADU06cal::dump -> not open output stream" << endl;
        return 0;

    }

    if (!freqs.size() || !trf.size()) {
        cerr << "ADU06cal::dump -> no frequencies / complex data to dump" << endl;
        return 0;

    }

    for (i = 0; i < freqs.size(); i++) {

        ofs << freqs[i] << "  " << trf[i].real() << "  " << trf[i].imag() << "       " << abs(trf[i]) << "  " << (180. / M_PI) * arg(trf[i]) << endl;

    }



    return freqs.size();
}




/*!
return frequencies
*/
valarray<double>& ADU06cal::freq() {
    return freqs;
}


/*!
  return complex
  */
valarray<complex<double> >& ADU06cal::trfkt() {
    return trf;
}


/*!
interpolate to a given list
f(wl/2 +1)
for ( i = 0; i < f.size(); i++) f[i] = i * samplefreq / wl;
 
 since induction coils having a frequency dependend output interpolation is bad for low frequencies
 where interpolation especially for tha phase is very bad; solution: normalize again by f, interpolate, and
 multiply by f again
*/
size_t ADU06cal::interpolate(const valarray<double>& newfreq) {

    size_t i;
    valarray<double> b, c, d;

    // transfer function must be loaded here!!
    if (trf.size()) {


        //normalize for better interpolation

        for (size_t ii = 1; ii < trf.size(); ii++)
            trf[ii] /= freqs[ii];
        // can be DC, check
        if (freqs[0])
            trf[0] /= freqs[0];

        //copy original data into temp
        valarray<double>temp_re(trf.size()), temp_im(trf.size());
        my_valarray::cplx2ri(trf, temp_re, temp_im);



        // generate new size - empty
        trf.resize(newfreq.size());

        cerr << "ADU06cal::interpolate -> interpolating adu trf (" << newfreq[1] << " - "
        << newfreq[newfreq.size()-1] << " Hz), " << newfreq.size() << "pts" << endl;

        //
        // use the old data to prepare
        my_valarray_double::aspline(freqs, temp_re, b, c, d);
        for (i = 0; i < trf.size(); i++)
            trf[i] = complex<double> (my_valarray_double::seval(newfreq[i], freqs, temp_re, b, c, d), 0.);

        // use the old data to prepare
        my_valarray_double::aspline(freqs, temp_im, b, c, d);
        for (i = 0; i < trf.size(); i++)
            trf[i] = complex<double> (trf[i].real(),
                                      my_valarray_double::seval(newfreq[i], freqs, temp_im, b, c, d));


    } else
        cerr << "ADU06cal::interpolate -> interpolating ERROR" << endl;



    freqs.resize(newfreq.size());
    freqs = newfreq;

    // undo normalization
    for (size_t ii = 1; ii < trf.size(); ii++)
        trf[ii] *= freqs[ii];
    // can be DC, check
    if (freqs[0])
        trf[0] *= freqs[0];
    // else correct DC part
    else
        trf[0] = complex<double> (1.0, 0.);



    return freqs.size();
}



/*!
interpolate and extrapolate
*/
size_t ADU06cal::interpolate_extend_theoretical(const valarray<double>& newfreq, string chopper, const double& samplefreq, const double& gain,
        const double& factor = 0,  const double& offset = 0, string adc = "", string tags = "") {
    cerr << "ADU06cal::interpolate_extend_theoretical -> using theoretical function " << endl;
		ADU06cal::theoretical(newfreq, " ", samplefreq, gain, 0, 0, " ", " ");

    return freqs.size();
}

/*!
use a build in transfer function
*/
size_t ADU06cal::hardwired(const valarray<double>& newfreq, string chopper, const double& samplefreq, const double& gain, const double& factor,
                           const double& offset, string adc, string tags) {


    return freqs.size();
}

/*!
use a theoretical function
we should use samplefreq_orig to avoid changes after decimation
*/
size_t ADU06cal::theoretical(const valarray<double>& newfreq, string chopper, const double& samplefreq, const double& gain,
                             const double& factor = 0,  const double& offset = 0, string adc = "", string tags = "") {



    complex<double> re (1.,0.), im(0.,1.), ctemp, p1;
    size_t i, n;


    if (samplefreq > 4009 || gain > 24.) {
        freqs.resize(newfreq.size());
        freqs = newfreq;
        trf.resize(freqs.size());


        cerr << "ADU06cal::theoretical-> transfer function for " << freqs[1] << " - " << freqs[freqs.size()-1] << " [Hz] ";

        if (freqs[0] > 0)
            n = 0;
        else
            n = 1;

        // please refer to section gen_trf!!!!!!!!!!
        // check for: adu trf seems to be inverse defined

        if ( (24. < gain) && ( 36. > gain) ) {
            cerr << "gain used" << endl;
            for (i = n; i < freqs.size(); i++) {
                p1 = (freqs[i] / 33800.) * im;
                ctemp = (1. / ( 1. + p1));
                trf[i] = ctemp;
            }
        }
        else
            for (i = n; i < freqs.size(); i++) trf[i] += re;


        if (samplefreq > 4097) {          // HF Band of ADU-06

            cerr << " HF Band used ";

            // high pass filter only switched on for samplein rates 40kHz!!!!!
            //trf.resize(freqs.size());
            for (i = n; i < freqs.size(); i++) {
                p1 = (freqs[i]/846.) * im;
                ctemp = (p1 / ( 1. + p1));
                trf[i] *= ctemp;
            }

            // this filter is switched on channel wise if Gain was ON
            // also relevant for HF only

            // this radio filter is always on
            // also relevant for HF only
            for (i = n; i < freqs.size(); i++) {
                p1 = (freqs[i] / 338000.) * im;
                ctemp = (1. / ( 1. + p1));
                trf[i] *= ctemp;
            }


            if (n)
                trf[0] = complex<double> (1.,0.);

            // ceck again the DC part
            if (freqs[0] == 0)
                trf[0] = complex<double> (1.,0.);

        }

        cerr << endl;
				return freqs.size();
    }

		// this will crash because freqs are not initialized 
    // if (freqs[0] == 0) trf[0] = complex<double> (1.,0.);

    return 0;
}

/*!
    \fn ADU06cal::set_calib(const double& f, const complex<double>& trfn)
 */
size_t ADU06cal::set_calib(const valarray<double>& f, const valarray<complex<double> >& trfn) {
    if (f.size() != trfn.size()) {
        cerr << "ADU06cal::set_calib -> f and trf are not of the same size, exit!" << endl;
        exit(0);
    }

    freqs.resize(f.size());
    trf.resize(f.size());
    freqs = f;
    trf = trfn;

    return freqs.size();
}





/*!
    \fn ICalbiration::set_calib(const double& f, const double& ampl, const double& ph_deg)
 */
size_t ADU06cal::set_calib(const valarray<double>& f, const valarray<double>& ampl, const valarray<double>& ph_deg) {
    /// @todo implement me
    return 0;

}


