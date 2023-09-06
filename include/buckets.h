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

#ifndef BUCKETS
#define BUCKETS
#include "iterator_templates.h"
#include "procmt_alldefines.h"
#include "statmaps.h"
#include "vector_utils.h"
#include <algorithm>
#include <cfloat>
#include <iostream>
#include <list>
#include <unordered_map>
#include <vector>

/*!
 *  \brief The mt_data_t template struct: for one frequency holds the data d (complex or double),
 * the bool selection b (true if to processed) together with the corresponding frequency f;
 *   data can be for example z[xx] ... z[yy] where z[xx] has a size of 3212 stacks <br>
 *   you shall work like if (mt.d[xy][100] > too big) mt.b[xy][100] = false
 */
template <typename T>
struct mt_data_t

{
  mt_data_t(const size_t sz, const double &f, const size_t &min_stacks = _min_stacks, const size_t &vector_size = 0) {
    this->d.resize(sz);
    this->f = f;
    if (vector_size) {
      for (auto &x : this->d) {
        x.resize(vector_size);
      }
    }
    this->b.resize(sz);
    if (vector_size) {
      for (auto &x : this->b) {
        x.resize(vector_size, true);
      }
    }

    this->coh.resize(sz);
    int i = 0;
    if (vector_size) {
      for (auto &x : this->coh) {
        if ((i == xy) || (i == yx))
          x.resize(vector_size, 0.0);
        ++i;
      }
    }

    this->trues.resize(sz, vector_size);
    this->mean.resize(sz);
    this->median.resize(sz);

    if (min_stacks >= _min_stacks) {
      this->min_stacks = min_stacks;
    } else
      this->min_stacks = _min_stacks;
  }
  ~mt_data_t() {}

  mt_data_t(const mt_data_t &rhs) {
    this->operator=(rhs);
  }

  mt_data_t &operator=(const mt_data_t &rhs) {

    if (&rhs == this)
      return *this;

    this->d.resize(rhs.d.size());
    this->b.resize(rhs.b.size());
    this->trues.resize(rhs.trues.size());
    this->mean.resize(rhs.b.size());
    this->median.resize(rhs.b.size());
    this->coh.resize(rhs.coh.size());

    this->f = rhs.f;
    size_t i = 0;
    for (auto &dy : d) {
      dy = rhs.d.at(i++);
    }
    i = 0;
    for (auto &dy : b) {
      dy = rhs.b.at(i++);
    }
    i = 0;
    for (auto &dy : trues) {
      dy = rhs.trues.at(i++);
    }
    i = 0;
    for (auto &dy : mean) {
      dy = rhs.mean.at(i++);
    }
    i = 0;
    for (auto &dy : median) {
      dy = rhs.median.at(i++);
    }
    i = 0;
    for (auto &dy : coh) {
      dy = rhs.coh.at(i++);
    }

    this->min_stacks = rhs.min_stacks;

    return *this;
  }

  bool resize(const double &f, const size_t &newsize) {
    if (this->f != f)
      return false;

    for (auto &x : this->d) {
      x.resize(newsize);
    }
    for (auto &x : this->b) {
      x.resize(newsize, true);
    }
    int i = 0;
    for (auto &x : this->coh) {
      if ((i == xy) || (i == yx))
        x.resize(newsize, 0.0);
      ++i;
    }
    return true;
  }

  /*!
   * \brief size
   * \return size of first avalable data vector; in this class ALL data vectors must have same size
   */
  size_t size() const {
    for (auto &yd : d) {
      if (yd.size())
        return yd.size();
    }
    return 0;
  }

  /*!
   * \brief reset_bool set all to true
   */
  void reset_bool() {
    for (auto &x : this->b) {
      std::fill(x.begin(), x.end(), true);
    }
    size_t i = 0;
    for (auto &x : this->trues) {
      x = this->b[i++].size();
    }
  }

  /*!
   * \brief count_true_selections - count the individual xx, xy, yx, yy selections; sure: after merge they must be the same
   */
  void count_true_selections() {
    size_t i = 0;
    for (auto &xcount : this->trues) {
      xcount = 0;
      xcount = size_t(std::count(this->b.at(i).cbegin(), this->b.at(i).cend(), true));
      ++i;
    }
  }

  /*!
   * \brief is_true
   * \return true if ALL elements have more than min_stacks
   */
  bool is_true_tensor_min_stacks(const bool count_first) {

    if (count_first)
      this->count_true_selections();

    for (auto &tr : this->trues) {
      if (tr < this->min_stacks)
        return false;
    }
    return true;
  }

  /*!
   * \brief merge_bool for the final tensor calculation we need all components; if one is de-selected I can't process at all
   */
  void merge_bool(const size_t what) {

    std::vector<std::vector<bool>::iterator> all_elem;

    for (auto &iters : b) {
      all_elem.emplace_back(iters.begin()); // that contians now  b[xy].....
    }

    if (what == tns_sel::xy_yx && all_elem.size() >= tns_size) {

      auto xy_e = this->b[xy].end();
      while (all_elem[xy] != xy_e) {
        if ((*all_elem[xy] == false) || (*all_elem[yx] == false)) {
          for (auto &iter : all_elem) {
            *iter = false;
          }
        } else {
          for (auto &iter : all_elem) {
            *iter = true;
          }
        }

        for (auto &iter : all_elem) {
          ++iter;
        }
      }
    }

    else if (what == tns_sel::xx_xy_yx_yy && all_elem.size() >= tns_size) {
      auto xy_e = this->b[xy].end();
      while (all_elem[xy] != xy_e) {
        if ((*all_elem[xy] == false) || (*all_elem[yx] == false) || (*all_elem[xx] == false) || (*all_elem[yy] == false)) {
          for (auto &iter : all_elem) {
            *iter = false;
          }
        } else {
          for (auto &iter : all_elem) {
            *iter = true;
          }
        }

        for (auto &iter : all_elem) {
          ++iter;
        }
      }
    }

    else if (what == tns_sel::xx_xy_yx_yy_tx_ty && all_elem.size() >= tns_tip_size) {
      auto xy_e = this->b[xy].end();
      while (all_elem[xy] != xy_e) {
        if ((*all_elem[xy] == false) || (*all_elem[yx] == false) || (*all_elem[xx] == false) || (*all_elem[yy] == false) || (*all_elem[tx] == false) || (*all_elem[ty] == false)) {
          for (auto &iter : all_elem) {
            *iter = false;
          }
        } else {
          for (auto &iter : all_elem) {
            *iter = true;
          }
        }

        for (auto &iter : all_elem) {
          ++iter;
        }
      }
    }

    else if (what == tns_sel::xx_yy && all_elem.size() >= tns_size) {

      auto xy_e = this->b[xy].end();
      while (all_elem[xy] != xy_e) {
        if ((*all_elem[xx] == false) || (*all_elem[yy] == false)) {
          for (auto &iter : all_elem) {
            *iter = false;
          }
        } else {
          for (auto &iter : all_elem) {
            *iter = true;
          }
        }

        for (auto &iter : all_elem) {
          ++iter;
        }
      }
    }

    // auto update counts
    this->count_true_selections();
  }

