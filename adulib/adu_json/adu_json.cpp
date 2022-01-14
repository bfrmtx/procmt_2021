#include "adu_json.h"

adu_json::adu_json(QObject *parent) : QObject(parent) {

}

adu_json::adu_json(const adu_json &rhs) :  QObject(nullptr)
{
    this->operator =(rhs);

}

adu_json &adu_json::operator =(const adu_json &rhs)
{
    if(&rhs == this) return *this;

    this->channels = rhs.channels;
    this->system = rhs.system;
    this->sensors = rhs.sensors;


    this->duration = rhs.duration;

    // calculate stop time?
    return *this;
}

adu_json::~adu_json()
{

}

bool adu_json::init_channels(const uint &chans)
{

    if (chans < 1) {
        qDebug()  << Q_FUNC_INFO  << "no channels";
        return false;
    }

    // protect against overflow
    if (chans > adu_defs::max_channels) {
        qDebug()  << Q_FUNC_INFO  << "too many channels?" << chans;
        return false;
    }

    this->clear();

    uint i = 0;
    for (i = 0; i < chans; ++i) this->channels.emplace_back(QVariantMap());
    for (i = 0; i < chans; ++i) this->sensors.emplace_back(QVariantMap());

    i = 0;
    for (auto &chan : this->channels) {
        chan.insert("id", i++);
    }
    i = 0;
    for (auto &chan : this->sensors) {
        chan.insert("id", i++);
    }




    QFileInfo info_db_file(app_homepath("info.sql3"));
    QString keys, labels, value_types, xmlnodes, editables;
    QVariant values;
    if (info_db_file.exists()) {
        QSqlDatabase info_db = QSqlDatabase::addDatabase("QSQLITE", "populate");
        info_db.setConnectOptions("QSQLITE_OPEN_READONLY");
        info_db.setDatabaseName(info_db_file.absoluteFilePath());

        if (info_db.open() ) {
            QSqlQuery xquery(info_db);


            QString querystr("SELECT xml_adu.key, xml_adu.value, xml_adu.label, xml_adu.value_type, xml_adu.xmlnode, xml_adu.editable FROM xml_adu;");

            if (xquery.exec(querystr)) {
                while (xquery.next()) {
                    keys = (xquery.value(0).toString());
                    values = (xquery.value(1));
                    labels = (xquery.value(2).toString());
                    value_types = (xquery.value(3).toString());
                    xmlnodes = (xquery.value(4).toString());
                    editables = (xquery.value(5).toString());
                    if (!value_types.compare("int")) this->system.insert(keys, values.toInt());
                    if (!value_types.compare("double")) this->system.insert(keys, values.toDouble());
                    if (!value_types.compare("string")) this->system.insert(keys, values.toString());
                    if (!value_types.compare("ullong")) this->system.insert(keys, values.toULongLong());
                    if (!value_types.compare("date")) this->system.insert(keys, values.toString());
                    if (!value_types.compare("time")) this->system.insert(keys, values.toString());
                }
            }



            querystr = ("SELECT xml_channel.key, xml_channel.value, xml_channel.label, xml_channel.value_type, xml_channel.xmlnode, xml_channel.editable FROM xml_channel;");
            if (xquery.exec(querystr)) {
                while (xquery.next()) {
                    keys = (xquery.value(0).toString());
                    values = (xquery.value(1));
                    labels = (xquery.value(2).toString());
                    value_types = (xquery.value(3).toString());
                    xmlnodes = (xquery.value(4).toString());
                    editables = (xquery.value(5).toString());
                    for (auto &chan : this->channels) {
                        if (!value_types.compare("int")) chan.insert(keys, values.toInt());
                        if (!value_types.compare("double")) chan.insert(keys, values.toDouble());
                        if (!value_types.compare("string")) chan.insert(keys, values.toString());
                        if (!value_types.compare("ullong")) chan.insert(keys, values.toULongLong());
                        if (!value_types.compare("date")) chan.insert(keys, values.toString());
                        if (!value_types.compare("time")) chan.insert(keys, values.toString());
                    }
                }
            }



        }
    }
    qDebug()  << Q_FUNC_INFO  << "data maps created";
    QSqlDatabase::removeDatabase("populate"); // correct doing this after getting out of scope

    this->set_system_value("meas_channels", QVariant(chans));

    return true;


}

