#include "atsheader.h"


atsheader::atsheader(const QFileInfo &qfi, const short &header_version, QObject *parent) :  QObject(parent), QFileInfo(qfi), prc_com(false)
{
    this->clear(header_version);
    qds.setByteOrder(QDataStream::LittleEndian);
    this->file.setFileName(this->absoluteFilePath());
    this->qds.setDevice(&this->file);

    this->prepare_options();

    this->parse_filename_header_version();
}

atsheader::atsheader(const QString &dir, const QString &name, const short &header_version, QObject *parent) : QObject(parent), prc_com(false)
{
    this->clear(header_version);
    qds.setByteOrder(QDataStream::LittleEndian);
    if (dir.size() && dir.endsWith("/")) this->setFile(dir.trimmed() + name.trimmed());
    else if (dir.size() && dir.endsWith("\\")) this->setFile(dir.trimmed() + "/" + name.trimmed());
    else this->setFile(dir.trimmed() + "/" + name.trimmed());

    this->file.setFileName(this->absoluteFilePath());
    this->qds.setDevice(&this->file);

    this->prepare_options();

    this->parse_filename_header_version();

}

atsheader::atsheader(const QFileInfo &qfi, const QByteArray &qba, const bool &scope_mode, QObject *parent)  :  QObject(parent), QFileInfo(qfi), prc_com(false)
{
    if (qba.size() > 1024) return;
    this->clear(80);
    qds.setByteOrder(QDataStream::LittleEndian);
    this->file.setFileName(this->absoluteFilePath());
    this->qds.setDevice(&this->file);

    this->prepare_options();
    this->parse_filename_header_version();

    this->write_header_qba(qba, scope_mode, true, false);


}



short int atsheader::get_header_version() const
{
    return short (this->value("header_version").toInt());
}


atsheader::atsheader(const atsheader &atsh)  :  QObject(Q_NULLPTR), QFileInfo(atsh.absoluteFilePath()), prc_com(false)
{
    this->operator =(atsh);
    qDebug() << "header copy";
}


void atsheader::set_basedir(QDir const &basedir)
{
    this->setFile(basedir.absolutePath() + "/" + this->fileName());
    this->file.setFileName(this->absoluteFilePath());
    this->prc_com_to_classmembers();


}

void atsheader::set_basedir_fake_deep(const QDir &basedir)
{
    this->setFile(basedir.absolutePath() + "/meas/" + this->fileName());
    this->file.setFileName(this->absoluteFilePath());
    this->prc_com_to_classmembers();

}

bool atsheader::dip_to_pos(const double length, const double decangle)
{
    if ( (fabs(length) < 0.001) ) return false;
    double angle = decangle;
    if ( (fabs(angle) < 0.1) ) angle = 0.0;
    insert_double("pos_x1", 0.0);
    insert_double("pos_y1", 0.0);
    insert_double("pos_z1", 0.0);
    insert_double("pos_x2", 0.0);
    insert_double("pos_y2", 0.0);
    insert_double("pos_z2", 0.0);

    const double assume_zero_length = 10E-3;
    insert_double("pos_x1", -cos(M_PI / 180. * angle) * length / 2.);
    insert_double("pos_x2",  cos(M_PI / 180. * angle) * length / 2.);
    insert_double("pos_y1", -sin(M_PI / 180. * angle) * length / 2.);
    insert_double("pos_y2",  sin(M_PI / 180. * angle) * length / 2.);

    if  (fabs(dvalue("pos_x1")) <  assume_zero_length) insert_double("pos_x1", 0.);
    if  (fabs(dvalue("pos_x2")) <  assume_zero_length) insert_double("pos_x2", 0.);
    if  (fabs(dvalue("pos_y1")) <  assume_zero_length) insert_double("pos_y1", 0.);
    if  (fabs(dvalue("pos_y2")) <  assume_zero_length) insert_double("pos_y2", 0.);
    if  (fabs(dvalue("pos_z1")) <  assume_zero_length) insert_double("pos_z1", 0.);
    if  (fabs(dvalue("pos_z2")) <  assume_zero_length) insert_double("pos_z2", 0.);


    this->prc_com_to_classmembers();

    return true;
}



atsheader::atsheader(const int &file_version, const int header_version, const int &system_serial_number, const double &sample_freq,
                     const int &run_number, const int &channel_number, const QString &channel_type, const QString &ADB_board_type,
                     const QString &observatory, const QString &ceadate, const QString &ceameastype)
{
    this->clear(short(header_version));
    this->insert("file_version", file_version);
    this->insert("header_version", header_version);
    if (header_version >= 1080) {
        // CEA header
        this->insert("header_length", quint16(33760));
    }
    else this->insert("header_length", quint16(1024));
    this->insert("system_serial_number", system_serial_number);
    this->insert("run_number", run_number);
    this->insert("channel_number", channel_number);
    this->insert("channel_type", channel_type);
    this->insert("ADB_board_type", ADB_board_type);
    this->insert("observatory", observatory);
    this->insert("ceadate", ceadate);
    this->insert("ceameas", ceameastype);

    // calls map members
    this->set_sample_freq(sample_freq, true, false);

}

atsheader::atsheader(QObject *parent) : QObject(Q_NULLPTR), QFileInfo(), prc_com(false) {

    this->clear();
    this->classmembers_to_prc_com();
}

atsheader &atsheader::operator = (atsheader const &atsh)
{
    if(&atsh == this) return *this;
    this->clear(atsh.get_header_version());
    prc_com::operator = (atsh);
    this->setFile(atsh.absoluteFilePath());

    if (!this->svalue("ADB_board_type").size()) {
        if (this->dvalue("sample_freq") < 4096.1) {
            this->insert("ADB_board_type", "LF");

        }
        else {
            this->insert("ADB_board_type", "HF");

        }

    }



    this->prc_com_to_classmembers();
    this->dipole_length = atsh.dipole_length;
    this->headersize_buf = atsh.headersize_buf;
    if (atsh.atswriter_section.size()) {
        this->atswriter_section = atsh.atswriter_section;
    }

    this->options = atsh.options;
    return *this;
}


atsheader::~atsheader()
{
    if (this->file.isOpen()) this->file.close();
    if (this->msg != nullptr) this->msg.reset();
}

short int atsheader::parse_filename_header_version()
{
    if (!this->baseName().size()) return 0;
    QStringList tokens = this->baseName().split("_");

    // check if default 80, 90 metronix ats header
    //  008_V01_C00_R000_TEx_BL_32H.ATS
    if (tokens.size() > 6) {
        if (tokens.at(2).contains("C") && tokens.at(3).contains("R") && tokens.at(4).contains("T")) {

            if  (this->value("header_version").toInt() >= 1080) {

                qWarning()  << "atsheader::parse_filename_header_version " << " -> metronix ats file - but CEA header given";
            }
            else if (this->value("header_version").toInt() == 0) {
                this->insert("header_version", short (80));
                this->set_key_value("file_version", tokens.at(1).mid(1));
                this->set_key_value("run_number", tokens.at(3).mid(1));


            }
        }
    }
    // not a metronix file - assume CEA file
    else {
        if (this->value("header_version").toInt() == 0) {
            this->insert("header_version", short (1080));
        }

        // assumed a CEA file but header wnanted metronix
        else if ( (this->value("header_version").toInt() > 0) && (this->value("header_version").toInt() < 1080) ) {
            qWarning()  << "atsheader::parse_filename_header_version " << " -> CEA ats file - but metronix header given";
        }

    }

    return short (this->value("header_version").toInt());

}


double atsheader::get_dipole_length() const
{
    double tx, ty, tz;
    tx = double(bin_atsheader.rPosX2 - bin_atsheader.rPosX1);
    ty = double(bin_atsheader.rPosY2 - bin_atsheader.rPosY1);
    tz = double(bin_atsheader.rPosZ2 - bin_atsheader.rPosZ1);

    double diplength = sqrt(tx * tx  + ty * ty + tz * tz);
    if (diplength < 0.001) diplength = 0;                   // avoid rounding errors
    return diplength;
}

void atsheader::scale_dipole_length(const double factor)
{
    insert_double("pos_x1", dvalue("pos_x1") * factor);
    insert_double("pos_y1", dvalue("pos_y1") * factor);
    insert_double("pos_z1", dvalue("pos_z1") * factor);
    insert_double("pos_x2", dvalue("pos_x2") * factor);
    insert_double("pos_y2", dvalue("pos_y2") * factor);
    insert_double("pos_z2", dvalue("pos_z2") * factor);

    this->prc_com_to_classmembers();
}

double atsheader::angle() const
{
    double tx, ty;
    tx = double(bin_atsheader.rPosX2 - bin_atsheader.rPosX1);
    ty = double(bin_atsheader.rPosY2 - bin_atsheader.rPosY1);

    double diplength = this->get_dipole_length();

    // avoid calculation noise
    if (fabs(tx) < 0.001) tx = 0.0;
    if (fabs(ty) < 0.001) ty = 0.0;

    if ((diplength == 0) && !this->value("channel_type").toString().compare("Hx")) {
        return 0.;                                                  // NORTH
    }
    // Hy
    if ((diplength == 0) && !this->value("channel_type").toString().compare("Hy")) {
        return M_PI_2;                                              // EAST
    }
    // Hz
    if ((diplength == 0) && !this->value("channel_type").toString().compare("Hz")) {
        return  0.;
    }



    // hmm hmm possible but you normally set the system N S E W
    return atan2(ty,tx);
}

void atsheader::set_start_seconds_offset(const qint64 &start_seconds_offset)
{
    if (!start_seconds_offset) this->filepos_offset = 0;

    else if (double(this->bin_atsheader.rSampleFreq )> 0.999999999999) {
        qint64 ifreq = qint64(this->bin_atsheader.rSampleFreq);
        if (this->is_64bit_samples()) {
            this->filepos_offset = (ifreq * start_seconds_offset * qint64(sizeof(qint64)));
        }
        else this->filepos_offset = (ifreq * start_seconds_offset * qint64(sizeof(qint32)));
    }

    //!@todo here we must avoid that we say 7 seconds but - maybe don't get 7s with 4sec samples 0,4,8 ,,,,
    else {
        if (this->is_64bit_samples()) {
            this->filepos_offset = qint64(double(start_seconds_offset * qint64(sizeof(qint64))) / double(this->bin_atsheader.rSampleFreq));
        }
        else this->filepos_offset = qint64(double(start_seconds_offset * qint64(sizeof(qint32))) / double(this->bin_atsheader.rSampleFreq));
    }

    this->start_seconds_offset = start_seconds_offset;
}

