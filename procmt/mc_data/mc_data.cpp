#include "mc_data.h"


mc_data::mc_data(QObject *parent) : QObject(parent), prc_com(false)
{
    this->myname = "mt_data";

    this->fill_prc_com_with_base_values();

}

void mc_data::fill_prc_com_with_base_values()
{
    this->insert_double("auto_bandwidth", 0);
    this->insert_double("auto_parzen_radius", 0);
    this->insert_int("skip_marked_parts", 0);
    this->insert("target_frequencies", QString(""));  //!< can be used for auto_gen_tf_40 for example, contains a name and _ key
    this->insert_double("BaseFrequency", double(0.0));
    this->insert_szt("n_harmonics", size_t(0));
    this->insert("target_frequencies_table", QString("default_mt_frequencies"));  //! from info.sql3
    this->insert("info_db", QString("info.sql3"));  //! from info.sql3


}


void mc_data::set_messenger(std::shared_ptr<msg_logger> msg)
{
    this->msg = msg;
}

mc_data::~mc_data()
{

    this->close_infiles();
    this->close_outfiles();
    this->close_measdocs();
    this->close_ascii_outfiles();
    this->close_calibration();
    this->close_atsfile_collectors();

    for (auto &v : this->tsbuffers) {
        if (v != nullptr)  v.reset();
    }
    this->tsbuffers.clear();

    for (auto &v : this->spcbuffers) {
        if (v != nullptr)  v.reset();
    }
    this->spcbuffers.clear();


    if (this->firfil != nullptr) this->firfil.reset();
    //if (this->mtspc != nullptr) this->mtspc.reset();
    if (this->mxc != nullptr) this->mxc.reset();

    if (this->msg != nullptr) this->msg.reset();

    if (this->all_acspc_collector != nullptr) this->all_acspc_collector.reset();

    qDebug() << "delete mc_data";

}

bool mc_data::set_prc_com(std::shared_ptr<prc_com> mxc, const bool strip_classname, const bool check_options)
{
    if (mxc != nullptr) {
        this->mxc = mxc;
        prc_com::set_data(this->mxc->get_data(), strip_classname, check_options);
        return true;
    }
    return false;
}

