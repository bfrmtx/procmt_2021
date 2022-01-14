#include "phoenix_json.h"

Phoenix_json::Phoenix_json(QObject *parent) : QObject(parent)

{
    QFileInfo info_db_file("info.sql3");
    this->info_db = fall_back_default(info_db_file);

}

bool Phoenix_json::from_JSON_scal(const QFileInfo *qfi)
{
    if (qfi != nullptr) {
        if (!qfi->exists()) return false;
        QFile file_obj(qfi->absoluteFilePath());
        if(!file_obj.open(QIODevice::ReadOnly)){
            qDebug() << Q_FUNC_INFO << "Failed to open " << qfi->absoluteFilePath();
            return false;
        }
        this->myfile = *qfi;

        QByteArray json_bytes(file_obj.readAll());
        file_obj.close();

        auto json_doc = QJsonDocument::fromJson(json_bytes);

        if(json_doc.isNull()){
            this->phoenix_tags.insert("fatal", "Failed to create JSON doc.");
            qDebug() << Q_FUNC_INFO  << "Failed to create JSON doc.";
            return false;
        }
        if(!json_doc.isObject()){
            this->phoenix_tags.insert("fatal", "JSON is not an object.");
            qDebug() << Q_FUNC_INFO  << "JSON is not an object.";
            return false;
        }

        this->what = 2; // coil
        auto jobjects = json_doc.object();
        iterate_json(jobjects);

        // now check the consistancy
        uint sz = this->phoenix_tags.value("num_records").toUInt();
        uint nr = this->phoenix_tags.value("num_of_responses").toUInt();

        if (this->freqs_coil.size() != nr) {
            this->phoenix_tags.insert("fatal", "no frequencies loaded");

            qDebug() << Q_FUNC_INFO  << "no frequencies loaded";
            return false;

        }
        if (this->magnitude_coil.size() != nr) {
            this->phoenix_tags.insert("fatal", "no amplitudes loaded");
            qDebug() << Q_FUNC_INFO  << "no amplitudes loaded";
            return false;

        }
        if (this->phs_deg_coil.size() != nr) {
            this->phoenix_tags.insert("fatal", "no phases loaded");
            qDebug() << Q_FUNC_INFO  << "no phases loaded";
            return false;

        }




        for (size_t i = 0; i < this->freqs_coil.size(); ++i) {

            auto minmax = std::minmax_element(this->freqs_coil[i].begin(), this->freqs_coil[i].end());
            if ((*minmax.first < 1E-10) || (*minmax.second > 1E10)) {
                this->phoenix_tags.insert("fatal", "frequencies out of range");
                qDebug() << Q_FUNC_INFO  << "frequencies out of range";
                return false;
            }
            else {
                for (size_t j = 0; j < this->freqs_coil[i].size(); ++j) {
                    this->magnitude_coil[i][j] /= this->freqs_coil[i][j];
                    //this->magnitude_coil[i][j] /= 1000.;
                }
            }

            this->calfiles.emplace_back(std::make_shared<calibration>(QFileInfo(this->info_db.absoluteFilePath()), 0, -1, true, "", Q_NULLPTR));
            eQDateTime edt(this->phoenix_tags.value("date_time").toLongLong(), 0);
            this->calfiles.back()->create_type_and_date("MTC-X-" + QString::number(i), this->phoenix_tags.value("sensor_serial").toInt(0), edt);
            this->calfiles.back()->set_f_a_p_fromVector_chopper_on(this->freqs_coil.at(i), this->magnitude_coil.at(i), this->phs_deg_coil.at(i));
            this->calfiles.back()->set_f_a_p_fromVector_chopper_off(this->freqs_coil.at(i), this->magnitude_coil.at(i), this->phs_deg_coil.at(i));
        }

        qDebug() << "coil cal loaded";
        return true;

    }

    return false;

}