bool atsheader::is_64bit_samples() const
{
    if ( (this->bin_atsheader.bit_indicator == 1) && (this->bin_atsheader.siHeaderVers > 80) && (this->bin_atsheader.siHeaderVers != 1080) ) return true;
    return false;
}


void atsheader::set_samples_offset(const quint64 &samples_offset)
{
    if (this->is_64bit_samples()) {
        this->filepos_offset = qint64(samples_offset * sizeof(qint64));
    }
    else this->filepos_offset = qint64(samples_offset * sizeof(qint32));
}

QDataStream::Status atsheader::get_QDataStream_status() const
{
    return this->qds.status();
}

QString atsheader::set_run(const int &negative_increments_positive_sets)
{
    if (negative_increments_positive_sets >= 0) {
        this->set_key_value("run_number", negative_increments_positive_sets);
    }
    else {
        int rn = this->ivalue("run_number");
        this->set_key_value("run_number", abs(negative_increments_positive_sets) + rn);
    }

    this->prc_com_to_classmembers();
    return this->absoluteFilePath();

}

QString atsheader::subname_file() const
{
    return this->svalue_rformat("system_serial_number", 3, '0') + "_V" + this->svalue_rformat("file_version", 2, '0') + "_C" + this->svalue_rformat("channel_number", 2, '0')
            + "_R" + this->svalue_rformat("run_number", 3, '0') + "_T" + this->svalue("channel_type") +
            "_B" + this->svalue("ADB_board_type").at(0) + "_" + this->svalue("fname_sample_period") + this->svalue("fname_sample_period_unit") + "." + this->suffix;
}

QString atsheader::subname_xml() const
{
    return this->svalue_rformat("system_serial_number", 3, '0') + "_" + this->edt_start.measdoc_part_time(this->quint64value("num_samples")) +  "_R" + this->svalue_rformat("run_number", 3, '0')
            + "_" + this->svalue("fname_sample_period") + this->svalue("fname_sample_period_unit") + ".xml";
}

std::shared_ptr<calibration> atsheader::get_cal() const
{
    return this->calib;
}

void atsheader::prepare_options()
{
    this->options.set_class_name("ats");
    this->options.insert("calibration", "auto");        // auto, txt, xml, sql, theo, off
    this->options.insert_szt("wl", 0);                  // FFT window length or read chunk
    this->options.insert_double("fcut_upper", 0.0);     // cut upper part of FFT
    this->options.insert_double("fcut_lower", 0.0);     // cut lower part of FFT
    this->options.insert_szt("use_samples", 0);         // max samples
    this->options.insert_szt("overlapping", 0);         // overlapping means read 1024 256 (overlapping move back) and read again from ; the FIR filter calculates this and sets the lcoal variable move_back_after_read
    this->options.insert_szt("last_read", 0);           // last window to read - which has a different size (smaller) than all previous; can be used for ascii export BUT NOT for FFT or FIR filter, because all window MUST be same here
    this->options.insert("scale", "auto");              // automatically scale E field to mV/km; maybe don't want to use it for FIR filtering in order to avoid different E length for the filtered, which can be confusing
    this->options.insert("parzen_radius", double(0.0));
    this->options.insert("apply_syscal", 0);            // apply additionial system calibrattion of the ADB boards
    this->options.insert("other_calibration_db_absolute_filepath", "");   // try another calibration database to open instead of ../db/calibration.sql3
}

QString atsheader::get_new_filename(const bool bchange_measdir, const QString &type)
{
    QString name = "empty";
    int fname_sample_period;
    QString fname_sample_period_unit;
    if (this->dvalue("output_sample_freq") != 0.0) {
        this->sample_freq_to_file_str(this->dvalue("output_sample_freq"), fname_sample_period, fname_sample_period_unit);
    }
    else {
        this->sample_freq_to_file_str(this->dvalue("sample_freq"), fname_sample_period, fname_sample_period_unit);
    }
    this->insert("fname_sample_period",fname_sample_period);
    this->insert("fname_sample_period_unit", fname_sample_period_unit);
    if (!QString::compare("07", type) || !QString::compare("08", type) ) {

        name = this->subname_file();


        if (bchange_measdir) {
            QString tmpmeas = this->edt_start.measdir_time();

            QDir dirs = this->absoluteDir();
            if(dirs.cdUp()) {
                QDir tmp(dirs.path() + "/" + tmpmeas);
                this->setFile(tmp, name);

                if ( (this->can_write == false) || (!this->file.isOpen()))
                    if (this->exists()) {
                        do {
                            int run = this->ivalue("run_number");
                            this->insert("run_number", ++run);
                            name = this->subname_file();
                            this->setFile(tmp, name);
                        } while (this->exists() && (ivalue("run_number") < 999));

                    }

                this->measdoc_name = this->subname_xml();
                this->insert("xml_header", measdoc_name);
                strncpy(this->bin_atsheader.tscComment.achXmlHeader, this->value("xml_header").toString().toLatin1(), sizeof(this->bin_atsheader.tscComment.achXmlHeader));


            }
            else {
                this->setFile(this->absoluteDir(), name);
            }
        }
        else {
            this->setFile(this->absoluteDir(), name);
        }

    }


    //        std::cerr << "emiting filename changed for channel" << this->ivalue("channel_number") << this->baseName().toStdString() << std::endl;
    ///! @todo check for thread safety
    this->notify_xml();

    std::cerr << "atsheader::get_new_filename changed:" << this->absoluteFilePath().toStdString() << std::endl;
    return this->absoluteFilePath();
}

QMap<QString, QVariant> atsheader::get_atswriter_section() const
{

    QMap<QString, QVariant> atswriter_data;

    atswriter_data.insert("pos_x1", dvalue("pos_x1"));
    atswriter_data.insert("pos_x2", dvalue("pos_x2"));
    atswriter_data.insert("pos_y1", dvalue("pos_y1"));
    atswriter_data.insert("pos_y2", dvalue("pos_y2"));
    atswriter_data.insert("pos_z1", dvalue("pos_z1"));
    atswriter_data.insert("pos_z2", dvalue("pos_z2"));
    atswriter_data.insert("ats_data_file", svalue("ats_data_file"));
    atswriter_data.insert("ats_data_file", this->fileName());
    atswriter_data.insert("sensor_cal_file", svalue("sensor_cal_file"));
    atswriter_data.insert("ts_lsb", dvalue("lsb"));
    atswriter_data.insert("num_samples", quint64value("num_samples"));
    atswriter_data.insert("start_time", svalue("start_time"));
    atswriter_data.insert("start_date", svalue("start_date"));
    atswriter_data.insert("channel_type", svalue("channel_type"));
    atswriter_data.insert("sensor_type", svalue("sensor_type"));
    atswriter_data.insert("sample_freq", dvalue("sample_freq"));
    atswriter_data.insert("sensor_sernum", ivalue("sensor_sernum"));

    return atswriter_data;

}

QMap<QString, QVariant> atsheader::get_gps_section() const
{
    QMap<QString, QVariant> qps;
    qps.insert("Date", svalue("start_date"));
    qps.insert("Height", ivalue("elevation"));
    qps.insert("Latitude", ivalue("latitude"));
    qps.insert("Longitude", ivalue("longitude"));
    qps.insert("Time", svalue("start_time"));

    return qps;

}

void atsheader::set_suffix(const QString &suffix)
{
    this->suffix = suffix;
}

void atsheader::set_change_measdir(const bool true_or_false)
{
    this->change_measdir = true_or_false;
}

void atsheader::set_extra_signals(const bool &true_false)
{
    this->emit_extra = true_false;
}

void atsheader::notify_xml()
{
    if (this->atswriter_section.size()) {
        this->measdoc_name = this->subname_xml();
        this->insert("xml_header", measdoc_name);
        strncpy(this->bin_atsheader.tscComment.achXmlHeader, this->value("xml_header").toString().toLatin1(), sizeof(this->bin_atsheader.tscComment.achXmlHeader));

        emit this->filename_changed(this->atswriter_section, this->ivalue("channel_number"), this->absolutePath(), this->measdoc_name);
    }
}

QString atsheader::get_measdoc_name() const
{
    return this->measdoc_name;
}

QString atsheader::get_xmlcal() const
{
    if (this->calib != nullptr) {
        return calib->create_xlmstr();
    }
    return QString("empty");
}

QString atsheader::get_syscal_string() const
{

    QString str_settings;
    if ( (this->svalue("system_type") == "ADU08") && (this->svalue("ADB_board_type") == "LF")) str_settings.append("ADB08e-LF_");
    else if ( (this->svalue("system_type") == "ADU08") && (this->svalue("ADB_board_type") == "HF")) str_settings.append("ADB08e-HF_");
    else if ( (this->svalue("system_type") == "ADU07") && (this->svalue("ADB_board_type") == "LF")) str_settings.append("ADB07e-LF_");
    else if ( (this->svalue("system_type") == "ADU07") && (this->svalue("ADB_board_type") == "HF")) str_settings.append("ADB07e-HF_");
    else if ( (this->svalue("system_type") == "ADU07") && (this->svalue("ADB_board_type") == "MF")) str_settings.append("ADB07e-MF_");

    int gain_stage1_i = int(this->dvalue("gain_stage1"));
    int gain_stage2_i = int(this->dvalue("gain_stage2"));

    str_settings.append("Gain1-"); str_settings.append(QString::number(gain_stage1_i)); str_settings.append("_");
    str_settings.append("Gain2-"); str_settings.append(QString::number(gain_stage2_i)); str_settings.append("_");

    if (this->ivalue("input_divder_on") == 1)  str_settings.append("DIV-1_");
    else if (this->ivalue("input_divder_on") == 0)  str_settings.append("DIV-8_");

    QStringList filters = this->get_filter();

    for (auto const &str : filters) {
        str_settings.append(str + "_");
    }

    // H is maby strange is resistivity BUT!! H == DIV_8 always; therefor we are in the cal function WITHOUT sensor resistance
    str_settings.append("SensorResistance-");
    quint64 val = std::abs(int(this->dvalue("probe_resistivity")));
    str_settings.append(QString::number(val));


    str_settings.append(".txt");

    return str_settings;
}

void atsheader::set_ats_options(const std::shared_ptr<prc_com> cmdline)
{
    this->options.set_data(cmdline->get_data(), false);
}

void atsheader::set_ats_options(const prc_com &cmdline)
{
    this->options.set_data(cmdline.get_data(), false);

}

