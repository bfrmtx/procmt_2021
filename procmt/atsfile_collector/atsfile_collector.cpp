#include "atsfile_collector.h"


atsfile_collector::atsfile_collector(size_t chan_type, QObject *parent) :  QObject (parent), prc_com(false), chan_type(chan_type)
{
}

atsfile_collector::atsfile_collector(std::shared_ptr<atsfile> atsf, QObject *parent) : QObject (parent), prc_com(*atsf)
{
    if (atsf != nullptr) {
        this->set_data(atsf->get_data(), false);
        this->chan_type = atsf->chan_type;
        this->spc.chan_type = this->chan_type;
        this->qfi = atsf->get_qfile_info();
        this->slot = atsf->get_slot();
    }
}


atsfile_collector::~atsfile_collector()
{
    this->clear();

    qDebug() << "atsfile_collector deleted, slot" << this->slot;
}

void atsfile_collector::clear()
{
    this->reset_buffer();
    this->freqs.clear();
    this->target_freqs.clear();
    this->spc_data.clear();
    this->ts_data.clear();
    this->calibrated_ts.clear();
    this->all_spc.clear();
    this->all_ts_scaled.clear();
    this->QMap<QString,QVariant>::clear();
}



void atsfile_collector::reset_buffer()
{
    if (this->spcbuffer != nullptr) this->spcbuffer.reset();
    if (this->tsbuffer != nullptr) this->tsbuffer.reset();
    if (this->asp_coll != nullptr) this->asp_coll.reset();

}

void atsfile_collector::set_spc_buffer(std::shared_ptr<threadbuffer<std::complex<double>> > &buffer)
{
    std::cerr << "atsfile_collector::set_spc_buffer[" << this->slot <<  "] init from threadbuffer[" << buffer->get_id() << "] " << buffer->get_vector_size() << std::endl;
    this->spcbuffer = buffer;
    this->buffer_type = buffer->get_buffer_type();
    this->spc_data.reserve(buffer->get_vector_size());
    this->spc_data.resize(buffer->get_vector_size());
}

void atsfile_collector::set_ts_buffer(std::shared_ptr<threadbuffer<double> > &buffer)
{
    std::cerr << "atsfile_collector::set_ts_buffer[" << this->slot <<  "] init from threadbuffer[" << buffer->get_id() << "] " << buffer->get_vector_size() << std::endl;
    this->tsbuffer = buffer;
    this->buffer_type = buffer->get_buffer_type();
    this->ts_data.reserve(buffer->get_vector_size());
    this->ts_data.resize(buffer->get_vector_size());
}

void atsfile_collector::set_frequencies_and_target(const std::vector<double> &fft, const std::vector<double> &ft_target, const bool &take_all)
{

    if(!fft.size() || !ft_target.size()) {
        this->clear();
        qFatal("atsfile_collector::set_frequencies_and_target -> no frequencies exit");
    }

    this->freqs = fft;

    std::vector<double> lc_targets;

    if (take_all) {
        this->select_from_target_freqs = false;
        this->target_freqs = fft;
        this->spc_data.reserve(this->target_freqs.size());
        this->spc_data.resize(this->target_freqs.size());
        return;
    }
    this->target_freqs = ft_target;

    this->select_from_target_freqs = true;


    vector_2_indices<double>(this->freqs, this->target_freqs, lc_targets, this->indices);


    // finally the same
    if (   (this->indices.size() == this->target_freqs.size()) && (lc_targets.size() == this->freqs.size())) {
        this->indices.clear();
        this->select_from_target_freqs = false;
        this->spc_data.reserve(this->target_freqs.size());
        this->spc_data.resize(this->target_freqs.size());
    }
    else {
        this->spc_data.reserve(this->indices.size());
        this->spc_data.resize(this->indices.size());
        this->target_freqs = lc_targets;

    }


}

