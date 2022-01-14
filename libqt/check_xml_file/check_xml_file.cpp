#include "check_xml_file.h"


check_xml_file::check_xml_file(const QString &filename) : QFileInfo(filename)
{

}

check_xml_file::check_xml_file(const QDir &dir, const QString &filename) : QFileInfo(dir, filename)
{
    ;
}

check_xml_file::check_xml_file(const QFileInfo &qfi) : QFileInfo(qfi)
{

}

check_xml_file::~check_xml_file()
{
    if (this->file.isOpen()) this->file.close();
}

bool check_xml_file::open()
{
    this->file.setFileName(this->absoluteFilePath());
    if(!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    qts.setDevice(&file);

    return this->file.isOpen();
}

QString check_xml_file::first_root()
{
    if (this->file.isOpen()) this->open();
    QString line;
    size_t max_tries = 20;
    for (int i = 0; i < max_tries; ++i) {
        line = qts.readLine(1000);
        line = line.simplified();

    }
    return line;
}