void atsheader::set_ats_option(const QString &key, const QVariant &value)
{
    this->options.set_key_value(key, value);
}

QVariant atsheader::get_ats_option(const QString &key) const
{
    if (!this->options.keys().contains(key)) return QVariant("empty");
    return this->options.value(key);
}

void atsheader::init_calibration(const int auto0_txt1_sql2_xml3, const bool enable_adb_cal)
{

    this->info_db.setFile(procmt_homepath("info.sql3"));
    this->master_cal_db.setFile(procmt_homepath("master_calibration.sql3"));

    if (enable_adb_cal) {
        if (this->sys_calib != nullptr) this->sys_calib.reset();
        this->sys_calib = std::make_shared<calibration>(this->info_db);
        QFileInfo virtual_file(this->get_syscal_string());
        this->sys_calib->parse_filename_other_cal(virtual_file);
        this->sys_calib->set_sample_freq(this->get_sample_freq());
        if (this->ivalue("chopper_status")) this->sys_calib->chopper = cal::chopper_on;
        else this->sys_calib->chopper = cal::chopper_on;
        this->options.insert("apply_syscal", 1);



    }
    else {
        this->sys_calib.reset();
    }
    if (this->calib != nullptr) this->calib.reset();

    this->calib = std::make_shared<calibration>(QFileInfo(this->info_db.absoluteFilePath()), ivalue("channel_number"), this->slot,
                                                true, this->baseName() + "_cal_query", Q_NULLPTR, this->msg);
    this->calib->open_master_cal(this->master_cal_db, this->baseName() + "_mastercal_query");

    // are we in survey dir
    // must have /ts/"
    // and  /cal/ on the same level

    qDebug() << this->svalue("sensor_type") << this->ivalue("sensor_sernum");

    QString calres("empty");

    if ((auto0_txt1_sql2_xml3 == 0) || (auto0_txt1_sql2_xml3 == 1)) {
        if (this->svalue("sensor_type").size() && (this->ivalue("sensor_sernum") != INT_MAX) && this->survey_basedir.absolutePath().size() ) {
            QDir caldir(this->survey_basedir.absolutePath() + "/cal");
            calres = this->calib->search_coil_dir(caldir, this->svalue("sensor_type"), this->ivalue("sensor_sernum"), ivalue("channel_number"), true );

        }
    }
    if (((auto0_txt1_sql2_xml3 == 0) || (auto0_txt1_sql2_xml3 == 2)) && (calres == "empty")) {


        QDir caldir(this->survey_basedir.absolutePath() + "/db");
        QString db_name("calibration.sql3");
        QString connection_name = "calib" + this->svalue("channel_type") + this->svalue("channel_number");
        QFileInfo excal(caldir.absolutePath() + "/" + db_name);

        if (this->options.svalue("other_calibration_db_absolute_filepath").size() > 3) {
            QFileInfo other(this->options.svalue("other_calibration_db_absolute_filepath"));
            caldir.setPath(other.absolutePath());
            db_name = other.fileName();
            excal.setFile(other.absoluteFilePath());
        }



        if (excal.exists()) {
            this->calib->open_create_db(db_name, caldir, connection_name);
            if (this->calib->select_coil(this->svalue("sensor_type"), this->ivalue("sensor_sernum"), true)) {
                calres = this->calib->sensortype + QString::number(calib->sernum);
            }

            else calres = QString("empty");
        }
    }
    if (((auto0_txt1_sql2_xml3 == 0) || (auto0_txt1_sql2_xml3 == 3)) && (calres == "empty")) {
        if (svalue("xml_header").size()) {
            QFileInfo qfimx(this->absolutePath()+ "/" + svalue("xml_header"));
            this->calib->read_measdoc(ivalue("channel_number"), cal::chopper_on, qfimx);
            this->calib->read_measdoc(ivalue("channel_number"), cal::chopper_off, qfimx);
            if (this->calib->ampl_off.size() && this->calib->ampl_on.size()) {
                calres = this->calib->sensortype + QString::number(calib->sernum);
            }
            else calres = QString("empty");
        }
    }
    // take a master if really nothing exists
    if ( (auto0_txt1_sql2_xml3 == 0)  && ( (calres == "empty") || (this->calib->f_off.size() + this->calib->f_on.size() == 0)) ) {

        calres = this->calib->set_sensor(this->svalue("sensor_type"));
        if (calres != "empty") {
            this->calib->sensortype = calres;
            if (!this->calib->get_master_cal() ) {
                calres = QString("empty");
            }
            else {
                // make as if we had caldata
                this->calib->master_cal_to_caldata();
            }
        }
    }

    if (calres == "empty") {
        qDebug() << "can not determin calibration";

    }
    // do not delete calib in case of theo; however sensor type must be known!
    if (this->svalue("sensor_type").size() && (this->svalue("sensor_type") != "empty") && (this->options.svalue("calibration") == "theo")) return;
    if ((!this->calib->f_on.size()) && (!this->calib->f_off.size())) {
        if (this->sys_calib != nullptr) {
            this->calib->set_linear();
            return;
        }
        this->calib.reset();
    }
}

void atsheader::force_calibration(const QString which_caldb_absolute_filepath, const QString which_sensor_type, const int which_sensor_sernum)
{
    this->set_key_value("sensor_type", which_sensor_type);
    this->set_key_value("sensor_sernum", which_sensor_sernum);
    this->options.insert("other_calibration_db_absolute_filepath", which_caldb_absolute_filepath);   // try another calibration database to open instead of ../db/calibration.sql3
    this->prc_com_to_classmembers();
    this->init_calibration(2, false);
}


void atsheader::cp_essentials_to(std::shared_ptr<atsheader> ats, int i_6_7_8) const
{
    qDebug() << "copy /clean essentials and guess a new header";

    QString st(this->svalue("sensor_type").simplified());
    st.remove(" ");
    ats->set_key_value("sensor_type", st);

    st = this->svalue("channel_type");
    st = st.simplified();
    st.remove(" ");
    // this is a must for remapping the channels to  standard essential
    ats->set_key_value("channel_type", st);

    if (!QString::compare("ex", st, Qt::CaseInsensitive)) {
        ats->set_key_value("channel_number", 0);
        ats->set_key_value("sensor_type", "EFP06");

    }
    if (!QString::compare("ey", st, Qt::CaseInsensitive)) {
        ats->set_key_value("channel_number", 1);
        ats->set_key_value("sensor_type", "EFP06");

    }
    if (!QString::compare("hx", st, Qt::CaseInsensitive)) {
        ats->set_key_value("channel_number", 2);
    }
    if (!QString::compare("hy", st, Qt::CaseInsensitive)) {
        ats->set_key_value("channel_number", 3);
    }
    if (!QString::compare("hz", st, Qt::CaseInsensitive)) {
        ats->set_key_value("channel_number", 4);
    }
    if (!QString::compare("rhx", st, Qt::CaseInsensitive)) {
        ats->set_key_value("channel_number", 2);
    }
    if (!QString::compare("rhy", st, Qt::CaseInsensitive)) {
        ats->set_key_value("channel_number", 3);
    }
    if (!QString::compare("rhz", st, Qt::CaseInsensitive)) {
        ats->set_key_value("channel_number", 4);
    }
    if (ats->get_sample_freq() > 4096.1) {
        ats->set_LSBMV_BB_HF_default();
    }
    else {
        ats->set_LSBMV_BB_LF_default();
    }

    if (i_6_7_8 >= 6) {
        if (ats->get_sample_freq() > 512.1) {
            ats->set_key_value("chopper_status", 0);
        }
        else {
            ats->set_LSBMV_BB_LF_default();
            ats->set_key_value("chopper_status", 1);

        }
    }
    if (i_6_7_8 == 6) {
        ats->set_key_value("system_type", "ADU06");
    }
    else if (i_6_7_8 == 7) {
        ats->set_key_value("system_type", "ADU07");
    }
    else {
        ats->set_key_value("system_type", "ADU08");

    }


    ats->set_sample_freq(this->dvalue("sample_freq"), false, false);

    /*
    ats->set_key_size_t("num_samples", this->quint64value("num_samples"));

    // we want top copy the data and therfor the samples will be set
    //if (isadu06) ats->set_key_size_t("num_samples_64bit", this->quint64value("num_samples"));
    //else ats->set_key_size_t("num_samples_64bit", this->quint64value("num_samples_64bit"));

    ats->set_key_value("start_date_time", this->qint64value("start_date_time"));
    ats->set_key_value("pos_x1", this->dvalue("lsb"));
    ats->set_key_value("pos_y1", this->dvalue("lsb"));
    ats->set_key_value("pos_z1", this->dvalue("lsb"));
    ats->set_key_value("pos_x2", this->dvalue("lsb"));
    ats->set_key_value("pos_y2", this->dvalue("lsb"));
    ats->set_key_value("pos_z2", this->dvalue("lsb"));

    ats->set_key_value("probe_resistivity", this->dvalue("probe_resistivity"));
    ats->set_key_value("DC_offset", this->dvalue("DC_offset"));
    ats->set_key_value("gain_stage1", this->dvalue("gain_stage1"));
    ats->set_key_value("gain_stage2", this->dvalue("gain_stage2"));
    // I assume very old data
    ats->set_key_value("orig_sample_freq", this->dvalue("sample_freq"));
    ats->set_key_value("powerline_freq1", this->dvalue("powerline_freq1"));
    ats->set_key_value("powerline_freq2", this->dvalue("powerline_freq2"));
    ats->set_key_value("DC_offset_correction_value", this->dvalue("DC_offset_correction_value"));

    ats->set_key_value("system_serial_number", this->value("system_serial_number"));

    ats->set_key_value("chopper_status", this->value("chopper_status"));



    ats->set_key_value("sensor_sernum", this->value("sensor_sernum"));
    ats->set_key_value("latitude", this->value("latitude"));
    ats->set_key_value("longitude", this->value("longitude"));
    ats->set_key_value("elevation", this->value("elevation"));
    ats->set_key_value("lat_long_type", this->value("lat_long_type"));
    ats->set_key_value("system_type", this->value("ADU08"));


    ats->set_key_value("UTC_GMT_offset", this->value("UTC_GMT_offset"));
    ats->set_key_value("AddCoordType", this->value("AddCoordType"));
    ats->set_key_value("GPS_status", this->value("GPS_status"));
    ats->set_key_value("GPS_accuracy", this->value("GPS_accuracy"));
    ats->set_key_value("UTCOffset", this->value("UTCOffset"));
    ats->set_key_value("survey_header_name", this->value("survey_header_name"));
    ats->set_key_value("meas_type", this->value("meas_type"));
    ats->set_key_value("DC_offset_corr_on", this->value("DC_offset_corr_on"));
    ats->set_key_value("input_divder_on", this->value("input_divder_on"));
    ats->set_key_value("selftest_result", this->value("selftest_result"));
    ats->set_key_value("LF_filters", this->value("LF_filters"));
    ats->set_key_value("HF_filters", this->value("HF_filters"));


*/
}