void atsfile_collector::set_frequencies_and_target_indices(const std::vector<double> &fft, const std::vector<double> &ft_target, const std::vector<size_t> &selindices, const std::vector<size_t> &selindices_sizes)
{
    if (fft.size() && ft_target.size() && selindices.size() && selindices_sizes.size()) {

        if (selindices_sizes.size() != ft_target.size()) {
            qFatal("atsfile_collector::set_frequencies_and_target_indices selected indices differ from taget frequencies size");
        }

        this->freqs = fft;
        this->target_freqs = ft_target;
        this->indices = selindices;
        this->selindices_sizes = selindices_sizes;
        this->spc_data.reserve(this->indices.size());
        this->spc_data.resize(this->indices.size());
        select_from_target_freqs = true;
        for (auto &f : target_freqs) this->line_data.emplace(f, std::vector<std::complex<double>>());

        for (auto &v : line_data) {
            v.second.reserve(40000);
        }

    }
}


void atsfile_collector::set_double_buffer(std::shared_ptr<threadbuffer<double> > &buffer)
{
    std::cerr << "atsfile_collector::set_double_buffer[" << this->slot <<  "] init from threadbuffer[" << buffer->get_id() << "] " << buffer->get_vector_size() << std::endl;
    this->tsbuffer = buffer;
    this->buffer_type = buffer->get_buffer_type();
    this->ts_data.reserve(buffer->get_vector_size());
    this->ts_data.resize(buffer->get_vector_size());
}

void atsfile_collector::set_complex_buffer(std::shared_ptr<threadbuffer<std::complex<double> > > &buffer)
{
    std::cerr << "atsfile_collector::set_complex_buffer[" << this->slot <<  "] init from threadbuffer[" << buffer->get_id() << "] " << buffer->get_vector_size() << std::endl;
    this->spcbuffer = buffer;
    this->buffer_type = buffer->get_buffer_type();

}

void atsfile_collector::set_all_spectra_collector(std::shared_ptr<all_spectra_collector<std::complex<double> > > &asp_coll)
{
    this->asp_coll = asp_coll;
}

void atsfile_collector::set_parzen_vectors(std::shared_ptr<std::vector<std::vector<double> > > parzen_vectors_sptr,  const std::vector<double> &f, const std::vector<double> &ft)
{
    this->parzen_vectors_sptr = parzen_vectors_sptr;
    this->target_freqs = ft;
    this->freqs = f;
}



