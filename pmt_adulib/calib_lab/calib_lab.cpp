#include "calib_lab.h"


calib_lab::calib_lab(QObject *parent) : QObject(parent)
{


}

calib_lab::~calib_lab()
{
    // auto
    /*
    this->msg.reset();
    mxcd.reset();
    mathvector.reset();
    this->cmdline.reset();
    this->mtsite.reset();
    this->all_channel_collector.reset();
    */
    if (this->data != nullptr) this->data.reset();

}

void calib_lab::set_storage(std::shared_ptr<QMap<QString, parallel_test_coherency> > data)
{
    this->data = data;
}

void calib_lab::run(const prc_com &cmdline_parm, const QList<QFileInfo> &qfis)
{

    this->cmdline = std::make_shared<prc_com>(false);

    *this->cmdline = cmdline_parm;


    if (!this->cmdline->contains("num_threads")) {
        size_t num_threads = std::thread::hardware_concurrency();
        if (num_threads < 4) num_threads = 4;
        cmdline->insert_szt("num_threads", num_threads);
    }
    if (!this->cmdline->contains("fcut_upper")) this->cmdline->insert_double("fcut_upper", 0.15);
    if (!this->cmdline->contains("fcut_lower")) this->cmdline->insert_double("fcut_lower", 0.05);
    if (!this->cmdline->contains("nbuffers")) this->cmdline->insert_szt("nbuffers", 8);
    if (!this->cmdline->contains("parzen_radius")) this->cmdline->insert_double("parzen_radius", 0.1);
    if (!this->cmdline->contains("force_frequency_list")) this->cmdline->insert_int("force_frequency_list", 1); // default MT freqs

    if (!this->cmdline->contains("nbuffers")) this->cmdline->insert_szt("nbuffers", 8);

    if (!this->cmdline->contains("min_lines")) cmdline->insert_szt("min_lines", 0);


    if( this->cmdline->svalue("calib_lab_run") == "parallel_test") {

        if (!this->cmdline->contains("wl") && qfis.size()) {
            if (qfis.at(0).exists() && qfis.at(0).fileName().endsWith("ats", Qt::CaseInsensitive)) {
                atsheader atsh(qfis.at(0));
                atsh.scan_header_close();
                this->f_sample = atsh.get_sample_freq();
                this->chopper_status = atsh.ivalue("chopper_status");
                if (this->f_sample < 63.99) this->wl = 64;
                else this->wl = size_t(this->f_sample);
                this->cmdline->insert_szt("wl", this->wl);

            }
            else return;
        }
        this->qfis = qfis;
    }



    // for calibration we must NOT load the calibration, we need the raw spectra
    if (this->cmdline->svalue("calib_lab_run") == "insitu_calibration") {
        this->cmdline->insert("calibration", "off");
        this->cmdline->insert_double("parzen_radius", 0.0);
        this->cmdline->insert_double("auto_bandwidth", 1.0);
        this->wl = 1024;
        this->cmdline->insert_szt("wl", 1024); // deafult AND will be changed by auto bandwidth
        this->qfisxml = qfis;

    }

    std::cout << std::endl << "cut " <<  this->cmdline->dvalue("fcut_lower") << " " << this->cmdline->dvalue("fcut_lower") << std::endl;


    // do this first before we run anything

    this->all_channel_collector = std::make_shared<all_spectra_collector<std::complex<double>>>();
    this->mtsite = std::make_unique<mt_site>(cmdline->sztvalue("num_threads"), mttype::nomt);
    this->msg = std::make_shared<msg_logger>();

    this->process();


}

void calib_lab::write_ascii_spectra()
{
    this->mtsite->slot_dump_ampl_spc();
}

void calib_lab::slot_fde_vector_created(const QString &acsp_name, std::vector<double> &f, std::vector<double> &v, std::vector<double> &e)
{
    if (acsp_name.endsWith("_ampl")) {
        std::lock_guard<std::mutex> lock(this->mutex);
        parallel_test_coherency pttm;
        pttm.f = f;
        pttm.ampl = v;
        pttm.wl = this->wl;
        pttm.f_sample = this->f_sample;
        pttm.chopper = this->chopper_status;
        if(e.size())pttm.err = e;
        this->data->insert(acsp_name,pttm);
    }



}

void calib_lab::slot_fd_vector_created(const QString &acsp_name, std::vector<double> &f, std::vector<double> &v)
{
    if (acsp_name.endsWith("_coh")) {

        std::lock_guard<std::mutex> lock(this->mutex);
        parallel_test_coherency pttm;
        pttm.f = f;
        pttm.coh = v;
        pttm.wl = this->wl;
        pttm.f_sample = this->f_sample;
        pttm.chopper = this->chopper_status;
        this->data->insert(acsp_name,pttm);
    }
}

