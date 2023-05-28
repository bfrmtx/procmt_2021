#include "mt_site.h"



mt_site::mt_site(const size_t num_threads, const mttype enum_mttype, QObject *parent) : QObject(parent), prc_com(true)
{
    this->set_class_name("mt_site");
    this->fill_prc_com_with_base_values();

    this->num_threads = num_threads;
    if (this->num_threads < 4) this->num_threads = 4;
    this->mymttye = enum_mttype;

    this->dump_path = QDir::homePath();

    //this->assign();

}

//mt_site::mt_site(const mt_site &rhs) : QObject(rhs.parent()), prc_com(true) {

//    this->operator =(rhs);
//    this->set_class_name("mt_site");
//    //this->assign();
//}


//mt_site &mt_site::operator =(const mt_site &rhs)
//{
//    if(&rhs == this) return *this;
//    //this->data = rhs.data;

//    // copy


//    prc_com::operator =(rhs);
//    this->set_class_name("mt_site");
//    //   this->assign();

//    return *this;
//}

mt_site::~mt_site()
{


    for (auto &ax : this->ac_spectra) ax.clear();
    this->ac_spectra.clear();

    this->z.clear();
    qDebug() << "mt_site deleted";
}

void mt_site::clear()
{
    this->ac_spectra.clear();
    this->z.clear();

}

void mt_site::resize()
{
    for (auto &ac : this->ac_spectra) {
        ac.resize();
    }
}

void mt_site::resize_z()
{
    this->z.clear();
    this->z.reserve(this->ac_spectra.size());
    if (this->ac_spectra.at(0).ac.at(pmt::hxhz).size() && this->ac_spectra.at(0).ac.at(pmt::hyhz).size()) {
        for (auto &ac : this->ac_spectra) {
            this->z.emplace_back(mt_data_t<std::complex<double>>(tns::tns_tip_size, ac.f, this->sztvalue("min_stacks"), ac.size()));
        }
    }
    else if (this->ac_spectra.at(0).ac.at(pmt::hxhx).size() && this->ac_spectra.at(0).ac.at(pmt::hyhy).size()) {
        for (auto &ac : this->ac_spectra) {
            this->z.emplace_back(mt_data_t<std::complex<double>>(tns::tns_size, ac.f, this->sztvalue("min_stacks"), ac.size()));
        }
    }

}


void mt_site::get_vectors_fde(const QString &what)
{
    if (!this->ac_spectra.size()) return;
    if (what == "single_amplitude_stacked_spectra") {

        size_t spce = 0; // for each type ExEx ... RHzRHz
        for (auto &acv : this->ac_spectra.at(0).ac) {
            if (acv.size()) {
                QString acsp_name(pmt::ac_spectra_names.at(int(spce)) + "_ampl");
                std::vector<double> f;
                std::vector<double> v;
                std::vector<double> e;
                f.reserve(this->ac_spectra.size());
                v.reserve(this->ac_spectra.size());
                e.reserve(this->ac_spectra.size());
                // for all frequencies
                for (auto &ac : this->ac_spectra) {
                    if(ac.ac_vals[spce].size()) {
                        f.push_back(ac.f);
                        v.push_back(ac.ac_vals[spce].at(g_stat::mean_x));
                        e.push_back(ac.ac_vals[spce].at(g_stat::stddev_x));
                    }

                }
                if (f.size() && f.size() == v.size())emit this->signal_fde_vector_created(acsp_name, f, v, e);

            }
            ++spce;
        }

    }

    if (what == "parallel_coherency") {

        size_t spce = 0; // for each type ExEx ... RHzRHz
        for (auto &acv : this->ac_spectra.at(0).coh) {
            if (acv != DBL_MAX) {
                QString acsp_name(pmt::ac_spectra_names.at(int(spce)) + "_coh");
                std::vector<double> f;
                std::vector<double> v;
                f.reserve(this->ac_spectra.size());
                v.reserve(this->ac_spectra.size());
                // for all frequencies
                for (auto &ac : this->ac_spectra) {
                    if(ac.coh[spce] != DBL_MAX) {
                        f.push_back(ac.f);
                        v.push_back(ac.coh[spce]);
                    }

                }
                if (f.size() && f.size() == v.size())emit this->signal_fd_vector_created(acsp_name, f, v);

            }
            ++spce;
        }

    }


}


void mt_site::resize_auto_spc_only()
{
    for (auto &ac : this->ac_spectra) {
        ac.resize_auto_spc_only();
    }
}

void mt_site::insert_prc_com(std::shared_ptr<prc_com> cmdline)
{
    this->insert_data(cmdline->get_data(), true);
    if ( svalue("survey_basedir").size() && (svalue("survey_basedir") != "empty" )) {
        this->dump_path = svalue("survey_basedir") + "/dump";
    }
    else if ( cmdline->svalue("survey_basedir").size() && (cmdline->svalue("survey_basedir") != "empty" )) {
        this->dump_path = cmdline->svalue("survey_basedir") + "/dump";
        this->insert("survey_basedir", cmdline->svalue("survey_basedir"));
    }

    //    if ( svalue("site_dir").size() && (svalue("site_dir") != "empty" )) {
    //        this->dump_path = svalue("site_dir") + "/dump";
    //    }
    //    else if ( cmdline->svalue("site_dir").size() && (cmdline->svalue("site_dir") != "empty" )) {
    //        this->dump_path = cmdline->svalue("site_dir") + "/dump";
    //        this->insert("site_dir", cmdline->svalue("site_dir"));
    //    }
}

void mt_site::set_mt_mode(const mttype &enum_mttype, const proc_type &enum_proc_type)
{
    this->mymttye = enum_mttype;
    this->myproc_type = enum_proc_type;
}

void mt_site::dump_ac_spc()
{
    for (auto &ac : this->ac_spectra) {
        if (ac.size()) {
            QString fstr = QString::number(ac.f, 'g');
            int inam = 0;
            for (auto &data : ac.ac) {
                if (data.size()) {
                    QString component = pmt::ac_spectra_names.at(inam);
                    QFileInfo qfi(this->dump_path + "/" + fstr + "_" + component + "_abs.dat");
                    QFile file(qfi.absoluteFilePath());
                    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
                        QTextStream out(&file);
                        for (auto &dat : data) out << std::abs(dat) << Qt::endl;
                        out << Qt::endl;
                        file.close();
                    }

                    qfi.setFile(this->dump_path + "/" + fstr + "_" + component + "_re.dat");
                    file.setFileName(qfi.absoluteFilePath());
                    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
                        QTextStream out(&file);
                        for (auto &dat : data) out << std::real(dat) << Qt::endl;
                        out << Qt::endl;
                        file.close();
                    }

                    qfi.setFile(this->dump_path + "/" + fstr + "_" + component + "_im.dat");
                    file.setFileName(qfi.absoluteFilePath());
                    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
                        QTextStream out(&file);
                        for (auto &dat : data) out << std::imag(dat) << Qt::endl;
                        out << Qt::endl;
                        file.close();
                    }

                }
                ++inam;
            }
        }
    }
}

void mt_site::clean_up_residuals()
{
    if (this->ivalue("remove_rr_residuals")) {
        for (auto &ac : this->ac_spectra) ac.reset_cross_remote_residuals();
    }

    if (this->ivalue("remove_auto_residuals")) {
        for (auto &ac : this->ac_spectra) ac.reset_auto_residuals();
    }
}


void mt_site::open(std::shared_ptr<all_spectra_collector<std::complex<double> > > &all_channel_collector, const bool resize, const bool resize_z, const int ac1_ssp2_acssp3)
{

    //emit this->signal_update_progress_bar(0);

    this->ac_spectra.reserve(all_channel_collector->get_frequencies().size());
    std::sort(all_channel_collector->all_freqs.begin(), all_channel_collector->all_freqs.end());

    //all_channel_collector->take_5();

    this->freqs = all_channel_collector->get_frequencies(); // all frequencies
    std::vector<size_t> sizes(all_channel_collector->get_sizes(this->freqs));
    size_t i = 0;
    for (auto &f : all_channel_collector->all_freqs) {

        if (sizes.at(i) > this->sztvalue("min_stacks")) {
            this->ac_spectra.emplace_back(*all_channel_collector, f, ac1_ssp2_acssp3);
            qDebug() << "    accepted " << doublefreq2string_unit(f) << "with size" << sizes.at(i);
            emit this->signal_frequency_added(f);

        }
        else {
            qDebug() << "NOT accepted " << doublefreq2string_unit(f) << "with size" << sizes.at(i);
        }

        ++i;
        //emit this->signal_update_progress_bar(i);
    }

    this->clean_up_residuals();



    // have line spectra
    if ( (this->mymttye >= mttype::mt) && (this->mymttye <= mttype::emap_rr_full) ) {
        if ((this->ac_spectra.at(0).size() == 0) && (this->ac_spectra.at(0).ssp_size() != 0) ) {
            for (auto &ac : this->ac_spectra) {
                ac.backup_e();
            }
            this->huber(1.58, this->sztvalue("huber_iterations"));
            this->slot_calc_ac_spc();
        }
    }

    if (this->mymttye != mttype::csem) this->resize_z();
    this->freqs = get_freqs_acsp(this->ac_spectra); // all frequencies

}


void mt_site::run()
{

    emit this->signal_general_msg("mt_site", "starting", "run");
    emit this->signal_update_progress_bar(0);


    if (this->sztvalue("min_stacks") < _min_stacks) this->insert_szt("min_stacks", _min_stacks);
    qDebug() << "mtsite::run -> frequencies incoming:";
    for (auto &d : this->get_freqs()) qDebug() << doublefreq2string_unit(d);

    // z from spectra
    this->calc_z();

    // if quadrant check is checked do it
    if (this->ivalue("activate_quadrant_check")) {
        this->quadrant_check(this->sztvalue("tns_sel"));
    }
    if (this->ivalue("activate_ct")) {
        this->coherency_threshold(this->dvalue("lower_rejection_coh"), this->dvalue("upper_rejection_coh"), this->sztvalue("tns_sel") );
    }

    if (this->ivalue("activate_median_rho_phi")) {
        this->calc_rho_phi();
        this->inside_range_median_rho_phi(this->dvalue("lower_median_rho"), this->dvalue("upper_median_rho"),
                                          this->dvalue("lower_median_phi"), this->dvalue("upper_median_phi"),this->sztvalue("tns_sel") );

    }


    // if (this->ivalue("activate_median_complex")) {

    this->stack_all();

    if ( (this->ivalue("xstddev_active") == 1) && this->ivalue("xstddev_auto_active") == 0) {
        this->xstddev(this->sztvalue("tns_sel"));
        this->stack_all();
    }

    if ( (this->ivalue("xstddev_active") == 1) && this->ivalue("xstddev_auto_active") == 1) {
        this->xstddev_auto(this->sztvalue("tns_sel"));
        this->stack_all();
    }


    // avoid that the edi file gets garbarge
    this->zet.check_delete_err_dbl_max();

    emit this->signal_update_progress_bar(this->ac_spectra.size());
    emit this->signal_general_msg("mt_site", "finished", "run");

    // this->slot_save_data_sql("/home/bfr/mtsite.sql3");

    emit this->signal_mt_site_EDI_finished(this->zet);
    //emit this->signal_mt_site_updated();
    emit this->signal_mt_site_finished();
}

