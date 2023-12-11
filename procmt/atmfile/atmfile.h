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

#ifndef ATMFILE_H
#define ATMFILE_H


// this is early in the project - do not include other libraries

#include "atmfile_global.h"
#include <QObject>
#include "atmheader_80_def.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDataStream>
#include <QBitArray>

/*!
 * \brief The atmfile class for exclusion (true) of data (false = nothing to to, include)
 */
class atmfile : public QObject, public QFileInfo
{
    Q_OBJECT


public:
    atmfile(QObject *parent = Q_NULLPTR);

    /*!
     * \brief atmfile open / create an atm file from an existing ats file
     * \param atsfile_atmfile name of ats file
     * \param parent
     */
    atmfile(const QFileInfo &atsfile_atmfile, QObject *parent = Q_NULLPTR);

    void merge(const atmfile &other, const size_t starts = 0, const size_t stops = 0);

    /*!
     * \brief get_slice create a sub-slice for comparison of segments
     * \param starts_at start sample
     * \param stops_at stop sample
     * \return QBitArray slice
     */
    QBitArray get_slice(const size_t starts_at, const size_t stops_at) const;

    /*!
     * \brief set_slice set the bits for this range (=), the in maybe was set by master |= others
     * \param in master bit array
     * \param starts_at sample start
     * \param stops_at sample stop
     */
    void set_slice(const QBitArray in, const size_t starts_at, const size_t stops_at);


    /*!
     * \brief set_skip_slice
     * \param starts_at first sample for exclusion
     * \param stops_at last sample for exclusion
     */
    void set_skip_slice(const size_t starts_at, const size_t stops_at);

    ~atmfile();

    /*!
     * \brief resize resize if needed - shoulb not happen because earlier selections may be destroyed
     * \param size
     * \return
     */
    size_t resize(const size_t &size);

    /*!
     * \brief free_atm_window if skip marked parts is active, find next window without skips
     * \return
     */
    size_t free_atm_window(const size_t &sample_start, const size_t &window_length);

    void fir_filter(const atmfile &in, const size_t starts_at, const size_t filter_factor);

    size_t read_all();
    size_t write_all();

    QBitArray b; // that is either a single or merged bool

private:

    QFile file;
    atmheader atm;
};

#endif // ATMFILE_H
