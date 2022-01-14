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

#ifndef ITERATOR_COMPLEX_TEMPLATES_H
#define ITERATOR_COMPLEX_TEMPLATES_H

#include <complex>
#include <vector>
#include <numeric>
#include <algorithm>
#include <string>
#include <utility>

/*!
 * \brief is_complex
 * \param val number to be checked
 * \return true if complex, otherwise false
 */

template<typename T> bool is_complex(const T &val) {
  std::string check_complex(typeid(val).name());
  if (check_complex.find("complex") != check_complex.npos) {
    return true;
  }
  return false;
}


/*!
 * \brief complex_vector_to_real splits a complex vector into real and imaginary
 * \param c complex vector
 * \param r real parts
 * \param i imaginary parts
 */
template<typename T, typename S> void complex_vector_to_real(const std::vector<T> &c, std::vector<S> &r, std::vector<S> &i) {
  r.resize(c.size());
  i.resize(c.size());
  auto ci = c.cbegin();
  auto ce = c.cend();
  auto ri = r.begin();
  auto ii = i.begin();
  while (ci != ce) {
    *ri = std::real(*ci);
    *ii = std::imag(*ci);
    ++ri;
    ++ii;
    ++ci;
  }
}

/*!
 * \brief complex_vector_to_real splits a complex vector into real
 * \param c complex vector
 * \param r real parts
 */
template<typename T, typename S> void complex_vector_to_real(const std::vector<T> &c, std::vector<S> &r) {
  r.resize(c.size());
  auto ci = c.cbegin();
  auto ce = c.cend();
  auto ri = r.begin();
  while (ci != ce) {
    *ri = std::real(*ci);
    ++ri;
    ++ci;
  }
}

/*!
 * \brief complex_vector_to_imag splits a complex vector into imaginary
 * \param c complex vector
 * \param i imaginary parts
 */
template<typename T, typename S> void complex_vector_to_imag(const std::vector<T> &c, std::vector<S> &i) {
  i.resize(c.size());
  auto ci = c.cbegin();
  auto ce = c.cend();
  auto ii = i.begin();
  while (ci != ce) {
    *ii = std::imag(*ci);
    ++ii;
    ++ci;
  }
}


/*!
 * \brief vector_to_complex joins a real and an imaginary vector to a complex vector
 * \param r real parts
 * \param i imaginary parts
 * \param c complex vector
 */
template<typename T, typename S> void vector_to_complex(const std::vector<S> &r, const std::vector<S> &i, std::vector<T> &c) {
  if (r.size() != i.size()) return;
  c.resize(r.size());
  auto ci = c.begin();
  auto ce = c.end();
  auto ri = r.cbegin();
  auto ii = i.cbegin();
  while (ci != ce) {
    (*ci).real(*ri);
    (*ci).imag(*ii);
    ++ri;
    ++ii;
    ++ci;
  }
}

/*!
 * \brief minmax_element_cplx returns min max as complex (min_real, min_imag)(max_real, max_imag)
 * \param v complex vector v
 * \return pair  (min_real, min_imag)(max_real, max_imag)
 */

template<typename T> std::pair<std::complex<double>, std::complex<double>> minmax_element_cplx(const std::vector<T> &v)
{

    std::pair<std::complex<double>, std::complex<double>> min_max;
    if (!is_complex(*v.cbegin())) return  min_max;
    min_max.first = *v.begin(); // will be min real, min imag
    min_max.second = *v.begin(); // will be max real, max imag

    for (auto &cplx : v) {
        if (std::real(cplx) < std::real(min_max.first)) min_max.first.real(std::real(cplx));
        if (std::imag(cplx) < std::imag(min_max.first)) min_max.first.imag(std::imag(cplx));

        if (std::real(cplx) > std::real(min_max.second)) min_max.second.real(std::real(cplx));
        if (std::imag(cplx) > std::imag(min_max.second)) min_max.second.imag(std::imag(cplx));
    }

    return  min_max;
}

/*!
 * \brief min_max2v compares two vectors and returns absolute minima and maxima
 * \param vector x1
 * \param vector x2
 * \return pair min, max
 */
template<typename T> std::pair<T, T> min_max2v(const std::vector<T> &x1, const std::vector<T> &x2) {
    std::pair<T, T> min_max;
    auto r1 = std::minmax_element(x1.cbegin(), x1.cend());
    auto r2 = std::minmax_element(x2.cbegin(), x2.cend());
    if (*r1.first < *r2.first) min_max.first = *r1.first;
    else min_max.first = *r2.first;

    if (*r1.second > *r2.second) min_max.second = *r1.second;
    else min_max.second = *r2.second;

    return min_max;
}

/*!
 * \brief min_max2v compares twocomplex vectors and returns absolute minima and maxima of either real or imags; total values are stored in abs_min_max
 * \param vector x1
 * \param vector x2
 * \param real_true_imag_false decides if real or imaginary pair is returned
 * \param pair abs_min_max - always the total maxima ad minima regardless from real or imaginary
 * \return pair min, max (either from real or from imag)
 */
template<typename T> std::pair<double, double> min_max2v_cplx(const std::vector<T> &x1, const std::vector<T> &x2,
                                                              const bool real_true_imag_false, std::pair<double, double> &abs_min_max) {

    std::pair<double, double> min_max_r;
    std::pair<double, double> min_max_i;

    auto r1 = minmax_element_cplx(x1); // first will be min real, min imag ; second will be max real, max imag
    auto r2 = minmax_element_cplx(x2);

    if (std::real(r1.first) < std::real(r2.first)) min_max_r.first = std::real(r1.first);
    else min_max_r.first = std::real(r2.first);

    if (std::real(r1.second) > std::real(r2.second)) min_max_r.second = std::real(r1.second);
    else min_max_r.second = std::real(r2.second);


    if (std::imag(r1.first) < std::imag(r2.first)) min_max_i.first = std::imag(r1.first);
    else min_max_i.first = std::imag(r2.first);

    if (std::imag(r1.second) > std::imag(r2.second)) min_max_i.second = std::imag(r1.second);
    else min_max_i.second = std::imag(r2.second);


    if (min_max_r.first < min_max_i.first) abs_min_max.first = min_max_r.first;
    else abs_min_max.first = min_max_i.first;

    if (min_max_r.second > min_max_i.second) abs_min_max.second = min_max_r.second;
    else abs_min_max.second = min_max_i.second;

    if (real_true_imag_false) return min_max_r; // min and max of the real parts
    return min_max_i;
}

/*!
 * \brief inner_product_ri inner product for real and imaginary parts seperately
 * \param first iterator x1
 * \param last iterator (range)
 * \param first2 iterator
 * \return pair real_sum, imaginary_sum
 */

template<typename const_iterator> std::pair<double, double> inner_product_ri(const_iterator first1, const_iterator last1, const_iterator first2) {
    double re(0);
    double im(0);
    for (; first1 != last1; ++first1, ++first2) {
        re += std::real(*first1) * std::real(*first2);
        im += std::imag(*first1) * std::imag(*first2);
    }

    return std::pair<double, double>(re, im);
}


#endif
