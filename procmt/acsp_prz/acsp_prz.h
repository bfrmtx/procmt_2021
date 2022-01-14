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

#ifndef ACSP_PRZ_H
#define ACSP_PRZ_H


#include <QObject>
#include "procmt_alldefines.h"
#include "buckets.h"
#include "threadbuffer.h"
#include "prc_com.h"
#include "atsheader.h"
#include "atsfile.h"
#include "ats_ascii_file.h"
#include "vector_utils.h"
#include "atsfile_collector.h"
#include <memory>
#include <future>

class acsp_prz: public QObject, public prc_com
{

    Q_OBJECT

public:
    acsp_prz(std::shared_ptr<all_spectra_collector<std::complex<double>>>  &all_acspc_collector, QObject *parent = Q_NULLPTR);

    ~acsp_prz();
    void acsp(const size_t le_max_enum_spectra = SIZE_MAX);

    bool set_frequencies_and_target_indices(const std::vector<double> &fft, const std::vector<double> &ft_target, const std::vector<size_t> &selindices, const std::vector<size_t> &selindices_sizes);


    std::vector<std::shared_ptr<atsfile_collector>> out_atsfile_collectors; //!< contains channel_single_spc_collector_buff as spc

    std::vector<std::vector<std::complex<double>>> data;
    std::vector<std::vector<std::vector<std::complex<double>>>> in;

    bool set_parzen_vectors(std::shared_ptr<std::vector<std::vector<double>>> parzen_vectors_sptr, const std::vector<double> &f, const std::vector<double> &ft);

public slots:


    void set_data_flush_out_atsfile_collectors(std::vector<std::shared_ptr<atsfile_collector>> &out_atsfile_collectors);

private:

    void fold_prz(const size_t lhs, const size_t rhs, const size_t ac_pos);

    void fold_lines(const size_t lhs, const size_t rhs, const size_t ac_pos);

    void fold_lines_tilt(const size_t lhs, const size_t rhs, const size_t ac_pos);


    void tilt_e_lines(const size_t idx);


    std::shared_ptr<std::vector<std::vector<double>>> parzen_vectors_sptr = nullptr; //!< already generated parzen vectors from mc_data
    std::vector<double> freqs;
    std::vector<size_t> indices;
    std::vector<size_t> selindices_sizes;


    std::vector<double> target_freqs;
    bool select_from_target_freqs = false;              //!< discrete frequencies like CSAMT or without parzening

    std::shared_ptr<all_spectra_collector<std::complex<double>>>  all_acspc_collector; //!< store the result outside!

    int take_prz_lines_csamt = 0;                       //!< 0 nothing, 1 parzen, 2 lines, 3 csamt



};

#endif // ACSP_PRZ_H