void adu_json::prepare()
{
    if (this->root_obj != nullptr) this->root_obj.reset();
    this->root_obj = std::make_unique<QJsonObject>();

    this->system_to_json();
    this->channels_to_json();
}

QByteArray adu_json::toJsonByteArray(QJsonDocument::JsonFormat format)
{
    this->prepare();
    if (this->json_doc != nullptr) this->json_doc.reset();
    this->json_doc = std::make_unique<QJsonDocument>(*root_obj);
    return this->json_doc->toJson(format);
}

bool adu_json::fromQBytearray(const QByteArray &json_bytes)
{
    auto json_doc = QJsonDocument::fromJson(json_bytes);

    if(json_doc.isNull()){
        qDebug() << Q_FUNC_INFO  << "Failed to create JSON doc.";
        return false;
    }
    if(!json_doc.isObject()){
        qDebug() << Q_FUNC_INFO  << "JSON is not an object.";
        return false;
    }

    auto jobjects = json_doc.object();

    QVariantMap root_map = jobjects.toVariantMap();

    QVariantMap take_channels = root_map.value("system").toMap();

    bool ok = false;

    uint uchans = take_channels.value("meas_channels").toUInt(&ok);
    if (!ok) {
        qDebug() << Q_FUNC_INFO << "failed to init ADU! .. chans:" << take_channels.value("meas_channels").toString();
        return false;
    }
    if (uchans > adu_defs::max_channels) {
        qDebug() << Q_FUNC_INFO << "failed to init ADU channel, too many channels! " << uchans;
        return false;
    }
    if (!init_channels(uchans)) {
        qDebug() << Q_FUNC_INFO << "failed to init ADU channel init failed";
        return false;
    }


    QJsonObject::const_iterator first =  jobjects.constBegin();
    QJsonObject::const_iterator last =  jobjects.constEnd();

    while (first != last) {

        qDebug() << first.key();
        QStringList split(first.key().split("_"));

        // these are top objects of top arrays
        if ( first.value().isArray()) {
            QJsonArray chv(first.value().toArray());
            QJsonArray::const_iterator first_chan =  chv.constBegin();
            QJsonArray::const_iterator last_chan =   chv.constEnd();
            if (unsigned(chv.size()) == uchans) {
                qDebug() << "set";
            }
            //this->atswriters.resize(uchans);
            while (first_chan != last_chan) {
                QVariantMap map((*first_chan).toObject().toVariantMap());
                ok = false;
                uint ch = map.value("id").toUInt(&ok);
                if (!ok || ch > uchans) {
                    qDebug() << Q_FUNC_INFO << "failed to init ADU channel object  failed";
                    return false;
                }

                if (first.key() == "channels") {
                    if (this->channels[ch].size() == map.size()) {
                        qDebug() << "set";
                        map_hairdresser(this->channels.at(ch), map);
                        this->channels[ch] = map;
                        qDebug() << (*first_chan).toObject().value("id");
                    }
                }

                ++first_chan;
            }
        }

        // seems not to be an array - should be an object
        else {
            if(!first.key().compare("system")) {
                qDebug() << first.value().toObject().size();
                if (this->system.size() == first.value().toObject().size()) {
                    QVariantMap map(root_map[first.key()].toMap());
                    map_hairdresser(this->system, map);
                    this->system = map;
                    //this->system = root_map[first.key()].toMap();
                }
            }
        }
        ++first;
    }
    return true;
}