bool Phoenix_json::from_JSON_rxcal(const QFileInfo *qfi)
{
    if (qfi != nullptr) {
        if (!qfi->exists()) return false;
        QFile file_obj(qfi->absoluteFilePath());
        if(!file_obj.open(QIODevice::ReadOnly)){
            qDebug() << Q_FUNC_INFO << "system:  Failed to open " << qfi->absoluteFilePath();
            return false;
        }
        this->myfile = *qfi;

        QByteArray json_bytes(file_obj.readAll());
        file_obj.close();

        auto json_doc = QJsonDocument::fromJson(json_bytes);

        if(json_doc.isNull()){
            this->phoenix_tags.insert("fatal", "system: Failed to create JSON doc.");
            qDebug() << Q_FUNC_INFO  << "Failed to create JSON doc.";
            return false;
        }
        if(!json_doc.isObject()){
            this->phoenix_tags.insert("fatal", "system: JSON is not an object.");
            qDebug() << Q_FUNC_INFO  << "JSON is not an object.";
            return false;
        }

        this->what = 3; // system
        auto jobjects = json_doc.object();
        iterate_json(jobjects);

        // now check the consistancy
        uint sz = this->phoenix_tags.value("num_records").toUInt();
        uint nr = this->phoenix_tags.value("num_of_responses").toUInt();

        //        if (this->freqs_system.size() != nr) {
        //            this->phoenix_tags.insert("fatal", "no frequencies loaded");

        //            qDebug() << Q_FUNC_INFO  << "no frequencies loaded";
        //            return false;

        //        }
        //        if (this->magnitude_system.size() != nr) {
        //            this->phoenix_tags.insert("fatal", "no amplitudes loaded");
        //            qDebug() << Q_FUNC_INFO  << "no amplitudes loaded";
        //            return false;

        //        }
        //        if (this->phs_deg_system.size() != nr) {
        //            this->phoenix_tags.insert("fatal", "no phases loaded");
        //            qDebug() << Q_FUNC_INFO  << "no phases loaded";
        //            return false;

        //        }



        for (size_t ich = 0; ich < this->freqs_system.size(); ++ich) {
            for (size_t i = 0; i < this->freqs_system.at(ich).size(); ++i) {

                auto minmax = std::minmax_element(this->freqs_system[ich][i].cbegin(), this->freqs_system[ich][i].cend());
                if ((*minmax.first < 1E-10) || (*minmax.second > 1E10)) {
                    this->phoenix_tags.insert("fatal", " system frequencies out of range");
                    qDebug() << Q_FUNC_INFO  << "frequencies out of range";
                    return false;
                }
//                else {
//                    for (size_t j = 0; j < this->freqs_system[ich][i].size(); ++j) {
//                        this->magnitude_system[ich][i][j] /= this->freqs_system[ich][i][j];
//                        //this->magnitude_coil[ich][i][j] /= 1000.;
//                    }
//                }

                this->calfiles.emplace_back(std::make_shared<calibration>(QFileInfo(this->info_db.absoluteFilePath()), 0, -1, true, "", Q_NULLPTR));
                eQDateTime edt(this->phoenix_tags.value("date_time").toLongLong(), 0);
                this->calfiles.back()->create_type_and_date("Phoenix_system_" + QString::number(ich) + "_" + QString::number(i), this->phoenix_tags.value("system_serial").toInt(0), edt);
                this->calfiles.back()->set_f_a_p_fromVector_chopper_on(this->freqs_system.at(ich).at(i), this->magnitude_system.at(ich).at(i), this->phs_deg_system.at(ich).at(i));
                this->calfiles.back()->set_f_a_p_fromVector_chopper_off(this->freqs_system.at(ich).at(i), this->magnitude_system.at(ich).at(i), this->phs_deg_system.at(ich).at(i));
            }
        }

        qDebug() << " system cal loaded";
        return true;
    }

    return false;

}