size_t mc_data::open_files(const QList<QFileInfo> qfis, const bool slide_to_samepos_after_open, const mttype mt_type, const threadctrl tcl)
{

    size_t i;
    QString sender = QString("mc_data");
    QString component = ("open_files");
    QString message;

    QStringList basenames;
    if (!qfis.size()) {
        qWarning() << "mc_data::open_files "  << " -> no files slected";
        exit (0);
    }

    this->threadcontrol = tcl;
    this->mt_type = mt_type;

    if ( qfis.at(0).absoluteFilePath().endsWith(".xml", Qt::CaseInsensitive)) {
        QList<QDir> dirs;
        QStringList xmlatsfiles;
        for (auto &qfi : qfis ) {
            if (!qfi.exists()) {
                qDebug() << qfi.filePath() << "does not exist";
                qFatal("xml file does not exist");
            }
        }
        for (auto &qfi : qfis ) {
            if (qfi.absoluteFilePath().endsWith(".xml", Qt::CaseInsensitive)) {

                // Windows problem with special CHARACTERS
                this->measdocs.emplace_back(std::make_shared<measdocxml>(&qfi, nullptr, nullptr));
                dirs.append(qfi.absoluteDir());
                qDebug() << "append dir" << qfi.absolutePath();
            }
        }

        int s = 0;
        // use i for slot counter UNIQUE
        i = 0;
        for (auto &measdoc : measdocs) {
            QList<int> channel_nos;
            xmlatsfiles = measdoc->get_atsfiles(channel_nos);

            if (((s == 0) && xmlatsfiles.size()) || ((this->mt_type == mttype::nomt) && xmlatsfiles.size()) ) {
                for (auto xmlf : xmlatsfiles) {
                    message = "mc_data open";



                    QFileInfo qfi (dirs.at(s), xmlf);

                    //std::cout << dirs.at(s).absolutePath() << " " << xmlf.toStdString() << endl;
                    // if we have EMAP we always take Ex, Ey from the first XML only
                    if ( (this->mt_type == mttype::emap) || (this->mt_type == mttype::emap_rr) ) {

                        if ( xmlf.contains("_TEx") || xmlf.contains("_TEy")  ) {
                            this->in_atsfiles.emplace_back(std::make_shared<atsfile>(qfi, i++, short(0), this));
                        }
                    }
                    // else take all
                    else {
                        this->in_atsfiles.emplace_back(std::make_shared<atsfile>(qfi, i++, short(0), this));

                    }
                }
                xmlatsfiles.clear();
            }
            // now EMAP - use H from next XML
            if ( (s == 1) && xmlatsfiles.size() && ((this->mt_type == mttype::emap) || (this->mt_type == mttype::emap_rr))) {
                for (auto xmlf : xmlatsfiles) {
                    QFileInfo qfi (dirs.at(s), xmlf);
                    // we treat them aslocal and don't use a remote indicator
                    if ( xmlf.contains("_THx") || xmlf.contains("_THy") || xmlf.contains("_THz") ) {
                        this->in_atsfiles.emplace_back(std::make_shared<atsfile>(qfi, i++, short(0), this));
                    }

                }
                xmlatsfiles.clear();
            }

            // now we add remote after local and/or emap
            if ( ((s == 1) || (s == 2)) && xmlatsfiles.size() && ( ( this->mt_type == mttype::rr) || ( this->mt_type == mttype::emap_rr) ) ) {
                for (auto xmlf : xmlatsfiles) {
                    QFileInfo qfi (dirs.at(s), xmlf);
                    // we treat them with a remote indicator
                    if ( xmlf.contains("_THx") || xmlf.contains("_THy")  ) {
                        this->in_atsfiles.emplace_back(std::make_shared<atsfile>(qfi, i++, short(0), this));
                        this->in_atsfiles.back()->set_remote(true);
                    }

                }
                xmlatsfiles.clear();
            }

            if ( ((s == 1) || (s == 2)) && xmlatsfiles.size() && (  this->mt_type == mttype::csem)  ) {
                for (auto xmlf : xmlatsfiles) {
                    QFileInfo qfi (dirs.at(s), xmlf);
                    // we treat them with a remote indicator
                    if ( xmlf.contains("_THx") || xmlf.contains("_THy") || xmlf.contains("_THz") ) {
                        this->in_atsfiles.emplace_back(std::make_shared<atsfile>(qfi, i++, short(0), this));
                        this->in_atsfiles.back()->set_remote(true);
                    }

                }
                xmlatsfiles.clear();
            }

            ++s;
        }


    }

    else {
        for (int ii = 0; ii < qfis.size(); ++ii) {
            if (qfis.at(ii).absoluteFilePath().endsWith(".ats", Qt::CaseInsensitive)) {
                this->in_atsfiles.emplace_back(std::make_shared<atsfile>(qfis.at(ii), ii, short(0), this));

            }

        }
    }

    if (!this->in_atsfiles.size()) {
        this->close_infiles();
        this->close_outfiles();
        this->close_measdocs();

        return 0;
    }

    // check that we have the same frequencies


    // we need same starts and stops - the ats header emits the needed data
    if ((this->threadcontrol == threadctrl::threadctrl_continuous_join ) || slide_to_samepos_after_open)
        for (auto &ats : this->in_atsfiles) {
            connect(ats.get(), &atsheader::signal_sync_info, this, &mc_data::slot_sync_info);
        }

    for (auto &ats : this->in_atsfiles) {

        // again FIRST the options THEN the actions
        ats->set_ats_options(*this);



        connect(this, &mc_data::signal_survey_basedir, ats.get(), &atsheader::set_survey_basedir, Qt::DirectConnection);
        if (this->msg != nullptr) {
            ats->set_messenger(msg);
        }


    }

    size_t only_first = 0;
    for (auto &ats : this->in_atsfiles) {
        if (!only_first) this->check_survey_basedir(this->in_atsfiles.at(only_first++)->absolutePath());
        ats->read_header();                         // emits sync data, signal was conneted above
        basenames.append(ats->baseName());

    }

    double ftest = this->in_atsfiles.at(0)->get_sample_freq();
    bool cont = true;

    for (auto &qfi : this->in_atsfiles) {
        if (qfi->get_sample_freq() != ftest) cont = false;
    }

    if (!cont) {
        message = "files have different frequencies";
        emit this->signal_general_msg(sender, component, message);

        this->close_infiles();
        this->close_outfiles();
        this->close_measdocs();

        return 0;
    }



    // map the channel to remote in case
    // and prepare for vector and matrix storage
    for (auto &ats : this->in_atsfiles) {

        if (!ats->get_remote()) {
            ats->chan_type = size_t (pmt::channel_types.indexOf(ats->channel_type()));
        }
        else {
            ats->chan_type = 5 + size_t(pmt::channel_types.indexOf(ats->channel_type()));
        }
    }

    // this overrides the settings - nneded for parallel test; here we do use ats files only
    if (this->ivalue("force_second_set_remote") == 1) {
        this->force_second_half_remote();
    }


    if (this->files_not_synced != 0) {

        message = "files have no overlap in time";
        emit this->signal_general_msg(sender, component, message);
        this->close_infiles();
        this->close_outfiles();
        this->close_measdocs();

        return 0;

    }
    for (auto &ats : this->in_atsfiles) {
        // copy the prc_com from the ats files
        this->in_ats_prc_coms.push_back(prc_com(*ats));
    }
    this->sample_freq = this->in_atsfiles.at(0)->get_sample_freq();

    connect(this->in_atsfiles.back().get(), &atsfile::signal_get_max_counts_and_last, this, &mc_data::slot_set_max_counts_and_last, Qt::DirectConnection);

    // prepare progress bar
    //connect(this->in_atsfiles.back().get(), &atsfile::signal_counts_per, this, &mc_data::slot_counts_per, Qt::DirectConnection);
    connect(this->in_atsfiles.back().get(), &atsfile::signal_counts_per, this, &mc_data::signal_counts_per);
    this->in_atsfiles.back()->set_progress_bar_resolution(5);
    this->in_atsfiles.back()->set_extra_signals(true);



    // prepare sliding
    connect(this, &mc_data::signal_slide_to_sample, this->in_atsfiles.back().get(), &atsfile::slot_slide_to_sample, Qt::DirectConnection);


    if (slide_to_samepos_after_open) {
        qDebug() << "slided to same start time";

    }

    if (this->ivalue("skip_marked_parts") == 1) {
        for (auto &infiles : this->in_atsfiles) {
            infiles->open_atm(1);
            qDebug() << infiles->get_sample_pos() << infiles->get_override_use_samples();
        }
        this->atm_master.resize(this->in_atsfiles.at(0)->get_override_use_samples());
        for (auto &infiles : this->in_atsfiles) {
            this->atm_master = this->atm_master |= infiles->atm->get_slice(infiles->get_sample_pos(), infiles->get_sample_pos() + infiles->get_override_use_samples());
        }
        int has_any_slections = this->atm_master.count(true);

        if (has_any_slections) {
            for (auto &infiles : this->in_atsfiles) {
                infiles->atm->set_slice(this->atm_master, infiles->get_sample_pos(), infiles->get_sample_pos() + infiles->get_override_use_samples());
            }
        }
        else {
            for (auto &infiles : this->in_atsfiles) {
                infiles->atm.reset();
            }
        }
        this->atm_master.clear();

    }


    // connect if you want the info here
    emit files_open(basenames);
    return this->in_atsfiles.size();

}

void mc_data::force_second_half_remote()
{
    size_t half = this->in_atsfiles.size()/2;
    size_t i = 0;
    for (auto &ats : this->in_atsfiles) {

        if (i < half) {
            ats->set_remote(false);
            ats->chan_type = size_t (pmt::channel_types.indexOf(ats->channel_type()));
        }
        else {
            ats->set_remote(true);
            ats->chan_type = 5 + size_t(pmt::channel_types.indexOf(ats->channel_type()));
        }

        ++i;
    }
}

void mc_data::stack_all_tsplotter_start()
{
    if (!this->in_atsfiles.size()) return;
    int i = 0;
    for (auto &v : this->in_atsfiles) {
        this->spc_threads.emplace_back( std::thread (&atsfile::read_all_tsplotter, v) );
        qDebug() << "starting out channel thread tsplotter reader" << i++;

    }


}

void mc_data::stack_all_tsplotter_join()
{
    for (auto &v : this->spc_threads) {
        v.join();
    }

    this->spc_threads.clear();

    for (auto &v : this->in_atsfiles) {
        v->emit_stacked();
    }
}

