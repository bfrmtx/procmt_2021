#include "measdocxml.h"


measdocxml::measdocxml(const QFileInfo *qfi, const QUrl* qurl, const QByteArray *qba , QObject *parent)  :  QObject(parent)
{

    this->open(qfi, qurl, qba);

}

measdocxml::measdocxml(QObject *parent)   :  QObject(parent)
{
    this->clear();

}

bool measdocxml::open(const QFileInfo *qfi, const QUrl *qurl, const QByteArray *qba)
{
    this->clear();
    if ((qfi == nullptr) && (qurl == nullptr) && (qba == nullptr)) {
        std::cerr << " measdocxml::measdocxml -> failed to initialize class - all nullptr! " << std::endl;
        return false;
    }


    if (qfi != nullptr) {
        this->qfi.setFile(qfi->absoluteFilePath());
        if (!this->qfi.exists()) {
            std::cerr <<  "measdocxml::measdocxml -> file does not exist " << this->qfi.absoluteFilePath().toStdString() << std::endl;
            return false;
        }
/*
#ifdef _msvc

        FILE* fp = nullptr;
        errno_t err = _wfopen_s(&fp, this->qfi.absoluteFilePath().toStdWString().c_str(), L"rb" );
        if( fp && !err )
        {
            if (doc.LoadFile( fp )) {


                this->root = doc.FirstChildElement();
                if (this->root == nullptr) {
                    this->doc.Clear();
                    std::cerr <<  "measdocxml::measdocxml -> can not parse file " << qfi->absoluteFilePath().toStdString() << std::endl;
                    return false;

                }

                fclose( fp );
            }
        }
#else
*/
//#if defined(_MSC_VER) && (_MSC_VER >= 1400 ) && (!defined WINCE)
//        if (!this->doc.LoadFile(this->qfi.absoluteFilePath().toStdWString().c_str())) {
//            this->root = doc.FirstChildElement();
//            if (this->root == nullptr) {
//                this->doc.Clear();
//                std::cerr <<  "measdocxml::measdocxml -> can not parse file " << qfi->absoluteFilePath().toStdString() << std::endl;
//                return false;

//            }
//        }
//#endif
        if (!this->doc.LoadFile(this->qfi.absoluteFilePath())) {
            this->root = doc.FirstChildElement();
            if (this->root == nullptr) {
                this->doc.Clear();
                std::cerr <<  "measdocxml::measdocxml -> can not parse file " << qfi->absoluteFilePath().toStdString() << std::endl;
                return false;

            }
        }
/*
#endif
*/
    }
    else if (qurl != nullptr) {
        if (qurl->isLocalFile()) {
            this->qfi.setFile(qurl->toLocalFile());
            if (!this->doc.LoadFile(this->qfi.absoluteFilePath().toStdString().c_str())) {
                this->root = doc.FirstChildElement();
                if (this->root == nullptr) {
                    this->doc.Clear();
                    std::cerr <<  "measdocxml::measdocxml -> can not parse file " << qfi->absoluteFilePath().toStdString() << std::endl;
                    return false;

                }
            }
            else {
                std::cerr <<  "measdocxml::measdocxml -> can not parse file " << qfi->absoluteFilePath().toStdString() << std::endl;
                return false;
            }

        }
        else {
            std::cerr << " measdocxml::measdocxml -> failed to initialize class from URL" << std::endl;
            return false;
        }
    }
    else if (qba != nullptr) {
        if(!this->doc.Parse(qba->data())) {
            this->root = doc.FirstChildElement();
            if (this->root == nullptr) {
                this->doc.Clear();
                std::cerr << "measdocxml::measdocxml -> can not parse QByteArray "  << std::endl;
                return false;
            }
        }
        else {
            std::cerr << "measdocxml::measdocxml -> can not parse QByteArray "  << std::endl;
            return false;
        }
    }

    else {
        std::cerr << " measdocxml::measdocxml -> failed to initialize class - all nullptr! " << std::endl;
        return false;
    }

    this->query_default_attributes(this->root);
    loaded_doc = tinyxml2::XML_SUCCESS;
    return true;
}


void measdocxml::clear()
{
    this->doc.Clear();
    this->section = nullptr;
    this->subsection = nullptr;
    this->node = nullptr;
    this->attribute = nullptr;
    this->element = nullptr;
    this->root = nullptr;
}

measdocxml::~measdocxml()
{
    this->doc.Clear();
    qDebug() << "measdocxml destroyed";
}

QByteArray *measdocxml::to_QByteArray()
{

    if(!this->root) return new QByteArray() ;

    tinyxml2::XMLPrinter printer;
    doc.Print(&printer);
    QByteArray* qba = new QByteArray(printer.CStr());
    return qba;
}

QString measdocxml::getXmlStr()
{

    std::lock_guard<std::recursive_mutex> lock(this->mutex);
    QByteArray *qba = this->to_QByteArray();
    return QString::fromUtf8(qba->data());
}

bool measdocxml::save(const QFileInfo* qfi)
{

    std::lock_guard<std::recursive_mutex> lock(this->mutex);
    if (qfi != nullptr) this->qfi.setFile(qfi->absoluteFilePath()); ;
    if(!this->root) return false;
    tinyxml2::XMLError er;
    if (!this->qfi.baseName().size()) return false;
    else {
        er =  doc.SaveFile(this->qfi.absoluteFilePath());
    }
    if (er == 0) return true;
    return false;
}


bool measdocxml::close()
{

    if(!this->root) return false;
    this->doc.Clear();
    this->root = nullptr;
    return true;


}

int measdocxml::set_date_time(const QDateTime &start_datetime, const QDateTime &stop_datetime)
{
    if(!this->root) return 0;
    std::lock_guard<std::recursive_mutex> lock(this->mutex);


    QString start_date = start_datetime.toString("yyyy-MM-dd");
    QString stop_date = stop_datetime.toString("yyyy-MM-dd");

    QString start_time = start_datetime.toString("hh:mm:ss");
    QString stop_time =  stop_datetime.toString("hh:mm:ss");


    int n = this->change_all_elements_text("start_date", start_date);
    n    += this->change_all_elements_text("stop_date", stop_date);

    n    += this->change_all_elements_text("start_time", start_time);
    n    += this->change_all_elements_text("stop_time", stop_time);


    qDebug() << n << "replacements made";


    //doc.SaveFile( "foo.xml" );
    return n;

}

int measdocxml::set_lat_lon_elev(const int &msec_lat, const int &msec_lon, const int &cm_elev)
{
    if(!this->root) return 0;
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    int n = this->change_all_elements_text("Latitude", QString::number(msec_lat));
    n    += this->change_all_elements_text("Longitude", QString::number(msec_lon));
    n    += this->change_all_elements_text("Height", QString::number(cm_elev));

    n    += this->change_all_elements_text("latitude", QString::number(msec_lat));
    n    += this->change_all_elements_text("longitude", QString::number(msec_lon));
    n    += this->change_all_elements_text("height", QString::number(cm_elev));
    qDebug() << n << "replacements made";
    return n;
}

