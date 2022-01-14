#include "measdoc_from_mc_data.h"

measdoc_from_mc_data::measdoc_from_mc_data(std::shared_ptr<mc_data> mxcd, QObject *parent)  : QObject(parent)
{
    if (!mxcd->in_atsfiles.size()) return;
    if (this->measdoc != nullptr) this->measdoc.reset();
    if (this->mxcd != nullptr) this->mxcd.reset();
    this->mxcd = mxcd;
    this->adujob = std::make_unique<adulib>(1, this);

    this->can_continue = this->prepare_adu_from_skeleton();

}

measdoc_from_mc_data::measdoc_from_mc_data(const QList<QFileInfo> qfis, QObject *parent) : QObject(parent)
{
    if (!qfis.size()) return;
    if (this->measdoc != nullptr) this->measdoc.reset();
    if (this->mxcd != nullptr) this->mxcd.reset();
    this->mxcd = std::make_shared<mc_data>(this);
    this->adujob = std::make_unique<adulib>(1, this);

    if (!this->mxcd->open_files(qfis)) return;

    this->can_continue = this->prepare_adu_from_skeleton();

}

std::shared_ptr<measdocxml> measdoc_from_mc_data::make_measdoc_xml(bool &success)
{
    success = this->can_continue;
    if (!this->can_continue) return std::make_shared<measdocxml>(nullptr,nullptr, nullptr, nullptr);;

    this->prepare_adu_from_skeleton();
    this->data_from_ats_files();
    this->get_calibration_xml();

    this->adujob.reset();
    success = this->can_continue;

    // measdoc kann nur! durch adulib erzeugt werden

    if (this->can_continue && (this->measdoc != nullptr)) {
        return std::make_shared<measdocxml>(nullptr,nullptr, this->measdoc->to_QByteArray(), nullptr);
    }
    return std::make_shared<measdocxml>(nullptr,nullptr, nullptr, nullptr);
}

measdoc_from_mc_data::~measdoc_from_mc_data()
{
    if (this->measdoc != nullptr) this->measdoc.reset();
    if (this->mxcd != nullptr) this->mxcd.reset();

}

void measdoc_from_mc_data::slot_qmap_created(const QMap<QString, QVariant> &data_map)
{
    if (this->adujob != nullptr) {
        auto miter = data_map.constBegin();
        while (miter != data_map.constEnd()) {
            this->adujob->set(miter.key(), miter.value(), 0);
            ++miter;
        }
    }
}

void measdoc_from_mc_data::connect_measdoc()
{
    if (!this->can_continue) return;
    connect(this->measdoc.get(), &measdocxml::signal_gps_status_qmap_created, this, &measdoc_from_mc_data::slot_qmap_created);
    connect(this->measdoc.get(), &measdocxml::signal_txm_cycle_qmap_created, this, &measdoc_from_mc_data::slot_qmap_created);
    connect(this->measdoc.get(), &measdocxml::signal_txm_sequence_qmap_created, this, &measdoc_from_mc_data::slot_qmap_created);
    connect(this->measdoc.get(), &measdocxml::signal_txm_waveform_qmap_created, this, &measdoc_from_mc_data::slot_qmap_created);
    connect(this->measdoc.get(), &measdocxml::signal_txm_epos_qmap_created, this, &measdoc_from_mc_data::slot_qmap_created);
}

void measdoc_from_mc_data::get_calibration_xml()
{

    for (auto &ich : used_channels) {
        for ( auto & ats : mxcd->in_atsfiles) {
            if (ats->value("channel_number").toInt() == ich) {
                QString cal(ats->get_xmlcal());
                if (cal != "empty") this->adujob->set("xmlcal_sensor", QVariant(cal), ich, false);
            }
        }

    }



//    else {
//        if (this->calibration_db.exists()) {

//        }
//    }



}

bool measdoc_from_mc_data::prepare_adu_from_skeleton()
{
    if (!this->can_continue) return false;

    this->xml_cnf_basedir= procmt_homepath("5chanEHnoise") + "/";
    this->HWConfig =      this->xml_cnf_basedir +      "HwConfig.xml";
    this->HwDatabase =    this->xml_cnf_basedir +    "HwDatabase.xml";
    this->HwStatus =      this->xml_cnf_basedir +      "HwStatus.xml";
    this->ChannelConfig = this->xml_cnf_basedir + "ChannelConfig.xml";

    QFileInfo test;
    test.setFile(this->HWConfig);
    if (!test.exists()) { this->can_continue = false; return false;}
    test.setFile(this->HwDatabase);
    if (!test.exists()) { this->can_continue = false; return false;}
    test.setFile(this->HwStatus);
    if (!test.exists()) { this->can_continue = false; return false;}
    test.setFile(this->ChannelConfig);
    if (!test.exists()) { this->can_continue = false; return false;}

    this->qfiHWConfig = new QFileInfo(HWConfig);
    this->qfiHwDatabase = new QFileInfo(HwDatabase);
    this->qfiChannelConfig = new QFileInfo(ChannelConfig);
    this->qfiHwStatus = new QFileInfo(HwStatus);

    this->adujob->fetch_hwcfg_virtual(10, 512.);
    this->adujob->fetch_hwdb(qfiHwDatabase);
    this->adujob->build_hardware();

    return true;
}

bool measdoc_from_mc_data::data_from_ats_files()
{
    if (!this->can_continue) return false;
    if (!this->mxcd->in_atsfiles.size()) {
        this->can_continue = false;
        return false;
    }
    QList<QFileInfo> qfis;
    int max_chan = 10;

    eQDateTime  starts = mxcd->in_atsfiles.at(0)->get_start_datetime();
    this->adujob->set("sample_freq", mxcd->in_atsfiles.at(0)->get_sample_freq(), -1);
    this->adujob->set_start_time(starts);
    this->adujob->on_set_duration(qint64( (double(mxcd->in_atsfiles.at(0)->get_num_samples())) / mxcd->in_atsfiles.at(0)->get_sample_freq() ),false);

    for ( auto & ats : mxcd->in_atsfiles) {

        if (ats->value("channel_number").toInt() < max_chan) {
            qfis.append(QFileInfo(ats->absoluteFilePath()));
            QMap<QString, QVariant> sec = ats->get_data();
            auto sec_iter = sec.cbegin();
            while (sec_iter != sec.cend()) {
                this->adujob->set(sec_iter.key(), sec_iter.value(), ats->value("channel_number").toInt(), false);
                sec_iter++;
            }
            used_channels.push_back(ats->value("channel_number").toInt());
            this->adujob->set("Latitude", ats->value("latitude"),  ats->value("channel_number").toInt(), false);
            this->adujob->set("Longitude", ats->value("longitude"),  ats->value("channel_number").toInt(), false);
            this->adujob->set("Height", ats->value("elevation"),  ats->value("channel_number").toInt(), false);
            this->adujob->set("chopper_status", ats->value("chopper_status"),  ats->value("channel_number").toInt(), false);
            this->adujob->set("ats_data_file", ats->get_qfile_info().fileName(), ats->value("channel_number").toInt(), false);
            this->adujob->set("ts_lsb", ats->value("lsb"),  ats->value("channel_number").toInt(), false);
            ats->get_new_filename(false);


        }
    }
}
