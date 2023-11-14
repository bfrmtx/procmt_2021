#include "atsfile.h"
#include "QFile"
#include "QVector"
#include <QDebug>

ATSFile::ATSFile(const QString qstrTargetFileName)
{
    this->qstrTargetFile = qstrTargetFileName;

    // try to read the header of the file
    this->bFileOpen = this->readHeader ();

    this->resetPos ();
}


bool ATSFile::isOpen (void) const
{
    return (this->bFileOpen);
}


bool ATSFile::readHeader ()
{
    bool bRetValue = true;
    QFile qfATSFile;
    QByteArray qbarTmp;

    if (QFile::exists (this->qstrTargetFile) == false)
    {
        bRetValue = false;
    }
    else
    {
        qfATSFile.setFileName (this->qstrTargetFile);
        if (qfATSFile.open (QIODevice::ReadOnly) == false)
        {
            bRetValue = false;
        }
        else
        {
            qbarTmp = qfATSFile.read (sizeof (C_ATSHeader80));
            this->clATSHeader = *((C_ATSHeader80*)qbarTmp.data ());
        }
    }


    return (bRetValue);
}


QString ATSFile::getSensorType   (void) const
{
    QString qstrRetValue;

    qstrRetValue.clear ();
    qstrRetValue.append (this->clATSHeader.abySensorType [0]);
    qstrRetValue.append (this->clATSHeader.abySensorType [1]);
    qstrRetValue.append (this->clATSHeader.abySensorType [2]);
    qstrRetValue.append (this->clATSHeader.abySensorType [3]);
    qstrRetValue.append (this->clATSHeader.abySensorType [4]);
    qstrRetValue.append (this->clATSHeader.abySensorType [5]);

    return (qstrRetValue);
}


QString ATSFile::getSensorSerial (void) const
{
    QString qstrTemp;

    qstrTemp = QString::number (this->clATSHeader.siSensorSerNum);

    while (qstrTemp.size () < 3)
    {
        qstrTemp.prepend ("0");
    }
    qstrTemp.prepend ("#");

    return (qstrTemp);
}


void ATSFile::resetPos (void)
{
    this->uiActPos = 0;
}


bool ATSFile::getNextTSBuffer (const unsigned int uiNumSamples, QVector<double>& qvecTSData) const
{
    qvecTSData.clear ();
    QFile qfATSFile;
    QByteArray qbarTmp;
    bool bRetValue = true;
    unsigned int uiNumBytesRead;
    unsigned int uiSampleCount;
    int iTemp;

    if (QFile::exists (this->qstrTargetFile) == false)
    {
        bRetValue = false;
    }
    else
    {
        qfATSFile.setFileName (this->qstrTargetFile);
        if (qfATSFile.open (QIODevice::ReadOnly) == false)
        {
            bRetValue = false;
        }
        else
        {
            qfATSFile.seek (sizeof (C_ATSHeader80) + this->uiActPos);
            if (qfATSFile.atEnd () == false)
            {
                qbarTmp.clear  ();
                qbarTmp.resize (uiNumSamples * 4);
                uiNumBytesRead = qfATSFile.read (qbarTmp.data (), uiNumSamples * 4);
                if (uiNumBytesRead != uiNumSamples * 4)
                {
                    bRetValue = false;
                }
                else
                {
                    for (uiSampleCount = 0; uiSampleCount < uiNumSamples; uiSampleCount++)
                    {
                        unsigned int uiChar;
                        unsigned int uiTemp;

                        uiChar  = (unsigned int) qbarTmp.data () [(uiSampleCount * 4) + 3];
                        uiChar &= 0x000000FF;
                        uiTemp  = uiChar;
                        uiTemp  = uiTemp << 8;

                        uiChar  = (unsigned int) qbarTmp.data () [(uiSampleCount * 4) + 2];
                        uiChar &= 0x000000FF;
                        uiTemp |= uiChar;
                        uiTemp  = uiTemp << 8;

                        uiChar  = (unsigned int) qbarTmp.data () [(uiSampleCount * 4) + 1];
                        uiChar &= 0x000000FF;
                        uiTemp |= uiChar;
                        uiTemp  = uiTemp << 8;

                        uiChar  = (unsigned int) qbarTmp.data () [(uiSampleCount * 4) + 0];
                        uiChar &= 0x000000FF;
                        uiTemp |= uiChar;

                        iTemp   = (int) uiTemp;

                        qvecTSData.push_back (iTemp * this->clATSHeader.dblLSBMV);
                    }
                }
            }
        }
    }

    return (bRetValue);
}