int measdocxml::change_all_elements_text(const QString &element_name, const QString &value)
{

    if(!this->root) return 0;
    std::lock_guard<std::recursive_mutex> lock(this->mutex);


    std::string sSearchString = element_name.toStdString();
    std::string strval = value.toStdString();

    int n = 0;

    tinyxml2::XMLNode *xnode;
    tinyxml2::XMLElement *xlement = doc.FirstChildElement();
    while(xlement)
    {
        if (xlement->Value() && !std::string(xlement->Value()).compare(sSearchString))
        {
            if (this->verbose) qInfo() << "---> measdocxml::change_all_element_text setting" << xlement->Value() << "-> " << value;
            xlement->SetText(strval.c_str());
            ++n;
        }
        else {
            if (this->verbose) qInfo() << "measdocxml::change_all_element_text scanning -> " << xlement->Value() << xlement->GetText();
        }
        /*
         *   We move through the XML tree following these rules (basically in-order tree walk):
         *   (1) if there is one or more child element(s) visit the first one
         *       else
         *   (2)     if there is one or more next sibling element(s) visit the first one
         *               else
         *   (3)             move to the parent until there is one or more next sibling elements
         *   (4)             if we reach the end break the loop
         */
        if (xlement->FirstChildElement()) //(1)
            xlement = xlement->FirstChildElement();
        else if (xlement->NextSiblingElement())  //(2)
            xlement = xlement->NextSiblingElement();
        else
        {
            while(xlement->Parent() && !xlement->Parent()->NextSiblingElement()) { //(3)
                xnode = xlement->Parent();
                if (!xnode) return 1;
                xlement = xnode->ToElement();

                if (xlement == this->root) break;

            }
            if(xlement->Parent() && xlement->Parent()->NextSiblingElement()) {
                xlement = xlement->Parent()->NextSiblingElement();
            }
            else //(4)
                break;
        }//else
    }//while
    return n;
}

int measdocxml::goto_section(const QString &section_name, const int &nth_section)
{
    if(!this->root) return 0;
    return this->pvt_set_section(section_name, nth_section, false);
}

int measdocxml::goto_sub_section(const QString &section_name, const QString &subsection_name, const int &nth_section)
{
    if(!this->root) return 0;
    if(this->pvt_set_section(section_name, nth_section, false)) {
        return this->pvt_set_section(subsection_name, nth_section, true);
    }
    return 0;
}

bool measdocxml::is_ok() const
{
    if(!this->root) return false;
    return true;
}



QFileInfo measdocxml::get_qfile_info() const
{
    return this->qfi;
}

int measdocxml::pvt_set_section(const QString &section_name, const int &nth_section, const bool second_call_for_subsection)
{

    if(!this->root) return 0;

    if (second_call_for_subsection && !this->section) return 0;

    if (second_call_for_subsection) {
        this->subsection = nullptr;
    }
    else this->section = nullptr;


    std::string sSearchString = section_name.toStdString();

    int n = 0;

    tinyxml2::XMLNode *xnode;
    tinyxml2::XMLElement *xmlelement = nullptr;
    if (second_call_for_subsection) xmlelement = this->section->FirstChildElement();
    else xmlelement = doc.FirstChildElement();
    while(xmlelement)
    {
        // hence that a section has children ... right?
        if ((xmlelement->Value()) && (!std::string(xmlelement->Value()).compare(sSearchString)) && (xmlelement->FirstChildElement()))
        {
            if (this->verbose) qInfo() << " ---> measdocxml::set_section got section" << xmlelement->Value();

            this->query_default_attributes(xmlelement);

            ++n;
            if (n == nth_section) {
                if (second_call_for_subsection) this->subsection = xmlelement;
                else this->section = xmlelement;
                return n;
            }
        }
        else {
            if (this->verbose) qInfo() << "measdocxml::set_section reaching" << xmlelement->Value() << xmlelement->GetText();
        }

        /*
         *   We move through the XML tree following these rules (basically in-order tree walk):
         *   (1) if there is one or more child element(s) visit the first one
         *       else
         *   (2)     if there is one or more next sibling element(s) visit the first one
         *               else
         *   (3)             move to the parent until there is one or more next sibling elements
         *   (4)             if we reach the end break the loop
         */
        if (xmlelement->FirstChildElement()) //(1)
            xmlelement = xmlelement->FirstChildElement();
        else if (xmlelement->NextSiblingElement())  //(2)
            xmlelement = xmlelement->NextSiblingElement();
        else
        {
            while(xmlelement->Parent() && !xmlelement->Parent()->NextSiblingElement()) { //(3)
                xnode = xmlelement->Parent();
                if (!xnode) return 1;
                xmlelement = xnode->ToElement();

                if (second_call_for_subsection && xmlelement == this->section) break;
                else if (xmlelement == this->root) break;

            }
            if(xmlelement->Parent() && xmlelement->Parent()->NextSiblingElement()) {
                xmlelement = xmlelement->Parent()->NextSiblingElement();
            }
            else //(4)
                break;
        }//else
    }//while
    return n;




}

int measdocxml::get_siblings_data( tinyxml2::XMLElement *start_sibling, QMap<QString, QVariant> &data, const bool insert_undefinded)
{
    if(!this->root) return 0;
    int n = 0;
    bool conv = false;
    if (data.size()) conv = true;
    QString element_value, element_str;
    tinyxml2::XMLElement* xmlelement = start_sibling;
    if (!xmlelement) return 0;
    do {
        if (!xmlelement->FirstChildElement()) {
            if (this->verbose) qInfo() << "measdocxml::get_siblings_data getting : " << xmlelement->Value() << xmlelement->GetText();
            element_str = xmlelement->Value();
            element_value = xmlelement->GetText();

            if (conv) {
                if (data.contains(element_str)) {
                    QVariant in = element_value;
                    data.insert(element_str, this->get_native(in, data.value(element_str)));
                    ++n;

                }
                else if (insert_undefinded) {
                    data.insert(element_str, element_value);
                    ++n;
                }
            }
            else if (insert_undefinded) {
                data.insert(element_str, element_value);
                ++n;
            }

        }
        xmlelement = xmlelement->NextSiblingElement();
    } while (xmlelement);
    return n;
}

