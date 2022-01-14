#include "atsfile.h"


atsfile::atsfile(const QFileInfo &qfi, const short &header_version, QObject *parent): atsheader(qfi, header_version, parent)
{

}
atsfile::atsfile(const QFileInfo &qfi, const size_t &slot, const short &header_version, QObject *parent): atsheader(qfi, header_version, parent)
{
    this->slot = slot;

}
atsfile::atsfile( QObject *parent) : atsheader(parent) {
    this->clear(0);
}


atsfile::atsfile(const QString &dir, const QString &name, const short &header_version, QObject *parent) : atsheader(dir, name, header_version, parent)
{

}

atsfile::atsfile(const atsfile &atsf) : atsheader(atsf)
{
    this->slot = atsf.slot;
}

atsfile::atsfile(const QFileInfo &qfi, const int &file_version, const int header_version, const int &system_serial_number, const double &sample_freq,
                 const int &run_number, const int &channel_number, const QString &channel_type, const QString &ADB_board_type,
                 const QString &observatory, const QString &ceadate, const QString &ceameastype)
    : atsheader(file_version, header_version, system_serial_number, sample_freq, run_number, channel_number, channel_type, ADB_board_type,
                observatory, ceadate, ceameastype)
{

}






//void atsfile::set_emit_ats_thrd_tsdata_ready(const bool active)
//{
//    emit_ats_thrd_tsdata_ready = active;
//}

//void atsfile::set_emit_ats_thrd_spcdata_ready(const bool active)
//{
//    emit_ats_thrd_spcdata_ready = active;
//}

//void atsfile::set_emit_ats_thrd_inverse_spcdata_ready(const bool active)
//{
//    emit_ats_thrd_inverse_spcdata_ready = active;
//}

//void atsfile::set_emit_ats_thrd_spcdata_ampl_ready(const bool active)
//{
//    emit_ats_thrd_spcdata_ampl_ready = active;

//    if (active && this->fftreal == nullptr) {
//        this->fftreal = new fftreal_vector();
//        this->fftreal->set_forward_buffer(this->tsdata, this->spcdata, this->map.value("sample_freq").toDouble(), true, 0, 0, 0);
//    }
//}



atsfile::~atsfile()
{
    this->reset_buffer();

    if (this->fftreal != nullptr) this->fftreal.reset();
    if (this->firfil != nullptr) this->firfil.reset();
    if (this->atm != nullptr) this->atm.reset();


    qDebug() << "atsfile deleted, slot" << this->slot;

}

bool atsfile::open_atm(const int open_0_onthefly_1_create_2)
{
    if (this->atm != nullptr) this->atm.reset(nullptr);

    QFileInfo qfi(this->absoluteFilePath());
    this->atm = std::make_unique<atmfile>(qfi, this);
    if (open_0_onthefly_1_create_2  < 2) {
        size_t nsamp;
        nsamp = this->atm->read_all();
        if (nsamp != this->get_num_samples() && (open_0_onthefly_1_create_2 == 0)) {
            this->atm.reset(nullptr);
            return false;
        }
        if (open_0_onthefly_1_create_2 == 1) {
            this->atm->b.resize(this->get_num_samples());
            return true;
        }
    }

    return true;
}

void atsfile::reset_buffer()
{
    if (this->tsbuffer != nullptr) this->tsbuffer.reset();
    if (this->spcbuffer != nullptr) this->spcbuffer.reset();
    if (this->spc_amplbuffer != nullptr) this->spc_amplbuffer.reset();
    std::cerr << "ats reset" << std::endl;
}

std::shared_ptr<threadbuffer<double> > atsfile::get_create_buffer(const size_t &nbuffers, const atsfileout &buffer_type)
{
    // time series data

    // should always be true except
    if (buffer_type == atsfileout::atsfileout_unscaled_timeseries) this->scale_me = false;
    if (buffer_type == atsfileout::atsfileout_unscaled_fir_filtered) this->scale_me = false;

    if (buffer_type < atsfileout::atsfileout_spectra) {
        this->tsbuffer = std::make_shared<threadbuffer<double>>(nbuffers, tsdata.size(), buffer_type, this->slot);
        this->buffer_type = buffer_type;
        return this->tsbuffer;

    }

    // stacked spectra for example
    if (buffer_type >= atsfileout::atsfileout_stacked_spectra) {
        this->spc_amplbuffer = std::make_shared<threadbuffer<double>>(nbuffers, this->spc_ampl_data.size(), buffer_type, this->slot);
        this->buffer_type = buffer_type;
        return this->spc_amplbuffer;
    }

    return nullptr;
}

std::shared_ptr<threadbuffer<std::complex<double> > > atsfile::get_create_cplx_buffer(const size_t &nbuffers, const atsfileout &buffer_type)
{
    if (buffer_type >= atsfileout::atsfileout_spectra && buffer_type < atsfileout::atsfileout_stacked_spectra) {
        this->spcbuffer = std::make_shared<threadbuffer<std::complex<double>>>(nbuffers, this->spcdata.size(), buffer_type, this->slot);
        this->buffer_type = buffer_type;
        return this->spcbuffer;
    }

    return nullptr;
}

std::shared_ptr<threadbuffer<int> > atsfile::get_create_int_buffer(const size_t &nbuffers, const atsfileout &buffer_type)
{
    this->scale_me = false;
    this->ts_intbuffer = std::make_shared<threadbuffer<int>>(nbuffers, ts_intdata.size(), buffer_type, this->slot);
    this->buffer_type = buffer_type;
    return this->ts_intbuffer;

}

std::shared_ptr<threadbuffer<QString> > atsfile::get_create_csv_buffer(const size_t &nbuffers)
{
    this->scale_me = false;
    this->ts_strbuffer = std::make_shared<threadbuffer<QString>>(nbuffers, 1, atsfileout::atsfileout_void, this->slot);
    return this->ts_strbuffer;
}


bool atsfile::csvline(const QString &line_in)
{
    QString line(line_in.simplified());
    QString remove;
    size_t i = 0;
    if (!QString::compare(this->channel_type(), "Ex")) remove = "\"E1\": [";
    else if (!QString::compare(this->channel_type(), "Ey")) remove = "\"E2\": [";
    else if (!QString::compare(this->channel_type(), "Hx")) remove = "\"H1\": [";
    else if (!QString::compare(this->channel_type(), "Hy")) remove = "\"H2\": [";
    else if (!QString::compare(this->channel_type(), "Hz")) remove = "\"H3\": [";

    if ((line.startsWith(remove)) && (line.endsWith("],")) ) {
        int posa = line.indexOf(QChar('['));
        int posb = line.lastIndexOf(QChar(']'));

        QStringView ref(QStringView{line}.mid(posa + 1, posb - posa - 1));
        QVector<QStringView> qvref(ref.split(QChar(',')));
        this->queue_data.emplace(std::vector<double>(qvref.size()));

        for (auto const &strf : qvref) {
            this->samples_read++;
            this->queue_data.back()[i++] = (strf.toDouble() * 1000.);

        }
    }

    if (i) return true;
    return false;


}

QString atsfile::phoenix_json_ts_tag() const
{
    QString remove;
    if (!QString::compare(this->channel_type(), "Ex")) remove = "\"E1\": [";
    else if (!QString::compare(this->channel_type(), "Ey")) remove = "\"E2\": [";
    else if (!QString::compare(this->channel_type(), "Hx")) remove = "\"H1\": [";
    else if (!QString::compare(this->channel_type(), "Hy")) remove = "\"H2\": [";
    else if (!QString::compare(this->channel_type(), "Hz")) remove = "\"H3\": [";
    return remove;
}


std::shared_ptr<threadbuffer<double> > atsfile::get_create_double_input_buffer(const size_t &nbuffers, const atsfileout &buffer_type, const size_t size)
{
    this->scale_me = false;
    this->tsdata.resize(size);
    this->tsbuffer = std::make_shared<threadbuffer<double>>(nbuffers, tsdata.size(), buffer_type, this->slot);
    return this->tsbuffer;

}

void atsfile::clear(const short header_version)
{
    this->insert("file_aduser",  0);
    this->insert("file_version",  1);
    this->insert("file_channel_number", 0);
    this->insert("file_run_number",  std::uint32_t(0) );

    // Ex Ey Hx Hy Hz
    this->insert("file_channel_type",  QString("Ezz"));
    this->insert("file_ADB_board_type",  QString("NN"));
    this->insert("file_sample_freq", double(0));

    this->insert("file_date", QDate(1970, 1, 1));

    // "ns" natural source, "ca" controlled source AM, "cp" controlled source PM
    this->insert("file_source", QString("xx"));

    // e.g a number like 21007
    this->insert("file_observatory", int(-1));

    atsheader::clear(header_version);
    this->old_filepos = 0;

    this->can_read = false;
    this->slot = -1;
    this->is_remote = false;
    this->ats_read_counts = 0;
    this->ats_write_counts = 0;
    this->max_counts = SIZE_MAX;
    this->ats_read_counts = 0;
    this->ats_write_counts = 0;


}