void mc_data::make_auto_cross_spectra_parzen()
{

    if (!this->out_atsfile_collectors.size()) return;
    if (this->all_acspc_collector == nullptr) return;
    //    std::lock_guard<std::mutex> lock(this->mutex_acsp);

    //    this->flush_count++;
    //    qDebug() << " <<<<<<<<<<<<<<<<<<<<<<<<< GOT SIGN <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << this->flush_count;

    //    if (this->flush_count == this->out_atsfile_collectors.size()) {
    //        qDebug() << " <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< FLUSH <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<";
    //        this->flush_count = 0;
    //    }
    //    else return;

    std::unique_ptr<acsp_prz>                                   acsp_all;                //!< contains all auto and cross spectra w/wo parzen



    //    acsp_all = std::make_unique<acsp_prz>(this->out_atsfile_collectors, all_acspc_collector);
    //    if (this->mt_type >  mttype::nomt) {
    //        if (!acsp_all->set_parzen_vectors(this->parzen_vectors_sptr, this->in_atsfiles.at(0)->get_frequencies(), this->target_frequencies)) {
    //            qFatal("mc_data::make_auto_cross_spectra_parzen -> can not set parzen of target frequencies!" );
    //        }
    //        acsp_all->acsp();
    //    }

    acsp_all = std::make_unique<acsp_prz>(this->all_acspc_collector);
    if (this->mt_type >  mttype::nomt) {
        acsp_all->set_data_flush_out_atsfile_collectors(this->out_atsfile_collectors);
        if (!this->min_lines) {
            if (!acsp_all->set_parzen_vectors(this->parzen_vectors_sptr, this->in_atsfiles.at(0)->get_frequencies(), this->target_frequencies)) {
                qFatal("mc_data::make_auto_cross_spectra_parzen -> can not set parzen of target frequencies!" );
            }
        }
        if (this->min_lines) {
            if (!acsp_all->set_frequencies_and_target_indices(this->in_atsfiles.at(0)->get_frequencies(), this->target_frequencies, this->selindices, this->selindices_sizes)) {
                qFatal("mc_data::make_auto_cross_spectra_lines -> can not set lines of target frequencies!" );

            }
        }
        if (this->mt_type == mttype::rr) acsp_all->acsp();
        else        acsp_all->acsp();
    }
}

void mc_data::slot_apply_sys_calib(const uint on_1_off_0)
{
    for (auto &ats : this->in_atsfiles) {
        ats->slot_apply_sys_calib(on_1_off_0);
    }
}

//void mc_data::make_auto_cross_spectra_lines(std::shared_ptr<all_spectra_collector<std::complex<double> > > all_acspc_collector)
//{
//    if (!this->out_atsfile_collectors.size()) return;
//    std::unique_ptr<acsp_lines>                                   acsp_all;                //!< contains all auto and cross spectra w/wo parzen

//    acsp_all = std::make_unique<acsp_lines>(this->out_atsfile_collectors, all_acspc_collector);
//    if (this->mt_type >  mttype::nomt) {
//        if (!acsp_all->set_frequencies_and_target_indices(this->in_atsfiles.at(0)->get_frequencies(), this->target_frequencies, this->selindices, this->selindices_sizes)) {
//            qFatal("mc_data::make_auto_cross_spectra_lines -> can not set lines of target frequencies!" );

//        }
//        acsp_all->acsp();

//    }
//}


prc_com mc_data::scan_header_close(const QList<QFileInfo> qfis)
{

    prc_com first_header;
    QStringList xmlatsfiles;



    if (!qfis.size()) {
        qWarning() << "mc_data::open_files "  << " -> no files slected";
        return first_header;
    }

    if ( qfis.at(0).absoluteFilePath().endsWith(".xml", Qt::CaseInsensitive)) {
        measdocxml measdoc(&qfis.at(0));
        QString dir(qfis.at(0).absolutePath());
        QList<int> channel_nos;
        xmlatsfiles = measdoc.get_atsfiles(channel_nos);
        if (!xmlatsfiles.size()) {
            return first_header;
        }
        QFileInfo qfi (dir + "/" + xmlatsfiles.at(0));
        atsfile atsf(qfi, 0, this);
        atsf.scan_header_close();
        first_header.insert_data(atsf.get_data(), false);

    }


    else {

        atsfile atsf(qfis.at(0), 0, this);
        atsf.scan_header_close();

        first_header.insert_data(atsf.get_data(), false);

    }

    return first_header;

}

void mc_data::slot_counts_per(const int counts)
{
    std::cout << "mcd " << counts << std::endl;
    emit this->signal_counts_per(counts);
}

void mc_data::slot_guess_100(int value_0_100)
{
    std::cout << "mcd guess " << value_0_100 << std::endl;
    emit this->signal_guess_100(value_0_100);
    emit this->signal_repaint();

}

void mc_data::slot_rx_adu_msg(const int &system_serial_number, const int &channel, const int &slot, const QString &message)
{
    emit this->signal_fwd_rx_adu_msg(system_serial_number, channel, slot, message);
}

void mc_data::slot_rx_adu_msg_write_ascii(const int &system_serial_number, const int &channel, const int &slot, const QString &message)
{
    QString amessage = "read finsihed, flushing buffers";
    emit this->signal_read_ascii_finished(amessage);
}

int mc_data::slot_sync_info(const eQDateTime &timings)
{
    this->atstimings.append(timings);
    this->files_not_synced = 0;
    if ( size_t(this->atstimings.size()) == this->in_atsfiles.size() ) {

        QList<eQDateTime> stops;
        for (auto &st : this->atstimings) {
            stops.append(st.sample_time(ULLONG_MAX));
        }


        int i = 0;
        for (auto &st : this->atstimings) {
            qDebug() << "start stop " << st.ISO_date_time() << " <  >  " << stops.at(i).ISO_date_time();
            ++i;
        }

        std::sort(atstimings.begin(), atstimings.end());
        std::sort(stops.begin(), stops.end());

        qDebug() << atstimings.at(atstimings.size()-1).ISO_date_time();
        qDebug() << stops.at(0).ISO_date_time();


        QList<QDataStream::Status> stats;
        for (auto &ats : this->in_atsfiles) {
            stats.append(ats->sync_for_max_min_time(atstimings.at(atstimings.size()-1), stops.at(0)));
        }

        for (auto &stat : stats) {
            if (stat != QDataStream::Ok) {
                qDebug() << "can not find sync";
                ++this->files_not_synced;
            }
        }
    }
    return this->files_not_synced;
}

QList<QFileInfo>  mc_data::close_infiles()
{
    QList<QFileInfo> qfis;

    qDebug() << "mc_data closing in files ";

    if (!this->in_atsfiles.size()) return qfis;
    //    if (this->out_ats_ascii_files.size())
    for (auto &ats : this->in_atsfiles) {
        if (ats != nullptr) {
            ats->close();
            qfis.append(ats->get_qfile_info());
            ats.reset();
        }
    }
    this->in_atsfiles.clear();

    return qfis;
}

QList<QFileInfo> mc_data::close_measdocs()
{
    QList<QFileInfo> qfis;
    if (!this->measdocs.size()) return qfis;

    for (auto &xml : this->measdocs) {
        if (xml != nullptr) {
            qfis.append(xml->get_qfile_info());
            xml->close();
            xml.reset();
        }
    }

    measdocs.clear();

    return qfis;


}

void mc_data::slot_set_max_counts_and_last(const size_t &max_counts, const size_t &last_reading)
{
    emit this->signal_get_max_counts_and_last(max_counts, last_reading);
}