int measdocxml::get_siblings_data_txm(tinyxml2::XMLElement *start_sibling, QMap<QString, QVariant> &data, const QString &idstr,
                                      const QString use_topnode, const QString use_node, const int position_id)
{
    if(!this->root) return 0;
    int n = 0;
    bool conv = false;
    if (data.size()) conv = true;
    QString element_value, element_str;
    int iattr;
    tinyxml2::XMLElement* xmlelement = start_sibling;
    if (!xmlelement) return 0;
    do {
        iattr = -1;
        if (!xmlelement->FirstChildElement()) {
            element_str = xmlelement->Value();
            element_value = xmlelement->GetText();
            xmlelement->QueryIntAttribute(idstr.toStdString().c_str(), &iattr);
            if (this->verbose) qInfo() << "measdocxml::get_siblings_data_txm getting : " << xmlelement->Value() << xmlelement->GetText() << iattr;
            //E3_Position_0_Elevation
            if (use_topnode.size() && use_node.size() && (position_id != -1)) element_str = use_topnode + "_" + use_node + "_" + QString::number(position_id) + "_" + element_str;
            else if (use_topnode.size() && use_node.size()) element_str = use_topnode + "_" + use_node + "_" + element_str;
            if (iattr != -1 && (position_id == -1)) element_str += "_" + idstr + "_" + QString::number(iattr);
            if (conv) {
                if (data.contains(element_str)) {
                    QVariant in = element_value;
                    data.insert(element_str, this->get_native(in, data.value(element_str)));
                    ++n;

                }
            }
        }
        xmlelement = xmlelement->NextSiblingElement();
    } while (xmlelement);
    return n;
}

int measdocxml::set_siblings_data(tinyxml2::XMLElement *start_sibling, const QMap<QString, QVariant> &data)
{
    if(!this->root) return 0;
    int n = 0;
    tinyxml2::XMLElement* xmlelement = start_sibling;
    if (!xmlelement) return 0;
    do {
        // qDebug() << "sibb : " << xmlelement->Value();
        QString name(xmlelement->Value());
        if (data.contains(name)) {
            // compiler warning may be ok here - enum compare
            if (data.value(name).metaType().id() == QMetaType::Double) {
                QString dnum = QString::number(data.value(name).toDouble(), 'E', 8 );
                xmlelement->SetText(dnum.toStdString().c_str());
            }
            else if (data.value(name).metaType().id() == QMetaType::Float) {
                QString dnum = QString::number(data.value(name).toDouble(), 'G', 6 );
                xmlelement->SetText(dnum.toStdString().c_str());
            }
            else xmlelement->SetText(data.value(name).toString().toStdString().c_str());
        }
        xmlelement = xmlelement->NextSiblingElement();
        ++n;

    } while (xmlelement);
    return n;
}

QStringList measdocxml::get_atsfiles(QList<int> &channel_nos)
{


    QStringList filenames;
    if(!this->root) return filenames;

    QString topnode = "ATSWriter";
    QList<QMap<QString, QVariant>> writers;

    if (this->goto_sub_section(topnode, "channel", 1) != 1)  {
        topnode = "ATSWriterSettings";
        if (this->goto_sub_section(topnode, "channel", 1) != 1) return filenames;
    }

    tinyxml2::XMLElement *xmlelement = this->subsection;
    if (!xmlelement) return filenames;

    do {
        int id = -1;
        this->create_atswriter(false);
        xmlelement->QueryIntAttribute( "id", &id );
        qDebug() << " : " << xmlelement->Value() << id;

        this->get_siblings_data(xmlelement->FirstChildElement(), this->atswriter_data, false);
        if (id >= 0) channel_nos.append(id);


        writers.append(this->atswriter_data);


        xmlelement = xmlelement->NextSiblingElement();
        id = -1;
    } while (xmlelement);


    if (!writers.size()) return filenames;

    for (auto &map : writers) {

        QString string(map.value("ats_data_file").toString());
        if (string.size() && string.contains(".ats", Qt::CaseInsensitive)) {
            filenames.append(string);

        }
    }
    if (channel_nos.size() == filenames.size()) return filenames;
    return QStringList();
}

int measdocxml::get_calentries(const int channel, QMap<QString, QVariant> &calinfo,  std::vector<double> &f_on, std::vector<double> &ampl_on, std::vector<double> &phase_grad_on,
                               std::vector<double> &f_off, std::vector<double> &ampl_off, std::vector<double> &phase_grad_off)
{
    if(!this->root) return 0;
    QString topnode = "calibration_sensors";
    if (this->goto_sub_section(topnode, "channel", 1) != 1) {
        qDebug() << "no calibration sensor section found";
        return 0;
    }
    tinyxml2::XMLElement *xmlelementtop = this->subsection;
    if (!xmlelementtop) return 0;


    QMap<QString, QVariant> calitem;
    calitem.insert("ci", QString(""));
    calitem.insert("ci_serial_number", int(0));
    calitem.insert("ci_date", QString(""));
    calitem.insert("ci_time", QString(""));


    QMap<QString, QVariant> calentry;
    calentry.insert("c0", double(0.0));
    calentry.insert("c1", double(0.0));
    calentry.insert("c2", double(1.0));
    calentry.insert("c3", double(1.0));

    do {
        int id = -1;
        xmlelementtop->QueryIntAttribute( "id", &id );
        qDebug() << "sensorcal channel : " << xmlelementtop->Value() << id;

        if (id == channel) {
            tinyxml2::XMLElement *xmlelement = nullptr;
            tinyxml2::XMLElement *xmlelement_cdata = nullptr;
            tinyxml2::XMLElement *xmlelement_ci = nullptr;


            xmlelement = xmlelementtop->FirstChildElement("calibration");

            if (xmlelement == nullptr) {
                qDebug() << "no valid sensor entry";
                return 0;
            }

            xmlelement_ci = xmlelement->FirstChildElement("calibrated_item");
            if (xmlelement_ci == nullptr) {
                qDebug() << "no sensor description";
                calinfo = calitem;
                return 0;
            }
            else {
                this->get_siblings_data(xmlelement_ci->FirstChildElement(), calitem, false);
                calinfo = calitem;
            }

            xmlelement_cdata = xmlelement->FirstChildElement("caldata");

            if (xmlelement_cdata == nullptr) {
                qDebug() << "no cal entries - ok if E sensor";
            }
            else {

                do {
                    const char* chopper = xmlelement_cdata->Attribute( "chopper" );
                    this->get_siblings_data(xmlelement_cdata->FirstChildElement(), calentry, false);
                    if (chopper != nullptr) {
                        QString qchooper = QString::fromUtf8(chopper);
                        if (!qchooper.compare("on") && (calentry.value("c1").toDouble() != 0.0)) {
                            f_on.push_back(calentry.value("c1").toDouble());
                            ampl_on.push_back(calentry.value("c2").toDouble());
                            phase_grad_on.push_back(calentry.value("c3").toDouble());

                        }
                        else if (calentry.value("c1").toDouble() != 0.0) {
                            f_off.push_back(calentry.value("c1").toDouble());
                            ampl_off.push_back(calentry.value("c2").toDouble());
                            phase_grad_off.push_back(calentry.value("c3").toDouble());

                        }
                    }
                    calentry.insert("c0", double(0.0));
                    calentry.insert("c1", double(0.0));
                    calentry.insert("c2", double(1.0));
                    calentry.insert("c3", double(1.0));


                    xmlelement_cdata = xmlelement_cdata->NextSiblingElement();
                } while (xmlelement_cdata);
            }
        }
        xmlelementtop = xmlelementtop->NextSiblingElement();
        id = -1;

    } while (xmlelementtop);

    return (int)(f_on.size() + f_off.size());


}

