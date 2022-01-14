#include "prc_com_xml.h"

prc_com_xml::prc_com_xml(QObject *parent) : QObject(parent){
    this->clear();
}


//prc_com_xml::prc_com_xml(const QFileInfo *qfi, const QUrl *qurl, const QByteArray *qba, QObject *parent) : QObject(parent)
//{
//    this->open(qfi, qurl, qba);
//}

bool prc_com_xml::open(const QFileInfo *qfi, const QUrl *qurl, const QByteArray *qba)
{
    this->mdxml = std::make_unique<measdocxml>(this);
    return this->mdxml->open(qfi, qurl, qba);

}



bool prc_com_xml::create(const QString root_node, const QFileInfo *qfi, const QUrl *qurl, const QByteArray *qba) {
    if ((qfi == nullptr) && (qurl == nullptr) && (qba == nullptr)) {
        std::cerr << " prc_com_xml::create -> failed to initialize class - all nullptr! " << std::endl;
        return false;
    }

    if (qfi != nullptr) {
        this->qfi.setFile(qfi->absoluteFilePath());
        this->root_node = root_node;
        this->tix.clear(false);
        this->tix.header();
        tix.push(root_node);

        return true;
    }

    return false;
}

int prc_com_xml::add_main(const QString node_name, const QMap<QString, QVariant> &map)
{
    if (!node_name.size()) return 0;
    if (this->main_node.size()) this->tix.pop(this->main_node);
    this->main_node = node_name;
    tix.push(node_name);
    this->write_map(this->tix, map);

    return map.size();
}

int prc_com_xml::add_sub(const QString node_name, const QMap<QString, QVariant> &map)
{
    if (!node_name.size()) return 0;
    if (!this->main_node.size()) return 0;
    this->tixsub.clear(false);
    this->tixsub.push(node_name);
    this->write_map(this->tixsub, map);
    this->tixsub.pop(node_name);
    this->tix.insert(this->tixsub.getXmlStr());
    this->tixsub.clear(false);
    return map.size();

}

bool prc_com_xml::close_and_write()
{
    this->tix.pop(this->main_node);
    this->tix.pop(this->root_node);

    QTextStream xml_out;
    QFile file(this->qfi.absoluteFilePath());
    if (file.open(QIODevice::WriteOnly)) {
        xml_out.setDevice(&file);
        xml_out << this->tix.getXmlStr();
        file.close();

        return true;
    }
    return false;
}



void prc_com_xml::clear()
{

    if (this->mdxml != nullptr) {
        this->mdxml->close();
        this->mdxml.reset();
    }
    this->tix.clear(false);
    this->tixsub.clear(false);
}

void prc_com_xml::write_map(tinyxmlwriter &tixml, const QMap<QString, QVariant> &map)
{
    QMap<QString, QVariant>::const_iterator kv = map.constBegin();
    while (kv != map.constEnd()) {
        if (kv.value().metaType().id() == QMetaType::Double) {
            QString attributes("type=\"double\"");
            tixml.qv_sci_element(kv.key(), attributes, kv.value(),6);
        }
        else if (kv.value().metaType().id() == QMetaType::Int) {
            QString attributes("type=\"int\"");
            tixml.qv_int_element(kv.key(), attributes, kv.value());
        }
        else if (kv.value().metaType().id() == QMetaType::UInt) {
            QString attributes("type=\"uint\"");
            tixml.qv_int_element(kv.key(), attributes, kv.value());
        }
        else if (kv.value().metaType().id() == QMetaType::ULongLong) {
            QString attributes("type=\"uint64\"");
            tixml.qv_int_element(kv.key(), attributes, kv.value());
        }
        else if (kv.value().metaType().id() == QMetaType::LongLong) {
            QString attributes("type=\"int64\"");
            tixml.qv_int_element(kv.key(), attributes, kv.value());
        }
        else if (kv.value().metaType().id() == QMetaType::QString) {
            QString attributes("type=\"string\"");
            tixml.qv_int_element(kv.key(), attributes, kv.value());
        }
        else tixml.qv_string_element(kv.key(), kv.value());

        ++kv;
    }
}



QMap<QString, QVariant> prc_com_xml::read(const QString &topnode, const QString &node)
{
    QMap<QString, QVariant> map;
    if (this->mdxml == nullptr) return map;

    this->mdxml->get_section(topnode, node, map, true);


    return map;
}

void prc_com_xml::read_existing(const QString &topnode, const QString &node, QMap<QString, QVariant> &map)
{
    if (this->mdxml == nullptr) return;

    this->mdxml->get_section(topnode, node, map, true);
}