  /*!
   * \brief istrue_xy_yx check if the (important) off diagonal elements are true and selected therewith for processing
   * \param idx
   * \return true if bith are true, false other wise (or too small -> less than three elements
   */
  bool istrue_xy_yx(const size_t &idx) const {
    if (this->b[xy].size() > this->min_stacks) {
      return (this->b[xy][idx] && this->b[yx][idx]);
    }
    return false;
  }

  /*!
   * \brief istrue_xx_yy check if the diagonal elements are true and selected therewith for processing; normally you DON'T want to do this
   * \param idx
   * \return true if bith are true, false other wise (or too small -> less than three elements
   */
  bool istrue_xx_yy(const size_t &idx) const {
    if (this->b[xx].size() > this->min_stacks) {
      return (this->b[xx][idx] && this->b[yy][idx]);
    }
    return false;
  }

  /*!
   * \brief istrue_xx_xy_yx_yy check if ALL elements are true and selected therewith for processing; that may happen for strong this->min_stacksD
   * \param idx
   * \return true if bith are true, false other wise (or too small -> less than three elements
   */
  bool istrue_xx_xy_yx_yy(const size_t &idx) const {
    if (this->b[xy].size() > this->min_stacks) {
      return (this->b[xx][idx] && this->b[xy][idx] && this->b[yx][idx] && this->b[yy][idx]);
    }
    return false;
  }

  /*!
   * \brief istrue_xy check a single tensor element if true and selected for processing
   * \param idx
   * \return
   */
  bool istrue_xy(const size_t &idx) const {
    if (this->b[xy].size() > this->min_stacks) {
      return this->b[xy][idx];
    }
    return false;
  }

  /*!
   * \brief istrue_yx check a single tensor element if true and selected for processing
   * \param idx
   * \return
   */
  bool istrue_yx(const size_t &idx) const {
    if (this->b[yx].size() > this->min_stacks) {
      return this->b[yx][idx];
    }
    return false;
  }

  /*!
   * \brief istrue_xx check a single tensor element if true and selected for processing
   * \param idx
   * \return
   */
  bool istrue_xx(const size_t &idx) const {
    if (this->b[xx].size() > this->min_stacks) {
      return this->b[xx][idx];
    }
    return false;
  }
  /*!
   * \brief istrue_yy check a single tensor element if true and selected for processing
   * \param idx
   * \return
   */
  bool istrue_yy(const size_t &idx) const {
    if (this->b[yy].size() > this->min_stacks) {
      return this->b[yy][idx];
    }
    return false;
  }

  /*!
   * \brief rho calc rho apparent for one element
   * \param elem xx, xy, yx, yy
   * \param idx  [0,1,2, ..., n-1] of the data
   * \return apparent resistivity
   */
  double rho(const size_t &elem, const size_t &idx) const {
    return (mue0 / (2. * M_PI * this->f)) * pow(abs(this->d[elem][idx]), 2.) * 1000000.0;
  }

  /*!
   * \brief phi
   * \param elem
   * \param idx
   * \return
   */
  double phi(const size_t &elem, const size_t &idx) const {
    return arg(this->d[elem][idx]);
  }

  /*!
   * \brief phi_deg
   * \param elem
   * \param idx
   * \return
   */
  double phi_deg(const size_t &elem, const size_t &idx, const bool swap = false) const {
    if (swap) {
      if (elem == tns::yx)
        return ((arg(this->d[elem][idx]) * 180.) / M_PI) + 180.0;
      if (elem == tns::yy)
        return ((arg(this->d[elem][idx]) * 180.) / M_PI) + 180.0;
    }
    return (arg(this->d[elem][idx]) * 180.) / M_PI;
  }

  std::vector<double> rho_vector(const size_t &elem) const {
    std::vector<double> data;
    data.reserve(this->d[elem].size());
    for (auto const &dat : this->d[elem]) {
      data.push_back((mue0 / (2. * M_PI * this->f)) * pow(abs(dat), 2.) * 1000000.0);
    }
    return data;
  }

  std::vector<double> phi_vector(const size_t &elem) const {
    std::vector<double> data;
    data.reserve(this->d[elem].size());
    for (auto const &dat : this->d[elem]) {
      data.push_back(arg(dat));
    }
    return data;
  }

  std::vector<double> phi_vector_deg(const size_t &elem) const {
    std::vector<double> data;
    data.reserve(this->d[elem].size());
    for (auto const &dat : this->d[elem]) {
      data.push_back((arg(dat) * 180.) / M_PI);
    }
    return data;
  }

  void backup_booleans() {
    bcopy.resize(b.size());
    size_t i;
    for (i = 0; i < bcopy.size(); ++i) {
      bcopy[i] = b[i];
    }
  }

  void restore_booleans() {
    if (bcopy.size() != b.size())
      return;
    size_t i;
    for (i = 0; i < bcopy.size(); ++i) {
      b[i] = bcopy[i];
    }
  }

  double f;                             //!< frequency of the main data vector, so elem, idx STD EDI should have descending order 1000 ... 100 .. 10
  std::vector<std::vector<T>> d;        //!< data (complex or double) [xy][0,1,2,3,...600 ..] so elem, idx
  std::vector<std::vector<bool>> b;     //!< selection vector [xx, xy, yx, yy] [0,1,2, ..., n-1] true or false, so elem, idx
  std::vector<std::vector<bool>> bcopy; //!< selection vector [xx, xy, yx, yy] [0,1,2, ..., n-1] true or false, so elem, idx
  std::vector<std::vector<double>> coh; //!< coherency xy, yx ... - init on demand! , so elem, idx

  std::vector<size_t> trues;       //!< selection summary for each component together so true is all true, so idx
  std::vector<T> mean;             //!< mean, size of d ... so 4 or 6 .. components, so idx
  std::vector<T> median;           //!< mean, size of d, so idx
  size_t min_stacks = _min_stacks; //!< procmt_alldefines;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*!
 * \brief mt_data_res holds the results for the z tensor including statistical data
 *  This is also used for EDI data and files
 */
template <typename T>
struct mt_data_res {

  /*!
   * \brief mt_data_res construcor - allocates all and stays always full; this is needed to access/test [yy] for example
   */
  mt_data_res() {

    this->assemble();
  }

  /*!
   * \brief mt_data_res constructor from other (does not copy a backup but makes one)
   * \param rhs
   */
  mt_data_res(const mt_data_res &rhs) {
    this->operator=(rhs);
  }

  /*!
   * \brief assemble prepares the base tensor elemens for access
   */
  void assemble() {
    this->d.resize(tns::tns_tip_size);
    this->err.resize(tns::tns_tip_size);
    this->coh.resize(tns::tns_tip_size);
    this->gstat.resize(tns::tns_tip_size);
    this->mstat.resize(tns::tns_tip_size);
    this->cstat.resize(tns::tns_tip_size);
    this->angle = 0.0;
  }

