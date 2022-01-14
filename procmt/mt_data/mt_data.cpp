#include "mt_data.h"


void acsp::init()
{
    this->ac.resize( size_t(pmt::ac_spectra_names.size()));
    this->ac_sel.resize(size_t(pmt::ac_spectra_names.size()));
    this->ac_vals.resize( size_t(pmt::ac_spectra_names.size()));
    this->ac_ampl_vals.resize( size_t(pmt::ac_spectra_names.size()));

    this->ssp.resize(size_t(pmt::channel_types.size()));
    this->ssp_sel.resize(size_t(pmt::channel_types.size()));
    this->ssp_vals.resize(size_t(pmt::channel_types.size()));
    this->ssp_vals_2.resize(size_t(pmt::channel_types.size()));
    this->ssp_vals_imag.resize(size_t(pmt::channel_types.size()));
    this->ssp_vals_imag_2.resize(size_t(pmt::channel_types.size()));

    this->ssp_orig.resize(size_t(pmt::channel_types.size()));

    this->coh.resize( size_t(pmt::ac_spectra_names.size()), DBL_MAX);
    this->f = DBL_MAX;

}

void acsp::set_ac_sel_true()
{
    for (auto &vsel : this->ac_sel) {
        std::fill(vsel.begin(), vsel.end(), true);
    }
}

void acsp::set_ssp_sel_true()
{
    for (auto &vsel : this->ssp_sel) {
        std::fill(vsel.begin(), vsel.end(), true);
    }
}

acsp::~acsp()
{

    this->clear(); // the auto delete is maybe faster
    //qDebug() << "acsp deleted";
}

void acsp::resize()
{
    size_t i = 0, k = 0;
    size_t j;

    for (auto &cplxv1 : this->ssp) {

        if (cplxv1.size()) {
            k = 0;
            for (auto &cplxv2 : this->ssp) {
                if (cplxv2.size() && (k >= i)) {
                    j = pmt::spc_to_auto_cross_spc<size_t>(i, k);
                    if (j < pmt::spce::spce_size) {
                        // if was not init with size size; we also init statmap - because adding was above
                        // we only want to resize the first time - that is the default
                        // and we do not have the process of adding calculating and adding
                        if (this->ac[j].size() != cplxv1.size()) {
                            this->ac[j].resize(cplxv1.size());
                            // what happens if I have already set a selection vector -  first elements stay as they are!
                            this->ac_sel[j].resize( cplxv1.size(), true);
                            this->ac_vals[j].reserve(g_stat::statmap_size);
                            for (size_t sj = 0; sj < g_stat::statmap_size; ++sj) this->ac_vals[j][sj] = 0.0;
                            this->ac_ampl_vals[j].reserve(g_stat::statmap_size);
                            for (size_t sj = 0; sj < g_stat::statmap_size; ++sj) this->ac_ampl_vals[j][sj] = 0.0;
                        }
                    }
                }
                ++k;
            }
        }
        ++i;

    }

}

void acsp::resize_auto_spc_only()
{
    size_t i = 0;
    size_t j;
    for (auto &cplxv : this->ssp) {
        if (cplxv.size()) {
            // index of auto- and cross spectra
            j = pmt::spc_to_auto_cross_spc<size_t>(i, i);
            if (j < pmt::spce_size) {
                // if was not init with size size; we also init statmap - because adding was above
                // we only want to resize the first time - that is the default
                // and we do not have the process of adding calculating and adding
                if (this->ac[j].size() != cplxv.size()) {
                    this->ac[j].resize(cplxv.size());
                    // what happens if I have already set a selection vector -  first elements stay as they are!
                    this->ac_sel[j].resize( cplxv.size(), true);
                    //this->ac_sel[j] = this->ssp_sel[j];
                    this->ac_vals[j].reserve(g_stat::statmap_size);
                    for (size_t sj = 0; sj < g_stat::statmap_size; ++sj) this->ac_vals[j][sj] = 0.0;
                    this->ac_ampl_vals[j].reserve(g_stat::statmap_size);
                    for (size_t sj = 0; sj < g_stat::statmap_size; ++sj) this->ac_ampl_vals[j][sj] = 0.0;
                }
            }
        }
        ++i;
    }
}