bool adu_json::to_JSON_file(const QFileInfo &qfi)
{

    QFile jsonFile(qfi.absoluteFilePath());
    if (jsonFile.open(QFile::WriteOnly)) {
        jsonFile.write(this->toJsonByteArray());
        jsonFile.close();

        return true;
    }
    return false;
}

bool adu_json::to_XML_file(const QFileInfo &qfi)
{
    QFile xmlFile(qfi.absoluteFilePath());
    if (xmlFile.open(QFile::WriteOnly)) {
        xmlFile.write(this->toXMLByteArray());
        xmlFile.close();

        return true;
    }
    return false;
}

QByteArray adu_json::toXMLByteArray()
{
    this->xmlqba.clear();
    QStringList keys;
    QXmlStreamWriter xmls(&xmlqba);
    xmls.setAutoFormatting(true);
    xmls.writeStartDocument();

    xmls.writeStartElement("measurement");
    xmls.writeStartElement("recording");

    keys = this->get_channel_node_keys("recording");
    for (auto &str : keys) {
        xmls.writeTextElement(str, this->get_system_value(str).toString());
    }



    xmls.writeStartElement("input");
    xmls.writeStartElement("Hardware");
    xmls.writeAttribute("GMS", this->GMS);
    xmls.writeAttribute("Version", this->Version);
    xmls.writeAttribute("Name", this->Name);
    xmls.writeAttribute("Type", this->Type);


    xmls.writeEndElement(); // Hardware
    xmls.writeEndElement(); // input

    xmls.writeStartElement("output");
    // -------------------------------------------Processing Tree BEGIN --------------------
    xmls.writeStartElement("ProcessingTree");
    xmls.writeAttribute("id", "0");
    xmls.writeStartElement("configuration");
    xmls.writeTextElement("processings", "mt_auto");
    xmls.writeEndElement();  // configuration


    xmls.writeStartElement("output");
    xmls.writeStartElement("ATSWriter");


    xmls.writeStartElement("configuration");
    keys.clear();
    keys = this->get_channel_node_keys("atswriter");
    for (int i = 0; i < this->size(); ++i) {
        xmls.writeStartElement("channel");
        xmls.writeAttribute("id", QString::number(i));
        for (auto &str : keys) {
            xmls.writeTextElement(str, channels.at(i).value(str).toString());
        }
        xmls.writeEndElement();
    }
    xmls.writeEndElement(); // configuration
    // ... comments
    xmls.writeEndElement(); // ATSWriter
    xmls.writeEndElement(); // output
    xmls.writeEndElement(); // ProcessingTree
    // -------------------------------------------Processing Tree END --------------------


    xmls.writeEndElement(); // output

    xmls.writeEndElement();  // recording

    // -------------------------------------------------------SELFTEST -----------------------------------------------------------
    xmls.writeStartElement("Selftest");
    xmls.writeAttribute("GMS", this->GMS);
    xmls.writeAttribute("Version", this->Version);
    xmls.writeAttribute("Name", this->Name);
    xmls.writeAttribute("Type", this->Type);

    xmls.writeStartElement("AdbBoards");
    keys.clear();
    keys = this->get_channel_node_keys("selftest");
    for (int i = 0; i < this->size(); ++i) {
        xmls.writeStartElement("channel");
        xmls.writeAttribute("id", QString::number(i));
        for (auto &str : keys) {
            xmls.writeTextElement(str, channels.at(i).value(str).toString());
        }
        xmls.writeEndElement();
    }
    xmls.writeEndElement();  // AdbBoards


    xmls.writeEndElement();  // selftest
    // ------------------------------------------------------- -----------------------------------------------------------


    xmls.writeEndElement(); // measurement

    xmls.writeEndDocument();
    return xmlqba;


}