  /*!
   * \brief test_tensor check that we have at least one component
   * \return true if at least one component is there
   */
  bool test_tensor() {
    if (!this->freqs.size())
      return false;
    int valid = 0;
    // test without tipper
    for (size_t i = 0; i < tns::tns_size; ++i) {
      if (this->d.at(i).size() == this->freqs.size())
        valid += 1;
      if (this->freqs.size() == this->err[i].size()) {
        // remove numerical error like err = -0.00000000001 which is rounding problem
        for (auto &xe : this->err[i]) {
          if (xe < 0)
            xe = 0.0;
        }
      }
    }
    if (valid == 0)
      return false;
    return true;
  }

  /*!
   * \brief test_full_tensor needed for rotation and 2D/3D interpretation
   * \return true if we have all xx, xy,yx, yy; false in case no tensor or scalar
   */
  bool test_full_tensor() {
    if (!this->freqs.size())
      return false;
    int valid = 0;
    for (size_t i = 0; i < tns::tns_size; ++i) {
      if (this->d.at(i).size() == this->freqs.size())
        valid += 1;
      if (this->freqs.size() == this->err[i].size()) {
        // remove numerical error like err = -0.00000000001 which is rounding problem
        for (auto &xe : this->err[i]) {
          if (xe < 0)
            xe = 0.0;
          if (xe > 1.0E31)
            xe = 0.0; // avoid display crash
        }
      }
    }
    if (valid != tns::tns_size) {
      this->clear(true);
      return false;
    }
    return true;
  }

  /*!
   * \brief test_full_tensor_tipper test if we have a full tensor AND tipper (Hz was recorded)
   * \return
   */
  bool test_full_tensor_tipper() {
    if (!this->freqs.size())
      return false;
    int valid = 0;
    for (size_t i = 0; i < tns::tns_tip_size; ++i) {
      if (this->d.at(i).size() == this->freqs.size())
        valid += 1;
      if (this->freqs.size() == this->err[i].size()) {
        // remove numerical error like err = -0.00000000001 which is rounding problem
        for (auto &xe : this->err[i]) {
          if (xe < 0)
            xe = 0.0;
          if (xe > 1.0E31)
            xe = 0.0; // avoid display crash
        }
      }
    }
    if (valid != tns::tns_tip_size)
      return false;
    return true;
  }

  /*!
   * \brief test_errors check that an existing component has an error; cleans negative errors which may be cuased by numerical rounding in case
   * errors are always positive!
   * \return false if no error was provided - plot in case without error bar
   */
  bool test_error(const size_t &elem) {
    if (!this->freqs.size())
      return false;
    if ((this->freqs.size() == this->d[elem].size()) && (this->freqs.size() == this->err[elem].size())) {
      for (auto &xe : this->err[elem]) {
        if (xe < 0)
          xe = 0.0;
      }
      return true;
    }
    return false;
  }

  /*!
   * \brief test_tipper test ONLY the Hz
   * \return
   */
  bool test_tipper() const {
    if (!this->freqs.size())
      return false;

    if ((this->freqs.size() == this->d[tx].size()) && (this->freqs.size() == this->d[ty].size()))
      return true;
    return false;
  }

  bool test_coh() {
    if (!this->freqs.size())
      return false;
    // check if all coh is 0; that may happen for CSAMT data (old files)
  }

  /*!
   * \brief remove_fake_errors in case errors are all zero or 1E+32 remove them
   */
  void remove_fake_errors() {
    if (!this->freqs.size())
      return;

    for (auto &e : this->err) {

      double fake = 1.0E+32; // std EDI says this is a non valid entry
      fake -= 1.0;           // avoid rounding errors after read from ASCII
      size_t count = 0;
      for (const auto &x : e) {
        if (x >= fake)
          ++count;
      }

      if (count == e.size())
        e.clear();

      fake = 1.0E-32; // that is almost zero - including rounding error from ASCII
      count = 0;
      for (const auto &x : e) {
        if (x <= fake)
          ++count;
      }

      if (count == e.size())
        e.clear(); // vector to zero size
    }
  }

  void remove_fake_coherencies() {
    if (!this->freqs.size())
      return;
    for (auto &e : this->coh) {

      double fake = 1.0E+32; // std EDI says this is a non valid entry
      fake -= 1.0;           // avoid rounding errors after read from ASCII
      size_t count = 0;
      for (const auto &x : e) {
        if (x >= fake)
          ++count;
      }

      if (count == e.size())
        e.clear();

      fake = 1.0E-32; // that is almost zero - including rounding error from ASCII
      count = 0;
      for (const auto &x : e) {
        if (x <= fake)
          ++count;
      }

      if (count == e.size())
        e.clear(); // vector to zero size
    }
  }

  /*!
   * \brief rho return rho as Ohm/m
   * \param elem tensor element such as tns::xx tns::xy tns::yx tns::yy
   * \param idx idx is the index of the frequency
   * \return rho
   */
  double rho(const size_t &elem, const size_t &idx) const {
    if (!this->freqs.size())
      return 0.0;
    return (mue0 / (2. * M_PI * this->freqs[idx])) * pow(abs(this->d[elem][idx]), 2.) * 1000000.0;
  }

  /*!
   * \brief rho_vector same as rho() BUT returns data as vector
   * \param elem
   * \return
   */
  std::vector<double> rho_vector(const size_t &elem) const {
    std::vector<double> data;
    if (d.size() < elem)
      return data;
    data.reserve(this->d[elem].size());
    size_t i = 0;
    for (auto const &dat : this->d[elem]) {
      data.push_back((mue0 / (2. * M_PI * this->freqs[i++])) * pow(abs(dat), 2.) * 1000000.0);
    }
    return data;
  }

  /*!
   * \brief phi_deg returns phase as degrees
   * \param elem tensor element such as tns::xx tns::xy tns::yx tns::yy
   * \param idx is the index of the frequency
   * \param swap maps yx and yy to poistive quadrant
   * \return phi
   */
  double phi_deg(const size_t &elem, const size_t &idx, const bool swap = false) const {
    if (!this->freqs.size())
      return 0.0;
    if (swap) {
      if (elem == tns::yx)
        return ((arg(this->d[elem][idx]) * 180.) / M_PI) + 180.0;
      if (elem == tns::yy)
        return ((arg(this->d[elem][idx]) * 180.) / M_PI) + 180.0;
    }
    return (arg(this->d[elem][idx]) * 180.) / M_PI;
  }

  /*!
   * \brief phi_vector_deg same as phi_deg() BUT returns phi as vector
   * \param elem
   * \param swap
   * \return vector phi
   */
  std::vector<double> phi_vector_deg(const size_t &elem, const bool swap = false) const {
    std::vector<double> data;
    if (d.size() < elem)
      return data;
    if (!this->freqs.size())
      return data;

    data.reserve(this->d[elem].size());
    if (swap) {
      for (auto const &dat : this->d[elem]) {
        if (elem == tns::yx)
          data.push_back(((arg(this->dat) * 180.) / M_PI) + 180.0);
        else if (elem == tns::yy)
          data.push_back(((arg(this->dat) * 180.) / M_PI) + 180.0);
        else
          data.push_back((arg(dat) * 180.) / M_PI);
      }
    } else {
      for (auto const &dat : this->d[elem]) {
        data.push_back((arg(dat) * 180.) / M_PI);
      }
    }
    return data;
  }