void acsp::clear()
{
    // for (auto &d : this->ac) d.clear();
    this->ac.clear();
    // for (auto &d : this->ac_sel) d.clear();
    this->ac_sel.clear();
    // for (auto &d : this->ac_vals) d.clear();
    this->ac_vals.clear();
    // for (auto &d : this->ac_ampl_vals) d.clear();
    this->ac_ampl_vals.clear();
    // for (auto &d : this->ssp) d.clear();
    this->ssp.clear();
    // for (auto &d : this->ssp_sel) d.clear();
    this->ssp_sel.clear();
    // for (auto &d : this->ssp_vals) d.clear();
    this->ssp_vals.clear();
    // for (auto &d : this->ssp_vals_2) d.clear();
    this->ssp_vals_2.clear();
    // for (auto &d : this->ssp_vals_imag) d.clear();
    this->ssp_vals_imag.clear();
    // for (auto &d : this->ssp_vals_imag_2) d.clear();
    this->ssp_vals_imag_2.clear();


}

void acsp::normalize_ssp(const size_t spce_numerator, const size_t spce_denominator)
{
    if (this->ssp[spce_numerator].size() != this->ssp[spce_denominator].size()) return;

    auto nb = this->ssp[spce_numerator].begin();
    auto db = this->ssp[spce_denominator].begin();

    while (nb != this->ssp[spce_numerator].end()) {
        *nb /= *db;
        ++nb; ++db;
    }
}

void acsp::normalize_ssp_all(const size_t spce_denominator)
{
    size_t i = 0;
    for (i = 0; i < this->ssp.size(); ++i) {
        size_t spce_numerator = i;
        if (i != spce_denominator) {
            this->normalize_ssp(spce_numerator, spce_denominator);
        }

    }
}

void acsp::reset_cross_remote_residuals()
{
    if (this->ac[pmt::hxrhx].size()) {
        for (auto &c : this->ac[pmt::hxrhx]) c = std::complex<double>(c.real(), 0.0);
    }
    if (this->ac[pmt::hyrhy].size()) {
        for (auto &c : this->ac[pmt::hyrhy]) c = std::complex<double>(c.real(), 0.0);
    }
}

void acsp::reset_auto_residuals()
{
    if (this->ac[pmt::exex].size()) {
        for (auto &c : this->ac[pmt::exex]) c = std::complex<double>(c.real(), 0.0);
    }
    if (this->ac[pmt::eyey].size()) {
        for (auto &c : this->ac[pmt::eyey]) c = std::complex<double>(c.real(), 0.0);
    }
    if (this->ac[pmt::hxhx].size()) {
        for (auto &c : this->ac[pmt::hxhx]) c = std::complex<double>(c.real(), 0.0);
    }
    if (this->ac[pmt::hyhy].size()) {
        for (auto &c : this->ac[pmt::hyhy]) c = std::complex<double>(c.real(), 0.0);
    }
    if (this->ac[pmt::hzhz].size()) {
        for (auto &c : this->ac[pmt::hzhz]) c = std::complex<double>(c.real(), 0.0);
    }
}

void acsp::cross_coherency_raw_spectra()
{
    size_t i = 0;
    size_t j, k;

    for (i = 0; i < this->ssp.size() - 1; ++i) {
        for (j = i+1; j < this->ssp.size(); ++j) {
            if (this->ssp[i].size() && this->ssp[j].size()) {
                // index of auto- and cross spectra
                k = pmt::spc_to_auto_cross_spc<size_t>(i, j);

                auto xb = this->ssp[i].cbegin();
                auto yb = this->ssp[j].cbegin();
                auto xe = this->ssp[i].cend();
                double mean_nominator = 0;
                double mean_x = 0;
                double mean_y = 0;
                int count = 0;

                while (xb != xe) {

                    mean_nominator += std::abs(*xb * *yb);
                    mean_x += real(*xb * conj(*xb));
                    mean_y += real(*yb * conj(*yb));
                    ++xb;
                    ++yb;
                    ++count;
                }
                mean_nominator /= double(count);
                mean_nominator = pow(mean_nominator, 2.0);
                mean_x /= double(count);
                mean_y /= double(count);

                if (k >= coh.size()) {
                    qDebug() << "nope";
                }
                coh[k] = mean_nominator / (mean_x * mean_y);
                coh[k] = sqrt(coh[k]);
            }
        }
    }
}

