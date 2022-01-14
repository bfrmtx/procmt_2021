#include "fftreal_vector.h"



fftreal_vector::fftreal_vector()
{

}

fftreal_vector::~fftreal_vector()
{
    if (this->fft_object != nullptr) delete this->fft_object;

}



bool fftreal_vector::set_buffer(const std::vector<double> &in_out_buf, std::vector<std::complex<double>> &outbuf,
                                const double freq, const bool bwincal, const bool isdcfft, const bool nspc,
                                const bool inverse_from_forward, const double cut_upper, const double cut_lower)
{
    if (!in_out_buf.size()) return false;                           // must have a size!
    this->wl = in_out_buf.size();                                   // catch the site independed
    this->psrc.resize(this->wl);                                    // so that is the only wl - all others should be wl/2 or less
    // onlyy if inverted fft will use full wl and OVERWRITE in_out_buf
    if (this->fft_object != nullptr) delete this->fft_object;
    this->fft_object = new ffft::FFTReal<double>(long(this->wl));

    this->fcut_upper = cut_upper;
    this->fcut_lower = cut_lower;
    this->isdcfft = isdcfft;
    this->inverse_from_forward = inverse_from_forward;
    this->nspc = nspc;                                              // always FALSE except for inverse FFT incase


    // we generate frequencies start & stop
    // Hence that always 0... < wl (ts) -> 0...< wl/2 (spc) will be processed; start & stop will be a subset defined by cut upper & lower
    if (freq != 0.0) {
        this->fwl = double (this->wl);
        this->f_sample =  freq;
        if (bwincal) this->wincal = sqrt(1./double(f_sample * (this->wl/2)) );
        this->start_idx = size_t ( double(this->wl/2) * this->fcut_lower);
        // in general we don't want the DC part - so if no parameters are given explictly we skip the DC part
        // for a full inverse FFT we require all in and all out - that includes the DC part unforunately;
        // make a detrend ! befor zero padding in case
        if (!this->start_idx && !this->isdcfft) start_idx = 1;
        this->stop_idx = size_t( double(this->wl/2) - ( double(this->wl/2) * this->fcut_upper)  ) + 1;
        if (stop_idx > this->wl/2) stop_idx = wl/2 ; // stop idx can be 1024 because STL like we do i = start && < stop
        this->n = this->stop_idx - this->start_idx;
        if (!nspc) outbuf.resize(n);
    }
    else {
        this->n = wl/2;
        if (!nspc) outbuf.resize(n);
        this->stop_idx = this->wl/2;
    }
    // a full fft require all
    if (this->inverse_from_forward) {
        this->start_idx = 0;
        this->isdcfft = true;
        this->stop_idx = this->wl/2;
        this->n = wl/2;
        if (!nspc) outbuf.resize(n-1); // we can not use the DC for the spectra (log/log)
    }


    return true;
}

void fftreal_vector::set_amplitude_spectra(std::vector<double> &fft_amplitude)
{
    fft_amplitude.reserve(n);
    fft_amplitude.resize(n);
    this->fft_amplitude = &fft_amplitude;
}

bool fftreal_vector::set_calibration(std::vector<std::complex<double>> &mulcal)
{
    if (mulcal.size() != this->n) return false;

    this->mulcal = &mulcal;
    this->has_calibration = true;
    return true;
}

bool fftreal_vector::set_mtx_calibration(std::vector<std::complex<double> > &divcal)
{
     if ((divcal.size() != (this->n - 1)) && this->isdcfft) return false;
     if ((divcal.size() !=  this->n ) && !this->isdcfft) return false;
     this->divcal = &divcal;
     this->has_calibration = true;
     return true;
}

void fftreal_vector::unset_calibration()
{
    this->has_calibration = false;
}

void fftreal_vector::set_amplitude_spectra_stacked(std::vector<double> &fft_amplitude_stacked)
{
    fft_amplitude_stacked.reserve(n);
    fft_amplitude_stacked.resize(n);
    this->fft_amplitude_stacked = &fft_amplitude_stacked;
}