size_t atsfile::scan_header_close()
{
    size_t samples = atsheader::scan_header_close();
    if (samples == SIZE_MAX) return samples;
    this->old_filepos = 0;

    // do not emit close here
    return samples;
}

size_t atsfile::read_header()
{
    this->old_filepos = 0;
    size_t samples = atsheader::read_header();
    if (samples == SIZE_MAX) return samples;


    return samples;
}

qint64 atsfile::get_filepos() const
{
    return this->file.pos();
}

bool atsfile::set_window_length(const size_t &wl, const double &fcut_upper, const double &fcut_lower, const bool &last_read, const quint64 &use_samples, const quint64 &overlapping, const atsfileout &buffer_type)
{
    // 06e and 07e : 10kHz is max 06e and 50 kHz is may 07e - check outside in advance
    // max_counts should be 1 or greater; in case of 1 the reader must handle max_counts
    if (!wl) {
        this->max_counts = 0;
        return false;
    }
    this->fcut_upper = fcut_upper;
    this->fcut_lower = fcut_lower;
    if (buffer_type != atsfileout::atsfileout_void) this->buffer_type = buffer_type;

    // filter has to be set // or buffer type  - see below
    if (this->buffer_type != atsfileout::atsfileout_unscaled_fir_filtered) {

        if (overlapping < wl) this->move_back_after_read = overlapping;
    }


    quint64 samples_to_use = use_samples;
    if (this->override_use_samples) samples_to_use = this->override_use_samples;

    // we have already an auto window size from filter for example
    if (this->buffer_type == atsfileout::atsfileout_unscaled_fir_filtered) {
        if (!tsdata.size()) {
            qDebug() << "atsfile::set_window_length -> missing FIR filter?? ---------------------";
            exit(0);
        }
        // do not try to resize in order to fit the end - buffer must be wl or filter length
        this->last_read_size = 0;
        // this->move_back_after_read must have a vaule for decimation filter
    }
    else {

        if ((buffer_type == atsfileout::atsfileout_unscaled_timeseries) || (buffer_type == atsfileout::atsfileout_scaled_timeseries) ) {
            if (wl > samples_to_use) {
                this->tsdata.reserve(samples_to_use);
                this->tsdata.resize(samples_to_use);
            }
            else {
                this->tsdata.reserve(wl);
                this->tsdata.resize(wl);
            }
        }

        else {
            this->tsdata.reserve(wl);
            this->tsdata.resize(wl);
        }
    }

    ldiv_t divresult;
    if (!samples_to_use) divresult = ldiv( (this->sztvalue("num_samples") - this->tsdata.size()) , (this->tsdata.size() - this->move_back_after_read ) );
    else divresult = ldiv((samples_to_use - this->tsdata.size()),((this->tsdata.size() - this->move_back_after_read )));
    this->max_counts = size_t(divresult.quot + 1);
    if (last_read)    this->last_read_size = size_t(divresult.rem);
    else {
        this->last_read_size = 0;
    }
    if (this->buffer_type == atsfileout::atsfileout_unscaled_fir_filtered)  last_read_size = 0; // dont use for fir filter



    if ((this->buffer_type >= atsfileout::atsfileout_stacked_timeseries) && (this->buffer_type <= atsfileout::atsfileout_stacked_calibrated_scaled_timeseries) ) {
        this->tsdata_stacked.resize(wl);
        this->last_read_size = 0;
        this->max_counts = 1;
        emit this->signal_get_max_counts_and_last(1,0);
    }

    if (this->fftreal != nullptr) {
        double ffreq;
        // save old settings when changing the window length
        this->fftreal->get_settings(ffreq, this->bwincal, this->isdcfft, this->nspc, this->inverse_from_forward, this->fcut_upper, this->fcut_lower);
        this->fftreal.reset();
        this->fftreal = nullptr;
    }

    // want spectra in general; e.g for MT
    if ((this->buffer_type >= atsfileout::atsfileout_spectra) && (this->buffer_type <= atsfileout::atsfileout_stacked_amplitude_spectra)) {
        this->fftreal =  std::make_unique<fftreal_vector>();
        // here spcdata is allocated
        this->fftreal->set_buffer(this->tsdata, this->spcdata, this->get_sample_freq(), this->bwincal, this->isdcfft, this->nspc, this->inverse_from_forward, this->fcut_upper, this->fcut_lower);
        if ( (this->buffer_type >= atsfileout::atsfileout_stacked_spectra) && (this->buffer_type <= atsfileout::atsfileout_stacked_amplitude_spectra)) {
            this->spc_ampl_data.reserve(this->spcdata.size());
            this->spc_ampl_data.resize(this->spcdata.size());
            last_read_size = 0;
            this->max_counts = 1;
        }
        // generate a frequency vector

        //this->freqs = this->fftreal->get_frequencies();
        emit this->signal_fft_freqs(this->fftreal->get_frequencies());

        if ((this->calib != nullptr) && (this->fftreal != nullptr)) {
            this->calib->slot_set_chopper(this->ivalue("chopper_status"));
            if (this->options.svalue("calibration") == "theo") {
                std::cout << "read_all_single calibration theoretical used " << QString::number(this->get_sample_freq()).toStdString() << " Hz" << std::endl;
                this->calib->gen_theo_cal(this->fftreal->get_frequencies());
                this->calib->theo_cal_to_active();
            }
            else {
                if (this->ivalue("chopper_status") == cal::chopper_off) {
                    emit this->signal_input_cal_used(calib->sensortype, calib->sernum, this->svalue("channel_type"), calib->f_off, calib->ampl_off, calib->phase_grad_off, cal::chopper_off);
                    this->calib->interploate(this->fftreal->get_frequencies(), ivalue("channel_number"), cal::chopper_off, true, true, false ) ;
                    if (this->sys_calib != nullptr) {
                        this->sys_calib->gen_theo_cal(this->fftreal->get_frequencies());
                        this->sys_calib->theo_cal_to_active();
                        this->calib->fold(this->sys_calib);
                    }

                    emit this->signal_interpolated_cal_used(calib->sensortype, calib->sernum, this->svalue("channel_type"), calib->f_off_ipl, calib->ampl_off_ipl, calib->phase_grad_off_ipl, cal::chopper_off);

                }
                else {
                    emit this->signal_input_cal_used(calib->sensortype, calib->sernum, this->svalue("channel_type"), calib->f_on, calib->ampl_on, calib->phase_grad_on, cal::chopper_on);
                    this->calib->interploate(this->fftreal->get_frequencies(), ivalue("channel_number"), cal::chopper_on, true, true, false ) ;
                    if (this->sys_calib != nullptr) {
                        this->sys_calib->gen_theo_cal(this->fftreal->get_frequencies());
                        this->sys_calib->theo_cal_to_active();
                        this->calib->fold(this->sys_calib);
                    }
                    emit this->signal_interpolated_cal_used(calib->sensortype, calib->sernum, this->svalue("channel_type"), calib->f_on_ipl, calib->ampl_on_ipl, calib->phase_grad_on_ipl, cal::chopper_on);

                }
            }
        }

    }

    //


    this->set_progress_bar_resolution(5);
    emit this->signal_get_max_counts_and_last(this->max_counts, this->last_read_size);

    this->options.set_key_size_t("wl", this->tsdata.size());
    this->options.set_key_value("fcut_upper", fcut_upper);     // cut upper part of FFT
    this->options.set_key_value("fcut_lower", fcut_lower);     // cut lower part of FFT
    this->options.set_key_size_t("overlapping", this->move_back_after_read);
    this->options.set_key_size_t("use_samples", samples_to_use);
    this->options.set_key_size_t("last_read", this->last_read_size);



    return true;
}

