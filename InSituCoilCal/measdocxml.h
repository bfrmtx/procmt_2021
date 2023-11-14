#ifndef MEASDOCXML_H
#define MEASDOCXML_H

#include "QString"

class MeasDocXml
{
    public:
        MeasDocXml(const QString qstrTargetFileName);

        bool isOpen (void) const;

        double getCalFreq (void) const;

    private:
        bool bIsOpen;

        QString qstrTargetFile;

        QString qstrXML;

};

#endif // MEASDOCXML_H