void calib_lab::slot_input_cal_used(const QString sensortype, const int sernum, const QString channel_type, const std::vector<double> &freqs, const std::vector<double> &ampls, const std::vector<double> &degs, const int chopper)
{
    std::lock_guard<std::mutex> lock(this->mutex);

    parallel_test_coherency pttm;
    pttm.cal_f = freqs;
    pttm.cal_ampl = ampls;
    pttm.cal_phase = degs;
    pttm.chopper = chopper;
    pttm.sensortype = sensortype;
    pttm.sernum = sernum;
    this->data->insert(channel_type,pttm);
}



void calib_lab::process()
{
    int main_loops = 0;

    // excpect that I got a single ist of ats files like for parallel test
    if (this->qfis.size()) main_loops = 1;
    if (this->qfisxml.size()) main_loops = qfisxml.size();
    if (qfis.size() && qfisxml.size()) {
        qFatal("calib_lab::process -> can not get ats and xml files same time!");
        exit(1);
    }

    for (int ii = 0; ii < main_loops; ++ii) {

        QList<QFileInfo> qfisxml_local;

        std::shared_ptr<math_vector> mathvector = std::make_shared<math_vector>();
        std::unique_ptr<mc_data> mxcd = std::make_unique<mc_data>();

        mxcd->insert_prc_com(this->cmdline);
        mxcd->set_messenger(this->msg);

        // generate target frequencies here
        //   or
        // in case of auto_gen_tf_40 ... auto_gen_tf_NN inside mxcd when the fft frequencies are known
        std::vector<double> tf;
        if (this->cmdline->svalue("target_frequencies").contains("step_10")) {
            vector_f_stepper_Hz(tf, 10);
            mxcd->slot_set_target_frequencies(tf);
        }
        else if (this->cmdline->svalue("target_frequencies").contains("step_20")) {
            vector_f_stepper_Hz(tf, 20);
            mxcd->slot_set_target_frequencies(tf);
        }
        else if (this->cmdline->svalue("target_frequencies").contains("step_40")) {
            vector_f_stepper_Hz(tf, 40);
            mxcd->slot_set_target_frequencies(tf);
        }

        else if (this->cmdline->svalue("target_frequencies").contains("step_80")) {
            vector_f_stepper_Hz(tf, 80);
            mxcd->slot_set_target_frequencies(tf);
        }
        else if (this->cmdline->svalue("target_frequencies").contains("FFT_freqs")) {
            this->cmdline->set_key_value("force_frequency_list", 0);
        }

        // from the Insitu
        else if (this->cmdline->svalue("target_frequencies").contains("harmonics")) {

            std::unique_ptr<measdocxml> test_calfreq = std::make_unique<measdocxml> (&this->qfisxml[ii]);
            test_calfreq->create_global_config();
            QMap<QString, QVariant> global_config;
            test_calfreq->get_section("Hardware", "global_config", global_config);
            if (!global_config.size()) {
                test_calfreq->get_section("ADU07Hardware", "global_config", global_config);
            }
            if (!global_config.size()) {
                qFatal("calib_lab::process -> can not find calfreq entry");
                exit(1);
            }

            if ( (global_config.value("calon").toInt() == 1) && global_config.value("calfreq").toDouble() != 0.0 ) {
                this->cmdline->insert_double("base_frequency", global_config.value("calfreq").toDouble());
            }
            tf = create_nharmonics<double>(this->cmdline->dvalue("base_frequency"), this->cmdline->sztvalue("n_harmonics"), true);
            mxcd->slot_set_target_frequencies(tf);
            if (tf.at(0) < 1.0) this->cmdline->set_key_value("auto_bandwidth", double(tf.at(0)));
        }

        mxcd->insert_prc_com(this->cmdline);



        QObject::connect(mathvector.get(), &math_vector::parzen_radius_changed,
                         this->msg.get(), &msg_logger::parzen_radius_changed, Qt::DirectConnection);
        QObject::connect(mxcd.get(), &mc_data::signal_got_freq_targets,
                         mathvector.get(), &math_vector::slot_parzen_vector, Qt::DirectConnection);
        QObject::connect(mathvector.get(), &math_vector::parzenvector_created,
                         mxcd.get(), &mc_data::slot_append_parzenvector, Qt::DirectConnection);



        size_t nfiles = 0;
        if (this->qfisxml.size()) {
            qfisxml_local.append(this->qfisxml.at(ii));


            nfiles = mxcd->open_files(qfisxml_local, true, this->spcx, threadctrl::threadctrl_continuous_join);
        }
        else {
            nfiles = mxcd->open_files(this->qfis, true, this->spcx, threadctrl::threadctrl_continuous_join);

        }

        if(!nfiles) {
            return;
        }

        // here we want to see the cal which has been really applied to the spectra finally
        for (auto &ats : mxcd->in_atsfiles) {
            QObject::connect(ats.get(), &atsfile::signal_interpolated_cal_used, this, &calib_lab::slot_input_cal_used);
        }

        // this->cmdline->ivalue("force_second_set_remote") activated the second ADU

        // what if no calib is wanted ? : set it in the cmdline!
        if (mxcd->prepare_mt_spectra(this->cmdline->sztvalue("nbuffers"), this->cmdline->sztvalue("wl"),
                                     this->cmdline->dvalue("fcut_upper"), this->cmdline->dvalue("fcut_lower"),
                                     0, 0, 0, this->cmdline->dvalue("parzen_radius"), this->cmdline->sztvalue("min_lines"),
                                     this->cmdline->ivalue("force_frequency_list"), atsfileout::atsfileout_calibrated_spectra, true)) {



            mxcd->start_in_ats_channel_threads();
            mxcd->start_collect_all_spc_threads();
            mxcd->join_in_ats_channel_threads();
            mxcd->join_collect_all_spc_threads();

            //all_channel_collector
            qDebug() << "spectra collected";

            //size_t i;
            for (auto &ssp : mxcd->out_atsfile_collectors) {
                // i = ssp
                this->all_channel_collector->data[ssp->spc.chan_type].append_clear_data(ssp->spc);
            }
        }

        mathvector.reset();
        mxcd.reset();
    }

    QObject::connect(this->mtsite.get(), &mt_site::signal_fde_vector_created, this, &calib_lab::slot_fde_vector_created);
    QObject::connect(this->mtsite.get(), &mt_site::signal_fd_vector_created, this, &calib_lab::slot_fd_vector_created);

    int ac1_ssp2_acssp3 = 2;  // await that single spectra are coming ... and we will calc ac later
    this->mtsite->insert_prc_com(this->cmdline);
    this->mtsite->open(all_channel_collector, false, false, ac1_ssp2_acssp3);
    this->all_channel_collector.reset();

    if (this->cmdline->svalue("calib_lab_run") == "parallel_test") {

        this->mtsite->resize_auto_spc_only();
        this->mtsite->slot_calc_auto_spc();
        this->mtsite->slot_ssp_stack_all();
        this->mtsite->ssp_xstddev();
        this->mtsite->slot_ssp_stack_all();
        this->mtsite->prepare_parallel_coherency();
        this->mtsite->get_vectors_fde("single_amplitude_stacked_spectra");
        this->mtsite->get_vectors_fde("parallel_coherency");

        auto iter = this->data->begin();
        auto end = this->data->end();
        while (iter != end) {
            if (iter.key().contains("_ampl")) {
                auto iterin = this->data->begin();
                auto endin = this->data->end();
                while (iterin != endin) {
                    if (iter.key().startsWith(iterin.key())) {
                        iter.value().cal_f = iterin.value().cal_f;
                        iter.value().cal_ampl = iterin.value().cal_ampl;
                        iter.value().cal_phase = iterin.value().cal_phase;
                        iter.value().sernum = iterin.value().sernum;
                        iter.value().chopper = iterin.value().chopper;
                        iter.value().sensortype = iterin.value().sensortype;
                        iter.value().channel_type = iterin.value().channel_type;
                    }
                    ++iterin;
                }
            }
            ++iter;
        }
        iter = this->data->begin();
        while (iter != end) {
            if (!iter.key().contains("_ampl") && (!iter.key().contains("_coh")) ) {
                iter = this->data->erase(iter);
            }
            else ++iter;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////

    if (this->cmdline->svalue("calib_lab_run") == "insitu_calibration") {
        this->mtsite->ssp_normalize_by_E();
    }


    qDebug() << "finished";


    this->mtsite->slot_dump_ampl_spc(true);
    //    this->mtsite->slot_dump_ssp_coherency();

    this->mtsite.release();
    this->mtsite.reset();




}

//bool calib_lab::ckeck_storage()
//{
//    if (this->amplitudes == nullptr) return  false;
//    if (this->amplitudes_errors == nullptr) return  false;
//    if (this->coherencies == nullptr) return false;
//    return true;

//}