void mt_site::cp_ssp_vals_to_QMap(const acsp &ac, QMap<QString, QVariant> &data, const int dip)
{

    if (!ac.ssp.size()) return;

    data.clear();

    // we need all for the table data
    for (auto &str : pmt::csem_dipcols) {
        if (str != "rowid") data.insert(str, double(0.0));
    }

    data.insert("site_name", this->svalue("site_dir"));
    data.insert("freq", ac.f);

    if (dip == 1) {
        data.insert("Ex_real",      ac.ssp_vals.at(pmt::ex).at(g_stat::mean_x));
        data.insert("Ex_real_Err",  ac.ssp_vals.at(pmt::ex).at(g_stat::stddev_x));
        data.insert("Ex_imag",      ac.ssp_vals_imag.at(pmt::ex).at(g_stat::mean_x));
        data.insert("Ex_imag_Err",  ac.ssp_vals_imag.at(pmt::ex).at(g_stat::stddev_x));

        data.insert("Ey_real",      ac.ssp_vals.at(pmt::ey).at(g_stat::mean_x));
        data.insert("Ey_real_Err" , ac.ssp_vals.at(pmt::ey).at(g_stat::stddev_x));
        data.insert("Ey_imag" ,     ac.ssp_vals_imag.at(pmt::ey).at(g_stat::mean_x));
        data.insert("Ey_imag_Err",  ac.ssp_vals_imag.at(pmt::ey).at(g_stat::stddev_x));

        data.insert("Ez_real",      double (0.0));
        data.insert("Ez_real_Err" , double (0.0));
        data.insert("Ez_imag" ,     double (0.0));
        data.insert("Ez_imag_Err",  double (0.0));

        data.insert("Hx_real",      ac.ssp_vals.at(pmt::hx).at(g_stat::mean_x));
        data.insert("Hx_real_Err",  ac.ssp_vals.at(pmt::hx).at(g_stat::stddev_x));
        data.insert("Hx_imag" ,     ac.ssp_vals_imag.at(pmt::hx).at(g_stat::mean_x));
        data.insert("Hx_imag_Err",  ac.ssp_vals_imag.at(pmt::hx).at(g_stat::stddev_x));

        data.insert("Hy_real" ,     ac.ssp_vals.at(pmt::hy).at(g_stat::mean_x));
        data.insert("Hy_real_Err" , ac.ssp_vals.at(pmt::hy).at(g_stat::stddev_x));
        data.insert("Hy_imag" ,     ac.ssp_vals_imag.at(pmt::hy).at(g_stat::mean_x));
        data.insert("Hy_imag_Err",  ac.ssp_vals_imag.at(pmt::hy).at(g_stat::stddev_x));

        if (ac.ssp_vals.at(pmt::hz).size() && ac.ssp_vals_imag.at(pmt::hz).size()) {
            data.insert("Hz_real" ,     ac.ssp_vals.at(pmt::hz).at(g_stat::mean_x));
            data.insert("Hz_real_Err" , ac.ssp_vals.at(pmt::hz).at(g_stat::stddev_x));
            data.insert("Hz_imag" ,     ac.ssp_vals_imag.at(pmt::hz).at(g_stat::mean_x));
            data.insert("Hz_imag_Err",  ac.ssp_vals_imag.at(pmt::hz).at(g_stat::stddev_x));
        }
        data.insert("Tx_Curr_U_real" ,      ac.ssp_vals.at(pmt::rhx).at(g_stat::mean_x));
        data.insert("Tx_Curr_U_real_Err" ,  ac.ssp_vals.at(pmt::rhx).at(g_stat::stddev_x));
        data.insert("Tx_Curr_U_imag",       ac.ssp_vals_imag.at(pmt::rhx).at(g_stat::mean_x));
        data.insert("Tx_Curr_U_imag_Err",   ac.ssp_vals_imag.at(pmt::rhx).at(g_stat::stddev_x));

        data.insert("Tx_Curr_V_real" ,      ac.ssp_vals.at(pmt::rhy).at(g_stat::mean_x));
        data.insert("Tx_Curr_V_real_Err" ,  ac.ssp_vals.at(pmt::rhy).at(g_stat::stddev_x));
        data.insert("Tx_Curr_V_imag" ,      ac.ssp_vals_imag.at(pmt::rhy).at(g_stat::mean_x));
        data.insert("Tx_Curr_V_imag_Err",   ac.ssp_vals_imag.at(pmt::rhy).at(g_stat::stddev_x));

        data.insert("Tx_Curr_W_real" ,      ac.ssp_vals.at(pmt::rhz).at(g_stat::mean_x));
        data.insert("Tx_Curr_W_real_Err" ,  ac.ssp_vals.at(pmt::rhz).at(g_stat::stddev_x));
        data.insert("Tx_Curr_W_imag" ,      ac.ssp_vals_imag.at(pmt::rhz).at(g_stat::mean_x));
        data.insert("Tx_Curr_W_imag_Err",   ac.ssp_vals_imag.at(pmt::rhz).at(g_stat::stddev_x));

    }

    if (dip == 2) {
        data.insert("Ex_real",      ac.ssp_vals_2.at(pmt::ex).at(g_stat::mean_x));
        data.insert("Ex_real_Err",  ac.ssp_vals_2.at(pmt::ex).at(g_stat::stddev_x));
        data.insert("Ex_imag",      ac.ssp_vals_imag_2.at(pmt::ex).at(g_stat::mean_x));
        data.insert("Ex_imag_Err",  ac.ssp_vals_imag_2.at(pmt::ex).at(g_stat::stddev_x));

        data.insert("Ey_real",      ac.ssp_vals_2.at(pmt::ey).at(g_stat::mean_x));
        data.insert("Ey_real_Err" , ac.ssp_vals_2.at(pmt::ey).at(g_stat::stddev_x));
        data.insert("Ey_imag" ,     ac.ssp_vals_imag_2.at(pmt::ey).at(g_stat::mean_x));
        data.insert("Ey_imag_Err",  ac.ssp_vals_imag_2.at(pmt::ey).at(g_stat::stddev_x));

        data.insert("Ez_real",      double (0.0));
        data.insert("Ez_real_Err" , double (0.0));
        data.insert("Ez_imag" ,     double (0.0));
        data.insert("Ez_imag_Err",  double (0.0));

        data.insert("Hx_real",      ac.ssp_vals_2.at(pmt::hx).at(g_stat::mean_x));
        data.insert("Hx_real_Err",  ac.ssp_vals_2.at(pmt::hx).at(g_stat::stddev_x));
        data.insert("Hx_imag" ,     ac.ssp_vals_imag_2.at(pmt::hx).at(g_stat::mean_x));
        data.insert("Hx_imag_Err",  ac.ssp_vals_imag_2.at(pmt::hx).at(g_stat::stddev_x));

        data.insert("Hy_real" ,     ac.ssp_vals_2.at(pmt::hy).at(g_stat::mean_x));
        data.insert("Hy_real_Err" , ac.ssp_vals_2.at(pmt::hy).at(g_stat::stddev_x));
        data.insert("Hy_imag" ,     ac.ssp_vals_imag_2.at(pmt::hy).at(g_stat::mean_x));
        data.insert("Hy_imag_Err",  ac.ssp_vals_imag_2.at(pmt::hy).at(g_stat::stddev_x));

        if (ac.ssp_vals_2.at(pmt::hz).size() && ac.ssp_vals_imag_2.at(pmt::hz).size()) {
            data.insert("Hz_real" ,     ac.ssp_vals_2.at(pmt::hz).at(g_stat::mean_x));
            data.insert("Hz_real_Err" , ac.ssp_vals_2.at(pmt::hz).at(g_stat::stddev_x));
            data.insert("Hz_imag" ,     ac.ssp_vals_imag_2.at(pmt::hz).at(g_stat::mean_x));
            data.insert("Hz_imag_Err",  ac.ssp_vals_imag_2.at(pmt::hz).at(g_stat::stddev_x));
        }

        data.insert("Tx_Curr_U_real" ,      ac.ssp_vals_2.at(pmt::rhx).at(g_stat::mean_x));
        data.insert("Tx_Curr_U_real_Err" ,  ac.ssp_vals_2.at(pmt::rhx).at(g_stat::stddev_x));
        data.insert("Tx_Curr_U_imag",       ac.ssp_vals_imag_2.at(pmt::rhx).at(g_stat::mean_x));
        data.insert("Tx_Curr_U_imag_Err",   ac.ssp_vals_imag_2.at(pmt::rhx).at(g_stat::stddev_x));

        data.insert("Tx_Curr_V_real" ,      ac.ssp_vals_2.at(pmt::rhy).at(g_stat::mean_x));
        data.insert("Tx_Curr_V_real_Err" ,  ac.ssp_vals_2.at(pmt::rhy).at(g_stat::stddev_x));
        data.insert("Tx_Curr_V_imag" ,      ac.ssp_vals_imag_2.at(pmt::rhy).at(g_stat::mean_x));
        data.insert("Tx_Curr_V_imag_Err",   ac.ssp_vals_imag_2.at(pmt::rhy).at(g_stat::stddev_x));

        data.insert("Tx_Curr_W_real" ,      ac.ssp_vals_2.at(pmt::rhz).at(g_stat::mean_x));
        data.insert("Tx_Curr_W_real_Err" ,  ac.ssp_vals_2.at(pmt::rhz).at(g_stat::stddev_x));
        data.insert("Tx_Curr_W_imag" ,      ac.ssp_vals_imag_2.at(pmt::rhz).at(g_stat::mean_x));
        data.insert("Tx_Curr_W_imag_Err",   ac.ssp_vals_imag_2.at(pmt::rhz).at(g_stat::stddev_x));


    }

}

void mt_site::cp_tx_epos_vals_to_QMap(QMap<QString, QVariant> &data)
{
    if (!this->tx_e_pos.size()) return;
    QStringList epos;
    epos << "U_msec_lat" << "U_msec_lon" << "U_cm_elev"
         << "V_msec_lat" << "V_msec_lon" << "V_cm_elev"
         << "W_msec_lat" << "W_msec_lon"<< "W_cm_elev";
    for (auto &str : epos) {
        qDebug() << data.value(str).toDouble() ;
        qDebug() << this->tx_e_pos.contains(str);
        if ((std::abs(data.value(str).toDouble()) <= 0.0000001) && (this->tx_e_pos.contains(str))) {
            data.insert(str, this->tx_e_pos.value(str).toDouble());
            qDebug() << this->tx_e_pos.value(str).toDouble();
        }
    }
}


void mt_site::run_csem()
{
    emit this->signal_general_msg("mt_site", "starting", "run csem");
    QMap<QString, QVariant> data;
    this->slot_ssp_stack_half_csem(1);
    for (auto &acp : this->ac_spectra) {
        if (acp.ssp.size()) {
            this->cp_ssp_vals_to_QMap(acp, data, 1);
            this->cp_tx_epos_vals_to_QMap(data);
            if (this->in_ats_prc_coms.size()) {
                for (auto &prc : this->in_ats_prc_coms) {
                    if (prc.svalue("channel_type").startsWith("Ex", Qt::CaseInsensitive)) {
                        // prc.ivalue("latitude"), prc.ivalue("longitude"), (prc.dvalue("elevation") / 100.)
                        data.insert("msec_lat", prc.ivalue("latitude"));
                        data.insert("msec_lon", prc.ivalue("longitude"));
                        data.insert("cm_elev",  prc.ivalue("elevation"));
                    }
                }
            }
            emit this->signal_csem_dip_qmap(data, 1);
        }
    }

    this->slot_ssp_stack_half_csem(2);
    for (auto &acp : this->ac_spectra) {
        if (acp.ssp.size()) {
            this->cp_ssp_vals_to_QMap(acp, data, 2);
            this->cp_tx_epos_vals_to_QMap(data);
            if (this->in_ats_prc_coms.size()) {
                for (auto &prc : this->in_ats_prc_coms) {
                    if (prc.svalue("channel_type").startsWith("Ex", Qt::CaseInsensitive)) {
                        // prc.ivalue("latitude"), prc.ivalue("longitude"), (prc.dvalue("elevation") / 100.)
                        data.insert("msec_lat", prc.ivalue("latitude"));
                        data.insert("msec_lon", prc.ivalue("longitude"));
                        data.insert("cm_elev",  prc.ivalue("elevation"));
                    }
                }
            }
            emit this->signal_csem_dip_qmap(data, 2);
        }
    }



    emit this->signal_csem_site_finished();

}

size_t mt_site::write_bin_spectra(QFileInfo &qfi)
{
    if (!this->ac_spectra.size()) return 0;
    //    QString filename = this->dump_path + "/" + this->svalue("site_dir") + ".binspectra";
    //    qfi.setFile(filename);
    return write_acsp_vector(qfi, this->ac_spectra);


}

size_t mt_site::read_bin_spectra(const QFileInfo &qfi)
{
    if (!qfi.exists()) return 0;
    size_t spectras;
    spectras = read_acsp_vector(qfi, this->ac_spectra);
    this->resize_z();
    return spectras;

}

size_t mt_site::write_ascii_single_spectra(QFileInfo &qfidirname)
{
    if (!this->ac_spectra.size()) return 0;
    return write_ssp_vector_ascii(qfidirname, this->ac_spectra);

}

