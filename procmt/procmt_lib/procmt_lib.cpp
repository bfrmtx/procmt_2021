#include "procmt_lib.h"


procmt_lib::procmt_lib(std::shared_ptr<prc_com> cmdline, std::shared_ptr<msg_logger> msg, std::shared_ptr<mt_site> mtsite, QObject *parent) : QObject(parent), prc_com(false)
{
    this->cmdline = cmdline;
    this->msg = msg;
    this->mtsite = mtsite;

}


procmt_lib::~procmt_lib()
{

    this->msg.reset();
    this->cmdline.reset();
    this->mtsite.reset();
}

void procmt_lib::set_data(const QStringList &allmeasdocs_center, const QStringList &allmeasdocs_emap, const QStringList &allmeasdocs_rr, const mttype &my_mttype, const proc_type &my_proc_type)
{

    this->qfis_center.clear();
    this->qfis_emap.clear();
    this->qfis_rr.clear();
    for (auto &f : allmeasdocs_center) {
        qfis_center.append(QFileInfo(f));
    }
    for (auto &f : allmeasdocs_emap) {
        qfis_emap.append(QFileInfo(f));
    }
    // that can also be CSEM
    for (auto &f : allmeasdocs_rr) {
        qfis_rr.append(QFileInfo(f));
    }
    this->my_mttype          = my_mttype;
    this->myproc_type       = my_proc_type;
}

void procmt_lib::slot_update_progress_bar(const int counts)
{
    emit this->signal_update_progress_bar(counts);
}

void procmt_lib::slot_set_max_counts_and_last(const size_t &max_counts, const size_t &last_reading)
{
    emit this->signal_get_max_counts_and_last(max_counts, last_reading);
}

void procmt_lib::run()

