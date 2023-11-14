/**
 * \file utils.h
 *
 * \brief This file contains the definition of several utility functions
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

#ifndef UTILS_H
#define UTILS_H


// includes
#include <qthread.h>
#include <QDebug>
#include "mainwindow.h"



/**
 * \brief This function is used to check a polarisation angle for its valid value.
 *
 * All polarisations for the TXM-22 output dipole may only be entered in the range
 * of -179 .. 0 .. +180 degrees. This function checks the input polarisation angle
 * and limits its to the range mentioned above.
 *
 * @param[in] const QString& qstrAngle = angle to be checked as string value
 * @param[out] QString = limited output angle
 *
 * \date 21.05.2012
 * \author MWI
 */
extern QString checkPolarisationAngle (const QString& qstrAngle);


/**
 * \brief This function is used to normalise a string.
 *
 * This function will normalise a string. It will remove spaces from beginning and end,
 * replace all interior spaces with "_" (underscore) and additionally check, if all used
 * characters are part of the ASCII charset.
 *
 * @param[in] const QString qstrInputString = input string
 * @param[in] QString& qstrOutputString = reference to string that shall contain the normalised string
 * @param[out] bool = true: string was converted / false: string could not be converted
 *
 * \date 05.07.2012
 * \author MWI
 */
extern bool normaliseString (const QString qstrInputString, QString& qstrOutputString);


/**
 * \brief This function is used to check a Date as String for correctness.
 *
 * This function is used to check, if a date, that was entered by the user as string in the
 * format YYYY-MM-DD is correct. If not, the return value will be set to 1970-01-01.
 *
 * @param[in] const QString qstrInputString = input string
 * @param[in] QString& qstrOutputString = reference to string that shall contain the normalised string
 * @param[out] bool = true: string was converted / false: string could not be converted
 *
 * \date 06.07.2012
 * \author MWI
 */
extern bool checkDateString (const QString qstrInputString, QString& qstrOutputString);


/**
 * \brief This function is used compute the cycle time in seconds from "Cycle" and "Granularity" values.
 *
 * -
 *
 * @param[in] const unsigned int uiCycle = value of the "Cycle" node inside XML job file
 * @param[in] const QString& qstrGranularity = value of "Granularity" node inside XML job file
 * @param[out] const int = cycle time in seconds or < 0 if conversion failed
 *
 * \date 25.03.2014
 * \author MWI
 */
extern int getCycleTimeSeconds (const unsigned int uiCycle, const QString& qstrGranularity);


extern double getRandomNumber(const unsigned int uiMax);


extern QString getFormatedDouble (const double dValue);

extern bool dumpTextFile (const QString qstrTargetFileName, const QVector<double> qvecData);

extern bool dumpTextFile (const QString qstrTargetFileName, const QVector<double> qvecIndex, const QVector<double> qvecData);


/**
 * \class Timer
 *
 * \brief This class contains helper functions for platform independent timers and sleeps.
 *
 * This class implements some helper functions, that provide platform independent
 * (Linux / Windows / ...) support for sleeping and timers.
 *
 * \date 21.05.2012
 * \author MWI
 */
class Timer : public QThread
{
    public:

        /**
         * \brief pauses the calling thread for n seconds
         *
         * -
         *
         * @param[in] const unsigned long& ulSecs = seconds to sleep
         *
         * \date 21.05.2012
         * \author MWI
         */
        static void sleep(const unsigned long& ulSecs)
        {
            QThread::sleep(ulSecs);
        }

        /**
         * \brief pauses the calling thread for n milli seconds
         *
         * -
         *
         * @param[in] const unsigned long& ulMSecs = milli seconds to sleep
         *
         * \date 21.05.2012
         * \author MWI
         */
        static void msleep(const unsigned long& ulMSecs)
        {
            QThread::msleep(ulMSecs);
        }

        /**
         * \brief pauses the calling thread for n micro seconds
         *
         * -
         *
         * @param[in] const unsigned long& ulUSecs = micro seconds to sleep
         *
         * \date 21.05.2012
         * \author MWI
         */
        static void usleep(const unsigned long& ulUSecs)
        {
            QThread::usleep(ulUSecs);
        }
};



#endif // UTILS_H
