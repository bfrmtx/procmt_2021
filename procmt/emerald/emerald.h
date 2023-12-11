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

#ifndef EMERALD_H
#define EMERALD_H

#include "emerald_global.h"
#include "procmt_alldefines.h"
#include "threadbuffer.h"
#include "atsheader.h"
#include "atsfile.h"
#include <algorithm>

#include <QDataStream>
#include <QTextStream>
#include <QFile>
#include <QFileInfo>
#include <QDate>
#include <QObject>
#include <thread>
#include <cfloat>
#include <vector>
#include <QPair>
#include <QMap>
#include <QVariant>

#include "vector_utils.h"

#include <string>
#include <iomanip>

/*!
  @todo cross check raw and xtr header; add XML file from adulib!
*/

/*!
 * \brief The emerald_header class
 * the header has a varibale length
 * header entries (general header, event header) are filled with "_"
 * first we read to file pos 100 (more! that the general header) and continue to read untile the last "_" is reached
 * after that the data section begins
 * The header is ASCII seperated by " " - so splitting the strings is ok
 *
 */
class emerald_header {

public:

    emerald_header();

    void buffer_to_map(const QString &buffer);

    QMap<QString, QVariant> header;

    QStringList items;

    eQDateTime edt_start;
    eQDateTime edt_stop;

};


// class compstart_lhs_lt;

class emerald :  public QObject, public QFileInfo, public prc_com
{
    Q_OBJECT
public:
    // should be C8 (200) and OR 192
    /*!
     * \brief emerald create a emerald raw
     * \param qfi
     * \param skip_bytes -1 = auto (look for _ ending after byte 180, else 192 (C0, 4chan), or 200 (C8, 5chan)
     * \param parent
     */
    explicit emerald(const QFileInfo &qfi, const int &skip_bytes = 0, QObject *parent = 0);

    /*!
     * \brief open opens and skips the ascii header
     * \param close_again
     * \return
     */
    size_t open(bool close_again = false);

    size_t check_first(const size_t &nsamples);

    size_t to_textfile(const QFileInfo &name_qfi);

    bool set_survey_basedir(const QDir &basedir, const QString &site_str, const bool create = true);

    // angle 90 = EAST
    bool dip_to_pos(const double length, const double decangle );

    void set_collect_only(const bool coll);

    eQDateTime get_start() const;
    eQDateTime get_stop() const;

    bool set_data_write_append(const std::vector<std::vector<double> > &xdata_in, const QMap<QString, int> &xchannel_map);
    void write_ats();
    void delete_data();


    std::vector<std::vector<double>> get_data() const;
    QMap<QString, int> get_channel_map() const;



    void set_guessed_samples(const quint64 guessed_samples);

    quint64 get_guessed_samples() const;

    void set_sernum(const int sern);

    bool friend compstart_lhs_lt(const std::unique_ptr<emerald>& lhs, const std::unique_ptr<emerald>& rhs);


public slots:

    //void slot_emerald_channel_data_qmap_created(const QMap<QString, QVariant> &data_map);
    void slot_got_atswriter_channel(const QMap<QString, QVariant> &data_map, const int id);

    void slot_got_emerald_emerald(const QMap<QString, QVariant> &data_map);


    void slot_convert(const quint64 new_guessed_samples = 0);


signals:

    void signal_start_str(const QString &starts_str);

    void signal_detected_bytes(const int bytes);


private:


    emerald_header emh;

    QFile file;
    QDataStream qds;

    eQDateTime edt_start;
    eQDateTime edt_stop;


    QTextStream qts;
    QFile textfile;

    QList<QMap<QString, QVariant>> emerald_channels;
    QMap<QString, QVariant> emerald_site;

    std::vector<std::shared_ptr<atsfile>> atsfiles;
    std::vector<std::vector<double>> data_in;
    QMap<QString, int> channel_map;
    QMap<int, int> remap;
    bool collect_only = false;

    quint64 guessed_samples = 0;
    int sernum = 1;


    QDir basedir;
    QDir sdir; // my site dir

    int nchan = 0;

    int skip_bytes = 0;

    // pos
    double x1, x2, y1, y2, z1 = 0.0, z2 =0.0;




};


bool compstart_lhs_lt(const std::unique_ptr<emerald>& lhs, const std::unique_ptr<emerald>& rhs);




#endif // EMERALD_H