void fftreal_vector::process(std::vector<double> &inbuf, std::vector<std::complex<double> > &outbuf, const int ctrl)
{

    if (!nspc && !this->inverse_from_forward) outbuf.resize(n);
    if (!nspc && (this->inverse_from_forward || this->isdcfft)) outbuf.resize(n - 1); // avoid DC

    if (ctrl == fftreal_ctrl::hanning) {

        iter::hanning<double>(inbuf.begin(), inbuf.end());
    }
    else if (ctrl == fftreal_ctrl::detrend) {
        iter::detrend<double>(inbuf.begin(), inbuf.end());
    }
    else if (ctrl == fftreal_ctrl::detrend_hanning) {
        iter::detrend_and_hanning<double>(inbuf.begin(), inbuf.end());
    }

    // double sided FFT, left "output", right "input"; that is still the COMPLETE spectra
    fft_object->do_fft(&this->psrc[0], &inbuf[0]);


    size_t j = 0;

    // make sure about the DC part if you have a full inverse


    if (this->has_calibration_external) {
           emit this->signal_gen_pure_theo_cal(this->psrc, this->f_sample, this->start_idx, this->stop_idx,
                                          this->dc_ampl, this->dc_phase_deg, this->mul_by, this->mul_by_f, this->invert);
    }

    else if (this->has_calibration && this->mulcal != nullptr) {
        if (this->mulcal->size() == this->n) {
            size_t wlh = this->wl/2;
            for (size_t i = this->start_idx; i < this->stop_idx; ++i) {
                auto ctmp = std::complex<double> (psrc[i], -1. * psrc[wlh + i]);
                ctmp *= this->mulcal->operator[](i);
                this->psrc[i] = ctmp.real();
                this->psrc[wlh + i] = -1.0 * ctmp.imag();
            }
        }
    }

    else if (this->has_calibration && this->divcal != nullptr) {
        if (((this->divcal->size() == (this->n -1)) && this->isdcfft ) || ((this->divcal->size() == this->n) && !this->isdcfft )) {
            size_t wlh = this->wl/2;
            size_t i = this->start_idx;
            auto c_cal = this->divcal->begin();
//            if (is_dc_ftt) {
//                this->psrc[i] = 0.0; this->psrc[wlh] = 0.0;
//            }
            if (i == 0) ++i; // all mtx function do not include the DC part
            for (; i < this->stop_idx; ++i) {
                auto ctmp = std::complex<double> (psrc[i], -1. * psrc[wlh + i]);
                // last part represents the frequency
                ctmp /= ((-1000.0) * (*c_cal++) );
                ctmp /=  ( double(i) * (this->f_sample/this->fwl));
                this->psrc[i] = ctmp.real();
                this->psrc[wlh + i] = -1.0 * ctmp.imag();
            }
        }
    }


    if (!nspc) {
        if (this->wincal != 0.0){
            j = 0;
            size_t i = this->start_idx;
            if ((this->isdcfft || this->inverse_from_forward) && (i == 0)) ++i;
            for (; i < this->stop_idx; ++i) {

                outbuf[j++] = std::complex<double>(psrc.at(i), -1. * psrc.at(wl/2 + i)) * this->wincal;
            }
        }
        if (this->wincal == 0.0){
            j = 0;
            size_t i = this->start_idx;
            if ((this->isdcfft || this->inverse_from_forward) && (i == 0)) ++i;
            for (; i < this->stop_idx; ++i) {

                outbuf[j++] = std::complex<double>(psrc.at(i), -1. * psrc.at(wl/2 + i));
            }
        }


        if (this->fft_amplitude != nullptr) {
            for (j = 0; j < outbuf.size(); ++j) {
                this->fft_amplitude->operator [](j) = std::abs(outbuf.at(j));
            }
        }

        if (this->fft_amplitude_stacked != nullptr) {
            for (j = 0; j < outbuf.size(); ++j) {
                this->fft_amplitude_stacked->operator [](j) += std::abs(outbuf.at(j));
            }
        }
    }

    // that is meaningless if you have no cal - only for test without cal
    // we require FULL data
    if (this->inverse_from_forward) {
        // src -> dest

        //        if (this->has_calibration && (this->divcal->size() == (outbuf.size()))) {
        //            for (size_t i = 0; i < outbuf.size(); ++i) {
        //                psrc[i] = outbuf[i].real();
        //                psrc[wl/2 + i] = -1. * outbuf[i].imag();
        //            }
        //        }
        //this->psrc[0] = 0;
        //        for (size_t i = 1; i < this->psrc.size()/2; ++i) {
        //            psrc[i] /= (this->freqs[i] * 1000.);
        //            psrc[wl/2 + i] /= (this->freqs[i] * 1000.);
        //        }



        fft_object->do_ifft(&this->psrc[0], &inbuf[0]);
        double length = this->wl;
        for (auto &d : inbuf) {
            d /= length;
        }
    }



    ++this->counter;

}

