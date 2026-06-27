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
#include "ltscal.h"

LTSCal::LTSCal()
        : ICalibration() {

    name = "ltscal";
}


LTSCal::~LTSCal() {}


/*!
  \fn LTSCal::read(string filename, string chopper)
  reads from a file; chopper: on, off, auto, none
  simple check for double entry,
  makes sure that f[0] < f[1]
*/
size_t LTSCal::read(const valarray<double>& newfreq, string chopper = "on", const double& samplefreq = 0, const double& gain = 0,
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
            cerr << "LTSCal::read -> can nor open file " << filename << " neither " << share_dir << " exit" << endl;
            exit(0);
        } else
            cerr << "LTSCal::read -> local file not found, taking " << share_dir << " instead" << endl;
    }

    double dy;

    freqs.resize(1);
    trf.resize(1);


    cerr << "LTSCal::read -> using acqua format; single number nT/V" << chopper << endl;

    ifs >> dy ;              // read temp variables
    dy =  1000./(dy);


    // file should have V/(nT *Hz) as units; so multiply
    // with 1000 to get mV as (stored in ats files)
    // hence that mapros might uses nT instead of pT


    trf[0] = polar(dy ,0.);
    freqs[0] = 1;
    // if last operation was successful

    cerr << "done\n";


    return freqs.size();



}

/*!
                        \fn LTSCal::dump(string filename)
                        debug function; just dump values to file
*/

size_t LTSCal::dump(const string& filename) {

    ofstream ofs;
    size_t i;
    if (!filename.size()) {
        cerr << "LTSCal::dump -> no filename given" << endl;
        return 0;
    }
    ofs.open(filename.c_str());
    if (!ofs) {
        cerr << "LTSCal::dump -> not open output stream" << endl;
        return 0;

    }

    if (!freqs.size() || !trf.size()) {
        cerr << "LTSCal::dump -> no frequencies / complex data to dump" << endl;
        return 0;

    }

    for (i = 1; i < freqs.size(); i++) {

        ofs << freqs[i] << "  " << trf[i].real() << "  " << trf[i].imag() << "                     "
        << abs(trf[i]) << "  " << (180. / M_PI) * arg(trf[i]) << "               "
        << (abs(trf[i])  / freqs[i]) / 1000. << "  " << (180. / M_PI) * arg(trf[i]) << endl;

    }



    return freqs.size();
}




/*!
                        interpolate to a given list
                        f(wl/2 +1)
                        for ( i = 0; i < f.size(); i++) f[i] = i * samplefreq / wl;

since induction coils having a frequency dependend output interpolation is bad for low frequencies where interpolation especially for tha phase is very bad; solution: normalize again by f, interpolate, and
                                                                  multiply by f again
*/
size_t LTSCal::interpolate(const valarray<double>& newfreq) {

    size_t i;
    valarray<double> b, c, d;

    // transfer function must be loaded here!!
    if (trf.size()) {


        freqs.resize(newfreq.size());
        freqs = newfreq;

        // generate new size - empty
        complex<double> z;
        z = trf[0];
        trf.resize(newfreq.size());

        for (i = 0; i < trf.size(); i++) trf[i] = z;





        cerr << "LTSCal::interpolate -> interpolating sensor trf (" << newfreq[1] << " - "
        << newfreq[newfreq.size()-1] << " Hz), " << newfreq.size() << "pts" << endl;


    } else
        cerr << "LTSCal::interpolate -> interpolating ERROR" << endl;


    return freqs.size();
}



/*!
interpolate and extrapolate
*/
size_t LTSCal::interpolate_extend_theoretical(const valarray<double>& newfreq, string chopper = "on", const double& samplefreq = 0,
        const double& gain = 0, const double& factor = 0,  const double& offset = 0, string adc = "", string tags = "") {

    valarray<double> old_f(freqs);  // save the frequencies
    LTSCal::interpolate(newfreq);
    complex<double> z = trf[0];   // squid is a single number


    // interpolate with theoretical transfer function instead of spline
    // outside give values from read in transfer function
    // phase wil jump about 0.1 deg @ 0.1 Hz ... where the measured trf function ends
    // there is no theoretical function for chopper off!
    // interpolation gives bad results for imaginary part!
    complex<double> p1, p2, im(0,1.);

    if (freqs[freqs.size()-1] < 513) {
        cerr << "LTSCal::interpolate_extend_theoretical -> extrapolating trf function for LTS" << endl;
        for (size_t i = 0; i < freqs.size(); i++) {
            if ( (freqs[i] < old_f.min()) || (freqs[i] > old_f.max()) ) {

                trf[i] = z;
                // theoretical fluxgate is constant


            }
        }

    }




    return freqs.size();
}

/*!
                                                                      use a build in transfer function
*/
size_t LTSCal::hardwired(const valarray<double>& newfreq, string chopper, const double& samplefreq, const double& gain, const double& factor,
                        const double& offset, string adc, string tags) {


    return freqs.size();
}

/*!
                                                                          use a theoretical function
*/
size_t LTSCal::theoretical(const valarray<double>& newfreq, string chopper = "on", const double& samplefreq = 0, const double& gain = 0,
                          const double& factor = 0,  const double& offset = 0, string adc = "", string tags = "") {

    cerr << "LTSCal::theoretical -> trf function for FGS01 " << newfreq.size() << endl;
    freqs.resize(newfreq.size());
    freqs = newfreq;
    trf.resize(newfreq.size());
    for (size_t i = 0; i < freqs.size(); i++) {
        trf[i] = complex<double>(0.1, 0.0);
        // theoretical fluxgate is constant
    }




    return freqs.size();
}




/*!
\fn LTSCal::set_calib(const double& f, const complex<double>& trfn)
*/
size_t LTSCal::set_calib(const valarray<double>& f, const valarray<complex<double> >& trfn) {
    if (f.size() != trfn.size()) {
        cerr << "LTSCal::set_calib -> f and trf are not of the same size, exit!" << endl;
        exit(0);
    }

    freqs.resize(f.size());
    trf.resize(f.size());
    freqs = f;
    trf = trfn;

    return freqs.size();
}





/*!
\fn
ICalbiration::set_calib(const double& f, const double& ampl, const double& ph_deg)
*/
size_t LTSCal::set_calib(const valarray<double>& f, const valarray<double>& ampl, const valarray<double>& ph_deg) {
    /// @todo implement me
    return 0;
}