void atsheader::clear(const short int &header_version)
{
    this->bin_atsheader.uiHeaderLength = 0;
    this->bin_atsheader.siHeaderVers = header_version;
    this->bin_atsheader.uiSamples = 0;
    this->bin_atsheader.rSampleFreq = 0;
    this->bin_atsheader.uiStartDateTime = 0;
    this->bin_atsheader.dblLSBMV = 0;
    this->bin_atsheader.iGMTOffset = 0;
    this->bin_atsheader.rOrigSampleFreq = 0;
    this->bin_atsheader.uiADUSerNum = 0;
    this->bin_atsheader.uiADCSerNum = 0;
    this->bin_atsheader.uiChanNo = 0;
    this->bin_atsheader.uiChopper = 0;
    strncpy(this->bin_atsheader.achChanType, "", sizeof(this->bin_atsheader.achChanType));
    strncpy(this->bin_atsheader.achSensorType, "", sizeof(this->bin_atsheader.achSensorType));
    this->bin_atsheader.siSensorSerNum = 0;
    this->bin_atsheader.rPosX1 = 0;
    this->bin_atsheader.rPosY1 = 0;
    this->bin_atsheader.rPosZ1 = 0;
    this->bin_atsheader.rPosX2 = 0;
    this->bin_atsheader.rPosY2 = 0;
    this->bin_atsheader.rPosZ2 = 0;
    this->bin_atsheader.rDipLength = 0;
    this->bin_atsheader.rAngle = 0;
    this->bin_atsheader.rProbeRes = 0;
    this->bin_atsheader.rDCOffset = 0;
    this->bin_atsheader.rPreGain = 0;
    this->bin_atsheader.rPostGain = 0;
    this->bin_atsheader.iLatitude = 0;
    this->bin_atsheader.iLongitude = 0;
    this->bin_atsheader.iElevation = 0;
    this->bin_atsheader.byLatLongType = 'G';
    this->bin_atsheader.byAddCoordType = 'U';
    this->bin_atsheader.siRefMeridian = 0;
    this->bin_atsheader.dblNorthing = 0;
    this->bin_atsheader.dblEasting = 0;
    this->bin_atsheader.byGPSStat = 0;
    this->bin_atsheader.byGPSAccuracy = 0;
    this->bin_atsheader.iUTCOffset = 0;
    strncpy(this->bin_atsheader.achSystemType, "", sizeof(this->bin_atsheader.achSystemType));
    strncpy(this->bin_atsheader.achSurveyHeaderName, "", sizeof(this->bin_atsheader.achSurveyHeaderName));
    strncpy(this->bin_atsheader.achMeasType, "", sizeof(this->bin_atsheader.achMeasType));
    this->bin_atsheader.DCOffsetCorrValue = 0;
    this->bin_atsheader.DCOffsetCorrOn = 0;
    this->bin_atsheader.InputDivOn = 0;
    this->bin_atsheader.bit_indicator = 0;
    strncpy(this->bin_atsheader.achSelfTestResult, "", sizeof(this->bin_atsheader.achSelfTestResult));
    this->bin_atsheader.numslices = 0;
    this->bin_atsheader.siCalFreqs = 0;
    this->bin_atsheader.siCalEntryLength = 0;
    this->bin_atsheader.siCalVersion = 0;
    this->bin_atsheader.siCalStartAddress = 0;
    strncpy(this->bin_atsheader.abyLFFilters, "", sizeof(this->bin_atsheader.abyLFFilters));
    strncpy(this->bin_atsheader.achUTMZone, "", sizeof(this->bin_atsheader.achUTMZone));
    this->bin_atsheader.uiADUCalTimeDate = 0;
    strncpy(this->bin_atsheader.achSensorCalFilename, "", sizeof(this->bin_atsheader.achSensorCalFilename));
    this->bin_atsheader.uiSensorCalTimeDate = 0;
    this->bin_atsheader.rPowerlineFreq1 = 0;
    this->bin_atsheader.rPowerlineFreq2 = 0;
    strncpy(this->bin_atsheader.abyHFFilters, "", sizeof(this->bin_atsheader.abyHFFilters));
    this->bin_atsheader.uiSamples64bit = 0;
    this->bin_atsheader.rExtGain = 0;
    strncpy(this->bin_atsheader.achADBBoardType, "", sizeof(this->bin_atsheader.achADBBoardType));

    // comment block
    strncpy(this->bin_atsheader.tscComment.achClient, "", sizeof(this->bin_atsheader.tscComment.achClient));
    strncpy(this->bin_atsheader.tscComment.achContractor, "", sizeof(this->bin_atsheader.tscComment.achContractor));
    strncpy(this->bin_atsheader.tscComment.achArea, "", sizeof(this->bin_atsheader.tscComment.achArea));
    strncpy(this->bin_atsheader.tscComment.achSurveyID, "", sizeof(this->bin_atsheader.tscComment.achSurveyID));
    strncpy(this->bin_atsheader.tscComment.achOperator, "", sizeof(this->bin_atsheader.tscComment.achOperator));
    strncpy(this->bin_atsheader.tscComment.achSiteName, "", sizeof(this->bin_atsheader.tscComment.achSiteName));
    strncpy(this->bin_atsheader.tscComment.achXmlHeader, "", sizeof(this->bin_atsheader.tscComment.achXmlHeader));
    strncpy(this->bin_atsheader.tscComment.achComments, "", sizeof(this->bin_atsheader.tscComment.achComments));

    this->QMap<QString, QVariant>::clear();
    this->classmembers_to_prc_com();

    // activate all here; delete later
    bool is07 = true;
    bool is08 = true;


    //! filter banks setup contains following
    if(is07 || is08) LFFilters["LF-RF-1"] =     0x01;   //! ADU07/8_LF-RF-1 filter on LF board with capacitor 22pF
    if(is07 || is08) LFFilters["LF-RF-2"] =     0x02;   //! ADU07/8_LF-RF-2 filter on LF board with capacitor 122pF
    if (is07)        LFFilters["LF-RF-3"] =     0x04;   //! ADU07_LF-RF-3 filter on LF board with capacitor 242pF
    if (is07)        LFFilters["LF-RF-4"] =     0x08;   //! ADU07_LF-RF-4 filter on LF board with capacitor 342pF
    if(is07 || is08) LFFilters["LF_LP_4HZ"] =   0x10;   //! ADU07/8_LF_LP_4HZ filter on LF board with 4 Hz Lowpass characteristic

    if (is07)        LFFilters["MF-RF-1"] =     0x40;   //! ADU07_MF_RF_1 filter on MF board with capacitor 470nF
    if (is07)        LFFilters["MF-RF-2"] =     0x20;   //! ADU07_MF_RF_2 filter on MF board with capacitor 4.7nF

    // HF Path
    // 1 Hz has been dropped for 08
    if (is07)        HFFilters["HF-HP-1Hz"] =   0x01;   //! ADU07_HF-HP-1Hz 1Hz filter enable for HF board
    // 500Hz is the HP for 08
    if (is08)        HFFilters["HF-HP-500Hz"] = 0x02;   //! ADU08_HF-HP-500Hz 500Hz filter enable for HF board

    // **************************** non standard part of header and file

    // values must be inserted at first otherwise the checked input does not work with type comparision
    this->insert("file_version", int(99));
    this->insert("run_number", int(0));
    this->insert("observatory", QString("   "));
    this->insert("ceadate", QString("     "));
    this->insert("ceameas", QString("  "));
    this->insert("output_sample_freq", double(0.));
    this->insert("add_secs_to_start_time", qint64(0));



    if (this->file.isOpen()) this->file.close();

}

short int atsheader::get_header_size() const
{
    return short(this->bin_atsheader.uiHeaderLength);
}


quint64 atsheader::read_header(const bool &silent, const bool keep_open_for_append)
{

    // open in case
    if (!this->file.isOpen()) {
        if(!file.open(QIODevice::ReadOnly)) {
            return SIZE_MAX;
        }
    }
    else {
        file.seek(0);
    }

    qds.readRawData((char*) &this->bin_atsheader, sizeof(this->bin_atsheader));

    qDebug() << this->file.pos()  << "pos";

    if (this->qds.status()) {
        emit this->QDataStreamStatus(this->qds.status());
        return SIZE_MAX;
    }


    // prevent garbage from old headers
    if (this->bin_atsheader.siHeaderVers < 80) {
        this->bin_atsheader.DCOffsetCorrOn = 0;
        this->bin_atsheader.DCOffsetCorrValue = 0;
        this->bin_atsheader.InputDivOn = 0;
        this->bin_atsheader.uiSamples64bit = 0;
    }

    this->classmembers_to_prc_com();
    this->used_slices = 0;

    if (this->bin_atsheader.siHeaderVers >= 1080) {
        size_t ii;
        this->tslices.reserve(C_ATS_CEA_NUM_HEADERS);
        this->tslices.resize(C_ATS_CEA_NUM_HEADERS);
        for (ii = 0; ii < this->tslices.size(); ++ii) {
            qds.readRawData((char*) &this->tslices[ii], sizeof(ATSSliceHeader_1080));
        }

        for (ii = 0; ii < this->tslices.size(); ++ii) {
            if (this->tslices.at(ii).uiSamples > 0) {
                ++this->used_slices;
            }
        }

        this->insert("numslices", uint(this->used_slices));

    }

    this->headersize_buf = qint64 (this->bin_atsheader.uiHeaderLength);   // avoid endless casts
    this->get_dipole_length();                                            // prepare for scaled read

    if (!this->is_remote) {
        emit this->tx_adu_msg(ivalue("system_serial_number"), ivalue("channel_number"), this->slot, "atsheader::read_header -> atsfile opened " +  this->baseName());
    }
    else {
        emit this->tx_adu_msg(ivalue("system_serial_number"), ivalue("channel_number"), this->slot, "atsheader::read_header -> atsfile opened " +  this->baseName() + " (remote)");
    }



    this->can_read = true;
    this->can_write = false;
    this->activate_scalable();

    if (this->options.svalue("calibration") != "off") {
        this->init_calibration(0, false);
    }

    emit this->signal_sync_info(this->edt_start);

    if (keep_open_for_append) {
        this->file.close();
        if(!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            return SIZE_MAX;
        }
        this->qds.setDevice(&this->file);
        this->qds.setByteOrder(QDataStream::LittleEndian);

        this->can_write = true;
        this->can_read = false;
    }

    return this->quint64value("num_samples");
}

