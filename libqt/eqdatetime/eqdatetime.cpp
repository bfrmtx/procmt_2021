#include "eqdatetime.h"
#include <QTimeZone>

// #ifdef ANDROID
// namespace std {
// typedef struct {
//     long long quot;
//     long long rem;
// } lldiv_t;
// lldiv_t lldiv(long long int numer, long long int denom) {
//     lldiv_t r;
//     r.quot = numer / denom;
//     r.rem = numer - r.quot * denom;
//     return r;
// }
// }
// #endif

bool eQDateTime::cuts(quint64 const &starts, const quint64 &use, const bool &force_min_change) {

  if (!this->num_samples)
    return false;
  if ((starts == this->start_sample) && (use == this->use_samples))
    return true;

  if (this->fullsec_mode && this->samples_per_sec) {

    // want to increment - in case multiple samples when fullsecmode is active;
    // in fullsec mode the start sample IS ALREADY on the second grid
    // the start sample will jump when set to full seconds mode
    bool inc = (starts > this->start_sample);
    bool dec = (starts < this->start_sample);

    quint64 diff_start = 0;
    quint64 diff_steps;
    quint64 old_steps_start = this->nsteps_start;
    if (inc) {
      diff_start = starts - this->start_sample;
    }

    if (dec) {
      diff_start = this->start_sample - starts;
    }
    diff_steps = diff_start / quint64(this->samples_per_sec);

    // if I only change use dont change
    if (force_min_change && !diff_steps && diff_start)
      diff_steps = 1;
    if (diff_steps) {
      if (inc) {
        this->nsteps_start += diff_steps;
      } else if (dec) {
        if (this->nsteps_start > diff_steps)
          this->nsteps_start -= diff_steps;
        else
          this->nsteps_start = 0;
      }

      // try to set the start sample
      if ((quint64(this->samples_per_sec) * this->nsteps_start) < (this->num_samples)) {
        this->start_sample = quint64(this->samples_per_sec) * this->nsteps_start;
      } else
        this->nsteps_start = old_steps_start;
    }

    if (use != this->use_samples) {
      this->use_samples = use;
    }

  }

  else {

    // can not be greater as
    if (starts < this->num_samples) {
      this->start_sample = starts;
      this->use_samples = use;
    }
  }

  // shrink use in case
  if ((this->start_sample + this->use_samples) > (this->num_samples)) {
    this->use_samples = this->num_samples - this->start_sample;
  }

  if (this->start_sample == starts && (this->use_samples == use))
    return true;
  else
    return false;
}

quint64 eQDateTime::sync_samples_other(const eQDateTime &edt_time_starts_other, const eQDateTime &edt_time_stops_other) {
  quint64 starts = this->samples_to(edt_time_starts_other);
  quint64 samples_to = this->samples_to(edt_time_stops_other) - starts;

  // that is more than we have
  if (starts + samples_to > this->num_samples) {
    this->use_samples = 0;
    return 0;
  }

  this->start_sample = starts;
  this->use_samples = samples_to;

  return this->use_samples;
}

eQDateTime::eQDateTime() :
    QDateTime() {
  this->setTimeZone(QTimeZone::utc());
  this->setSecsSinceEpoch(0);
}

eQDateTime::eQDateTime(const qint64 &secs_since_1970, const qint64 &msecs, const double &frac_msecs, const double &sample_freq, const quint64 &num_samples, const quint64 &start_sample, const quint64 &use_samples) {
  this->setTimeZone(QTimeZone::utc());
  this->setSecsSinceEpoch(0);
  this->setMSecsSinceEpoch((secs_since_1970 * 1000) + msecs);
  this->frac_msecs = frac_msecs;
  this->set_sample_freq(sample_freq);
  this->num_samples = num_samples;
  this->start_sample = start_sample;
  this->use_samples = use_samples;
  this->cuts(this->start_sample, this->use_samples);
}

eQDateTime::eQDateTime(const eQDateTime &edt) :
    QDateTime() {
  this->setTimeZone(QTimeZone::utc());
  // QDateTime::operator =(edt);
  eQDateTime::operator=(edt);
  this->cuts(this->start_sample, this->use_samples);
}