bool adu_json::from_JSON(const QFileInfo *qfi, const QByteArray *qba)
{
    if (qfi != nullptr) {
        if (!qfi->exists()) return false;
        QFile file_obj(qfi->absoluteFilePath());
        if(!file_obj.open(QIODevice::ReadOnly)){
            qDebug() << Q_FUNC_INFO << "Failed to open " << qfi->absoluteFilePath();
            return false;
        }

        QByteArray json_bytes(file_obj.readAll());
        file_obj.close();

        if (this->fromQBytearray(json_bytes)) {
            qDebug() << Q_FUNC_INFO << "ADU built from file";
            return true;
        }

        return false;
    }

    if (qba != nullptr) {

        return false;
    }

    return false;

}

bool adu_json::from_XML(const QFileInfo *qfi)
{
    std::unique_ptr<QXmlStreamReader>xmlreader;
    QStringList node_pos;
    uint chan = 0;
    uint nchannels_adu = 1000;
    bool chan_ok = false;

    if (qfi != nullptr) {
        this->clear();
        QFile file(qfi->absoluteFilePath());

        if (!file.open(QIODevice::ReadOnly)) return false;


        // prepare the channel size of the ADU
        xmlreader = std::make_unique<QXmlStreamReader>(&file) ;
        //xmlreader->setDevice(&file);
        while (!xmlreader->atEnd()) {
            auto token = xmlreader->readNext();
            if (token == QXmlStreamReader::StartElement) {
                if (xmlreader->name().toString() == "meas_channels") {
                    bool ok = false;
                    QString init_chans(xmlreader->readElementText());
                    nchannels_adu = init_chans.toUInt(&ok);
                    if (!ok) {
                        qDebug() << Q_FUNC_INFO << "can not init ADU from XML, channels" << init_chans;
                        return false;
                    }
                    if (nchannels_adu < adu_defs::max_channels) {
                        // this initilaizes the ADU
                        this->init_channels(nchannels_adu);
                        break;
                    }


                }
            }
        }
        xmlreader.reset();
        file.close();
        if (!file.open(QIODevice::ReadOnly)) return false;
        xmlreader = std::make_unique<QXmlStreamReader>(&file) ;
        //xmlreader->setDevice(&file);

        // get the values after preparation

        std::vector<QVariantMap> xchannels(this->get_channels());
        QVariantMap xsystem(this->get_system());


        while (!xmlreader->atEnd()) {
            auto token = xmlreader->readNext();

            if (token == QXmlStreamReader::StartElement) {

                if (!node_pos.size()) node_pos.append(xmlreader->name().toString());
                if (node_pos.last() != xmlreader->name().toString()) {
                    node_pos.append(xmlreader->name().toString());
                }
                qDebug() << xmlreader->name();
                if (xmlreader->name().toString() == "channel") {
                    qDebug() << node_pos;
                    if( xmlreader->attributes().hasAttribute("id")) {
                        chan = xmlreader->attributes().value("id").toUInt(&chan_ok);
                        if (chan_ok) {
                            if (chan < this->size()) {
                                qDebug() << "reading channel no" << chan;
                                qDebug() << node_pos;
                            }
                            else {
                                chan_ok = false;
                            }
                        }
                    }
                    else {
                        chan_ok = false;
                    }
                }
                if (node_pos.contains("ATSWriter", Qt::CaseSensitive) && node_pos.contains("channel", Qt::CaseSensitive) && chan_ok) {
                    qDebug() << xchannels.at(chan).keys() << " -----" << xmlreader->name().toString();
                    if (xchannels.at(chan).keys().contains(xmlreader->name().toString())) {
                        xchannels[chan].insert(xmlreader->name().toString(), xmlreader->readElementText() );
                        if (node_pos.size()) node_pos.removeLast();
                    }
                }
                if (node_pos.contains("channel_config", Qt::CaseSensitive) && node_pos.contains("channel", Qt::CaseSensitive) && chan_ok) {
                    // qDebug() << xchannels.at(chan).keys() << " -----" << xmlreader->name().toString();
                    if (xchannels.at(chan).keys().contains(xmlreader->name().toString())) {
                        xchannels[chan].insert(xmlreader->name().toString(), xmlreader->readElementText() );
                        if (node_pos.size()) node_pos.removeLast();
                    }
                }
                if (node_pos.contains("Selftest", Qt::CaseSensitive) && node_pos.contains("AdbBoards", Qt::CaseSensitive) && node_pos.contains("channel", Qt::CaseSensitive) && chan_ok) {
                    // qDebug() << xchannels.at(chan).keys() << " -----" << xmlreader->name().toString();
                    if (xchannels.at(chan).keys().contains(xmlreader->name().toString())) {
                        xchannels[chan].insert(xmlreader->name().toString(), xmlreader->readElementText() );
                        if (node_pos.size()) node_pos.removeLast();
                    }
                }
                if (node_pos.contains("HwConfig", Qt::CaseSensitive) && node_pos.contains("AdbBoards", Qt::CaseSensitive) && node_pos.contains("channel", Qt::CaseSensitive) && chan_ok) {
                    // qDebug() << xchannels.at(chan).keys() << " -----" << xmlreader->name().toString();
                    if (xchannels.at(chan).keys().contains(xmlreader->name().toString())) {
                        xchannels[chan].insert(xmlreader->name().toString(), xmlreader->readElementText() );
                        if (node_pos.size()) node_pos.removeLast();
                    }
                }

                if (xmlreader->name().toString() == "NumSats") {
                    qDebug() << "NumSats" << xmlreader->lineNumber();
                    qDebug() << "top";
                }
                if (node_pos.contains("measurement", Qt::CaseSensitive) && !node_pos.contains("TXM22Hardware", Qt::CaseSensitive) && !node_pos.contains("SystemHistory", Qt::CaseSensitive)
                        && !node_pos.contains("calibration_sensors", Qt::CaseSensitive)  && !node_pos.contains("coordinates_external", Qt::CaseSensitive)
                        && !node_pos.contains("Software", Qt::CaseSensitive) && !chan_ok) {
                    // qDebug() << xsystem.keys() << " -----" << xmlreader->name().toString();
                    if (xsystem.keys().contains(xmlreader->name().toString())) {
                        xsystem.insert(xmlreader->name().toString(), xmlreader->readElementText() );
                        if (node_pos.size()) node_pos.removeLast();
                    }
                }

            }
            if (token == QXmlStreamReader::EndElement) {
                if (node_pos.size() && node_pos.last() == "channel") {
                    qDebug() << "out of channel section" << xmlreader->lineNumber();
                    chan_ok = false;
                }
                if (node_pos.size()) node_pos.removeLast();
            }

        }
        //if (token == QXmlStreamReader::Characters) qDebug() << xmlreader->readElementText();

        if (xmlreader->hasError()) {

            this->clear();
            return false;
        }

        auto xmap =xchannels.begin();
        for (auto &mymap : this->channels) {
            map_hairdresser(mymap, *xmap);
            mymap = *xmap;
            xmap->clear();
            ++xmap;
        }

        map_hairdresser(this->system, xsystem);
        this->system = xsystem;
        xsystem.clear();

        qDebug() << "here we are";

        return true;
    }


    return false;

}

