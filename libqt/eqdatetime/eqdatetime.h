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

#ifndef EQDATETIME_H
#define EQDATETIME_H

#include "doubles.h"
#include "eqdatetime_global.h"
#include <QDate>
#include <QDateTime>
#include <QTime>
#include <QTimeZone>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdlib>

#include <QDebug>
#define grid_time_rr 64    //!< in order to make RR the jobs must be on a 64 s grid; if the job is downfiltered and virtual sampling is on 64s
#define grid_time_60 60    //!< in in case to use better reading time - no rr desired of self made
#define grid_time_1 1      //!< no grid, just full seconds
#define min_delta_low 24   //!< min break between jobs
#define min_delta_high 107 //!< for HF jobs where the ADU has stopped BUT still writes data to the disk

/*!
   \brief The eQDateTime class a date time class with sub-milli seconds; always UTC<br>
   Timings are calculated always from start_sample and use_samples; stop_samples is calculated or mapped back<br>
   does not support sample freqs like 16.66 or so<br>
   for samples we use quint64 <br>
   for seconds we use qint64 - because that is the format of QDatetime<br>
   size_t will be stored as quint64 internally

 */

class eQDateTime : public QDateTime {

private:
  double sample_freq = 0.0;
  double frac_msecs = 0.0; //!< everything smaller than a ms; that is already with f > 8 Hz the case (8Hz = 0.125), 0.5 means half a millisecond 0.0005
  quint64 wl = 0;          //!< 1024 would be date time of window end 0 .... 1023
  quint64 stacks = 0;
  quint64 start_sample = 0;
  quint64 num_samples = 0;
  quint64 use_samples = 0;
  qint64 samples_per_sec = 0; //! convinience to be set by sample freq
  qint64 secs_per_sample = 0; //! convinience to be set by sample freq
  bool fullsec_mode = false;
  quint64 nsteps_start = 0;
  quint64 nsteps_stop = 0;

public:
  /*!
   * \brief eQDateTime default constructor - and sets UTC
   */
  eQDateTime();
  eQDateTime(const qint64 &secs_since_1970, const qint64 &msecs = 0, const double &frac_msecs = 0, const double &sample_freq = 0, const quint64 &num_samples = 0, const quint64 &start_sample = 0, const quint64 &use_samples = ULLONG_MAX);
  void set_date_time(const qint64 &secs_since_1970, const qint64 &msecs = 0, const double &frac_msecs = 0, const double &sample_freq = 0, const quint64 &num_samples = 0, const quint64 &start_sample = 0, const quint64 &use_samples = ULLONG_MAX);
  void set_date_time_double(const double &double_secs_since_1970, const double &sample_freq = 0, const quint64 &num_samples = 0, const quint64 &start_sample = 0, const quint64 &use_samples = ULLONG_MAX);
  void set_date_time_int_doublefracs(const qint64 &secs_since_1970, const double &double_fracseconds, const double &sample_freq = 0, const quint64 &num_samples = 0, const quint64 &start_sample = 0, const quint64 &use_samples = ULLONG_MAX);

  /*!
   * \brief update_time_stamp_from_string to use this set up the class before; used to textedit 64bit
   * \param timestamp_str : only numbers; seconds since 1970
   */
  void update_time_stamp_from_string(const QString &timestamp_str);

  QString timestamp_to_str() const;

  eQDateTime(const eQDateTime &edt);
  eQDateTime(const QDateTime &date);
  eQDateTime(const QDate &date);
  eQDateTime(const QDate &date, const QTime &time, const QTimeZone &timezone = QTimeZone::utc());

  /*!
   * \brief set_window_length
   * \param wl wl may be different from use_samples
   */
  void set_window_length(const quint64 &wl);
  quint64 get_window_length() const;

  /*!
   * \brief set_sample_freq - essential for most calculations; together with num_samples; turns on off stepping
   * \param f frequency in Hz; 0 TURNS OFF
   */
  void set_sample_freq(const double &f);
  double get_sample_freq() const;

  //    does not work on greater increments
  /*!
   * \brief check_get_change_stop_time - if 8Hz it will pass through, if 0.25 it must increment / decrement by 4s
   * \param new_stop_time_msecs_since_1970 - that can be use natively and will be rounded to seconds
   * \param inc if true increments, else decrement
   * \return possible new stoptime as seconds since 1970  - after use this function, check and set in case or use cut function in case
   */
  qint64 check_get_change_stop_time(const qint64 &new_stop_time_msecs_since_1970);

  qint64 check_get_change_start_time(const qint64 &new_start_time_msecs_since_1970);

  /*!
   * \brief set_num_samples - needed if you work with ats files
   * \param num_samples
   */
  void set_num_samples(const quint64 &num_samples);
  quint64 get_num_samples() const;

  /*!
   * \brief set_start_sample set where to start, e.g. 0, 1023, 4095 and so on; hence 0...1023 are 1024 samples and is one second in case of 1024Hz
   * \param start_sample
   * \param use_samples 0 = take the already existing use_samples (default); take ULLONG_MAX to take all from beginning to end or set value
   * \return
   */
  bool set_start_sample(const quint64 &start_sample, const quint64 &use_samples = 0);
  quint64 get_start_sample() const;

  /*!
   * \brief get_stop_sample end of; there is NO setter for stop sample; use start and use set_use_samples
   * \return
   */
  quint64 get_stop_sample() const;

  /*!
   * \brief set_use_samples max samples to use; that is maybe not the window length;
   * \param use_samples; if ULLONG_MAX take all num_samples
   * \return
   */
  bool set_use_samples(const quint64 &use_samples = ULLONG_MAX);
  quint64 get_use_samples() const;