eQDateTime::eQDateTime(const QDateTime &date) :
    QDateTime(date) {
  this->setTimeZone(QTimeZone::utc());
}

eQDateTime::eQDateTime(const QDate &date) :
    QDateTime(date, QTime(0, 0, 0)) {
  this->setTimeZone(QTimeZone::utc());
}

eQDateTime::eQDateTime(const QDate &date, const QTime &time, const QTimeZone &spec) :
    QDateTime(date, time, spec) {
  this->setTimeZone(QTimeZone::utc());
}

void eQDateTime::set_window_length(const quint64 &wl) {
  this->wl = wl;
}

quint64 eQDateTime::get_window_length() const {
  return wl;
}

void eQDateTime::set_sample_freq(const double &f) {
  this->sample_freq = f;

  // allow round errro from XML text files for example
  if (f >= 0.99) {
    this->secs_per_sample = 0;
    this->samples_per_sec = qint64(this->sample_freq);
  } else if (f < 1.0) {
    this->samples_per_sec = 0;
    this->secs_per_sample = qint64(1. / this->sample_freq);
  }
}

double eQDateTime::get_sample_freq() const {
  return this->sample_freq;
}

qint64 eQDateTime::check_get_change_stop_time(const qint64 &new_stop_time_msecs_since_1970) {

  // can always return on full seconds
  if (this->sample_freq > 0.999) {
    return new_stop_time_msecs_since_1970 / 1000;
  }

  // make shure that we UTC
  if (!this->fullsec_mode)
    qFatal("check_get_change_stop_time is not in full seconds mode; unpredictable results");

  // clean up sub seconds
  qint64 newtime = new_stop_time_msecs_since_1970 / qint64(1000);

  // rounded to seconds use
  qint64 use = this->sample_unix_time_stamp(this->start_sample + this->use_samples);

  //  latest eraliest full second
  qint64 latest = this->sample_unix_time_stamp(this->num_samples);
  qint64 earliest = this->sample_unix_time_stamp(0);

  // force increase

  if ((newtime > use) && (newtime < latest)) {
    if (newtime < (use + this->secs_per_sample)) {
      if ((use + this->secs_per_sample) < latest)
        return use + this->secs_per_sample;
      else
        return latest;
    } else {
      qint64 diff = (newtime - use) / this->secs_per_sample;
      if ((use + diff * this->secs_per_sample) < latest)
        return use + diff * this->secs_per_sample;
      else
        return latest;
    }
  }

  if ((newtime < use) && (newtime > earliest)) {
    if (newtime > (use - this->secs_per_sample)) {
      if ((use - this->secs_per_sample) > earliest)
        return use - this->secs_per_sample;
      else
        return earliest;
    } else {
      qint64 diff = (use - newtime) / this->secs_per_sample;
      if ((use - diff * this->secs_per_sample) > earliest)
        return use - diff * this->secs_per_sample;
      else
        return earliest;
    }
  }
  return 0;
}

qint64 eQDateTime::check_get_change_start_time(const qint64 &new_start_time_msecs_since_1970) {
  // can always return on full seconds
  if (this->sample_freq > 0.999) {
    return new_start_time_msecs_since_1970 / 1000;
  }

  // make shure that we UTC
  if (!this->fullsec_mode)
    qFatal("check_get_change_stop_time is not in full seconds mode; unpredictable results");

  // clean up sub seconds
  qint64 newtime = new_start_time_msecs_since_1970 / 1000;

  // rounded to starts
  qint64 starts = this->sample_unix_time_stamp(this->start_sample);

  //  latest eraliest full second
  qint64 latest = this->sample_unix_time_stamp(this->num_samples);
  qint64 earliest = this->sample_unix_time_stamp(0);

  // force increase

  if ((newtime > starts) && (newtime < latest)) {
    if (newtime < (starts + this->secs_per_sample)) {
      if ((starts + this->secs_per_sample) < latest)
        return starts + this->secs_per_sample;
      else
        return latest;

    } else {
      qint64 diff = (newtime - starts) / this->secs_per_sample;
      if ((starts + diff * this->secs_per_sample) < latest)
        return starts + diff * this->secs_per_sample;
      else
        return latest;
    }
  }

  // force decrease
  if ((newtime < starts) && (newtime > earliest)) {
    if (newtime > (starts - this->secs_per_sample)) {
      if ((starts - this->secs_per_sample) > earliest)
        return starts - this->secs_per_sample;
      else
        return earliest;
    } else {
      qint64 diff = (starts - newtime) / this->secs_per_sample;
      if ((starts - diff * this->secs_per_sample) > earliest)
        return starts - diff * this->secs_per_sample;
      else
        return earliest;
    }
  }
  return 0;
}