bool atsfile::min_max_cal(const size_t &wl, double &get_maxf, double &get_minf, double &max_cut_upper, double &min_cut_lower, const bool limit_upper_to_cal)
{
    // let the fft calculate as above
    std::unique_ptr<fftreal_vector> fft = std::make_unique<fftreal_vector>();
    std::vector<double> tsd(wl);
    std::vector<std::complex<double>> spcx;
    fft->set_buffer(tsd, spcx, this->get_sample_freq(), this->bwincal, this->isdcfft, this->nspc, this->inverse_from_forward, this->fcut_upper, this->fcut_lower);
    std::vector<double> ff(fft->get_frequencies());
    double tmp_max;

    if (!ff.size()) {
        get_maxf = 0.0;
        get_minf = 0.0;
        min_cut_lower = 0.0;
        max_cut_upper = 0.0;
    }


    if (this->calib != nullptr) {
        this->calib->blockSignals(true);
        this->calib->slot_set_chopper(this->ivalue("chopper_status"));

        if (this->options.svalue("calibration") == "theo") {
            this->calib->gen_theo_cal(ff);
            bool isok = false;
            if (this->ivalue("chopper_status") == cal::chopper_off) {
                if (this->calib->f_off_theo.size()) {
                    get_maxf = this->calib->f_off_theo.back();
                    get_minf = this->calib->f_off_theo.front();
                    isok = true;
                }
            }
            else {
                if (this->calib->f_on_theo.size()) {
                    get_maxf = this->calib->f_on_theo.back();
                    get_minf = this->calib->f_on_theo.front();
                    isok = true;
                }
            }
            if (isok) {
                this->calib->blockSignals(false);

                size_t start_idx = size_t(std::lower_bound (ff.begin(), ff.end(), get_minf) - ff.begin());
                min_cut_lower = ((double (start_idx)) / (double (wl/2)));

                // this->stop_idx = (size_t) ( (double)this->wl/2 - ( (double)this->wl/2 * this->cut_upper)  );
                size_t stop_idx = size_t (std::upper_bound (ff.begin(), ff.end(), get_maxf) - ff.begin());
                if (stop_idx > wl/2) stop_idx = wl/2;
                max_cut_upper = (double(wl/2 - stop_idx)) / (double(wl/2));
            }
            else {
                get_maxf = 0.0;
                get_minf = 0.0;
                min_cut_lower = 0.0;
                max_cut_upper = 0.0;

                return false;
            }


        }
        else {


            if (this->ivalue("chopper_status") == cal::chopper_off) {

                this->calib->interploate(ff, ivalue("channel_number"), cal::chopper_off, true, true, false ) ;
                tmp_max = this->calib->f_off.back();
            }
            else {
                this->calib->interploate(ff, ivalue("channel_number"), cal::chopper_on, true, true, false ) ;
                tmp_max = this->calib->f_on.back();
            }
            if (this->calib->f.size()) {
                this->calib->blockSignals(false);
                if (limit_upper_to_cal) {
                    get_maxf = tmp_max;
                }
                else get_maxf = this->calib->f.back();
                get_minf = this->calib->f.front();

                //   this->start_idx = (size_t) ( (double)this->wl/2 * this->cut_lower);
                size_t start_idx = size_t(std::lower_bound (ff.begin(), ff.end(), get_minf) - ff.begin());
                min_cut_lower = ((double (start_idx)) / (double (wl/2)));

                // this->stop_idx = (size_t) ( (double)this->wl/2 - ( (double)this->wl/2 * this->cut_upper)  );
                size_t stop_idx = size_t (std::upper_bound (ff.begin(), ff.end(), get_maxf) - ff.begin());
                if (stop_idx > wl/2) stop_idx = wl/2;
                max_cut_upper = (double(wl/2 - stop_idx)) / (double(wl/2));
            }

            // calibration failed
            else {
                get_maxf = 0.0;
                get_minf = 0.0;
                min_cut_lower = 0.0;
                max_cut_upper = 0.0;

                return false;

            }
        }

    }

    else {
        get_maxf = ff.back();
        get_minf = ff.front();
        min_cut_lower = 0.0;
        max_cut_upper = 0.0;

    }

    return true;



}

void atsfile::set_buffer_type(const atsfileout &buffer_type)
{
    this->buffer_type = buffer_type;
}

void atsfile::set_buffer(std::shared_ptr<threadbuffer<double> > &buffer)
{
    this->tsbuffer = buffer;
    this->buffer_type = buffer->get_buffer_type();
    this->tsdata.reserve(buffer->get_vector_size());
    this->tsdata.resize(buffer->get_vector_size());
}

size_t atsfile::get_window_length() const
{
    return this->tsdata.size();
}

size_t atsfile::get_spectra_length() const
{
    return this->spcdata.size();
}

std::vector<double> atsfile::get_frequencies() const
{
    if (this->fftreal != nullptr) return this->fftreal->get_frequencies();
    return std::vector<double>();
}


QDataStream::Status atsfile::get_double_samples(std::vector<double> &v, bool scaleme)
{
    this->old_filepos = this->file.pos();

    double factor = this->bin_atsheader.dblLSBMV;
    if (this->scale_me && this->is_scalable) factor *= (1000.0 / this->dipole_length);

    if (this->is_64bit_samples()) {
        qint64 s;
        for (auto &ve : v) {
            this->qds >> s;
            ve = s * factor;
        }
    }
    else {
        qint32 s;
        for (auto &ve : v) {
            this->qds >> s;
            ve = s * factor;
        }
    }


    ++this->ats_read_counts;
    // fails - action if this slot is connected
    emit this->QDataStreamStatus(this->qds.status());

    return this->qds.status();


}

QDataStream::Status atsfile::set_double_samples(const std::vector<double> &v)
{
    if (this->is_64bit_samples()) {
        for (auto &ve : v) {
            this->qds << qint64(ve/this->bin_atsheader.dblLSBMV);
            ++this->samples_written;
        }
    }

    else {
        for (auto &ve : v) {
            this->qds << qint32(ve/this->bin_atsheader.dblLSBMV);
            ++this->samples_written;
        }
    }


    ++this->ats_write_counts;
    // fails - action if this slot is connected
    emit this->QDataStreamStatus(this->qds.status());

    if (!this->qds.status()) {
        this->set_key_size_t("num_samples", this->sztvalue("num_samples") + v.size());
    }

    return this->qds.status();
}

QDataStream::Status atsfile::mytsdata_tofile()
{
   QDataStream::Status status;
   if (!this->tsdata.size()) return QDataStream::ReadCorruptData;
   status = this->set_double_samples(this->tsdata);
   this->tsdata.clear();
   return status;
}

QDataStream::Status atsfile::slot_slide_to_old_pos()
{

    qDebug() << "slide to old pos";
    this->qds.resetStatus();
    this->file.seek(this->old_filepos);
    this->old_filepos = this->file.pos();

    if (this->qds.status()) {
        // can't slide back to former position ??? nope !
        this->file.close();
        emit this->atsfile_closed(this->qds.status());
        return this->qds.status();
    }

    emit this->slided();
    return this->qds.status();
}

QDataStream::Status atsfile::write()
{

    size_t result = 0;
    if (!this->can_write) {
        this->get_new_filename();
        result = this->write_header(false);
    }
    if (result == SIZE_MAX) {
        qFatal(" atsfile::write -> can not create header");
        exit(1);
    }
    qDebug() << "streampos" << this->file.pos();
    if (this->buffer_type < atsfileout::atsfileout_spectra) {
        do {
            if ((this->tsbuffer == nullptr)) {
                std::cerr << std::endl << "what the fuck" << std::endl;
            }
            this->tsbuffer->fetch(this->tsdata, this->tsbuffer_status);
            //std::cerr << "atswriter[" << this->slot << "] writing" << std::endl;
            if (this->is_64bit_samples()) {
                for (auto &ve : this->tsdata) {
                    //   qDebug() << (qint32) (ve/this->bin_atsheader.dblLSBMV);
                    this->qds << qint64(ve/this->bin_atsheader.dblLSBMV);
                }
            }

            else {
                for (auto &ve : this->tsdata) {
                    //   qDebug() << (qint32) (ve/this->bin_atsheader.dblLSBMV);
                    this->qds << qint32(ve/this->bin_atsheader.dblLSBMV);
                }
            }

            this->ats_write_counts++;
            this->samples_written += this->tsdata.size();


        } while (this->tsbuffer_status == threadbuffer_status::running);
    }

    if (!this->qds.status()) {
        this->file.flush();
        this->set_key_value("num_samples", uint(this->samples_written));
        std::cerr << "write finished, rewrite header" << std::endl;
        qDebug() << "atsfile::write -> rewrite header" << this->get_new_filename() << this->uivalue("num_samples") << this->samples_written;
        this->write_header(true);
    }

    emit this->tx_adu_msg(ivalue("system_serial_number"), ivalue("channel_number"), this->slot, QString("atsfile::write finsihed, writings " + QString::number(this->ats_write_counts)));

    qDebug() << "atsfile finished, slot" << this->slot << "writings" << this->ats_write_counts;

    return this->qds.status();
}



// simple forward reader
// simple overlapping reader
// universal reader


