#include "emerald_xtrx.h"




emerald_xtrx::emerald_xtrx(const QFileInfo *qfi, const QUrl *qurl, const QByteArray *qba, QObject *parent)
    :measdocxml(qfi, qurl, qba, parent)
{

}

//emerald_xtrx::emerald_xtrx(QObject *parent) : measdocxml(parent)
//{

//}

emerald_xtrx::~emerald_xtrx()
{
   this->doc.Clear();
}

void emerald_xtrx::get_emerald_channel_section()
{
    if(!this->root) return;

    QString topnode = "Site";

    if (this->goto_sub_section(topnode, "Channel", 1) != 1)  {
        return;
    }

    tinyxml2::XMLElement *xmlelement = this->subsection;
    if (!xmlelement) return;

    do {
        int id = -1;
        this->create_channel(false);
        xmlelement->QueryIntAttribute( "IndexInFile", &id );

        if (id >= 0) {
            qDebug() << " : " << xmlelement->Value() << id;

            this->get_siblings_data(xmlelement->FirstChildElement(), this->emerald_Channel_data, false);
            // transport the id
            this->emerald_Channel_data.insert("IndexInFile",id);
            emit this->signal_got_atswriter_channel(this->emerald_Channel_data, id);
        }
        xmlelement = xmlelement->NextSiblingElement();
        id = -1;

    } while (xmlelement);



    return ;
}

QString emerald_xtrx::get_emeral_emerald_section()
{

    QString topnode = "EmeraldData";
    this->create_emerald(false);;
    this->get_top_section(topnode, this->emerald_EmeraldData_data, false);

    emit this->signal_got_emerald_emerald(this->emerald_EmeraldData_data);

    return this->emerald_EmeraldData_data.value("DataFilename").toString();


}


void emerald_xtrx::create_channel(const bool emits)
{

    /*
<Type>Bx</Type>
<DataUnit>Volt</DataUnit>
<Gain>1.000000</Gain>
<DCOffset>0.000000</DCOffset>
<HorizontalOrientation Unit="deg">0.000000</HorizontalOrientation>
<VerticalOrientation Unit="deg">0.000000</VerticalOrientation>
<ModuleResponse Type="INFO">SPAM_MkIV_55XXXXX-TYPE-6.25HF-ID-000027.RSPX</ModuleResponse>
<ModuleResponse Type="INFO">SPAM_SensorBox----TYPE-001_V2-ID-127000.RSPX</ModuleResponse>
<ModuleResponse Type="RESP">Metronix_Coil-----TYPE-006_LF-ID-000412.RSPX</ModuleResponse>
*/

    // n times repeated
    this->emerald_Channel_data.insert("Type", QString("empty"));
    this->emerald_Channel_data.insert("DataUnit", QString("empty"));
    this->emerald_Channel_data.insert("Gain", double(1.0));
    this->emerald_Channel_data.insert("DCOffset", double(1.0));
    this->emerald_Channel_data.insert("HorizontalOrientation", double(0.0));
    this->emerald_Channel_data.insert("VerticalOrientation", double(0.0));
    this->emerald_Channel_data.insert("ModuleResponse", QString("empty"));

    this->emerald_Channel_data.insert("ContactResistancePos", double(0.0));
    this->emerald_Channel_data.insert("ContactResistanceNeg", double(0.0));
    this->emerald_Channel_data.insert("ContactResistanceGnd", double(0.0));
    this->emerald_Channel_data.insert("DipoleLength", double(0.0));
    // emerald starts at one
    this->emerald_Channel_data.insert("IndexInFile", int(0));




    if (emits) emit this->signal_emerald_channel_data_qmap_created(this->emerald_Channel_data);




}

void emerald_xtrx::create_emerald(const bool emits)
{
    this->emerald_EmeraldData_data.insert("XtrxVersion", QString("empty"));
    this->emerald_EmeraldData_data.insert("ProjectName", QString("empty"));
    this->emerald_EmeraldData_data.insert("Lowpass", double(0.0));
    this->emerald_EmeraldData_data.insert("Highpass", double(0.0));
    this->emerald_EmeraldData_data.insert("SampleRate", double(0.0));

    this->emerald_EmeraldData_data.insert("StartTime", QString("empty"));
    this->emerald_EmeraldData_data.insert("StopTime", QString("empty"));
    this->emerald_EmeraldData_data.insert("DataEncoding", QString("empty"));
    this->emerald_EmeraldData_data.insert("DataFilename", QString("empty"));

    if (emits) emit this->signal_emerald_emerald_data_qmap_created(this->emerald_EmeraldData_data);



}

void emerald_xtrx::create_site(const bool emits)
{
    if (emits) qDebug() << "nothing done";
}