//bool fftreal_vector::set_inverse_buffer_after_forward(std::vector<double> &outbuf)
//{
//    outbuf.resize(this->wl);
//    this->ts_calibrated = &outbuf;
//    this->inverse_from_forward = true;

//    return true;
//}

//bool fftreal_vector::unset_inverse_buffer_after_forward()
//{
//    this->inverse_from_forward = false;
//    return true;
//}


//bool fftreal_vector::set_inverse_buffer(std::vector<std::complex<double>> &in_inv_buf, std::vector<double> &outbuf)
//{
//    if (!in_inv_buf.size()) return false;
//    outbuf.resize(in_inv_buf.size() * 2);

//    return true;
//}


std::vector<double> fftreal_vector::get_frequencies() const
{
    std::vector<double> freqs;
    if (this->f_sample != 0.0) {
        freqs.resize(this->n);
        size_t j = 0, i = 0;
        for (i = 0; i < this->wl/2; ++i) {
            if (i >= this->start_idx && i < this->stop_idx) {
                freqs[j] = ( double(i) * (this->f_sample/this->fwl) );
                ++j;
            }
        }
    }
    return freqs;
}

void fftreal_vector::set_external_calibration(const double &dc_ampl, const double &dc_phase_deg, const double &mul_by, const bool mul_by_f, const bool invert)
{
    this->dc_ampl = dc_ampl;
    this->dc_phase_deg = dc_phase_deg;
    this->mul_by = mul_by;
    this->mul_by_f = mul_by_f;
    this->invert = invert;      // typical for mfs coil *cbeg = 1.0/ (*cbeg * 1000.0 * *f++) in order to get mV and unset normalization by f
    this->has_calibration_external = true;
}

//std::vector<double> fftreal_vector::gen_frequencies()
//{
//    if (this->f_sample != 0.0) {

//        this->freqs.resize(this->n);
//        size_t j = 0, i = 0;
//        for (i = 0; i < this->wl/2; ++i) {
//            if (i >= this->start_idx && i < this->stop_idx) {
//                this->freqs[j] = ( double(i) * (this->f_sample/this->fwl) );
//                ++j;
//            }
//        }
//    }
//    return this->freqs;
//}

//std::vector<double> fftreal_vector::get_frequencies_for_inverse(const bool skip_dc) const
//{
//    std::vector<double> allfreqs;;
//    if (this->f_sample != 0.0) {
//        size_t i = 0;
//        if (skip_dc) {

//            allfreqs.resize(this->wl/2 -1);
//            size_t j = 0;
//            for (i = 1; i < this->wl/2; ++i) {
//                allfreqs[j++] = ( double(i) * (this->f_sample/this->fwl) );
//            }
//        }
//        else {
//            allfreqs.resize(this->wl/2);
//            for (i = 0; i < this->wl/2; ++i) {
//                allfreqs[i] = ( double(i+1) * (this->f_sample/this->fwl) );
//            }
//        }

//    }
//    return allfreqs;
//}

size_t fftreal_vector::next_power_of_two(const size_t n) const
{
    size_t target, m;
    if (n > (SIZE_MAX - 1) / 2)
        throw "Vector too large";
    target = n;
    for (m = 1; m < target; m *= 2) {
        if (SIZE_MAX / 2 < m)
            throw "Vector too large";
    }

    return m;

}

void fftreal_vector::get_settings(double &freq, bool &bwincal, bool &isdcfft, bool &nspc, bool &inverse_from_forward, double &cut_upper, double &cut_lower) const
{
    freq = this->f_sample;
    if (wincal == 0.0) bwincal = false;
    else bwincal = true;
    isdcfft = this->isdcfft;
    cut_upper = this->fcut_upper;
    cut_lower = this->fcut_lower;
    nspc = this->nspc;
    inverse_from_forward = this->inverse_from_forward;
}


double fftreal_vector::get_nyquist_freq() const
{
    return ( double(this->wl/2) * (f_sample/fwl) );
}

void fftreal_vector::set_multiply(const double &factor)
{
    this->multiply_by = factor;
}