QDataStream::Status atsfile::read()
{
    //!< @todo move_back_after_read and swap ovr vector
    if (this->can_read) {

        size_t marked_failed = SIZE_MAX;

        /*!
         * \todo move this block out here
         */



        qint32 move_size = sizeof(int32_t);
        if (this->bin_atsheader.bit_indicator == 1) move_size = sizeof(int64_t);


        double factor = this->bin_atsheader.dblLSBMV;
        if (this->scale_me && this->is_scalable) factor *= (1000.0 / this->dipole_length);


        // that is the position when we left the loop earlier
        this->old_filepos = this->file.pos();

        // we have read already samples - so we can move/back
        // if pos > header pos && read counts
        // move forward
        if (this->move_ahead_after_read && this->ats_read_counts) {
            this->file.seek(this->old_filepos + this->move_ahead_after_read * move_size);


        }
        // move back cant be bigger than ?
        if (this->move_back_after_read && this->ats_read_counts) {

            if (this->atm != nullptr) {
                // last window was ok, check the remaining
                marked_failed = this->atm->free_atm_window(this->get_sample_pos(), this->tsdata.size() - this->move_back_after_read);
                if (marked_failed == SIZE_MAX) {
                    // qDebug() << marked_failed << this->file.pos() << this->get_sample_pos();

                    // causes no - action if this slot is connected
                    emit this->QDataStreamStatus(QDataStream::ReadPastEnd);
                    return QDataStream::ReadPastEnd;
                }
                // we have a complete new position DO NOT USE FOR DIGFIL;
                if (marked_failed != this->get_sample_pos()) {
                    // qDebug() << marked_failed << this->file.pos() << this->get_sample_pos();
                    // this->file.seek(this->old_filepos - this->move_back_after_read * move_size);

                    this->file.seek(this->file.pos() + ( (marked_failed - this->get_sample_pos()) * move_size ));
                    if (this->is_64bit_samples()) {
                        qint64 s;
                        for (auto &ve : this->tsdata) {
                            this->qds >> s;
                            ve = s * factor;
                        }
                    }

                    else {
                        qint32 s;
                        for (auto &ve : this->tsdata) {
                            this->qds >> s;
                            ve = s * factor;
                        }
                    }

                }
            }
            if ( (this->atm == nullptr) || (marked_failed == this->get_sample_pos())) { // free window, use the buffer
                // fir filter: read 471, move back 439, add another 32
                size_t j = 0, i;
                for (i = this->tsdata.size() - this->move_back_after_read; i < this->tsdata.size(); ++i) {
                    this->tsdata[j++] = this->tsdata_overlap[i];
                }
                // that should be the remaining 32 or remaining overlap for FFT
                if (this->is_64bit_samples()) {
                    qint64 s;
                    for (i = j; i < this->tsdata.size(); ++i) {
                        this->qds >> s;
                        tsdata[j++] = s * factor;
                    }
                }
                else {
                    qint32 s;
                    for (i = j; i < this->tsdata.size(); ++i) {
                        this->qds >> s;
                        tsdata[j++] = s * factor;
                    }
                }

            }

        }
        else {   // that includes the forward reading
            // selection active
            if (this->atm != nullptr) {
                marked_failed = this->atm->free_atm_window(this->get_sample_pos(), this->tsdata.size());
                if (marked_failed == SIZE_MAX) {
                    // causes no - action if this slot is connected
                    emit this->QDataStreamStatus(QDataStream::ReadPastEnd);
                    return QDataStream::ReadPastEnd;
                }
                if (marked_failed) {
                    this->file.seek(this->file.pos() + ( (marked_failed - this->get_sample_pos()) * move_size) );
                }
            }
            if (this->is_64bit_samples()) {
                qint64 s;
                for (auto &ve : this->tsdata) {
                    this->qds >> s;
                    ve = s * factor;
                }
            }
            else {
                qint32 s;
                for (auto &ve : this->tsdata) {
                    this->qds >> s;
                    ve = s * factor;
                }
            }

        }

        // we may already reached the end
        if (!this->qds.status()) {

            // copy a segment instead of -re-reading from the disk
            if (this->move_back_after_read) {
                this->tsdata_overlap = this->tsdata;
            }

            ++this->ats_read_counts;
            this->samples_read += this->tsdata.size();

            if (this->emit_extra) {
                if (this->progress_bar_emits_at.size()) {
                    if(std::find(this->progress_bar_emits_at.begin(), this->progress_bar_emits_at.end(), this->ats_read_counts) != this->progress_bar_emits_at.end()) {
                        emit this->signal_counts_per(this->ats_read_counts);
                    }
                }

            }

        }
        else {
            // destroy unusable data
            this->tsdata.resize(0);
        }
        // causes no - action if this slot is connected
        emit this->QDataStreamStatus(this->qds.status());


        return this->qds.status();



    }


    else return QDataStream::ReadPastEnd;

}

void atsfile::read_tsplotter(const bool emits, const bool stack_all)
{

    if (!this->can_read || this->tsplotter_buffer_type_ts == atsfileout::atsfileout_void || !this->tsdata.size()) {
        this->QDataStreamStatus(QDataStream::ReadCorruptData);
        return;
    }

    double factor = this->bin_atsheader.dblLSBMV;

    // to use this signal, first use in your class
    // eQDateTime mytime = astfile.get_start_datetime()
    // then fetch the signal ad set the window time like
    // eQDateTime sample_time(const quint64 &nsamples) const;


    emit this->signal_start_window_sample(this->get_sample_pos());

    if ( (this->tsplotter_buffer_type_ts > atsfileout::atsfileout_unscaled_timeseries) && this->is_scalable) factor *= (1000.0 / this->dipole_length);

    if (this->is_64bit_samples()) {
        qint64 s;
        for (auto &ve : this->tsdata) {
            this->qds >> s;
            ve = s * factor;
        }
    }

    else {
        qint32 s;
        for (auto &ve : this->tsdata) {
            this->qds >> s;
            ve = s * factor;
        }
    }

    // avoid double detrend for calibrated time series - this will be done by fft
    if (tsplotter_detrend_active && (this->tsplotter_buffer_type_ts != atsfileout_calibrated_scaled_timeseries))
        iter::simple_detrend<double>(this->tsdata.begin(), this->tsdata.end());

    this->old_filepos = this->file.pos();

    emit this->QDataStreamStatus(this->qds.status());

    if(this->qds.status() != QDataStream::Ok) return;

    // CECK for INVERSE FFT FOR H if (tsplotter_buffer_type_ts != atsfileout_calibrated_scaled_timeseries)
    // do not emit in case of inverse, do that later

    if (this->tsplotter_buffer_type_ts != atsfileout_calibrated_scaled_timeseries) {

        if (emits) emit this->signal_tsdata(this->tsdata, this->slot);
        if (stack_all) std::transform (this->tsdata_stacked.begin(), this->tsdata_stacked.end(), this->tsdata.begin(), this->tsdata_stacked.begin(), std::plus<double>());
    }
    if (this->tsplotter_buffer_type_spc == atsfileout::atsfileout_void)                 return;

    // --------------------------------------------------------------------------------------------------------------------------------------------------------------

    // for E we do need an inverse FFT for the calibrated - but we detrend in case because we do the same in FFT
    if ((this->tsplotter_buffer_type_ts == atsfileout_calibrated_scaled_timeseries) && this->is_scalable) {
        iter::simple_detrend<double>(this->tsdata.begin(), this->tsdata.end());
        if (emits) emit this->signal_tsdata(this->tsdata, this->slot);
        if (stack_all) std::transform (this->tsdata_stacked.begin(), this->tsdata_stacked.end(), this->tsdata.begin(), this->tsdata_stacked.begin(), std::plus<double>());
        // and continue with H and or E, H spectra
    }



    // that needs a full rectangular FFT without hanning for H channels
    if ((this->tsplotter_buffer_type_ts == atsfileout_calibrated_scaled_timeseries) && (this->calib != nullptr) && !this->is_scalable) {

        std::vector<double> mytsdata(this->tsdata);
        // fftfull will reallocate the spcdata buffer - we have to include all AND the DC part here
        fftfull->set_buffer(mytsdata, this->spcdata, this->get_sample_freq(), this->bwincal, true, true, true, 0, 0);
        fftfull->set_mtx_calibration(this->calib->cap);

        fftfull->process(mytsdata, this->spcdata, fftreal_ctrl::detrend);
        for (auto &all : mytsdata) all *= -1.0;
        if (emits) emit this->signal_tsdata(mytsdata, this->slot);
        if (stack_all) std::transform (this->tsdata_stacked.begin(), this->tsdata_stacked.end(), mytsdata.begin(), this->tsdata_stacked.begin(), std::plus<double>());


    }

    this->fftreal->set_buffer(this->tsdata, this->spcdata, this->get_sample_freq(), this->bwincal, false, false, false, 0.0, 0.0);
    if (this->calib != nullptr && this->tsplotter_buffer_type_spc ==  atsfileout::atsfileout_calibrated_spectra) {
        this->fftreal->set_mtx_calibration(this->calib->cap);
    }
    this->fftreal->process(this->tsdata, this->spcdata, fftreal_ctrl::detrend_hanning);

    // I have other than tsplotter because full window is also used here;
    std::vector<double> myfftfreqs(this->fftreal->get_frequencies());





    if (emits) emit this->signal_cplx_spcdata(this->spcdata, this->slot);

    // prepare for te frequencies tsplotter wants
    this->spc_ampl_data.resize(this->plotter_frequencies->size());

    // that should happen only one we stack at first time
    if ((this->spc_ampl_data.size() != this->spc_ampl_data_stacked.size() ) && stack_all) {
        this->spc_ampl_data_stacked.resize(this->spc_ampl_data.size());
        std::fill(this->spc_ampl_data_stacked.begin(), this->spc_ampl_data_stacked.end(), 0.0);

    }

    auto myf = myfftfreqs.begin();
    auto plf = this->plotter_frequencies->begin();
    auto myd = this->spcdata.begin();
    auto myampl = this->spc_ampl_data.begin();

    // check!!
    while ( (myf != myfftfreqs.end()) && (myd != spcdata.end()) ) {

        if (*myf == *plf) break;

        ++myf; // move f pointer
        ++myd; // and data pointer synchroniuosly
    }
    if (myf == (myfftfreqs.end() - 1)) return;

    // now f should be at the same frequency as desired frequencies plf
    while ((myf != myfftfreqs.end()) && (myd != spcdata.end()) && plf != this->plotter_frequencies->end()) {
        if (*myf != *plf) break;

        *myampl++ = std::abs(*myd++);
        ++myf;
        ++plf;
    }

    if (plf != (this->plotter_frequencies->end())) return;


    //    this->spc_ampl_data.resize(this->spcdata.size());
    //    auto spcd = spc_ampl_data.begin();
    //    for (auto &cplx : this->spcdata) *spcd++ = std::abs(cplx);

    //#ifdef QT_DEBUG
    //    std::cout << std::endl;
    //    for (size_t g = 10; g < 20; ++g ) std::cout << this->spc_ampl_data[g] << " ";
    //    std::cout << std::endl;
    //#endif

    if (emits) emit this->signal_spcdata(this->spc_ampl_data, this->slot);
    if (stack_all) {
        std::transform (this->spc_ampl_data_stacked.begin(), this->spc_ampl_data_stacked.end(), spc_ampl_data.begin(), this->spc_ampl_data_stacked.begin(), std::plus<double>());
#ifdef QT_DEBUG
        std::cout << std::endl;
        for (size_t g = 10; g < 20; ++g ) std::cout << this->spc_ampl_data_stacked[g] << " ";
        std::cout << std::endl;
#endif
    }


}

