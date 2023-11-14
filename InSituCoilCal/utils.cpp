/**
 * \file utils.cc
 *
 * \brief This file contains the implementation of several utility functions
 *
 * -
 *
 * \author MWI
 * \date 21.05.2012
 * \version 1.0
 *
 * - changed at: -
 * - changed by: -
 * - changes:    -
 *
 */

// includes
#include "utils.h"

#include <QDebug>
#include <QDateTime>
#include "mainwindow.h"
#include <cmath>
#include <QDateTime>
#include <QFile>


// function implementation
QString checkPolarisationAngle (const QString& qstrAngle)
{
    // declaration of variabled
    QString qstrResult;

    // check for valid angle - must be in the range of -179.9 .. 0 .. +180
    // if outside this range, limit to valid value
    if (qstrAngle.toDouble() > 180.0)
    {
        qstrResult = QString("%1").arg(180.0);
    }
    else if (qstrAngle.toDouble() < -179)
    {
        qstrResult = QString("%1").arg(-179);
    }
    else
    {
        qstrResult = qstrAngle;
    }

    return (qstrResult);
}


bool normaliseString (const QString qstrInputString, QString& qstrOutputString)
{
    // declaration of variables
    bool         bRetValue = true;
    unsigned int uiCounter;

    // as first step remove spaces
    qstrOutputString = qstrInputString.toLatin1  ();
    qstrOutputString = qstrOutputString.trimmed ();
    qstrOutputString = qstrOutputString.replace (' ', '_');

    // now check, if all characters are valid ASCII chars
    uiCounter = 0;
    while (uiCounter < qstrOutputString.size())
    {
        // all printable ASCII chars are allowed
        if ((qstrOutputString.toLatin1().at(uiCounter) < 0x21) ||
            (qstrOutputString.toLatin1().at(uiCounter) > 0x7E))
        {
            qstrOutputString.remove(uiCounter, 1);
        }
        else
        {
            uiCounter++;
        }
    }

    qDebug () << "[utils]: normalising string!";
    qDebug () << "[utils]: input string.:" << qstrInputString;
    qDebug () << "[utils]: output string:" << qstrOutputString;

    if (qstrInputString != qstrOutputString)
    {
        bRetValue = false;
    }

    return (bRetValue);
}


bool checkDateString (const QString qstrInputString, QString& qstrOutputString)
{
    // declaration of variables
    bool      bRetValue = true;
    QDateTime clTmpDate;

    qstrOutputString = qstrInputString + " 00:00:00";
    clTmpDate        = QDateTime::fromString (qstrOutputString, "yyyy-MM-dd hh:mm:ss");
    if (clTmpDate.isValid() == false)
    {
        qDebug () << "[utils]: Date/Time is invalid:" << qstrOutputString;
        bRetValue        = false;
        qstrOutputString = "1970-01-01";
    }
    else
    {
        qstrOutputString = qstrInputString;
    }

    return (bRetValue);
}


int getCycleTimeSeconds (const unsigned int uiCycle, const QString& qstrGranularity)
{
    // declarations of variables
    int iRetValue = -1;

    if (qstrGranularity.contains("seconds") == true)
    {
        iRetValue = uiCycle;
    }
    else if (qstrGranularity.contains("minutes") == true)
    {
        iRetValue = uiCycle * 60;
    }
    else if (qstrGranularity.contains("hours") == true)
    {
        iRetValue = uiCycle * 3600;
    }
    else if (qstrGranularity.contains("days") == true)
    {
        iRetValue = uiCycle * 86400;
    }

    return (iRetValue);
}


double getRandomNumber (const unsigned int uiMax)
{
    double dTmp = rand ();

    dTmp /= ((double) RAND_MAX);
    dTmp *= ((double) uiMax);

    return (dTmp);
}


QString getFormatedDouble (const double dValue)
{
    QString qstrOutput;
    QString qstrTmp;
    char arcTemp [1024];

    snprintf (&(arcTemp[0]), 1024, "%.4e", dValue);
    qstrTmp.clear ();
    qstrTmp = &(arcTemp[0]);

    qstrOutput  = qstrTmp.left  (qstrTmp.size () - 5);
    qstrOutput += "E";
    qstrOutput += qstrTmp.at    (qstrTmp.size () - 4);
    qstrOutput += qstrTmp.right (2);

    if (dValue >= 0.0)
    {
        qstrOutput.prepend ("+");
    }

    return (qstrOutput);
}


bool dumpTextFile (const QString qstrTargetFileName, const QVector<double> qvecData)
{
    QFile qfTargetFile;
    bool bRetValue = true;
    unsigned int uiCounter;
    QString qstrTemp1;
    QString qstrTemp2;

    qfTargetFile.setFileName (qstrTargetFileName);
    if (qfTargetFile.open (QIODevice::WriteOnly) == false)
    {
        bRetValue = false;
    }
    else
    {
        qstrTemp1.clear ();
        for (uiCounter = 0; uiCounter < qvecData.size (); uiCounter++)
        {
            qstrTemp2  = QString::number (uiCounter);
            qstrTemp2 += " " + getFormatedDouble (qvecData [uiCounter]) + "\n";

            qstrTemp1 += qstrTemp2;
        }

        qfTargetFile.write (qstrTemp1.toStdString ().c_str (), qstrTemp1.size ());
        qfTargetFile.close ();
    }

    return (bRetValue);
}


bool dumpTextFile (const QString qstrTargetFileName, const QVector<double> qvecIndex, const QVector<double> qvecData)
{
    QFile qfTargetFile;
    bool bRetValue = true;
    unsigned int uiCounter;
    QString qstrTemp1;
    QString qstrTemp2;

    qfTargetFile.setFileName (qstrTargetFileName);
    if (qfTargetFile.open (QIODevice::WriteOnly) == false)
    {
        bRetValue = false;
    }
    else
    {
        qstrTemp1.clear ();
        for (uiCounter = 0; uiCounter < qvecData.size (); uiCounter++)
        {
            qstrTemp2  = getFormatedDouble (qvecIndex [uiCounter]) + " ";
            qstrTemp2 += getFormatedDouble (qvecData  [uiCounter]) + "\n";

            qstrTemp1 += qstrTemp2;
        }

        qfTargetFile.write (qstrTemp1.toStdString ().c_str (), qstrTemp1.size ());
        qfTargetFile.close ();
    }

    return (bRetValue);
}