int measdocxml::renumber_ids_calentries(const QMap<int, int> old_id_new_id)
{

    if(!this->root) return 0;
    int hits = 0;
    QString topnode = "calibration_sensors";
    if (this->goto_sub_section(topnode, "channel", 1) != 1) {
        qDebug() << "no calibration sensor section found";
        return 0;
    }
    tinyxml2::XMLElement *xmlelementtop = this->subsection;
    if (!xmlelementtop) return 0;

    do {
        int id = -1;
        //char *chint;
        xmlelementtop->QueryIntAttribute( "id", &id );

        qDebug() << "sensorcal channel : " << xmlelementtop->Value() << id;
        if (old_id_new_id.contains(id)) {
            qDebug() << "remapping" << id << "to" << old_id_new_id.value(id);
            xmlelementtop->SetAttribute("id", old_id_new_id.value(id));
            ++hits;
        }

        xmlelementtop = xmlelementtop->NextSiblingElement();
        id = -1;

    } while (xmlelementtop);


    return hits;

}


void measdocxml::set_verbose(const bool true_false)
{
    this->verbose = true_false;
}

void measdocxml::query_default_attributes(const tinyxml2::XMLElement *xmlelement)
{
    if(!this->root) return;


    if (!xmlelement) return;

    const char* cname = xmlelement->Attribute( "Name" );
    const char* cvers = xmlelement->Attribute( "Version" );

    if (!cname || !cvers) return;


    xmlelement->QueryIntAttribute( "GMS", &this->GMS );     // if "GMS" isn't found, value will still be as initialized
    xmlelement->QueryIntAttribute( "Type", &this->Type );   // if "Type" isn't found, value will still be as initialized


    if (cname) {
        this->Name = QString::fromUtf8(cname);
    }
    if (cvers) {
        this->Version = QString::fromUtf8(cvers);
    }

    if (this->verbose) qInfo() << "GMS =" << this->GMS << ", Type =" << this->Type << ", Version =" << this->Version << ", Name =" << this->Name;

}

int measdocxml::set_channel_section(const QString &topnode, const QList<QMap<QString, QVariant>> &channel_maps, const int &channel_id, const bool main_time_sample_freq)
{

    if(!this->root) return 0;

    int n = 0;
    int id = -1;

    if (this->goto_sub_section(topnode, "channel", 1) == 1) {
        tinyxml2::XMLElement *xmlelement = this->subsection;
        if (!xmlelement) return 0;
        do {
            xmlelement->QueryIntAttribute( "id", &id );
            qDebug() << " : " << xmlelement->Value() << id;
            if (id == channel_id) {
                this->set_siblings_data(xmlelement->FirstChildElement(), channel_maps[id]);
            }
            xmlelement = xmlelement->NextSiblingElement();
            id = -1;
        } while (xmlelement);

    }
    else return 0;

    return n;

}

int measdocxml::set_channel_section(const QString &topnode, const QMap<QString, QVariant> &channel_map, const int &channel_id, const bool main_time_sample_freq)
{
    if(!this->root) return 0;

    int n = 0;
    int id = -1;

    if (this->goto_sub_section(topnode, "channel", 1) == 1) {
        tinyxml2::XMLElement *xmlelement = this->subsection;
        if (!xmlelement) return 0;
        do {
            xmlelement->QueryIntAttribute( "id", &id );
            //qDebug() << " : " << xmlelement->Value() << id;
            if (id == channel_id) {
                this->set_siblings_data(xmlelement->FirstChildElement(), channel_map);
            }
            xmlelement = xmlelement->NextSiblingElement();
            id = -1;
        } while (xmlelement);

    }
    else return 0;

    if (main_time_sample_freq) {
        if (channel_map.contains("start_time")  && channel_map.contains("stop_time")  && channel_map.contains("start_date")&& channel_map.contains("stop_date") && channel_map.contains("sample_freq") ) {
            this->set_main_time_sample_freq(channel_map.value("start_time"), channel_map.value("stop_time") ,channel_map.value("start_date"), channel_map.value("stop_date"), channel_map.value("sample_freq"));
        }
    }




    return n;
}

int measdocxml::set_simple_section(const QString &topnode, const QString &subnode, const QMap<QString, QVariant> &section_data)
{
    if(!this->root) return 0;

    if (this->goto_sub_section(topnode, subnode) == 1) {
        tinyxml2::XMLElement *xmlelement = this->subsection;
        if (!xmlelement) return 0;
        this->set_siblings_data(xmlelement->FirstChildElement(), section_data);
    }

    return 1;


}

int measdocxml::set_main_time_sample_freq(const QVariant &start_time, const QVariant &stop_time, const QVariant &start_date, const QVariant &stop_date, const QVariant &sample_freq)
{


    if (this->goto_section("recording")) {

        QMap<QString, QVariant> map;
        map.insert("start_time", start_time);
        map.insert("stop_time", stop_time);
        map.insert("start_date", start_date);
        map.insert("stop_date", stop_date);
        tinyxml2::XMLElement *xmlelement = this->section;
        this->set_siblings_data(xmlelement->FirstChildElement(), map);

    }
    if (this->goto_section("global_config")) {
        QMap<QString, QVariant> map;
        map.insert("sample_freq", sample_freq);
        tinyxml2::XMLElement *xmlelement = this->section;
        this->set_siblings_data(xmlelement->FirstChildElement(), map);

    }
    return 0; // return value was missing
}

int measdocxml::get_channel_section(const QString &topnode, QList<QMap<QString, QVariant> > &channel_maps, const int &channel_id, const bool insert_undefinded)
{
    if(!this->root) return 0;

    int n = 0;
    int id = -1;
    if (this->goto_sub_section(topnode, "channel", 1) == 1) {
        tinyxml2::XMLElement *xmlelement = this->subsection;
        if (!xmlelement) return 0;
        do {
            xmlelement->QueryIntAttribute( "id", &id );
            if (this->verbose) qDebug() << " : " << xmlelement->Value() << id;
            if (id == channel_id) {
                this->get_siblings_data(xmlelement->FirstChildElement(), channel_maps[id], insert_undefinded);
            }
            xmlelement = xmlelement->NextSiblingElement();
            id = -1;
        } while (xmlelement);

    }
    else return 0;


    return n;
}