void atsfile::read_all_tsplotter()
{

    if (!this->can_read || this->tsplotter_buffer_type_ts == atsfileout::atsfileout_void || !this->tsdata.size()) {
        this->QDataStreamStatus(QDataStream::ReadCorruptData);
        return;
    }

    this->tsdata_stacked.resize(this->tsdata.size(), 0);
    std::fill(this->tsdata_stacked.begin(), this->tsdata_stacked.end(), 0.0);
    this->spc_ampl_data_stacked.clear();


    // move to sync begin
    this->slot_slide_to_sample(this->edt_start.get_start_sample());

    for (size_t i = 0; i < this->max_counts; ++i) {
        this->read_tsplotter(false, true);
        if (this->progress_bar_emits_at.size()) {
            if(std::find(this->progress_bar_emits_at.begin(), this->progress_bar_emits_at.end(), i) != this->progress_bar_emits_at.end()) {
                emit this->signal_counts_per(i);
            }
        }
    }

    emit this->signal_counts_per(this->max_counts);

    double counts = double(this->max_counts);
    for (auto &d : this->tsdata_stacked)  d /= counts;

    // emit this->signal_tsdata(this->tsdata_stacked, this->slot);

    if (this->tsplotter_buffer_type_spc != atsfileout::atsfileout_void) {
        for (auto &d : this->spc_ampl_data_stacked) d /= counts;

        // this->signal_spcdata(this->spc_ampl_data_stacked, this->slot);
    }

    this->slot_slide_to_sample(this->edt_start.get_start_sample());

}

void atsfile::emit_stacked()
{
    emit this->signal_tsdata(this->tsdata_stacked, this->slot);
    if (this->tsplotter_buffer_type_spc != atsfileout::atsfileout_void) {
        this->signal_spcdata(this->spc_ampl_data_stacked, this->slot);
    }
}


/*
 * if read last: wait for status: read and calculate the rest
 * else let the while loop go
 * overlapping missing
 *
 *
 */
void atsfile::read_all()

{

    // hence that skip first must be done already - and so my functions are - if used they are immedeately shifted
    qDebug() << "read all, slot:" << this->slot;
    if (this->calib != nullptr) this->calib->eliminate_except_complex(); // don't need the rest here ... I think so

    // test this->emit_ats_thrd_tsdata_ready = false;

    if (this->buffer_type == atsfileout::atsfileout_calibrated_spectra) this->last_read_size = 0;

    while (this->ats_read_counts < this->max_counts) {


        // do not stack all - return all time series
        if (this->buffer_type < atsfileout::atsfileout_calibrated_scaled_timeseries) {
            this->read();
            if ((this->ats_read_counts < this->max_counts) || this->last_read_size) {
                this->tsbuffer_status = threadbuffer_status::running;
                this->tsbuffer->deposit(this->tsdata, threadbuffer_status::running);
            }
            else {
                this->tsbuffer_status = threadbuffer_status::finished;
                this->tsbuffer->deposit(this->tsdata, threadbuffer_status::finished);
            }

        }

        else if (this->buffer_type == atsfileout::atsfileout_unscaled_fir_filtered) {
            this->last_read_size = 0;
            while (!this->read() ) {
                this->fir_fil_output = iter::fold<double, std::vector<double>::const_iterator>(this->tsdata.begin(), this->tsdata.end(),
                                                                                               this->fir_fil_coeff.begin(), this->fir_fil_coeff.end());
                this->tsdata_firfiltered.push_back(this->fir_fil_output);

                // limit input file size ??
                if (this->tsdata_firfiltered.size() > 1023) {
                    this->tsbuffer_status = threadbuffer_status::running;
                    this->tsbuffer->deposit(this->tsdata_firfiltered, threadbuffer_status::running);
                    this->tsdata_firfiltered.clear();
                }

            }
            this->tsbuffer_status = threadbuffer_status::finished;
            this->tsbuffer->deposit(this->tsdata_firfiltered, threadbuffer_status::finished);
            this->tsbuffer->set_status(threadbuffer_status::finished);
            this->max_counts = this->ats_read_counts;
        }

        // stack all ts  - not spectra
        else if( (this->buffer_type >= atsfileout::atsfileout_stacked_timeseries) && (this->buffer_type <= atsfileout::atsfileout_stacked_calibrated_scaled_timeseries) ) {
            while (!this->read() ) {
                for (size_t i = 0; i < this->tsdata_stacked.size(); ++i) {
                    this->tsdata_stacked[i] += this->tsdata[i];
                }

            }
            for (auto &v : this->tsdata_stacked)  v /= double(this->ats_read_counts);
            this->tsbuffer_status = threadbuffer_status::finished;
            this->tsbuffer->deposit(this->tsdata_stacked, threadbuffer_status::finished);
        }

        else if( (this->buffer_type >= atsfileout::atsfileout_stacked_spectra) && (this->buffer_type <= atsfileout::atsfileout_stacked_amplitude_spectra) ) {
            this->fftreal->set_amplitude_spectra_stacked(this->spc_ampl_data_stacked);
            while (!this->read() ) {
                this->fftreal->process(this->tsdata, this->spcdata, fftreal_ctrl::detrend_hanning);
            }
            for (auto &v : this->spc_ampl_data_stacked)  v /= double(this->ats_read_counts);
            this->tsbuffer_status = threadbuffer_status::finished;
            this->spc_amplbuffer->deposit(this->spc_ampl_data_stacked, threadbuffer_status::finished);
        }

        else if( this->buffer_type == atsfileout::atsfileout_calibrated_spectra ) {
            if (QDataStream::Ok == this->read() ) {
                this->fftreal->process(this->tsdata, this->spcdata, fftreal_ctrl::detrend_hanning);
                if (this->calib != nullptr) {
                    auto c = this->calib->cap.cbegin();
                    auto f = this->calib->f.cbegin();
                    for (auto &s : this->spcdata) {
                        s /= (-1.0) * *c++;
                        s /= 1000.;
                        s /= *f++;

                    }
                }
                if (this->ats_read_counts < this->max_counts) {
                    this->tsbuffer_status = threadbuffer_status::running;
                    this->spcbuffer->deposit(this->spcdata, threadbuffer_status::running);
                }
                else {
                    this->tsbuffer_status = threadbuffer_status::finished;
                    this->spcbuffer->deposit(this->spcdata, threadbuffer_status::finished);

                }
                // debug input threads
                // qDebug() << "in" << this->ats_read_counts << "[" << this->slot << "]";

            }
            else {
                if (this->atm != nullptr) {
                    // we have early stop
                    qDebug() << "buffer early stop";

                    this->tsbuffer_status = threadbuffer_status::finished;
                    this->tsdata.resize(0);
                    this->spcdata.resize(0);
                    this->spcbuffer->deposit(this->spcdata, threadbuffer_status::terminated);
                    break;



                }
                else {


                    this->spcbuffer->terminate_deposit();
                    qDebug() << "buffer read error!, terminate threadbuffer!";
                    emit this->tx_adu_msg(ivalue("system_serial_number"), ivalue("channel_number"), this->slot, QString("buffer read error!, terminate threadbuffer!"));
                    this->tsbuffer_status = threadbuffer_status::finished;
                    break;
                }

            }
        }
    }

    // when the loop above has finished, no stacks and not MT mode (same size fft):
    if (this->last_read_size && (this->buffer_type < atsfileout::atsfileout_stacked_spectra)) {
        this->tsdata.resize(this->last_read_size);
        this->read();
        this->tsbuffer_status = threadbuffer_status::finished;
        this->tsbuffer->deposit(this->tsdata, threadbuffer_status::finished);
    }



    qDebug() << "atsfile finished, slot" << this->slot << "readings" << this->ats_read_counts;
    emit this->tx_adu_msg(ivalue("system_serial_number"), ivalue("channel_number"), this->slot, QString("atsfile::read_all finsihed, readings " + QString::number(this->ats_read_counts)));



}