void mt_site::calc_z()
{
    if ((this->mymttye >= mttype::mt) && this->mymttye < mttype::rr) {
        // all frequencies
        std::vector<std::future<void>> futures;
        this->counts = 0;

        for ( auto &spc : get_freqs_acsp(this->ac_spectra)) {
            futures.emplace_back(std::async(&mt_site::calc_z_vector, this, spc));
        }
        for(auto &f : futures) {
            f.get();
        }
    }

    if ((this->mymttye == mttype::rr) || (this->mymttye == mttype::emap_rr)) {
        // all frequencies
        std::vector<std::future<void>> futures;

        for ( auto &spc : get_freqs_acsp(this->ac_spectra)) {
            futures.emplace_back(std::async(&mt_site::calc_z_vector_rr, this, spc));
        }
        for(auto &f : futures) {
            f.get();
        }
    }


    // this->mymttye == mttype::emap
    // this->mymttye == mttype::rr
    // this->mymttye == mttype::emap_rr
    // this->mymttye == mttype::rr_full


}



void mt_site::calc_z_vector(const double &f)
{
    this->z_mutex.lock();
    this->signal_update_progress_bar(this->counts++);
    this->z_mutex.unlock();
    size_t i, stacks, f_idx, j;
    //stacks = this->ac_spectra[f_idx].ac[pmt::hxhx][f].size();
    stacks = f_size_a(this->ac_spectra, f, f_idx);
    //this->z_mutex.unlock();

    if (f_idx == SIZE_MAX) return;
    if (f_idx == SIZE_MAX) return;
    bool has_t = false;
    if (this->ac_spectra.at(f_idx).ac.at(pmt::hxhz).size() && this->ac_spectra.at(f_idx).ac.at(pmt::hyhz).size()) has_t = true;

    qDebug() << "calculating z, freq" << doublefreq2string_unit(f) << " stacks:" << stacks;

    for (i = 0; i < stacks; ++i) {
        std::complex<double> denom =  ( (this->ac_spectra[f_idx].ac[pmt::hxhx][i] * this->ac_spectra[f_idx].ac[pmt::hyhy][i] ) -
                (this->ac_spectra[f_idx].ac[pmt::hxhy][i] * conj(this->ac_spectra[f_idx].ac[pmt::hxhy][i]))  );

        //!< @todo mark forever the boolean - or eliminate or what
        // check NULL
        if (denom.imag() != 0.0) {
            qDebug() << " not real";
        }
        if (denom.real() == 0.0 && denom.imag() == 0.0) {
            for (j = 0; j < this->z[f_idx].d.size(); ++j) {
                this->z[f_idx].b[j][i] = false;
                // this->z[f_idx].d[j][i] = std::complex<double> (0.0, 0.0);
            }
        }
        else {
            this->z[f_idx].d[xx][i] =  ( ( this->ac_spectra[f_idx].ac[pmt::hyhy][i] * this->ac_spectra[f_idx].ac[pmt::exhx][i] ) - ( conj(this->ac_spectra[f_idx].ac[pmt::hxhy][i]) * this->ac_spectra[f_idx].ac[pmt::exhy][i] ) ) / denom;  // Z_xx

            this->z[f_idx].d[xy][i] =  ( ( this->ac_spectra[f_idx].ac[pmt::hxhx][i] * this->ac_spectra[f_idx].ac[pmt::exhy][i] ) - ( this->ac_spectra[f_idx].ac[pmt::hxhy][i] * this->ac_spectra[f_idx].ac[pmt::exhx][i] ) )       / denom;  // Z_xy

            this->z[f_idx].d[yx][i] =  ( ( this->ac_spectra[f_idx].ac[pmt::hyhy][i] * this->ac_spectra[f_idx].ac[pmt::eyhx][i] ) - ( conj(this->ac_spectra[f_idx].ac[pmt::hxhy][i]) * this->ac_spectra[f_idx].ac[pmt::eyhy][i] ) ) / denom;  // Z_yx

            this->z[f_idx].d[yy][i] =  ( ( this->ac_spectra[f_idx].ac[pmt::hxhx][i] * this->ac_spectra[f_idx].ac[pmt::eyhy][i] ) - ( this->ac_spectra[f_idx].ac[pmt::hxhy][i] * this->ac_spectra[f_idx].ac[pmt::eyhx][i] ) )       / denom;  // Z_yy

            if (has_t) {
                this->z[f_idx].d[tx][i] = ( ( this->ac_spectra[f_idx].ac[pmt::hxhz][i] * this->ac_spectra[f_idx].ac[pmt::hyhy][i] ) -
                        ( this->ac_spectra[f_idx].ac[pmt::hyhz][i] * conj(this->ac_spectra[f_idx].ac[pmt::hxhy][i]) ) ) / denom;

                this->z[f_idx].d[ty][i] = ( ( this->ac_spectra[f_idx].ac[pmt::hyhz][i] * this->ac_spectra[f_idx].ac[pmt::hxhx][i] ) -
                        ( this->ac_spectra[f_idx].ac[pmt::hxhz][i] * this->ac_spectra[f_idx].ac[pmt::hxhy][i] ) ) / denom;
            }
        }
    }
}

void mt_site::calc_z_vector_rr(const double &f)
{
    //this->z_mutex.lock();
    size_t i, stacks, f_idx, j;
    //stacks = this->ac_spectra[f_idx].ac[pmt::hxhx][f].size();
    stacks = f_size_a(this->ac_spectra, f, f_idx);
    //this->z_mutex.unlock();

    if (f_idx == SIZE_MAX) return;
    bool has_t = false;
    if (this->ac_spectra.at(f_idx).ac.at(pmt::hxhz).size() && this->ac_spectra.at(f_idx).ac.at(pmt::hyhz).size()) has_t = true;

    qDebug() << "calculating z, freq" << doublefreq2string_unit(f) << " stacks:" << stacks;

    for (i = 0; i < stacks; ++i) {

        std::complex<double> denom =  ( (this->ac_spectra[f_idx].ac[pmt::hxrhx][i] * this->ac_spectra[f_idx].ac[pmt::hyrhy][i])) -
                ( (this->ac_spectra[f_idx].ac[pmt::hxrhy][i] * (this->ac_spectra[f_idx].ac[pmt::hyrhx][i])))  ;


        //denom = std::abs(denom);
        //!< @todo mark forever the boolean - or elimnate or what
        // check NULL
        if (denom.real() == 0.0 && denom.imag() == 0.0) {
            // if (denom < 10E-36) {
            for (j = 0; j < this->z[f_idx].d.size(); ++j) {
                this->z[f_idx].b[j][i] = false;
                // this->z[f_idx].d[j][i] = std::complex<double> (0.0, 0.0);
            }
        }
        else {
            // bfr
            //            this->z[f_idx].d[xx][i] =  ( ( this->ac_spectra[f_idx].ac[pmt::hyrhy][i] * this->ac_spectra[f_idx].ac[pmt::exrhx][i] ) - ( conj(this->ac_spectra[f_idx].ac[pmt::hxrhy][i]) * this->ac_spectra[f_idx].ac[pmt::exrhy][i] ) ) / denom;  // Z_xx

            //            this->z[f_idx].d[xy][i] =  ( ( this->ac_spectra[f_idx].ac[pmt::hxrhx][i] * this->ac_spectra[f_idx].ac[pmt::exrhy][i] ) - ( this->ac_spectra[f_idx].ac[pmt::hxrhy][i] * this->ac_spectra[f_idx].ac[pmt::exrhx][i] ) )       / denom;  // Z_xy

            //            this->z[f_idx].d[yx][i] =  ( ( this->ac_spectra[f_idx].ac[pmt::hyrhy][i] * this->ac_spectra[f_idx].ac[pmt::eyrhx][i] ) - ( conj(this->ac_spectra[f_idx].ac[pmt::hxrhy][i]) * this->ac_spectra[f_idx].ac[pmt::eyrhy][i] ) ) / denom;  // Z_yx

            //            this->z[f_idx].d[yy][i] =  ( ( this->ac_spectra[f_idx].ac[pmt::hxrhx][i] * this->ac_spectra[f_idx].ac[pmt::eyrhy][i] ) - ( this->ac_spectra[f_idx].ac[pmt::hxrhy][i] * this->ac_spectra[f_idx].ac[pmt::eyrhx][i] ) )       / denom;  // Z_yy

            //kba

            this->z[f_idx].d[xx][i] =  ( ( this->ac_spectra[f_idx].ac[pmt::hyrhy][i] * this->ac_spectra[f_idx].ac[pmt::exrhx][i] ) - ( (this->ac_spectra[f_idx].ac[pmt::hyrhx][i]) * this->ac_spectra[f_idx].ac[pmt::exrhy][i] ) ) / denom;  // Z_xx

            this->z[f_idx].d[xy][i] =  ( ( this->ac_spectra[f_idx].ac[pmt::hxrhx][i] * this->ac_spectra[f_idx].ac[pmt::exrhy][i] ) - ( this->ac_spectra[f_idx].ac[pmt::hxrhy][i] * this->ac_spectra[f_idx].ac[pmt::exrhx][i] ) )       / denom;  // Z_xy

            this->z[f_idx].d[yx][i] =  ( ( this->ac_spectra[f_idx].ac[pmt::hyrhy][i] * this->ac_spectra[f_idx].ac[pmt::eyrhx][i] ) - ( (this->ac_spectra[f_idx].ac[pmt::hyrhx][i]) * this->ac_spectra[f_idx].ac[pmt::eyrhy][i] ) ) / denom;  // Z_yx

            this->z[f_idx].d[yy][i] =  ( ( this->ac_spectra[f_idx].ac[pmt::hxrhx][i] * this->ac_spectra[f_idx].ac[pmt::eyrhy][i] ) - ( this->ac_spectra[f_idx].ac[pmt::hxrhy][i] * this->ac_spectra[f_idx].ac[pmt::eyrhx][i] ) )       / denom;  // Z_yy


            //            std::cout <<    ( this->ac_spectra[f_idx].ac[pmt::hxrhx][i] * this->ac_spectra[f_idx].ac[pmt::exrhy][i] ) << ( this->ac_spectra[f_idx].ac[pmt::hxrhy][i] * this->ac_spectra[f_idx].ac[pmt::exrhx][i] )  << Qt::endl;    // Z_xy

            //            std::cout <<  ( this->ac_spectra[f_idx].ac[pmt::hyrhy][i] * this->ac_spectra[f_idx].ac[pmt::eyrhx][i] ) << ( conj(this->ac_spectra[f_idx].ac[pmt::hxrhy][i]) * this->ac_spectra[f_idx].ac[pmt::eyrhy][i] )  << Qt::endl;  // Z_yx

            //            std::cout << denom << Qt::endl;

            //            std::cout <<  ( this->ac_spectra[f_idx].ac[pmt::hxhx][i] * this->ac_spectra[f_idx].ac[pmt::exhy][i] ) << ( this->ac_spectra[f_idx].ac[pmt::hxhy][i] * this->ac_spectra[f_idx].ac[pmt::exhx][i] )  << Qt::endl;  // Z_xy

            //            std::cout <<  ( this->ac_spectra[f_idx].ac[pmt::hyhy][i] * this->ac_spectra[f_idx].ac[pmt::eyhx][i] ) << ( conj(this->ac_spectra[f_idx].ac[pmt::hxhy][i]) * this->ac_spectra[f_idx].ac[pmt::eyhy][i] ) << Qt::endl;  // Z_yx

            //            std::cout << denoml << Qt::endl;


            //                        this->z[f_idx].d[xx][i] =  ( ( this->ac_spectra[f_idx].ac[pmt::hyrhy][i] * this->ac_spectra[f_idx].ac[pmt::exhx][i] ) - ( conj(this->ac_spectra[f_idx].ac[pmt::hxrhy][i]) * this->ac_spectra[f_idx].ac[pmt::exhy][i] ) ) / denom;  // Z_xx

            //                        this->z[f_idx].d[xy][i] =  ( ( this->ac_spectra[f_idx].ac[pmt::hxrhx][i] * this->ac_spectra[f_idx].ac[pmt::exhy][i] ) - ( this->ac_spectra[f_idx].ac[pmt::hxrhy][i] * this->ac_spectra[f_idx].ac[pmt::exhx][i] ) )       / denom;  // Z_xy

            //                        this->z[f_idx].d[yx][i] =  ( ( this->ac_spectra[f_idx].ac[pmt::hyrhy][i] * this->ac_spectra[f_idx].ac[pmt::eyhx][i] ) - ( conj(this->ac_spectra[f_idx].ac[pmt::hxrhy][i]) * this->ac_spectra[f_idx].ac[pmt::eyhy][i] ) ) / denom;  // Z_yx

            //                        this->z[f_idx].d[yy][i] =  ( ( this->ac_spectra[f_idx].ac[pmt::hxrhx][i] * this->ac_spectra[f_idx].ac[pmt::eyhy][i] ) - ( this->ac_spectra[f_idx].ac[pmt::hxrhy][i] * this->ac_spectra[f_idx].ac[pmt::eyhx][i] ) )       / denom;  // Z_yy



            if (has_t) {
                this->z[f_idx].d[tx][i] = ( ( this->ac_spectra[f_idx].ac[pmt::hxhz][i] * this->ac_spectra[f_idx].ac[pmt::hyhy][i] ) -
                        ( this->ac_spectra[f_idx].ac[pmt::hyhz][i] * conj(this->ac_spectra[f_idx].ac[pmt::hxhy][i]) ) ) / denom;

                this->z[f_idx].d[ty][i] = ( ( this->ac_spectra[f_idx].ac[pmt::hyhz][i] * this->ac_spectra[f_idx].ac[pmt::hxhx][i] ) -
                        ( this->ac_spectra[f_idx].ac[pmt::hxhz][i] * this->ac_spectra[f_idx].ac[pmt::hxhy][i] ) ) / denom;
            }
        }
    }
}