bool Phoenix_json::ts_header_reader(const QFileInfo &qfi, int &nerrors)
{
    if (!qfi.exists()) return false;
    this->nerrors = 0;

    QFile textfile(qfi.absoluteFilePath(), this);
    textfile.setFileName(qfi.absoluteFilePath());

    textfile.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!textfile.isOpen()) {
        this->phoenix_tags.insert("fatal", "can not open" + qfi.absoluteFilePath() );
        return false;
    }
    size_t i = 0;
    QString line;
    line.reserve(2000);
    QTextStream in(&textfile);
    QByteArray json_bytes;
    QString start_time;  // JSON has no long int
    QString stop_time; // JSON has no long int
    bool ok = false;
    do {
        line = in.readLine();
        line = line.trimmed();
        if (line.startsWith("\"start_time")) start_time = line.simplified();
        if (line.startsWith("\"stop_time")) stop_time = line.simplified();

        if (!line.startsWith("\"data\": [")) json_bytes.append(line.toUtf8());
        if (i > 79) {
            textfile.close();
            this->phoenix_tags.insert("fatal", "can not fimd headers end at line < 79");
            return false;
        }
        ++i;
    } while (!in.atEnd() && !line.startsWith("\"data\": [") && (i < 80));

    textfile.close();

    if (json_bytes.endsWith(",")) json_bytes.remove(json_bytes.size()-1, 1);
    json_bytes.append("}");

    auto json_doc = QJsonDocument::fromJson(json_bytes);

    if(json_doc.isNull()){
        qDebug() << Q_FUNC_INFO  << "Failed to read JSON doc.";
        this->phoenix_tags.insert("fatal", "Failed to create JSON doc.");
        return false;
    }
    if(!json_doc.isObject()){
        this->phoenix_tags.insert("fatal", "JSON is not an object.");
        qDebug() << Q_FUNC_INFO  << "JSON is not an object.";
        return false;
    }
    start_time.remove(QChar('"'), Qt::CaseInsensitive);
    start_time.remove(QChar(':'), Qt::CaseInsensitive);
    start_time.remove(QChar(','), Qt::CaseInsensitive);
    start_time.remove(QString("start_time"), Qt::CaseInsensitive);
    start_time = start_time.trimmed();


    stop_time.remove(QChar('"'), Qt::CaseInsensitive);
    stop_time.remove(QChar(':'), Qt::CaseInsensitive);
    stop_time.remove(QChar(','), Qt::CaseInsensitive);
    stop_time.remove(QString("stop_time"), Qt::CaseInsensitive);
    stop_time = stop_time.trimmed();


    qint64 t = start_time.toLongLong(&ok);
    if (ok) this->phoenix_tags.insert("start_date_time", t);
    else ++this->nerrors;

    t = stop_time.toLongLong(&ok);
    if (ok) this->phoenix_tags.insert("stop_date_time", t);
    else ++this->nerrors;

    this->what = 01; // ts data
    auto jobjects = json_doc.object();
    iterate_json(jobjects);

    nerrors = this->nerrors;

    if (!this->phoenix_tags.contains("start_date_time")) {
        this->phoenix_tags.insert("start_date_time", 0);
    }

    if (this->phoenix_tags.contains("sample_freq")) {
        if (this->phoenix_tags.value("sample_freq").toInt() == 0) return false;
        this->phoenix_tags.insert("board", "LF");
        if (this->phoenix_tags.value("sample_freq").toInt() > 4097) {
            this->phoenix_tags.insert("board", "HF");
        }

        return true;

    }

    return false;

}