void eQDateTime::set_num_samples(const quint64 &num_samples) {
  this->num_samples = num_samples;
}

quint64 eQDateTime::get_num_samples() const {
  return this->num_samples;
}

bool eQDateTime::set_start_sample(const quint64 &start_sample, const quint64 &use_samples) {
  if (!use_samples)
    return this->cuts(start_sample, this->use_samples);
  else if (use_samples == ULLONG_MAX)
    return this->cuts(start_sample, this->num_samples);

  else
    return this->cuts(start_sample, use_samples);
}

quint64 eQDateTime::get_start_sample() const {
  return this->start_sample;
}

quint64 eQDateTime::get_stop_sample() const {
  return (this->start_sample + this->use_samples);
}

bool eQDateTime::set_use_samples(const quint64 &use_samples) {

  if (use_samples == ULLONG_MAX)
    return this->cuts(this->start_sample, this->num_samples);
  return this->cuts(this->start_sample, use_samples);
}

quint64 eQDateTime::get_use_samples() const {
  return this->use_samples;
}

bool eQDateTime::set_fullsec_mode(const bool &on_true) {
  if (this->sample_freq == 0.0)
    return false;
  this->fullsec_mode = on_true;
  if ((this->fullsec_mode == true) && (this->samples_per_sec != 0)) {
    this->nsteps_start = (this->start_sample) / quint64(this->samples_per_sec);
    this->start_sample = (this->nsteps_start * quint64(this->samples_per_sec));
    this->cuts(this->start_sample, this->use_samples);
  }
  // in other f <1 sample frequencies we always have full seconds
  return true;
}

bool eQDateTime::get_fullsec_mode() const {
  return this->fullsec_mode;
}

QString eQDateTime::measdir_time() const {
  return "meas_" + this->toString("yyyy-MM-dd") + "_" + this->toString("hh-mm-ss");
}

void eQDateTime::set_meastime(const QString &meastimestring) {
  this->setTimeZone(QTimeZone::utc());
  this->setSecsSinceEpoch(0);
  *this = QDateTime::fromString(meastimestring, "yyyy-MM-dd_hh-mm-ss");
}

QString eQDateTime::measdoc_part_time(const quint64 &nsamples) const {
  QString a = this->toString("yyyy-MM-dd") + "_" + this->toString("hh-mm-ss") + "_";
  eQDateTime endtime = this->sample_time(nsamples);
  QString b = endtime.toString("yyyy-MM-dd") + "_" + endtime.toString("hh-mm-ss");

  return a + b;
}

void eQDateTime::set_date_time(const qint64 &secs_since_1970, const qint64 &msecs, const double &frac_msecs, const double &sample_freq, const quint64 &num_samples, const quint64 &start_sample, const quint64 &use_samples) {
  this->setTimeZone(QTimeZone::utc());
  this->setMSecsSinceEpoch(secs_since_1970 * 1000 + msecs);
  this->frac_msecs = frac_msecs;
  this->set_sample_freq(sample_freq);
  this->num_samples = num_samples;
  this->start_sample = start_sample;
  this->use_samples = use_samples;

  this->cuts(this->start_sample, this->use_samples);
}