uint adu_json::size() const
{
    return this->channels.size();
}

void adu_json::clear()
{
    this->channels.clear();
    this->sensors.clear();
    this->system.clear();
    if (this->root_obj != nullptr) this->root_obj.reset();
    if (this->json_doc != nullptr) this->json_doc.reset();

}

QVariant adu_json::get_channel_value(const uint &channel, const QString &key) const
{
    if (!this->size()) {
        qDebug() << Q_FUNC_INFO << "ADU has no channels";
        return QVariant("invalid");
    }
    if (channel >= this->size()) {
        qDebug() << Q_FUNC_INFO << "ADU has not enough channels:" << this->size() << "requested:" << channel;
        return QVariant("invalid");
    }

    return this->channels.at(channel).value(key, QVariant("invalid"));

}

bool adu_json::set_channel_value(const uint &channel, const QString &key, const QVariant &value)
{

    if (!this->size()) {
        qDebug() << Q_FUNC_INFO << "ADU has no channels";
        return false;
    }
    if (channel >= this->size()) {
        qDebug() << Q_FUNC_INFO << "ADU has not enough channels:" << this->size() << "requested:" << channel;
        return false ;
    }

    if (!this->channels.at(channel).contains(key)) return false;
    this->channels[channel].insert(key, value);
    return true;
}