void measdocxml::get_atswriter_section()
{
    if(!this->root) return;

    QString topnode = "ATSWriter";

    if (this->goto_sub_section(topnode, "channel", 1) != 1)  {
        topnode = "ATSWriterSettings";
        if (this->goto_sub_section(topnode, "channel", 1) != 1) return;
    }

    tinyxml2::XMLElement *xmlelement = this->subsection;
    if (!xmlelement) return;

    do {
        int id = -1;
        this->create_atswriter();
        xmlelement->QueryIntAttribute( "id", &id );
        qDebug() << " : " << xmlelement->Value() << id;

        this->get_siblings_data(xmlelement->FirstChildElement(), this->atswriter_data, false);
        emit this->signal_got_atswriter_channel(this->atswriter_data, id);

        xmlelement = xmlelement->NextSiblingElement();
        id = -1;
    } while (xmlelement);

    this->get_section(topnode, "comments", this->atswriter_comments);
    emit this->signal_atswriter_comments_qmap_created(this->atswriter_comments);

    this->get_section(topnode, "output_file", this->atswriter_output_file);
    emit this->signal_atswriter_output_file_qmap_created(this->atswriter_output_file);

    this->get_section(topnode, "configuration", this->atswriter_configuration);
    emit this->signal_atswriter_configuration_qmap_created(this->atswriter_configuration);


    return ;



}

void measdocxml::get_adbboard_autgains()
{

    if(!this->root) return;

    QString topnode = "AdbBoards";
    QString subnode = "channel";


    if (this->goto_sub_section(topnode, "channel", 1) != 1)  {
        topnode = "AdbBoards";
        if (this->goto_sub_section(topnode, "channel", 1) != 1) return;
    }

    tinyxml2::XMLElement *xmlelementtop = this->subsection;
    if (!xmlelementtop) return;

    do {

        int id = -1;
        xmlelementtop->QueryIntAttribute( "id", &id );
        qDebug() << " -->  gains board : " << xmlelementtop->Value() << id;


        //if (this->goto_sub_section(subnode, "Settings", 1) != 1) return;
        //tinyxml2::XMLElement *xmlelement
        tinyxml2::XMLElement *xmlelement = xmlelementtop->FirstChildElement("Settings");

        if (!xmlelement) return;


        do {
            this->create_adbboard_autogain(false);


            int input = -1;
            QString input_circuitry = "empty";
            QString filter_type_main = "empty";

            const char* cinput_circuitry = xmlelement->Attribute( "input_circuitry" );
            const char* cfilter_type_main = xmlelement->Attribute( "filter_type" );

            if (cinput_circuitry) {
                input_circuitry = QString::fromUtf8(cinput_circuitry);
            }
            if (cfilter_type_main) {
                filter_type_main = QString::fromUtf8(cfilter_type_main);
            }

            xmlelement->QueryIntAttribute( "input", &input );
            qDebug() <<  xmlelement->Value() << "input:" << input << input_circuitry << filter_type_main << " id" << id;

            this->adbboard_autogain.insert("input", input);
            this->adbboard_autogain.insert("input_circuitry", input_circuitry);
            this->adbboard_autogain.insert("filter_type_main", filter_type_main);

            this->get_siblings_data(xmlelement->FirstChildElement(), this->adbboard_autogain, false);
            emit this->signal_got_adbboard_autogain_channel(this->adbboard_autogain, id);

            xmlelement = xmlelement->NextSiblingElement();

        } while (xmlelement);

        id = -1;
        xmlelementtop = xmlelementtop->NextSiblingElement();
    } while (xmlelementtop);



    return;

}

void measdocxml::get_txm(const bool emits)
{
    if(!this->root) return;
    this->create_txm(false);

    this->get_txm_section("TransmissionList", "Cycle", this->txm_cycle, "dipole", "", "", -1);
    this->get_txm_section("TXM22Hardware", "Waveform", this->txm_waveform, "",  "", "", -1);
    this->get_txm_section("TXM22Hardware", "Sequence", this->txm_sequence, "",  "", "", -1);
    this->get_txm_section("E1", "Position", this->txm_epos, "id", "E1", "Position", 0);
    this->get_txm_section("E2", "Position", this->txm_epos, "id", "E2", "Position", 0);
    this->get_txm_section("E3", "Position", this->txm_epos, "id", "E3", "Position", 0);

    if (emits) {
        emit signal_txm_cycle_qmap_created(this->txm_cycle);
        emit signal_txm_sequence_qmap_created(this->txm_sequence);
        emit signal_txm_waveform_qmap_created(this->txm_waveform);
        emit signal_txm_epos_qmap_created(this->txm_epos);

    }

}

// TODO: @bfr please review, the meas information was missing via get_section
int measdocxml::get_recording_section(const QString &topnode, const QString &node, QMap<QString, QVariant> &data_map, const bool insert_undefinded)
{
    if(!this->root) return 0;

    if (this->goto_sub_section(topnode, node, 1) == 1) {
        tinyxml2::XMLElement *xmlelement = this->subsection;
        if (!xmlelement) return 0;
        qDebug() << "measdocxml::get_recording_section -> " << xmlelement->Value();
        int flat = this->get_siblings_data(xmlelement->FirstChildElement(), data_map, insert_undefinded);
        if (this->goto_sub_section(node, "Measurements", 1) == 1) {
            if (this->goto_sub_section("Measurements", "Meas", 1) == 1) {
                this->get_siblings_data(this->subsection->FirstChildElement(), data_map, insert_undefinded);
            }
        }
        return flat;
    }
    return 0;
}

int measdocxml::get_section(const QString &topnode, const QString &node, QMap<QString, QVariant> &data_map, const bool insert_undefinded)
{
    if(!this->root) return 0;

    if (this->goto_sub_section(topnode, node, 1) == 1) {
        tinyxml2::XMLElement *xmlelement = this->subsection;
        if (!xmlelement) return 0;
        qDebug() << "measdocxml::get_section -> " << xmlelement->Value();
        return this->get_siblings_data(xmlelement->FirstChildElement(), data_map, insert_undefinded);
    }
    return 0;


}

int measdocxml::get_txm_section(const QString &topnode, const QString &node, QMap<QString, QVariant> &data_map,
                                const QString idstr, const QString use_topnode, const QString use_node, const int position_id)
{
    if(!this->root) return 0;

    if (this->goto_sub_section(topnode, node, 1) == 1) {
        int iattr = -1;
        tinyxml2::XMLElement *xmlelement = this->subsection;
        if (!xmlelement) return 0;
        qDebug() << "measdocxml::get_section -> " << xmlelement->Value();
        xmlelement->QueryIntAttribute("id", &iattr);
        if (position_id != -1) this->get_siblings_data_txm(xmlelement->FirstChildElement(), data_map, idstr, use_topnode, use_node, iattr);
        else this->get_siblings_data_txm(xmlelement->FirstChildElement(), data_map, idstr, use_topnode, use_node, -1);
        xmlelement = xmlelement->NextSiblingElement();
        if (!xmlelement) return 0;
        iattr = -1;
        xmlelement->QueryIntAttribute("id", &iattr);
        if (position_id != -1) this->get_siblings_data_txm(xmlelement->FirstChildElement(), data_map, idstr, use_topnode, use_node, iattr);
        else this->get_siblings_data_txm(xmlelement->FirstChildElement(), data_map, idstr, use_topnode, use_node, -1);
    }
    return 0;
}