void eQDateTime::set_date_time_double(const double &double_secs_since_1970, const double &sample_freq, const quint64 &num_samples, const quint64 &start_sample, const quint64 &use_samples) {
  double dt_double = double_secs_since_1970;
  qint64 secs_since_1970 = qint64(dt_double);
  dt_double -= double(secs_since_1970);
  dt_double *= 1000.;
  qint64 msecs = qint64(dt_double);
  dt_double -= double(msecs);
  double frac_msecs = (dt_double / 1000.0);

  this->set_date_time(secs_since_1970, msecs, frac_msecs, sample_freq, num_samples, start_sample, use_samples);
}

void eQDateTime::set_date_time_int_doublefracs(const qint64 &secs_since_1970, const double &double_fracseconds, const double &sample_freq, const quint64 &num_samples, const quint64 &start_sample, const quint64 &use_samples) {
  double dt_double = double_fracseconds * 1000;
  qint64 msecs = qint64(dt_double);
  dt_double -= double(msecs);
  double frac_msecs = (dt_double / 1000.);
  this->set_date_time(secs_since_1970, msecs, frac_msecs, sample_freq, num_samples, start_sample, use_samples);
}

void eQDateTime::update_time_stamp_from_string(const QString &timestamp_str) {
  qint64 secs_since_1970 = qint64(timestamp_str.toLongLong());

  this->setTimeZone(QTimeZone::utc());
  this->setSecsSinceEpoch(0);
  this->setMSecsSinceEpoch(secs_since_1970 * 1000);
}

QString eQDateTime::timestamp_to_str() const {
  quint64 msecs_since_1970 = quint64(this->toMSecsSinceEpoch());

  return QString::number((msecs_since_1970 / 1000));
}

eQDateTime eQDateTime::sample_time(const quint64 &nsamples) const {

  if (nsamples == 0)
    return *this;
  if (this->sample_freq == 0.0)
    return *this;

  eQDateTime qdtnew;
  quint64 sam = this->num_samples; // return stop time in case
  if (nsamples <= this->num_samples)
    sam = nsamples;

  // length of time segment
  double tsseg = (double(sam)) / this->sample_freq;

  double fullsecs, fracsecs, fracsesc_msecs, msecs;

  fracsecs = modf(tsseg, &fullsecs);
  fracsecs *= 1000.0; // contains ms now

  fracsesc_msecs = modf(fracsecs, &msecs); // contains fracs of msescs

  qdtnew.set_date_time(qint64(fullsecs), (qint64(msecs) + this->toMSecsSinceEpoch()), fracsesc_msecs, this->sample_freq);

  return qdtnew;
}

qint64 eQDateTime::samples_to_seconds(const quint64 &num_samples, quint64 &samples_really_used, const bool &use_half_for_filter) {
  if (this->sample_freq == 0.0)
    return 0;
  double len = 0.0;
  len = double(num_samples);

  if (use_half_for_filter)
    len *= 0.5;

  if (len < 1.)
    return 0;

  // example for 128 Hz and 32 x filter
  // fracs = 1.8398s delay = 471 * 0.5 = 235 / 128.
  double fracs = (len / this->sample_freq);
  double fulldelay = 0;
  // delay can be more than 1s

  // miss = 0.8398
  double miss = modf(fracs, &fulldelay);

  quint64 secs = quint64(fulldelay);

  // 20 = 128 - (128 * 0.8398)              128                      -          107.5 -> 108
  samples_really_used = quint64(this->sample_freq - miss * this->sample_freq);

  // miss is in the sub sample ?
  if (((samples_really_used + 1) == quint64(this->sample_freq)) ||
      ((samples_really_used - 1) == quint64(this->sample_freq))) {
    samples_really_used = 0;
  }

  return qint64(secs);
}

qint64 eQDateTime::sample_unix_time_stamp(const quint64 &nsamples) const {
  if (!nsamples)
    return this->toMSecsSinceEpoch() / 1000;
  if (this->sample_freq == 0.0)
    return this->toMSecsSinceEpoch() / 1000;

  // length of time segment
  double tsseg = double(nsamples) / this->sample_freq;

  double fullsecs, fracsecs;

  fracsecs = modf(tsseg, &fullsecs);

  return (this->toMSecsSinceEpoch() / 1000) + (qint64(fullsecs));
}