bool adu_json::set_sensor_value(const uint &channel, const QString &key, const QVariant &value)
{
    if (!this->size()) {
        qDebug() << Q_FUNC_INFO << "ADU has no channels";
        return false;
    }
    if (channel >= this->size()) {
        qDebug() << Q_FUNC_INFO << "ADU has not enough channels:" << this->size() << "requested:" << channel;
        return false ;
    }

    if (!this->sensors.at(channel).contains(key)) return false;
    this->sensors[channel].insert(key, value);
    return true;
}

bool adu_json::get_selftest_channels(QList<QStringList> &txt_values, QStringList &out_keys, QStringList &out_tooltips) const
{
    if (!this->size()) {
        qDebug() << Q_FUNC_INFO << "ADU has no channels";
        return false;
    }
    out_keys.clear();
    out_tooltips.clear();
    QFileInfo info_db_file(app_homepath("info.sql3"));
    if (info_db_file.exists()) {
        QSqlDatabase info_db = QSqlDatabase::addDatabase("QSQLITE", "populate");
        info_db.setConnectOptions("QSQLITE_OPEN_READONLY");
        info_db.setDatabaseName(info_db_file.absoluteFilePath());

        if (info_db.open() ) {
            QSqlQuery xquery(info_db);

            QString querystr;
            querystr = ("SELECT xml_channel.key, xml_channel.tooltip FROM xml_channel WHERE xmlnode = 'selftest' ;");
            if (xquery.exec(querystr)) {
                while (xquery.next()) {
                    out_keys << (xquery.value(0).toString());
                    out_tooltips << (xquery.value(1).toString());
                }
            }
        }
        info_db.close();
        if (!out_keys.size()) return false;
        for (auto &chan : this->channels) {
            QStringList txt_vals;
            for (auto const &out_key : out_keys) {
                txt_vals.append(chan.value(out_key).toString());
            }
            txt_values.append(txt_vals);
            txt_vals.clear();
        }
    }
    qDebug()  << Q_FUNC_INFO  << "data maps created";
    QSqlDatabase::removeDatabase("populate"); // correct doing this after getting out of scope
    return true;

}

QStringList adu_json::get_channel_node_keys(const QString xmlnode) const
{
    QStringList node_keys;
    QFileInfo info_db_file(app_homepath("info.sql3"));
    if (info_db_file.exists()) {
        QSqlDatabase info_db = QSqlDatabase::addDatabase("QSQLITE", "nodekeys");
        info_db.setConnectOptions("QSQLITE_OPEN_READONLY");
        info_db.setDatabaseName(info_db_file.absoluteFilePath());

        if (info_db.open() ) {
            QSqlQuery xquery(info_db);

            QString querystr;
            querystr = ("SELECT xml_channel.key FROM xml_channel WHERE xmlnode = '" + xmlnode + "';");
            if (xquery.exec(querystr)) {
                while (xquery.next()) {
                    node_keys << (xquery.value(0).toString());
                }
            }
        }
        info_db.close();
    }
    qDebug()  << Q_FUNC_INFO  << " node selected";
    QSqlDatabase::removeDatabase("nodekeys"); // correct doing this after getting out of scope
    return node_keys;
}

bool adu_json::set_system_value(const QString &key, const QVariant &value)
{
    if (!this->size()) {
        qDebug() << Q_FUNC_INFO << "ADU has no channels";
        return false;
    }

    if (!this->system.contains(key)) return false;
    this->system.insert(key, value);
    return true;
}

QVariant adu_json::get_system_value(const QString &key) const
{

    if (!this->size()) {
        qDebug() << Q_FUNC_INFO << "ADU has no channels";
        return false;
    }
    return this->system.value(key, QVariant("invalid"));
}