void acsp::set_ssp_chan(const size_t ch_type, const std::vector<std::complex<double> > spcx)
{
    if (this->ssp[ch_type].size() == spcx.size()) {
        this->ssp[ch_type] = spcx;

    }
}

void acsp::backup_e()
{
    this->ssp_orig[pmt::ex] = this->ssp[pmt::ex];
    this->ssp_orig[pmt::ey] = this->ssp[pmt::ey];
}


acsp::acsp()
{
    this->init();
}

acsp::acsp(const acsp &rhs)
{
    this->operator =(rhs);
}

acsp::acsp(all_spectra_collector<std::complex<double> > &all_channel_collector, const double &f, const int ac1_ssp2_acssp3)
{
    this->add(all_channel_collector, f, ac1_ssp2_acssp3);
}



void acsp::add(all_spectra_collector<std::complex<double>> &all_channel_collector, const double &f,  const int ac1_ssp2_acssp3)
{
    if (f == DBL_MAX) return;
    if (this->f == DBL_MAX) {

        this->init();  // was not initialized yet
        this->f = f;
    }
    else if (this->f != f) {
        return;
    }

    // prepare all possible input ExEx, ExEy ... we need all, otherwise we can'T iterate
    // first check the existing buckets ex .. rhx sizes
    size_t i = 0, mysize;

    if (ac1_ssp2_acssp3 == 1) {
        // for each exex, exey .. hz ...hzrhz
        for (auto &aspcdat : all_channel_collector.data ) {
            if (aspcdat.data.size()) {                                              // ex has size ... rhz has size ?
                if (!aspcdat.data[f].empty()) {                                     // does ex has 512 hz ... yes! this is extra security
                    size_t oldsize = this->ac[i].size();
                    mysize = this->ac[i].size() + aspcdat.data[f].size();
                    this->ac[i].resize(mysize);
                    auto iter = this->ac[i].begin() + oldsize;
                    std::copy(aspcdat.data[f].begin(), aspcdat.data[f].end(), iter);
                    aspcdat.data[f].clear();
                    this->ac_sel[i].resize(mysize, true);
                    if (this->ac_vals.size() != g_stat::statmap_size) {
                        this->ac_vals[i].reserve(g_stat::statmap_size);
                        for (size_t j = 0; j < g_stat::statmap_size; ++j) this->ac_vals[i][j] = 0.0;
                    }
                }

            }
            ++i;  // local exex, exey ... rhz
        }
    }

    // that can be single spectra ex, ey, rhz
    else if (ac1_ssp2_acssp3 == 2) {
        for (auto &aspcdat : all_channel_collector.data ) {
            if (aspcdat.data.size()) {                                              // ex has size ... rhz has size ?
                if (!aspcdat.data[f].empty()) {                                     // does ex has 512 hz ... yes! this is extra security
                    size_t oldsize = this->ac[i].size();
                    mysize = this->ssp[i].size() + aspcdat.data[f].size();
                    this->ssp[i].resize(mysize);
                    auto iter = this->ssp[i].begin() + oldsize;
                    std::copy(aspcdat.data[f].begin(), aspcdat.data[f].end(), iter);
                    aspcdat.data[f].clear();
                    this->ssp_sel[i].resize(mysize, true);
                    if (this->ssp_vals.size() != g_stat::statmap_size) {
                        this->ssp_vals[i].reserve(g_stat::statmap_size);
                        for (size_t j = 0; j < g_stat::statmap_size; ++j) this->ssp_vals[i][j] = 0.0;
                    }
                    if (this->ssp_vals_2.size() != g_stat::statmap_size) {
                        this->ssp_vals_2[i].reserve(g_stat::statmap_size);
                        for (size_t j = 0; j < g_stat::statmap_size; ++j) this->ssp_vals_2[i][j] = 0.0;
                    }
                    if (this->ssp_vals_imag.size() != g_stat::statmap_size) {
                        this->ssp_vals_imag[i].reserve(g_stat::statmap_size);
                        for (size_t j = 0; j < g_stat::statmap_size; ++j) this->ssp_vals_imag[i][j] = 0.0;
                    }
                    if (this->ssp_vals_imag_2.size() != g_stat::statmap_size) {
                        this->ssp_vals_imag_2[i].reserve(g_stat::statmap_size);
                        for (size_t j = 0; j < g_stat::statmap_size; ++j) this->ssp_vals_imag_2[i][j] = 0.0;
                    }
                }

            }
            ++i;  // local exex, exey ... rhz
        }
    }



}