quint64 atsheader::write_header_qba(const QByteArray &qba, const bool &scope_mode, const bool &silent, const bool keep_open_for_append)
{

    //qds.readRawData((char*) &this->bin_atsheader, sizeof(this->bin_atsheader));
    this->bin_atsheader = *(ATSHeader_80*)qba.data();

    // or so back qba.append((char*)&this->bin_atsheader, sizeof(this->bin_atsheader));

    if (this->bin_atsheader.siHeaderVers >= 1080) {
      qFatal("CEA mode not possible");
      return ULLONG_MAX;
    }


    // prevent garbage from old headers
    if (this->bin_atsheader.siHeaderVers < 80) {
        this->bin_atsheader.DCOffsetCorrOn = 0;
        this->bin_atsheader.DCOffsetCorrValue = 0;
        this->bin_atsheader.InputDivOn = 0;
        this->bin_atsheader.uiSamples64bit = 0;
    }

    // set the new start time ATE
    if (scope_mode) {
        this->classmembers_to_prc_com();
        this->edt_start = this->edt_stop;
        this->bin_atsheader.uiSamples = 0;
        this->bin_atsheader.uiSamples64bit = 0;
        this->bin_atsheader.uiStartDateTime = quint32 (this->qint64value("start_date_time"));
    }

    this->classmembers_to_prc_com();
    this->used_slices = 0;



    this->headersize_buf = qint64 (this->bin_atsheader.uiHeaderLength);   // avoid endless casts
    this->get_dipole_length();                                            // prepare for scaled read

    if (!this->is_remote) {
        emit this->tx_adu_msg(ivalue("system_serial_number"), ivalue("channel_number"), this->slot, "atsheader::read_header -> atsfile opened " +  this->baseName());
    }
    else {
        emit this->tx_adu_msg(ivalue("system_serial_number"), ivalue("channel_number"), this->slot, "atsheader::read_header -> atsfile opened " +  this->baseName() + " (remote)");
    }


    this->activate_scalable();

    if (this->options.svalue("calibration") != "off") {
        this->init_calibration(0, false);
    }

    emit this->signal_sync_info(this->edt_start);

    size_t test = this->write_header(true);

    if (test == SIZE_MAX) return ULLONG_MAX;

    return this->quint64value("num_samples");  // should be zero in case of scope mode
}

size_t atsheader::write_header(const bool rewrite_close)
{
    if ((this->dvalue("output_sample_freq") != 0.0) && !rewrite_close) {
        //this->set_key_value("sample_freq", this->dvalue("output_sample_freq"));
        this->set_sample_freq(this->dvalue("output_sample_freq"), false, false);
    }

    if (!rewrite_close) {
        this->prc_com_to_classmembers();
        if (this->file.isOpen()) this->file.close();
        QDir dirs = this->absoluteDir();
        if (!dirs.exists()) {
            dirs.mkpath(dirs.absolutePath());
        }
        this->file.setFileName(this->absoluteFilePath());
        if(!file.open(QIODevice::WriteOnly)) {
            return SIZE_MAX;
        }
        this->qds.setDevice(&this->file);
        this->qds.setByteOrder(QDataStream::LittleEndian);
    }
    else {

        this->change_measdir = false;
        this->prc_com_to_classmembers();
        if (this->file.isOpen()) {
            this->file.flush();
            this->qds.resetStatus();
            this->file.close();
        }

        this->file.setFileName(this->absoluteFilePath());
        if(!file.open(QIODevice::ReadWrite)) {
            return SIZE_MAX;
        }
        this->qds.setDevice(&this->file);
        this->qds.setByteOrder(QDataStream::LittleEndian);
    }

    int bw = this->qds.writeRawData((char*) &this->bin_atsheader, sizeof(this->bin_atsheader));

    qDebug() << "header written, bytes:" << bw;

    if (this->bin_atsheader.siHeaderVers >= 1080) {
        size_t ii;
        this->tslices.reserve(C_ATS_CEA_NUM_HEADERS);
        this->tslices.resize(C_ATS_CEA_NUM_HEADERS);
        for (ii = 0; ii < this->tslices.size(); ++ii) {
            qds.writeRawData((char*) &this->tslices[ii], sizeof(ATSSliceHeader_1080));
        }
    }

    this->file.flush();

    if (!rewrite_close) {
        this->can_write = true;
        this->can_read = false;
    }
    else {
        this->file.close();
        this->qds.resetStatus();
        this->can_write = false;
        this->can_read = false;
    }

    this->get_new_filename(this->change_measdir);

    if (this->bin_atsheader.siHeaderVers >= 1080) return (size_t) (sizeof(this->bin_atsheader) + C_ATS_CEA_NUM_HEADERS * sizeof(ATSSliceHeader_1080));
    return sizeof(this->bin_atsheader);





}

quint64 atsheader::scan_header_close(const bool &silent)
{
    //    if (this->file.isOpen()) {
    //        file.seek(0);
    //    }
    quint64 xsamples = this->read_header(silent);
    if (this->file.isOpen()) {
        file.close();
    }

    // do not emit close here
    this->can_write = false;
    this->can_read = false;
    return xsamples;
}

std::string atsheader::console_site_name() const
{
    return value("site_name").toString().toStdString();
}

std::string atsheader::console_site_name_rr() const
{
    return value("site_name_rr").toString().toStdString();
}

std::string atsheader::console_site_name_emap() const
{
    return value("site_name_emap").toString().toStdString();
}

quint32 atsheader::get_start_datetime_ui() const
{
    return quint32 (this->qint64value("start_date_time"));
}

quint32 atsheader::get_stop_datetime_ui() const
{
    return quint32 (this->qint64value("stop_date_time"));
}

eQDateTime atsheader::get_start_datetime() const
{
    return this->edt_start;
}

eQDateTime atsheader::get_stop_datetime() const
{
    return this->edt_stop;
}

void atsheader::set_start_date_time_ui(const quint32 &secs_since_1970)
{
    this->insert("start_date_time", qint64(secs_since_1970));
    this->prc_com_to_classmembers();
}



double atsheader::get_sample_freq() const
{
    return this->dvalue("sample_freq");
}

QString atsheader::set_sample_freq(const double &sample_freq, const bool set_Hchopper_auto, const bool set_Echopper_auto)
{
    this->insert("sample_freq", sample_freq);
    int fname_sample_period;
    QString fname_sample_period_unit;
    this->sample_freq_to_file_str(sample_freq, fname_sample_period, fname_sample_period_unit);
    this->insert("fname_sample_period",fname_sample_period);
    this->insert("fname_sample_period_unit", fname_sample_period_unit);

    if (set_Hchopper_auto) {
        if (sample_freq > 512) {
            this->insert("chopper_status", 0);

        }
        else {
            this->insert("chopper_status", 1);

        }
    }

    // E chopper (on main baqckplain does not exist in reality
    if (set_Echopper_auto) {
        if (sample_freq > 512) {
            this->insert("chopper_status", 0);
        }
        else {
            this->insert("chopper_status", 1);

        }
    }



    this->prc_com_to_classmembers();
    return this->absoluteFilePath();

}

void atsheader::set_LSBMV(const double &lsb_mv)
{

    this->insert("lsb", double(lsb_mv));
    this->prc_com_to_classmembers();
}

void atsheader::set_LSBMV_BB_LF_default()
{
    this->insert("lsb", double (5.970E-7));
    this->insert("ADB_board_type", "LF");
    this->prc_com_to_classmembers();
}

void atsheader::set_LSBMV_BB_HF_default()
{
    //
    this->insert("lsb", double(1.190E-6));
    this->insert("ADB_board_type", "HF");
    this->prc_com_to_classmembers();
}

void atsheader::set_e_pos(const double &x1, const double &x2, const double &y1, const double &y2, const double &z1, const double &z2)
{
    this->insert("pos_x1", x1);
    this->insert("pos_y1", y1);
    this->insert("pos_z1", z1);
    this->insert("pos_x2", x2);
    this->insert("pos_y2", y2);
    this->insert("pos_z2", z2);
    this->prc_com_to_classmembers();

}

void atsheader::set_start_date_time(const QDateTime &qdt)
{

    this->insert("start_date_time", ((qdt.toMSecsSinceEpoch() / 1000)));
    this->prc_com_to_classmembers();
}

void atsheader::sample_freq_to_file_str(const double &sample_freq, int &fname_sample_period, QString &fname_sample_period_unit)
{
    if (sample_freq >= 1.0) {
        fname_sample_period = int(sample_freq);
        fname_sample_period_unit = "H";
    }
    else {
        fname_sample_period = int((1./sample_freq));
        fname_sample_period_unit = "S";


    }
}

quint64 atsheader::size() const
{
    return this->quint64value("num_samples");
}

QString atsheader::channel_type() const
{

    return this->value("channel_type").toString();
}



QFileInfo atsheader::get_qfile_info() const
{
    QFileInfo qfix(this->absoluteFilePath());

    return qfix;
}

void atsheader::close(const bool rewrite)
{

    // can be closed earlier
    if (this->file.isOpen()) {
        this->file.close();
    }
    // notify the xml in case
    this->notify_xml();

    emit this->atsfile_closed(0);




}

void atsheader::set_atswriter_section(const QMap<QString, QVariant> &atswriter_section)
{
    this->atswriter_section = atswriter_section;
}

void atsheader::set_survey_basedir(const QDir base)
{
    this->survey_basedir = base;
}

void atsheader::set_messenger(std::shared_ptr<msg_logger> msg)
{
    if (this->msg != nullptr) {
        this->msg.reset();
    }
    this->msg = msg;
    connect(this, &atsheader::tx_adu_msg, msg.get(), &msg_logger::rx_adu_msg);
}

void atsheader::slot_apply_sys_calib(const uint on_1_off_0)
{
    this->init_calibration(0, on_1_off_0);
}



void atsheader::prc_com_to_classmembers()