int measdocxml::get_top_section(const QString &topnode, QMap<QString, QVariant> &data_map, const bool insert_undefinded)
{
    if(!this->root) return 0;

    if (this->goto_section(topnode, 1) == 1) {
        tinyxml2::XMLElement *xmlelement = this->section;
        if (!xmlelement) return 0;
        qDebug() << "measdocxml::get_top_section -> " << xmlelement->Value();
        return this->get_siblings_data(xmlelement->FirstChildElement(), data_map, insert_undefinded);
    }
    return 0;


}

bool measdocxml::root_attributes(int &GMS, int &Type, QString &Version, QString &Name, QString &root_node_name)
{
    if(!this->root) return false;
    this->query_default_attributes(this->root);
    GMS = this->GMS;
    Type = this->Type;
    Version = this->Version;
    Name = this->Name;
    root_node_name = QString(this->root->Value());
    return true;
}

int measdocxml::get_adbboard(const QString &topnode, const bool insert_undefinded)
{
    if(!this->root) return 0;

    this->create_adbboard(false);


    if (this->goto_section(topnode, 1) == 1) {
        tinyxml2::XMLElement *xmlelement = this->section;
        if (!xmlelement) return 0;
        do {
            qDebug() << "measdocxml::get_section -> " << xmlelement->Value();
            this->get_siblings_data(xmlelement->FirstChildElement(), this->adbboard_data, insert_undefinded);
            emit this->signal_adbboard_data_qmap_created(this->adbboard_data);
            this->create_adbboard(false);
            xmlelement = xmlelement->NextSiblingElement();
        } while (xmlelement);
    }

    return 0;

}

int measdocxml::compare_gms_revmain_rev_sub_type(QMap<QString, QVariant> &data_map, const int &GMS, const int &RevMain, const int &RevSub, const int &Type)
{
    if (data_map.contains("GMS") && data_map.contains("RevMain") && data_map.contains("RevSub") && data_map.contains("Type")) {
        if ( (data_map.value("GMS").toInt() == GMS) && (data_map.value("RevMain").toInt() == RevMain) && (data_map.value("RevSub").toInt() == RevSub) && (data_map.value("Type").toInt() == Type) ) {
            return 1;
        }
    }

    return 0;
}

QVariant measdocxml::get_native(const QVariant &in, const QVariant &native)
{
    if (in == native) return native;
    QVariant tmp = in;
  bool ok = tmp.convert(native.metaType());
    if (!ok) return native;
    return tmp;
}



void measdocxml::create_adbboard(const bool emits)
{

    this->adbboard_data.insert("GMS", int(0));
    this->adbboard_data.insert("RevMain", int(0));
    this->adbboard_data.insert("RevSub", int(0));
    this->adbboard_data.insert("Type", int(0));
    this->adbboard_data.insert("Serial", int(0) );


    this->adbboard_data.insert("Name", QString("empty"));
    this->adbboard_data.insert("Vendor", QString("empty"));
    this->adbboard_data.insert("SupplyVoltage", double(0.0));
    this->adbboard_data.insert("MtxPartNumber", QString("empty"));
    this->adbboard_data.insert("InputRange", QString("empty"));

    this->adbboard_data.insert("GainsStage1", QString("empty"));
    this->adbboard_data.insert("GainsStage2", QString("empty"));
    this->adbboard_data.insert("SampleFreqs", QString("empty"));
    this->adbboard_data.insert("Filters", QString("empty"));
    this->adbboard_data.insert("Filters_LF", QString("empty"));
    this->adbboard_data.insert("Filters_HF", QString("empty"));

    this->adbboard_data.insert("DAC", QString("empty"));
    this->adbboard_data.insert("Chopper", QString("empty"));
    this->adbboard_data.insert("InputConnection", QString("empty"));
    this->adbboard_data.insert("ExtGain", QString("empty"));

    if (emits) emit this->signal_adbboard_data_qmap_created(this->adbboard_data);

}

void measdocxml::create_sensor(const bool emits)
{

    this->sensor_data.insert("GMS", int(0));
    this->sensor_data.insert("RevMain", int(0));
    this->sensor_data.insert("RevSub", int(0));
    this->sensor_data.insert("Type", int(0));
    this->sensor_data.insert("Serial", int(0) );

    this->sensor_data.insert("Name", QString("empty"));
    this->sensor_data.insert("Vendor", QString("empty"));
    this->sensor_data.insert("SupplyVoltage", double(0.0));
    this->sensor_data.insert("MtxPartNumber", QString("empty"));

    if (emits) emit this->signal_sensor_data_qmap_created(this->sensor_data);
}

void measdocxml::create_atswriter(const bool emits)
{

    std::lock_guard<std::recursive_mutex> lock(this->mutex);


    // <ATSWriter>
    // <configuration>
    this->atswriter_configuration.insert("survey_header_name", QString("empty"));
    this->atswriter_configuration.insert("meas_type", QString("empty"));
    this->atswriter_configuration.insert("powerline_freq1", double(0.0));
    this->atswriter_configuration.insert("powerline_freq2", double(0.0));

    if (emits) emit this->signal_atswriter_configuration_qmap_created(atswriter_configuration);

    // channel id =  ... n times repeated
    this->atswriter_data.insert("pos_x1", double(0.0));
    this->atswriter_data.insert("pos_x2", double(0.0));
    this->atswriter_data.insert("pos_y1", double(0.0));
    this->atswriter_data.insert("pos_y2", double(0.0));
    this->atswriter_data.insert("pos_z1", double(0.0));
    this->atswriter_data.insert("pos_z2", double(0.0));
    this->atswriter_data.insert("ats_data_file", QString("empty"));
    this->atswriter_data.insert("sensor_cal_file", QString("empty"));
    this->atswriter_data.insert("ts_lsb", double(0.0));
    this->atswriter_data.insert("num_samples", (quint64) 0);
    this->atswriter_data.insert("start_time", QString("empty"));
    this->atswriter_data.insert("start_date", QString("empty"));
    this->atswriter_data.insert("channel_type", QString("empty"));
    this->atswriter_data.insert("sensor_type", QString("empty"));
    this->atswriter_data.insert("sample_freq", double(0.0));
    this->atswriter_data.insert("sensor_sernum", int(0));

    if (emits) emit this->signal_atswriter_data_qmap_created(this->atswriter_data);

    // </configuration>
    // <comments>
    this->atswriter_comments.insert("site_name", QString("empty")); // part of atsheader
    this->atswriter_comments.insert("site_name_rr", QString("empty")); // part of atsheader
    this->atswriter_comments.insert("site_name_emap", QString("empty")); // part of atsheader
    this->atswriter_comments.insert("client", QString("empty")); // part of atsheader
    this->atswriter_comments.insert("contractor", QString("empty")); // part of atsheader
    this->atswriter_comments.insert("area", QString("empty"));  // part of atsheader
    this->atswriter_comments.insert("survey_id", QString("empty"));  // part of atsheader
    this->atswriter_comments.insert("operator", QString("empty")); // part of atsheader
    //    // weather string NOT supported anymore
    //    this->atswriter_comments.insert("weather", QString("empty"));
    this->atswriter_comments.insert("general_comments", QString("empty")); // part of atsheader

    if (emits) emit this->signal_atswriter_comments_qmap_created(this->atswriter_comments);

    // </comments>
    // <output_file>
    this->atswriter_output_file.insert("line_num", QString("empty")); // part of XML only
    this->atswriter_output_file.insert("site_num", QString("empty")); // part of XML only
    this->atswriter_output_file.insert("run_num", int(0));  // part of atsheader and FILENAME
    this->atswriter_output_file.insert("ats_file_size", quint64(534773760));
    // </output_file>

    if (emits) emit this->signal_atswriter_output_file_qmap_created(this->atswriter_output_file);

    // </ATSWriter>







}