QList<QFileInfo> mc_data::close_outfiles()
{

    QList<QFileInfo> qfis;
    if (!this->out_atsfiles.size()) return qfis;
    qDebug() << "mc_data closing out files ";

    for (auto &ats : this->out_atsfiles) {
        if (ats != nullptr) {
            // we may have a new samples and XML have a new name
            // notify will not change the file name - as it would do
            // get_new_filename() because it makes a NEW filename, based on the decision if there are allready files
            // and would e.g. increase the run number
            ats->notify_xml();
            ats->close();
            qfis.append(ats->get_qfile_info());
            ats.reset();
        }
    }

    this->out_atsfiles.clear();

    for (auto &xml : this->measdocs) {
        if (xml != nullptr) {
            xml->save(nullptr);
            xml->close();
        }
    }

    return qfis;

}

void mc_data::close_ascii_outfiles()
{
    if (!this->out_ats_ascii_files.size()) return;
    for (auto &ats : this->out_ats_ascii_files) {
        ats->close();
        ats.reset();         // should destroy connections
    }
    this->out_ats_ascii_files.clear();


}

void mc_data::close_atsfile_collectors()
{
    if (!this->out_atsfile_collectors.size()) return;
    for (auto &ats : this->out_atsfile_collectors) {
        ats.reset();         // should destroy connections
    }
    this->out_atsfile_collectors.clear();

}

void mc_data::close_calibration()
{
    if (!this->calibrations.size()) return;
    for (auto &cal : this->calibrations) {
        cal.reset();
    }
    this->calibrations.clear();
}

void mc_data::slot_repaint()
{
    emit this->signal_repaint();
}

void mc_data::slot_set_GB_mem(int gigabytes)
{
    if (gigabytes > 1) this->has_gigybytes = gigabytes;
    else this->has_gigybytes = 1;
}

void mc_data::read_error(const QDataStream::Status QDataStreamStatus)
{

    // collect the signals
    ++this->accumulate_read_errors;
    if (this->accumulate_read_errors != (int(this->in_atsfiles.size() + this->out_atsfiles.size()) )) return;
    else this->accumulate_read_errors = 0;    // all files collected

    // want no error handling
    if (this->read_nothing0_close1_rewindlast2_on_read_at_eof_beof == 0) {
        emit this->read_tsdata_ready(QDataStream::Ok);
        return;
    }

    QDataStream::Status status;
    bool do_rewind = false;

    // rewind all to last pos in case one fails
    if ( read_nothing0_close1_rewindlast2_on_read_at_eof_beof == 2) {
        for (auto &ats : this->in_atsfiles) {
            status = ats->get_QDataStream_status();
            if (status == QDataStream::ReadPastEnd) {
                do_rewind = true;
            }
        }
        if (do_rewind) {
            for (auto &ats : this->in_atsfiles) {
                ats->slot_slide_to_old_pos();
            }
        }
        return;
    }


    qWarning() <<  "mc_data::read_error " << "read error";


    //!< @todo individual error handling

}

bool mc_data::set_window_length(const size_t &wl, const double &fcut_upper, const double &fcut_lower, const bool &last_read, const quint64 &use_samples, const quint64 &overlapping)
{
    // nothing to do
    if (!this->in_atsfiles.size()) return false;

    //!@todo try window length first
    // attach the files to the data

    for (auto &ats : this->in_atsfiles) {
        ats->set_window_length(wl, fcut_upper, fcut_lower, last_read, use_samples, overlapping);
    }


    return true;
}

bool mc_data::try_set_window_length(const size_t &wl, double &get_maxf, double &get_minf, double &fcut_upper, double &fcut_lower, const bool limit_upper_to_cal)
{
    if (!this->in_atsfiles.size()) return false;
    std::vector<double> minf(this->in_atsfiles.size());
    std::vector<double> maxf(this->in_atsfiles.size());
    std::vector<double> cutl(this->in_atsfiles.size());
    std::vector<double> cutu(this->in_atsfiles.size());
    QList<double> filter_result;

    size_t i = 0;
    for (auto &ats : this->in_atsfiles) {
        ats->min_max_cal(wl, maxf[i], minf[i], cutu[i], cutl[i], limit_upper_to_cal);
        ++i;
        QStringList filters = ats->get_filter();
        if (filters.contains("HF-HP-500Hz")) filter_result.append(500.0);
        if (filters.contains("HF-HP-1Hz")) filter_result.append(1.0);
    }
    get_minf = *std::max_element(minf.begin(), minf.end());
    get_maxf = *std::min_element(maxf.begin(), maxf.end());

    fcut_lower = *std::max_element(cutl.begin(), cutl.end());
    fcut_upper = *std::max_element(cutu.begin(), cutu.end());

    //    if (filter_result.size()) {
    //        double corner = *std::max_element(filter_result.begin(), filter_result.end());

    //        if (fcut_lower < corner * 2) fcut_lower = corner * 2;
    //    }


    if (get_maxf == get_minf) return false;
    return true;

}

size_t mc_data::get_window_length() const
{
    if (!this->in_atsfiles.size()) return 0;

    size_t i = 0;
    std::vector<size_t> sizes(this->in_atsfiles.size(),0);
    for (auto &ats : this->in_atsfiles) {
        sizes[i++] = ats->get_window_length();
    }

    for (i = 1; i < sizes.size(); ++i) {

        if (sizes.at(i) != sizes.at(0)) {
            qWarning() << "window length differs!!!";
            return 0;
        }
    }
    return sizes.at(0);
}

void mc_data::read_infiles(bool emits)
{
    emit read(emits);
    //this->in_atsfiles[0]->read();
}



void mc_data::start_in_ats_channel_threads()
{
    int i = 0;
    for (auto &v : this->in_atsfiles) {
        this->readerthreads.emplace_back( std::thread (&atsfile::read_all, v));
        qDebug() << "starting in channel thread" << i++;
    }
}

void mc_data::start_out_ats_channel_threads()
{
    int i = 0;
    for (auto &v : this->out_atsfiles) {
        this->writerthreads.emplace_back( std::thread (&atsfile::write, v) );
        qDebug() << "starting out channel thread" << i++;

    }
}

/*
void mc_data::start_mt_threads()
{


    // local spectra
    if (this->mtspc->buffer_size("ex")) this->mtthreads.emplace_back(std::thread (&mtspectra::fetch_ex, this->mtspc));
    if (this->mtspc->buffer_size("ey")) this->mtthreads.emplace_back(std::thread (&mtspectra::fetch_ey, this->mtspc));
    if (this->mtspc->buffer_size("hx")) this->mtthreads.emplace_back(std::thread (&mtspectra::fetch_hx, this->mtspc));
    if (this->mtspc->buffer_size("hy")) this->mtthreads.emplace_back(std::thread (&mtspectra::fetch_hy, this->mtspc));
    if (this->mtspc->buffer_size("hz")) this->mtthreads.emplace_back(std::thread (&mtspectra::fetch_hz, this->mtspc));

    // remote spectra
    if (this->mtspc->buffer_size("rhx")) this->mtthreads.emplace_back(std::thread (&mtspectra::fetch_rhx, this->mtspc));
    if (this->mtspc->buffer_size("rhy")) this->mtthreads.emplace_back(std::thread (&mtspectra::fetch_rhy, this->mtspc));

    this->mtthreads.emplace_back(std::thread (&mtspectra::ac_spectra, this->mtspc));

    qDebug() << "starting MT threads";

}
*/
void mc_data::start_collect_all_spc_threads()
{

    int i = 0;
    for (auto &v : this->out_atsfile_collectors) {
        this->spc_threads.emplace_back( std::thread (&atsfile_collector::collect_all_spc, v) );
        qDebug() << "starting out channel thread collect all spectra" << i++;

    }
}