{
    std::string sstr;
    this->bin_atsheader.uiHeaderLength =                quint16 (this->value("header_length").toUInt());
    this->bin_atsheader.siHeaderVers =                  qint16 (this->value("header_version").toInt());
    // check 64bit
    if (this->value("num_samples").toULongLong() > UINT32_MAX) {
        this->bin_atsheader.uiSamples = UINT32_MAX;
    }
    else {
        this->bin_atsheader.uiSamples =                 quint32 (this->value("num_samples").toUInt());
    }
    this->bin_atsheader.rSampleFreq =                   this->value("sample_freq").toFloat();
    this->bin_atsheader.uiStartDateTime =               quint32 ((this->qint64value("start_date_time") + this->qint64value("add_secs_to_start_time")));
    this->bin_atsheader.dblLSBMV =                      this->value("lsb").toDouble();
    this->bin_atsheader.iGMTOffset =                    qint32 (this->value("UTC_GMT_offset").toInt());
    this->bin_atsheader.rOrigSampleFreq =               this->value("orig_sample_freq").toFloat() ;
    this->bin_atsheader.uiADUSerNum =                   quint16 (this->value("system_serial_number").toUInt());
    this->bin_atsheader.uiADCSerNum =                   quint16 (this->value("ADC_sernum").toUInt());
    this->bin_atsheader.uiChanNo =                      quint8 (this->value("channel_number").toUInt());
    this->bin_atsheader.uiChopper =                     quint8 (this->value("chopper_status").toUInt());
    strncpy(this->bin_atsheader.achChanType, this->value("channel_type").toString().toLatin1(), sizeof(this->bin_atsheader.achChanType));
    strncpy(this->bin_atsheader.achSensorType, this->value("sensor_type").toString().toLatin1(), sizeof(this->bin_atsheader.achSensorType));
    this->bin_atsheader.siSensorSerNum =                qint16 (this->value("sensor_sernum").toUInt());
    this->bin_atsheader.rPosX1 =                        this->value("pos_x1").toFloat();
    this->bin_atsheader.rPosY1 =                        this->value("pos_y1").toFloat();
    this->bin_atsheader.rPosZ1 =                        this->value("pos_z1").toFloat();
    this->bin_atsheader.rPosX2 =                        this->value("pos_x2").toFloat();
    this->bin_atsheader.rPosY2 =                        this->value("pos_y2").toFloat();
    this->bin_atsheader.rPosZ2 =                        this->value("pos_z2").toFloat();
    this->bin_atsheader.rDipLength =                    this->value("dipole_length").toFloat();
    this->bin_atsheader.rAngle =                        this->value("dipole_angle").toFloat();
    this->bin_atsheader.rProbeRes =                     this->value("probe_resistivity").toFloat();
    this->bin_atsheader.rDCOffset =                     this->value("DC_offset").toFloat();
    this->bin_atsheader.rPreGain =                      this->value("gain_stage1").toFloat();
    this->bin_atsheader.rPostGain =                     this->value("gain_stage2").toFloat();
    this->bin_atsheader.iLatitude =                     qint32 (this->value("latitude").toInt());
    this->bin_atsheader.iLongitude =                    qint32 (this->value("longitude").toInt());
    this->bin_atsheader.iElevation =                    qint32 (this->value("elevation").toInt());
    this->bin_atsheader.byLatLongType =                 QVariant_QString_to_aby("lat_long_type");
    this->bin_atsheader.byAddCoordType =                quint8(this->ivalue("AddCoordType"));
    this->bin_atsheader.siRefMeridian =                 qint16 (this->value("GaussRefMeridian").toInt());
    this->bin_atsheader.dblNorthing =                   this->value("northing").toDouble();
    this->bin_atsheader.dblEasting =                    this->value("easting").toDouble();
    this->bin_atsheader.byGPSStat =                     QVariant_QString_to_aby("GPS_status");
    this->bin_atsheader.byGPSAccuracy =                 quint8(this->ivalue("GPS_accuracy"));
    this->bin_atsheader.iUTCOffset =                    qint16(this->value("UTCOffset").toInt());
    strncpy(this->bin_atsheader.achSystemType, this->value("system_type").toString().toLatin1(), sizeof(this->bin_atsheader.achSystemType));
    strncpy(this->bin_atsheader.achSurveyHeaderName, this->value("survey_header_name").toString().toUtf8(), sizeof(this->bin_atsheader.achSurveyHeaderName));
    strncpy(this->bin_atsheader.achMeasType, this->value("meas_type").toString().toLatin1(), sizeof(this->bin_atsheader.achMeasType));
    this->bin_atsheader.DCOffsetCorrValue = this->value("DC_offset_correction_value").toDouble();
    this->bin_atsheader.DCOffsetCorrOn =                qint8 (this->value("DC_offset_corr_on").toInt());
    this->bin_atsheader.InputDivOn =                    qint8 (this->value("input_divder_on").toInt());
    this->bin_atsheader.bit_indicator =                 qint16 (this->value("bit_indicator").toInt());
    strncpy(this->bin_atsheader.achSelfTestResult, this->value("selftest_result").toString().toLatin1(), sizeof(this->bin_atsheader.achSelfTestResult));
    this->bin_atsheader.numslices =                     quint16 (this->value("num_slices").toUInt());
    this->bin_atsheader.siCalFreqs =                    qint16 (this->value("cal_freqs").toInt()) ;
    this->bin_atsheader.siCalEntryLength =              qint16 (this->value("cal_entry_length").toInt());
    this->bin_atsheader.siCalVersion =                  qint16 (this->value("cal_version").toInt()) ;
    this->bin_atsheader.siCalStartAddress =             qint16 (this->value("cal_start_address").toInt()) ;
    strncpy(this->bin_atsheader.abyLFFilters, this->value("LF_filters").toString().toLatin1(), sizeof(this->bin_atsheader.abyLFFilters));
    strncpy(this->bin_atsheader.achUTMZone, this->value("UTMZone").toString().toLatin1(), sizeof(this->bin_atsheader.achUTMZone));
    this->bin_atsheader.uiADUCalTimeDate =              quint32 (this->value("ADU_cal_time_date").toUInt());
    strncpy(this->bin_atsheader.achSensorCalFilename, this->value("sensor_cal_file").toString().toLatin1(), sizeof(this->bin_atsheader.achSensorCalFilename));
    this->bin_atsheader.uiSensorCalTimeDate =           quint32 (this->value("sensor_cal_date_time").toUInt());
    this->bin_atsheader.rPowerlineFreq1 =               this->value("powerline_freq1").toFloat();
    this->bin_atsheader.rPowerlineFreq2 =               this->value("powerline_freq2").toFloat();
    strncpy(this->bin_atsheader.abyHFFilters, this->value("HF_filters").toString().toLatin1(), sizeof(this->bin_atsheader.abyHFFilters));

    // check 64bit
    if (this->value("num_samples").toULongLong() > UINT32_MAX) {
        this->bin_atsheader.uiSamples64bit =                this->value("num_samples").toULongLong();
    }
    else {
        this->bin_atsheader.uiSamples64bit =             quint64 (0);
    }

    this->bin_atsheader.rExtGain =                      this->value("external_gain").toFloat();
    strncpy(this->bin_atsheader.achADBBoardType, this->value("ADB_board_type").toString().toLatin1(), sizeof(this->bin_atsheader.achADBBoardType));

    // comment block
    strncpy(this->bin_atsheader.tscComment.achClient, this->value("client").toString().toUtf8(), sizeof(this->bin_atsheader.tscComment.achClient));
    strncpy(this->bin_atsheader.tscComment.achContractor, this->value("contractor").toString().toUtf8(), sizeof(this->bin_atsheader.tscComment.achContractor));
    strncpy(this->bin_atsheader.tscComment.achArea, this->value("area").toString().toUtf8(), sizeof(this->bin_atsheader.tscComment.achArea));
    strncpy(this->bin_atsheader.tscComment.achSurveyID, this->value("survey_id").toString().toUtf8(), sizeof(this->bin_atsheader.tscComment.achSurveyID));
    strncpy(this->bin_atsheader.tscComment.achOperator, this->value("operator").toString().toUtf8(), sizeof(this->bin_atsheader.tscComment.achOperator));
    strncpy(this->bin_atsheader.tscComment.achSiteName, this->value("site_name").toString().toUtf8(), sizeof(this->bin_atsheader.tscComment.achSiteName));
    strncpy(this->bin_atsheader.tscComment.achXmlHeader, this->value("xml_header").toString().toUtf8(), sizeof(this->bin_atsheader.tscComment.achXmlHeader));
    strncpy(this->bin_atsheader.tscComment.achComments, this->value("comments").toString().toUtf8(), sizeof(this->bin_atsheader.tscComment.achComments));
    strncpy(this->bin_atsheader.tscComment.achSiteNameRR, this->value("site_name_rr").toString().toUtf8(), sizeof(this->bin_atsheader.tscComment.achSiteNameRR));
    strncpy(this->bin_atsheader.tscComment.achSiteNameEMAP, this->value("site_name_emap").toString().toUtf8(), sizeof(this->bin_atsheader.tscComment.achSiteNameEMAP));




    this->edt_start.set_date_time((this->qint64value("start_date_time") + this->qint64value("add_secs_to_start_time")), 0, 0, this->value("sample_freq").toDouble(), this->quint64value("num_samples"));
    this->edt_stop = this->edt_start.sample_time(value("num_samples").toULongLong());


    if (this->atswriter_section.size()) {
        QMap<QString, QVariant>::const_iterator m = this->cbegin();

        this->atswriter_section.insert("ats_data_file", this->fileName());

        for (m = this->constBegin(); m != this->constEnd(); ++m) {

            if (this->atswriter_section.contains(m.key())) {

                this->atswriter_section.insert(m.key(), m.value());
            }
        }
        if (this->atswriter_section.contains("start_time")) this->atswriter_section.insert("start_time", this->edt_start.toString("hh:mm:ss"));
        if (this->atswriter_section.contains("start_date")) this->atswriter_section.insert("start_date", this->edt_start.toString("yyyy-MM-dd"));
        if (this->atswriter_section.contains("ts_lsb")) this->atswriter_section.insert("ts_lsb", this->dvalue("lsb"));

        this->atswriter_section.insert("stop_date", this->edt_stop.toString("yyyy-MM-dd"));
        this->atswriter_section.insert("stop_time", this->edt_stop.toString("hh:mm:ss"));



    }

    this->get_new_filename(this->change_measdir);


}

