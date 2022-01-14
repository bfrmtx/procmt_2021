#include "xmlreader.h"
#include <iostream>

/*
 * Small introduction why we use QXmlStreamReader here:
 * In some future we wanna read files from the internet. To support this its easier
 * to use a stream-based xml reader to maybe directly access files from a possible
 * network-stream. Otherwise we would use a dom-based xml reader.
 */

TS_NAMESPACE_BEGIN

QString const elem_root                             = "measurement";
QString const elem_rec                              = "recording";
QString const elem_rec_in                           = "input";
QString const elem_rec_out                          = "output";
QString const elem_rec_out_pt                       = "ProcessingTree";
QString const elem_rec_out_pt_cfg                   = "configuration";
QString const elem_rec_out_pt_out                   = "output";
QString const elem_rec_out_pt_out_atsw              = "ATSWriter";
QString const elem_rec_out_pt_out_atsw_cfg          = "configuration";
QString const elem_rec_out_pt_out_atsw_cfg_info     = "information";

QString const elem_rec_channel                      = "channel";
QString const elem_rec_channel_type                 = "channel_type";
QString const elem_rec_channel_id                   = "id";
QString const elem_rec_channel_atsfile              = "ats_data_file";
QString const elem_rec_channel_samplecount          = "num_samples";
QString const elem_rec_channel_freq                 = "sample_freq";

#define XML_TS_DEBUGOUTPUT

#define XML_TS_READER_HEAD \
    while(true) { \
        auto token_type = reader.readNext(); \
        auto token_name = reader.name(); \
        if(token_type == QXmlStreamReader::StartElement) {

#define XML_TS_READER_TAIL(end_element_name) \
        } \
        if(token_type == QXmlStreamReader::EndDocument || \
          (token_type == QXmlStreamReader::EndElement && token_name == end_element_name)) { \
            break; \
        } \
    }




void read_channel(measurement_xml_data & data, QXmlStreamReader & reader) {
    measurement_output_channel new_channel;

    if(reader.attributes().hasAttribute(elem_rec_channel_id)) {
        new_channel.id = reader.attributes().value(elem_rec_channel_id).toString().toStdString();
    }

    XML_TS_READER_HEAD
    if(token_name == elem_rec_channel_freq) {
        new_channel.frequency = reader.readElementText().toDouble();
    } else if(token_name == elem_rec_channel_atsfile) {
        new_channel.filename = reader.readElementText().toStdString();
    } else if(token_name == elem_rec_channel_samplecount) {
        new_channel.samplecount = reader.readElementText().toLongLong();
    } else if(token_name == elem_rec_channel_type) {
        new_channel.channeltype = reader.readElementText().toStdString();
    }
    XML_TS_READER_TAIL(elem_rec_channel)

    data.channel_data.push_back(new_channel);
}

void read_atswriter_cfg(measurement_xml_data & data, QXmlStreamReader & reader) {
    XML_TS_READER_HEAD
        if(token_name == elem_rec_channel) {
            read_channel(data, reader);
        }
    XML_TS_READER_TAIL(elem_rec_out_pt_out_atsw_cfg)
}

void read_processing_tree_atswriter(measurement_xml_data & data, QXmlStreamReader & reader) {
    XML_TS_READER_HEAD
        if(token_name == elem_rec_out_pt_out_atsw_cfg) {
            read_atswriter_cfg(data, reader);
        }
    XML_TS_READER_TAIL(elem_rec_out_pt_out_atsw)
}

void read_processing_tree_output(measurement_xml_data & data, QXmlStreamReader & reader) {
    XML_TS_READER_HEAD
        if(token_name == elem_rec_out_pt_out_atsw) {
            read_atswriter_cfg(data, reader);
        }
    XML_TS_READER_TAIL(elem_rec_out_pt_out)
}

void read_processing_tree(measurement_xml_data & data, QXmlStreamReader & reader) {
    XML_TS_READER_HEAD
        if(token_name == elem_rec_out_pt_out) {
            read_processing_tree_output(data, reader);
        }
    XML_TS_READER_TAIL(elem_rec_out_pt)
}

void read_record_output(measurement_xml_data & data, QXmlStreamReader & reader) {
    XML_TS_READER_HEAD
        if(token_name.mid(0, elem_rec_out_pt.size()) == elem_rec_out_pt) {
            read_processing_tree(data, reader);
        }
    XML_TS_READER_TAIL(elem_rec_out)
}

void read_record(measurement_xml_data & data, QXmlStreamReader & reader) {
    XML_TS_READER_HEAD
        if(token_name == elem_rec_out) {
            read_record_output(data, reader);
        }
    XML_TS_READER_TAIL(elem_rec)
}

void read_measurement(measurement_xml_data & data, QXmlStreamReader & reader) {
    XML_TS_READER_HEAD
        if(token_name == elem_rec) {
            read_record(data, reader);
        }
    XML_TS_READER_TAIL(elem_root)
}

measurement_xml_data read_xml_file(const QString & filename) {
    measurement_xml_data ret;

    QFileInfo fInfo(filename);
    if(fInfo.exists()) {
        ret.filepath = fInfo.path().toStdString();
        QFile file(filename);
        if(file.open(QFile::ReadOnly)) {
            QXmlStreamReader reader(&file);

            while(true) {
                auto token_type = reader.readNext();
                auto token_name = reader.name().toString();

                if(token_type == QXmlStreamReader::StartElement) {
                    if(token_name == elem_root) {
                        read_measurement(ret, reader);
                    }
                }
                if(token_type == QXmlStreamReader::EndDocument) {
                    break;
                }
            }
        }
    }

    return ret;
}

TS_NAMESPACE_END

std::ostream &operator<<(std::ostream & os, const tsplot::measurement_output_channel & data) {
    os << "<channel-info>" << std::endl;
    os << "id      : " << data.id << std::endl;
    os << "type    : " << data.channeltype << std::endl;
    os << "file    : " << data.filename << std::endl;
    os << "samples : " << data.samplecount << std::endl;
    os << "freq    : " << data.frequency << std::endl;
    return os;
}

#undef XML_TS_READER_HEAD
#undef XML_TS_READER_TAIL