  /*!
   * \brief coherency returns a coherency for the corresponding tensor ROW, so only tns::xy tns::yx or tns::xxyx tns::yxyy
   * \param elem tensor row
   * \param idx is the index of the frequency
   * \return coherency
   */
  double coherency(const size_t &elem, const size_t &idx) const {
    // data of xx or yy may not be initialized
    if ((elem < tns::xxxy) || (elem > tns::yxyy))
      return 0.0;
    if (!this->freqs.size())
      return 0.0;
    return this->coh[elem][idx];
  }

  /*!
   * \brief coherency_vector same as coherency() but returns a vector
   * \param elem
   * \return vector of coherencies
   */
  std::vector<double> coherency_vector(const size_t &elem) const {
    std::vector<double> data;
    if (coh.size() < elem)
      return data;
    if (!this->freqs.size())
      return data;

    if ((elem < tns::xxxy) || (elem > tns::yxyy))
      return data;
    data.reserve(this->coh[elem].size());
    for (auto const &dat : this->coh[elem]) {
      data.push_back(dat);
    }

    return data;
  }

  void append_real_to_z(const size_t elem, const std::vector<double> &dat, const bool real_true_im_false) {
  }

  /*!
   * \brief rho_err returns error of rho; the error bar is divided by two in display for error greater than value
   * \param elem
   * \param bsqrt check for error vs variance
   * \return
   */
  double rho_err(const size_t &elem, const size_t &idx, const bool bsqrt) const {
    if (!this->freqs.size())
      return 0.0;

    double xerr;
    if (bsqrt) {
      xerr = ((abs(this->d[elem][idx]) * std::sqrt(this->err[elem][idx])) / this->freqs[idx]);
    } else {
      xerr = ((abs(this->d[elem][idx]) * this->err[elem][idx]) / this->freqs[idx]);
    }

    double xrho = this->rho(elem, idx);
    // avoid plot err in log scale
    if (xerr > xrho)
      xerr *= 0.5;
    return xerr;
  }

  std::vector<double> rho_vector_err(const size_t &elem, const bool bsqrt) const {
    std::vector<double> data;
    if (d.size() < elem)
      return data;
    if (err.size() < elem)
      return data;
    if (!this->freqs.size())
      return data;

    if (this->d[elem].size() != this->err[elem].size())
      return data;
    data.reserve(this->d[elem].size());
    auto iter_err = this->err[elem].cbegin();
    auto iter_freq = this->freqs.cbegin();

    if (bsqrt) {
      for (auto const &dat : this->d[elem]) {
        // 0.4
        data.push_back((abs(dat) * std::sqrt(*iter_err++)) / *iter_freq++);
      }
    } else {
      for (auto const &dat : this->d[elem]) {
        // 0.4
        data.push_back((abs(dat) * (*iter_err++)) / *iter_freq++);
      }
    }

    auto vals = this->rho_vector(elem);
    if (vals.size() != data.size()) {
      data.clear();
      return data;
    }

    // avoid plot err in log scale
    for (size_t i = 0; i < data.size(); ++i) {
      if (data[i] > vals[i])
        data[i] = vals[i] / 2.0;
    }

    return data;
  }

  /*!
   * \brief phi_deg_err calculates erro of phase
   * \param elem
   * \param idx
   * \param bsqrt check for error vs variance, default true!
   * \return
   */
  double phi_deg_err(const size_t &elem, const size_t &idx, const bool bsqrt) const {
    if (!this->freqs.size())
      return 0.0;
    // asin MUST be 1 < x < 1 interval
    double xerr = 0.0;
    if (bsqrt) {
      if (sqrt(this->err[elem][idx]) / abs(this->d[elem][idx]) > 0.99)
        xerr = 0.0;
      else
        xerr = (asin(sqrt(this->err[elem][idx]) / abs(this->d[elem][idx]))) / 2.0;
    } else {
      if (sqrt(this->err[elem][idx]) / abs(this->d[elem][idx]) > 0.99)
        xerr = 0.0;
      else
        xerr = (asin(this->err[elem][idx] / abs(this->d[elem][idx]))) / 2.0;
    }
    if (std::isnan(xerr))
      xerr = M_PI_2;
    xerr *= (180. / M_PI);
    return xerr;
  }

  /*!
   * \brief phi_vector_deg_err same as phi_deg_err() BUT as vector
   * \param elem
   * \param bsqrt
   * \return
   */
  std::vector<double> phi_vector_deg_err(const size_t &elem, const bool bsqrt) const {
    std::vector<double> data;
    if (d.size() < elem)
      return data;
    if (err.size() < elem)
      return data;
    if (!this->freqs.size())
      return data;
    if (this->d[elem].size() != this->err[elem].size())
      return data;
    data.reserve(this->d[elem].size());
    auto iter_err = this->err[elem].cbegin();

    double x;

    for (auto const &dat : this->d[elem]) {
      x = 0.0;
      if (bsqrt) {
        if (*iter_err < 0)
          x = 0.0;
        else if (sqrt(*iter_err) / abs(dat) > 0.99)
          x = 0.0;
        else
          x = (asin(sqrt(*iter_err) / abs(dat))) / 2.0;
      } else {
        if (*iter_err < 0)
          x = 0.0;
        else if (sqrt(*iter_err) / abs(dat) > 0.99)
          x = 0.0;
        else
          x = (asin(*iter_err / abs(dat))) / 2.0;
      }
      if (std::isnan(x))
        x = M_PI_2;
      x *= (180. / M_PI);
      data.push_back(x);
      iter_err++;
    }
  }

  /*!
   * \brief get_rho_star_z_star
   * \param rs
   * \param rs_err
   * \param zs
   * \param zs_err
   * \param elem
   * \return 0 fail, other success
   */
  size_t get_rho_star_z_star(std::vector<double_t> &rs, std::vector<double_t> &rs_err, std::vector<double_t> &zs, std::vector<double_t> &zs_err, const size_t &elem) const {
    if (!this->freqs.size())
      return 0;
    rs.clear();
    rs_err.clear();
    zs.clear();
    zs_err.clear();
    if ((elem < tns::xxxy) || (elem > tns::yxyy))
      return 0;
    if (!this->d[elem].size())
      return 0;
    if (!this->err[elem].size())
      return 0;
    if (this->d[elem].size() != this->err[elem].size())
      return 0;

    rs.resize(this->d[elem].size(), 0.0);
    rs_err.resize(this->d[elem].size(), 0.0);
    zs.resize(this->d[elem].size(), 0.0);
    zs_err.resize(this->d[elem].size(), 0.0);

    for (size_t i = 0; i < d[elem].size(); ++i) {

      double ra = (mue0 / (2. * M_PI * this->freqs[i])) * pow(abs(this->d[elem][i]), 2.) * 1000000.0;
      double phi = arg(this->d[elem][i]);
      if (phi < 0)
        phi += M_PI;
      if ((phi > M_PI_4)) {
        rs[i] = 2. * ra * pow(cos(phi), 2.);
        rs_err[i] = sqrt(2. * rs.at(i) * this->err[elem][i] / fabs(real(this->d[elem][i])));

      } else if ((phi >= 0 && phi <= M_PI_4)) {
        rs[i] = 2. * ra * pow(sin(phi), 2.);
        rs_err[i] = sqrt(2. * rs.at(i) * this->err[elem][i] / fabs(real(this->d[elem][i])));
      }

      zs[i] = fabs(real(this->d[elem][i] / std::complex<double>(0, 2. * M_PI * this->freqs[i])));
      zs_err[i] = sqrt(fabs(zs.at(i) * this->err[elem][i] / fabs(real(this->d[elem][i]))));
    }
    return rs.size();
  }

