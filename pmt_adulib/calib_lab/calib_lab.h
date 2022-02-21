/***************************************************************************
**                                                                        **
**  ProcMT and modules                                                    **
**  Copyright (C) 2017-2021 metronix GmbH                                 **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
** Parts of the software are linked against the Open Source Version of Qt **
** Qt source code copy is available under https://www.qt.io               **
** Qt source code modifications are non-proprietary,no modifications made **
** https://www.qt.io/licensing/open-source-lgpl-obligations               **
****************************************************************************
**           Author: metronix geophysics                                  **
**  Website/Contact: www.metronix.de                                      **
**                                                                        **
**                                                                        **
****************************************************************************/

#ifndef CALIB_LAB_H
#define CALIB_LAB_H

#include <QObject>
#include "prc_com.h"
#include "mc_data.h"
#include "math_vector.h"
#include "mt_site.h"
#include "vector_utils.h"
#include "procmt_alldefines.h"
#include "qstring_utilities.h"
#include <iostream>
#include <climits>
#include <cfloat>
#include <memory>
#include "measdocxml.h"
#include "parallel_test_coherency.h"



class calib_lab : public QObject
{
    Q_OBJECT

public:
    calib_lab(QObject *parent = Q_NULLPTR);

    ~calib_lab();

    void set_storage(std::shared_ptr<QMap<QString, parallel_test_coherency>> data);


public slots:

    /*!
     * \brief run
     * \param cmdline_parm, ckecks for this->cmdline->svalue("calib_lab_run") == "parallel_test" or "insitu_calibration"
     * \param qfis
     */
    void run(const prc_com &cmdline_parm, const QList<QFileInfo> &qfis);

    void write_ascii_spectra();           //!< for testing

    void slot_fde_vector_created(const QString &acsp_name, std::vector<double> &f, std::vector<double> &v, std::vector<double> &e);
    void slot_fd_vector_created(const QString &acsp_name, std::vector<double> &f, std::vector<double> &v);
    void slot_input_cal_used(const QString sensortype, const int sernum, const QString channel_type, const std::vector<double> &freqs, const std::vector<double> &ampls, const std::vector<double> &degs, const int chopper);


signals:

    void amplitude_spc(const QString &ac_spectra_name, std::vector<double>& amplitude, std::vector<double>& stddev);
    void coherency(const QString &ac_spectra_name, std::vector<double>& amplitude);
    void noise(const QString &ac_spectra_name, std::vector<double>& amplitude);


    void finished_amplitudes();
    void finished_coherencies();


private:

    std::shared_ptr<msg_logger> msg;                //!< log messages and transport to GUI

    std::unique_ptr<mt_site> mtsite;                //!< does the math
    std::shared_ptr<all_spectra_collector<std::complex<double>>>  all_channel_collector; //!< collects the data

    std::shared_ptr<prc_com> cmdline;               //!< all options will be collected here

    mttype spcx = mttype::nomt;

    void process();

    QList<QFileInfo> qfis;                          //!< all input QFileInfos for PST
    QList<QFileInfo> qfisxml;                       //!< all input QFileInfos for Insitu Calibration


    // all data at the end for transportation
    std::shared_ptr<QMap<QString, parallel_test_coherency>> data;


    double f_sample = DBL_MAX;
    size_t wl = SIZE_MAX;
    int chopper_status = -1;

    mutable std::mutex mutex;


    //bool ckeck_storage();

};

#endif // CALIB_LAB_H
