#include "measdocxml.h"
#include <QFile>
#include <QString>
#include <QDebug>

MeasDocXml::MeasDocXml(const QString qstrTargetFileName)
{
    this->bIsOpen = false;

    this->qstrTargetFile = qstrTargetFileName;

    if (QFile::exists (qstrTargetFileName) == true)
    {
        QFile qfXMLFile;
        qfXMLFile.setFileName (this->qstrTargetFile);
        if (qfXMLFile.open (QIODevice::ReadOnly) == true)
        {
            this->qstrXML.clear  ();
            this->qstrXML.append (qfXMLFile.readAll ().data ());
            this->bIsOpen = true;
        }
    }
}


bool MeasDocXml::isOpen (void) const
{
    return (this->bIsOpen);
}


double MeasDocXml::getCalFreq (void) const
{
    double  dRetValue = 1.0;
    QString qstrTemp;

    if (this->isOpen () == true)
    {
        qstrTemp  = this->qstrXML.left (this->qstrXML.indexOf ("</calfreq>"));
        qstrTemp  = qstrTemp.right     (qstrTemp.size () - qstrTemp.indexOf ("<calfreq>") - strlen ("<calfreq>"));
        qstrTemp  = qstrTemp.trimmed   ();
        dRetValue = qstrTemp.toDouble  ();
    }


    return (dRetValue);
}
