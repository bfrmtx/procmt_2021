
#include "adu07cal.h"

ADU07cal::ADU07cal()
        : ICalibration() {
    name = "adu07cal";
}


ADU07cal::~ADU07cal() {}


/*!
  \fn ADU07cal::read(string filename, string chopper)
  reads from a file; chopper: on, off, auto, none
  simple check for double entry,
  makes sure that f[0] < f[1]
*/
size_t ADU07cal::read(const valarray<double>& newfreq, string chopper = "on", const double& samplefreq = 0, const double& gain = 0,
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
            cerr << "ADU07cal::read -> can nor open file " << filename << " neither " << share_dir << " exit" << endl;
            exit(0);
        } else
            cerr << "ADU07cal::read -> local file not found, taking " << share_dir << " instead" << endl;
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
        cerr << "ADU07cal::read -> can not find key words chopper on or chopper off; I try a file with numbers only now.... check!!!" << endl;

        ifs.clear();
        ifs.seekg(0);
        chopper = "no chopper info, blank file, freq[hz], ampl [V/(nT*Hz)], phase[deg]";
    }


    cerr << "ADU07cal::read -> using " << chopper << endl;
    while (ifs) {
        ifs >> dx >> dy >> dz;							// read temp variables
        dy *=  (dx * 1000.);                // mtx data is normalized by frequency and we do NOT keep that!!!!!!!!!

        // file should have V/(nT *Hz) as units; so multiply
        // with 1000 to get mV as (stored in ats files)


        z = polar(dy ,(M_PI/180.) * dz);
        // if last operation was successful
        if (ifs) {
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
        cerr << "ADU07cal::read -> at least on frequency with double entry!!! " << endl;
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
                        \fn ADU07cal::dump(string filename)
                        debug function; just dump values to file
*/

size_t ADU07cal::dump(const string& filename) {

    ofstream ofs;
    size_t i;
    if (!filename.size()) {
        cerr << "ADU07cal::dump -> no filename given" << endl;
        return 0;
    }
    ofs.open(filename.c_str());
    if (!ofs) {
        cerr << "ADU07cal::dump -> not open output stream" << endl;
        return 0;

    }

    if (!freqs.size() || !trf.size()) {
        cerr << "ADU07cal::dump -> no frequencies / complex data to dump" << endl;
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
valarray<double>& ADU07cal::freq() {
    return freqs;
}


/*!
                        return complex
*/
valarray<complex<double> >& ADU07cal::trfkt() {
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
size_t ADU07cal::interpolate(const valarray<double>& newfreq) {

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

        cerr << "ADU07cal::interpolate -> interpolating adu trf (" << newfreq[1] << " - "
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
        cerr << "ADU07cal::interpolate -> interpolating ERROR" << endl;



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
size_t ADU07cal::interpolate_extend_theoretical(const valarray<double>& newfreq, string chopper, const double& samplefreq, const double& gain,
        const double& factor = 0,  const double& offset = 0, string adc = "", string tags = "") {
    cerr << "ADU07cal::interpolate_extend_theoretical -> using theoretical function " << endl;
    ADU07cal::theoretical(newfreq, " ", samplefreq, gain, 0, 0, " ", " ");

    return freqs.size();
}

/*!
                                                                      use a build in transfer function
*/
size_t ADU07cal::hardwired(const valarray<double>& newfreq, string chopper, const double& samplefreq, const double& gain, const double& factor,
                           const double& offset, string adc, string tags) {


    return freqs.size();
}

/*!
                                                                          use a theoretical function
                                                                          we should use samplefreq_orig to avoid changes after decimation
*/
size_t ADU07cal::theoretical(const valarray<double>& newfreq, string chopper, const double& samplefreq, const double& gain,
                             const double& factor = 0,  const double& offset = 0, string adc = "", string tags = "") {



    complex<double> re (1.,0.), im(0.,1.), ctemp, p1;
    size_t i, n;

    cerr << "ADU07cal::theoretical assuming linear";

    return 0;
}

/*!
                                                                              \fn ADU07cal::set_calib(const double& f, const complex<double>& trfn)
*/
size_t ADU07cal::set_calib(const valarray<double>& f, const valarray<complex<double> >& trfn) {
    if (f.size() != trfn.size()) {
        cerr << "ADU07cal::set_calib -> f and trf are not of the same size, exit!" << endl;
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
size_t ADU07cal::set_calib(const valarray<double>& f, const valarray<double>& ampl, const valarray<double>& ph_deg) {
    /// @todo implement me
    return 0;
}