void atsfile::read_all_single(const bool inverse_fft, const bool write_immedeately)
{
    if (this->can_read) {

        size_t samples_to_read = this->get_num_samples();
        this->tsdata.resize(samples_to_read);

        emit this->signal_guess_100(2);
        std::cerr << " read_all_single started" << std::endl;
        double factor = this->bin_atsheader.dblLSBMV;

        if (this->is_64bit_samples()) {
            qint64 s;
            if (this->is_scalable) {
                factor *= (1000.0 / this->dipole_length);
            }

            for (auto &ve : this->tsdata) {
                this->qds >> s;
                ve = s * factor;
            }
        }

        else {
            qint32 s;
            if (this->is_scalable) {
                factor *= (1000.0 / this->dipole_length);
            }

            for (auto &ve : this->tsdata) {
                this->qds >> s;
                ve = s * factor;
            }
        }


        if (this->qds.status()) {
            this->tsdata.resize(0);
        }
        emit this->signal_guess_100(10);

        if (!this->is_scalable && inverse_fft) {

            if (this->fftreal != nullptr) this->fftreal.reset();
            this->fftreal =  std::make_unique<fftreal_vector>();
            std::cerr << " read_all_single started prep fftreal" << std::endl;


            // detrend the input - otherwise we jump up or down when zeros are appended
            iter::simple_detrend<double>(this->tsdata.begin(), this->tsdata.end());

            // make sure that the last element is zero after detrend - this may be not perfectly the case
            // after detrending
            //            double final_offset = this->tsdata.back();
            //            for (auto &d : this->tsdata) {
            //                d -= final_offset;
            //            }

            this->tsdata.resize(this->fftreal->next_power_of_two(samples_to_read));  // check if zeros padded
            //&inbuf, &outbuf, freq = 0, bwincal = true, isdcfft = false, fcut_upper = 0, fcut_lower = 0);
            this->fftreal->set_buffer(this->tsdata, this->spcdata, this->get_sample_freq(), false, true, true, true, 0.0, 0.0);


            // this we must have also for theo only
            if (this->calib != nullptr) {
                // after reading the ats we know which chopper we have and want
                this->calib->slot_set_chopper(this->ivalue("chopper_status"));

                if (this->options.svalue("calibration") != "theo") {
                    std::cout << "read_all_single calibration data used " << QString::number(this->get_sample_freq()).toStdString() << " Hz" << std::endl;
                    if (this->ivalue("chopper_status") == 0) {
                        this->calib->interploate(this->fftreal->get_frequencies(), ivalue("channel_number"), cal::chopper_off, true, true, false ) ;
                    }
                    else {
                        this->calib->interploate(this->fftreal->get_frequencies(), ivalue("channel_number"), cal::chopper_on, true, true, false ) ;

                    }
                    this->calib->eliminate_except_complex();
                    //this->calib->prepend_dc_part_f0_to_complex_final();


                    auto f = this->calib->f.cbegin();
                    auto cbeg = this->calib->cap.begin();
                    auto cend = this->calib->cap.end();
                    cbeg++;
                    f++;
                    while (cbeg != cend) {
                        //cbeg++ /= (1000.0 * *f++);
                        *cbeg = 1.0/ (*cbeg * 1000.0 * *f++);
                        ++cbeg;

                    }
                    this->calib->cap[0] = std::complex<double>(0.0,0.0);

                    this->fftreal->set_calibration(this->calib->cap);
                }
                else {
                    std::cout << "read_all_single calibration theoretical used " << QString::number(this->get_sample_freq()).toStdString() << " Hz" << std::endl;
                    this->fftreal->set_external_calibration(0.0, 0.0, 1000., false, true);
                    QObject::connect(this->fftreal.get(), &fftreal_vector::signal_gen_pure_theo_cal, this->calib.get(), &calibration::slot_gen_pure_theo_cal, Qt::DirectConnection);
                }


                emit this->signal_guess_100(20);

                this->fftreal->process(this->tsdata, this->spcdata, fftreal_ctrl::nothing);
                this->calib.reset();
                this->fftreal.reset();
                this->spcdata.clear();

                this->tsdata.resize(samples_to_read);


                //std::reverse(data.begin(),data.end());
                //                double final_offset = this->tsdata.front();
                //                for (auto &d : this->tsdata) {
                //                    d -= final_offset;
                //                }

                //                emit this->signal_guess_100(25);

            }


        }
        emit this->signal_guess_100(30);
        std::cerr << " read_all_single finished" << std::endl;

        emit this->read_all_single_finished(this->tsdata, write_immedeately);
    }

    // that is 0 in case ok

    emit this->QDataStreamStatus(this->qds.status());

}

void atsfile::set_remote(bool is_remote)
{
    this->is_remote = is_remote;
}

bool atsfile::get_remote() const
{
    return this->is_remote;
}


bool atsfile::set_firfilter(std::shared_ptr<firfilter> xfirfil, const bool shift_to_full_second)
{
    this->firfil = std::make_unique<firfilter>(*xfirfil.get(), this);

    //this->firfil->copy_data(xfirfil.get());
    bool success;
    this->fir_fil_coeff = this->firfil->generate_coefficients(this->firfil->svalue("filter_name"), success, true, this->dvalue("sample_freq"), this->quint64value("num_samples"));
    if (!this->fir_fil_coeff.size()) {
        //this->firfil->clear();
        this->firfil.reset();
        this->firfil = nullptr;

        return false;
    }
    // we use our own coefficients
    qInfo() << "atsfile::set_firfilter -> loaded " << this->firfil->svalue("filter_name") ;
    this->firfil->clear_coefficients();
    this->tsdata.resize(this->fir_fil_coeff.size());
    this->move_back_after_read = this->tsdata.size() - this->firfil->uivalue("filter_move");
    if (this->firfil->svalue("filter_type").contains("decimation"))
        this->insert_double("output_sample_freq", double(this->dvalue("sample_freq") / this->firfil->dvalue("filter_move")));

    if (shift_to_full_second) {

        this->skip_samples_from_filter();

    }
    return true;
}

QDataStream::Status atsfile::skip_samples_from_filter()
{
    if (!this->firfil) {
        qDebug() << "atsfile::skip_samples_from_filter no filter active!";
    }
    // what eveer we do we have a full second


    // example for 128 Hz and 32 x filter
    // fracs = 1.8398s delay = 471 * 0.5 = 235 / 128.
    double fracs =  (((double) this->firfil->uivalue("filter_length" )) * 0.5) / ( this->dvalue("sample_freq") );
    double fulldelay = 0;
    // delay can be more than 1s

    // miss = 0.8398
    double miss =  modf (fracs , &fulldelay);

    //!<  @todo work around for fsample <= 1Hz use ceil like in older fast filter
    if (( this->dvalue("sample_freq") <= 1.001)) {
        this->set_key_size_t("add_secs_to_start_time", (size_t) (ceil(fracs)) + this->sztvalue("add_secs_to_start_time"));
    }
    else {
        this->set_key_size_t("add_secs_to_start_time", (size_t) (fulldelay) + this->sztvalue("add_secs_to_start_time"));

    }

    // 20 = 128 - (128 * 0.8398)              128                      -          107.5 -> 108
    size_t tmp_skip_first_samples =  (size_t) (this->dvalue("sample_freq") - miss * this->dvalue("sample_freq"));

    // miss is in the sub sample ?
    if (((tmp_skip_first_samples + 1) == (size_t) (this->dvalue("sample_freq"))) || ((tmp_skip_first_samples -1) == (size_t) (this->dvalue("sample_freq") )) ) {
        tmp_skip_first_samples = 0;
    }

    if (tmp_skip_first_samples) this->set_key_size_t("add_secs_to_start_time", 1 + this->sztvalue("add_secs_to_start_time"));

    // store for later usage
    //this->skip_first_samples_read += tmp_skip_first_samples;

    // 20 + 235 = 255 ---> 0.....255 = 2s

    if (this->file.isOpen()) {
        if (tmp_skip_first_samples) {
            this->old_filepos = this->file.pos();
            this->file.seek(this->old_filepos + tmp_skip_first_samples * sizeof(int32_t));
        }
    }
    else {
        qFatal("atsfile::skip_samples_from_filter;  ERROR file should be already OPEN");
        exit(1);
    }

    return this->qds.status();

}