  void sort() {
    if (!this->freqs.size())
      return;
    if (!this->d.size())
      return;

    for (auto &v : this->d) {
      sort_by_first_mod_second_only<double, std::complex<double>>(this->freqs, v);
    }
    for (auto &v : this->err) {
      sort_by_first_mod_second_only<double, double>(this->freqs, v);
    }
    for (auto &v : this->coh) {
      sort_by_first_mod_second_only<double, double>(this->freqs, v);
    }

    std::sort(this->freqs.begin(), this->freqs.end());
  }

  double anisotropy(const size_t &idx) const {
    if (!this->freqs.size())
      return 0.0;
    return (std::abs(d[yx][idx]) / std::abs(d[xy][idx]));
  }

  std::vector<double> anisotropy_vector() const {
    std::vector<double> data;
    if (!this->freqs.size())
      return data;
    data.resize(this->freqs.size());
    for (size_t idx = 0; idx < freqs.size(); ++idx) {
      data[idx] = std::abs(d[yx][idx]) / std::abs(d[xy][idx]);
    }

    return data;
  }

  // place holder
  double strike(const size_t &idx) const {
    return 2.0;
  }

  std::vector<double> strike_vector() const {
    std::vector<double> data;
    if (!this->freqs.size())
      return data;
    data.resize(this->freqs.size());
    for (size_t idx = 0; idx < freqs.size(); ++idx) {
      data[idx] = this->strike(idx);
    }

    return data;
  }

  // blue x north up
  double tipper_px(const size_t &idx) const {
    if (!this->test_tipper())
      return 0;
    return std::real(this->d[tx][idx]); // blue
  }

  // blue y east right
  double tipper_py(const size_t &idx) const {
    if (!this->test_tipper())
      return 0;
    return std::real(this->d[ty][idx]); // blue
  }

  // red x north up
  double tipper_qx(const size_t &idx) const {
    if (!this->test_tipper())
      return 0;
    return std::imag(this->d[tx][idx]); // red
  }

  // red y east right
  double tipper_qy(const size_t &idx) const {
    if (!this->test_tipper())
      return 0;
    return std::imag(this->d[ty][idx]); // red
  }

  /*!
   * \brief rotate rotate tensor and tipper
   * \param rad
   * \param radians if false rad will be assumed as deg and converted to rad
   * \param ai_limit - rotate only if anisotroy limit is exceeded
   * \return true in case of succes
   */
  bool rotate(const double &rad, bool radians = true, const double ai_limit = 0) {

    return true;
  }