void measdocxml::create_txm(const bool emits)
{
    this->txm_cycle.insert("BaseFrequency", double(0));
    this->txm_cycle.insert("Iterations", double(0));
    this->txm_cycle.insert("Polarisation_dipole_0", double(0));
    this->txm_cycle.insert("Polarisation_dipole_1", double(0));

    if (emits) {
        emit signal_txm_cycle_qmap_created(this->txm_cycle);
    }

    this->txm_waveform.insert("Name", "DefaultRect");
    this->txm_waveform.insert("MaxAmplitude", double(0));
    this->txm_waveform.insert("NumSamplPoints", int(0));
    this->txm_waveform.insert("SamplePointList", int(0));

    if (emits) {
        emit signal_txm_waveform_qmap_created(this->txm_waveform);
    }

    this->txm_sequence.insert("Name", "DefaultRect");
    this->txm_sequence.insert("ActivateRotDipole", int(0));
    this->txm_sequence.insert("ActivateAltDipole", int(0));
    this->txm_sequence.insert("DipoleRotFreq", double(0));

    if (emits) {
        emit signal_txm_sequence_qmap_created(this->txm_sequence);
    }

    this->txm_epos.insert("E1_Position_0_Longitude", int(0));
    this->txm_epos.insert("E1_Position_0_Latitude", int(0));
    this->txm_epos.insert("E1_Position_0_Elevation", int(0));
    this->txm_epos.insert("E1_Position_1_Longitude", int(0));
    this->txm_epos.insert("E1_Position_1_Latitude", int(0));
    this->txm_epos.insert("E1_Position_1_Elevation", int(0));

    this->txm_epos.insert("E2_Position_0_Longitude", int(0));
    this->txm_epos.insert("E2_Position_0_Latitude", int(0));
    this->txm_epos.insert("E2_Position_0_Elevation", int(0));
    this->txm_epos.insert("E2_Position_1_Longitude", int(0));
    this->txm_epos.insert("E2_Position_1_Latitude", int(0));
    this->txm_epos.insert("E2_Position_1_Elevation", int(0));

    this->txm_epos.insert("E3_Position_0_Longitude", int(0));
    this->txm_epos.insert("E3_Position_0_Latitude", int(0));
    this->txm_epos.insert("E3_Position_0_Elevation", int(0));
    this->txm_epos.insert("E3_Position_1_Longitude", int(0));
    this->txm_epos.insert("E3_Position_1_Latitude", int(0));
    this->txm_epos.insert("E3_Position_1_Elevation", int(0));

    if (emits) {
        emit signal_txm_epos_qmap_created(this->txm_epos);
    }
}

void measdocxml::create_gps_status(const bool emits)
{
    // <HwStatus GMS="8" Type="1" Version="2.0" Name="ADU-08e">


    // <GPS>
    this->gps_status.insert("Date", QString("empty") );
    this->gps_status.insert("Time", QString("empty"));
    this->gps_status.insert("DynamicMode", int(0));
    this->gps_status.insert("Height", int(0));
    this->gps_status.insert("Latitude", int(0));
    this->gps_status.insert("Longitude", int(0));
    this->gps_status.insert("NumSats", int(0));
    this->gps_status.insert("SyncState", int(0));

    this->gps_status.insert("NumSatsGPS", int(0));
    this->gps_status.insert("NumSatsGalileo", int(0));
    this->gps_status.insert("NumSatsBeiDou", int(0));
    this->gps_status.insert("NumSatsGlonass", int(0));




    // </GPS>

    if (emits) this->signal_gps_status_qmap_created(this->gps_status);


}

void measdocxml::create_system_status(const bool emits)
{
    // <HwStatus GMS="8" Type="1" Version="2.0" Name="ADU-08e">
    // <System>
    this->system_status.insert("BattState", int(0));
    this->system_status.insert("BiosDate", QDate(2016, 06, 28));
    this->system_status.insert("BiosTime",  QTime(12, 0, 0, 0));
    this->system_status.insert("CurrBatt1", int(0));
    this->system_status.insert("CurrBatt2", int(0));
    this->system_status.insert("DiskFreeSpace", qint64(0));
    this->system_status.insert("DiskFreeSpaceDB", qint64(0));
    this->system_status.insert("DiskFreeSpaceUSB1", QString("N/A"));
    this->system_status.insert("DiskFreeSpaceUSB2",  QString("N/A"));
    this->system_status.insert("DiskSize", qint64(0));
    this->system_status.insert("DiskSizeDB", qint64(0));
    this->system_status.insert("DiskSizeUSB1",  QString("N/A"));
    this->system_status.insert("DiskSizeUSB2",  QString("N/A"));
    this->system_status.insert("FullySynced", int(0));
    this->system_status.insert("Voltage", int(0));
    this->system_status.insert("LANIP", QString("empty"));
    this->system_status.insert("LANNetmask", QString("empty"));
    this->system_status.insert("TSCopyPercentComplete", QString("empty"));
    this->system_status.insert("TSSDCardMountStatus", QString("-255"));
    this->system_status.insert("TSSDCardFormatStatus", QString("-255"));
    this->system_status.insert("Temperature", int(0));
    this->system_status.insert("WLANIP", QString("empty"));
    this->system_status.insert("WLANNetmask", QString("empty"));

    // </System>

    if (emits) emit this->signal_system_status_qmap_created(this->system_status);


}