void atsheader::classmembers_to_prc_com()
{

    const QChar gchar('G'), uchar('U');
    // some values will be used as doubles as we want to use them; converted back to float finally
    this->insert("header_length",         quint16 (this->bin_atsheader.uiHeaderLength));
    this->insert("header_version",        qint16 (this->bin_atsheader.siHeaderVers));
    // internally we use 64bit - conflict can come if write hugh files in case
    this->insert("num_samples",           quint64  (this->bin_atsheader.uiSamples));
    this->insert("sample_freq",           double (this->bin_atsheader.rSampleFreq));
    // make a 64bit and forget; Qt uses qint64 and milliseconds; check when use "1000" - we are using seconds!
    this->insert("start_date_time",       qint64  (this->bin_atsheader.uiStartDateTime));
    this->insert("lsb",                   double (this->bin_atsheader.dblLSBMV));
    this->insert("UTC_GMT_offset",        qint32 (this->bin_atsheader.iGMTOffset));
    this->insert("orig_sample_freq",      double(this->bin_atsheader.rOrigSampleFreq));
    this->insert("system_serial_number",  quint16(this->bin_atsheader.uiADUSerNum));
    this->insert("ADC_sernum",            quint16(this->bin_atsheader.uiADCSerNum));
    this->insert("channel_number",        quint8(this->bin_atsheader.uiChanNo));
    this->insert("chopper_status",        quint8(this->bin_atsheader.uiChopper));
    this->insert("channel_type",          this->clean_b_string(this->bin_atsheader.achChanType, sizeof(this->bin_atsheader.achChanType)));
    this->insert("sensor_type",           this->clean_b_string(this->bin_atsheader.achSensorType, sizeof(this->bin_atsheader.achSensorType)));
    this->insert("sensor_sernum",         quint16(this->bin_atsheader.siSensorSerNum));
    this->insert("pos_x1",                double(this->bin_atsheader.rPosX1));
    this->insert("pos_y1",                double(this->bin_atsheader.rPosY1));
    this->insert("pos_z1",                double(this->bin_atsheader.rPosZ1));
    this->insert("pos_x2",                double(this->bin_atsheader.rPosX2));
    this->insert("pos_y2",                double(this->bin_atsheader.rPosY2));
    this->insert("pos_z2",                double(this->bin_atsheader.rPosZ2));
    this->insert("dipole_length",         double(this->bin_atsheader.rDipLength));
    this->insert("dipole_angle",          double(this->bin_atsheader.rAngle));
    this->insert("probe_resistivity",     double(this->bin_atsheader.rProbeRes));
    this->insert("DC_offset",             double(this->bin_atsheader.rDCOffset));
    this->insert("gain_stage1",           double(this->bin_atsheader.rPreGain));
    this->insert("gain_stage2",           double(this->bin_atsheader.rPostGain));
    this->insert("latitude",              qint32(this->bin_atsheader.iLatitude));
    this->insert("longitude",             qint32(this->bin_atsheader.iLongitude));
    this->insert("elevation",             qint32(this->bin_atsheader.iElevation));
    this->insert("lat_long_type",         this->aby_to_QString(this->bin_atsheader.byLatLongType));
    this->insert("AddCoordType",          quint8(this->bin_atsheader.byAddCoordType));
    this->insert("GaussRefMeridian",      qint16(this->bin_atsheader.siRefMeridian));
    this->insert("northing",              double(this->bin_atsheader.dblNorthing));
    this->insert("easting",               double(this->bin_atsheader.dblEasting));
    this->insert("GPS_status",            this->aby_to_QString(this->bin_atsheader.byGPSStat));
    this->insert("GPS_accuracy",          quint8(this->bin_atsheader.byGPSAccuracy));
    this->insert("UTCOffset",             qint16(this->bin_atsheader.iUTCOffset));
    this->insert("system_type",           this->clean_b_string(this->bin_atsheader.achSystemType, sizeof(this->bin_atsheader.achSystemType)));
    this->insert("survey_header_name",    this->clean_b_stringUTF8(this->bin_atsheader.achSurveyHeaderName, sizeof(this->bin_atsheader.achSurveyHeaderName)));
    this->insert("meas_type",             this->clean_b_string(this->bin_atsheader.achMeasType, sizeof(this->bin_atsheader.achSurveyHeaderName)));
    this->insert("DC_offset_correction_value", double(this->bin_atsheader.DCOffsetCorrValue));
    this->insert("DC_offset_corr_on",     qint8 (this->bin_atsheader.DCOffsetCorrOn));
    this->insert("input_divder_on",       qint8 (this->bin_atsheader.InputDivOn));
    this->insert("bit_indicator",         qint16(this->bin_atsheader.bit_indicator));
    this->insert("selftest_result",       this->clean_b_string(this->bin_atsheader.achSelfTestResult, sizeof(this->bin_atsheader.achSelfTestResult)));
    this->insert("num_slices",            quint16(this->bin_atsheader.numslices));
    this->insert("cal_freqs",             qint16(this->bin_atsheader.siCalFreqs));
    this->insert("cal_entry_length",      qint16(this->bin_atsheader.siCalEntryLength));
    this->insert("cal_version",           qint16(this->bin_atsheader.siCalVersion));
    this->insert("cal_start_address",     qint16(this->bin_atsheader.siCalStartAddress));
    this->insert("LF_filters",            this->bin_atsheader.abyLFFilters);
    this->insert("UTMZone",               this->clean_b_string(this->bin_atsheader.achUTMZone, sizeof(this->bin_atsheader.achUTMZone)));
    this->insert("ADU_cal_time_date",     quint32(this->bin_atsheader.uiADUCalTimeDate));
    this->insert("sensor_cal_file",       this->clean_b_string(this->bin_atsheader.achSensorCalFilename, sizeof(this->bin_atsheader.achSensorCalFilename)));
    this->insert("sensor_cal_date_time",  quint32(this->bin_atsheader.uiSensorCalTimeDate));
    this->insert("powerline_freq1",       double (this->bin_atsheader.rPowerlineFreq1));
    this->insert("powerline_freq2",       double (this->bin_atsheader.rPowerlineFreq2));
    this->insert("HF_filters",            this->bin_atsheader.abyHFFilters);

    // num_samples_64bit is mapped to num_samples - and will be set during mapping class member
    if (this->bin_atsheader.uiSamples == UINT32_MAX) {
        this->insert("num_samples_64bit",     quint64 (this->bin_atsheader.uiSamples64bit));
        this->insert("num_samples",           quint64 (this->bin_atsheader.uiSamples64bit));
    }
    else {
        this->insert("num_samples_64bit",     quint64  (0));
    }
    this->insert("external_gain",         double (this->bin_atsheader.rExtGain));
    this->insert("ADB_board_type",        this->clean_b_string(this->bin_atsheader.achADBBoardType, sizeof(this->bin_atsheader.achADBBoardType)));

    // correct by sample frequency
    double tsf = 0;
    if (this->dvalue("rOrigSampleFreq") != 0.0) tsf = this->dvalue("rOrigSampleFreq");
    else {
        tsf = this->dvalue("sample_freq");
    }
    if (!this->svalue("ADB_board_type").size()) {
        if (tsf < 4096.1) {
            this->insert("ADB_board_type", "LF");
        }
        else {
            this->insert("ADB_board_type", "HF");

        }
    }

    this->insert("client",        this->clean_b_stringUTF8(this->bin_atsheader.tscComment.achClient, sizeof(this->bin_atsheader.tscComment.achClient)));
    this->insert("contractor",    this->clean_b_stringUTF8(this->bin_atsheader.tscComment.achContractor, sizeof(this->bin_atsheader.tscComment.achContractor)));
    this->insert("area",          this->clean_b_stringUTF8(this->bin_atsheader.tscComment.achArea, sizeof(this->bin_atsheader.tscComment.achArea)));
    this->insert("survey_id",     this->clean_b_stringUTF8(this->bin_atsheader.tscComment.achSurveyID, sizeof(this->bin_atsheader.tscComment.achSurveyID)));
    this->insert("operator",      this->clean_b_stringUTF8(this->bin_atsheader.tscComment.achOperator, sizeof(this->bin_atsheader.tscComment.achOperator)));



/*
    // make UTF-8 UTF8 out of it
    QByteArray utfhelp(this->bin_atsheader.tscComment.achSiteName, sizeof(this->bin_atsheader.tscComment.achSiteName));
    this->insert("site_name",             QString::fromUtf8(utfhelp).simplified());
    this->insert("xml_header",    this->clean_b_stringUTF8(this->bin_atsheader.tscComment.achXmlHeader, sizeof(this->bin_atsheader.tscComment.achXmlHeader)));
    QString comments(this->clean_b_stringUTF8(this->bin_atsheader.tscComment.achComments, sizeof(this->bin_atsheader.tscComment.achComments)));
    if (comments.startsWith('\n')) comments.remove(0,1);
    if (comments.endsWith('\n')) comments.remove(comments.size()-1,1);
    this->insert("comments",              comments);
    QByteArray utfhelprr(this->bin_atsheader.tscComment.achSiteNameRR, sizeof(this->bin_atsheader.tscComment.achSiteNameRR));
    this->insert("site_name_rr",             QString::fromUtf8(utfhelprr).simplified());
    QByteArray utfhelpem(this->bin_atsheader.tscComment.achSiteNameEMAP, sizeof(this->bin_atsheader.tscComment.achSiteNameEMAP));
    this->insert("site_name_emap",             QString::fromUtf8(utfhelpem).simplified());
*/
    this->insert("site_name", this->clean_b_stringUTF8(this->bin_atsheader.tscComment.achSiteName, sizeof(this->bin_atsheader.tscComment.achSiteName)));
    this->insert("xml_header",    this->clean_b_stringUTF8(this->bin_atsheader.tscComment.achXmlHeader, sizeof(this->bin_atsheader.tscComment.achXmlHeader)));
    QString comments(this->clean_b_stringUTF8(this->bin_atsheader.tscComment.achComments, sizeof(this->bin_atsheader.tscComment.achComments)));
    if (comments.startsWith('\n')) comments.remove(0,1);
    if (comments.endsWith('\n')) comments.remove(comments.size()-1,1);
    this->insert("comments",              comments);
    this->insert("site_name_rr", this->clean_b_stringUTF8(this->bin_atsheader.tscComment.achSiteNameRR, sizeof(this->bin_atsheader.tscComment.achSiteNameRR)));
    this->insert("site_name_emap", this->clean_b_stringUTF8(this->bin_atsheader.tscComment.achSiteNameEMAP, sizeof(this->bin_atsheader.tscComment.achSiteNameEMAP)));
    //
    this->insert("numslices", uint(0));

    //this->LFFilter = this->get_filter();


    bool is07 = false;
    bool is08 = false;
    if (this->svalue("system_type") == "ADU07") is07 = true;
    if (this->svalue("system_type") == "ADU08") is08 = true;



    //! filter banks setup contains following
    if (is08)        LFFilters.remove("LF-RF-3");
    if (is08)        LFFilters.remove("LF-RF-4");

    if (is08)        LFFilters.remove("MF_RF_1");
    if (is08)        LFFilters.remove("MF_RF_2");

    // HF Path
    // 1 Hz has been dropped for 08
    if (is08)        HFFilters.remove("HF-HP-1Hz");
    // 500Hz is the HP for 08
    if (is07)        HFFilters.remove("HF-HP-500Hz");



    // this->clean_hash_strings();
    edt_start.set_date_time(this->bin_atsheader.uiStartDateTime, 0, 0, double(this->bin_atsheader.rSampleFreq), this->bin_atsheader.uiSamples);
    edt_stop = edt_start.sample_time(this->bin_atsheader.uiSamples);
    this->insert("start_date",  edt_start.toString("yyyy-MM-dd"));
    this->insert("start_time",  edt_start.toString("hh:mm:ss"));
    this->insert("stop_date",  edt_stop.toString("yyyy-MM-dd"));
    this->insert("stop_time",  edt_stop.toString("hh:mm:ss"));
    this->insert("stop_date_time", edt_stop.sample_unix_time_stamp(0)); // add 0 samples to end

}