void mc_data::join_collect_all_spc_threads()
{
    for (auto &v : this->spc_threads) {
        v.join();
    }
    qDebug() << "spc threads joined";
    this->spc_threads.clear();

    // we need to sort into the buckets; for auto cross spectra we don't do that
    if ((this->mt_type == mttype::csem) || (this->mt_type == mttype::nomt) ) return;

    qDebug() << "trim in case";
    std::vector<size_t> sizes;
    for (auto &outs : this->out_atsfile_collectors) {
        sizes.push_back(outs->spc.size());
    }
    std::sort(sizes.begin(), sizes.end());
    for (auto &outs : this->out_atsfile_collectors) {
        outs->spc.to_vector_vector(sizes.at(0));
    }
    qDebug() << "trimmed";


}

void mc_data::start_single_spectra_threads(const bool inverse_fft, const bool write_immedeately)
{

    size_t max_parallel_samples = 10000000;

    if (this->in_atsfiles.at(0)->get_ats_option("calibration") == "theo") max_parallel_samples = 40000000;

    if (has_gigybytes == 32) {
        max_parallel_samples *= 4;
        std::cout << "start_single_spectra_threads compute for 32 GB" << std::endl;
    }


    //    size_t max_parallel_samples = 100;
    //    if (theo_only) max_parallel_samples = 40;

    for (auto &v : this->in_atsfiles) {
        v->insert("mode", "single_spectra");

    }


    // can parallize ~ 50 MB/channel ?
    if (this->in_atsfiles.at(0)->get_num_samples() < max_parallel_samples) {


        for (auto &v : this->in_atsfiles) {
            this->out_atsfile_collectors.emplace_back(std::make_shared<atsfile_collector>(v));
        }

        auto w = this->out_atsfile_collectors.begin();
        for (auto &v : this->in_atsfiles) {
            connect(v.get(), &atsfile::read_all_single_finished, w->get(), &atsfile_collector::slot_get_all_ts, Qt::DirectConnection);
            ++w;
        }


        connect(this->in_atsfiles.back().get(), &atsfile::signal_guess_100, this, &mc_data::slot_guess_100, Qt::DirectConnection);
        connect(this->out_atsfile_collectors.back().get(), &atsfile_collector::signal_guess_100, this, &mc_data::slot_guess_100, Qt::DirectConnection);
        connect(this->out_atsfile_collectors.back().get(), &atsfile_collector::signal_repaint, this, &mc_data::slot_repaint, Qt::DirectConnection);


        QString msx = "starting parallel queue";
        emit this->signal_text_message(msx);
        emit this->signal_repaint();
        for (auto &v : this->in_atsfiles) {
            this->spc_threads.emplace_back(std::thread(&atsfile::read_all_single, v, inverse_fft, write_immedeately));
        }
        // can paralleize
        // return true;
    }

    else {
        QString sall = QString::number(this->in_atsfiles.size());

        int ns = 0;
        //auto out_atsfile_collectors_iter = this->out_atsfile_collectors.begin();
        for (auto &v : this->in_atsfiles) {
            auto out_atsfile_collectors_ptr = std::make_unique<atsfile_collector>(v);
            connect(v.get(), &atsfile::read_all_single_finished, out_atsfile_collectors_ptr.get(), &atsfile_collector::slot_get_all_ts, Qt::DirectConnection);
            connect(v.get(), &atsfile::signal_guess_100, this, &mc_data::slot_guess_100, Qt::DirectConnection);
            connect(out_atsfile_collectors_ptr.get(), &atsfile_collector::signal_guess_100, this, &mc_data::slot_guess_100, Qt::DirectConnection);
            //connect(out_atsfile_collectors_ptr.get(), &atsfile_collector::signal_repaint, this, &mc_data::slot_repaint, Qt::DirectConnection);



            ++ns;
            QString act = QString::number(ns);
            QString msx = "starting serial queue (large file) " + act + " / " + sall;
            emit this->signal_text_message(msx);
            emit this->signal_repaint();


            std::cerr << " huge file invert" << std::endl;
            auto thrd = std::thread(&atsfile::read_all_single, v, true, true);
            thrd.join();

            // free a.s.a.p all memory
            v->close();
            v.reset();
            out_atsfile_collectors_ptr.reset();
        }
    }

    //return false;
}

void mc_data::join_single_spectra_threads()
{
    for (auto &v : this->spc_threads) {
        v.join();
    }
    this->spc_threads.clear();
}

void mc_data::start_out_ats_ascii_channel_threads()
{
    int i = 0;
    for (auto &v : this->out_ats_ascii_files) {
        this->asciiwriterthreads.emplace_back( std::thread (&ats_ascii_file::write, v));
        qDebug() << "starting out ascii channel thread" << i++;
    }

}

void mc_data::join_in_ats_channel_threads()
{
    for (auto &v : this->readerthreads) {
        v.join();
    }
    this->readerthreads.clear();
}

void mc_data::join_out_ats_channel_threads()
{
    for (auto &v : this->writerthreads) {
        v.join();
    }
    this->writerthreads.clear();
}
/*
void mc_data::join_mtthreads()
{

    qDebug() << "joining mtthreads ....";
    for (auto &v : this->mtthreads) {
        v.join();
    }
    qDebug() << "mtthreads joined";
    // joins the sub threads (parzen threads) and release all buffers
    qDebug() << "joining parzen_threads ....";
    this->mtspc->join_parzen_threads();
    qDebug() << "parzen_threads joined";

}
*/
void mc_data::join_out_ats_ascii_channel_threads()
{
    emit signal_read_ascii_finished("flushing ascii data to disk");
    for (auto &v : this->asciiwriterthreads) {
        v.join();
    }
    this->asciiwriterthreads.clear();
}

void mc_data::slot_append_parzenvector(const double &f, const std::vector<double> &przv)
{
    if (f == 0.0 && !przv.size()) {
        this->parzen_vector_failed = true;
        return;
    }
    this->parzen_vector_failed = false;
    if (this->parzen_vectors_sptr == nullptr) this->parzen_vectors_sptr = std::make_shared<std::vector<std::vector<double>>>();

    this->parzen_vectors_sptr->emplace_back(przv);

}