bool Phoenix_json::ts_block_reader(const QFileInfo &qfi, std::vector<std::shared_ptr<atsfile> > &atsfiles, const QMap<size_t, QString> &chan_str_mtx, const QMap<size_t, QString> &chan_str_phoenix)
{
    if (!atsfiles.size()) return false;
    if (!qfi.exists()) return false;

    QFile textfile(qfi.absoluteFilePath(), this);
    textfile.setFileName(qfi.absoluteFilePath());

    textfile.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!textfile.isOpen()) return false;

    this->chan_str_mtx = chan_str_mtx;
    this->chan_str_phoenix = chan_str_phoenix;

    //    for (auto &ats : atsfiles) {
    //        this->atsfiles.emplace_back(ats);
    //    }



    size_t i = 0;
    size_t l = 0;
    QString line;
    line.reserve(200000);
    QTextStream in(&textfile);
    do {
        line = in.readLine();
        line = line.trimmed();
        if (i > 79) {
            textfile.close();
            this->phoenix_results.insert("header", "can not finish reading header");
            return false;
        }
        ++i;
        ++l;
    } while (!in.atEnd() && !line.startsWith("\"data\": [") && (i < 80));

    for (auto &atsf : atsfiles) {
        this->async_runs.emplace_back(std::future<bool>());
        this->async_has_lines.emplace_back(0);
    }

    line.clear();
    // maybe with async ?
    size_t j;


    do {
        line = in.readLine();
        line = line.trimmed();
        j = 0;
        for (auto &atsf : atsfiles) {
            if (line.startsWith(atsf->phoenix_json_ts_tag()) && line.endsWith("],")) {
                if (this->async_has_lines[j]) {
                    this->async_runs[j].get();
                }
                this->async_runs[j] = std::async(&atsfile::csvline, atsf.get(), line.trimmed());
                ++this->async_has_lines[j];
            }

            ++j;
        }
        qDebug() << l++;

        if (l % 500 == 0) {
            emit this->signal_lines_processed(QString::number(qint64(l)));
        }

    }  while (!in.atEnd());

    this->phoenix_results.insert("total lines", QString::number(qint64(l)));

    for (auto &asyncs : async_runs) {
        asyncs.get();
    }


    //    do {
    //        line = in.readLine();
    //        line = line.trimmed();
    //        for (auto &atsf : atsfiles) {
    //            atsf->csvline(line);
    //        }
    //        qDebug() << l++;

    //        if (l % 500 == 0) {
    //            emit this->signal_lines_processed(QString::number(l));
    //        }

    //    }  while (!in.atEnd());


    i = 0; j = 0;
    textfile.close();

    for (auto &atsf : atsfiles) {
        atsf->write_header(false);
    }


    for (auto &atsf : atsfiles) {
        this->async_runs_lsb.emplace_back( std::async(&atsfile::queue_data_to_vector, atsf.get()));
    }
    for (auto &asyncs : async_runs_lsb) {
        asyncs.get();
    }


    for (auto &atsf : atsfiles) {
        this->async_runs_file_write.emplace_back( std::async(&atsfile::mytsdata_tofile, atsf.get()));
    }
    for (auto &asyncs : async_runs_file_write) {
        asyncs.get();
    }


    for (auto &atsf : atsfiles) {
        atsf->write_header(true);
        atsf->classmembers_to_prc_com(); // as we do read it --- add some strings to prc_com which generated while reading only

        this->phoenix_results.insert((atsf->channel_type() + " lines:"), QVariant(qint64(this->async_has_lines[j])));
        this->phoenix_results.insert((atsf->channel_type() + " samples:"), QVariant(atsf->size()));

        qDebug() << "channel thread finished" << i  << "read: " << atsf->get_samples_read() << "write: " << atsf->get_samples_read();
    }

    //    for (auto &atsf : atsfiles) {
    //        atsf->write_header(false);
    //        atsf->queue_data_to_vector();
    //        atsf->mytsdata_tofile();
    //        atsf->write_header(true);
    //        atsf->classmembers_to_prc_com(); // as we do read it --- add some strings to prc_com which generated while reading only

    //        this->phoenix_results.insert((atsf->channel_type() + " lines:"), QVariant(qint64(this->async_has_lines[j])));
    //        this->phoenix_results.insert((atsf->channel_type() + " samples:"), QVariant(atsf->size()));

    //        qDebug() << "channel thread finished" << i  << "read: " << atsf->get_samples_read() << "write: " << atsf->get_samples_read();
    //    }


    return true;
}


