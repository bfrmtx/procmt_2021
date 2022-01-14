#include "acsp_prz.h"


acsp_prz::acsp_prz(std::shared_ptr<all_spectra_collector<std::complex<double> > > &all_acspc_collector,
                   QObject *parent) :  QObject (parent), prc_com(false)
{
    if (all_acspc_collector == nullptr) return;
    if (all_acspc_collector->data.size() != size_t(pmt::ac_spectra_names.size())) return;
    this->all_acspc_collector = all_acspc_collector;


}

void acsp_prz::set_data_flush_out_atsfile_collectors(std::vector<std::shared_ptr<atsfile_collector> > &out_atsfile_collectors)
{

    this->out_atsfile_collectors = out_atsfile_collectors;
    this->in.resize(pmt::chan_type::chan_type_size);

    // is that a "copy"?
    std::sort(this->out_atsfile_collectors.begin(), this->out_atsfile_collectors.end(), comp_atsfile_collector_by_channel);

    //already done!
//    // the queue is finished ? here - I need vectors now
//    for (auto &v : this->out_atsfile_collectors) {
//        v->spc.to_vector_vector();
//    }

    this->data.resize(pmt::spce_size);
}

acsp_prz::~acsp_prz()
{
    if (this->out_atsfile_collectors.size()) {
        size_t i =  this->out_atsfile_collectors.size();
        for (size_t j = 0; j < i; ++j) {
            this->out_atsfile_collectors[j].reset();
        }

    }
    this->all_acspc_collector.reset();
}

void acsp_prz::acsp(const size_t le_max_enum_spectra)
{
    size_t i, j, k;

    std::vector<std::future<void>> futures;

    // avoid reallocation during threads
    if ((this->parzen_vectors_sptr != nullptr)  && !this->select_from_target_freqs && this->target_freqs.size()) {
        for (i = 0; i < this->out_atsfile_collectors.size(); ++i) {
            for (j = i; j < this->out_atsfile_collectors.size(); ++j) {
                k = pmt::spc_to_auto_cross_spc<size_t> (this->out_atsfile_collectors[i]->ch_type(), this->out_atsfile_collectors[j]->ch_type());
                if ((k < pmt::spce_size) && ( k <= le_max_enum_spectra )) {
                    qDebug() << this->out_atsfile_collectors.at(i)->spc.chan_type << i << "i," << this->out_atsfile_collectors.at(j)->spc.chan_type << j << "j" << pmt::ac_spectra_names.at(k) << k;
                    //                }
                    //                if (k == pmt::spce_size) {
                    //                    qFatal("acsp_prz::acsp ac spectra mix up!");
                    //                }
                    this->all_acspc_collector->data[k].reserve_mem_freqs(this->target_freqs);
                    this->all_acspc_collector->data[k].spce = k;
                }
            }
        }
    }

    // for each spectra
    for (i = 0; i < this->out_atsfile_collectors.size(); ++i) {
        for (j = i; j < this->out_atsfile_collectors.size(); ++j) {
            k = pmt::spc_to_auto_cross_spc<size_t> (this->out_atsfile_collectors[i]->ch_type(), this->out_atsfile_collectors[j]->ch_type());
            // start only if we get a valid spectra
            if (( k < pmt::spce_size)  && ( k <= le_max_enum_spectra )) {
                if (this->take_prz_lines_csamt == 1) {
                    futures.emplace_back(std::async(&acsp_prz::fold_prz, this, i, j, k));
                }
                else if (this->take_prz_lines_csamt == 2) {
                    futures.emplace_back(std::async(&acsp_prz::fold_lines_tilt, this, i, j, k));

                }
            }
        }
    }


    for(auto &f : futures) {
        f.get();
    }

    // can I clear here ?
    this->in.clear();
    this->data.clear();
    this->out_atsfile_collectors.clear();


}

bool acsp_prz::set_frequencies_and_target_indices(const std::vector<double> &fft, const std::vector<double> &ft_target, const std::vector<size_t> &selindices, const std::vector<size_t> &selindices_sizes)
{
    this->take_prz_lines_csamt = 0;
    if (fft.size() && ft_target.size() && selindices.size() && selindices_sizes.size()) {

        if (selindices_sizes.size() != ft_target.size()) {
            std::cerr << "acsp_lines::set_frequencies_and_target_indices selected indices differ from taget frequencies size" << std::endl;
            return false;
        }

        this->target_freqs = ft_target;
        this->freqs = fft;
        this->indices = selindices;
        this->selindices_sizes = selindices_sizes;
        this->take_prz_lines_csamt = 2;
        return true;

    }

    return false;
}