  bool toASCII_file(const QFileInfo &qfi) const {
    if (!freqs.size())
      return false;
    QFile file(qfi.absoluteFilePath());
    QStringList header;
    if (tns_size <= d.size()) {
      header << "#f"
             << "zxx.re"
             << "zxx.im"
             << "delta_zxx";
      header << "zxy.re"
             << "zxy.im"
             << "delta_zxy";
      header << "zyx.re"
             << "zyx.im"
             << "delta_zyx";
      header << "zyy.re"
             << "zyy.im"
             << "delta_zyy";
    }
    if (tns_tip_size == d.size()) {
      header << "tx.re"
             << "tx.im"
             << "delta_tx";
      header << "ty.re"
             << "ty.im"
             << "delta_ty";
    }

    if (tns_size <= d.size()) {
      header << "rho_xx"
             << "delta_rho_xx";
      header << "rho_xy"
             << "delta_rho_xy";
      header << "rho_yx"
             << "delta_rho_yx";
      header << "rho_yy"
             << "delta_rho_yy";
      header << "phi_deg_xx"
             << "delta_phi_deg_xx";
      header << "phi_deg_xy"
             << "delta_phi_deg_xy";
      header << "phi_deg_yx"
             << "delta_phi_deg_yx";
      header << "phi_deg_yy"
             << "delta_phi_deg_yy";
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
      return false;
    QTextStream qts(&file);
    qts.setDevice(&file);
    qts.setRealNumberNotation(QTextStream::ScientificNotation);
    qts.setFieldWidth(18);
    qts.setRealNumberPrecision(8);

    qts.setFieldAlignment(QTextStream::AlignCenter);

    for (auto &head : header)
      qts << head;
    qts.setFieldWidth(0);
    qts << Qt::endl;
    qts.setFieldWidth(18);

    qts.setFieldAlignment(QTextStream::AlignRight);

    for (size_t i = 0; i < this->freqs.size(); ++i) {
      qts << freqs.at(i);
      for (size_t j = 0; j < this->d.size(); ++j) {
        qts << std::real(this->d.at(j).at(i)) << std::imag(this->d.at(j).at(i)) << this->err.at(j).at(i);
      }
      for (size_t j = 0; j < tns_size; ++j) {
        qts << this->rho(j, i) << this->rho_err(j, i, false);
      }
      for (size_t j = 0; j < tns_size; ++j) {
        qts << this->phi_deg(j, i) << this->phi_deg_err(j, i, false);
      }
      qts.setFieldWidth(0);
      qts << Qt::endl;
      qts.setFieldWidth(18);
    }

    qts.flush();
    file.close();

    return true;
  }

  std::vector<double> freqs;               //!< STD EDI should have descending order 1000 ... 100 .. 10
  std::vector<std::vector<T>> d;           //!< structure is d[xx][freq] .. d[yx][freq] ....
  std::vector<std::vector<double>> err;    //!< structure is err[xx][freq] .. err[yx][freq] ....
  std::vector<std::vector<double>> coh;    //!< structure is coh[xy][freq] .. coh[yx][freq] ....
  std::vector<std::vector<statmap>> gstat; //!< gaussian statistics based on mean gstat[pmt::xy][freq] holds e.g. the stat values for zxy
  std::vector<std::vector<statmap>> mstat; //!< gaussian statistics based on median
  std::vector<std::vector<statmap>> cstat; //!< gaussian statistics based on coherency

  std::vector<bool> trues; //!< selection vector [0,1,2, ..., n-1] true or false for visiblity or merge; false will be used!!

  std::vector<double> freqs_bck;
  std::vector<std::vector<T>> d_bck; //!<  structure is d[xx][freq] .. d[yx][freq] ....
  std::vector<std::vector<double>> err_bck;
  std::vector<std::vector<double>> coh_bck;
  std::vector<std::vector<statmap>> gstat_bck; //!< gaussian statistics based on mean gstat[pmt::xy][freq] holds e.g. the stat values for zxy
  std::vector<std::vector<statmap>> mstat_bck; //!< gaussian statistics based on median
  std::vector<std::vector<statmap>> cstat_bck; //!< gaussian statistics based on coherency

  double angle;     //!< in radians, is 0 for North Pi/4 is East
  double angle_bck; //!< in radians, is 0 for North Pi/4 is East

  /*!
   * \brief clear all data; you have an option not to clear the backup
   */
  void clear(const bool clear_backup = true) {

    this->freqs.clear();
    for (auto &dy : this->d) {
      dy.clear();
    }
    for (auto &dy : this->err) {
      dy.clear();
    }
    for (auto &dy : this->coh) {
      dy.clear();
    }
    for (auto &dy : this->gstat) {
      dy.clear();
    }
    for (auto &dy : this->mstat) {
      dy.clear();
    }
    for (auto &dy : this->cstat) {
      dy.clear();
    }

    this->trues.clear();
    this->d.clear();
    this->err.clear();
    this->coh.clear();
    this->gstat.clear();
    this->mstat.clear();
    this->cstat.clear();

    this->assemble();

    if (clear_backup) {
      this->clear_backup();
    }
  }

  void clear_backup() {
    for (auto &dy : this->d_bck) {
      dy.clear();
    }
    for (auto &dy : this->err_bck) {
      dy.clear();
    }
    for (auto &dy : this->coh_bck) {
      dy.clear();
    }
    for (auto &dy : this->gstat_bck) {
      dy.clear();
    }
    for (auto &dy : this->mstat_bck) {
      dy.clear();
    }
    for (auto &dy : this->cstat_bck) {
      dy.clear();
    }

    this->d_bck.clear();
    this->err_bck.clear();
    this->coh_bck.clear();
    this->gstat_bck.clear();
    this->mstat_bck.clear();
    this->cstat_bck.clear();
  }

  /*!
   * \brief backup makes a copy - needed to restore after rotate and others
   */
  void backup() {

    this->d_bck.resize(this->d.size());
    this->err_bck.resize(this->err.size());
    this->coh_bck.resize(this->coh.size());
    this->freqs_bck = this->freqs;
    this->gstat_bck.resize(this->gstat.size());
    this->mstat_bck.resize(this->mstat.size());
    this->cstat_bck.resize(this->cstat.size());

    size_t i = 0;
    for (auto &dy : this->d_bck) {
      dy = this->d.at(i++);
    }

    i = 0;
    for (auto &dy : this->err_bck) {
      dy = this->err.at(i++);
    }

    i = 0;
    for (auto &dy : this->coh_bck) {
      dy = this->coh.at(i++);
    }
    i = 0;
    for (auto &dy : this->gstat_bck) {
      dy = this->gstat.at(i++);
    }
    i = 0;
    for (auto &dy : this->mstat_bck) {
      dy = this->mstat.at(i++);
    }
    i = 0;
    for (auto &dy : this->cstat_bck) {
      dy = this->cstat.at(i++);
    }
  }

  /*!
   * \brief restore restores the data from last
   */
  void restore() {

    this->d.resize(this->d_bck.size());
    this->err.resize(this->err_bck.size());
    this->coh.resize(this->coh_bck.size());
    this->freqs = this->freqs_bck;
    this->gstat.resize(this->gstat_bck.size());
    this->mstat.resize(this->mstat_bck.size());
    this->cstat.resize(this->cstat_bck.size());

    size_t i = 0;
    for (auto &dy : this->d) {
      dy = this->d_bck.at(i++);
    }

    i = 0;
    for (auto &dy : this->err) {
      dy = this->err_bck.at(i++);
    }

    i = 0;
    for (auto &dy : this->coh) {
      dy = this->coh_bck.at(i++);
    }
    i = 0;
    for (auto &dy : this->gstat) {
      dy = this->gstat_bck.at(i++);
    }
    i = 0;
    for (auto &dy : this->mstat) {
      dy = this->mstat_bck.at(i++);
    }
    i = 0;
    for (auto &dy : this->cstat) {
      dy = this->cstat_bck.at(i++);
    }
  }

  void check_delete_err_dbl_max() {

    std::vector<double> t_freqs;
    std::vector<std::vector<T>> t_d(this->d.size());
    std::vector<std::vector<double>> t_err(this->err.size());
    std::vector<std::vector<double>> t_coh(this->coh.size());

    std::vector<size_t> idx_del;

    // for all err types xx ..
    for (auto &x : this->err) {
      size_t i = 0;
      // along all frequencies
      for (auto &y : x) {
        if (y == DBL_MAX) {
          // not found, insert, avoid that we have several times the same index
          if (std::find(idx_del.begin(), idx_del.end(), i) == idx_del.end())
            idx_del.push_back(i);
        }
        ++i;
      }
    }
    // if we have DBL_MAX errors
    if (idx_del.size()) {
      std::swap(t_freqs, this->freqs);
      std::swap(t_d, this->d);
      std::swap(t_err, this->err);
      std::swap(t_coh, this->coh);

      for (size_t i = 0; i < t_freqs.size(); ++i) {

        if (std::find(idx_del.begin(), idx_del.end(), i) == idx_del.end()) {
          this->freqs.push_back(t_freqs.at(i));

          size_t j = 0;
          // d and t_d have same types xx, xy ..
          for (auto &y : t_d) {
            // push back the data of index with no DBL_MAX errr
            this->d[j++].push_back(y.at(i));
          }

          j = 0;
          for (auto &y : t_err) {
            this->err[j++].push_back(y.at(i));
          }

          j = 0;
          for (auto &y : t_coh) {
            this->coh[j++].push_back(y.at(i));
          }
        }
      }
    }
  }

  /*!
   * \brief resize if one tensor has xx ... yy and the other one ony xx, xy - the tensor will be destroyed
   * \param sz
   * \param freqs
   */
  void resize(const size_t sz, const std::vector<double> &freqs) {

    if (this->d.size() != sz)
      this->clear(true);
    if (!this->d.size()) {
      this->d.resize(sz);
      this->err.resize(sz);
      this->coh.resize(sz);
      this->gstat.resize(sz);
      this->mstat.resize(sz);
      this->cstat.resize(sz);
    }
    this->freqs = freqs;
    this->trues.resize(freqs.size(), false); // false == not selected for hidden / exclusion

    for (auto &dy : this->d) {
      dy.resize(this->freqs.size());
    }
    for (auto &dy : this->err) {
      dy.resize(this->freqs.size(), 0.0);
    }
    for (auto &dy : this->coh) {
      dy.resize(this->freqs.size(), 0.0);
    }
    for (auto &dy : this->gstat) {
      dy.resize(this->freqs.size());
    }
    for (auto &dy : this->mstat) {
      dy.resize(this->freqs.size());
    }
    for (auto &dy : this->cstat) {
      dy.resize(this->freqs.size());
    }
  }

  /*!
   * \brief operator = the assignment operator doe not want to copy the backup
   * \param rhs
   * \return
   */
  mt_data_res &operator=(const mt_data_res &rhs) {

    if (&rhs == this)
      return *this;

    this->d.resize(rhs.d.size());
    this->err.resize(rhs.err.size());
    this->coh.resize(rhs.coh.size());
    this->freqs = rhs.freqs;
    this->gstat.resize(rhs.gstat.size());
    this->mstat.resize(rhs.mstat.size());
    this->cstat.resize(rhs.cstat.size());
    this->trues = rhs.trues;

    size_t i = 0;
    for (auto &dy : this->d) {
      dy = rhs.d.at(i++);
    }

    i = 0;
    for (auto &dy : this->err) {
      dy = rhs.err.at(i++);
    }

    i = 0;
    for (auto &dy : this->coh) {
      dy = rhs.coh.at(i++);
    }
    i = 0;
    for (auto &dy : this->gstat) {
      dy = rhs.gstat.at(i++);
    }
    i = 0;
    for (auto &dy : this->mstat) {
      dy = rhs.mstat.at(i++);
    }
    i = 0;
    for (auto &dy : this->cstat) {
      dy = rhs.cstat.at(i++);
    }

    this->backup();

    return *this;
  }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*!
 * \brief The single_spc_collecor_buff struct collects e.g. all FFTs of one channel in a queue \br
 *  when finished you want to call to_map in order to have <64Hz, vector<all 64Hz spectra data> \br
 *  when using full FFT this is slow - it is more to have selected frequencies
 * in case you have a next channel and a different sample freq - you must create a next struct for next fft
 * so this collects only "one shot"
 * We assumme that the data is either mV/km, nT
 */
template <typename T>
struct channel_single_spc_collector_buff {

  /*!
   * \brief channel_single_spc_collector_buff constructor with nil or given channel or auto cross spectra
   */
  explicit channel_single_spc_collector_buff(const size_t cht = SIZE_MAX, const size_t spcet = SIZE_MAX) :
      chan_type(cht), spce(spcet) {}

  /*!
   * \brief channel_single_spc_collector_buff constructor with other class
   * \param rhs
   */
  explicit channel_single_spc_collector_buff(const channel_single_spc_collector_buff &rhs) {
    this->operator=(rhs);
  }

  /*!
   * \brief operator =
   * \param rhs
   * \return
   */
  channel_single_spc_collector_buff &operator=(const channel_single_spc_collector_buff &rhs) {

    if (&rhs == this)
      return *this;
    this->freqs = rhs.freqs;
    this->chan_type = rhs.chan_type;
    this->spce = rhs.spce;
    this->data = rhs.data;
    this->vdata = rhs.vdata;
  }

  /*!
   * \brief operator < channel_type: we need to to sort the channels ascendig in order to have always Ex*Hy and not Hy * Ex
   * \param rhs
   * \return
   */
  bool operator<(const channel_single_spc_collector_buff &rhs) {
    return (this->chan_type < rhs.chan_type);
  }

  size_t size() const {
    return this->data.size();
  }

  /*!
   * \brief to_vector_vector to vector convert the queue to internal vector
   * \param max_size limit the data size - in case of different size when having seletions and RR or EMAP
   */

  void to_vector_vector(const size_t max_size = SIZE_MAX) {
    if (max_size != SIZE_MAX)
      this->vdata.reserve(max_size);
    else
      this->vdata.reserve(this->data.size());
    size_t i = 0;
    while (!data.empty()) {
      if (i < max_size)
        vdata.emplace_back(std::move(this->data.front()));
      this->data.pop();
      ++i;
    }
  }

  /*!
   * \brief pop_front return a single queue vector
   * \return
   */
  std::vector<T> pop_front() {
    if (this->data.empty()) {
      std::vector<T> dat;
      return dat;
    }
    std::vector<T> dat(std::move(move(this->data.front())));
    this->data.pop();
    return dat;
  }

  /*!
   * \brief to_map creates a unordered map - which should be sorted already!!; so data_map[64] contains all 64Hz data; that is simply spoken a memory resort \br
   * DO NOT call this if you want to collect later in channel_single_spc_collector_mutli_runs_buff !!
   */
  void to_map() {
    if (!this->freqs.size())
      return;
    if (!this->data.size())
      return;
    // create an empty map with all frequencies
    for (auto &f : this->freqs)
      this->data_map.emplace(f, std::vector<T>());
    // take the queus
    while (!this->data.empty()) {
      // move the vector out (not copy)
      std::vector<T> tmp(std::move(this->data.front()));
      auto tmp_iter = tmp.cbegin();
      // freqs and tmp (spectra) have same size
      for (auto &f : this->freqs) {
        this->data_map[f].emplace_back(*tmp_iter++);
      }
      this->data.pop(); // vector is empty already, remove it
    }
  }

  size_t chan_type = SIZE_MAX; //!< enum channel type
  size_t spce = SIZE_MAX;      //!< enum auto cross spectra type

  std::vector<double> freqs; //!< vector of frequencies of the spectra from FFT; freqs and vector<T> have ALWAYS sam size

  std::queue<std::vector<T>> data;                     //!< collects N vectors of freqs.size queue(17)[32] contains the spectra for freqs[32]; queue(17 = stack no )) is the fft vector(1024) for example
  std::vector<std::vector<T>> vdata;                   //!< if finished we can make a vector of vectors in case - and empty the finalized queue
  std::unordered_map<double, std::vector<T>> data_map; //!< map of frequencies - if we want to transform
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*!
 *  \brief channel_single_spc_collector_mutli_runs_buff collects for one channel all channel_single_spc_collector_buff and sorts by f
 *  so have two FFTs one with 4096 and one with 512 and same channel it goes here
 *  THIS CLASS IS MENT TO BE ALLOCATED FROM all_spectra_collector AND MAY NOT APPEAR IN YOUR CODE
 *  for hugh raw spectra this method is expensive; the class however can run in a thread, for example in atsfile_collector
 *  may looks like all_spc_coll.data[input[0].channel_type].enque_data(input[0]);
 *
 */
template <typename T>
struct channel_single_spc_collector_mutli_runs_buff {

  explicit channel_single_spc_collector_mutli_runs_buff(const size_t cht = SIZE_MAX) :
      chan_type(cht) {}

  ~channel_single_spc_collector_mutli_runs_buff() {
    this->data.clear();
  }

  void clear() {
    this->data.clear();
  }

  QString get_channel_name() const {

    // channel data? chan_type should be size_max if not used
    if (this->chan_type < size_t(pmt::channel_types.size()))
      return pmt::channel_types.at(this->chan_type);
    // spectra data? spce should be size:max if not used
    if (this->spce < size_t(pmt::ac_spectra_names.size()))
      return pmt::ac_spectra_names.at(this->spce);
    return QString("nil");
  }

  //!
  //! \brief enque_data collects the data from single spectra and different runs; empties the input
  //! \param input channel_multi_spc_collector_buff which will be emptied
  //!
  void append_clear_data(channel_single_spc_collector_buff<T> &input) {

    if (input.data.empty())
      return;
    this->chan_type = input.chan_type;
    this->spce = input.spce;

    // should only create a NEW queue only when f NOT exists
    for (auto &f : input.freqs)
      this->data.emplace(f, std::vector<T>());
    // resvere new size in order to avoid too many resize
    for (auto &f : input.freqs)
      this->data[f].reserve(this->data[f].size() + input.data.size());

    // iterate over the input queue, the queue contains a vector of same size containing a spectra
    while (!input.data.empty()) {
      // move the vector out (not copy)
      std::vector<T> tmp(std::move(input.data.front()));
      auto tmp_iter = tmp.cbegin();
      // freqs and tmp (spectra) have same size
      for (auto &f : input.freqs) {
        this->data[f].emplace_back(*tmp_iter++);
      }
      input.data.pop(); // vector is empty already, remove it
    }

    input.freqs.clear(); // clear the input
  }

  /*!
   * \brief reserve_mem_freqs reserve map nodes in advance for threading
   * \param target_freqs
   */
  void reserve_mem_freqs(const std::vector<double> target_freqs) {
    // should only create a NEW queue only when f NOT exists
    for (auto &f : target_freqs)
      this->data.emplace(f, std::vector<T>());
  }

  void append(const std::vector<T> &data, const std::vector<double> &freqs) {
    auto tmp_iter = data.cbegin();
    // freqs and tmp (spectra) have same size
    for (auto &f : freqs) {
      this->data[f].emplace_back(*tmp_iter++);
    }
  }

  void append(const T &data, const double &freq) {
    this->data[freq].emplace_back(data);
  }

  void append(const std::vector<T> &data, const double &freq) {
    size_t n(this->data[freq].size());
    this->data[freq].reserve(n + data.size());
    for (auto &d : data)
      this->data[freq].emplace_back(d);
  }

  /*!
   * \brief freqs
   * \return sorted vector of all frequencies for this channel
   */
  std::vector<double> get_frequencies() const {
    std::vector<double> vfreqs;
    if (this->data.size()) {
      vfreqs.reserve(this->data.size());
      for (auto &ds : this->data) {
        vfreqs.push_back(ds.first);
      }
      std::sort(vfreqs.begin(), vfreqs.end());
    }
    return vfreqs;
  }

  /*!
   * \brief get_stack_size
   * \param f target frequency like 64 of 34
   * \return size of stacks for this frequency
   */
  size_t get_stack_size(const double &f) const {
    return data.at(f).size();
  }

  //    void take_5() const {
  //        if ((data.size() > 3) && spce < SIZE_MAX) {
  //            auto diter = data.cbegin();
  //            ++diter;
  //            if (diter->second.size() > 5) {
  //                std::cout << this->get_channel_name().toStdString() << " ";
  //                for (size_t i = 0; i < 5; ++i) std::cout << diter->second.at(i) << "  ";
  //                std::cout << std::endl;
  //            }
  //        }
  //    }

  std::unordered_map<double, std::vector<T>> data; //!< this map is a all frequencies container; data[64] contains all 64Hz from all collected spectra of different runs
  size_t chan_type = SIZE_MAX;                     //!< enum channel type - in case this is channel data
  size_t spce = SIZE_MAX;                          //!< enum auto cross spectra type - in case this is spetral data; eg. you find for example 2, that is hxhx, or 23 for rhxrhx
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*!
 * \brief all_spectra_collector contains all channel_multi_spc_collector_buff ; all_spectra_collector.data[Ex] conatains all Ex data from all FFTs and runs
 *  the underlying conatiner is public
 */
template <typename T>
struct all_spectra_collector {

  explicit all_spectra_collector(bool use_ac_spectra_names = true) {
    if (!use_ac_spectra_names)
      data.resize(size_t(pmt::channel_types.size()));
    else {
      data.resize(size_t(pmt::ac_spectra_names.size()));
    }
  }
  ~all_spectra_collector() {
    for (auto &dat : this->data)
      dat.clear();
    this->all_freqs.clear();
  }

  void clear() {
    for (auto &dat : this->data)
      dat.clear();
    this->all_freqs.clear();
  }

  //    void take_5() const {
  //        for (auto &dat : this->data) {
  //            dat.take_5();
  //        }
  //    }

  QStringList info() const {
    QStringList strall;

    for (auto &dat : data) {
      std::vector<double> ffs(dat.get_frequencies());
      std::vector<size_t> bsize;
      for (auto &fs : ffs) {
        bsize.emplace_back(dat.get_stack_size(fs));
      }
      size_t j = 0;
      if (bsize.size() && (bsize.size() == ffs.size())) {
        QString str;
        for (auto &vsz : bsize) {
          str += dat.get_channel_name() + " " + QString::number(ffs.at(j++)) + "Hz: " + QString::number(vsz) + ";    ";
        }
        strall.append(str);
      }
    }
    return strall;
  }

  std::vector<double> get_frequencies() {
    this->all_freqs.clear();
    for (auto &dat : this->data) {
      this->all_freqs = dat.get_frequencies();
      if (this->all_freqs.size()) {
        return this->all_freqs;
      }
    }
    return this->all_freqs;
  }

  std::vector<size_t> get_sizes(const std::vector<double> &freqs) const {

    std::vector<size_t> sizes;
    if (!freqs.size())
      return sizes;
    std::vector<double> fake_f;
    for (auto &dat : this->data) {
      fake_f = dat.get_frequencies();
      if (fake_f.size()) {

        for (auto &f : freqs) {
          sizes.push_back(dat.get_stack_size(f));
        }
      }
    }
    return sizes;
  }

  std::vector<double> all_freqs;
  std::vector<channel_single_spc_collector_mutli_runs_buff<T>> data; //!< this vector is a ALL CHANNEL CONTAINER for Ex, .. Hz
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*!
 * \brief is a FINAL container and vector based - you do not want to reallocate
 *  the in main you have VECTOR access is ssp[f].d[Ex][...specta] and  ssp[f].d[Hz][...specta]] is the vector of these channel by frequency
 */
template <typename T>
struct f_spectra {

  explicit f_spectra(all_spectra_collector<T> &all_channel_collector, const double f) {
    this->f = f;
    // prepare all possible input Ex, Ey ... we need all, otherwise we can'T iterate
    this->d.resize(size_t(pmt::channel_types.size()));
    // first check the existing buckets ex .. rhx sizes
    size_t i = 0;
    // for each ex, ey .. hz ...rhz
    for (auto &aspcdat : all_channel_collector.data) {
      if (aspcdat.data.size()) {        // ex has size ... rhz has size ?
        if (!aspcdat.data[f].empty()) { // does ex has 512 hz ... yes! this is extra security
          std::swap(this->d[i], aspcdat.data[f]);
        }
      }
      ++i; // local ex ey ... rhz
    }
  }

  double f = DBL_MAX;            //!< frequency for this container
  std::vector<std::vector<T>> d; //!< contains all spectra for this frequency f; for ex, ey ... rhz
};

#endif // buckets
