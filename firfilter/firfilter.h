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

#ifndef FIRFILTER_H
#define FIRFILTER_H

#include <vector>
#include <QMap>
#include <QVariant>
#include <QObject>

#include "prc_com.h"
#include "iterator_templates.h"
#include "memory"



class firfilter : public QObject, public prc_com
{
    Q_OBJECT

public:
    explicit firfilter(QObject *parent = 0);

    explicit firfilter(const firfilter &fil, QObject *parent = 0);

    ~firfilter();

    void copy_data(firfilter *fil);


    firfilter& operator = (firfilter const &fil);

    friend bool operator == (firfilter &lhs, firfilter const &rhs) {
        return (lhs.value("filter_name") == rhs.value("filter_name"));
    }
    friend bool operator != (firfilter &lhs, firfilter const &rhs) {
        return (lhs.value("filter_name") != rhs.value("filter_name"));
    }

    void clear();

    void clear_coefficients();

    /*!
     * \brief fir_fold fold the data with the filter coefficients and return the result
     * \param tsdata_in
     * \return
     */
    double fir_fold(const std::vector<double> &tsdata_in);

    /*!
     * \brief set_filter parse the settings from the command line or mcd
     * \param filter_part
     * \return
     */
    bool set_firfilter(const prc_com &filter_part);

public slots:

    /*!
     * \brief generate_coefficients
     * \param filter_name - one of the pre-defined:  mtx32 mtx8 mtx4 mtx2  mtx25 mtx10 notch highpass lowpass bandpass
     * \param success check in case - especially when no coeff are returned
     * \param create_coeff_vector false is you only want create and check the parameters
     * \param sample_freq at least needed for band- and notch filters
     * \param num_samples if samples to small we can not filter
     * \return a FIR fil vector to be folded or zero size vector in case
     */

    std::vector<double> generate_coefficients(const QString &filter_name,  bool &success, const bool create_coeff_vector = true,
                                              const double sample_freq = 0, const quint64 num_samples = 0);




private:

    std::vector<double> coeff;
    QStringList strs_opts;
};

#endif // FIRFILTER_H