void mc_data::prepare_ascii_outfiles(const size_t &nbuffers, const size_t &wl, const quint64 &use_samples, const atsfileout &buffer_type, const QString &file_extension)
{

    // the in files are already open here!
    size_t i = 0;

    for (auto &v : this->in_atsfiles) {
        this->out_ats_ascii_files.emplace_back(std::make_shared<ats_ascii_file>(v->get_qfile_info(), file_extension, i++, this));
    }



    for (auto &v : this->in_atsfiles) {
        v->set_window_length(wl, 0.0, 0.0, true, use_samples, 0, buffer_type);

    }

    for (auto &v : this->in_atsfiles) {
        this->tsbuffers.emplace_back(v->get_create_buffer(nbuffers, buffer_type));
    }

    i = 0;
    for (auto &v : this->out_ats_ascii_files) {

        v->set_buffer(this->tsbuffers[i]);
        if ((buffer_type >= atsfileout::atsfileout_stacked_spectra) && (buffer_type <= atsfileout::atsfileout_stacked_amplitude_spectra)) {
            v->set_frequencies(this->in_atsfiles[i]->get_frequencies());
        }
        ++i;
    }

    for (auto &v : this->out_ats_ascii_files) {
        v ->open();
    }

}



void mc_data::prepare_ats_outfiles(const size_t &nbuffers, const size_t &wl, const quint64 &use_samples, const atsfileout &buffer_type, const QString &file_extension)
{
    // the in files are already open here!
    size_t i = 0;

    if (this->measdocs.size()) {
        QStringList xmlatsfiles;
        int s;
        for (auto &measdoc : measdocs) {
            QList<int> channel_nos;
            xmlatsfiles = measdoc->get_atsfiles(channel_nos);
            s = 0;
            i = 0;
            for (auto xmlf : xmlatsfiles) {
                for (auto &v : this->in_atsfiles) {
                    if (v->fileName() == xmlf) {
                        this->out_atsfiles.emplace_back(std::make_shared<atsfile>(*v));
                        connect(this->out_atsfiles.back().get(), &atsheader::filename_changed, measdoc.get(), &measdocxml::slot_update_atswriter, Qt::DirectConnection);
                        connect(measdoc.get(), &measdocxml::signal_atswriter_data_qmap_created, this->out_atsfiles.back().get(), &atsheader::set_atswriter_section, Qt::DirectConnection );
                    }
                }

            }
            xmlatsfiles.clear();
            ++s;
        }

        for (auto &measdoc : measdocs) {
            measdoc->create_atswriter();
        }
    }
    else {


        for (auto &v : this->in_atsfiles) {
            this->out_atsfiles.emplace_back(std::make_shared<atsfile>(*v));
        }
    }

    for (auto &v : this->out_atsfiles) {

        // at this moment they have the same samples and so on
        //v->set_num_samples(use_samples);
        qDebug() << "init new out ats files " << v->get_new_filename();
    }

    for (auto &v : this->in_atsfiles) {
        v->set_window_length(wl, 0.0, 0.0, true, use_samples, 0, buffer_type);
    }

    for (auto &v : this->in_atsfiles) {
        this->tsbuffers.emplace_back(v->get_create_buffer(nbuffers, buffer_type));
    }

    i = 0;
    for (auto &v : this->out_atsfiles) {

        v->set_buffer(this->tsbuffers[i]);
        ++i;
    }


}