  /*!
   * \brief set_fullsec_mode when using new start times or spin boxes we can enforce start time to full seconds
   * \param on_true
   * \return
   */
  bool set_fullsec_mode(const bool &on_true);
  bool get_fullsec_mode() const;

  /*!
   * \brief measdir_time
   * \return "meas_" + this->toString("yyyy-MM-dd") + "_" + this->toString("hh-mm-ss");
   */
  QString measdir_time() const;

  /*!
   * \brief set_meastime
   * \param meastimestring String("yyyy-MM-dd") + "_" + toString("hh-mm-ss")
   */
  void set_meastime(const QString &meastimestring);
  QString measdoc_part_time(const quint64 &nsamples) const;

  /*!
   * \brief cuts set start and use; if full second mode is active a sample start of 5 will be mapped to 4 in case of 0.25Hz<br>
   * from start and use we can calculate start time and stop time
   * \param starts start sample
   * \param use samples to use
   * \return
   */
  bool cuts(const quint64 &starts, const quint64 &use, const bool &force_min_change = false);

  /*!
   * \brief sync_samples_other calculates start sample and use samples in oreder to fit other time
   * \param edt_time_starts_other
   * \param edt_time_stops_other
   * \return use samples, returns 0 if fails
   */
  quint64 sync_samples_other(const eQDateTime &edt_time_starts_other, const eQDateTime &edt_time_stops_other);

  /*!
   * \brief sample_time
   * \param nsamples if 0 start ... ULongLong end
   * \return eQdateTime of the sample or end of recording
   */
  eQDateTime sample_time(const quint64 &nsamples) const;

  /*!
   * \brief samples_to_seconds returns length in full seconds
   * \param num_samples length of filter (window may not really appropriate here)
   * \param samples_really_used if a filter is 471 and we use half we have 235.5
   * \param use_half_for_filter in case we have a filter, we want 1/2 length
   * \return seconds used for the 235.5 finally (as int for usage in datetime funcs)
   */

  qint64 samples_to_seconds(const quint64 &num_samples, quint64 &samples_really_used, const bool &use_half_for_filter = false);

  /*!
   * \brief sample_unix_time_stamp
   * \param nsamples
   * \return DOES NOT return the exact time; returns the next LESS time of a full second - 5 *0.25Hz returns 1
   */
  qint64 sample_unix_time_stamp(const quint64 &nsamples) const;

  /*!
   * \brief samples_to samples to this time; beware if f sample < 1; see corrected time
   * \param stops this time
   * \param corrected_time the newly corrected time in case we could not find time; for example 0.25Hz you can not set 3 because we can only find 0,4,8 and so on
   * \return samples to this time ; equal or less - in case you set 7 you get 4 in this example and a corrected start time
   */

  quint64 samples_to(const eQDateTime &stops, eQDateTime *corrected_time = nullptr) const;

  void to_full_minute(const bool round_up = true);

  qint64 get_secs_per_sample() const;

  /*!
   * \brief ISO_date_time
   * \param nsamples if 0 start time, if ULongLong then stop time
   * \return
   */
  QString ISO_date_time(const quint64 &nsamples = 0) const;
  QString ISO_date_time_frac(const quint64 &nsamples = 0) const;

  QString ISO_date_time_sample(const quint64 &nsamples = ULLONG_MAX) const;
  QString ISO_date_time_frac_sample(const quint64 &nsamples = ULLONG_MAX) const;

  eQDateTime &operator=(const QDateTime &e) {
    if (this == &e)
      return *this;
    QDateTime::operator=(e);
    return *this;
  }

  eQDateTime &operator=(const eQDateTime &edt) {
    if (this == &edt)
      return *this;
    QDateTime::operator=(edt);
    //  this->setTimeZone(QTimeZone::utc());
    this->set_sample_freq(edt.sample_freq);
    this->frac_msecs = edt.frac_msecs;
    this->num_samples = edt.num_samples;
    this->start_sample = edt.start_sample;
    this->use_samples = edt.use_samples;
    this->nsteps_start = edt.nsteps_start;
    return *this;
  }

  friend bool operator<(const eQDateTime &lhs, const eQDateTime &rhs) {

    double l = double(lhs.toMSecsSinceEpoch()) + lhs.frac_msecs;
    double r = double(rhs.toMSecsSinceEpoch()) + rhs.frac_msecs;
    return l < r;

    // return ( ( double(lhs.toMSecsSinceEpoch()) + lhs.frac_msecs) <  ( double(rhs.toMSecsSinceEpoch()) + rhs.frac_msecs) );
  }

  friend bool operator>(const eQDateTime &lhs, const eQDateTime &rhs) {
    return ((double(lhs.toMSecsSinceEpoch()) + lhs.frac_msecs) > (double(rhs.toMSecsSinceEpoch()) + rhs.frac_msecs));
  }

  friend bool operator==(const eQDateTime &lhs, const eQDateTime &rhs) {

    return ((double(lhs.toMSecsSinceEpoch()) + lhs.frac_msecs) == (double(rhs.toMSecsSinceEpoch()) + rhs.frac_msecs));
  }
  friend bool operator!=(const eQDateTime &lhs, const eQDateTime &rhs) {
    return ((double(lhs.toMSecsSinceEpoch()) + lhs.frac_msecs) != (double(rhs.toMSecsSinceEpoch()) + rhs.frac_msecs));
  }

  //!< @todo ISO times with frac msecs;
};

#endif // EQDATETIME_H