{
    if (parent() == Q_NULLPTR) return;


    // check the sample frequencies!
    // in mcd check start / stopsignal_general_msg



    int ifiles = 1;
    int inner_loop_counter = 0;

    if (!this->qfis_center.size()) {

        emit this->signal_mtdata_finished("NO files to processes");

        return;
    }

    if ((my_mttype == mttype::emap) || (my_mttype == mttype::emap_rr)) {
        qDebug() << "try EMAP";
        if (this->qfis_center.size() != this->qfis_emap.size()) {
            std::cerr << "eamp and main staion have different recordings, exit" << std::endl;
            emit this->signal_mtdata_finished("eamp and main staion have different recordings, exit");
            return;
        }
    }
    if ((my_mttype == mttype::rr) || (my_mttype == mttype::emap_rr)) {
        qDebug() << "try RR / RR EMAP";
        if (this->qfis_center.size() != qfis_rr.size()) {
            std::cerr << "rr and main staion have different recordings, exit" << std::endl;
            emit this->signal_mtdata_finished("rr and main staion have different recordings, exit");
            return;
        }
    }

    if (my_mttype == mttype::csem) {
        qDebug() << "try CSEM / CSAMT";
        if (this->qfis_center.size() != qfis_rr.size()) {
            std::cerr << "CSEM/CSAMT and main staion have different recordings, exit" << std::endl;
            emit this->signal_mtdata_finished("CSEM/CSAMT and main staion have different recordings, exit");
            return;
        }
    }

    std::vector<prc_com> prcs;
    QMultiMap<QString, qint64> poss;


    // collect all ExEx  ExHy... RHzRHz
    std::shared_ptr<all_spectra_collector<std::complex<double>>>  all_acspc_collector = std::make_shared<all_spectra_collector<std::complex<double>>>(true);

    for (auto &qfixml : this->qfis_center) {
        QList<QFileInfo> qfiXML;
        if ((my_mttype == mttype::emap) || (my_mttype == mttype::emap_rr)) qfiXML.append(this->qfis_emap.at(this->iqfis_emap++));
        qfiXML.append(qfixml);
        if ((my_mttype == mttype::rr) || (my_mttype == mttype::emap_rr)) qfiXML.append(this->qfis_rr.at(this->iqfis_rr++));

        if (my_mttype == mttype::csem) qfiXML.append(this->qfis_rr.at(this->iqfis_rr++));

        std::shared_ptr<mc_data> mxcd = std::make_shared<mc_data>();


        // do this first before we run anything
        mxcd->insert_prc_com(cmdline);
        mxcd->set_messenger(msg);
        mxcd->all_acspc_collector = all_acspc_collector;


        // check for chrash here
        QObject::connect(mxcd.get(), &mc_data::signal_get_max_counts_and_last, this, &procmt_lib::slot_set_max_counts_and_last, Qt::DirectConnection );
        QObject::connect(mxcd.get(),&mc_data::signal_counts_per, this, &procmt_lib::slot_update_progress_bar, Qt::DirectConnection);


        QObject::connect(mxcd.get(), &mc_data::signal_general_msg, msg.get(), &msg_logger::slot_general_msg, Qt::DirectConnection);
        QObject::connect(mxcd.get(), &mc_data::signal_general_msg_nums, msg.get(), &msg_logger::slot_general_msg_nums, Qt::DirectConnection);



        std::shared_ptr<math_vector> mathvector = std::make_shared<math_vector>();

        QObject::connect(mxcd.get(), &mc_data::signal_got_freq_targets, mathvector.get(), &math_vector::slot_parzen_vector, Qt::DirectConnection);
        QObject::connect(mxcd.get(), &mc_data::signal_got_freq_targets_lines, mathvector.get(), &math_vector::take_five, Qt::DirectConnection);
        QObject::connect(mathvector.get(), &math_vector::parzenvector_created, mxcd.get(), &mc_data::slot_append_parzenvector, Qt::DirectConnection);
        QObject::connect(mathvector.get(), &math_vector::frequencies_selected, mxcd.get(), &::mc_data::slot_set_target_frequencies, Qt::DirectConnection);
        QObject::connect(mathvector.get(), &math_vector::frequencies_fft_indices_selected, mxcd.get(), &::mc_data::slot_frequencies_fft_indices_selected, Qt::DirectConnection);

        QObject::connect(mathvector.get(), &math_vector::parzen_radius_changed, msg.get(), &msg_logger::parzen_radius_changed, Qt::DirectConnection);

        if(mxcd->open_files(qfiXML, true, my_mttype, threadctrl::threadctrl_continuous_join)) {

            if (!cmdline->contains("parzen_radius")) cmdline->insert_double("parzen_radius", 0);
            if (!cmdline->contains("min_lines")) cmdline->insert_szt("min_lines", 0);

            // try csem
            if (mxcd->prepare_csem_spectra(cmdline->sztvalue("nbuffers"), cmdline->sztvalue("wl"), cmdline->dvalue("fcut_upper"), cmdline->dvalue("fcut_lower"),
                                           atsfileout::atsfileout_calibrated_spectra, true)) {
                emit this->signal_file_progess("file " + QString::number(ifiles++) + " / " + QString::number(this->qfis_center.size()));

                mxcd->start_in_ats_channel_threads();
                mxcd->start_collect_all_spc_threads();
                mxcd->join_in_ats_channel_threads();
                mxcd->join_collect_all_spc_threads();

                //all_channel_collector
                qDebug() << "CSEM spectra collected";

                for (auto &ssp : mxcd->out_atsfile_collectors) {
                    all_acspc_collector->data[ssp->spc.chan_type].append_clear_data(ssp->spc);
                }

                if (!inner_loop_counter) {
                    prcs = mxcd->in_ats_prc_coms;
                    for (auto &prc : mxcd->in_ats_prc_coms) {
                        poss.insert(prc.svalue("channel_type") + "_lat", prc.value("latitude").toLongLong());
                        poss.insert(prc.svalue("channel_type") + "_lon", prc.value("longitude").toLongLong());
                        poss.insert(prc.svalue("channel_type") + "_elev", prc.value("elevation").toLongLong());
                    }
                }
                else {
                    if (prcs.size() == mxcd->in_ats_prc_coms.size()) {
                        size_t iii = 0;
                        for (auto &prc : prcs) {
                            for (auto &inprc : mxcd->in_ats_prc_coms) {
                                if (prc.svalue("channel_type") == inprc.svalue("channel_type")) {
                                    if (prc.qint64value("start_date_time") > inprc.qint64value("start_date_time")) prc.set_key_value("start_date_time", inprc.qint64value("start_date_time"));
                                    if (prc.qint64value("stop_date_time") < inprc.qint64value("stop_date_time")) prc.set_key_value("stop_date_time", inprc.qint64value("stop_date_time"));

                                }
                            }
                        }
                        for (auto &prc : mxcd->in_ats_prc_coms) {
                            poss.insert(prc.svalue("channel_type") + "_lat", prc.value("latitude").toLongLong());
                            poss.insert(prc.svalue("channel_type") + "_lon", prc.value("longitude").toLongLong());
                            poss.insert(prc.svalue("channel_type") + "_elev", prc.value("elevation").toLongLong());
                        }

                    }

                }
                mxcd.reset();
                ++inner_loop_counter;

            }
            // try csamt

            // try else mt

            else if (mxcd->prepare_mt_spectra(cmdline->sztvalue("nbuffers"), cmdline->sztvalue("wl"), cmdline->dvalue("fcut_upper"), cmdline->dvalue("fcut_lower"),
                                              0, 0, cmdline->dvalue("overlapping_factor"), cmdline->dvalue("parzen_radius"), cmdline->sztvalue("min_lines"),
                                              cmdline->ivalue("force_frequency_list"), atsfileout::atsfileout_calibrated_spectra, true)) {


                emit this->signal_file_progess("file " + QString::number(ifiles++) + " / " + QString::number(this->qfis_center.size()));


                mxcd->start_in_ats_channel_threads();
                mxcd->start_collect_all_spc_threads();
                mxcd->join_in_ats_channel_threads();
                mxcd->join_collect_all_spc_threads();

                qDebug() << "spectra collected";
                if (cmdline->sztvalue("min_lines") == 0) mxcd->make_auto_cross_spectra_parzen();


                mathvector.reset();
                if (!inner_loop_counter) {
                    prcs = mxcd->in_ats_prc_coms;
                    for (auto &prc : mxcd->in_ats_prc_coms) {
                        poss.insert(prc.svalue("channel_type") + "_lat", prc.value("latitude").toLongLong());
                        poss.insert(prc.svalue("channel_type") + "_lon", prc.value("longitude").toLongLong());
                        poss.insert(prc.svalue("channel_type") + "_elev", prc.value("elevation").toLongLong());
                    }
                }
                else {
                    if (prcs.size() == mxcd->in_ats_prc_coms.size()) {
                        size_t iii = 0;
                        for (auto &prc : prcs) {
                            for (auto &inprc : mxcd->in_ats_prc_coms) {
                                if (prc.svalue("channel_type") == inprc.svalue("channel_type")) {
                                    if (prc.qint64value("start_date_time") > inprc.qint64value("start_date_time")) prc.set_key_value("start_date_time", inprc.qint64value("start_date_time"));
                                    if (prc.qint64value("stop_date_time") < inprc.qint64value("stop_date_time")) prc.set_key_value("stop_date_time", inprc.qint64value("stop_date_time"));

                                }
                            }
                        }
                        for (auto &prc : mxcd->in_ats_prc_coms) {
                            poss.insert(prc.svalue("channel_type") + "_lat", prc.value("latitude").toLongLong());
                            poss.insert(prc.svalue("channel_type") + "_lon", prc.value("longitude").toLongLong());
                            poss.insert(prc.svalue("channel_type") + "_elev", prc.value("elevation").toLongLong());
                        }

                    }

                }
                mxcd.reset();
                ++inner_loop_counter;

            }
            else {
                //emit this->signal_mtdata_finished("could not open files, exit");
                //mxcd->close_infiles();
                mathvector.reset();
                //mxcd->disconnect();
                mxcd.reset();



            }

        }
        else {
            mxcd->close_infiles();
            mathvector.reset();
            //mxcd->disconnect();
            mxcd.reset();

            emit this->signal_mtdata_finished("could not prepare MT/CSEM/CSAMT spectra, exit");

        }
    }

    qDebug() << Q_FUNC_INFO << "inner_loop_counter" << inner_loop_counter ;

    if (inner_loop_counter > 1) {
        QStringList ag;
        ag << "_lat" << "_lon" << "_elev";
        for (auto &prc : prcs) {
            QString ChannelType(prc.svalue("channel_type"));
            for (auto &str : ag) {
                QString key(ChannelType + str);
                auto iter = poss.find(key);
                std::vector<qint64> x;
                while (iter != poss.end() && iter.key() == key) {
                    x.push_back(iter.value());
                    ++iter;
                }
                if (x.size() > 1) {
                    qint64 median;
                    median = iter::median<qint64>(x.begin(), x.end());
                    if (str == "_lat") prc.set_key_value("latitude", median);
                    if (str == "_lon") prc.set_key_value("longitude", median);
                    if (str == "_elev") prc.set_key_value("elevation", median);
                }
            }
        }
    }

    mtsite->insert_prc_com(cmdline);
    mtsite->in_ats_prc_coms = prcs;
    mtsite->set_mt_mode(this->my_mttype, this->myproc_type);
    if (cmdline->sztvalue("min_lines") >= 1)  ac1_ssp2_acssp3 = 2;    // line spectra
    if (this->my_mttype == mttype::csem && this->myproc_type == proc_type::csem_rxtx) ac1_ssp2_acssp3 = 2;         // single line spectra for CSEM

    emit this->signal_file_progess("processing tensor");
    int af = all_acspc_collector->get_frequencies().size();
    emit this->slot_set_max_counts_and_last(af, 0);
    emit this->signal_update_progress_bar(af/2);
    mtsite->open(all_acspc_collector, true, true, this->ac1_ssp2_acssp3);
    emit this->signal_update_progress_bar(af);

    emit this->signal_mtdata_finished(QString::number(this->qfis_center.size()) + " files processesd");
}