bool acsp_prz::set_parzen_vectors(std::shared_ptr<std::vector<std::vector<double> > > parzen_vectors_sptr, const std::vector<double> &f, const std::vector<double> &ft)
{
    this->take_prz_lines_csamt = 0;
    this->parzen_vectors_sptr = parzen_vectors_sptr;
    this->target_freqs = ft;
    this->freqs = f;

    if (this->target_freqs.size() > this->freqs.size())  return false;

    // want all data from FFT - and no selected target frequencies
    if (this->target_freqs.size() == this->freqs.size()) {
        if (std::equal(this->freqs.begin(), this->freqs.end(), this->target_freqs.begin())) {
            this->target_freqs.clear();
            qDebug() << "can we have parzen here - I don't know??????????????????????????????????????????????";
            this->take_prz_lines_csamt = 1;
            return true;
        }
        else {
            return false;
        }
    }

    // if parzening is active we do not need any indices
    if ((this->parzen_vectors_sptr != nullptr) && this->target_freqs.size()) {
        this->freqs.clear();
        this->take_prz_lines_csamt = 1;
        return true;
    }

    // we have a list of frequencies with no parzen


    std::vector<double> lc_targets; // my local frequencies get by comparison below
    vector_2_indices<double>(this->freqs, this->target_freqs, lc_targets, this->indices);

    if (std::equal(this->target_freqs.begin(), this->target_freqs.end(), lc_targets.begin())) {
        this->take_prz_lines_csamt = 1;
        return true;

    }
    return false;





}

void acsp_prz::fold_prz(const size_t lhs, const size_t rhs, const size_t ac_pos)
{
    // calc the auto cross value

    auto lspc = this->out_atsfile_collectors.at(lhs)->spc.vdata.cbegin();  // here we have to example 377 vectors of 256
    auto rspc = this->out_atsfile_collectors.at(rhs)->spc.vdata.cbegin();

    // take first with size of 256
    std::vector<std::complex<double>> result(this->out_atsfile_collectors.operator[](lhs)->spc.vdata.at(0).size());

    while (lspc != this->out_atsfile_collectors.at(lhs)->spc.vdata.cend()) {

        auto result_iter = result.begin();
        auto lspc_v = lspc->cbegin();          // point to the begin of fist 377 which should have size 256 then
        auto rspc_v = rspc->cbegin();
        while (lspc_v != lspc->cend()) {

            *result_iter = *lspc_v * std::conj(*rspc_v);
            ++result_iter;
            ++lspc_v;
            ++rspc_v;
        }

        if ((this->parzen_vectors_sptr != nullptr)  &&  this->target_freqs.size()) {
            std::vector<std::complex<double>> lc_cplx_buf(this->target_freqs.size());

            for (size_t i = 0; i < this->target_freqs.size(); ++i) {
                lc_cplx_buf[i] = (std::inner_product(this->parzen_vectors_sptr->at(i).begin(), this->parzen_vectors_sptr->at(i).end(), result.begin(), std::complex<double>(0.0)));
            }

            this->all_acspc_collector->data[ac_pos].append(lc_cplx_buf, this->target_freqs);
        }
        else if (this->target_freqs.size() && this->indices.size()) {

            size_t j = 0, k = 0;
            std::vector<std::complex<double>> lc_cplx_buf(this->indices.size());
            for (size_t i = 0; i < result.size(); ++i) {
                if (i == this->indices[j]) {
                    lc_cplx_buf[k++] = result[i];
                    ++j;
                }
            }
            this->all_acspc_collector->data[ac_pos].append(lc_cplx_buf, this->target_freqs);

        }
        else if (this->freqs.size()) {
            // complete FFT
            this->all_acspc_collector->data[ac_pos].append(result, this->freqs);
        }

        ++lspc; // next of the 377 vectors
        ++rspc;
    }


    // parzen

    qDebug() << "all auto cross parzen stored" << pmt::ac_spectra_names.at(ac_pos) << ac_pos;

    // store
}
void acsp_prz::fold_lines_tilt(const size_t lhs, const size_t rhs, const size_t ac_pos)
{
    // calc the auto cross value
    if ((this->target_freqs.size() == this->selindices_sizes.size()) && this->indices.size()) {


        auto lspc = this->out_atsfile_collectors.at(lhs)->spc.vdata.cbegin();  // here we have to example 377 vectors of 256
        auto rspc = this->out_atsfile_collectors.at(rhs)->spc.vdata.cbegin();

        // take first with size of 256
        std::vector<std::complex<double>> result(this->out_atsfile_collectors.operator[](lhs)->spc.vdata.at(0).size());

        while (lspc != this->out_atsfile_collectors.at(lhs)->spc.vdata.cend()) {                    //  0 ..1 ..n-1 of 377
            std::vector<double> tilts;
            auto lspc_v = lspc->cbegin();          // point to the begin of fist 377 which should have size 256 then
            auto rspc_v = rspc->cbegin();
            size_t ssi = 0;
            for (auto &f : this->target_freqs) {                                                    //  0 ... n target frequencies

                auto lh = iter::vector_from_indices<std::complex<double>>(lspc_v, this->selindices_sizes, this->indices, ssi);
                auto rh = iter::vector_from_indices<std::complex<double>>(rspc_v, this->selindices_sizes, this->indices, ssi);

                if(this->out_atsfile_collectors.at(lhs)->is_E() || this->out_atsfile_collectors.at(rhs)->is_E() ) {
                    tilts = iter::vector_from_indices<double>(this->freqs.cbegin(), this->selindices_sizes, this->indices, ssi);
                    for (auto &ft : tilts) {
                       ft = sqrt(f/ft);
                    }
                }

                if (this->out_atsfile_collectors.at(lhs)->is_E()) {
                        for (size_t li = 0; li < lh.size(); ++li) lh[li] *= tilts[li];
                }
                if (this->out_atsfile_collectors.at(rhs)->is_E()) {
                    for (size_t li = 0; li < rh.size(); ++li) rh[li] *= tilts[li];

                }

                for (size_t li = 0; li < lh.size(); ++li) {
                    this->all_acspc_collector->data[ac_pos].append(lh[li] * std::conj(rh[li]), f);
                }

                ++ssi;
            }

            ++lspc; // next of the 377 vectors
            ++rspc;
        }




    }
}

