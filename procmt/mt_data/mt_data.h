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

#ifndef MT_DATA_H
#define MT_DATA_H

#include "mt_data_global.h"
#include <complex>
#include "mt_data_global.h"
#include <vector>
#include "mt_data_global.h"
#include <climits>
#include "mt_data_global.h"
#include <float.h>
#include "mt_data_global.h"
#include <algorithm>
#include "mt_data_global.h"
#include <numeric>
#include "mt_data_global.h"
#include <QDebug>
#include "mt_data_global.h"
#include <QDataStream>
#include "mt_data_global.h"
#include <QBitArray>
#include "mt_data_global.h"
#include <QFileInfo>
#include "mt_data_global.h"
#include <QFile>

#include "mt_data_global.h"
#include "qtx_templates.h"
#include "mt_data_global.h"
#include "procmt_alldefines.h"
#include "mt_data_global.h"
#include "buckets.h"
#include "mt_data_global.h"
#include "statmaps.h"

#include "mt_data_global.h"
#include <mutex>

/*!
 * \brief The acsp struct contains the auto- and cross spectra and single spectra for ONE frequency
 * the class has no mutex - so only one thread can access one frequency - that is the design
 * also the decsion is to implement the math outside the class; that allows better testing
 * also we add spectra collectors and not acsp itself
 */
struct acsp
{
public:
    explicit acsp();
    explicit acsp(const acsp &rhs);
    /*!
     * \brief acsp
     * \param all_channel_collector
     * \param f
     * \param ac1_ssp2_acssp3 auto-cross == 1, single spectra == 2, auto-cross AND single spectra == 3
     */
    explicit acsp(all_spectra_collector<std::complex<double>> &all_channel_collector, const double &f, const int ac1_ssp2_acssp3);
    ~acsp();

    /*!
     * \brief resize resizes the auto and cross spectra to the input; this avoids reallocation during threads
     */
    void resize();

    /*!
     * \brief resize_auto_spc_only if only auto spectra are needed
     */
    void resize_auto_spc_only();


    /*!
     * \brief add add data and resizes single spectra
     * \param all_channel_collector data from a previous fft process
     * \param f frequency from the collector
     */
    void add(all_spectra_collector<std::complex<double>> &all_channel_collector, const double &f,  const int  ac1_ssp2_acssp3);



    acsp& operator = (const acsp  &rhs);

    bool operator < (const acsp &rhs) {
        return (this->f < rhs.f);
    }

    bool operator > (const acsp &rhs) {
        return (this->f > rhs.f);
    }

    bool operator == (const acsp &rhs) {
        return (this->f == rhs.f);
    }


    /*!
     * \brief has_which_spectra
     * \return 0 (no spectra), 1 ac spectra, 2 single complex spectra, 3 both aut- cross and single complex spectra
     */
    int has_which_spectra() const;

    /*!
     * \brief calc_auto_spc calculate only auto spectra IF we have single spectra available; EXISTING ac spectra will be overwritten ... naturally
     */
    void calc_auto_spc();

    /*!
     * \brief calc_ac_spc calculate all auto and cross spectra IF we have single spectra available
     */
    void calc_ac_spc();

    /*!
     * \brief ampl_spc_if returns a vector of valid amplitude spectra
     * \param spce say 0 for sqrt(ex * conj(ex))
     * \return vector of amplitudes
     */

    std::vector<double> ampl_spc_if(const size_t chan_type) const;


    /*!
     * \brief ssp_spc_ampls_if returns a vector of valid amplitude ssp spectra
     * \param spce say channel_type 0 ex single spectra
     * \return vector of amplitudes or normalized in case of insitu cal (I take sqrt because otherwise power spectra?)
     */
    std::vector<double> ssp_spc_ampls_if(const size_t chan_type) const;

    /*!
     * \brief ssp_spc_phz_if returns a vector of valid phase ssp spectra
     * \param spce say 1 for ey spectra
     * \return vector of phases (radians) or normalized in case of insiut cal
     */
    std::vector<double> ssp_spc_phz_if(const size_t chan_type) const;

    /*!
     * \brief ssp_spc_if returns a real or imag vector of single specta
     * \param real_false_imag_true false: real values, true imag values
     * \param first_1_second_2_all_3 1 first half of data, 2 second half of data, 3 all data
     * \param chan_type which channel type from ex to rhz
     * \return
     */
    std::vector<double> ssp_spc_if(const bool real_false_imag_true, const int first_1_second_2_all_3, const size_t chan_type) const;

    void init();

    /*!
     * \brief set_sel_true resets the selection for auto and cross spectra back to true
     */
    void set_ac_sel_true();

    /*!
     * \brief set_sel_true resets the selection for single spectra back to true
     */
    void set_ssp_sel_true();