acsp &acsp::operator =(const acsp &rhs)
{
    if(&rhs == this) return *this;

    this->clear();
    this->init();

    this->f = rhs.f;
    for (size_t i = 0; i < this->ac.size(); ++i) {
        this->ac[i] = rhs.ac.at(i);
    }
    for (size_t i = 0; i < this->ac_sel.size(); ++i) {
        this->ac_sel[i] = rhs.ac_sel.at(i);
    }
    for (size_t i = 0; i < this->ac_vals.size(); ++i) {
        this->ac_vals[i] = rhs.ac_vals.at(i);
    }

    for (size_t i = 0; i < this->ac_ampl_vals.size(); ++i) {
        this->ac_ampl_vals[i] = rhs.ac_ampl_vals.at(i);
    }

    for (size_t i = 0; i < this->ssp.size(); ++i) {
        this->ssp[i] = rhs.ssp.at(i);
    }
    for (size_t i = 0; i < this->ssp_sel.size(); ++i) {
        this->ssp_sel[i] = rhs.ssp_sel.at(i);
    }
    for (size_t i = 0; i < this->ssp_vals.size(); ++i) {
        this->ssp_vals[i] = rhs.ssp_vals.at(i);
    }
    for (size_t i = 0; i < this->ssp_vals_2.size(); ++i) {
        this->ssp_vals_2[i] = rhs.ssp_vals_2.at(i);
    }
    for (size_t i = 0; i < this->ssp_vals_imag.size(); ++i) {
        this->ssp_vals_imag[i] = rhs.ssp_vals_imag.at(i);
    }
    for (size_t i = 0; i < this->ssp_vals_imag_2.size(); ++i) {
        this->ssp_vals_imag_2[i] = rhs.ssp_vals_imag_2.at(i);
    }



    return *this;
}


void acsp::calc_auto_spc()
{

    if (!this->ssp.size()) return;
    size_t i = 0;
    size_t j, k;
    for (auto &cplxv : this->ssp) {
        if (cplxv.size()) {
            // index of auto- and cross spectra
            j = pmt::spc_to_auto_cross_spc<size_t>(i, i);
            if (j < pmt::spce_size) {
                // if was not init with size size; we also init statmap - because adding was above
                // we only want to resize the first time - that is the default
                // and we do not have the process of adding calculating and adding
                if (this->ac[j].size() != cplxv.size()) {
                    this->ac[j].resize(cplxv.size());
                    // what happens if I have already set a selection vector -  first elements stay as they are!
                    this->ac_sel[j].resize( cplxv.size(), true);
                    this->ac_sel[j] = this->ssp_sel[i];
                    this->ac_vals[j].reserve(g_stat::statmap_size);
                    for (size_t sj = 0; sj < g_stat::statmap_size; ++sj) this->ac_vals[j][sj] = 0.0;
                    this->ac_ampl_vals[j].reserve(g_stat::statmap_size);
                    for (size_t sj = 0; sj < g_stat::statmap_size; ++sj) this->ac_ampl_vals[j][sj] = 0.0;
                }

                k = 0;

                for (const auto &cplx : cplxv) {
                    // if sel ... !!!!!!!!!!!!!!!!!!!!!!; take real because there is "math noise" so imag may appear as 10exp-30 or so
                    if (this->ac_sel[j][k]) this->ac[j][k++] = std::real(cplx * std::conj(cplx));

                }
            }
        }
        ++i;
    }
}