double adu_json::dipole_length(const uint &channel, double &angle, bool *ok) const
{
    if (!this->size()) {
        qDebug() << Q_FUNC_INFO << "ADU has no channels";
        return false;
    }
    if (channel >= this->size()) {
        qDebug() << Q_FUNC_INFO << "ADU has not enough channels:" << this->size() << "requested:" << channel;
        return false ;
    }

    double length = 0.0;
    angle = 0.0;


    // if channel ty Hx ... Hz bool ok !
    bool isok = false;
    double pos_x1 = this->channels.at(channel).value("pos_x1").toDouble(&isok);
    if (!isok) {
        if (ok != nullptr) *ok = false;
        return length;

    }
    double pos_x2 = this->channels.at(channel).value("pos_x2").toDouble(&isok);
    if (!isok) {
        if (ok != nullptr) *ok = false;
        return length;

    }
    double pos_y1 = this->channels.at(channel).value("pos_y1").toDouble(&isok);
    if (!isok) {
        if (ok != nullptr) *ok = false;
        return length;

    }
    double pos_y2 = this->channels.at(channel).value("pos_y2").toDouble(&isok);
    if (!isok) {
        if (ok != nullptr) *ok = false;
        return length;

    }
    double pos_z1 = this->channels.at(channel).value("pos_z1").toDouble(&isok);
    if (!isok) {
        if (ok != nullptr) *ok = false;
        return length;

    }
    double pos_z2 = this->channels.at(channel).value("pos_z2").toDouble(&isok);
    if (!isok) {
        if (ok != nullptr) *ok = false;
        return length;

    }

    double tx, ty, tz;
    tx = double(pos_x2 - pos_x1);
    ty = double(pos_y2 - pos_y1);
    tz = double(pos_z2 - pos_z1);

    length = std::sqrt(tx * tx  + ty * ty + tz * tz);
    if (length < 0.001) length = 0.0;                   // avoid rounding errors

    // avoid calculation noise
    if (fabs(tx) < 0.001) tx = 0.0;
    if (fabs(ty) < 0.001) ty = 0.0;

    if ((length == 0) && !this->channels.at(channel).value("channel_type").toString().compare("Hx")) {
        angle = 0.;                                                  // NORTH
    }
    // Hy
    if ((length == 0) && !this->channels.at(channel).value("channel_type").toString().compare("Hy")) {
        angle = M_PI_2;                                              // EAST
    }
    // Hz
    if ((length == 0) && !this->channels.at(channel).value("channel_type").toString().compare("Hz")) {
        angle =  0.;
    }

    // hmm hmm possible but you normally set the system N S E W
    return atan2(ty,tx);


    return length;

}


std::vector<QVariantMap> adu_json::get_channels() const
{
    return this->channels;
}

QVariantMap adu_json::get_system() const
{
    return this->system;
}

std::vector<QVariantMap> adu_json::get_sensors() const
{
    return this->sensors;
}

QDateTime adu_json::get_start_time() const
{
    QDateTime adutime;
    adutime.setTimeSpec(Qt::UTC);
    adutime.setSecsSinceEpoch(0);

    if (!this->size()) {
        qDebug() << Q_FUNC_INFO << "ADU has no channels";
        return adutime;
    }

    QString datestr = this->system.value("start_date").toString();
    QString time_str = this->system.value("start_time").toString();
    QDateTime::fromString(datestr + " " + time_str, "yyyy-MM-dd hh:mm:ss");

    return adutime;

}

QDateTime adu_json::get_stop_time() const
{
    QDateTime adutime;
    adutime.setTimeSpec(Qt::UTC);
    adutime.setSecsSinceEpoch(0);

    if (!this->size()) {
        qDebug() << Q_FUNC_INFO << "ADU has no channels";
        return adutime;
    }

    QString datestr = this->system.value("stop_date").toString();
    QString time_str = this->system.value("stop_time").toString();
    QDateTime::fromString(datestr + " " + time_str, "yyyy-MM-dd hh:mm:ss");



    return adutime;
}