bool mc_data::prepare_mt_spectra(const size_t &nbuffers, const size_t &wl, const double &fcut_upper, const double &fcut_lower, const quint64 &use_samples,
                                 const quint64 &overlapping, const double &overlapping_factor, const double &parzen_radius, const size_t &min_lines,
                                 const int &force_frequency_list, const atsfileout &buffer_type, const bool limit_upper_to_cal)
{
    // in RR we need two, check
    // in emap we need two, check
    if (this->mt_type == mttype::csem) return false;
    size_t i;
    if (this->measdocs.size() || this->in_atsfiles.size()) {
        // if not in cmdline make 0.0
        if (parzen_radius == DBL_MAX) this->parzen_radius = 0;
        else this->parzen_radius = parzen_radius;
        this->wl = wl;
        this->overlapping = overlapping;
        this->overlapping_factor = overlapping_factor;

        if (min_lines && (min_lines != ULLONG_MAX)) {
            this->parzen_radius = 0;
            this->set_key_value("auto_parzen_radius", 0.0);
            this->min_lines = min_lines; // indicator
        }


        this->auto_bw_prz();

        // adjust the overlapping after auto in case

        if ((overlapping_factor != 0.0)  && !overlapping) {
            this->overlapping = quint64(this->wl * overlapping_factor);
        }
        else if (overlapping) {
            this->overlapping = overlapping;
        }

        double t_fcut_lower = fcut_lower;
        double t_fcut_upper = fcut_upper;
        double f_max, f_min;

        this->try_set_window_length(this->wl, f_max, f_min, t_fcut_upper, t_fcut_lower, limit_upper_to_cal);

        if ((t_fcut_lower + t_fcut_upper) > 0.9) return false;

        if (fcut_lower > t_fcut_lower) t_fcut_lower = fcut_lower;
        if (fcut_upper > t_fcut_upper) t_fcut_upper = fcut_upper;

        for (auto &v : in_atsfiles) {
            v->set_window_length(this->wl, t_fcut_upper, t_fcut_lower, false, use_samples, this->overlapping, buffer_type);

        }
        for (auto &v : this->in_atsfiles) {
            this->spcbuffers.emplace_back(v->get_create_cplx_buffer(nbuffers, buffer_type));
        }

        // that is for parallel test where we may NOT want to use MT frequency list
        if (force_frequency_list && !this->target_frequencies.size() && this->svalue("target_frequencies").contains("auto_gen_tf_")) {
            QString snum = this->svalue("target_frequencies").remove("auto_gen_tf_").simplified();
            size_t n = snum.toUInt();

            vector_f_select_n(this->in_atsfiles[0]->get_frequencies(), this->target_frequencies, n);
        }
        // fall back // that will send a signal to math vector AND SETS THE TARGET FREQUENCIES AGAIN!
        if (force_frequency_list && !this->target_frequencies.size()) {

            // here we emit either parzen or lines
            this->select_target_frequencies(this->svalue("info_db"), this->svalue("target_frequencies_table"));

        }
        if (this->parzen_vector_failed) return false;
        // missing CSAMT FREQUENCIES


        else if ((this->mt_type < mttype::emap_rr_full) && this->mt_type > mttype::nomt) {

            if (this->parzen_radius > 0.0001) {

                // that will send a signal to math vector AND SETS THE TARGET FREQUENCIES AGAIN!
                if (!this->target_frequencies.size()) this->select_target_frequencies(this->svalue("info_db"), this->svalue("target_frequencies_table"));
            }
            //            else {

            //                // if (this->acspectra_result_sptr == nullptr) this->acspectra_result_sptr = std::make_shared<acspectra_result_type >();
            //                // missing CSAMT FREQUENCIES
            //                for (auto &f : this->in_atsfiles.at(0)->get_freqs()) {
            //                    this->acspectra_result_sptr->emplace(f,std::queue<std::complex<double>>());
            //                }
            //                emit this->frequencies_selected(this->in_atsfiles.at(0)->get_freqs());
            //            }

        }



        for (auto &v : this->in_atsfiles) {
            v->set_ats_option("parzen_radius", this->parzen_radius);
            this->out_atsfile_collectors.emplace_back(std::make_shared<atsfile_collector>(v));
        }

        //        for (auto &v : this->out_atsfile_collectors) {
        //            QObject::connect(v.get(), &atsfile_collector::signal_flush_collector, this, &mc_data::make_auto_cross_spectra_parzen, Qt::DirectConnection);
        //        }
        // for mt we first make auto- cross and then parzening; but we need the parzen freqs and vectors later
        if (this->mt_type >  mttype::nomt) {
            for (auto &v : this->out_atsfile_collectors) {
                if (this->min_lines &&(this->selindices_sizes.size() == this->target_frequencies.size())) {
                    v->set_frequencies_and_target_indices(this->in_atsfiles.at(0)->get_frequencies(), this->target_frequencies, this->selindices, this->selindices_sizes);
                    v->set_all_spectra_collector(this->all_acspc_collector);

                }
                else {
                    v->set_frequencies_and_target(this->in_atsfiles.at(0)->get_frequencies(), this->in_atsfiles.at(0)->get_frequencies(), true);

                }
            }
        }
        else {
            for (auto &v : this->out_atsfile_collectors) {
                if (this->target_frequencies.size()) {
                    if (this->parzen_vector_failed) return false;
                    if (this->parzen_vectors_sptr != nullptr) {
                        if (this->target_frequencies.size() == this->parzen_vectors_sptr->size()) {
                            v->set_parzen_vectors(this->parzen_vectors_sptr, this->in_atsfiles.at(0)->get_frequencies(), this->target_frequencies);
                        }
                        else {
                            qFatal("prepare_mt_spectra ->parzen vectors not same amount as target frequencies");
                        }
                    }
                    else {
                        v->set_frequencies_and_target(this->in_atsfiles.at(0)->get_frequencies(), this->target_frequencies, false);

                    }
                }
                else {
                    v->set_frequencies_and_target(this->in_atsfiles.at(0)->get_frequencies(), this->in_atsfiles.at(0)->get_frequencies(), true);

                }
                // v->set_all_spectra_collector(asp_coll);
            }
        }
        i = 0;
        for (auto &v : this->out_atsfile_collectors) {

            v->set_complex_buffer(this->spcbuffers[i++]);
        }


        std::vector<double> fs(this->in_atsfiles.at(0)->get_frequencies());
        emit this->signal_general_msg("mc_data", "mt spectra prep", "wl " + QString::number(this->wl));
        emit this->signal_general_msg("mc_data", "mt spectra prep", "parzen " + QString::number(this->parzen_radius));
        emit this->signal_general_msg("mc_data", "mt spectra prep", "fcut upper " + QString::number(t_fcut_upper));
        emit this->signal_general_msg("mc_data", "mt spectra prep", "fcut lower " + QString::number(t_fcut_lower));
        emit this->signal_general_msg("mc_data", "mt spectra prep", "overlapping " + QString::number(this->overlapping));


        emit this->signal_general_msg("mc_data", "selected freqs", (QString::number(fs[0], 'g', 6) + "Hz  " + QString::number((1./fs[0]), 'g', 6) + "s"));
        emit this->signal_general_msg("mc_data", "selected freqs", (QString::number(fs[1], 'g', 6) + "Hz  " + QString::number((1./fs[0]), 'g', 6) + "s"));
        emit this->signal_general_msg("mc_data", "selected freqs", "...");
        emit this->signal_general_msg("mc_data", "selected freqs", (QString::number(fs[fs.size()-2], 'g', 6) + "Hz  " + QString::number((1./fs[fs.size()-2]), 'g', 6) + "s"));
        emit this->signal_general_msg("mc_data", "selected freqs", (QString::number(fs[fs.size()-1], 'g', 6) + "Hz  " + QString::number((1./fs[fs.size()-1]), 'g', 6) + "s"));


        return true;
    }
    return false;
}

bool mc_data::prepare_csem_spectra(const size_t &nbuffers, const size_t &wl, const double &fcut_upper, const double &fcut_lower, const atsfileout &buffer_type, const bool limit_upper_to_cal)
{
    if (!this->measdocs.size()) return false;
    if (this->mt_type != mttype::csem) return false;

    this->wl = wl;
    size_t i;

    measdocs[0]->create_txm(false);
    connect(measdocs[0].get(), &measdocxml::signal_txm_cycle_qmap_created, this, &mc_data::slot_get_txm_cycle_qmap_created);
    measdocs[0]->get_txm(true);

    if (this->dvalue("BaseFrequency") == 0) return false;

    double t_fcut_lower = fcut_lower;
    double t_fcut_upper = fcut_upper;
    double f_max, f_min;

    this->try_set_window_length(this->wl, f_max, f_min, t_fcut_upper, t_fcut_lower, limit_upper_to_cal);
    std::vector<double> tmpfr;
    tmpfr = create_nharmonics<double>(this->dvalue("BaseFrequency"), this->sztvalue("n_harmonics"), true);

    for (auto &f : tmpfr) {
        if ((f < f_max) && (f > f_min)) this->target_frequencies.push_back(f);
    }

    // try calc overlapping auto
    for (auto &v : in_atsfiles) {
        v->set_window_length(this->wl, t_fcut_upper, t_fcut_lower, false, 0, 0, buffer_type);
        this->spcbuffers.emplace_back(v->get_create_cplx_buffer(nbuffers, buffer_type));

    }
    for (auto &v : this->in_atsfiles) {
        this->out_atsfile_collectors.emplace_back(std::make_shared<atsfile_collector>(v));
    }


    i = 0;
    for (auto &v : this->out_atsfile_collectors) {

        v->set_frequencies_and_target(this->in_atsfiles.at(0)->get_frequencies(), this->target_frequencies, false);
        v->set_complex_buffer(this->spcbuffers[i++]);

    }


    emit this->signal_general_msg("mc_data", "mt spectra prep", "wl " + QString::number(this->wl));
    emit this->signal_general_msg("mc_data", "mt spectra prep", "fcut upper " + QString::number(t_fcut_upper));
    emit this->signal_general_msg("mc_data", "mt spectra prep", "fcut lower " + QString::number(t_fcut_lower));
    emit this->signal_general_msg("mc_data", "mt spectra prep", "overlapping " + QString::number(this->overlapping));

    return true;

}

void mc_data::slot_set_target_frequencies(const std::vector<double> &flist)
{
    this->target_frequencies = flist;
}