    /*!
     * \brief size IF we have any auto or cross spectra at all
     * \return  first spectra size - all spectra MUST have same size - otherwise you already have data corruption
     */
    size_t size() const;


    /*!
     * \brief ssp_size IF we have any ssingle spectra
     * \return first spectra size - all spectra MUST have same size - otherwise you already have data corruption
     */

    size_t ssp_size() const;

    /*!
     * \brief clear resets all data and vector to 0; may not be used because we don't have pointers
     */
    void clear();

    /*!
     * \brief normalize_ssp the numerator will be devided by denumerator; the result is numerator = numerator/denumerator
     * \param numerator for example an H channel
     * \param denominator for example an E channel
     */
    void normalize_ssp(const size_t spce_numerator, const size_t spce_denominator);

    void normalize_ssp_all(const size_t spce_denominator);

    /*!
     * \brief reset_cross_remote_residuals removes the imaginary part of HxHxr - which should be zero ... in theory; must not be in case too far away, but shouldn't; NOT for Hz
     */
    void reset_cross_remote_residuals();

    /*!
     * \brief reset_autospc_residuals avoid calculation noise - HxHx* is not really zero with some compilers BUT must be!
     */
    void reset_auto_residuals();


    void cross_coherency_raw_spectra();

    void set_ssp_chan(const size_t ch_type, const std::vector<std::complex<double>> spcx);

    void backup_e();

    double f = DBL_MAX;                                  //!< the frequency; class tests if DBL_MAX class is no initialized
    std::vector<std::vector<std::complex<double>>> ac;   //!< auto- cross spectra exex ..exhz ... in case, ac_spectra_names
    std::vector<std::vector<bool>> ac_sel;               //!< selections for exex ...
    std::vector<statmap> ac_vals;                        //!< stats for ex conj(ex) ... for all power
    std::vector<statmap> ac_ampl_vals;                   //!< stats for sqrt(ex conj(ex)) ... amplitudes

    std::vector<double> coh;                             //!< coh for cross-spectra

    std::vector<std::vector<std::complex<double>>> ssp;  //!< complex single spectra - e.g the input, ()size of all stacks, channel_types ex, ey .. rhz
    std::vector<std::vector<bool>> ssp_sel;              //!< selection vector for single spectra
    std::vector<statmap> ssp_vals;                       //!< statistics for the single spectra
    std::vector<statmap> ssp_vals_imag;                  //!< statistics for the single spectra - in case we have real (above) and imaginary seperated for CSEM
    std::vector<statmap> ssp_vals_2;                       //!< statistics for the single spectra
    std::vector<statmap> ssp_vals_imag_2;                  //!< statistics for the single spectra - in case we have real (above) and imaginary seperated for CSEM

    std::vector<std::vector<std::complex<double>>> ssp_orig; //!< should contain only the local E/EMAP for chan_type::ex, chan_type::ey for HUBER

};


// read and write
QDataStream& operator >> (QDataStream& is, acsp &dat );

/*!
 * \brief operator << writes data to an already open QDataStream
 * \param os QDataStream (already opened)
 * \param dat is the struct acsp
 * \return
 */
QDataStream& operator << (QDataStream& os, const acsp &dat);

size_t read_acsp_vector(const QFileInfo &qfi, std::vector<acsp> &data);

size_t write_acsp_vector(const QFileInfo &qfi, const std::vector<acsp> &data);

size_t write_ssp_vector_ascii(const QFileInfo &qfidirname, const std::vector<acsp> &data);



/*!
 * \brief get_freqs_acsp iterates over a vector of acsp and returns all frequencies
 * \param ac of acsp strutcs
 * \return a vector of frequencies
 */
std::vector<double> get_freqs_acsp(const std::vector<acsp> &ac);

size_t f_size_a(const std::vector<acsp> &ac, const double& f, size_t &f_idx);                    //!< returns the stack size for a given frequency
size_t f_size_s(const std::vector<acsp> &ac, const double &f, size_t &f_idx);



size_t f_index_acsp(const std::vector<acsp> &ac, const double& f);


/*!
 * \brief f_index_c
 * \param dat a vector bucket of type complex - eg mt complex data
 * \param f frequency
 * \return index of vector or SIZE_MAX
 */
size_t  f_index_c (const std::vector<mt_data_t<std::complex<double>>> &dat, const double &f);

std::vector<double> get_freqs_z(const std::vector<mt_data_t<std::complex<double>>> &dat);


/*!
 * \brief f_size_c
 * \param dat dat a vector bucket of type complex - eg mt complex data
 * \param f frequency
 * \return size of first vector of this structur not null or 0; solves the problem that the fist vector (EX) can be null but second (Ey) maybe not in case of scalar
 */
size_t  f_size_c (const std::vector<mt_data_t<std::complex<double>>> &dat, const double &f);


#endif // MT_DATA_H