// **********************************************************************************************************************


void mt_site::calc_tipper()
{
    if (this->ac_spectra.at(0).ac.at(pmt::hxhz).size() && this->ac_spectra.at(0).ac.at(pmt::hyhz).size()) {

        std::vector<std::future<void>> futures;

        for ( auto &spc : get_freqs_acsp(this->ac_spectra)) {
            futures.emplace_back(std::async(&mt_site::calc_tipper_vector, this, spc));
        }
        for(auto &f : futures) {
            f.get();
        }

    }
}

void mt_site::calc_tipper_vector(const double &f)
{
    size_t i, stacks, f_idx, j;
    stacks = f_size_a(this->ac_spectra, f, f_idx);
    if (f_idx == SIZE_MAX) return;
    qDebug() << "calculating tipper, freq" << doublefreq2string_unit(f) << " stacks:" << stacks;
    for (i = 0; i < stacks; ++i) {
        std::complex<double> denom =  ( (this->ac_spectra[f_idx].ac[pmt::hxrhx][i] * this->ac_spectra[f_idx].ac[pmt::hyrhy][i] ) -
                (this->ac_spectra[f_idx].ac[pmt::hxrhy][i] * conj(this->ac_spectra[f_idx].ac[pmt::hxrhy][i]))  );

        //!< @todo mark forever the boolean - or elimnate or what
        // check NULL
        if (denom.real() == 0.0 && denom.imag() == 0.0) {
            for (j = 0; j < this->z[f_idx].d.size(); ++j) {
                this->z[f_idx].b[j][i] = false;
                // this->z[f_idx].d[j][i] = std::complex<double> (0.0, 0.0);
            }
        }
        else {

            this->z[f_idx].d[tx][i] = ( ( this->ac_spectra[f_idx].ac[pmt::hxhz][i] * this->ac_spectra[f_idx].ac[pmt::hyhy][i] ) -
                    ( this->ac_spectra[f_idx].ac[pmt::hyhz][i] * conj(this->ac_spectra[f_idx].ac[pmt::hxhy][i]) ) ) / denom;

            this->z[f_idx].d[ty][i] = ( ( this->ac_spectra[f_idx].ac[pmt::hyhz][i] * this->ac_spectra[f_idx].ac[pmt::hxhx][i] ) -
                    ( this->ac_spectra[f_idx].ac[pmt::hxhz][i] * this->ac_spectra[f_idx].ac[pmt::hxhy][i] ) ) / denom;

        }


    }



}


// **********************************************************************************************************************


void mt_site::slot_ssp_stack_all()
{

    qDebug() << "mt_site::slot_ssp_stack_all starting";
    this->ssp_threads.clear();
    this->freqs = get_freqs_acsp(this->ac_spectra); // all frequencies
    size_t all = this->freqs.size();
    size_t i = 0, j;
    for (;;) {
        for (j = 0; j < this->num_threads; ++j) {
            if ( i < all) this->ssp_threads.emplace_back( std::thread (&mt_site::ssp_stack_all_vector, this, freqs.at(i++)) );
        }
        for (auto &thrd : this->ssp_threads) {
            thrd.join();
        }
        this->ssp_threads.clear();
        if (i == all) break;
    }
    this->ssp_threads.clear();
    qDebug() << "mt_site::slot_ssp_stack_all finished";

}


void mt_site::ssp_stack_all_vector(const double freq)
{

    size_t i = 0, stacks, f_idx;
    double f = freq;

    stacks = f_size_a(this->ac_spectra, f, f_idx);

    if (f_idx == SIZE_MAX) return;

    for (auto &data : this->ac_spectra[f_idx].ac) {
        if (data.size()) {
            auto itb = this->ac_spectra[f_idx].ac_sel[i];
            std::vector<double> ampls = (this->ac_spectra[f_idx].ampl_spc_if(i));
            if (ampls.size()) {
                std::unique_ptr<two_pass_variance> tplrg_sel = std::make_unique<two_pass_variance>();
                this->ac_spectra[f_idx].ac_vals[i] = tplrg_sel->variance(ampls.cbegin(), ampls.cend(), 0.0);
            }
        }
        ++i;
    }
}


// **********************************************************************************************************************

void mt_site::slot_ssp_stack_half_csem(const int &first_1_second_2_all_3)
{
    qDebug() << "mt_site::slot_ssp_stack_half_csem starting";

    std::vector<std::future<void>> futures;

    for ( auto &spc : get_freqs_acsp(this->ac_spectra)) {
        futures.emplace_back(std::async(&mt_site::ssp_stack_half_csem_vector, this, spc, first_1_second_2_all_3));
    }
    for(auto &f : futures) {
        f.get();
    }


    qDebug() << "mt_site::slot_ssp_stack_half_csem finished";

}

void mt_site::ssp_stack_half_csem_vector(const double freq, const int &first_1_second_2_all_3)
{
    size_t i = 0, stacks, f_idx;
    double f = freq;

    stacks = f_size_s(this->ac_spectra, f, f_idx);

    if (f_idx == SIZE_MAX) return;
    // acsp::ssp_spc_if(const bool real_false_imag_true, const int first_1_second_2_all_3, const size_t chan_type) const

    for (auto &data : this->ac_spectra[f_idx].ssp) {
        if (data.size()) {
            std::vector<double> vals = (this->ac_spectra[f_idx].ssp_spc_if(false, first_1_second_2_all_3, i));
            if (vals.size()) {
                std::unique_ptr<two_pass_variance> tplrg_sel = std::make_unique<two_pass_variance>();
                if ((first_1_second_2_all_3 == 1) || (first_1_second_2_all_3 == 3)) {
                    this->ac_spectra[f_idx].ssp_vals[i] = tplrg_sel->variance(vals.cbegin(), vals.cend(), 0.0);
                }
                else {
                    this->ac_spectra[f_idx].ssp_vals_2[i] = tplrg_sel->variance(vals.cbegin(), vals.cend(), 0.0);
                }
            }
        }
        ++i;
    }

    i = 0;
    for (auto &data : this->ac_spectra[f_idx].ssp) {
        if (data.size()) {
            std::vector<double> vals = (this->ac_spectra[f_idx].ssp_spc_if(true, first_1_second_2_all_3, i));
            if (vals.size()) {
                std::unique_ptr<two_pass_variance> tplrg_sel = std::make_unique<two_pass_variance>();
                if ((first_1_second_2_all_3 == 1) || (first_1_second_2_all_3 == 3)) {
                    this->ac_spectra[f_idx].ssp_vals_imag[i] = tplrg_sel->variance(vals.cbegin(), vals.cend(), 0.0);
                }
                else {
                    this->ac_spectra[f_idx].ssp_vals_imag_2[i] = tplrg_sel->variance(vals.cbegin(), vals.cend(), 0.0);
                }
            }
        }
        ++i;
    }


}


// **********************************************************************************************************************

void mt_site::slot_calc_auto_spc()
{


    if (!this->ssp_size()) return;

    qDebug() << "mt_site::slot_calc_auto_spc starting";
    this->ac_threads.clear();
    this->freqs = get_freqs_acsp(this->ac_spectra); // all frequencies
    size_t all = this->freqs.size();
    size_t i = 0, j;
    for (;;) {
        for (j = 0; j < this->num_threads; ++j) {
            if ( i < all) this->ac_threads.emplace_back( std::thread (&mt_site::calc_auto_spc_vector, this, freqs.at(i++)) );
        }
        for (auto &thrd : this->ac_threads) {
            thrd.join();
        }
        this->ac_threads.clear();
        if (i == all) break;
    }
    this->ac_threads.clear();
    qDebug() << "mt_site::slot_calc_auto_spc finished";

}

void mt_site::calc_auto_spc_vector(const double& f)
{
    size_t idx = f_index_acsp(this->ac_spectra, f);
    this->ac_spectra[idx].calc_auto_spc();
}


void mt_site::slot_calc_ac_spc()
{
    if (!this->ssp_size()) return;
    qDebug() << "mt_site::slot_calc_ac_spc starting";
    this->ac_threads.clear();
    this->freqs = get_freqs_acsp(this->ac_spectra); // all frequencies
    size_t all = this->freqs.size();
    size_t i = 0, j;
    for (;;) {
        for (j = 0; j < this->num_threads; ++j) {
            if ( i < all) this->ac_threads.emplace_back( std::thread (&mt_site::calc_ac_spc_vector, this, freqs.at(i++)) );
        }
        for (auto &thrd : this->ac_threads) {
            thrd.join();
        }
        this->ac_threads.clear();
        if (i == all) break;
    }
    this->ac_threads.clear();
    qDebug() << "mt_site::slot_calc_ac_spc finished";
}


void mt_site::calc_ac_spc_vector(const double &f)
{
    size_t idx = f_index_acsp(this->ac_spectra, f);
    this->ac_spectra[idx].calc_ac_spc();
}

// **********************************************************************************************************************


void mt_site::slot_dump_ampl_spc(bool error_bars)
{
    if (!this->ac_spectra.size()) return;

    size_t spce = 0; // for each type ExEx ... RHzRHz
    this->asciiwriterthreads.clear();
    for (auto &acv : this->ac_spectra.at(0).ac) {
        // if we have ... HyHz for example, create a writer thread for HyHz, passing the
        if (acv.size()) {
            this->asciiwriterthreads.emplace_back( std::thread (&mt_site::ssp_dump_stack_all_vector, this, spce, error_bars) );
        }
        ++spce;
    }

    for (auto &thrd : this->asciiwriterthreads) {
        thrd.join();
    }
    this->asciiwriterthreads.clear();

}
void mt_site::ssp_dump_stack_all_vector(const size_t &spce, bool error_bars)
{
    //    this->z_mutex.lock();
    //    std::thread::id this_id = std::this_thread::get_id();
    //    std::cerr << "thread mt_site::ssp_dump_stack_all_vector: " << this_id << Qt::endl;
    //    this->z_mutex.unlock();

    QString name (this->dump_path);
    QString ext("_ampl.dat");
    name.append("/" + pmt::ac_spectra_names.at(int(spce)));
    name.append(ext);

    QFile qfi(name);
    QTextStream out(&qfi);

    // check mapsize

    if (qfi.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (error_bars) {
            for (auto &ac : this->ac_spectra) {
                if(ac.ac_vals[spce].size()) out << ac.f << "  " << (ac.ac_vals[spce].at(g_stat::mean_x)) << "  " << (ac.ac_vals[spce].at(g_stat::stddev_x)) << Qt::endl;
            }
        }
        else {
            for (auto &ac : this->ac_spectra) {
                if(ac.ac_vals[spce].size()) out << ac.f << "   " << (ac.ac_vals[spce].at(g_stat::mean_x)  )<< Qt::endl;
            }
        }
    }
    qfi.close();

}

// **********************************************************************************************************************

void mt_site::slot_dump_ssp_coherency(bool error_bars)
{
    if (!this->ac_spectra.size()) return;

    size_t spce = 0; // for each type ExEx ... RHzRHz
    this->asciiwriterthreads.clear();
    for (auto &acv : this->ac_spectra.at(0).coh) {
        // if we have ... HyHz for example, create a writer thread for HyHz, passing the
        if (acv != DBL_MAX) {
            this->asciiwriterthreads.emplace_back( std::thread (&mt_site::ssp_dump_coherency_all_vector, this, spce) );
        }
        ++spce;
    }

    for (auto &thrd : this->asciiwriterthreads) {
        thrd.join();
    }
    this->asciiwriterthreads.clear();

}