quint64 atsheader::get_num_samples() const
{
    return quint64value("num_samples");
}

void atsheader::set_num_samples(const quint64 new_samples)
{
    this->set_key_value("num_samples", new_samples);
    this->prc_com_to_classmembers();

}

QStringList atsheader::get_filter() const
{
    QStringList sfilter;
    QString system_type = this->svalue("system_type").simplified();

    if (!system_type.contains("ADU07") && !system_type.contains("ADU08")) {
        return sfilter;
    }


    QMap<QString, qint8>::const_iterator ithq8;

    QChar chfilter;
    quint8 ifilter;

    if ((this->svalue("ADB_board_type") == "LF") ||(this->svalue("ADB_board_type") == "MF")  ) {
        if (this->svalue("LF_filters").size() && (this->svalue("LF_filters") != "empty")) {
            chfilter = this->svalue("LF_filters").at(0);
            ifilter = quint8 (chfilter.toLatin1());
            if (ifilter > 16) { sfilter.append("LF_LP_4HZ");
                ifilter -= 16;
            }
            ithq8 = LFFilters.constBegin();
            while (ithq8 != LFFilters.constEnd()) {
                if (ithq8.value() & ifilter) {
                    sfilter.append(ithq8.key());
                }
                ++ithq8;
            }
        }
    }
    if (this->svalue("ADB_board_type") == "HF") {
        if (this->svalue("HF_filters").size() && (this->svalue("HF_filters") != "empty") ) {
            chfilter = this->svalue("HF_filters").at(0);
            ifilter =quint8 (chfilter.toLatin1());
            ithq8 = HFFilters.constBegin();
            while (ithq8 != HFFilters.constEnd()) {
                if (ithq8.value() & ifilter) {
                    sfilter.append(ithq8.key());
                }
                ++ithq8;
            }

        }
    }
    return sfilter;
}

void atsheader::set_filter(const QStringList &filterstrings)
{
    // can not set more than 2 filters in ADU07/08
    if (filterstrings.size() > 2) return;

    QStringList cpFilterstrings(filterstrings);
    QStringList allkeys;
    qint8 myfilter = 0;
    int hits = 0;
    bool is_lowpass = false;

    // check if we are LF
    if ((this->svalue("ADB_board_type") == "LF") ||(this->svalue("ADB_board_type") == "MF")  ) {

        allkeys = this->LFFilters.keys();

        for (auto &str : filterstrings) {
            if (allkeys.contains(str)) ++hits;
            if (str.contains("LF_LP_4HZ")) is_lowpass = true;
        }
        // we have at least one LF so go one
        if (hits) {
            if (is_lowpass) {
                myfilter = 16;
                cpFilterstrings.removeAll("LF_LP_4HZ");
            }
            QStringList last_filter;
            for (auto &str : cpFilterstrings) {
                if (allkeys.contains(str)) last_filter.append(str);
            }

            if (last_filter.size() != 1) return;  // we can not set two RF filters
            QMap<QString, qint8>::const_iterator ithq8 = LFFilters.constBegin();
            while (ithq8 != LFFilters.constEnd()) {
                if (ithq8.key() == last_filter.at(0)) {
                    myfilter += ithq8.value();
                }
                ++ithq8;
            }
            auto lftmp = this->value("LF_filters").toString().toLatin1();
            lftmp[0] = myfilter;
            this->insert("LF_filters", lftmp);
            this->prc_com_to_classmembers();
            return;
        }
        return;
    }

    else if (this->svalue("ADB_board_type") == "HF") {
        cpFilterstrings.clear();
        cpFilterstrings = filterstrings;
        allkeys = this->HFFilters.keys();
        for (auto &str : filterstrings) {
            if (allkeys.contains(str)) ++hits;
            if (str.contains("LF_LP_4HZ")) is_lowpass = true;
        }
        // remove possible missmatch
        if (is_lowpass) cpFilterstrings.removeAll("LF_LP_4HZ");
        QStringList last_filter;
        for (auto &str : cpFilterstrings) {
            if (allkeys.contains(str)) last_filter.append(str);
        }

        if (last_filter.size() != 1) return;  // we can not set two RF filters

        QMap<QString, qint8>::const_iterator ithq8 = HFFilters.constBegin();
        while (ithq8 != HFFilters.constEnd()) {
            if (ithq8.key() == last_filter.at(0)) {
                myfilter = ithq8.value();
            }
            ++ithq8;
        }
        auto lftmp = this->value("HF_filters").toString().toLatin1();
        lftmp[0] = myfilter;
        this->insert("HF_filters", lftmp);
        this->prc_com_to_classmembers();

    }
    return;

}


bool atsheader::activate_scalable()
{
    QStringList scalable;
    scalable << "Ex" << "Ey" << "Ez";
    int i = 0;
    for (auto &sc : scalable ) {
        if (!QString::compare(sc, this->value("channel_type").toString(),Qt::CaseInsensitive)) {
            ++i;
        }
    }


    this->dipole_length = this->get_dipole_length();
    if (i && (this->dipole_length > 0.0001)) {
        this->is_scalable = true;
    }
    else {
        this->is_scalable = false;
    }

    return this->is_scalable;
}




QString atsheader::clean_b_string(const char *ins, const int s_size, const bool remove_terminator) const {

    QString rstring = QString::fromLatin1(ins, s_size);
    QChar bend('\x0');
    int fpos = rstring.indexOf(bend);
    if (fpos == 0) {        // string starts with NULL terminator
        return QString();
    }
    if (fpos > 0)rstring.truncate(fpos);
    rstring.remove(bend, Qt::CaseInsensitive);
    if (remove_terminator) {
        rstring.remove(QChar('\x0'), Qt::CaseInsensitive);
    }
    return rstring;
}
QString atsheader::clean_b_stringUTF8(const char *ins, const int s_size, const bool remove_terminator) const
{
    QString rstring = QString::fromUtf8(ins, s_size);
    QChar bend('\x0');
    int fpos = rstring.indexOf(bend);
    if (fpos == 0) {        // string starts with NULL terminator
        return QString();
    }
    if (fpos > 0)rstring.truncate(fpos);
    if (remove_terminator) {
        rstring.remove(bend, Qt::CaseInsensitive);
    }
    QChar invalid = QChar::ReplacementCharacter;

    while (rstring.size()) {
        if (rstring.endsWith(invalid) && rstring.size()) rstring.remove(rstring.size()-1, 1);
        else break;
    }

    return rstring;
}


bool compsamples_ats_lhs_lt(const atsheader &lhs, const atsheader &rhs)
{
    return ( lhs.get_num_samples() < rhs.get_num_samples());
}

bool compsamples_ats_lhs_lt_sp(const std::shared_ptr<atsheader> &lhs, const std::shared_ptr<atsheader> &rhs)
{
    return ( lhs->get_num_samples() < rhs->get_num_samples());
}

bool compstartime_ats_lhs_lt(const atsheader &lhs, const atsheader &rhs)
{
    return ( lhs.qint64value("start_date_time") < rhs.qint64value("start_date_time"));
}

bool compstartime_ats_lhs_lt_sp(const std::shared_ptr<atsheader> &lhs, const std::shared_ptr<atsheader> &rhs)
{
    return ( lhs->qint64value("start_date_time") < rhs->qint64value("start_date_time"));
}

bool compstoptime_ats_lhs_lt(const atsheader &lhs, const atsheader &rhs)
{
    return ( lhs.qint64value("stop_date_time") < rhs.qint64value("stop_date_time"));

}

bool compstoptime_ats_lhs_lt_sp(const std::shared_ptr<atsheader>& lhs, const std::shared_ptr<atsheader>& rhs)
{
    return ( lhs->qint64value("stop_date_time") < rhs->qint64value("stop_date_time") );

}

int64_t delta_stop_start(const atsheader &lhs, const atsheader &rhs)
{
    return ( rhs.qint64value("start_date_time") - lhs.qint64value("stop_date_time"));
}

int64_t delta_stop_start_sp(const std::shared_ptr<atsheader>& lhs, const std::shared_ptr<atsheader>& rhs)
{
    return ( rhs->qint64value("start_date_time") - lhs->qint64value("stop_date_time"));
}

bool same_recording(const atsheader &lhs, const atsheader &rhs)
{
    bool same = true;
    if (rhs.qint64value("start_date_time") != lhs.qint64value("start_date_time")) same = false;
    if (rhs.get_sample_freq() != lhs.get_sample_freq()) same = false;
    if (lhs.get_num_samples() != rhs.get_num_samples()) same = false;

    return same;
}

bool same_recording(const std::shared_ptr<atsheader> &lhs, const std::shared_ptr<atsheader> &rhs)
{
    bool same = true;
    if (rhs->qint64value("start_date_time") != lhs->qint64value("start_date_time")) same = false;
    if (rhs->get_sample_freq() != lhs->get_sample_freq()) same = false;
    if (lhs->get_num_samples() != rhs->get_num_samples()) same = false;

    return same;
}