void acsp::calc_ac_spc()
{


    if (!this->ssp.size()) return;
    size_t i = 0, j = 0;
    size_t k;

    auto lhs_spc = this->ssp.cbegin();
    auto rhs_spc = this->ssp.cbegin();

    // for all input spectra
    while (lhs_spc != this->ssp.cend()) {

        // have data
        if (lhs_spc->size()) {
            // take auto (lhs == rhs) and cross (lhs != rhs)
            while (rhs_spc != this->ssp.cend()) {
                if (rhs_spc->size()) {
                    // find the index in auto-cross
                    k = pmt::spc_to_auto_cross_spc<size_t>(i, j);

                    if (k < pmt::spce_size) {

                        qDebug() << intfreq2string_unit(this->f) << ": " << pmt::channel_types.at(i) << pmt::channel_types.at(j) << "-> " << pmt::ac_spectra_names.at(k);

                        if (this->ac[k].size() != lhs_spc->size()) {
                            this->ac[k].resize(lhs_spc->size());
                            // what happens if I have already set a selection vector -  first elements stay as they are!
                            this->ac_sel[k].resize( lhs_spc->size(), true);
                            this->ac_sel[k] = this->ssp_sel[i];
                            this->ac_vals[k].reserve(g_stat::statmap_size);
                            for (size_t sj = 0; sj < g_stat::statmap_size; ++sj) this->ac_vals[k][sj] = 0.0;
                            this->ac_ampl_vals[k].reserve(g_stat::statmap_size);
                            for (size_t sj = 0; sj < g_stat::statmap_size; ++sj) this->ac_ampl_vals[k][sj] = 0.0;
                        }

                        auto rhs_iter = rhs_spc->cbegin();
                        auto lhs_iter = lhs_spc->cbegin();
                        for (size_t p = 0; p < lhs_spc->size(); ++p) {
                            // if sel ... !!!!!!!!!!!!!!!!!!!!!!

                            if (this->ac_sel[k][p]) {
                                // avaoid calculation noise so imag may appear as 10exp-30 or so
                                if (i == j) this->ac[k][p] = std::real(*lhs_iter * std::conj(*rhs_iter));
                                else this->ac[k][p] = (*lhs_iter * std::conj(*rhs_iter));
                            }
                            ++lhs_iter;
                            ++rhs_iter;
                        }

                    }
                }
                ++rhs_spc;
                ++j;
            }
        }

        ++lhs_spc;
        rhs_spc = lhs_spc; // reset to next auto spectrum
        ++i;
        j = i;
    }
}



std::vector<double> acsp::ampl_spc_if(const size_t chan_type) const
{
    std::vector<double> ampls;
    if (chan_type < pmt::chan_type_size) {
        size_t trues = size_t(std::count(this->ac_sel[chan_type].cbegin(), this->ac_sel[chan_type].cend(), true));

        ampls.resize(trues);
        size_t i = 0;
        auto isit = this->ac_sel[chan_type].cbegin();
        for (const auto &cplx : this->ac[chan_type]) {
            if(*isit++)ampls[i++] = std::sqrt(std::abs(cplx));
        }
    }
    return ampls;

}

std::vector<double> acsp::ssp_spc_ampls_if(const size_t chan_type) const
{
    std::vector<double> ampls;
    if (chan_type < pmt::chan_type_size) {
        size_t trues = size_t(std::count(this->ssp_sel[chan_type].cbegin(), this->ssp_sel[chan_type].cend(), true));

        ampls.resize(trues);
        size_t i = 0;
        auto isit = this->ssp_sel[chan_type].cbegin();
        for (const auto &cplx : this->ssp[chan_type]) {
            if(*isit++)ampls[i++] = std::sqrt(std::abs(cplx));
        }
    }
    return ampls;
}

std::vector<double> acsp::ssp_spc_phz_if(const size_t chan_type) const
{
    std::vector<double> phz;
    if (chan_type < pmt::chan_type_size) {
        size_t trues = size_t(std::count(this->ssp_sel[chan_type].cbegin(), this->ssp_sel[chan_type].cend(), true));

        phz.resize(trues);
        size_t i = 0;
        auto isit = this->ssp_sel[chan_type].cbegin();
        for (const auto &cplx : this->ssp[chan_type]) {
            if(*isit++)phz[i++] = std::arg(cplx);
        }
    }
    return phz;
}