quint64 eQDateTime::samples_to(const eQDateTime &stops, eQDateTime *corrected_time) const {

  double startsat = double(this->toMSecsSinceEpoch() + this->frac_msecs);
  double stopsat = double(stops.toMSecsSinceEpoch() + stops.frac_msecs);

  if (startsat > stopsat)
    return 0;
  if (this->num_samples) {
    if (stops > this->sample_time(this->num_samples))
      return 0;
  }
  if (this->sample_freq > 0.99)
    return quint64((((stopsat - startsat) / 1000.) * this->sample_freq));

  // if we have 0.25Hz we step by 4 seconds
  double fullsteps = 0;
  double fracsteps = 0;

  double nsteps = ((((stopsat - startsat) / 1000.) * this->sample_freq));

  fracsteps = modf(nsteps, &fullsteps);

  if (corrected_time != nullptr)
    *corrected_time = this->sample_time(quint64(fullsteps));

  return quint64(fullsteps);
}

void eQDateTime::to_full_minute(const bool round_up) {
  qint64 msecs_since = this->toMSecsSinceEpoch();
  qint64 newtime = msecs_since % qint64(60000);
  msecs_since -= newtime;
  this->frac_msecs = 0.0;
  if (round_up)
    msecs_since += qint64(60000);
  this->setMSecsSinceEpoch(msecs_since);
}

qint64 eQDateTime::get_secs_per_sample() const {
  return this->secs_per_sample;
}

QString eQDateTime::ISO_date_time_frac(const quint64 &nsamples) const {
  //    if (!nsamples) {
  //        QString dt = this->toString(Qt::ISODate);
  //        if (dt.endsWith("Z")) {
  //            dt.remove(dt.size()-1, 1);
  //        }
  //        std::lldiv_t rem;
  //        rem = std::lldiv(this->toMSecsSinceEpoch(), 1000);

  //        if (rem.rem) {
  //            double fracs = 0.001 * double(rem.rem);
  //            if (this->frac_msecs != 0.0) fracs += this->frac_msecs;
  //            QString num = QString::number(fracs, 'f', 6);
  //            num.remove(0,2);
  //            dt.append("." + num);
  //        }

  //        return dt;
  //    }

  eQDateTime time = this->sample_time(nsamples);
  QString dt = time.toString(Qt::ISODate);
  if (dt.endsWith("Z")) {
    dt.remove(dt.size() - 1, 1);
  }
  //    std::lldiv_t rem;
  //    rem = std::lldiv(this->toMSecsSinceEpoch(), 1000);

  qint64 msecs(time.toMSecsSinceEpoch() - time.toSecsSinceEpoch() * 1000); // diff in msecs
  double dotsecs = (double(msecs) / 1000.) + time.frac_msecs / 1000.;
  if (dotsecs > 0.000000000000001) {
    QString num = QString::number(dotsecs, 'f', 12);
    num.remove(0, 2); // remove "0."
    dt.append("." + num);
  }

  //    if (rem.rem) {
  //        double fracs = 0.001 * double(rem.rem);
  //        if (this->frac_msecs != 0.0) fracs += this->frac_msecs;
  //        QString num = QString::number(fracs, 'f', 6);
  //        num.remove(0,2);
  //        dt.append("." + num);
  //    }

  return dt;
}

QString eQDateTime::ISO_date_time(const quint64 &nsamples) const {
  if (!nsamples) {
    QString dt = this->toString(Qt::ISODate);
    if (dt.endsWith("Z")) {
      dt.remove(dt.size() - 1, 1);
    }
    return dt;
  }

  eQDateTime time = this->sample_time(nsamples);
  QString dt = time.toString(Qt::ISODate);
  if (dt.endsWith("Z")) {
    dt.remove(dt.size() - 1, 1);
  }
  return dt;
}