bool atsfile::set_progress_bar_resolution(const unsigned int &one_to_twenty_percent)
{

    // no data yet, prepare
    if (!this->max_counts) {
        this->progress_bar_emits_at.resize((100/one_to_twenty_percent));
        return false;
    }

    // was set and will be used
    else if (this->max_counts && this->progress_bar_emits_at.size()){
        for (size_t i = 1; i < progress_bar_emits_at.size()+1; ++i) {
            this->progress_bar_emits_at[i-1] = ( i * this->max_counts / progress_bar_emits_at.size());
        }
        return true;
    }

    return false;

}

int atsfile::get_slot() const
{
    return this->slot;
}

void atsfile::set_slot(const int &slot)
{
    this->slot = slot;
}

void atsfile::set_ats_options(const prc_com &cmdline)
{
    atsheader::set_ats_options(cmdline);

    if (!this->options.svalue("scale").compare("auto")) this->scale_me = true;
    else this->scale_me = false;
}

size_t atsfile::queue_data_to_vector()
{
    if (!this->queue_data.size()) return 0;
    if (!this->samples_read) return 0;
    this->tsdata.reserve(this->samples_read);
    while (!this->queue_data.empty()) {
        this->tsdata.insert(this->tsdata.end(), this->queue_data.front().begin(), this->queue_data.front().end());
        this->queue_data.pop();
    }

    auto result = std::minmax_element(this->tsdata.begin(), this->tsdata.end());
    double min = (result.first - this->tsdata.begin());
    double max = (result.second - this->tsdata.begin());
    double lsb_mv;
    if (max > 0) lsb_mv = 2 * std::abs(max) / (INT_MAX);
    else   lsb_mv = 2 * abs(min) / (INT_MAX);
//    if (std::abs(min) > std::abs(max)) lsb_mv = std::abs(min) * 6 /  ((double)INT32_MAX);
//    else lsb_mv = std::abs(max) * 6 /  ((double)INT32_MAX);
    this->insert("lsb", double(lsb_mv));
    this->prc_com_to_classmembers();

    return this->tsdata.size();

}

size_t atsfile::get_override_use_samples() const
{
    return this->override_use_samples;
}






size_t atsfile::get_max_counts() const
{
    return this->max_counts;
}

size_t atsfile::get_sample_pos() const
{
    // this->headersize_buf + (sample_no * sizeof(qint32)) + this->filepos_offset
    if (this->is_64bit_samples()) {
        return (size_t) (this->file.pos() - this->headersize_buf) / sizeof(qint64);
    }

    return (size_t) (this->file.pos() - this->headersize_buf) / sizeof(qint32);

}

quint64 atsfile::set_write_double_vector(const std::vector<double> &complete_double_data, const double &lsb_mv)
{


    size_t result = 0;
    if (!this->can_write) {
        this->set_LSBMV(lsb_mv);
        this->set_key_value("num_samples", (uint) complete_double_data.size());
        result = this->write_header(false);

    }
    if (result == SIZE_MAX) {
        qFatal(" atsfile::write can not create header");
        exit(1);
    }

    //    size_t i = 0;
    //std::cerr << "atswriter[" << this->slot << "] writing" << std::endl;
    if (this->is_64bit_samples()) {
        for (auto &ve : complete_double_data) {
            this->qds << (qint64) (ve/this->bin_atsheader.dblLSBMV);
        }
    }
    else {
        for (auto &ve : complete_double_data) {
            //        if (i  == 10000) {
            //            qDebug() << (qint32) (ve/this->bin_atsheader.dblLSBMV) << ve << bin_atsheader.dblLSBMV;
            //            i = 0;
            //        }
            this->qds << (qint32) (ve/this->bin_atsheader.dblLSBMV);
            //        ++i;
        }
    }

    this->close();

    return this->scan_header_close();




}

quint64 atsfile::append_int32(const std::vector<int32_t> &v)
{
    if (this->is_64bit_samples()) return ULLONG_MAX;
    this->insert_szt("num_samples", qint64value("num_samples") + v.size());
    this->write_header(false);

    qint64 lpos = this->file.size();
    this->file.seek(lpos);

    for (const auto &i : v) {
        this->qds << i;
    }

    return this->get_num_samples();


}

quint64 atsfile::append_int64(const std::vector<int64_t> &v)
{
    if (!this->is_64bit_samples()) return ULLONG_MAX;
    this->insert_szt("num_samples", qint64value("num_samples") + v.size());
    this->write_header(false);

    qint64 lpos = this->file.size();
    this->file.seek(lpos);

    for (const auto &i : v) {
        this->qds << (qint64)i;
    }

    this->file.close();

    return this->get_num_samples();




}

quint64 atsfile::read_ascii_file(const QFileInfo &ascii_file, const bool isVolt, const bool is_64bit)
{
    if (!ascii_file.exists()) return 0;

    QFile file(ascii_file.absoluteFilePath());

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return 0;

    this->write_header(false);

    if (this->qds.status() != QDataStream::Ok) return 0;
    QTextStream qts(&file);
    QByteArray line;
    bool ok;
    double d;
    std::vector<double> data_in;
    data_in.reserve(10000000);

    while (!file.atEnd()) {
        line = file.readLine();
        d = line.toDouble(&ok);
        if (!ok) {
            file.close();
            return 0;
        }
        if (isVolt) d *= 1000.0;
        data_in.push_back(d);
    }
    auto result = std::minmax_element(data_in.begin(), data_in.end());
    double min = data_in[(result.first - data_in.begin())];
    double max = data_in[(result.second - data_in.begin())];
    if (!is_64bit) {
        if (std::abs(min) > std::abs(max)) this->bin_atsheader.dblLSBMV = std::abs(min) * 6 /  ((double)INT32_MAX);
        else this->bin_atsheader.dblLSBMV = std::abs(max) * 6 /  ((double)INT32_MAX);
        for (auto &d : data_in) {

            this->qds << qint32(d/this->bin_atsheader.dblLSBMV);
        }
        this->insert("bit_indicator", 0);


    }
    else {
        if (std::abs(min) > std::abs(max)) this->bin_atsheader.dblLSBMV = std::abs(min) * 6 /  ((double)INT64_MAX);
        else this->bin_atsheader.dblLSBMV = std::abs(max) * 6 /  ((double)INT64_MAX);
        for (auto &d : data_in) {

            this->qds << qint64(d/this->bin_atsheader.dblLSBMV);
        }
        this->insert("bit_indicator", 1);
        this->insert("header_version", qint16(81));
    }
    this->insert("lsb", double (this->bin_atsheader.dblLSBMV));
    this->set_key_value("num_samples", (uint) data_in.size());
    this->write_header(true);
    return this->scan_header_close();


}

void atsfile::get_tsdata(std::vector<double> &data) const
{
    data = this->tsdata;
}

quint64 atsfile::dump_tsbuffer(const QString &filename, const eQDateTime &startsat, const quint64 &current_sample_position, const bool timecol) const
{
    QFile data(filename);
    bool opensuccess = false;
    eQDateTime starts(startsat);
    quint64 i = 0;
    opensuccess = data.open(QFile::WriteOnly | QFile::Truncate | QFile::Text);
    if (!opensuccess) return 0;
    QTextStream out(&data);
    out.setRealNumberNotation(QTextStream::ScientificNotation);
    if (timecol) {
        for (size_t i = 0; i < this->tsdata.size(); ++i) {
            out << starts.ISO_date_time_frac(current_sample_position + i) << " " << this->tsdata[i] << Qt::endl;
        }
    }
    else {
        for (size_t i = 0; i < this->tsdata.size(); ++i) {
            out << this->tsdata[i] << Qt::endl;
        }
    }
    data.close();
    return i;

}

