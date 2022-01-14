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

#ifndef UNUSED_H
#define UNUSED_H

/*!
 * \brief make_d_r_i make d estimator for real and imaginary
 */
template<typename T> void make_d_r_i(const std::vector<T> &Hx, const std::vector<T> &Hy, const std::vector<T> &Exy, std::vector<double> &dr, std::vector<double> &di) {

    auto d1pair = inner_product_ri(Hx.cbegin(), Hx.cend(), Exy.cbegin());
    auto d2pair = inner_product_ri(Hy.cbegin(), Hy.cend(), Exy.cbegin());
    dr[0] = d1pair.first;
    dr[1] = d2pair.first;
    di[0] = d1pair.second;
    di[1] = d2pair.second;
}

/*!
 * \brief make_mest_ri make m estimator for real and imaginary
 */
template<typename T> void make_mest_ri(const std::vector<T> &Hx, const std::vector<T> &Hy,
                                       const std::vector<T> &Hxr, const std::vector<T> &Hyr,
                                       const std::vector<double> &dr, const std::vector<double> &di,
                                       std::vector<double> &mest_r, std::vector<double> &mest_i,
                                       const int real_1_imag_2_both_3) {

    std::vector<double> G1(2);
    std::vector<double> G2(2);

    std::pair<double, double> g10, g11, g20, g21;

    g10 = inner_product_ri(Hx.begin(), Hx.end(), Hxr.begin()); // row 1
    g11 = inner_product_ri(Hx.begin(), Hx.end(), Hyr.begin());
    g20 = inner_product_ri(Hy.begin(), Hy.end(), Hxr.begin()); // row 2
    g21 = inner_product_ri(Hy.begin(), Hy.end(), Hyr.begin());

    if (real_1_imag_2_both_3 == 1 || real_1_imag_2_both_3 == 3) {
        G1[0] = g10.first;
        G1[1] = g11.first;
        G2[0] = g20.first;
        G2[1] = g21.first;
        double divisor = G1[0] * G2[1] - G1[1] * G2[0];
        mest_r[0] = ( (-1.0  * G1[1] * dr[1]) + (G2[1] * dr[0])) / divisor;
        mest_r[1] = (         (G1[0] * dr[1]) - (G2[0] * dr[0])) / divisor;

    }
    if (real_1_imag_2_both_3 == 2 || real_1_imag_2_both_3 == 3) {
        G1[0] = g10.second;
        G1[1] = g11.second;
        G2[0] = g20.second;
        G2[1] = g21.second;
        double divisor = G1[0] * G2[1] - G1[1] * G2[0];
        mest_i[0] = ( (-1.0  * G1[1] * di[1]) + (G2[1] * di[0])) / divisor;
        mest_i[1] = (         (G1[0] * di[1]) - (G2[0] * di[0])) / divisor;

    }

}

template<typename T,  typename S> void make_residuals(std::vector<S> &er, std::vector<S> &ei,
                                                      const std::vector<T> &Exy_iter, const std::vector<T> &Exy_pred,
                                                      const int real_1_imag_2_both_3) {
    size_t i;

    if (real_1_imag_2_both_3 == 1) {
        for (i = 0; i < Exy_iter.size(); ++i) {
            er[i] = std::real(Exy_iter[i]) - std::real(Exy_pred[i]);
        }
    }
    if (real_1_imag_2_both_3 == 2) {
        for (i = 0; i < Exy_iter.size(); ++i) {
            ei[i] = std::imag(Exy_iter[i]) - std::imag(Exy_pred[i]);
        }
    }

    if (real_1_imag_2_both_3 == 3) {
        for (i = 0; i < Exy_iter.size(); ++i) {
            er[i] = std::real(Exy_iter[i]) - std::real(Exy_pred[i]);
            ei[i] = std::imag(Exy_iter[i]) - std::imag(Exy_pred[i]);
        }
    }

}

template<typename T,  typename S> void make_exy_iter(const std::vector<S> &wr, const std::vector<S> &wi,
                                                     const std::vector<S> &er, const std::vector<S> &ei,
                                                     std::vector<T> &Exy_iter, const std::vector<T> &Exy_pred,
                                                     const int real_1_imag_2_both_3) {
    size_t i;

    if (real_1_imag_2_both_3 == 1) {
        for (i = 0; i < Exy_iter.size(); ++i) {
            Exy_iter[i].real(std::real(Exy_pred[i]) + wr[i] * er[i]);
        }
    }
    if (real_1_imag_2_both_3 == 2) {
        for (i = 0; i < Exy_iter.size(); ++i) {
            Exy_iter[i].imag(std::imag(Exy_pred[i]) + wi[i] * ei[i]);
        }
    }
    if (real_1_imag_2_both_3 == 3) {
        for (i = 0; i < Exy_iter.size(); ++i) {
            Exy_iter[i].real(std::real(Exy_pred[i]) + wr[i] * er[i]);
            Exy_iter[i].imag(std::imag(Exy_pred[i]) + wi[i] * ei[i]);
        }
    }

}

template<typename T, typename S> void make_exy_pred( std::vector<T> &Exy, const std::vector<T> &Hx, const std::vector<T> &Hy,
                                                   const std::vector<S> &mest_r, const std::vector<S> &mest_i, const int real_1_imag_2_both_3) {

    size_t i;

    if (real_1_imag_2_both_3 == 1) {
        for (i = 0; i < Exy.size(); ++i) {
            Exy[i].real(mest_r[0] * std::real(Hx[i]) + mest_r[1] * std::real(Hy[i]) );
        }
    }
    if (real_1_imag_2_both_3 == 2) {
        for (i = 0; i < Exy.size(); ++i) {
            Exy[i].imag(mest_i[0] * std::imag(Hx[i]) + mest_i[1] * std::imag(Hy[i]) );
        }
    }
    if (real_1_imag_2_both_3 == 3) {
        for (i = 0; i < Exy.size(); ++i) {
            Exy[i].real(mest_r[0] * std::real(Hx[i]) + mest_r[1] * std::real(Hy[i]) );
            Exy[i].imag(mest_i[0] * std::imag(Hx[i]) + mest_i[1] * std::imag(Hy[i]) );
        }
    }
}


#endif // UNUSED_H