void mt_site::ssp_dump_coherency_all_vector(const size_t &spce)
{
    QString name (this->dump_path);
    name.append("/" + pmt::ac_spectra_names.at(int(spce)));
    name.append("_coh.dat");

    QFile qfi(name);
    QTextStream out(&qfi);

    if (qfi.open(QIODevice::WriteOnly | QIODevice::Text)) {
        for (auto &ac : this->ac_spectra) {
            if(ac.coh[spce] != DBL_MAX) out << ac.f << "   " << ac.coh[spce] << Qt::endl;
        }
    }
    qfi.close();
}


// **********************************************************************************************************************


void mt_site::join_dump_threads()
{
    for (auto &thrd : this->asciiwriterthreads) {
        thrd.join();
    }
    this->asciiwriterthreads.clear();

}

size_t mt_site::size() const
{
    size_t sz = 0;
    for (auto &ac : this->ac_spectra) {
        if (ac.size()) ++sz;
    }

    return sz;
}

size_t mt_site::ssp_size() const
{
    size_t sz = 0;
    for (auto &ac : this->ac_spectra) {
        if (ac.ssp_size()) ++sz;
    }

    return sz;
}

void mt_site::set_z_from_last_proc(const mt_data_res<std::complex<double> > &zet_old)
{
    this->zet_last_proc = zet_old;
}







void mt_site::fill_prc_com_with_base_values()
{

    // use in main modules  this->cmdline->append_data_from_other(this->mtsite->get_data(), "mt_site_");

    this->insert("processing", "empty");

    // ///////////////////
    this->insert("activate_median_rho_phi", int(0));

    this->insert_double("upper_median_phi", double(1.5));
    this->insert_double("upper_median_rho", double(1.3));

    this->insert_double("lower_median_phi", double(1.5));
    this->insert_double("lower_median_rho", double(1.3));

    // ///////////////////

    this->insert("activate_phi_limits", int(0));
    this->insert("activate_rho_limits", int(0));

    this->insert_double("upper_rejection_phi", double(0));
    this->insert_double("upper_rejection_rho", double(0));

    this->insert_double("lower_rejection_phi", double(0));
    this->insert_double("lower_rejection_rho", double(0));

    // ///////////////////

    this->insert("activate_ct", int(0));

    this->insert("upper_rejection_coh", double(1.000));    //!< if 0.98 data will not process perfect data - which could be synced spikes
    this->insert("lower_rejection_coh", double(0.3));      //!< that is maybe no data

    // ///////////////////

    this->insert_int("xstddev_active", 1);
    this->insert("xstddev", double(1.7));
    this->insert("xstddev_min", double(1.0));

    this->insert_int("xstddev_auto_active", 0);

    this->insert("recalc_stddev", int(1));
    this->insert("replace_stat_with_stack_all", int(0));

    // ///////////////////

    this->insert_int("activate_quadrant_check", int(0));

    // ///////////////////

    this->insert_szt("tns_sel", tns_sel::xy_yx);  // booloean sync is driven by xy_yx

    this->insert_szt("min_stacks", _min_stacks);

    this->insert_szt("huber_iterations", 20);

    this->insert("remove_auto_residuals", int(0) );
    this->insert("remove_rr_residuals", int(0) );



}



std::vector<double> mt_site::get_freqs() const
{

    return this->freqs;
}







void mt_site::stack_all_acspectra_vector(const double &f)
{

}



void mt_site::calc_parallel_coherency_vector(const double& f)
{
}



/*

void mt_site::append_spectra_collector(std::shared_ptr<spectra_collector> &spcl, const std::vector<prc_com> &prcs)
{

    std::cout << "mtsite receive" << Qt::endl;

    // types, maps pf frequencies


    // f and spcl->freqs are index and corresponding vector
    size_t spc_pos, f, acs = pmt::ac_spectra_names.size();
    // for all ExEX, ExEy .....
    for (spc_pos = 0; spc_pos < acs; ++spc_pos) {
        // spcl is alway a full matrix
        //                       spectra    f    stacks
        f = 0;
        for ( auto &freq : spcl->freqs) {
            size_t hits = 0;
            for (auto &ac : this->tmp_ac_spectra) {
                if (freq == ac.f) {
                    qDebug() << "check vs" << freq << ac.f;
                    ac.add(freq, spc_pos, spcl->all_spc_data[spc_pos][f]);
                    hits++;
                }
            }
            if (hits == 0) {
                this->tmp_ac_spectra.push_back(acsp());
                this->tmp_ac_spectra.back().add(freq, spc_pos, spcl->all_spc_data[spc_pos][f]);
            }
            ++f;
        }
    }

    // only need the first set for the EDI
    if (!this->in_ats_prc_coms.size()) {

        this->in_ats_prc_coms = prcs;
    }

    // and that fpr ACQDATE=08/20/09 13:22:03  ENDDATE=08/21/09 06:59:58
    for (auto &prc : prcs) {
        this->start_date_times.push_back(prc.qint64value("start_date_time"));
        this->stop_date_times.push_back(prc.qint64value("stop_date_time"));
    }

    std::sort(this->start_date_times.begin(),this->start_date_times.end() );
    std::sort(this->stop_date_times.begin(),this->stop_date_times.end() );

    for (auto &prc : this->in_ats_prc_coms) {
        prc.set_key_value("start_date_time", this->start_date_times.front() );
        prc.set_key_value("stop_date_time", this->stop_date_times.back() );

    }


    qDebug() << "mtsite receive finished";
}
*/


void mt_site::calc_rho_phi()
{
    if ((this->mymttye >= mttype::mt) && this->mymttye <= mttype::emap_rr) {

        // all frequencies
        std::vector<std::future<void>> futures;
        this->rho.resize(tns::tns_size,freqs);
        this->phi.resize(tns::tns_size,freqs);

        for ( auto &fs : get_freqs_acsp(this->ac_spectra)) {
            futures.emplace_back(std::async(&mt_site::calc_rho_phi_vector, this, fs));
        }
        for(auto &f : futures) {
            f.get();
        }
    }
}

void mt_site::calc_rho_phi_vector(const double &f)
{
    size_t f_idx(f_index_c(this->z, f));
    if (f_idx == SIZE_MAX) return;
    // this->z_mutex.unlock();
    //

    this->z[f_idx].count_true_selections();

    math_vector mv;

    size_t i = 0;
    auto itb = this->z[f_idx].b[i];
    // .d has isze of 0..3 or ..5 with Hz
    for (i = 0; i < this->z[f_idx].d.size(); ++i) {
        if (i < tns_size) {
            if (this->z[f_idx].trues[i] >= this->sztvalue("min_stacks")) {

                qDebug() << "processing calc_rho_phi_vector" << doublefreq2string_unit(f) << "selected:" << this->z[f_idx].trues[i];
                std::unique_ptr<two_pass_variance> tplrg_selr = std::make_unique<two_pass_variance>();
                std::vector<double> data = this->z[f_idx].rho_vector(i);
                this->rho.gstat[i][f_idx] = tplrg_selr->variance_if(data.cbegin(), data.cend(),itb.cbegin(),itb.cend(), 0.0);
                this->rho.gstat[i][f_idx][g_stat::median_x] = iter::median_if<double>(data.cbegin(), data.cend(), itb.cbegin(), itb.cend());
                std::unique_ptr<two_pass_variance> tplrg_selp = std::make_unique<two_pass_variance>();
                data = this->z[f_idx].phi_vector_deg(i);
                this->phi.gstat[i][f_idx] = tplrg_selp->variance_if(data.cbegin(), data.cend(),itb.cbegin(),itb.cend(), 0.0);
                this->phi.gstat[i][f_idx][g_stat::median_x] = iter::median_if<double>(data.cbegin(), data.cend(), itb.cbegin(), itb.cend());

            }
        }
    }

}
// **********************************************************************************************************************

void mt_site::huber(const double hlimit, const size_t iter_limit)
{
    std::vector<double> freqs(get_freqs_acsp(this->ac_spectra));
    if (this->ac_spectra.at(0).ac.at(pmt::hxhz).size() && this->ac_spectra.at(0).ac.at(pmt::hyhz).size()) {
        this->zet.resize(tns::tns_tip_size,freqs);
    }
    else this->zet.resize(tns::tns_size,freqs);

    std::vector<std::future<void>> futures;
    for ( auto &fs : freqs) {
        futures.emplace_back(std::async(&mt_site::huber_vector, this, fs, hlimit, iter_limit));
    }

    for(auto &f : futures) {
        f.get();
    }

    qDebug() << "huber weights finished, recalc auto & cross spectra";
    this->slot_calc_ac_spc();
}

void mt_site::huber_vector(const double &f, const double hlimit, const size_t iter_limit)
{


    size_t idx = f_index_acsp(this->ac_spectra, f);
    qDebug() << "huber weights " << f;


    int component = pmt::ex;
    for (int zz = 0; zz < 2; ++zz) {

        if (zz == 0) component = pmt::ex;
        else component = pmt::ey;

        std::vector<std::complex<double>> Exy_iter = this->ac_spectra[idx].ssp[component];
        size_t n = Exy_iter.size();
        std::vector<std::complex<double>> Exy_pred(Exy_iter.size());
        std::vector<std::complex<double>> d(2);
        std::vector<std::complex<double>> mest(2);
        std::vector<std::complex<double>> e(n);
        std::vector<std::complex<double>> w(n);
        std::complex<double> sigma;



        for (size_t i = 0; i < iter_limit; ++i) {

            d = make_d(this->ac_spectra[idx].ssp[pmt::hx], this->ac_spectra[idx].ssp[pmt::hy], Exy_iter);
            mest = make_mest(this->ac_spectra[idx].ssp[pmt::hx], this->ac_spectra[idx].ssp[pmt::hy], this->ac_spectra[idx].ssp[pmt::hx], this->ac_spectra[idx].ssp[pmt::hy],  d);

            for (size_t j = 0; j < n; ++j) {
                Exy_pred[j] = mest[0] * this->ac_spectra[idx].ssp[pmt::hx][j] + mest[1] * this->ac_spectra[idx].ssp[pmt::hy][j];
            }

            for (size_t j = 0; j < n; ++j) {
                e[j] = Exy_iter[j] - Exy_pred[j];
            }


            std::unique_ptr<two_pass_variance_cplx> tplrg_sel = std::make_unique<two_pass_variance_cplx>();
            tplrg_sel->variance(e.cbegin(), e.cend(), 0.0);
            sigma = tplrg_sel->d_stddev;

            size_t j = 0;
            for (auto &es : e) {
                if (1.58 * std::abs(sigma) > abs(es) ) w[j] = std::complex<double> (1.0, 0.0);
                else w[j] = 1.58 * sigma / es;
                ++j;
            }
            for (size_t j = 0; j < n; ++j) {
                Exy_iter[j] = Exy_pred[j] + w[j] * e[j];
            }

        }


        this->z_mutex.lock();
        this->ac_spectra[idx].set_ssp_chan(component, Exy_iter);
        this->z_mutex.unlock();

    }
}


// **********************************************************************************************************************

void mt_site::stack_all()
{
    if ((this->mymttye >= mttype::mt) && this->mymttye <= mttype::emap_rr) {

        // allocate memory
        this->zet.clear();
        std::vector<double> freqs(get_freqs_acsp(this->ac_spectra));
        if (this->ac_spectra.at(0).ac.at(pmt::hxhz).size() && this->ac_spectra.at(0).ac.at(pmt::hyhz).size()) {
            this->zet.resize(tns::tns_tip_size,freqs);
        }
        else this->zet.resize(tns::tns_size,freqs);

        std::vector<std::future<void>> futures;
        for ( auto &fs : freqs) {
            futures.emplace_back(std::async(&mt_site::stack_all_vector, this, fs));
        }

        for(auto &f : futures) {
            f.get();
        }
    }
}