void measdocxml::create_recording_status(const bool emits)
{

    // <HwStatus GMS="8" Type="1" Version="2.0" Name="ADU-08e">
    // <Recording>
    this->recording_status.insert("BadDataDetected", int(0));
    this->recording_status.insert("FlushActive", int(0) );
    this->recording_status.insert("NumMeasActive", int(0) );
    this->recording_status.insert("NumMeasRemaining", int(0) );
    this->recording_status.insert("SDRAMActFill", qint64(0) );
    this->recording_status.insert("SDRAMFlushFill", qint64(0));
    this->recording_status.insert("SDRAMMaxFill", qint64(0) );
    this->recording_status.insert("SDRAMOvlFill", qint64(0) );
    this->recording_status.insert("SDRAMStartStopFill", qint64(0) );
    this->recording_status.insert("SecondsToFinishAll", qint64(0));
    this->recording_status.insert("SecondsToNextJob", qint64(-1));
    this->recording_status.insert("SelftestActive", int(0) );
    this->recording_status.insert("BufferSize", qint64(0));
    this->recording_status.insert("DestPath", QString("")  );
    this->recording_status.insert("MeasIndex", int(0) );
    this->recording_status.insert("NumBuffers", qint64(0) );
    this->recording_status.insert("PercentComplete", double(0.0));
    this->recording_status.insert("SampleFreq", double(0.0));
    this->recording_status.insert("SecondsToGo", qint64(0) );
    this->recording_status.insert("UsedChannels", QString("")  );


    // </Recording>

    if (emits) emit this->signal_recording_status_qmap_created(this->recording_status);

}

void measdocxml::create_global_config(const bool emits)
{
    // global_config - sub selection only
    this->global_config.insert("calon", int(0));
    this->global_config.insert("atton", int(0));
    this->global_config.insert("calref", int(0));
    this->global_config.insert("calint", int(0));
    this->global_config.insert("calfreq", double(0));
    this->global_config.insert("<short_circuit", int(0));
    if (emits) emit this->signal_global_config_qmap_created(this->global_config);
}

void measdocxml::create_adbboard_autogain(const bool emits)
{

    this->adbboard_autogain.insert("input", int(0));
    this->adbboard_autogain.insert("input_circuitry", QString("empty"));
    this->adbboard_autogain.insert("filter_type_main", QString("empty"));
    this->adbboard_autogain.insert("gain_stage1", double(0.0));
    this->adbboard_autogain.insert("gain_stage2", double(0.0));
    this->adbboard_autogain.insert("filter_type", QString("empty"));
    this->adbboard_autogain.insert("dac_val", double(0.0));
    this->adbboard_autogain.insert("dac_on", int(0));

    if (emits) emit this->signal_adbboard_autogain_qmap_created(this->adbboard_autogain);


}

QString measdocxml::filename_from_atswriter_section(const QMap<QString, QVariant> &atswriter_section)
{
    if (atswriter_section.value("ats_data_file").toString() == this->qfi.fileName()) return this->qfi.fileName();
    QStringList file = atswriter_section.value("ats_data_file").toString().split("_");
    //if (file.size() > )

    return QString("empty");

}

QMap<QString, QVariant> measdocxml::get_qmap(const QString which)
{
    if (which == "recording_status") {
        if (!this->recording_status.size()) this->create_recording_status(false);
        return this->recording_status;
    }

    else if (which == "system_status") {
        if (!this->system_status.size()) this->create_system_status(false);
        return this->system_status;
    }

    else if (which == "gps_status") {
        if (!this->gps_status.size()) this->create_gps_status(false);
        return this->gps_status;
    }

    else if (which == "atswriter_comments") {
        if (!this->atswriter_comments.size()) this->create_atswriter(false);
        return this->atswriter_comments;
    }

    else if (which == "atswriter_configuration") {
        if (!this->atswriter_configuration.size()) this->create_atswriter(false);
        return this->atswriter_configuration;
    }

    else if (which == "atswriter_output_file") {
        if (!this->atswriter_output_file.size()) this->create_atswriter(false);
        return this->atswriter_output_file;
    }

    else if (which == "atswriter_data") {
        if (!this->atswriter_data.size()) this->create_atswriter(false);
        return this->atswriter_data;
    }



    else if (which == "sensor_data") {
        if (!this->sensor_data.size()) this->create_sensor(false);
        return this->sensor_data;
    }

    else if (which == "adbboard_data") {
        if (!this->adbboard_data.size()) this->create_adbboard(false);
        return this->adbboard_data;
    }

    else if (which == "global_config") {
        if (!this->global_config.size()) this->create_global_config(false);
        return this->global_config;
    }

    QMap<QString, QVariant> x;
    return x;


}

void measdocxml::get_hw_status(const bool emits)
{

    if (this->root == nullptr) return;
    this->create_gps_status(false);
    if (!this->get_section("HwStatus", "GPS", this->gps_status, true )) {
        if (!this->get_section("ADU07HwStatus", "GPS", this->gps_status, true )) {
            this->get_section("TXM22HwStatus", "GPS", this->gps_status, true );
        }
    }
    this->create_recording_status(false);
    if (!this->get_recording_section("HwStatus", "Recording", this->recording_status, true)){ // TODO: @bfr please review, the meas information was missing via get_section
        this->get_recording_section("ADU07HwStatus", "Recording", this->recording_status, true);
    }
    //    this->get_section("HwStatus", "Recording", this->recording_status, true );
    this->create_system_status(false);
    if (!this->get_section("HwStatus", "System", this->system_status, true ) ) {
        this->get_section("ADU07HwStatus", "System", this->system_status, true ) ;
    }

    if (emits) {
        emit this->signal_gps_status_qmap_created(this->gps_status);
        emit this->signal_recording_status_qmap_created(this->recording_status);
        emit this->signal_system_status_qmap_created(this->system_status);

    }

}

void measdocxml::slot_update_atswriter(const QMap<QString, QVariant> &atswriter_section, const int &channel_id, const QString &measdir, const QString &measdoc)
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);


    this->qfi.setFile(QDir(measdir), measdoc);
    std::cerr << "measdocxml::slot_update_atswriter: " << atswriter_section.value("ats_data_file").toString().toStdString() << "   " << qfi.absoluteFilePath().toStdString() << std::endl;
    std::cerr << " --------------" << std::endl;
    std::cerr << "measdocxml::slot_update_atswriter-->: " << atswriter_section.value("sensor_type").toString().toStdString() << "   " << qfi.absoluteFilePath().toStdString() << std::endl;
    qDebug() << "update writer section for channel" << channel_id;
    auto atsw(atswriter_section);
    bool exchanged = false;
    if (atsw.contains("sensor_type")) {
        if (atsw.value("sensor_type").toString() == "SHFT03") {
            atsw["sensor_type"] = "SHFT03e";
            exchanged = true;
        }
        if (atsw.value("sensor_type").toString() == "SHFT2e") {
            atsw["sensor_type"] = "SHFT-02e";
            exchanged = true;
        }
        if (exchanged) this->set_channel_section("ATSWriter", atsw, channel_id, true);
    }
    if (!exchanged) this->set_channel_section("ATSWriter", atswriter_section, channel_id, true);



}