std::vector<double> acsp::ssp_spc_if(const bool real_false_imag_true, const int first_1_second_2_all_3, const size_t chan_type) const
{
    std::vector<double> vals;
    if (chan_type < pmt::chan_type_size) {
        if (!this->ssp[chan_type].size()) return vals;

        auto isit = this->ssp_sel[chan_type].cbegin();
        auto wasit = this->ssp_sel[chan_type].cbegin();

        auto starts = this->ssp[chan_type].cbegin();
        auto stops= this->ssp[chan_type].cbegin();

        // first half
        if (first_1_second_2_all_3 == 1) {
            std::advance(wasit, (std::distance(this->ssp_sel[chan_type].cbegin(), this->ssp_sel[chan_type].cend()) / 2));
            std::advance(stops, (std::distance(this->ssp[chan_type].begin(), this->ssp[chan_type].end()) / 2));
        }

        // second half
        if (first_1_second_2_all_3 == 2) {
            std::advance(isit, (std::distance(this->ssp_sel[chan_type].cbegin(), this->ssp_sel[chan_type].cend()) / 2));
            std::advance(starts, (std::distance(this->ssp[chan_type].begin(), this->ssp[chan_type].end()) / 2));
        }

        if ((first_1_second_2_all_3 == 3) || (first_1_second_2_all_3 == 2)) {
            wasit = this->ssp_sel[chan_type].cend();
            stops= this->ssp[chan_type].cend();
        }


        size_t trues = size_t(std::count(isit, wasit, true));
        vals.resize(trues);

        size_t i = 0;

        if (!real_false_imag_true) {
            do {
                if(*isit++)vals[i++] = std::real(*starts++);
            } while(starts != stops);
        }
        else  {
            do {
                if(*isit++)vals[i++] = std::imag(*starts++);
            } while(starts != stops);
        }
    }

    return vals;

}


size_t acsp::size() const
{
    for (const auto &d : this->ac) {
        if (d.size()) return d.size();
    }

    return 0;
}

int acsp::has_which_spectra() const
{
    // int ac1_ssp2_acssp3 = 0;
    size_t acspsz = this->size();
    size_t sspsz = this->ssp_size();
    if (!acspsz && !sspsz) return 0;        // has no spectra
    if (acspsz && sspsz) return 3;          // has both spectra
    if (acspsz && !sspsz) return 1;         // has ac (auto- cross) spectra only
    if (!acspsz && sspsz) return 2;         // has single complex spectra only

    return 0;
}


size_t acsp::ssp_size() const
{
    for (const auto &d : this->ssp) {
        if (d.size()) return d.size();
    }

    return 0;
}




size_t f_index_c(const std::vector<mt_data_t<std::complex<double> > > &dat, const double &f) {

    size_t sz = 0;
    for (const auto &xd : dat) {
        if (xd.f == f) return sz;
        ++sz;
    }

    return SIZE_MAX;
}

size_t f_size_c(const std::vector<mt_data_t<std::complex<double> > > &dat, const double &f) {

    for (const auto &xd : dat) {
        if (xd.f == f) {
            return xd.size();
        }
    }
    return 0;
}



std::vector<double> get_freqs_acsp(const std::vector<acsp> &ac)
{
    std::vector<double> freqs;
    for (const auto &acx : ac) {
        freqs.push_back(acx.f);
    }

    return freqs;
}

size_t f_size_a(const std::vector<acsp> &ac, const double &f, size_t &f_idx)
{
    f_idx = 0;
    for (const auto &spc : ac) {
        if (spc.f == f) {
            for (const auto &v : spc.ac) {
                if (v.size()) return v.size();
            }
        }
        ++f_idx;
    }
    f_idx = SIZE_MAX;
    return 0;

}

size_t f_size_s(const std::vector<acsp> &ac, const double &f, size_t &f_idx)
{
    f_idx = 0;
    for (const auto &spc : ac) {
        if (spc.f == f) {
            for (const auto &v : spc.ssp) {
                if (v.size()) return v.size();
            }
        }
        ++f_idx;
    }
    f_idx = SIZE_MAX;
    return 0;
}

size_t f_index_acsp(const std::vector<acsp> &ac, const double &f)
{
    size_t sz = 0;
    for (const auto &spc : ac) {
        if (spc.f == f) return sz;
        ++sz;
    }
    return SIZE_MAX;
}


std::vector<double> get_freqs_z(const std::vector<mt_data_t<std::complex<double> > > &dat)
{
    std::vector<double> freqs;
    for (const auto &d : dat) {
        freqs.push_back(d.f);
    }

    return freqs;
}

