#include <QCoreApplication>
#include <QDir>
#include "edi_file.h"
#include "prc_com.h"
#include "mc_data.h"
#include "mt_site.h"
#include "math_vector.h"
#include "acsp_prz.h"
#include "procmt_alldefines.h"
#include "qstring_utilities.h"
#include <iostream>
#include <climits>
#include <cfloat>
#include <memory>
#include "acsp_prz.h"
#include "msg_logger.h"
#include <list>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    std::shared_ptr<prc_com> cmdline = std::make_shared<prc_com>(false);
    std::shared_ptr<edi_file> edifile = std::make_shared<edi_file>();
    int inner_loop_counter = 0;

    std::shared_ptr<msg_logger> msg = std::make_shared<msg_logger>();




    // collect all ExEx  ExHy... RHzRHz
    std::shared_ptr<all_spectra_collector<std::complex<double>>>  all_acspc_collector = std::make_shared<all_spectra_collector<std::complex<double>>>(true);

    //QStringList allargs;
    QStringList allfiles;
    QStringList allatsfiles;
    QStringList allmeasdocs;                //!< all local station, single site

    QStringList allmeasdocs_emap;           //!< all emap sites, E only taken
    QStringList allmeasdocs_rr;             //!< all RR sites


    // -mtspectra std -task cross-coherency -wl 1024 -nbuffers 3 -fcut_upper 0.1 -fcut_lower 0.03 -parzen_radius 0.1 -overlapping_factor 0.3  -write_bin_spectra true -survey_basedir /survey/procmt_new/ac_spectra_test -site_dir BW6_short -site_dir_rr RR_BW16_short
    // -mtspectra std -task cross-coherency -wl 1024 -nbuffers 3 -fcut_upper 0.1 -fcut_lower 0.03 -parzen_radius 0.1 -overlapping_factor 0.3  -write_bin_spectra true -survey_basedir /survey/procmt_new/ac_spectra_test -site_dir BW6_short -site_dir_rr RR_BW16_short

    // debug
    // -mtspectra std -task cross-coherency -wl 1024 -nbuffers 3 -fcut_upper 0.05 -fcut_lower 0.002 -parzen_radius 0.2 -survey_basedir /home/bfr/survey/procmt_new/ac_spectra_test -site_dir Site_512

    // theo noise
    // -mtspectra std -task cross-coherency -wl 1024 -nbuffers 3 -fcut_upper 0.05 -fcut_lower 0.002 -parzen_radius 0.2 -survey_basedir /home/bfr/survey/procmt_new/theo_noise_mfs06e -site_dir Site_2


    // yasc
    // -mtspectra std -task cross-coherency -wl 1024 -nbuffers 3 -fcut_upper 0.1 -fcut_lower 0.03 -parzen_radius 0.1 -write_bin_spectra true -survey_basedir /home/bfr/survey/procmt_new/ac_spectra_test -site_dir Site_1

    // inv
    // -ascii ts -task to_nT /home/bfr/survey/procmt_new/full_inv/ts/full_inversion_test/meas_2009-08-20_13-23-37/084_V01_C02_R001_THx_BL_8S.ats
    //

    // stacked spectra
    //  -mtspectra std -task cross-coherency -wl 1024 -nbuffers 3 -fcut_upper 0.1 -fcut_lower 0.03 -survey_basedir /home/bfr/survey/procmt_new/stacked_spectra_and_ccf
    //  -mtspectra std -task cross-coherency -wl 1024 -nbuffers 3 -fcut_upper 0.1 -fcut_lower 0.03 -parzen_radius 0.0 /home/bfr/survey/procmt_new/stacked_spectra_and_ccf/ts/Site_1/meas_2009-08-21_07-01-01/084_2009-08-21_07-01-01_2009-08-21_07-05-59_R001_512H.xml

    // 2018-10-22 MT
    // -mtspectra std -task cross-coherency -wl 1024 -nbuffers 3 -fcut_upper 0.1 -fcut_lower 0.03  -survey_basedir /home/bfr/survey/procmt_new/emap -site_dir Site_1
    // -mtspectra std -task cross-coherency -wl 1024 -nbuffers 3 -fcut_upper 0.1 -fcut_lower 0.03  -survey_basedir /home/bfr/survey/procmt_new/emap -site_dir Site_1

    // 2018-10-14 MT
    // -mtspectra std -task cross-coherency -wl 1024 -nbuffers 3 -fcut_upper 0.1 -fcut_lower 0.03 -parzen_radius 0.1 -survey_basedir /home/bfr/survey/procmt_new/emap -site_dir Site_1 -site_dir_emap Site_2

    // 2018-10-14 nT conversion
    // -ascii ts -task to_nT /home/bfr/survey/procmt_new/ac_spectra_test/ts/Rh_New_Coil/meas_2018-04-20_11-57-56/216_2018-04-20_11-57-56_2018-04-23_07-57-20_R000_4S.xml



    // ./atstool -mtspectra std -task cross-coherency -wl 512 -nbuffers 3 -fcut_upper 0.22 -fcut_lower 0.1 -parzen_radius 0.0 /home/bfr/survey/procmt_new/ac_spectra_test/ts/NorthernMining/meas_2009-08-21_07-01-00/084_2009-08-21_07-01-00_2009-08-21_07-06-00_R001_2048H.xml
    // ./atstool -mtspectra std -ats_calibration auto -task acspectra -wl 512 -nbuffers 3 -fcut_upper 0.22 -fcut_lower 0.1 -parzen_radius 0.14 /home/bfr/survey/procmt_new/ac_spectra_test/ts/NorthernMining/meas_2009-08-21_07-01-00/084_2009-08-21_07-01-00_2009-08-21_07-06-00_R001_2048H.xml

    //alla << "-pos_x2" << "-50" << "-site_name" << "Rhoden" << "084_V01_C00_R001_TEx_BL_128H.ats" << "084_V01_C00_R001_TEy_BL_128H.ats";

    // -task filter -filter_name highpass -ats ts -upper_freq 1 -lower_freq 0.0009765625 01/*ats

    // -task convert -ats ts -ascii ts /home/bfr/tmp/ftx/meas_2009-08-21_07-01-03/*xml
    //  -ascii spc  -wl 4096 *Hy*ats    (simple spectra)

    // test for mt

    // -mtspectra std -task acspectra -wl 256 -nbuffers 3 -fcut_upper 0.2 -fcut_lower -parzen_radius 0.0 /home/bfr/cpp/sw/mtx_devel/procmt/mt_test/pmt/ts/LL1Rhoden/meas_2017-04-05_10-46-55/203_2017-04-05_10-46-55_2017-04-12_12-52-47_R000_128S.xml /home/bfr/cpp/sw/mtx_devel/procmt/mt_test/pmt/ts/RR2Wittstock/meas_2017-04-04_12-45-49/153_2017-04-04_12-45-49_2017-04-12_18-03-41_R000_128S.xml

    // -mtspectra std -task acspectra /home/bfr/tmp/test_ts/meas_2009-08-21_07-01-01/084_2009-08-21_07-01-21_07-06-00_R001_512H.xml
    // -mtspectra std -task acspectra /home/bfr/cpp/sw/mtx_devel/procmt/mt_test/meas_2009-08-21_07-01-01/084_2009-08-21_07-01-01_2009-08-21_07-06-00_R001_512H.xml
    // -mtspectra std -task acspectra /home/bfr/cpp/sw/mtx_devel/procmt/mt_test/pmt/ts/local_same/meas_2009-08-20_13-23-37/084_2009-08-20_13-23-37_2009-08-21_06-58-33_R001_8S.xml


    // remote mt
    // -mtspectra std -task acspectra /home/bfr/cpp/sw/mtx_devel/procmt/mt_test/pmt/ts/local_same/meas_2009-08-20_13-23-37/084_2009-08-20_13-23-37_2009-08-21_06-58-33_R001_8S.xml /home/bfr/cpp/sw/mtx_devel/procmt/mt_test/pmt/ts/rr_same/meas_2009-08-20_13-23-37/085_2009-08-20_13-23-37_2009-08-21_06-58-33_R001_8S.xml
    // -mtspectra std -task acspectra /mt_test/pmt/ts/local_same/meas_2009-08-20_13-23-37/084_2009-08-20_13-23-37_2009-08-21_06-58-33_R001_8S.xml /mt_test/pmt/ts/rr_same/meas_2009-08-20_13-23-37/085_2009-08-20_13-23-37_2009-08-21_06-58-33_R001_8S.xml
    //
    // not same time
    // -mtspectra std -task acspectra /home/bfr/cpp/sw/mtx_devel/procmt/mt_test/pmt/ts/LL1Rhoden/meas_2017-04-05_10-46-55/203_2017-04-05_10-46-55_2017-04-12_12-52-47_R000_128S.xml /home/bfr/cpp/sw/mtx_devel/procmt/mt_test/pmt/ts/RR2Wittstock/meas_2017-04-04_12-45-49/153_2017-04-04_12-45-49_2017-04-12_18-03-41_R000_128S.xml
    //

    // test for stacked spectra


    //cmdline->insert("pos_x2", DBL_MAX);          // want double
    //cmdline->insert("site_name", "empty");

    // so these will be cleared later HOWEVER I force type conversion WHILE SETTING and not as usual WHILE GETTING
    // this may be not necessary
    cmdline->insert_szt("wl", 1024);                 // want size_t for window length of fft
    cmdline->insert_szt("nbuffers", 300);              // chache size of the ats threadbuffer capacity; min should be three - max depends on your system memory; if greater than possbile stack: useless

    cmdline->insert("ascii", "ts");                  // ? wants ascii spectra
    //cmdline->insert("ats", "ts");                  // ? wants ats timeseries
    cmdline->insert("task", "convert");              // ?
    cmdline->insert("filter_name", "mtx32");         // filter with mtx32, mtx8 ... mtx4: mtx4 give best overlapping; in theory: record with 16x sample freqs and filter in between with 4
    cmdline->insert_double("upper_freq", double(0.0));
    cmdline->insert_double("lower_freq", double(0.0));

    cmdline->insert_double("fcut_upper", double(0.2));      // 20% loss of upper FFT - that is ok
    cmdline->insert_double("fcut_lower", double(0.1));      // 10% loss of lower part off FFT
    cmdline->insert_double("parzen_radius", double(0.1));   // parzen radius for mtspectra

    cmdline->insert_double("auto_bandwidth", double(1.0));
    cmdline->insert_double("auto_parzen_radius", double(1.0));
    cmdline->insert_szt("min_lines", 0);

    cmdline->insert_double("overlapping_factor", 0);
    cmdline->insert_int("skip_marked_parts", 0);

    cmdline->insert("mtspectra", "std");
    cmdline->insert("task", "acspectra");
    cmdline->insert("subtask", "nothing");

    cmdline->insert("calibration", "auto");
    cmdline->insert("ats_scale", "auto");        // filter does not want to scale

    cmdline->insert("survey_basedir", "");
    cmdline->insert("site_dir", "");
    cmdline->insert("site_dir_emap", "");
    cmdline->insert("site_dir_rr", "");

    cmdline->insert("num_threads", 4);
    cmdline->insert_int("force_frequency_list", 1);
    cmdline->insert("write_bin_spectra", "false");




    // atstool -task filter -ats ts -filter_name mtx32 *128H.ats
    // if true (like here) pre-configured cmdline is delete (only used as template for strings, double and size_t for correct input)
    // all waht has been NOT DEFINED above will be rejected because no template found
    cmdline->scan_cmdline(a.arguments(), allfiles, allatsfiles, allmeasdocs,true, true, true);

    QList<QFileInfo> qfis;
    QList<QFileInfo> qfis_emap;
    QList<QFileInfo> qfis_rr;

    int iqfis = 0;
    int iqfis_emap = 0;
    int iqfis_rr = 0;
    mttype spcx = mttype::nomt;


    //allmeasdocs.append("/home/bfr/survey/nm/ts/Site_1/meas_2009-08-21_07-01-00/084_2009-08-21_07-01-00_2009-08-21_07-06-00_R001_2048H.xml");
    //allmeasdocs.append("/home/bfr/survey/nm/ts/Site_1/meas_2009-08-21_07-01-01/084_2009-08-21_07-01-01_2009-08-21_07-06-00_R001_512H.xml");





    // allmeasdocs.append("/home/bfr/survey/procmt_new/ac_spectra_test/ts/NorthernMining/meas_2009-08-21_07-01-00/084_2009-08-21_07-01-00_2009-08-21_07-06-00_R001_2048H.xml");
    // allmeasdocs.append("/home/bfr/survey/procmt_new/ac_spectra_test/ts/NorthernMining/meas_2009-08-21_07-01-01/084_2009-08-21_07-01-01_2009-08-21_07-06-00_R001_512H.xml");

    qDebug() << "starts";
    if ( (cmdline->svalue("site_dir") != "empty") && (cmdline->svalue("survey_basedir") != "empty")) {
        allmeasdocs.clear();
        allmeasdocs = xmls_in_site(cmdline->svalue("survey_basedir"), cmdline->svalue("site_dir"));
        cmdline->set_key_value("mtspectra", "std");
        spcx = mttype::mt;


    }

    if ( (cmdline->svalue("site_dir_emap") != "empty") && (cmdline->svalue("survey_basedir") != "empty")) {
        allmeasdocs_emap = xmls_in_site(cmdline->svalue("survey_basedir"), cmdline->svalue("site_dir_emap"));
        cmdline->set_key_value("mtspectra", "emap");
        spcx = mttype::emap;
    }

    if ( (cmdline->svalue("site_dir_rr") != "empty") && (cmdline->svalue("survey_basedir") != "empty")) {
        allmeasdocs_rr = xmls_in_site(cmdline->svalue("survey_basedir"), cmdline->svalue("site_dir_rr"));
        if (cmdline->svalue("mtspectra") == "emap") {
            cmdline->set_key_value("mtspectra", "emap_rr");
            spcx = mttype::emap_rr;

        }
        else {
            cmdline->set_key_value("mtspectra", "rr");
            spcx = mttype::rr;
        }
    }

    qDebug() << "starts" << allmeasdocs.size();
    if (allmeasdocs.size()) {

        for (auto &f : allmeasdocs) {
            qfis.append(QFileInfo(f));
        }
        for (auto &f : allmeasdocs_emap) {
            qfis_emap.append(QFileInfo(f));
        }
        for (auto &f : allmeasdocs_rr) {
            qfis_rr.append(QFileInfo(f));
        }

        // check the sample frequencies!
        // in mcd check start / stop

    }

    else {

        if (!allatsfiles.size()) {
            std::cerr << "no atsfiles loaded" << std::endl;
            exit (0);
        }

        for (auto &f : allatsfiles) {
            qfis.append(QFileInfo(f));
        }
    }



    if (!cmdline->contains("num_threads")) {

        size_t num_threads = std::thread::hardware_concurrency();
        if (num_threads < 4) num_threads = 4;
        cmdline->insert_szt("num_threads", num_threads);
    }
    if (!cmdline->contains("force_frequency_list"))cmdline->insert_int("force_frequency_list", 1);


    qDebug() << "cmdline contains --------------------------------";
    cmdline->show_my_map();
    qDebug() << " -------------------------------------------------";
    qDebug() << "read " << qfis.size() << " xml docs";
    qDebug() << " -------------------------------------------------";


    /////////////////////////////////////////////// ////////////////////////////////////////////////////////////  ///////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////// ////////////////////////////////////////////////////////////  ///////////////////////////////////////////////////////////////////////////////

    if (cmdline->contains("mtspectra")) {
        qDebug() << "mt spectra";
        std::unique_ptr<mt_site> mtsite = std::make_unique<mt_site>(cmdline->sztvalue("num_threads"), spcx);




        /////////////////////////////////////////////// MT DEFAULT & Spectra  ///////////////////////////////////////////////////////////////////////////////
        std::cout << "/////////////////////////////////////////////// MT DEFAULT & Spectra  /////////////////////////////" << std::endl;

        if ((cmdline->value("task").toString().contains("cross-coherency"))) {

            if (!qfis.size()) return 0;

            if ((spcx == mttype::emap) || (spcx == mttype::emap_rr)) {
                qDebug() << "try EMAP";
                if (qfis.size() != qfis_emap.size()) {
                    std::cerr << "eamp and main staion have different recordings, exit" << std::endl;
                    return 0;
                }
            }
            if ((spcx == mttype::rr) || (spcx == mttype::emap_rr)) {
                qDebug() << "try EMAP";
                if (qfis.size() != qfis_rr.size()) {
                    std::cerr << "rr and main staion have different recordings, exit" << std::endl;
                    return 0;
                }
            }


            for (auto &qfixml : qfis) {
                std::cout << "//////////////////////////////////////// CC CALC begin /////////////////////////////////////////////////////" << std::endl;
                QList<QFileInfo> qfiXML;
                if ((spcx == mttype::emap) || (spcx == mttype::emap_rr)) qfiXML.append(qfis_emap.at(iqfis_emap++));
                qfiXML.append(qfixml);
                if ((spcx == mttype::rr) || (spcx == mttype::emap_rr)) qfiXML.append(qfis_rr.at(iqfis_rr++));

                std::shared_ptr<mc_data> mxcd = std::make_shared<mc_data>();

//                cmdline->insert("force_frequency_list",1);
//                cmdline->insert("target_frequencies","auto_gen_tf_40");

                // do this first before we run anything
                // hence that he cmdline may not contain all values
                if (!cmdline->contains("skip_marked_parts")) cmdline->insert_int("skip_marked_parts", 0);
                if (!cmdline->contains("overlapping_factor")) cmdline->insert_double("overlapping_factor", 0);

                mxcd->insert_prc_com(cmdline);
                mxcd->set_messenger(msg);
                mxcd->all_acspc_collector = all_acspc_collector;


                std::shared_ptr<math_vector> mathvector = std::make_shared<math_vector>();

                QObject::connect(mxcd.get(), &mc_data::signal_got_freq_targets, mathvector.get(), &math_vector::slot_parzen_vector, Qt::DirectConnection);

                // new
                QObject::connect(mxcd.get(), &mc_data::signal_got_freq_targets_lines, mathvector.get(), &math_vector::take_five, Qt::DirectConnection);

                QObject::connect(mathvector.get(), &math_vector::parzenvector_created, mxcd.get(), &mc_data::slot_append_parzenvector, Qt::DirectConnection);

                QObject::connect(mathvector.get(), &math_vector::frequencies_selected, mxcd.get(), &::mc_data::slot_set_target_frequencies, Qt::DirectConnection);
                QObject::connect(mathvector.get(), &math_vector::frequencies_fft_indices_selected, mxcd.get(), &::mc_data::slot_frequencies_fft_indices_selected, Qt::DirectConnection);

                QObject::connect(mathvector.get(), &math_vector::parzen_radius_changed, msg.get(), &msg_logger::parzen_radius_changed, Qt::DirectConnection);

                size_t nfiles = mxcd->open_files(qfiXML, true, spcx, threadctrl::threadctrl_continuous_join);
                if(!nfiles) {
                    break;
                }

                if (!cmdline->contains("parzen_radius")) cmdline->insert_double("parzen_radius", 0);
                if (!cmdline->contains("min_lines")) cmdline->insert_szt("min_lines", 0);

                if (mxcd->prepare_mt_spectra(cmdline->sztvalue("nbuffers"), cmdline->sztvalue("wl"), cmdline->dvalue("fcut_upper"), cmdline->dvalue("fcut_lower"),
                                             0, 0, cmdline->dvalue("overlapping_factor"), cmdline->dvalue("parzen_radius"), cmdline->sztvalue("min_lines"),
                                             cmdline->ivalue("force_frequency_list"), atsfileout::atsfileout_calibrated_spectra, true)) {



                    mxcd->start_in_ats_channel_threads();
                    mxcd->start_collect_all_spc_threads();
                    mxcd->join_in_ats_channel_threads();
                    mxcd->join_collect_all_spc_threads();

                    //all_channel_collector
                    qDebug() << "spectra collected";

                    // in mcdata ziehen ?
                    if (cmdline->sztvalue("min_lines") == 0) mxcd->make_auto_cross_spectra_parzen();
//size_t i;
//                    for (auto &ssp : mxcd->out_atsfile_collectors) {
//                        // i = ssp
//                        all_acspc_collector->data[ssp->spc.chan_type].append_clear_data(ssp->spc);
//                    }
                    ++inner_loop_counter;



                    //                    mtsite->append_spectra_collector(spectracollector, mxcd->in_ats_prc_coms);
                    //                    qDebug() << "site data collected";
                    //                    spectracollector.reset();
                    //                    mathvector.reset();
                    //                    mxcd.reset();
                    //                    if (cmdline->value("subtask").toString().contains("split_spectra")) {
                    //                        QString inserts = qfiXML.at(0).baseName();
                    //                        int idx = inserts.lastIndexOf("_");
                    //                        inserts.remove(0,idx);
                    //                        mtsite->abs_stack_tensor_spectra(true, inserts);
                    //                        mtsite->clear();
                    //                    }
                }
                else {
                    mxcd->close_infiles();
                    mathvector.reset();
                    mxcd.reset();

                }
                std::cout << "//////////////////////////////////////// CC COLLECTED /////////////////////////////////////////////////////" << std::endl << std::endl;


            }
        }

        /// that is behing the measdoc loop and mtsite should have collected all data
        qDebug() << " ---------------------------------------------------->>>>>>>>>>>< measdoc set finished ";


        cmdline->insert("write_bin_spectra", "true");

        mtsite->insert_prc_com(cmdline);
        int ac1_ssp2_acssp3 = 1;
        if (cmdline->sztvalue("min_lines") >= 1)  ac1_ssp2_acssp3 = 2;
        mtsite->open(all_acspc_collector, true, true, ac1_ssp2_acssp3);

        QFileInfo spc_file; // .binspectra
        if (mtsite->svalue("write_bin_spectra") == "true") {
            if (mtsite->write_bin_spectra(spc_file)) {
                std::cout << "binspectra written to" << spc_file.fileName().toStdString() << std::endl;

                //                // test
                //                std::shared_ptr<mt_site> mtsite_load = std::make_shared<mt_site>(cmdline->sztvalue("num_threads"));
                //                mtsite_load->insert_prc_com(cmdline);
                //                QFileInfo qfi(mtsite_load->dump_path + "/" + mtsite_load->svalue("site_dir") + ".binspectra");
                //                mtsite_load->read_bin_spectra(qfi);
                //                qDebug() << "test loaded";

            }
        }


        // mtsite->slot_calc_ac_spc();
        // mtsite->slot_ssp_stack_all();

        // mtsite->slot_dump_ampl_spc();

        qDebug() << "runs: " << inner_loop_counter << ", frequencies processed:";
        for (auto &d : mtsite->get_freqs()) qDebug() << d;
        mtsite->calc_z();
        //mtsite->quadrant_check(tns_sel::xy_yx);
        //mtsite->calc_rho_phi();

        //mtsite->huber();

        mtsite->stack_all();
        QFileInfo qfi(mtsite->svalue("survey_basedir") + "/edi/" + mtsite->svalue("site_dir") + ".edi");
        edifile->set_QFileInfo(qfi);
        edifile->set_edi_data_z(mtsite->zet, mtsite->in_ats_prc_coms);
        edifile->write();
        msg->show_and_empty();
        mtsite.reset();



    }
    /////////////////////////////////////////////// ////////////////////////////////////////////////////////////  ///////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////// ////////////////////////////////////////////////////////////  ///////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////// ////////////////////////////////////////////////////////////  ///////////////////////////////////////////////////////////////////////////////

    else if (cmdline->contains("ats")) {
        qDebug() << "ats out:" <<  cmdline->value("ats");
        std::shared_ptr<mc_data> mxcd = std::make_shared<mc_data>();

        if ((cmdline->value("task").toString().contains("filter"))) {

            qDebug() << "main::prepare filter" << cmdline->value("filter_name");
            mxcd->insert_prc_com(cmdline);
            mxcd->open_files(qfis, true, mttype::nomt, threadctrl::threadctrl_continuous_individual);
            if (!mxcd->prepare_firfilter()) {
                qDebug() << "main:: fail";
                exit(1);
            }

            mxcd->prepare_ats_outfiles(10, 1024, 0, atsfileout::atsfileout_unscaled_fir_filtered ,".ats");

        }

        mxcd->start_in_ats_channel_threads();
        mxcd->start_out_ats_channel_threads();

        mxcd->join_in_ats_channel_threads();
        mxcd->join_out_ats_channel_threads();
        mxcd.reset();

    }


    else if (cmdline->contains("ascii")) {


        if ((cmdline->value("task").toString().contains("to_nT"))) {

            for (auto &qfixml : qfis) {
                QList<QFileInfo> qfiXML;
                qfiXML.append(qfixml);

                //                bool theo_only = false;
                //                if (cmdline->value("calibration").toString() == "auto") theo_only = true;

                qDebug() << "converting to mv/km and nT ASCII" <<  cmdline->value("task").toString();


                std::shared_ptr<mc_data> mxcd = std::make_shared<mc_data>();


                // do this first before we run anything
                mxcd->insert_prc_com(cmdline);
                mxcd->set_messenger(msg);

                std::shared_ptr<math_vector> mathvector = std::make_shared<math_vector>();

                // do we need these here
                QObject::connect(mxcd.get(), &mc_data::signal_got_freq_targets, mathvector.get(), &math_vector::slot_parzen_vector, Qt::DirectConnection);
                QObject::connect(mathvector.get(), &math_vector::parzenvector_created, mxcd.get(), &mc_data::slot_append_parzenvector, Qt::DirectConnection);
                QObject::connect(mathvector.get(), &math_vector::parzen_radius_changed, msg.get(), &msg_logger::parzen_radius_changed, Qt::DirectConnection);

                if(!mxcd->open_files(qfiXML, true, spcx, threadctrl::threadctrl_continuous_individual)) {
                    break;
                }

                // will create atsfile_collector(s)
                mxcd->start_single_spectra_threads(true, true);
                mxcd->join_single_spectra_threads();



            }
        }





        else {

            qDebug() << "converting to ascii" <<  cmdline->value("ascii").toString();
            std::shared_ptr<mc_data> mxcd = std::make_shared<mc_data>();

            // open input files for usage and access header infos
            mxcd->open_files(qfis, true, mttype::nomt, threadctrl::threadctrl_continuous_individual);

            if (cmdline->value("ascii").toString().contains("spc")) {
                if(cmdline->contains("wl")) {

                    //                // if calib || calib
                    // stacked returns one only
                    mxcd->prepare_ascii_outfiles(1, cmdline->value("wl").toULongLong(), 0, atsfileout::atsfileout_stacked_amplitude_spectra ,".spc_stacked_dat");

                }
            }
            else  if (cmdline->value("ascii").toString().contains("ts")) {
                // mxcd->prepare_ascii_outfiles(cmdline->key("wl").toULongLong(),threadbuffer_status::tsdata ,".tsdata");
                mxcd->prepare_ascii_outfiles(100, 1024, 0, atsfileout::atsfileout_scaled_timeseries ,".tsdata");

            }

            else exit(0);


            //mxcd->in_atsfiles[0]->show_my_map();

            mxcd->start_in_ats_channel_threads();
            mxcd->start_out_ats_ascii_channel_threads();

            mxcd->join_in_ats_channel_threads();
            mxcd->join_out_ats_ascii_channel_threads();

            mxcd.reset();

        }
    }

    qDebug() << "ending";
    std::cerr << "schluss ************************************************************************************************************" << std::endl;



    exit(0);
}