quint64 atsfile::dump_spc_tsplotter(const QString &filename) const
{
    if (!this->spc_ampl_data.size()) return 0;
    if (this->plotter_frequencies == nullptr) return 0;
    if (!this->plotter_frequencies->size()) return 0;
    if (this->spc_ampl_data.size() != this->plotter_frequencies->size()) return 0;
    QFile data(filename);
    bool opensuccess = false;

    quint64 i = 0;
    opensuccess = data.open(QFile::WriteOnly | QFile::Truncate | QFile::Text);
    if (!opensuccess) return 0;
    QTextStream out(&data);
    out.setRealNumberNotation(QTextStream::ScientificNotation);

    for (i = 0; i < this->spc_ampl_data.size(); ++i) {
        out << this->plotter_frequencies->at(i) << " " << this->spc_ampl_data[i] << Qt::endl;
    }

    data.close();
    return i;
}

quint64 atsfile::dump_spc_tsplotter_stacked(const QString &filename) const
{
    if (!this->spc_ampl_data_stacked.size()) return 0;
    if (this->plotter_frequencies == nullptr) return 0;
    if (!this->plotter_frequencies->size()) return 0;
    if (this->spc_ampl_data_stacked.size() != this->plotter_frequencies->size()) return 0;
    QFile data(filename);
    bool opensuccess = false;

    quint64 i = 0;
    opensuccess = data.open(QFile::WriteOnly | QFile::Truncate | QFile::Text);
    if (!opensuccess) return 0;
    QTextStream out(&data);
    out.setRealNumberNotation(QTextStream::ScientificNotation);

    for (i = 0; i < this->spc_ampl_data_stacked.size(); ++i) {
        out << this->plotter_frequencies->at(i) << " " << this->spc_ampl_data_stacked[i] << Qt::endl;
    }

    data.close();
    return i;
}


void atsfile::slot_ts_reader_mode(std::shared_ptr<std::vector<double>> plotter_frequencies, const atsfileout &tsplotter_buffer_type_ts, const atsfileout &tsplotter_buffer_type_spc,
                                  const bool &tsplotter_detrend_active)
{
    this->tsplotter_buffer_type_ts = tsplotter_buffer_type_ts;
    this->tsplotter_buffer_type_spc = tsplotter_buffer_type_spc;
    this->tsplotter_detrend_active = tsplotter_detrend_active;

    if (this->tsplotter_buffer_type_spc != atsfileout::atsfileout_void) {
        if (this->fftfull != nullptr) this->fftfull.reset();
        this->fftfull = std::make_unique<fftreal_vector>();

    }

    // check the settings

    // if we look at calibrated spectra, we compare with scaled calibrated timeseries
    if (this->tsplotter_buffer_type_spc == atsfileout::atsfileout_calibrated_spectra) {
        this->tsplotter_buffer_type_ts = atsfileout::atsfileout_calibrated_scaled_timeseries;



    }
    this->plotter_frequencies = plotter_frequencies;


    emit this->signal_ts_reader_mode(static_cast<atsfileout>(this->tsplotter_buffer_type_ts),
                                     static_cast<atsfileout>(this->tsplotter_buffer_type_spc),
                                     static_cast<atsfileout>(this->tsplotter_detrend_active));
}

void atsfile::close(const bool rewrite)
{
    if (this->ats_read_counts) {
        emit this->signal_loop_read_status(this->ats_read_counts, this->samples_read);
    }
    if (this->ats_write_counts) {
        emit this->signal_loop_write_status(this->ats_write_counts, this->samples_written);
    }

    atsheader::close(rewrite);

}


QDataStream::Status atsfile::slot_slide_to_time(const eQDateTime &edt_time, eQDateTime *corrected_time)
{

    quint64 slide_samples = this->edt_start.samples_to(edt_time, corrected_time);
    if (slide_samples) return this->slot_slide_to_sample(slide_samples);
    else return this->qds.status();

}

QDataStream::Status atsfile::sync_for_max_min_time(const eQDateTime &edt_time_starts_other, const eQDateTime &edt_time_stops_other)
{

    QDataStream::Status stat;
    this->edt_start.sync_samples_other(edt_time_starts_other, edt_time_stops_other);

    if (this->edt_start.get_use_samples() == 0) {
        qDebug() << "atsfile::sync_for_max_min_time FAILED";
        emit this->tx_adu_msg(ivalue("system_serial_number"), ivalue("channel_number"), this->slot,
                              "atsfile::sync_for_max_min_time -> FAILED " +  this->baseName());
        qDebug() << "samples to and end:" << edt_start.get_start_sample() << edt_start.get_use_samples();
        emit this->tx_adu_msg(ivalue("system_serial_number"), ivalue("channel_number"), this->slot,
                              "atsfile::sync_for_max_min_time -> samples to and end: " +
                              QString::number(edt_start.get_start_sample()) + " " +
                              QString::number(edt_start.get_use_samples()));


        return QDataStream::ReadPastEnd;
    }

    else {
        stat = this->slot_slide_to_sample(edt_start.get_start_sample());
        if (stat != QDataStream::Ok) {
            qDebug() << "atsfile::sync_for_max_min_time FAILED, can not slide to pos";
            emit this->tx_adu_msg(ivalue("system_serial_number"), ivalue("channel_number"), this->slot,
                                  "atsfile::sync_for_max_min_time -> FAILED " +  this->baseName() +
                                  ", can not slide to pos");

            return this->qds.status();
        }
    }



    // this is informal for MT
    //this->skip_first_samples_read = edt_start.get_start_sample();


    qDebug() << "atsfile::sync_for_max_min_time" << this->edt_start.ISO_date_time(edt_start.get_start_sample()) << "  <->  "
             << this->edt_start.ISO_date_time(edt_start.get_start_sample() +  this->edt_start.get_use_samples()) << "use samples:" << this->edt_start.get_use_samples();

    emit this->tx_adu_msg(ivalue("system_serial_number"), ivalue("channel_number"), this->slot,
                          "atsfile::sync_for_max_min_time ->  "
                          + this->edt_start.ISO_date_time(edt_start.get_start_sample()) + "  <->  "
                          + this->edt_start.ISO_date_time(edt_start.get_start_sample() + this->edt_start.get_use_samples())
                          + " use samples: " + QString::number(this->edt_start.get_use_samples()));

    this->override_use_samples = this->edt_start.get_use_samples();

    return stat;
}

QDataStream::Status atsfile::slot_slide_to_sample(const quint64 &sample_no)
{

    this->old_filepos = this->file.pos();

    if (sample_no == 0) {
        this->file.seek(this->headersize_buf + this->filepos_offset);
        this->set_key_size_t("add_secs_to_start_time", 0);
    }
    else {
        if (this->is_64bit_samples()) {
            this->file.seek(this->headersize_buf + (sample_no * sizeof(qint64)) + this->filepos_offset);
        }
        else {
            this->file.seek(this->headersize_buf + (sample_no * sizeof(qint32)) + this->filepos_offset);
        }
        this->set_key_size_t("add_secs_to_start_time", (((double)sample_no) / this->dvalue("sample_freq")));
    }
    // fails
    if (this->qds.status()) {
        emit this->QDataStreamStatus(this->qds.status());
        return this->qds.status();
    }
    else {

        emit this->slided();
    }
    return this->qds.status();
}



size_t atsfile::concat(std::shared_ptr<atsfile> &rhs)
{


    auto a = this->atsheader::read_header(true, true);
    auto b = rhs->atsheader::read_header(true, false);

    if (this->edt_stop > rhs->edt_start) return 0;


    ldiv_t divresult = ldiv( rhs->sztvalue("num_samples") , 524288 );
    size_t max_counts = size_t(divresult.quot);
    size_t last_read_size = size_t(divresult.rem);
    std::vector<double> data;

    std::unique_ptr<eQDateTime> true_time = std::make_unique<eQDateTime>();

    quint64 diff_samples = this->edt_stop.samples_to(rhs->get_start_datetime(), true_time.get());

    // todo evaluate true_time;

    if (diff_samples) {
        data.resize(diff_samples, 0.00);
        this->set_double_samples(data);

        signal_atm_exclude_tscat(size_t(this->get_num_samples()), size_t( size_t(this->get_num_samples()) + data.size()));
    }



    if (max_counts) {
        data.resize(524288);
        for (size_t i = 0; i < max_counts; ++i) {
            rhs->get_double_samples(data, true);
            if (this->is_scalable) {
                double dip1 =  this->get_dipole_length();
                double dip2 =  rhs->get_dipole_length();

                if (dip1 > 0.01) {
                    if ( std::fabs(dip1 - dip2) > 0.02) {
                        double e_scale = dip1 / dip2;
                        for (auto &dat : data) dat *= e_scale;
                    }
                }


            }
            this->set_double_samples(data);
        }

    }

    if (last_read_size) {
        data.resize(last_read_size);
        rhs->get_double_samples(data, true);
        this->set_double_samples(data);

    }

    this->write_header(true);

    return this->size();
}