QDataStream &operator >>(QDataStream &is, acsp &dat)
{
    dat.init();
    is >> dat.f;

    for (auto &v : dat.ac) {
        quint64 sz;
        is >> sz;
        if (sz) {
            v.resize(sz);
            double zr, zi;
            for (auto &val : v) {
                is >> zr;
                is >> zi;
                val = std::complex<double>(zr, zi);
            }
        }
    }

    // the selection is stored because we had other selection software
    for (auto &v : dat.ac_sel) {
        quint64 sz;
        is >> sz;
        if (sz) {
            QBitArray qbia(sz);
            v.resize(sz);
            is >> qbia;
            for (quint64 i = 0; i < sz; ++i) v[i] = qbia.at(i);
        }
    }

    // the statmaps must be re-calucalted because the data may be extended

    for (auto &v : dat.ssp) {
        quint64 sz;
        is >> sz;
        if (sz) {
            v.resize(sz);
            double zr, zi;
            for (auto &val : v) {
                is >> zr;
                is >> zi;
                val = std::complex<double>(zr, zi);
            }
        }
    }

    // the selection is stored because we had other selection software
    for (auto &v : dat.ssp_sel) {
        quint64 sz;
        is >> sz;
        if (sz) {
            QBitArray qbia(sz);
            v.resize(sz);
            is >> qbia;
            for (quint64 i = 0; i < sz; ++i) v[i] = qbia.at(i);
        }
    }

    // the statmaps must be re-calucalted because the data may be extended



    return is;
}

QDataStream &operator <<(QDataStream &os, const acsp &dat)
{
    os << dat.f;

    for (auto &v : dat.ac) {
        os << quint64(v.size());
        if (v.size()) {
            for (auto &val : v) {
                os << real(val);
                os << imag(val);
            }
        }
    }

    // the selection is stored because we had other selection software
    for (auto &v : dat.ac_sel) {
        os << quint64(v.size());
        if (v.size()) {
            QBitArray qbia(int(v.size()), true);
            int i = 0;
            for (auto val : v) qbia.setBit(i++, val);
            os << qbia;
        }
    }

    // the statmaps must be re-calucalted because the data may be ex[ten]xx»ded

    for (auto &v : dat.ssp) {
        os << quint64(v.size());
        if (v.size()) {
            for (auto &val : v) {
                os << real(val);
                os << imag(val);
            }
        }
    }

    // the selection is stored because we had other selection software
    for (auto &v : dat.ssp_sel) {
        os << quint64(v.size());
        if (v.size()) {
            QBitArray qbia(int(v.size()), true);
            int i = 0;
            for (auto val : v) qbia.setBit(i++, val);
            os << qbia;
        }
    }

    // the statmaps must be re-calucalted because the data may be extended


    return os;
}

size_t read_acsp_vector(const QFileInfo &qfi, std::vector<acsp> &data)
{
    if (!qfi.exists()) return 0;
    QFile file(qfi.absoluteFilePath());
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream qds(&file);
        data.clear();
        data.reserve(100);
        while (!qds.atEnd()) {
            data.emplace_back();
            qds >> data.back();
        }

        data.shrink_to_fit();
        file.close();
        return data.size();


    }

    return 0;

}


size_t write_acsp_vector(const QFileInfo &qfi, const std::vector<acsp> &data)
{

    QFile file(qfi.absoluteFilePath());
    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
        QDataStream qds(&file);
        for (auto &dat : data) {
            qds << dat;
        }
        file.close();
        return data.size();
    }

    return 0;

}




size_t write_ssp_vector_ascii(const QFileInfo &qfidirname, const std::vector<acsp> &data)
{
    size_t i = 0;
    for (auto &s : data) {
        QString fstr = QString::number(s.f,'g', 8);
        QString xfilepath = qfidirname.absoluteFilePath() + "/";

        size_t j = 0;
        for (auto &v : s.ssp) {
            if (v.size()) {
                QString chstr = pmt::channel_types.at(j);
                QString filepath (xfilepath + chstr + "_" + fstr + ".dat");
                QFile file(filepath);
                if (file.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
                    QTextStream qds(&file);

                    qds.setRealNumberNotation(QTextStream::ScientificNotation);
                    for (auto &spcnums : v) {
                        qds << std::real(spcnums) << " " << std::imag(spcnums) << Qt::endl;
                    }
                    file.close();
                }
            }
            ++j;
        }

        ++i;
    }

    return i;
}