void acsp_prz::fold_lines(const size_t lhs, const size_t rhs, const size_t ac_pos)
{
    // calc the auto cross value

    auto lspc = this->out_atsfile_collectors.at(lhs)->spc.vdata.cbegin();  // here we have to example 377 vectors of 256
    auto rspc = this->out_atsfile_collectors.at(rhs)->spc.vdata.cbegin();

    // take first with size of 256
    std::vector<std::complex<double>> result(this->out_atsfile_collectors.operator[](lhs)->spc.vdata.at(0).size());

    while (lspc != this->out_atsfile_collectors.at(lhs)->spc.vdata.cend()) {

        auto result_iter = result.begin();
        auto lspc_v = lspc->cbegin();          // point to the begin of fist 377 which should have size 256 then
        auto rspc_v = rspc->cbegin();
        while (lspc_v != lspc->cend()) {

            *result_iter = *lspc_v * std::conj(*rspc_v);
            ++result_iter;
            ++lspc_v;
            ++rspc_v;
        }


        if ((this->target_freqs.size() == this->selindices_sizes.size()) && this->indices.size()) {

            size_t j = 0, k = 0;

            for (auto &sel : selindices_sizes) {
                for (size_t i = 0; i < sel; ++i) {
                    this->all_acspc_collector->data[ac_pos].append(result[indices[j++]], this->target_freqs[k]);
                }
                ++k;
            }

        }
        else if (this->freqs.size()) {
            // complete FFT
            this->all_acspc_collector->data[ac_pos].append(result, this->freqs);
        }

        ++lspc; // next of the 377 vectors
        ++rspc;
    }


    // parzen

    qDebug() << "all auto cross lines stored" << pmt::ac_spectra_names.at(ac_pos) << ac_pos;

    // store
}



void acsp_prz::tilt_e_lines(const size_t idx)
{
    auto e_spc = this->out_atsfile_collectors.at(idx)->spc.vdata.cbegin();
}