void mc_data::slot_frequencies_fft_indices_selected(const std::vector<size_t> &selindices, const std::vector<size_t> &selindices_sizes)
{
    this->selindices = selindices;
    this->selindices_sizes = selindices_sizes;
}


void mc_data::auto_bw_prz()
{
    double bw = this->dvalue("auto_bandwidth");
    double prz = this->dvalue("auto_parzen_radius");
    size_t xwl = this->wl;

    if ((bw == 0.0) && (prz == 0.0)) return;

    if (bw != 0.0) {
        xwl = size_t((this->sample_freq / bw));
        // check that we not get 127 -> and correct to 128 in case
        xwl = this->next_power_of_two(xwl);
        // lower limits - otherwise fft is not useful
        if (xwl < 1024) xwl = 1024;
        //        if ( this->sample_freq > 4095) {
        //            if (xwl < 512) xwl = 1024;
        //        }
        //        else if ( this->sample_freq > 255) {
        //            if (xwl < 128) xwl = 512;
        //        }
        //        else {
        //            if (xwl < 256) xwl = 256;
        //        }

        //        else if ( this->sample_freq > 7.8) {
        //            if (xwl < 256) xwl = 256;
        //        }
        //        // so 128 is the smallest window we take
        //        else {
        //            if (xwl < 128) xwl = 128;
        //        }
        this->wl = xwl;
    }

    if (prz != 0.0) {

        if ( this->sample_freq > 65535) {
            this->parzen_radius = 0.01 * prz;
        }
        if ( this->sample_freq > 4095) {
            this->parzen_radius = 0.1 * prz;
        }
        else if ( this->sample_freq > 255) {
            this->parzen_radius = 0.15 * prz;
        }
        else if ( this->sample_freq > 127) {
            this->parzen_radius = 0.1 * prz;
        }
        else if ( this->sample_freq > 7.8) {
            this->parzen_radius = 0.1 * prz;
        }
        else {
            this->parzen_radius = 0.1 * prz;
        }
        // limit parzen

        if (this->parzen_radius > 0.3) {
            this->parzen_radius = 0.3;
        }
        if (this->parzen_radius < 0.0003) {
            this->parzen_radius = 0.0003;
        }
    }

    // for the logger
    emit this->signal_general_msg_nums("mc_data set parzen, radius:", this->parzen_radius, "window length:", uint(this->wl));
}

size_t mc_data::next_power_of_two(const size_t n)
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

void mc_data::slide_to_same_pos()
{

    // emit slide_to_second(45);
}

void mc_data::insert_prc_com(std::shared_ptr<prc_com> cmdline)
{
    this->insert_data(cmdline->get_data(), false);
}

bool mc_data::prepare_firfilter()
{
    if (this->svalue("filter_name") != "empty" && this->svalue("filter_name").size()) {
        if (this->in_atsfiles.size()) {

            bool success;
            this->firfil = std::make_shared<firfilter>(this);
            if (this->firfil->set_firfilter(*this)) {
                this->firfil->generate_coefficients(this->svalue("filter_name"), success, false, this->in_atsfiles.at(0)->get_sample_freq(), this->in_atsfiles.at(0)->get_num_samples() );
                if (!success) return false;
                for (auto &v : this->in_atsfiles) {
                    v->set_firfilter(this->firfil);
                }
                this->firfil.reset();
                return true;
            }
            else {
                qInfo() << "mc_data::prepare_firfilter -> filter not found / loaded";
                return false;
            }
        }
        else {
            qInfo() << "mc_data::prepare_firfilter -> no in ats files loaded yet";
            return false;

        }
        qInfo() << "mc_data::prepare_firfilter -> loaded FIR filter" << this->svalue("filter_name");
        return true;
    }

    else {
        qInfo() << "mc_data::prepare_firfilter -> could not load FIR filter";
    }
    return false;
}

int mc_data::select_target_frequencies(const QString &dbname, const QString &table_name)
{
    this->dbmcd = QSqlDatabase::addDatabase("QSQLITE", "get_mt_frequencies");
    QStringList strfreqs;
    this->dbmcd.setConnectOptions("QSQLITE_OPEN_READONLY");
    this->dbmcd.setDatabaseName(fall_back_default_str(dbname).absoluteFilePath());
    if (this->dbmcd.open()) {
        QSqlQuery query(this->dbmcd);
        QString query_str("select " + table_name + ".frequencies from " + table_name + ";");
        if( query.exec(query_str) ) {
            while (query.next()) {
                strfreqs << query.value(0).toString();
            }
        }
        else qDebug() << "db err: could not get frequencies for processing" ;

    }

    if (!strfreqs.size()){
        qDebug() << "mc_data::select_target_frequencies db err: NULL frequencies in table for processing" ;
        qFatal("mc_data::select_target_frequencies bye");
    }

    if (strfreqs.size()) {
        this->target_frequencies.reserve(strfreqs.size());
        for ( auto &str : strfreqs) {
            this->target_frequencies.emplace_back(str.toDouble());
        }
    }

    if (this->in_atsfiles.size() && this->target_frequencies.size()) {

        if (!(this->in_atsfiles.at(0)->get_frequencies().size())) {
            qFatal("mc_data::select_target_frequencies FFT not initialized yet");
        }

        if (!(this->target_frequencies.size())) {
            qFatal("mc_data::select_target_frequencies no target frequencies found ... info.sql3 missing?");
        }

        sort(this->target_frequencies.begin(), this->target_frequencies.end());

        if (this->min_lines) {
            emit this->signal_got_freq_targets_lines(this->in_atsfiles[0]->get_frequencies(), this->target_frequencies, this->min_lines, 0);
        }
        else {
            // if this not called no parzening takes place
            emit this->signal_got_freq_targets(this->in_atsfiles.at(0)->get_frequencies(), this->target_frequencies, this->parzen_radius);
        }
    }

    return (int)this->target_frequencies.size();
}

void mc_data::check_survey_basedir(const QDir from_atsfile)
{

    int pos = from_atsfile.absolutePath().indexOf("/ts/");
    if ( pos > 0) {
        QDir bas(from_atsfile.absolutePath().left(pos));
        QStringList dirss = bas.entryList(QDir::AllDirs);
        QStringList test(pmt::survey_dirs_minimum);
        int i = 0;
        for (auto &str : dirss) {
            for (auto sx : test) {
                if (!str.compare(sx)) {
                    ++i;
                }
            }
        }
        if (i == test.size()) {
            this->survey_base_dir.setPath(from_atsfile.absolutePath().left(pos));
        }



        // check cal edi dump
    }

    emit this->signal_survey_basedir(this->survey_base_dir);

}

void mc_data::slot_get_txm_cycle_qmap_created(const QMap<QString, QVariant> &data_map)
{
    this->insert_double("BaseFrequency",data_map.value("BaseFrequency"));
}