void mt_site::stack_all_vector(const double &f)
{

    // all components xx .. yy  ztensf[xy]map(f, vector complex)
    // ztensf has first 0, 1,2,3 , the second is the freq, vector<complex> pair

    // this->z_mutex.lock();
    // here we iterate over public base vector, shared by all threads
    size_t f_idx(f_index_c(this->z, f));
    if (f_idx == SIZE_MAX) return;
    // this->z_mutex.unlock();
    //

    this->z[f_idx].count_true_selections();

    math_vector mv;

    size_t i = 0;
    size_t selected = 0;
    // .d has isze of 0..3 or ..5 with Hz
    for (auto &ztensf : this->z[f_idx].d) {

        auto itb = this->z[f_idx].b[i];
        if (this->z[f_idx].trues[i] >= this->sztvalue("min_stacks")) {
            selected = this->z[f_idx].trues[i];
            if (!i) qDebug() << "processing stack all z" << doublefreq2string_unit(f) << "selected:" << selected;
            // give me the vector for f : ztensf.second[f])
            std::unique_ptr<two_pass_variance_cplx> tplrg_sel = std::make_unique<two_pass_variance_cplx>();

            // first run could be xx, second xy ...
            this->zet.gstat[i][f_idx] = tplrg_sel->variance_if(ztensf.cbegin(), ztensf.cend(),itb.cbegin(), itb.cend(), 0.0);
            //this->zet.gstat[i][f_idx] = var_data;
            this->zet.d[i][f_idx] = tplrg_sel->d_mean;
            this->zet.err[i][f_idx] = this->zet.gstat[i][f_idx][g_stat::stddev_x];
            if ((i == xy) || (i == yx)) {
                // catch the vector
                auto itc = this->z[f_idx].coh[i];
                this->zet.coh[i][f_idx] = mv.dmean(itc.cbegin(), itc.cend(), itb.cbegin(), itb.cend());

            }

        }
        else {
            qDebug() << "processing stack all z" << doublefreq2string_unit(f) << " NOT processesd, less than" << this->sztvalue("min_stacks") << " values left over !!!, component" << i;
            this->zet.err[i][f_idx] = DBL_MAX;

        }

        ++i;

    }

    // we may not be able to send this signal same time from multiple threads
    this->z_mutex.lock();
    emit this->signal_general_msg("mt_site stack", doublefreq2string_unit(this->z[f_idx].f), QString::number(selected));

    this->z_mutex.unlock();

}

// **********************************************************************************************************************

void mt_site::xstddev(const size_t what)
{
    qDebug() << "xstddev starting";
    std::vector<std::future<void>> futures;
    this->freqs = get_freqs_acsp(this->ac_spectra); // all frequencies

    for ( auto &fs : freqs) {
        futures.emplace_back(std::async(&mt_site::xstddev_vector, this, fs));
    }

    for(auto &f : futures) {
        f.get();
    }


    this->slot_update_booleans(what);
    qDebug() << "xstddev finished";

}




void mt_site::xstddev_vector(const double &f)
{
    size_t f_idx = f_index_c(this->z, f);
    if (f_idx == SIZE_MAX) return;

    this->z[f_idx].count_true_selections();

    // all components xx, xy ...
    size_t i;
    size_t sz = this->z[f_idx].d.size(); // size of tensor (e.g. 4)

    std::vector<size_t> in_trues(sz), out_trues(sz);

    for (i = 0; i < sz; ++i) {
        in_trues[i] = this->z[f_idx].trues[i];
    }


    double xstd_dev = this->dvalue("xstddev");

    for (i = 0; i < sz; ++i) {

        auto itzb = this->z[f_idx].d.at(i).cbegin();
        auto itze = this->z[f_idx].d.at(i).cend();
        auto itbb = this->z[f_idx].b.at(i).begin(); // b has always same size!

        while (itzb != itze) {
            double drp = this->zet.d[i][f_idx].real() + (this->zet.err[i][f_idx] * xstd_dev);
            double drm = this->zet.d[i][f_idx].real() - (this->zet.err[i][f_idx] * xstd_dev);
            double dip = this->zet.d[i][f_idx].imag() + (this->zet.err[i][f_idx] * xstd_dev);
            double dim = this->zet.d[i][f_idx].imag() - (this->zet.err[i][f_idx] * xstd_dev);

            if (((*itzb).real() > drp)  || ((*itzb).real() < drm) || ((*itzb).imag() < dim) || ((*itzb).imag() > dip)) {
                *itbb = false;
            }
            ++itzb;
            ++itbb;
        }
    }
    this->z[f_idx].count_true_selections();

    for (i = 0; i < sz; ++i) {
        out_trues[i] = this->z[f_idx].trues[i];
    }


    QString upl1 ("mt_site xstddev f: " + doublefreq2string_unit(this->z[f_idx].f) + "in: ");
    QString upl2;

    for (i = 0; i < sz; ++i) {
        if (i < sz - 1) {
            upl2.append(QString::number(in_trues.at(i)) + ", ");
        }
        else  upl2.append(QString::number(in_trues.at(i)) + " ");
    }

    QString upl3("out: ");

    QString upl4;
    for (i = 0; i < sz; ++i) {
        if (i < sz - 1) {
            upl4.append(QString::number(in_trues.at(i)) + ", ");
        }
        else  upl4.append(QString::number(in_trues.at(i)) + " ");
    }

    qDebug() << (upl1 +  upl2 + upl3 + upl4);

    this->z_mutex.lock();
    emit this->signal_general_msg_4strs(upl1, upl2, upl3, upl4);
    this->z_mutex.unlock();
}



// **********************************************************************************************************************

void mt_site::xstddev_auto(const size_t what)
{
    qDebug() << "xstddev auto starting";
    std::vector<std::future<void>> futures;
    this->freqs = get_freqs_acsp(this->ac_spectra); // all frequencies

    for ( auto &fs : freqs) {
        futures.emplace_back(std::async(&mt_site::xstddev_vector_auto, this, fs));
    }

    for(auto &f : futures) {
        f.get();
    }


    this->slot_update_booleans(what);
    qDebug() << "xstddev auto finished";
}

void mt_site::xstddev_vector_auto(const double &f)
{

    size_t f_idx = f_index_c(this->z, f);
    if (f_idx == SIZE_MAX) return;

    size_t min_size = 32;
    size_t sz = this->z[f_idx].d.size(); // size of tensor (e.g. 4)
    // all components xx, xy ...
    size_t i;

    this->z[f_idx].count_true_selections();

    std::vector<size_t> in_trues(sz), out_trues(sz);
    std::vector<std::size_t> sel_in(sz);
    std::vector<std::size_t> sel_out(sz);


    i = 0;
    for (i = 0; i < sz; ++i) {
        in_trues[i] = this->z[f_idx].trues[i];
        sel_in[i]   = this->z[f_idx].trues[i];
        sel_out[i]   = this->z[f_idx].trues[i];
    }

    double xstd_dev;
    double xstd_dev_min;
    for (i = 0; i < sz; ++i) {
        xstd_dev = this->dvalue("xstddev");
        xstd_dev_min = this->dvalue("xstddev_min");
        while (sel_out[i] > min_size && (xstd_dev > xstd_dev_min)) {
            this->z[f_idx].backup_booleans();

            auto itzb = this->z[f_idx].d.at(i).cbegin();
            auto itze = this->z[f_idx].d.at(i).cend();
            auto itbb = this->z[f_idx].b.at(i).begin(); // b has always same size!

            while (itzb != itze) {
                double drp = this->zet.d[i][f_idx].real() + (this->zet.err[i][f_idx] * xstd_dev);
                double drm = this->zet.d[i][f_idx].real() - (this->zet.err[i][f_idx] * xstd_dev);
                double dip = this->zet.d[i][f_idx].imag() + (this->zet.err[i][f_idx] * xstd_dev);
                double dim = this->zet.d[i][f_idx].imag() - (this->zet.err[i][f_idx] * xstd_dev);

                if (((*itzb).real() > drp)  || ((*itzb).real() < drm) || ((*itzb).imag() < dim) || ((*itzb).imag() > dip)) {
                    *itbb = false;
                }
                ++itzb;
                ++itbb;
            }

            this->z[f_idx].count_true_selections();
            sel_out[i] = this->z[f_idx].trues[i];
            xstd_dev -= 0.1;

        }
        this->z[f_idx].restore_booleans();
    }
    this->z[f_idx].count_true_selections();
    for (i = 0; i < sz; ++i) {
        out_trues[i] = this->z[f_idx].trues[i];
    }



    QString upl1 ("mt_site xstddev f: " + doublefreq2string_unit(this->z[f_idx].f) + "in: ");

    QString upl2;
    for (i = 0; i < sz; ++i) {
        if (i < sz - 1) {
            upl2.append(QString::number(in_trues.at(i)) + ", ");
        }
        else  upl2.append(QString::number(in_trues.at(i)) + " ");
    }

    QString  upl3 ("out: ");

    QString  upl4;
    for (i = 0; i < sz; ++i) {
        if (i < sz - 1) {
            upl4.append(QString::number(in_trues.at(i)) + ", ");
        }
        else  upl4.append(QString::number(in_trues.at(i)) + " ");
    }
    qDebug() << (upl1 +  upl2 + upl3 + upl4);

    this->z_mutex.lock();
    emit this->signal_general_msg_4strs(upl1, upl2, upl3, upl4);
    this->z_mutex.unlock();


}


// **********************************************************************************************************************

void mt_site::ssp_xstddev()
{
    qDebug() << "ssp xstddev starting";
    std::vector<std::future<void>> futures;
    this->freqs = get_freqs_acsp(this->ac_spectra); // all frequencies

    for ( auto &fs : freqs) {
        futures.emplace_back(std::async(&mt_site::ssp_xstddev_vector, this, fs));
    }

    for(auto &f : futures) {
        f.get();
    }


    qDebug() << "ssp xstddev finished";
}



void mt_site::ssp_xstddev_vector(const double &freq)
{
    size_t i = 0, stacks, f_idx;
    double f = freq;

    stacks = f_size_a(this->ac_spectra, f, f_idx);

    if (f_idx == SIZE_MAX) return;

    double xstd = this->dvalue("xstddev");

    for (auto &data : this->ac_spectra[f_idx].ac) {
        if (data.size()) {
            auto itb = this->ac_spectra[f_idx].ac_sel[i].begin();
            std::vector<double> ampls(this->ac_spectra[f_idx].ac_sel[i].size());
            size_t jj = 0;
            for (auto &cplx : data) ampls[jj++] = (std::sqrt(std::abs(cplx)));
            if (ampls.size()) {
                for (auto &amp : ampls) {
                    double lower = this->ac_spectra[f_idx].ac_vals[i].at(g_stat::mean_x) - this->ac_spectra[f_idx].ac_vals[i].at(g_stat::stddev_x) * xstd;
                    double upper = this->ac_spectra[f_idx].ac_vals[i].at(g_stat::mean_x) + this->ac_spectra[f_idx].ac_vals[i].at(g_stat::stddev_x) * xstd;
                    if ((amp > upper) || (amp  < lower)) {
                        *itb = false;
                    }
                    ++itb;
                }
            }
        }
        ++i;
    }
}


// **********************************************************************************************************************


void mt_site::ssp_normalize_by_E()
{
    qDebug() << "ssp xstddev starting";
    std::vector<std::future<void>> futures;
    this->freqs = get_freqs_acsp(this->ac_spectra); // all frequencies
    for ( auto &fs : freqs) {
        futures.emplace_back(std::async(&mt_site::ssp_normalize_by_E_vector, this, fs));
    }

    for(auto &f : futures) {
        f.get();
    }


    qDebug() << "ssp normalize by E finished";

}

void mt_site::ssp_normalize_by_E_vector(const double &freq)
{
    size_t i, stacks, f_idx;
    double f = freq;

    stacks = f_size_s(this->ac_spectra, f, f_idx);

    if (f_idx == SIZE_MAX) return;

    this->ac_spectra[f_idx].normalize_ssp_all(pmt::ex);

    double xstd = this->dvalue("xstddev");

    i = 0;
    for (auto &data : this->ac_spectra[f_idx].ssp) {
        if (data.size()) {
            auto itb = this->ac_spectra[f_idx].ssp_sel[i];
            std::vector<double> ampls = (this->ac_spectra[f_idx].ssp_spc_ampls_if(i));
            if (ampls.size()) {
                std::unique_ptr<two_pass_variance> tplrg_sel = std::make_unique<two_pass_variance>();
                this->ac_spectra[f_idx].ssp_vals[i] = tplrg_sel->variance(ampls.cbegin(), ampls.cend(), 0.0);
            }
        }
        ++i;
    }

    i = 0;
    for (auto &data : this->ac_spectra[f_idx].ssp) {
        if (data.size()) {
            auto itb = this->ac_spectra[f_idx].ssp_sel[i].begin();
            std::vector<double> ampls(this->ac_spectra[f_idx].ssp_sel[i].size());
            size_t jj = 0;
            for (auto &cplx : data) ampls[jj++] = (std::sqrt(std::abs(cplx)));
            if (ampls.size()) {
                for (auto &amp : ampls) {
                    double lower = this->ac_spectra[f_idx].ssp_vals[i].at(g_stat::mean_x) - this->ac_spectra[f_idx].ssp_vals[i].at(g_stat::stddev_x) * xstd;
                    double upper = this->ac_spectra[f_idx].ssp_vals[i].at(g_stat::mean_x) + this->ac_spectra[f_idx].ssp_vals[i].at(g_stat::stddev_x) * xstd;
                    if ((amp > upper) || (amp  < lower)) {
                        *itb = false;
                    }
                    ++itb;
                }
            }
        }
        ++i;
    }






}