QDateTime adu_json::get_stop_time_calc(const int64_t duration) const
{

    QDateTime adutime(this->get_start_time());

    return adutime.addSecs(duration);
}

uint64_t adu_json::get_samples() const
{
    uint64_t samples(0);
    if (!this->size()) {
        qDebug() << Q_FUNC_INFO << "ADU has no channels";
        return samples;
    }
    bool ok = false;
    samples = this->channels.at(0).key("num_samples").toULongLong(&ok);
    if (!ok) {
        qDebug() << Q_FUNC_INFO << "can not get number of samples";
        return 0;
    }

    return samples;
}

double adu_json::get_sample_freq() const
{
    double sample_freq(0.0);
    if (!this->size()) {
        qDebug() << Q_FUNC_INFO << "ADU has no channels";
        return sample_freq;
    }


    bool ok = false;
    sample_freq = this->channels.at(0).key("sample_freq").toDouble(&ok);
    if (ok) return sample_freq;
    qDebug() << Q_FUNC_INFO << "can not get sample frequency";

    return 0.0;
}

uint64_t adu_json::get_duration() const
{

    if (!this->size()) {
        qDebug() << Q_FUNC_INFO << "ADU has no channels";
        return 0;
    }

    return duration;

}

bool adu_json::set_duration(const qint64 duration)
{
    if (!this->size()) {
        qDebug() << Q_FUNC_INFO << "ADU has no channels";
        return false;
    }
    if (duration < 0) {
        qDebug() << Q_FUNC_INFO << "duration < 0";
        return false;
    }
    this->duration = duration;
    QDateTime adutime(this->get_start_time().addSecs(duration));
    QVariant stop_date(adutime.date().toString("yyyy-MM-dd"));
    QVariant stop_time(adutime.time().toString("hh:mm:ss"));


    if (!this->set_system_value("stop_date", stop_date)) {
        qDebug() << Q_FUNC_INFO << "can not set stop_date";
        return false;
    }
    if (!this->set_system_value("stop_time", stop_time)) {
        qDebug() << Q_FUNC_INFO << "can not set stop_time";
        return false;
    }

    // not needed for a job
    if (this->edit_mode) {
        for (uint i = 0; i < this->size(); ++i) {
            if (!this->set_channel_value(i, "stop_time", stop_time)) {
                qDebug() << Q_FUNC_INFO << "can not set stop_time for channel object in edit mode";
                return false;
            }
            if (!this->set_channel_value(i, "stop_date", stop_date)) {
                qDebug() << Q_FUNC_INFO << "can not set stop_date for channel object in edit mode";
                return false;
            }
        }
    }

    return true;
}

void adu_json::set_edit_mode(const bool on_off)
{
    this->edit_mode = on_off;
}

void adu_json::slot_sensortype_of_serial_changed(const QString &sensortype, const int &existing_sernum)
{
    for (auto &chan : this->channels) {
        if (chan.value("sensor_sernum").toInt() == existing_sernum) {
            chan.insert("sensor_type", QVariant(sensortype));
        }
    }
}

void adu_json::channels_to_json()
{
    if (this->root_obj == nullptr) {
        qDebug() << Q_FUNC_INFO << "JSON root_obj not initialized";
        return;
    }

    QJsonArray channels;
    for (auto &chan : this->channels) {
        //root_obj->insert("channel_" + QString::number(i++), QJsonObject::fromVariantMap(chan));
        channels.append(QJsonObject::fromVariantMap(chan));
    }

    root_obj->insert("channels", channels);

}


void adu_json::system_to_json()
{
    if (this->root_obj == nullptr) {
        qDebug() << Q_FUNC_INFO << "JSON root_obj not initialized";
        return;
    }
    root_obj->insert("system", QJsonObject::fromVariantMap(this->system));

}