void atsfile_collector::collect_all_spc()
{

    size_t local_count = 0;
    if (!this->freqs.size()  || !this->target_freqs.size()) {
        std::cerr << "atsfile_collector::collect_all_spc -> no target frequencies" << std::endl;
        return;
    }
    if ((this->buffer_type <= atsfileout::atsfileout_raw_spectra ) && (this->buffer_type >= atsfileout_spectra )) {
        this->spc.freqs = std::move(this->target_freqs);

        if (this->parzen_vectors_sptr != nullptr) {
            if (this->parzen_vectors_sptr->size() != this->spc.freqs.size()) {
                qFatal("atsfile_collector::collect_all_spc -> parzen_vectors_sptr->size() != this->spc.freqs.size()");
            }
        }
        std::vector<double> tilts;
        std::vector<std::complex<double>> segment;
        do  {
            spcbuffer->fetch(this->spc_data, this->buffer_status);
            //            if (this->buffer_status == threadbuffer_status::finished) {
            //                qDebug() << "fin";
            //                break;
            //            }
            if (spc_data.size() && (this->buffer_status != threadbuffer_status::terminated)) {
                if ((this->parzen_vectors_sptr != nullptr) &&  (this->spc_data.size() == this->parzen_vectors_sptr->at(0).size()) ) {
                    this->lc_cplx_buf.reserve(this->spc.freqs.size());
                    size_t i = 0;
                    for (i = 0; i < this->spc.freqs.size(); ++i) {
                        this->lc_cplx_buf.push_back(std::inner_product(this->parzen_vectors_sptr->at(i).begin(), this->parzen_vectors_sptr->at(i).end(), this->spc_data.begin(), std::complex<double>(0.0)));
                    }


                    this->spc.data.push(lc_cplx_buf);
                    this->lc_cplx_buf.clear();

                }
                // that works only with non overlapping indices
                else if (this->select_from_target_freqs && this->selindices_sizes.size()) {


                    size_t k = 0, l = 0;


                    if ((this->ch_type() == 0) || this->ch_type() == 1) {
                        for (size_t i = 0; i < selindices_sizes.size(); ++i) {
                            tilts = iter::vector_from_indices<double>(this->freqs.cbegin(), this->selindices_sizes, this->indices, k);
                            for (auto &ft : tilts) {
                                //   qDebug() << this->spc.freqs[k] << ft;
                                ft = sqrt(this->spc.freqs[k]/ft);
                            }
                            for ( size_t j = 0; j < selindices_sizes[i]; ++j) {

                                this->line_data[this->spc.freqs[k]].emplace_back(this->spc_data[this->indices[l++]] *sqrt(2*M_PI) * tilts[j]);
                                //this->line_data[this->spc.freqs[k]].emplace_back(this->spc_data[this->indices[l++]]);
                            }
                            ++k;
                        }
                    }
                    else {
                        for (size_t i = 0; i < selindices_sizes.size(); ++i) {
                            for ( size_t j = 0; j < selindices_sizes[i]; ++j) {
                                this->line_data[this->spc.freqs[k]].emplace_back(this->spc_data[this->indices[l++]]);
                            }
                            ++k;
                        }

                    }

                }
                // that is CSAMT / CSEM for example
                else if (this->select_from_target_freqs && this->indices.size()) {
                    segment.clear();
                    for (auto &iv : this->indices) {
                        segment.push_back(spc_data[iv]);
                    }
                    this->spc.data.push(segment);
                }

                else {
                    this->spc.data.push(spc_data);

                }

                this->ats_read_counts++;
                local_count++;


                if (local_count > flush_count ) {
                    emit this->signal_flush_collector();
                    local_count = 0;
                }
                // qDebug() << this->ats_read_counts;
                // enqueue
            }
        } while (this->buffer_status == threadbuffer_status::running);
        qDebug() << "atsfile collector finished, slot" << this->slot << "," << this->ats_read_counts << "buffers received";
        this->spc_data.clear();
        this->lc_cplx_buf.clear();

        // so spc.data is my local memmory of this collector thread, inside loop run1 run2 ... complete site
        // move it out now
        if ((this->asp_coll != nullptr) && (this->chan_type < size_t(pmt::channel_types.size()))) {
            if (this->line_data.size()) {
                this->asp_coll->data[this->chan_type].chan_type = this->chan_type;
                for (auto &v : line_data) {
                    this->asp_coll->data[this->chan_type].append(v.second, v.first);
                }
            }
            else {
                this->asp_coll->data[this->chan_type].append_clear_data(this->spc);
            }
        }
    }
}


void atsfile_collector::slot_get_all_ts(std::vector<double> &all_ts_scaled, const bool &write_immedeately)
{
    this->all_ts_scaled = std::move(all_ts_scaled);
    if (write_immedeately) this->slot_write_ascii_ts(".tsdata");
}

void atsfile_collector::slot_write_ascii_ts(const QString file_extension)
{
    if (this->all_ts_scaled.size()) {
        // ascii writing takes long
        emit this->signal_guess_100(40);

        std::unique_ptr<ats_ascii_file> asciiwriter = std::make_unique<ats_ascii_file>(this->qfi, file_extension, this->slot);
        connect(asciiwriter.get(), &ats_ascii_file::signal_guess_100, this, &atsfile_collector::slot_guess_100, Qt::DirectConnection);
        asciiwriter->write_all_data(this->all_ts_scaled, 40, 80);
        this->all_ts_scaled.clear();
        asciiwriter.reset();
        emit this->signal_ascii_write_finished();

        // guess that not all files closed perfectly, so let main finsih 100
        emit this->signal_guess_100(90);
    }

}

void atsfile_collector::slot_repaint()
{
    emit this->signal_repaint();
}

void atsfile_collector::slot_guess_100(int value_0_100)
{
    emit this->signal_guess_100(value_0_100);
}
