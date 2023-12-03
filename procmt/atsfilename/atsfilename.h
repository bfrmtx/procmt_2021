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

#ifndef ATSFILENAME_H
#define ATSFILENAME_H

#include "atsfilename_global.h"
#include <QObject>
#include "atsfilename_global.h"
#include <QFileInfo>
#include "atsfilename_global.h"
#include <QString>
#include "atsfilename_global.h"
#include <memory>
#include "atsfilename_global.h"
#include "prc_com.h"
#include "atsfilename_global.h"
#include "atsheader.h"
#include "atsfilename_global.h"
#include "atsheader_80_def.h"


class atsfilename : public QObject,  public prc_com
{
    Q_OBJECT

public:
    atsfilename(const QFileInfo &qfi, QObject *parent = Q_NULLPTR);

    atsfilename(QObject *parent = Q_NULLPTR);

    void dir_split(const QFileInfo &qfidir, QFileInfo &basedir, QFileInfo &sitedir, QFileInfo &measdir);

    /*!
     * \brief init_name
     * // metronix
     *  008_V01_C00_R000_TEx_BL_32H.ATS
     *  CEA nn: ns, ca, cp,
     *  21007_20130322_nn_1024H_Ex.ats
     *  21007_20130322_nn_1024s_Ex.ats

     */

    /*!
     * \brief clear_fname initializes the data
     */
    void clear_fname();
    bool parse_fname(const bool read_atsheader = false);


    /*!
     * \brief inc_run increment the run number (if increment = 0)
     * \param increment positive or negative number in case
     * \return new run number, INT_MAX if fails
     */
    int inc_run(const int increment = 0);


    void set_suffix(const QString &suffix);

    QString get_filename(const QString type = "07");

    void set_sampleFreq(const double &dsample, const bool check = true);

    /*!
     * \brief skip_samples_from_filter add skip_samples in that way that the filtered time series starts at a full second again
     * example: 128Hz, 32x filter, 471 coefficients = 235 half length, we start at sample 20 (20 + 235 = 255 = 2nd second)<br>
     * the filter is part of the class and can be accessed by this function
     */

    void skip_samples_from_filter();

public slots:

    void slot_set_atsfilename(const QString &filename);
    void slot_set_filename_for_measdoc_from_ats(const QString &filename, const QString &start_datetime, const QString &stop_datetime, const QString &absolute_measdir);

signals:

    void signal_measdoc_name_created(const QString &measdoc_filename, const QString &measdir);


private:
    std::unique_ptr<atsheader> atsh;
    int sample_freq_h;
    int sample_freq_s;
    QChar fill;
    QString suffix = "ats";

    bool is_init = false;

    bool emit_extra = false;                        //!< emit some extra signals; some require math and false avoids superfluous time

    std::int16_t  siHeaderVers = 80;
    QString t_name;
    QFileInfo atsfile;
    QString start_datetime;
    QString stop_datetime;
    QString measdoc_name;
    QString absolute_measdir;


    /*!
     * \brief channels_06_from_07 creates 06 channel names from 07 files
     */
    void channels_06_from_07();

    /*!
     * \brief channels_07_from_06 create channel names from 06 files
     */
    void channels_07_from_06();

    void bandstr_06_from_samplefreq();

    void channel_type_07_to_06();

    void channel_type_06_to_07();


};

#endif // ATSFILENAME_H