// **********************************************************************************************************************

void mt_site::prepare_parallel_coherency()
{

    qDebug() << "prepare_parallel_coherency starting";
    std::vector<std::future<void>> futures;
    for (size_t i = 0; i < this->ac_spectra.size(); ++i) {
        futures.emplace_back(std::async(&mt_site::prepare_parallel_coherency_vector, this, i));

    }

    for(auto &f : futures) {
        f.get();
    }

    qDebug() << "prepare_parallel_coherency finished";

}

void mt_site::prepare_parallel_coherency_vector(const size_t index)
{
    this->ac_spectra[index].cross_coherency_raw_spectra();
}

// **********************************************************************************************************************


void mt_site::coherency_threshold(const double cohlower, const double cohupper, const size_t what)
{
    if ((this->mymttye >= mttype::mt) && this->mymttye <= mttype::emap_rr) {

        // allocate memory

        std::vector<double> freqs(get_freqs_acsp(this->ac_spectra));
        std::vector<std::future<void>> futures;
        for ( auto &fs : freqs) {
            futures.emplace_back(std::async(&mt_site::coherency_vector, this, fs, cohlower, cohupper));
        }

        for(auto &f : futures) {
            f.get();
        }
    }

    this->slot_update_booleans(what);
}

void mt_site::coherency_vector(const double &f, const double &cohlower, const double &cohupper)
{
    //this->z_mutex.lock();
    // get the vector positions for this frequency
    size_t f_idx;
    size_t stacks = f_size_a(this->ac_spectra, f, f_idx);
    //this->z_mutex.unlock();
    if (f_idx == SIZE_MAX) return;
    // qDebug() << "processing coherency base all z" << f;
    double base_ct = cohlower;
    double ceil_ct = cohupper;

    size_t i = 0;
    int des = 0;
    int sel = 0;
    double cohxy, cohyx;

    for (i = 0; i < stacks; ++i) {
        if (this->z[f_idx].istrue_xx_xy_yx_yy(i)) {
            ++sel;
            cohxy = std::real(this->z[f_idx].d[xx][i] * std::conj(this->ac_spectra[f_idx].ac[pmt::exhx][i])
                    + this->z[f_idx].d[xy][i] * std::conj(this->ac_spectra[f_idx].ac[pmt::exhy][i]))
                    / (std::abs(this->ac_spectra[f_idx].ac[pmt::exex][i]));

            cohyx = std::real(this->z[f_idx].d[yx][i] * std::conj(this->ac_spectra[f_idx].ac[pmt::eyhx][i])
                    + this->z[f_idx].d[yy][i] * std::conj(this->ac_spectra[f_idx].ac[pmt::eyhy][i]))
                    / (std::abs(this->ac_spectra[f_idx].ac[pmt::eyey][i]));

            if (std::isnan(cohxy) || std::isnan(cohyx) || std::isinf(cohxy) || std::isinf(cohyx)) {
                this->z[f_idx].b[xx][i] = false;
                this->z[f_idx].b[xy][i] = false;
                this->z[f_idx].b[yx][i] = false;
                this->z[f_idx].b[yy][i] = false;
                ++des;

                this->z[f_idx].coh[xy][i] = 0;
                this->z[f_idx].coh[yx][i] = 0;
            }
            else {
                cohxy = std::sqrt(std::fabs(cohxy));
                cohyx = std::sqrt(std::fabs(cohyx));

                this->z[f_idx].coh[xy][i] = cohxy;
                this->z[f_idx].coh[yx][i] = cohyx;

                if ((cohxy < base_ct) || (cohyx < base_ct)) {
                    this->z[f_idx].b[xx][i] = false;
                    this->z[f_idx].b[xy][i] = false;
                    this->z[f_idx].b[yx][i] = false;
                    this->z[f_idx].b[yy][i] = false;
                    ++des;
                }

                if (ceil_ct < 1.0) {

                    if ((cohxy > ceil_ct) || (cohyx > ceil_ct)) {
                        this->z[f_idx].b[xx][i] = false;
                        this->z[f_idx].b[xy][i] = false;
                        this->z[f_idx].b[yx][i] = false;
                        this->z[f_idx].b[yy][i] = false;
                        ++des;
                    }

                }

                //qDebug() << "coh " << cohxy << cohyx;
            }

        }
    }
    QString uplo ("mt_site cohereny f: " + doublefreq2string_unit(this->z[f_idx].f) + ", lower: ");
    uplo.append(QString::number(base_ct, 'g', 2));
    uplo.append(", upper: ");
    uplo.append(QString::number(ceil_ct, 'g', 2));
    uplo.append(", in: ");

    qDebug() << uplo << sel << ", out:" << sel - des;

    // we may not be able to send this signal same time from multiple threads
    this->z_mutex.lock();
    emit this->signal_general_msg_nums(uplo, sel, "out:", sel - des);
    this->z_mutex.unlock();
}


// **********************************************************************************************************************


//void mt_site::calc_median_if_rho_phi()
//{
//    if (this->mymttye == mttype::mt) {
//        // allocate memory

//        std::vector<double> freqs(get_freqs_acsp(this->ac_spectra));
//        std::vector<std::future<void>> futures;
//        for ( auto &fs : freqs) {
//            futures.emplace_back(std::async(&mt_site::median_if_rho_phi_vector, this, fs));
//        }

//        for(auto &f : futures) {
//            f.get();
//        }
//    }
//}


// **********************************************************************************************************************


void mt_site::quadrant_check(const size_t what)
{
    if (this->ivalue("activate_quadrant_check") == 0) return;
    if ((this->mymttye >= mttype::mt) && this->mymttye <= mttype::emap_rr) {

        // allocate memory

        std::vector<double> freqs(get_freqs_z(this->z));
        std::vector<std::future<void>> futures;
        for ( auto &fs : freqs) {
            futures.emplace_back(std::async(&mt_site::quadrant_check_vector, this, fs));
        }

        for(auto &f : futures) {
            f.get();
        }

        this->slot_update_booleans(what);
    }

}

void mt_site::quadrant_check_vector(const double &f)
{

    //    this->z_mutex.lock();
    // get the vector positions for this frequency
    size_t f_idx = f_index_c(this->z, f);
    //    this->z_mutex.unlock();

    if (f_idx == SIZE_MAX) return;
    //qDebug() << "quadrant check of z" << doublefreq2string_unit(f) ;

    this->z[f_idx].count_true_selections();
    size_t a, b, c, d, aa, bb, cc, dd;
    a = this->z[f_idx].trues[xx];
    b = this->z[f_idx].trues[xy];
    c = this->z[f_idx].trues[yx];
    d = this->z[f_idx].trues[yy];

    // all components xx, xy ...
    size_t i;
    size_t sz = this->z[f_idx].d.size();

    if (sz >= tns_size) sz = tns_size;             // make only sense for Z but not for tipper

    for (i = 0; i < sz; ++i) {

        auto itzb = this->z[f_idx].d.at(i).cbegin();
        auto itze = this->z[f_idx].d.at(i).cend();
        auto itbb = this->z[f_idx].b.at(i).begin(); // b has always same size!

        if (i == xx) {
            while (itzb != itze) {
                if ((*itzb).real() > 0. || (*itzb).imag() < 0.) {
                    *itbb = false;
                }
                ++itzb;
                ++itbb;
            }
        }
        if (i == xy) {
            while (itzb != itze) {
                if ((*itzb).real() < 0. || (*itzb).imag() < 0.) {
                    *itbb = false;
                }
                ++itzb;
                ++itbb;
            }
        }
        if (i == yx) {
            while (itzb != itze) {
                if ( (*itzb).real() > 0. || (*itzb).imag() > 0.) {
                    *itbb = false;
                }
                ++itzb;
                ++itbb;
            }
        }
        if (i == yy) {
            while (itzb != itze) {
                if ((*itzb).real() < 0. || (*itzb).imag() > 0.) {
                    *itbb = false;
                }
                ++itzb;
                ++itbb;
            }
        }



    }
    this->z[f_idx].count_true_selections();
    aa = this->z[f_idx].trues[xx];
    bb = this->z[f_idx].trues[xy];
    cc = this->z[f_idx].trues[yx];
    dd = this->z[f_idx].trues[yy];

    QString upl1 ("mt_site quadrant check f: " + doublefreq2string_unit(this->z[f_idx].f) + "in: ");
    QString upl2;
    upl2.append(QString::number(a) + ", ");
    upl2.append(QString::number(b) + ", ");
    upl2.append(QString::number(c) + ", ");
    upl2.append(QString::number(d));
    QString upl3("out: ");

    QString upl4;
    upl4.append(QString::number(aa) + ", ");
    upl4.append(QString::number(bb) + ", ");
    upl4.append(QString::number(cc) + ", ");
    upl4.append(QString::number(dd));
    qDebug() << (upl1 +  upl2 + upl3 + upl4);

    this->z_mutex.lock();
    emit this->signal_general_msg_4strs(upl1, upl2, upl3, upl4);
    this->z_mutex.unlock();


}


// **********************************************************************************************************************

void mt_site::inside_range_median_rho_phi(const double lower_rho, const double upper_rho, const double lower_phi, const double upper_phi, const size_t what)
{

    if ((this->mymttye >= mttype::mt) && this->mymttye <= mttype::emap_rr) {

        // allocate memory

        std::vector<double> freqs(get_freqs_acsp(this->ac_spectra));
        std::vector<std::future<void>> futures;
        for ( auto &fs : freqs) {
            futures.emplace_back(std::async(&mt_site::inside_range_median_vector_rho_phi, this, fs, lower_rho, upper_rho, lower_phi, upper_phi));
        }

        for(auto &f : futures) {
            f.get();
        }

        this->slot_update_booleans(what);
    }

}

void mt_site::inside_range_median_vector_rho_phi(const double &f, const double &lower_rho, const double &upper_rho, const double &lower_phi, const double &upper_phi)
{

    //this->z_mutex.lock();
    // get the vector positions for this frequency
    size_t f_idx = f_index_c(this->z, f);
    if (f_idx == SIZE_MAX) return;
    qDebug() << "calculate the median range of rho and phi" << doublefreq2string_unit(f);



    this->z[f_idx].count_true_selections();

    math_vector mv;

    size_t i = 0;
    size_t idx = 0;
    auto itb = this->z[f_idx].b[i];




    this->z[f_idx].count_true_selections();
    auto old(z[f_idx].trues);

    for (i = 0; i < tns_size; ++i) {
        for (idx = 0; idx < this->z[f_idx].d.size(); ++idx) {

            //auto itb = this->z[f_idx].b[i];
            if (this->z[f_idx].trues[i] >= this->sztvalue("min_stacks") && i < tns_size) {

                double rho_med = this->rho.gstat[i][f_idx][g_stat::median_x];
                double phi_med = this->phi.gstat[i][f_idx][g_stat::median_x];
                double rho_med_low = rho_med * (1.0 - (lower_rho - 1.));
                double rho_med_high = rho_med * upper_rho;
                double phi_med_low = std::fabs(phi_med * (1.0 - (lower_phi -1.0)));
                double phi_med_high = std::fabs(phi_med * upper_phi);

                if ( (rho_med_low > z[f_idx].rho(i, idx)) || (z[f_idx].rho(i, idx) > rho_med_high) ) {
                    z[f_idx].b[i][idx] = false;
                }
                if ( (phi_med_low > std::fabs(z[f_idx].phi_deg(i, idx))) || ( std::fabs(z[f_idx].phi_deg(i, idx)) > phi_med_high) ) {
                    z[f_idx].b[i][idx] = false;
                }
            }

        }
    }

    this->z[f_idx].count_true_selections();
    QString upl1 ("mt_site median range rho/phi f: " + doublefreq2string_unit(this->z[f_idx].f) + ", in: ");

    QString upl2;
    upl2.append(QString::number(old[xx]) + ", ");
    upl2.append(QString::number(old[xy]) + ", ");
    upl2.append(QString::number(old[yx]) + ", ");
    upl2.append(QString::number(old[yy]));
    QString upl3("out: ");

    QString upl4;
    upl4.append(QString::number(z[f_idx].trues[xx]) + ", ");
    upl4.append(QString::number(z[f_idx].trues[xy]) + ", ");
    upl4.append(QString::number(z[f_idx].trues[yx]) + ", ");
    upl4.append(QString::number(z[f_idx].trues[yy]));
    qDebug() << (upl1 +  upl2 + " " + upl3 + upl4);

    this->z_mutex.lock();
    emit this->signal_general_msg_4strs(upl1, upl2, upl3, upl4);
    this->z_mutex.unlock();



}