int Phoenix_json::save_calfiles()
{
    int i = 0;
    for (auto &cal : this->calfiles) {

        if (what == 2) {  // sensor that should be a single file hopefuly
            QString str;
            str = (cal->sensortype + QString::number(cal->sernum) + ".txt");
            // else str = (cal->sensortype + "-" + QString::number(cal->sernum) + "_" + QString::number(i) + ".txt");
            QFileInfo qfi(this->myfile.absolutePath() + "/" + str);
            cal->write_std_txt_file(qfi);
        }
        if (what == 3) {   // system
            QString str(cal->sensortype + "-" + QString::number(cal->sernum) + "_" + QString::number(i) + ".txt");
            QFileInfo qfi(this->myfile.absolutePath() + "/" + str);
            cal->write_std_txt_file(qfi);
        }

         ++i;
    }

    return i;
}




void Phoenix_json::iterate_json(const QJsonObject &qjd)
{
    if (!this->what) return;
    QJsonObject::const_iterator first =  qjd.constBegin();
    QJsonObject::const_iterator last =  qjd.constEnd();

    while (first != last) {
        //qDebug() << first.key();
        // simple tags first
        if (!first.value().isObject()) {
            qDebug() << first.key();
            if (what == 1) { // ts

                this->special_values_system_ts(first);
            }


            if (what == 2) { // coil
                if (first.key() == "freq_Hz") {
                    this->freqs_coil.emplace_back(std::vector<double>());
                    this->freqs_coil.back().reserve(200);
                }
                else if (first.key() == "magnitude") {
                    this->magnitude_coil.emplace_back(std::vector<double>());
                    this->magnitude_coil.back().reserve(200);
                }

                else if (first.key() == "phs_deg") {
                    this->phs_deg_coil.emplace_back(std::vector<double>());
                    this->phs_deg_coil.back().reserve(200);
                }

                else this->special_values_coils(first);

            }
            if (what == 3) { // system

                if (first.key() == "freq_Hz") {
                    this->freqs_system.emplace_back(std::vector<std::vector<double>>());
                    this->freqs_system.back().emplace_back(std::vector<double>());
                    this->freqs_system.back().back().reserve(200);
                }
                if (first.key() == "magnitude") {
                    this->magnitude_system.emplace_back(std::vector<std::vector<double>>());
                    this->magnitude_system.back().emplace_back(std::vector<double>());
                    this->magnitude_system.back().back().reserve(200);
                }
                if (first.key() == "phs_deg") {
                    this->phs_deg_system.emplace_back(std::vector<std::vector<double>>());
                    this->phs_deg_system.back().emplace_back(std::vector<double>());
                    this->phs_deg_system.back().back().reserve(200);
                }
                else {
                    this->special_values_system(first);
                }
            }


        }

        if(first.value().isObject()) {
            this->iterate_json(first.value().toObject());
        }
        if (first.value().isArray()) {
            auto jarr = first.value().toArray();
            for (int i = 0; i < jarr.size(); ++i) {
                QJsonValue val = jarr.at(i);
                if (val.isObject()) {
                    this->iterate_json(val.toObject());
                }
                else {
                    if (what == 2) {
                        // freq_Hz emplace a new vector above and use it here
                        if (first.key() == "freq_Hz") this->freqs_coil.back().push_back(val.toDouble(-1));
                        if (first.key() == "magnitude") this->magnitude_coil.back().push_back(val.toDouble(-1));
                        if (first.key() == "phs_deg") this->phs_deg_coil.back().push_back(val.toDouble(-1));
                    }
                    if (what == 3) {
                        if (first.key() == "freq_Hz") this->freqs_system.back().back().push_back(val.toDouble(-1));
                        if (first.key() == "magnitude") this->magnitude_system.back().back().push_back(val.toDouble(-1));
                        if (first.key() == "phs_deg") this->phs_deg_system.back().back().push_back(val.toDouble(-1));
                    }



                }

            }
        }

        ++first;
    }
}


void Phoenix_json::slot_sensortype_changed(const QString &sensortype)
{
    if ((what == 2) && this->freqs_coil.size()) {
        qDebug() << "sensor changed: " << sensortype;
        if (this->calfiles.size()) {
            for (auto &cal : calfiles) {
                cal->sensortype = sensortype;
                emit this->signal_sensor_changed(cal->sensortype, cal->sernum);
            }
        }
    }
}

void Phoenix_json::special_values_coils(QJsonObject::const_iterator &first)
{

    if (first.value().isObject()) return;
    // first two are tags for subs
    if (!first.key().compare("chan_data", Qt::CaseInsensitive)) {
        return;
    }
    else if (!first.key().compare("cal_data", Qt::CaseInsensitive)) {
        return;
    }

    else if (!first.key().compare("manufacturer", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("manufacturer", QVariant(first.value().toString()));
    }
    else if (!first.key().compare("file_type", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("file_type", QVariant(first.value().toString()));
    }


    else if (!first.key().compare("file_type", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("file_type", QVariant(first.value().toString()));
    }
    else if (!first.key().compare("file_version", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("file_version", QVariant(first.value().toString()));
    }
    else if (!first.key().compare("empower_version", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("empower_version", QVariant(first.value().toString()));
    }
    else if (!first.key().compare("sensor_serial", Qt::CaseInsensitive)) {
        bool myok = false;
        QString QSser(first.value().toString());
        uint ser =  QSser.toUInt(&myok);
        if (myok) this->phoenix_tags.insert("sensor_serial", QVariant(ser));
        else this->phoenix_tags.insert("sensor_serial", 1);
    }
    else if (!first.key().compare("altitude", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("elevation", QVariant(first.value().toDouble()));
    }
    else if (!first.key().compare("latitude", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("latitude", QVariant(first.value().toDouble()));
    }
    else if (!first.key().compare("longitude", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("longitude", QVariant(first.value().toDouble()));
    }
    else if (!first.key().compare("timestamp_utc", Qt::CaseInsensitive)) {
        double ts = first.value().toDouble();
        qint64 time(ts);
        this->phoenix_tags.insert("date_time", time);
    }
    else if (!first.key().compare("num_of_responses", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("num_of_responses", first.value().toInt(0));
    }

    else if (!first.key().compare("num_channels", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("num_channels", first.value().toInt(0));
    }

    else {
        this->phoenix_tags.insert(first.key(), first.value().toVariant());
    }
}

void Phoenix_json::special_values_system_ts(QJsonObject::const_iterator &first)
{
    bool ok = false;
    if (!first.key().compare("E1", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("Ex_dipole_length_m", QVariant(first.value().toDouble()));
    }
    else if (!first.key().compare("E2", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("Ey_dipole_length_m", QVariant(first.value().toDouble()));
    }

    else if (!first.key().compare("E1", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("Ex_serial", QVariant(first.value().toString()).toInt(&ok));
        if (!ok) ++nerrors;
    }
    else if (!first.key().compare("E2", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("Ey_serial", QVariant(first.value().toString()).toInt(&ok));
        if (!ok) ++nerrors;
    }
    else if (!first.key().compare("H1", Qt::CaseInsensitive)) {
        qDebug()  << first.key() << first.value() ;
        this->phoenix_tags.insert("Hx_serial", QVariant(first.value().toString()).toInt(&ok));
        if (!ok) ++nerrors;
    }
    else if (!first.key().compare("H2", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("Hy_serial", QVariant(first.value().toString()).toInt(&ok));
        if (!ok) ++nerrors;
    }
    else if (!first.key().compare("H3", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("Hz_serial", QVariant(first.value().toString()).toInt(&ok));
        if (!ok) ++nerrors;
    }
    else if (!first.key().compare("manufacturer", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("manufacturer", QVariant(first.value().toString()));
    }
    else if (!first.key().compare("file_type", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("file_type", QVariant(first.value().toString()));
    }
    else if (!first.key().compare("file_version", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("file_version", QVariant(first.value().toString()));
    }
    else if (!first.key().compare("empower_version", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("empower_version", QVariant(first.value().toString()));
    }
    else if (!first.key().compare("recording_id", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("recording_id", QVariant(first.value().toString()));
        QStringList try_serial = this->phoenix_tags.value("recording_id").toString().split("_");
        if (try_serial.size() > 1) {
            uint ser = try_serial.at(0).toUInt(&ok);
            if (ok) this->phoenix_tags.insert("system_serial_number", QVariant(ser));
            else this->phoenix_tags.insert("system_serial_number", 1);
            if (!ok) ++nerrors;
        }
        else this->phoenix_tags.insert("system_serial_number", 1);


    }
    else if (!first.key().compare("instrument_type", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("instrument_type", QVariant(first.value().toString()));
    }
    else if (!first.key().compare("coords", Qt::CaseInsensitive)) {
        QStringList vals(first.value().toString().split(","));
        if (vals.size() == 2) {
            for (auto &val : vals) val = val.simplified();

            this->phoenix_tags.insert("latitude", QVariant(vals.at(0).toDouble(&ok)));
            if (!ok) ++nerrors;

            this->phoenix_tags.insert("longitude", QVariant(vals.at(1).toDouble(&ok)));
            if (!ok) ++nerrors;

        }
    }
    else if (!first.key().compare("data_units", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("data_units", QVariant(first.value().toString()));
    }
    else if (!first.key().compare("sampling_freq", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("sample_freq", QVariant(first.value().toDouble(0)));
    }

    else {
        this->phoenix_tags.insert(first.key(), QVariant(first.value().toString()));
    }


}

void Phoenix_json::special_values_system(QJsonObject::const_iterator &first)
{

    if (first.value().isObject()) return;
    // first two are tags for subs
    if (!first.key().compare("chan_data", Qt::CaseInsensitive)) {
        return;
    }
    else if (!first.key().compare("cal_data", Qt::CaseInsensitive)) {
        return;
    }

    else if (!first.key().compare("inst_serial", Qt::CaseInsensitive)) {
        bool myok = false;
        QString QSser(first.value().toString());
        uint ser =  QSser.toUInt(&myok);
        if (myok) this->phoenix_tags.insert("system_serial_number", QVariant(ser));
        else this->phoenix_tags.insert("system_serial_number", 1);
    }
    else if (!first.key().compare("instrument_type", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("instrument_type", QVariant(first.value().toString()));
    }
    else if (!first.key().compare("instrument_model", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("instrument_model", QVariant(first.value().toString()));
    }
    else if (!first.key().compare("manufacturer", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("manufacturer", QVariant(first.value().toString()));
    }
    else if (!first.key().compare("file_type", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("file_type", QVariant(first.value().toString()));
    }
    else if (!first.key().compare("file_version", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("file_version", QVariant(first.value().toString()));
    }
    else if (!first.key().compare("empower_version", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("empower_version", QVariant(first.value().toString()));
    }
    else if (!first.key().compare("num_channels", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("num_channels", first.value().toInt(0));
    }
    else if (!first.key().compare("altitude", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("elevation", QVariant(first.value().toDouble()));
    }
    else if (!first.key().compare("latitude", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("latitude", QVariant(first.value().toDouble()));
    }
    else if (!first.key().compare("longitude", Qt::CaseInsensitive)) {
        this->phoenix_tags.insert("longitude", QVariant(first.value().toDouble()));
    }
    else if (!first.key().compare("timestamp_utc", Qt::CaseInsensitive)) {
        double ts = first.value().toDouble();
        qint64 time(ts);
        this->phoenix_tags.insert("date_time", time);
    }

    else {
        this->phoenix_tags.insert(first.key(), first.value().toVariant());
    }

    if (!first.key().compare("tag", Qt::CaseInsensitive)) {
        QString tag = first.value().toString();

        if (!tag.compare("E1", Qt::CaseInsensitive)) {
            this->active_channel_str.append("Ex");
        }
        else if (!tag.compare("E2", Qt::CaseInsensitive)) {
            this->active_channel_str.append("Ey");
        }
        else if (!tag.compare("H1", Qt::CaseInsensitive)) {
            this->active_channel_str.append("Hx");
        }
        else if (!tag.compare("H2", Qt::CaseInsensitive)) {
            this->active_channel_str.append("Hy");
        }
        else if (!tag.compare("H3", Qt::CaseInsensitive)) {
            this->active_channel_str.append("Hz");
        }
    }


}