void mt_site::slot_save_data_sql(const QString &filenname)
{

    this->dbname.setFile(filenname);
    if (this->db != nullptr) this->db.reset();
    this->db = std::make_unique<QSqlDatabase>(QSqlDatabase::addDatabase(db_type, connection_name));
    this->db->setDatabaseName(dbname.absoluteFilePath());
    if (this->db->open()) {
        qDebug() << "open success: " <<  dbname.absoluteFilePath();
        QSqlQuery query(*db.get());

        for (auto &data : this->z) {

            QString fstr = QString::number(data.f, 'e', 6);
            QString query_str;
            query_str = "CREATE TABLE IF NOT EXISTS `";
            query_str.append(fstr);
            query_str.append("` (`zxx.re` REAL, `zxx.im` REAL, ");
            query_str.append("`zxy.re` REAL, `zxy.im` REAL, ");
            query_str.append("`zyx.re` REAL, `zyx.im` REAL, ");
            query_str.append("`zyy.re` REAL, `zyy.im` REAL, ");
            query_str.append("`cohxy` REAL, `cohyx` REAL ");
            query_str.append(");");

            if (query.exec(query_str)) {
                QString query_str_sub;
                query_str_sub.append("INSERT INTO `");
                query_str_sub.append(fstr);
                query_str_sub.append("` ");
                query_str_sub.append("(`zxx.re`, `zxx.im`, `zxy.re`, `zxy.im`, `zyx.re`, `zyx.im`, `zyy.re`, `zyy.im`, `cohxy`, `cohyx`) ");
                query_str_sub.append(" VALUES ");
                query_str_sub.append("(:zxx_re, :zxx_im, :zxy_re, :zxy_im, :zyx_re, :zyx_im, :zyy_re, :zyy_im, :cohxy, :cohyx); ");
                query.prepare(query_str_sub);
                //qDebug() << query_str_sub;
                qDebug() << "wrting table " << fstr;
                //query.prepare("INSERT INTO `372` (rho, phi) " "VALUES (:rho, :phi)");
                for (size_t i = 0; i < data.d[xy].size(); ++i) {

                    query.bindValue(":zxx_re", data.d[xx][i].real());
                    query.bindValue(":zxx_im", data.d[xx][i].imag());

                    query.bindValue(":zxy_re", data.d[xy][i].real());
                    query.bindValue(":zxy_im", data.d[xy][i].imag());

                    query.bindValue(":zyx_re", data.d[yx][i].real());
                    query.bindValue(":zyx_im", data.d[yx][i].imag());

                    query.bindValue(":zyy_re", data.d[yy][i].real());
                    query.bindValue(":zyy_im", data.d[yy][i].imag());

                    query.bindValue(":cohxy", data.coh[xy][i]);
                    query.bindValue(":cohyx", data.coh[yx][i]);

                    query.exec();
                }
            } // created table
        } // each f of z





        db->close();
    }
}





void mt_site::median_complex()
{
    if ((this->mymttye >= mttype::mt) && this->mymttye <= mttype::emap_rr) {

        // allocate memory

        std::vector<double> freqs(get_freqs_z(this->z));
        std::vector<std::future<void>> futures;
        for ( auto &fs : freqs) {
            futures.emplace_back(std::async(&mt_site::median_if_complex_vector, this, fs));
        }

        for(auto &f : futures) {
            f.get();
        }
    }

}

//void mt_site::median_rho_phi()
//{
//    if (this->mymttye == mttype::mt) {
//        // allocate memory

//        std::vector<double> freqs(get_freqs_acsp(this->ac_spectra));
//        std::vector<std::future<void>> futures;
//        for ( auto &fs : freqs) {
//            futures.emplace_back(std::async(&mt_site::median_if_rho_phi_vector, this, fs));
//        }

//        for(auto &f : futures) {
//            f.get();
//        }
//    }

//}


void mt_site::median_if_complex_vector(const double &f)
{

    //this->z_mutex.lock();
    // get the vector positions for this frequency
    size_t f_idx = f_index_c(this->z, f);
    if (f_idx == SIZE_MAX) return;
    qDebug() << "calculate the median of z" << f;

    //this->z_mutex.unlock();

    // all components xx, xy ...
    size_t elem,  sz = this->z[f_idx].d.size();
    size_t fin;

    for (elem = 0; elem < sz; ++elem) { // for all 4 .. 6 components
        fin = 0;
        this->z[f_idx].median[elem] = std::complex<double>(DBL_MAX, DBL_MAX);
        std::vector<double> reals(this->z[f_idx].d[elem].size());
        std::vector<double> imags(this->z[f_idx].d[elem].size());

        auto rs = reals.begin();
        auto is = imags.begin();
        auto zs = this->z[f_idx].d.at(elem).cbegin();
        auto zse = this->z[f_idx].d.at(elem).cend();
        auto bs = this->z[f_idx].b.at(elem).cbegin();

        while (zs != zse) {

            if (*bs) {
                *rs = real(*zs);
                *is = imag(*zs);
                ++fin;
                ++rs; ++is;
            }
            ++bs; ++zs;
        }
        // rs points behind, ok
        size_t dist = size_t (std::distance(reals.begin(), rs));
        if (dist > 4 && (dist == fin) ) {
            std::sort(reals.begin(), rs);
            std::sort(imags.begin(), is);

            if (fin % 2 == 0)  {     // array is even
                this->z[f_idx].median[elem] = std::complex<double>( ((reals[fin/2] + reals[fin/2 -1])/2.), ((imags[fin/2] + imags[fin/2 -1])/2.) );
            }
            else {
                this->z[f_idx].median[elem] = std::complex<double> (reals[fin/2], imags[fin/2]);  // integer divison should not round up
            }

        }
    }

    qDebug() << "calculate the median of z done" << f;

}

//void mt_site::median_if_rho_phi_vector(const double &f)
//{
//    // get the vector positions for this frequency
//    size_t f_idx = f_index_c(this->z, f);
//    if (f_idx == SIZE_MAX) return;
//    qDebug() << "calculate the median of rho & phi" << f;


//    // all components xx, xy ...
//    size_t i,  sz = this->rho[f_idx].d.size();
//    size_t fin;

//    for (i = 0; i < sz; ++i) {
//        fin = 0;
//        double mean = 0;
//        this->rho[f_idx].median[i] = DBL_MAX;
//        this->rho[f_idx].mean[i] = DBL_MAX;

//        std::vector<double> vals(this->rho[f_idx].d[i].size());

//        auto itval = vals.begin();
//        auto itb = this->rho[f_idx].d.at(i).cbegin();
//        auto ite = this->rho[f_idx].d.at(i).cend();
//        auto bs = this->z[f_idx].b.at(i).cbegin();

//        while (itb != ite) {
//            if (*bs) {
//                *itval = *itb;
//                mean += *itb;
//                ++fin;
//                ++itval;
//            }
//            ++bs; ++itb;
//        }
//        // itval points behind, ok
//        int dist = std::distance(vals.begin(), itval);
//        if (dist > 4 && ((size_t) dist == fin)) {
//            std::sort(vals.begin(), itval);
//            if (fin % 2 == 0)  {     // array is even
//                this->rho[f_idx].median[i] = (vals[fin/2] + vals[fin/2 -1])/2.;;
//            }
//            else {
//                this->rho[f_idx].median[i] =vals[fin/2];  // integer divison should not round up
//            }
//            this->rho[f_idx].mean[i] = mean /(double)fin;

//        }
//    }

//    // same for phi

//    sz = this->phi[f_idx].d.size();

//    for (i = 0; i < sz; ++i) {
//        fin = 0;
//        double mean = 0;
//        this->phi[f_idx].median[i] = DBL_MAX;
//        this->phi[f_idx].mean[i] = DBL_MAX;

//        std::vector<double> vals(this->phi[f_idx].d[i].size());

//        auto itval = vals.begin();
//        auto itb = this->phi[f_idx].d.at(i).cbegin();
//        auto ite = this->phi[f_idx].d.at(i).cend();
//        auto bs = this->z[f_idx].b.at(i).cbegin();

//        while (itb != ite) {
//            if (*bs) {
//                *itval = *itb;
//                mean += *itb;
//                ++fin;
//                ++itval;
//            }
//            ++bs; ++itb;
//        }
//        // itval points behind, ok
//        int dist = std::distance(vals.begin(), itval);
//        if (dist > 4 && ((size_t) dist == fin)) {
//            std::sort(vals.begin(), itval);
//            if (fin % 2 == 0)  {     // array is even
//                this->phi[f_idx].median[i] = (vals[fin/2] + vals[fin/2 -1])/2.;;
//            }
//            else {
//                this->phi[f_idx].median[i] =vals[fin/2];  // integer divison should not round up
//            }
//            this->phi[f_idx].mean[i] = mean /(double)fin;

//        }
//    }




//    qDebug() << "calculate the median of rho & phi done" << f;

//}




void mt_site::abs_stack_tensor_spectra(const bool dump, const QString insertme)
{

    this->ac_spectra_stacked.clear();

    // all freqeuncies
    for (auto &spc : this->ac_spectra) {

        // all frequencies of each
        statmap var_data;
        size_t k = 0;
        // all spectra
        for (auto &lspc : spc.ac) {
            if (lspc.size()) {
                auto itb = spc.ac_sel[k];
                std::unique_ptr<two_pass_linreg> tplrg_sel = std::make_unique<two_pass_linreg>();
                std::vector<double> x(lspc.size());
                std::vector<double> y(lspc.size());
                size_t i = 0;
                for (auto &d : lspc) {
                    x[i] = real(d);
                    y[i++] = imag(d);
                }
                var_data = tplrg_sel->linreg_if(x.cbegin(), x.cend(), y.cbegin(), y.cend(), itb.cbegin(), itb.cend(), 0.0);
            }
            ++k;
        }
    }


    QString appendme = insertme + "_ampl.dat";
    if (dump) {


        for (auto spc : this->ac_spectra_stacked) {


            QFile qf("/tmp/" + pmt::ac_spectra_names.at(spc.first) + appendme);
            QTextStream qts;
            qts.setRealNumberNotation(QTextStream::ScientificNotation);
            qts.setDevice(&qf);
            if (qf.open(QIODevice::WriteOnly)) {
                for (auto &lca : spc.second) {
                    qts << lca.first << "  " << lca.second << Qt::endl;
                }
            }

            qf.close();

        }

    }

}


void mt_site::slot_update_booleans(const size_t what, const bool emits)
{

    //    for ( auto &spc : get_freqs_acsp(this->ac_spectra)) {
    //        // qDebug() << "processing z" << spc.first << " sizes" << spc.second.size();
    //        futures.emplace_back(std::async(&mt_site::calc_z_vector, this, spc));
    //    }


    std::vector<std::future<void>> futures;
    size_t i;
    //for ( auto &data : z) {
    //    for (i = 0; i < this->z.size(); ++i) {
    //        //        data.merge_bool();
    //        futures.emplace_back(std::async(&mt_data_t<std::complex<double>>::merge_bool, z[i], what));
    //    }

    //    for(auto &f : futures) {
    //        f.get();
    //    }

    for (i = 0; i < this->z.size(); ++i) {
        z[i].merge_bool(what);
    }
    if (emits) emit this->signal_mt_site_updated();
    qDebug() << "mt_site::slot_update_booleans updated";
}

void mt_site::slot_reset_booleans()
{
    for ( auto &data : z) {
        data.reset_bool();
    }

    emit this->signal_mt_site_updated();
}



