#include "math_vector.h"



//  *************** some vector templates ***************************************
template <class T>
void cplx2ri(const std::vector<std::complex<T> >& in,
             std::vector<T>& treal, std::vector<T>& timag) {

    if (!in.size()) return;
    if (in.size() != treal.size()) treal.resize(in.size());
    if (in.size() != timag.size()) timag.resize(in.size());

    for (size_t i = 0; i < in.size(); ++i) {
        treal[i] = real(in[i]);
        timag[i] = imag(in[i]);
    }
}

template <class T>
void ri2cplx(const std::vector<T>& real, const std::vector<T>& imag,
             std::vector<std::complex<T> >& cplx) {

    if (real.size() != imag.size()) return;
    if (!real.size()) return;
    if (cplx.size() != real.size()) cplx.resize(real.size());

    for (size_t i = 0; i < real.size(); ++i) {
        cplx[i] = std::complex<T>(real, imag);
    }
}





math_vector::math_vector(QObject *parent)  : QObject(parent)
{
    this->channel = -1;
}

math_vector::~math_vector()
{

}

size_t math_vector::akima_vector(const std::vector<double> &vcalfreqs, const std::vector<std::complex<double> > &vcalentries,
                                 const std::vector<double> &newfreqs, std::vector<std::complex<double> > &calibipl) const {

    size_t cols = newfreqs.size();
    if (calibipl.size() != cols) calibipl.resize(cols);


    size_t i;
    size_t j, k;


    std::vector<double> reals;
    std::vector<double> imags;
    std::vector<double> newreals;
    std::vector<double> newimags;
    std::vector<double> b, c, d;
    // std::vector<double> f;


    std::vector<double>::const_iterator cfb, cfe, cyrb, cyre, cyib, cyie, cnfb, cnfe;
    std::vector<double>::iterator bb, be, cb, ce, db, de;


    size_t old_cols;

    // that must be the row size of the calibration matrix

    old_cols = vcalfreqs.size();
    reals.resize(old_cols);
    imags.resize(old_cols);
    b.resize(old_cols); c.resize(old_cols); d.resize(old_cols);
    cplx2ri<double>(vcalentries, reals, imags);
    newreals.resize(cols);
    newimags.resize(cols);
    // f = vcalfreqs;

    cfb = vcalfreqs.begin();
    cfe = vcalfreqs.end();
    cyrb = reals.begin();
    cyre = reals.end();
    cyib = imags.begin();
    cyie = imags.end();
    bb = b.begin();
    be = b.end();
    cb = c.begin();
    ce = c.end();
    db = d.begin();
    de = d.end();
    cnfb = newfreqs.begin();
    cnfe = newfreqs.end();


    if (old_cols > 1) {

        //qDebug()  << "akima run" << rows << cols << old_cols;
        //take the frequencies from calibration files, the real or imag from calibration and calculate the set of b,c,d coefficients
        // real part
        iter::aspline<double, std::vector<double>::const_iterator, std::vector<double>::iterator > (cfb, cfe, cyrb, cyre, bb, be, cb, ce, db, de);
        cfb = vcalfreqs.begin();
        cfe = vcalfreqs.end();
        cyrb = reals.begin();
        cyre = reals.end();
        cyib = imags.begin();
        cyie = imags.end();
        bb = b.begin();
        be = b.end();
        cb = c.begin();
        ce = c.end();
        db = d.begin();
        de = d.end();
        cnfb = newfreqs.begin();
        for (j = 0; j < newfreqs.size(); ++j) {
            newreals[j] = iter::seval<double, std::vector<double>::const_iterator> (cnfb, cfb, cfe, cyrb, bb, cb, db);
            //qDebug() << j << newfreqs.at(j) << *cnfb << newreals.at(j);
            ++cnfb;

        }
        // imaginary part
        iter::aspline<double, std::vector<double>::const_iterator, std::vector<double>::iterator > (cfb, cfe, cyib, cyie, bb, be, cb, ce, db, de);
        cfb = vcalfreqs.begin();
        cfe = vcalfreqs.end();
        cyrb = reals.begin();
        cyre = reals.end();
        cyib = imags.begin();
        cyie = imags.end();
        bb = b.begin();
        be = b.end();
        cb = c.begin();
        ce = c.end();
        db = d.begin();
        de = d.end();
        cnfb = newfreqs.begin();
        for (j = 0; j < newfreqs.size(); ++j) {
            newimags[j] = iter::seval<double, std::vector<double>::const_iterator> (cnfb, cfb, cfe, cyib, bb, cb, db);
            ++cnfb;
        }



        //            for (k = 0; k < newfreqs.size(); ++k) {
        //                qDebug() <<  real( calibipl[k]);
        //               qDebug() <<  newreals[k];
        //               qDebug() <<  newimags[k];
        //            }
        for (k = 0; k < newimags.size(); ++k) {
            calibipl[k] = std::complex<double> (newreals[k], newimags[k]);
        }

    }
    else {
        for (i = 0; i < calibipl.size(); ++i) calibipl[i] = std::complex<double> (1.0, 0.);
        //--qDebug()  << "akima skip" << rows << cols << old_cols;

    }




    return calibipl.size();


}

size_t math_vector::akima_vector_double(const std::vector<double> &vcalfreqs, const std::vector<double> &vcalentries, const std::vector<double> &newfreqs, std::vector<double> &calibipl) const
{




    // newfreqs                        // the new x-axis or frequencies
    calibipl.resize(newfreqs.size());  // the result - the new y axis or amplitude / phase



    size_t j;


    // nodes
    std::vector<double> b(vcalfreqs.size());
    std::vector<double> c(vcalfreqs.size());
    std::vector<double> d(vcalfreqs.size());


    std::vector<double>::const_iterator cfb, cfe, cyrb, cyre, cnfb, cnfe;
    std::vector<double>::iterator bb, be, cb, ce, db, de;


    size_t old_cols;

    // that must be the row size of the calibration matrix

    old_cols = vcalfreqs.size();

    b.resize(old_cols); c.resize(old_cols); d.resize(old_cols);

    // f = vcalfreqs;

    cfb = vcalfreqs.begin();
    cfe = vcalfreqs.end();
    cyrb = vcalentries.begin();
    cyre = vcalentries.end();

    bb = b.begin();
    be = b.end();
    cb = c.begin();
    ce = c.end();
    db = d.begin();
    de = d.end();
    cnfb = newfreqs.begin();
    cnfe = newfreqs.end();




    //qDebug()  << "akima run" << rows << cols << old_cols;
    //take the frequencies from calibration files, the real or imag from calibration and calculate the set of b,c,d coefficients
    // real part
    iter::aspline<double, std::vector<double>::const_iterator, std::vector<double>::iterator > (cfb, cfe, cyrb, cyre, bb, be, cb, ce, db, de);

    cfb = vcalfreqs.begin();
    cfe = vcalfreqs.end();
    cyrb = vcalentries.begin();

    bb = b.begin();
    cb = c.begin();
    db = d.begin();
    cnfb = newfreqs.begin();

    for (j = 0; j < newfreqs.size(); ++j) {
        calibipl[j] = iter::seval<double, std::vector<double>::const_iterator> (cnfb, cfb, cfe, cyrb, bb, cb, db);
        //qDebug() << j << newfreqs.at(j) << *cnfb << calibipl.at(j);
        ++cnfb;

    }

    return calibipl.size();
}

std::vector<double> math_vector::merge_sorted_low_to_high_outside(const std::vector<double> &orig, const std::vector<double> &inmerge)
{
    auto orig_mm = std::minmax_element(orig.begin(), orig.end());
    std::vector<double> out;
    out.reserve(inmerge.size());

    size_t j = 0;
    bool insert = true;
    for (auto &v: inmerge) {

        if ( v < *orig_mm.first) out.push_back(v);
        else if ((v > *orig_mm.first)  && insert)  {
            for ( j = 0; j < orig.size(); ++j ) out.push_back(orig.at(j));
            insert = false;
        }
        else if (v > *orig_mm.second) out.push_back(v);
    }

    return out;
}

int math_vector::merge_sorted_low_to_high_outside_join(const std::vector<double> &forig, const std::vector<double> &finmerge, std::vector<double> &foutmerge,
                                                       const std::vector<double> &aorig, const std::vector<double> &ainmerge, std::vector<double> &aoutmerge,
                                                       const std::vector<double> &porig, const std::vector<double> &pinmerge, std::vector<double> &poutmerge,
                                                       const bool overlapping)
{
    if (!forig.size() || !finmerge.size() || forig.size() < 3) return 0;


    double min = forig.at(0);
    double max = forig.back();
    if (max <= min) return 0;



    std::vector<double>::const_iterator ita = ainmerge.cbegin();
    std::vector<double>::const_iterator itp = pinmerge.cbegin();

    bool insert = true;


    for (auto &v: finmerge) {
        if ( v < min) {
            foutmerge.push_back(v);
            aoutmerge.push_back(*ita);
            poutmerge.push_back(*itp);
        }
        else if (insert) {
            for ( size_t j = 0; j < forig.size() ; ++j ) {
                foutmerge.push_back(forig.at(j));
                aoutmerge.push_back(aorig.at(j));
                poutmerge.push_back(porig.at(j));

            }
            insert = false;
        }
        else if (v > max) {
            foutmerge.push_back(v);
            aoutmerge.push_back(*ita);
            poutmerge.push_back(*itp);
        }
        ++ita;
        ++itp;

    }
    if (overlapping) {
        auto xmin_out = std::find(foutmerge.cbegin(), foutmerge.cend(), min);
        auto xmin_in = std::find(finmerge.cbegin(), finmerge.cend(), min);
        if ((xmin_in != finmerge.end()) && (xmin_out != foutmerge.end()) ) {
            auto pos_out = xmin_out -  foutmerge.cbegin();
            auto pos_in = xmin_in - finmerge.cbegin();
            aoutmerge[pos_out] = (aorig.front() + ainmerge.at(pos_in)) / 2.0;
            poutmerge[pos_out] = (porig.front() + pinmerge.at(pos_in)) / 2.0;


        }
        // find the position fmax of the original frequency vector - which was sortet!!
        auto xmax_out = std::find(foutmerge.cbegin(), foutmerge.cend(), max);
        auto xmax_in = std::find(finmerge.cbegin(), finmerge.cend(), max);
        if ((xmax_in != finmerge.end()) && (xmax_out != foutmerge.end()) ) {
            auto pos_out = xmax_out -  foutmerge.cbegin();
            auto pos_in = xmax_in - finmerge.cbegin();
            // merge the fmax from orig and mergerd
            aoutmerge[pos_out] = (aorig.back() + ainmerge.at(pos_in)) / 2.0;
            poutmerge[pos_out] = (porig.back() + pinmerge.at(pos_in)) / 2.0;


        }
    }



    return (int)foutmerge.size();
}

bool math_vector::sort_vectors(std::vector<double> &f, std::vector<double> &a, std::vector<double> &p, std::vector<double> &da, std::vector<double> &dp)
{
    if (f.size() != a.size()) return false;
    if (f.size() != p.size()) return false;
    if (f.size() != da.size()) return false;
    if (f.size() != dp.size()) return false;

    std::vector<std::pair<double, int>> v(f.size());
    size_t i, sz;

    sz = f.size();
    for (i = 0; i < f.size(); ++i) {
        v[i] = std::make_pair(f[i], i);
    }

    std::sort(v.begin(), v.end(), iter::cmp_pair_first_lt<std::pair<double, int>>());
    std::vector<double> tf(sz);
    std::vector<double> ta(sz);
    std::vector<double> tp(sz);
    std::vector<double> tda(sz);
    std::vector<double> tdp(sz);
    std::swap(f, tf);
    std::swap(a, ta);
    std::swap(p, tp);
    std::swap(da, tda);
    std::swap(dp, tdp);

    for (i = 0; i < sz; ++i) {
        f[i]  = tf.at(v.at(i).second);
        a[i]  = ta.at(v.at(i).second);
        p[i]  = tp.at(v.at(i).second);
        da[i] = tda.at(v.at(i).second);
        dp[i] = tdp.at(v.at(i).second);
    }


    return true;

}


bool math_vector::sort_vectors(std::vector<double> &f, std::vector<double> &a, std::vector<double> &p)
{
    if (f.size() != a.size()) return false;
    if (f.size() != p.size()) return false;

    std::vector<std::pair<double, double>> v(f.size());
    size_t i;

    for (i = 0; i < f.size(); ++i) {
        v[i] = std::make_pair(f[i], a[i]);
    }

    std::sort(v.begin(), v.end(), iter::cmp_pair_first_lt<std::pair<double, double>>());

    for (i = 0; i < f.size(); ++i) {
        a[i] = v[i].second;
    }


    for (i = 0; i < f.size(); ++i) {
        v[i] = std::make_pair(f[i], p[i]);
    }

    std::sort(v.begin(), v.end(), iter::cmp_pair_first_lt<std::pair<double, double>>());

    for (i = 0; i < f.size(); ++i) {
        f[i] = v[i].first;
        p[i] = v[i].second;
    }

    return true;
}

bool math_vector::sort_vectors(std::vector<int> &l, std::vector<double> &a, std::vector<double> &p)
{
    if (l.size() != a.size()) return false;
    if (l.size() != p.size()) return false;

    std::vector<std::pair<int, double>> v(l.size());
    size_t i;

    for (i = 0; i < l.size(); ++i) {
        v[i] = std::make_pair(l[i], a[i]);
    }

    std::sort(v.begin(), v.end(), iter::cmp_pair_first_lt<std::pair<int, double>>());

    for (i = 0; i < l.size(); ++i) {
        a[i] = v[i].second;
    }


    for (i = 0; i < l.size(); ++i) {
        v[i] = std::make_pair(l[i], p[i]);
    }

    std::sort(v.begin(), v.end(), iter::cmp_pair_first_lt<std::pair<int, double>>());

    for (i = 0; i < l.size(); ++i) {
        l[i] = v[i].first;
        p[i] = v[i].second;
    }

    return true;
}


bool math_vector::sort_vectors(std::vector<int> &l, std::vector<double> &a)
{
    if (l.size() != a.size()) return false;

    std::vector<std::pair<int, double>> v(l.size());
    size_t i;

    for (i = 0; i < l.size(); ++i) {
        v[i] = std::make_pair(l[i], a[i]);
    }

    std::sort(v.begin(), v.end(), iter::cmp_pair_first_lt<std::pair<int, double>>());

    for (i = 0; i < l.size(); ++i) {
        l[i] = v[i].first;
        a[i] = v[i].second;
    }

    return true;
}


bool math_vector::sort_vectors(std::vector<double> &f, std::vector<double> &a)
{
    if (f.size() != a.size()) return false;

    std::vector<std::pair<double, double>> v(f.size());
    size_t i;

    for (i = 0; i < f.size(); ++i) {
        v[i] = std::make_pair(f[i], a[i]);
    }

    std::sort(v.begin(), v.end(), iter::cmp_pair_first_lt<std::pair<double, double>>());

    for (i = 0; i < f.size(); ++i) {
        f[i] = v[i].first;
        a[i] = v[i].second;
    }

    return true;
}

bool math_vector::remove_duplicates_from_sorted(std::vector<double> &f, std::vector<double> &a, const bool emit_message)
{
    if (f.size() != a.size()) return false;

    QString message;

    std::vector<double> tf(f);
    std::vector<double> ta(a);

    f.clear();
    a.clear();

    f.push_back(tf.at(0));
    a.push_back(ta.at(0));

    size_t j = 1;
    for (size_t i = 1; i < tf.size(); ++i) {
        if (tf.at(i - j) != tf.at(i) ) {
            f.push_back(tf.at(i));
            a.push_back(ta.at(i));
            j = 1;
        }
        else {
            if (emit_message) {
                message = "math_vector::remove_duplicates_from_sorted -> ************* DUPLICATES please check " + QString::number(tf.at(i)) + " chopper on";
                emit this->math_message(channel, message);
            }

            ++j;
        }
    }

    return true;

}

bool math_vector::remove_duplicates_from_sorted(std::vector<double> &f, std::vector<double> &a, std::vector<double> &p, const bool emit_message)
{
    if (f.size() != a.size()) return false;
    if (f.size() != p.size()) return false;

    QString message;
    size_t sz = f.size();

    std::vector<double> tf;
    std::vector<double> ta;
    std::vector<double> tp;


    tf.reserve(sz); ta.reserve(sz); tp.reserve(sz);

    std::swap(f, tf);
    std::swap(a, ta);
    std::swap(p, tp);


    f.push_back(tf.at(0));
    a.push_back(ta.at(0));
    p.push_back(tp.at(0));


    size_t j = 1;
    for (size_t i = 1; i < tf.size(); ++i) {
        if (tf.at(i - j) != tf.at(i) ) {
            f.push_back(tf.at(i));
            a.push_back(ta.at(i));
            p.push_back(tp.at(i));
            j = 1;
        }
        else {
            if (emit_message) {
                message = "math_vector::remove_duplicates_from_sorted -> ************* DUPLICATES please check " + QString::number(tf.at(i)) + " chopper on";
                emit this->math_message(channel, message);
            }
            ++j;
        }
    }

    return true;
}

bool math_vector::remove_duplicates_from_sorted(std::vector<double> &f, std::vector<double> &a, std::vector<double> &p,
                                                std::vector<double> &da, std::vector<double> &dp,
                                                const bool emit_message) {

    if (f.size() != a.size()) return false;
    if (f.size() != p.size()) return false;
    if (f.size() != da.size()) return false;
    if (f.size() != dp.size()) return false;

    size_t sz = f.size();
    QString message;

    std::vector<double> tf;
    std::vector<double> ta;
    std::vector<double> tp;
    std::vector<double> tda;
    std::vector<double> tdp;
    tf.reserve(sz); ta.reserve(sz); tp.reserve(sz), tda.reserve(sz), tdp.reserve(sz);

    std::swap(f, tf);
    std::swap(a, ta);
    std::swap(p, tp);
    std::swap(da, tda);
    std::swap(dp, tdp);

    f.push_back(tf.at(0));
    a.push_back(ta.at(0));
    p.push_back(tp.at(0));
    da.push_back(tda.at(0));
    dp.push_back(tdp.at(0));


    size_t j = 1;
    for (size_t i = 1; i < tf.size(); ++i) {
        if (tf.at(i - j) != tf.at(i) ) {
            f.push_back(tf.at(i));
            a.push_back(ta.at(i));
            p.push_back(tp.at(i));
            da.push_back(tda.at(i));
            dp.push_back(tdp.at(i));
            j = 1;
        }
        else {
            if (emit_message) {
                message = "math_vector::remove_duplicates_from_sorted -> ************* DUPLICATES please check " + QString::number(tf.at(i)) + " chopper on";
                emit this->math_message(channel, message);
            }
            ++j;
        }
    }

    return true;

}



double math_vector::mean(const std::vector<double> &data)
{
    double sz = (double) data.size();
    return (std::accumulate(data.begin(), data.end(), 0.0) / sz);
}

std::vector<double> math_vector::gen_equidistant_logvector(const double start, const double stop, const size_t steps_per_decade)
{
    // dist would be log_stop - log_start
    // we calculate per decade
    std::vector<double> result;

    if ( (start <= 0) || (stop <= 0)) return result;
    if (stop < start) return result;

    double step = 1.0/ double(steps_per_decade);
    size_t i= 0;
    double value = DBL_MAX;

    do {
        double x = start * pow(10.0, double(i++));
        double lx = log10(x);
        for (size_t j = 0; j < steps_per_decade; ++j) {
            value = pow(10.0, lx + step * double(j));
            //Fsetting
            qDebug() << value;
            if (value <= stop) result.push_back(value);
        }
    } while (value <= stop);

    if ( (result.back() < stop) && result.size() ) result.push_back(stop);

    return result;
}

size_t math_vector::slot_parzen_vector(const std::vector<double> &freqs, const std::vector<double> &target_freqs, const double &prz_radius)
{

    if (!freqs.size() || !target_freqs.size()) return 0;
    const double max_parzen = 0.31;
    if (prz_radius <= 0.0) return 0;
    if (prz_radius >= max_parzen) return 0;


    double prz_rad = prz_radius;
    double step_change = 0.005;

    std::vector<double> selected_freqs;
    std::vector<std::vector<double>::const_iterator >  iter_lwr_bnds;
    std::vector<std::vector<double>::const_iterator >  iter_upr_bnds;
    std::vector<double>::const_iterator low;
    std::vector<double>::const_iterator high;
    std::vector<double>::const_iterator take;

    int max_tries = 40;
    int tries = 0;
/*
    size_t nlines = 3;    // left right

    if (freqs.back() < 0.065) {

        if (freqs.size() < 3 * nlines) return 0;
        auto tf_beg = target_freqs.cbegin();
        auto tf_end = target_freqs.cend();


        auto uf_beg = std::next(freqs.cbegin(), (nlines + 1));
        auto uf_end = std::prev(freqs.cend(), (nlines + 1));

        tf_beg =  std::lower_bound(tf_beg, tf_end, *freqs.cbegin()) ;




        for (; tf_beg != tf_end; ++tf_beg) {
            low =  std::lower_bound(uf_beg, uf_end, *tf_beg) ;
            qDebug() << *low << *tf_beg << *(std::next(low, 2));
            if ( (low != uf_end) && (low != uf_beg)) {
                take = std::prev(low, 1);
                if (*low == *tf_beg) {
                    high = std::next(low, nlines);
                    low = std::prev(low, nlines);
                    selected_freqs.push_back(*tf_beg);
                    iter_lwr_bnds.push_back(std::prev(low, nlines));
                    iter_upr_bnds.push_back(std::next(high, nlines));

                    qDebug() << "taken" << *(iter_lwr_bnds.back()) << *tf_beg << *(iter_upr_bnds.back());


                }
                else if ((*low > *tf_beg) && ( *take < *tf_beg)) {
                    high = std::next(low, (nlines -1));
                    take = std::prev(low, 1);
                    selected_freqs.push_back(*tf_beg);
                    iter_lwr_bnds.push_back(take);
                    iter_upr_bnds.push_back(high);
                    qDebug() << "taken" << *(iter_lwr_bnds.back()) << *tf_beg << *(iter_upr_bnds.back());


                }
            }
            // high = std::upper_bound(uf_beg, uf_end, *tf_beg) ; this will ONLY work if target frequency is REALLY inside


        }

        qDebug() << "line selector used";


        //        auto uf_beg = std::next(freqs.cbegin(), (nlines + 2));
        //        auto uf_end = std::prev(freqs.cend(), (nlines + 1));

        //        for (; uf_beg != uf_end; ++uf_beg) {
        //            low =  std::lower_bound(target_freqs.cbegin(), target_freqs.cend(), *uf_beg) ;
        //            high = std::upper_bound(target_freqs.cbegin(), target_freqs.cend(), *uf_beg) ;
        //            if (std::distance(low, high) > nlines) {
        //                selected_freqs.push_back(*low);
        //                iter_lwr_bnds.push_back(std::prev(low, nlines);
        //                iter_upr_bnds.push_back(std::next(high, nlines));
        //            }

        //        }

        qDebug() << "line selector used";



        //        iter_lwr_bnds.reserve(freqs.size()-2);
        //        selected_freqs.reserve(freqs.size()-2);
        //        iter_upr_bnds.reserve(freqs.size()-2);

        //        auto fbeg = freqs.cbegin() + 3;
        //        auto fend = freqs.cend() - 4;

        //        if (std::distance(fbeg, fend) > 3) {
        //            while (fbeg != fend) {
        //                if (std::find(target_freqs.begin(), target_freqs.end(), *fbeg) != target_freqs.end() ) {
        //                    iter_lwr_bnds.push_back(fbeg-1);
        //                    selected_freqs.push_back(*fbeg);
        //                    iter_upr_bnds.push_back(fbeg+1);
        //                }
        //                ++fbeg;
        //            }
        //        }


    }

    else do {
 */
    do {
        if (prz_rad < max_parzen) {
            selected_freqs.clear();
            iter_lwr_bnds.clear();
            iter_upr_bnds.clear();
            std::vector<double>::const_iterator  min_freq = freqs.begin();
            std::vector<double>::const_iterator  max_freq = freqs.end()-1;

            // eg. the zero element contains the DC part f = 0 Hz
            if (*min_freq == 0.0) ++min_freq;

            // copy the list
            std::vector<double> lower_prz_bounds = target_freqs;
            std::vector<double> upper_prz_bounds = target_freqs;


            //set the range of frequencies to be included in pazening
            // so if target f was 100 and prz_rad = 0.1 we have 90 .. 100 .. 110
            for ( size_t i = 0; i < lower_prz_bounds.size(); ++ i) {
                lower_prz_bounds[i] = lower_prz_bounds[i] - (lower_prz_bounds[i] * prz_rad);
                upper_prz_bounds[i] = upper_prz_bounds[i] + (upper_prz_bounds[i] * prz_rad);
            }

            // now find the itrators pointing to the valid areas of the frequency vector containing the parzen freqs
            // *iter_lwr_bnds[i] is value, iter_lwr_bnds - f.begin() is pos
            for (size_t i = 0; i < lower_prz_bounds.size(); ++i) {
                if ((lower_prz_bounds[i] >= *min_freq) && (upper_prz_bounds[i] <= *max_freq) ) {
                    low =  std::lower_bound(min_freq, max_freq, lower_prz_bounds[i]) ;
                    high = std::upper_bound(min_freq, max_freq, upper_prz_bounds[i]) ;
                    qDebug() << *high  << " <-> "   << *low;
                    qDebug() << 1.0 / (*high) << " <-> " << 1.0 / (*low) << " target: " << 1.0 / target_freqs[i];
                    if (high - low > 1) {
                        // all lower start points
                        iter_lwr_bnds.push_back(low);
                        // all upper end points
                        iter_upr_bnds.push_back(high);
                        // all freqs belong to that
                        selected_freqs.push_back(target_freqs[i]);
                    }
                    else {
                        qDebug() << "slot_parzen_vector -> no bounds" << lower_prz_bounds[i] << *min_freq << upper_prz_bounds[i] << *max_freq;
                        qDebug() << "slot_parzen_vector -> no bounds" << 1.0  / lower_prz_bounds[i] << 1.0/ (*min_freq) << 1.0 / upper_prz_bounds[i] << 1.0 / (*max_freq);

                    }
                }

            }

            ++tries;
            if(selected_freqs.size() < 1) {
                prz_rad += step_change;
                std::cerr << " math_vector::slot_parzen_vector ---> changed PARZEN! " << prz_rad << std::endl;
            }
        }

    } while ((selected_freqs.size() < 1) && (tries < max_tries));

    if (prz_rad != prz_radius) {
        emit this->parzen_radius_changed(prz_radius, prz_rad);
    }

    emit this->frequencies_selected(selected_freqs);



    std::vector<double> parzendists;

    // I must notify my mother
    if (!selected_freqs.size()) {
        std::cerr << " math_vector::slot_parzen_vector FAILED!!!" << std::endl;
        emit this->parzenvector_created(0, parzendists);
        return 0;
    }

    parzendists.resize(freqs.size());

    double dist = 0;
    double distsum = 0;
    size_t k;

    // for each of the frequencies found we make a parzen vector (with length of the incoming fft spectra freqs
    for ( size_t i = 0; i < selected_freqs.size(); ++i) {
        // no realloc here; reset to 0
        parzendists.assign(freqs.size(), 0.0);
        distsum = 0;
        dist = 0;
        low = iter_lwr_bnds[i];  // save the original iterpos
        high = iter_upr_bnds[i];
        while (low < high) {
            k = (low - freqs.begin()); // index of frequency vector is naturally the same as freq
            dist = ( abs(selected_freqs[i] - *low) * M_PI) / (selected_freqs[i] * prz_rad);
            // k is the index of freqs
            if (!dist) parzendists[k] = 1.0;
            else {
                dist = pow( ((sin(dist))/dist), 4.);
                //dist = ((sin(dist))/dist);
                parzendists[k] = dist;

            }
            distsum += dist;
            ++low;
            //if (dist > 0.0001) this->mtd->avgfs[i] += 1;
        }
        //this->mtd->bws[i] =  (this->mtd->avgfs[i] * this->mcd->get_key_dvalue("sample_freq")) / ( this->mcd->get_key_uivalue("read_samples") * M_PI);
        //       qDebug() << this->mtd->avgfs[i] << this->mcd->get_key_dvalue("sample_freq") << this->mcd->get_key_uivalue("read_samples");
        //       qDebug() << distsum << "at" << this->mtd->przfreq_sel.at(i) << *iter_lwr_bnds[i] << *iter_upr_bnds[i] << *iter_upr_bnds[i] - *iter_lwr_bnds[i] << j << "lines";
        for ( auto &val : parzendists) val /= distsum;
        emit this->parzenvector_created(selected_freqs.at(i), parzendists);
    }

    return 1;
}

size_t math_vector::take_five(const std::vector<double> &freqs, const std::vector<double> &target_freqs, const size_t odd_min_lines, const double &bw)
{
    if (!freqs.size() || !target_freqs.size()) {

        return 0;
    }

    size_t space = odd_min_lines/2;
    size_t lines = odd_min_lines;

    auto low = freqs.cbegin() + int(space) + 1;
    auto high = freqs.cend() - int(space) - 1;

    std::vector<double> selected_freqs;
    std::vector<size_t> selected_indices;
    std::vector<size_t> selected_indices_sizes;


    for (auto const &f : target_freqs) {
        if ( (f > *low) && (f < *high)) {
            selected_freqs.push_back(f);
        }
    }

    low = freqs.cbegin();
    high = freqs.cbegin();
    std::vector<double>::const_iterator plus;
    std::vector<double>::const_iterator minus;
    std::vector<double>::const_iterator take;



    for (auto const &f : selected_freqs) {
        low = high;
        while (*low < f) {
            ++low;
            high = low;
        }
        plus = low + 1;
        minus = low - 1;
        if ( (*plus - *low) < (*low - *minus)) take = plus;
        else take = minus;
        take = take - int(space);
        for (size_t i = 0; i < odd_min_lines; ++i) {
            selected_indices.push_back(size_t(take++ - freqs.cbegin()));
        }
        selected_indices_sizes.push_back(lines);


    }

    if (selected_indices.size() && (selected_indices_sizes.size() == selected_freqs.size()) ) {

        // emit frequencies FIRST
        emit this->frequencies_selected(selected_freqs);
        emit this->frequencies_fft_indices_selected(selected_indices, selected_indices_sizes);
    }


    qDebug() << "found";

    return 1;
}


void math_vector::lin_round_min(const double &min, const double one_ten_hundred_etc, double &new_min)
{
    new_min = min / one_ten_hundred_etc;
    new_min = floor(new_min) * one_ten_hundred_etc;
}

void math_vector::lin_round_max(const double &max, const double one_ten_hundred_etc, double &new_max)
{
    new_max = max / one_ten_hundred_etc;
    new_max = ceil(new_max) * one_ten_hundred_etc;
}

void math_vector::lin_round_min_max(const double &min, const double &max, const double one_ten_hundred_etc, double &new_min, double &new_max)
{
    new_min = min / one_ten_hundred_etc;
    new_min = floor(new_min) * one_ten_hundred_etc;
    new_max = max / one_ten_hundred_etc;
    new_max = ceil(new_max) * one_ten_hundred_etc;
}


bool math_vector::vector_size_if_min(std::vector<double>::const_iterator xcbeg, std::vector<double>::const_iterator xcend,
                                     std::vector<bool>::const_iterator selected_cbeg, std::vector<bool>::const_iterator selected_cend,
                                     const size_t min_valid_elements, const int control)
{
    size_t xdist = (size_t)std::distance(xcbeg, xcend);
    if ( xdist < min_valid_elements) {
        std::cerr << "x is smaller than: " << xdist << " required: " << min_valid_elements << std::endl;
        return false;
    }
    else if (control == 2) {
        return true;
    }

    size_t seldist = (size_t)std::distance(selected_cbeg, selected_cend);

    if (xdist != seldist) {
        std::cerr << " x and selection vector have different sizes: " << xdist << " " << seldist << std::endl;
        return false;

    }

    // don't check trues now
    if (control != 1) {
        size_t trues = (size_t)std::count(selected_cbeg, selected_cend, true);
        if (trues < min_valid_elements) {
            std::cerr << "selection vector has only true selections: " << trues << " required: " << min_valid_elements << std::endl;
            return false;

        }
    }
    return true;

}

bool math_vector::vector_size_if_min_cplx(std::vector<std::complex<double>>::const_iterator xcbeg, std::vector<std::complex<double>>::const_iterator xcend,
                                          std::vector<bool>::const_iterator selected_cbeg, std::vector<bool>::const_iterator selected_cend,
                                          const size_t min_valid_elements, const int control)
{
    size_t xdist = (size_t)std::distance(xcbeg, xcend);
    if ( xdist < min_valid_elements) {
        std::cerr << "x is smaller than: " << xdist << " required: " << min_valid_elements << std::endl;
        return false;
    }
    else if (control == 2) {
        return true;
    }

    size_t seldist = (size_t)std::distance(selected_cbeg, selected_cend);

    if (xdist != seldist) {
        std::cerr << " x and selection vector have different sizes: " << xdist << " " << seldist << std::endl;
        return false;

    }

    // don't check trues now
    if (control != 1) {
        size_t trues = (size_t)std::count(selected_cbeg, selected_cend, true);
        if (trues < min_valid_elements) {
            std::cerr << "selection vector has only true selections: " << trues << " required: " << min_valid_elements << std::endl;
            return false;

        }
    }
    return true;

}


bool math_vector::vector_sizes_if_min(std::vector<double>::const_iterator xcbeg, std::vector<double>::const_iterator xcend,
                                      std::vector<double>::const_iterator ycbeg, std::vector<double>::const_iterator ycend,
                                      std::vector<bool>::const_iterator selected_cbeg, std::vector<bool>::const_iterator selected_cend,
                                      const size_t min_valid_elements, const int control) {

    size_t xdist = (size_t)std::distance(xcbeg, xcend);
    if ( xdist < min_valid_elements) {
        std::cerr << "x is smaller than: " << xdist << " required: " << min_valid_elements << std::endl;
        return false;
    }

    size_t ydist = (size_t)std::distance(ycbeg, ycend);
    if ( xdist != ydist) {
        std::cerr << "x and y not of same size: " << xdist << " " << ydist << std::endl;
        return false;
    }
    else if (control == 2) {
        return true;
    }



    size_t seldist = (size_t)std::distance(selected_cbeg, selected_cend);
    if (xdist != seldist) {
        std::cerr << " x and selection vector have different sizes: " << xdist << " " << seldist << std::endl;
        return false;

    }
    // don't check trues now
    if (control != 1) {
        size_t trues = (size_t)std::count(selected_cbeg, selected_cend, true);
        if (trues < min_valid_elements) {
            std::cerr << "selection vector has only true selections: " << trues << " required: " << min_valid_elements << std::endl;
            return false;

        }
    }
    return true;

}

double math_vector::dmean(std::vector<double>::const_iterator xcbeg, std::vector<double>::const_iterator xcend,
                          std::vector<bool>::const_iterator selected_cbeg, std::vector<bool>::const_iterator selected_cend,
                          const size_t min_valid_elements, const int control) {

    if (!vector_size_if_min(xcbeg, xcend, selected_cbeg, selected_cend, min_valid_elements, control)) {
        return DBL_MAX;
    }
    // x only
    if (control == 2) {
        double mymean = std::accumulate(xcbeg, xcend, 0.0);
        return mymean / double(std::distance(xcbeg, xcend));
    }
    else {
        double sum = 0.0;
        size_t cnt = 0;
        for ( ; xcbeg != xcend; ++xcbeg, ++selected_cbeg) {
            if (*selected_cbeg) {
                sum += *xcbeg;
                ++cnt;
            }
        }
        return sum / double(cnt);
    }

    return DBL_MAX;

}

statmap math_vector::mk_variance_data(bool make_y) {

    statmap datamap;
    int i = 0;
    int j = 0;
    // th y data starts directly after x
    if (make_y) i = g_stat::xstat_size;

    // we could loop here but this is more easy to understand
    for (j = 0; j < g_stat::xstat_size; ++j) {
        datamap[j + i] =  DBL_MAX ;
        datamap[j + i] =  DBL_MAX;
        datamap[j + i] =  DBL_MAX;
        datamap[j + i] =  DBL_MAX;
        datamap[j + i] =  DBL_MAX;
        datamap[j + i] =  DBL_MAX;
        datamap[j + i] =  DBL_MAX;
        datamap[j + i] =  DBL_MAX;
        datamap[j + i] =  DBL_MAX;
        datamap[j + i] =  DBL_MAX;
        datamap[j + i] =  DBL_MAX;
        datamap[j + i] =  DBL_MAX;
        datamap[j + i] =  DBL_MAX;

    }

    return datamap;

}

statmap math_vector::mk_regression_data(const statmap &x_variance_data, const statmap &y_variance_data, bool &is_ok) {


    statmap datamap;

    int i;

    // at his moment y data war treated befor as x because it was calculated independently
    if ( (x_variance_data.at(g_stat::d_n_x) == y_variance_data.at(g_stat::d_n_x)) && (x_variance_data.at(g_stat::d_n_x) >= 4.00) ) {


        for (i = 0; i < g_stat::xstat_size; ++i) {
            if (x_variance_data.find(i) != x_variance_data.cend()) datamap[i] = x_variance_data.at(i);
        }


        for (i = 0; i < g_stat::xstat_size; ++i) {
            if (y_variance_data.find(i) != y_variance_data.cend()) datamap[g_stat::xstat_size + i] = y_variance_data.at(i);
        }



        datamap[g_stat::covariance_xy] = DBL_MAX;
        datamap[g_stat::correlation_xy] = DBL_MAX;
        datamap[g_stat::slope_xy] =  DBL_MAX;
        datamap[g_stat::abscissa_xy] = DBL_MAX;
        datamap[g_stat::sum_alldist_from_slope_xy] = DBL_MAX;

        is_ok = true;
    }
    else is_ok = false;


    return datamap;
}

statmap math_vector::mk_regression_result(const statmap& regression_data, bool &is_ok) {

    statmap datamap;


    if ( (regression_data.at(g_stat::d_n_x) == regression_data.at(g_stat::d_n_y)) && (regression_data.at(g_stat::d_n_x) >= 4.00) ) {

        for ( auto it = regression_data.cbegin(); it != regression_data.cend(); ++it ) {
            datamap[it->first] = it->second;
        }

        // copy convinience
        datamap[g_stat::delta_slope_xy] = DBL_MAX;
        datamap[g_stat::student_t_upper_quantile] =  DBL_MAX;
        datamap[g_stat::student_t_inv] = DBL_MAX;
        datamap[g_stat::student_t_upper_confidence_xy] = DBL_MAX;
        datamap[g_stat::student_t_lower_confidence_xy] = DBL_MAX;
        is_ok = true;
    }
    else {
        is_ok = false;
    }

    return statmap();

}


//////////////////////////////////////////////// regplot_data  //////////////////////////////////////////////////////////////////////////



regplot_data::regplot_data()
{

}

regplot_data::~regplot_data()
{
    // qDebug() << "regplot_data destroyed";
}

bool regplot_data::mkplot_slope(std::vector<double>::const_iterator xcbeg, std::vector<double>::const_iterator xcend,
                                std::vector<double> &x, std::vector<double> &y,
                                const statmap &regression_data,
                                const int ntimes_size,
                                const double newmin, const double newmax,
                                const double center_x, const double center_y,
                                const int slope_type_0s_1u_2l, const double extra_offsety) {

    std::vector<bool> selected;
    return this->mkplot_slope_if(xcbeg, xcend, x, y, selected.cbegin(), selected.cend(), regression_data,
                                 ntimes_size, newmin, newmax, center_x, center_y, slope_type_0s_1u_2l, extra_offsety);


}

bool regplot_data::mkplot_slope_if(std::vector<double>::const_iterator xcbeg, std::vector<double>::const_iterator xcend,
                                   std::vector<double> &x, std::vector<double> &y,
                                   std::vector<bool>::const_iterator selected_cbeg, std::vector<bool>::const_iterator selected_cend,
                                   const statmap &regression_data,
                                   const int ntimes_size,
                                   const double newmin, const double newmax,
                                   const double center_x, const double center_y,
                                   const int slope_type_0s_1u_2l, const double extra_offsety) {



    if (std::distance(xcbeg, xcend) < 4 ) {
        std::cerr << "x is smaller than: " << 4 << std::endl;
        return false;
    }

    this->d_n = regression_data.at(g_stat::d_n_x);
    this->d_slope = regression_data.at(g_stat::slope_xy);
    this->d_abscissa = regression_data.at(g_stat::abscissa_xy);
    this->d_lower_confidence = regression_data.at(g_stat::student_t_lower_confidence_xy);
    this->d_upper_confidence = regression_data.at(g_stat::student_t_upper_confidence_xy);
    this->d_varx = regression_data.at(g_stat::variance_x);
    this->d_student_t_inv = regression_data.at(g_stat::student_t_inv);
    this->d_meanx = regression_data.at(g_stat::mean_x);
    this->d_minx = regression_data.at(g_stat::min_x);
    this->d_maxx = regression_data.at(g_stat::max_x);

    double step = 0.0;
    double xmin, xmax;

    if (ntimes_size < 0) {
        x.resize(2);
        y.resize(2);


        if ( (newmin != newmax) && newmin < newmax ) {
            xmin = newmin;
            xmax = newmax;
            step = newmax - newmin;

        }
        else {
            xmin = this->d_minx;
            xmax = this->d_maxx;
            step = this->d_maxx - this->d_minx;
        }
    }

    else if (ntimes_size == 0) {
        x.resize((size_t)std::distance(xcbeg, xcend));
        y.resize((size_t)std::distance(xcbeg, xcend));
        std::copy(xcbeg, xcend, x.begin());
    }
    else {
        x.resize((size_t)std::distance(xcbeg, xcend) * ntimes_size);
        y.resize((size_t)std::distance(xcbeg, xcend) * ntimes_size);

        if ( (newmin != newmax) && newmin < newmax ) {
            xmin = newmin;
            xmax = newmax;
            step = (xmax - xmin) / (double (x.size() -1.));

        }
        else {
            auto resultxy = std::minmax_element (xcbeg, xcend);
            step = (*resultxy.second - *resultxy.first) / (double (x.size() -1.));
            xmin = *resultxy.first;
            xmax = *resultxy.second;

        }

    }

    // plot the center slope y = mx + b
    // this line should went through xmean and ymean
    if (slope_type_0s_1u_2l == 0) {

        // plot value y by x
        if (step == 0.0) {
            // plot
            if (selected_cbeg == selected_cend) {
                for (size_t i = 0; i < x.size(); ++i) {
                    y[i] = this->d_slope * x[i] + this->d_abscissa + extra_offsety;
                }
            }
            // plot all x, y according to selection vector
            else {
                for (size_t i = 0; i < x.size(); ++i, ++selected_cbeg) {
                    if(*selected_cbeg) y[i] = this->d_slope * x[i] + this->d_abscissa + extra_offsety;
                }
            }
        }

        // plot with steps resolution
        else {
            for (size_t i = 0; i < x.size(); ++i) {
                x[i] =  xmin + (double(i) * step);
                y[i] = this->d_slope * x[i] + this->d_abscissa + extra_offsety;

            }
        }
    }

    // plot with upper // lower confidence
    if ((slope_type_0s_1u_2l == 1) || (slope_type_0s_1u_2l == 2)) {

        double myconfidence = this->d_lower_confidence;
        if (slope_type_0s_1u_2l == 2) myconfidence = this->d_upper_confidence;
        if (step == 0.0) {
            // no selection == all plotted
            if (selected_cbeg == selected_cend) {
                for (size_t i = 0; i < x.size(); ++i) {
                    y[i] = myconfidence* (x[i] - center_x)  + center_y + extra_offsety;
                }
            }
            // plot according to selection vector
            else {
                for (size_t i = 0; i < x.size(); ++i, ++selected_cbeg) {
                    if(*selected_cbeg) y[i] = myconfidence * (x[i] - center_x)  + center_y + extra_offsety;
                }
            }
        }
        // plot by resolution given
        else {
            for (size_t i = 0; i < x.size(); ++i) {
                x[i] =  xmin + (double(i) * step);
                y[i] = myconfidence * (x[i] - center_x)  + center_y + extra_offsety;
            }
        }
    }


    if ((slope_type_0s_1u_2l == 3) || (slope_type_0s_1u_2l == 4)  ) {

        double dist_from_slope;
        double denom = (this->d_n - 1.0) * this->d_varx;
        double factor = this->d_student_t_inv * sqrt(this->d_abscissa) / sqrt((this->d_n - 2.0));
        if (step == 0.0) {

            if(selected_cbeg == selected_cend) {
                for (size_t i = 0; i < x.size(); ++i) {
                    dist_from_slope = factor * sqrt( ( (pow((x[i] - this->d_meanx), 2.0)/denom ) + (1.0/this->d_n) ))
                            + extra_offsety;
                    if (slope_type_0s_1u_2l == 3) dist_from_slope *= -1.0;
                    //std::cout << x[i] << " " << dist_from_slope << std::endl;
                    y[i] = this->d_slope * x[i] + this->d_abscissa + dist_from_slope;
                }
            }
            else {
                for (size_t i = 0; i < x.size(); ++i, ++selected_cbeg) {
                    if (*selected_cbeg) {
                        dist_from_slope = factor * sqrt( ( (pow((x[i] - this->d_meanx), 2.0)/denom ) + (1.0/this->d_n) ))
                                + extra_offsety;
                        if (slope_type_0s_1u_2l == 3) dist_from_slope *= -1.0;

                        //std::cout << x[i] << " " << dist_from_slope << std::endl;
                        y[i] = this->d_slope * x[i] + this->d_abscissa + dist_from_slope;
                    }
                }
            }

        }
        else {
            for (size_t i = 0; i < x.size(); ++i) {
                x[i] =  xmin + (double(i) * step);
                dist_from_slope = factor * sqrt( ( (pow((x[i] - this->d_meanx), 2.0)/denom ) + (1.0/this->d_n) ))
                        + extra_offsety;
                if (slope_type_0s_1u_2l == 4) dist_from_slope *= -1.0;
                //std::cout << x[i] << " " << dist_from_slope << std::endl;
                y[i] = this->d_slope * x[i] + this->d_abscissa + dist_from_slope;

            }
        }
    }



    return true;
}


//////////////////////////////////////////////// two_pass_variance  //////////////////////////////////////////////////////////////////////////

two_pass_variance::two_pass_variance()
{

}

two_pass_variance::~two_pass_variance()
{
    // qDebug() << "two_pass_variance destroyed";
}

statmap two_pass_variance::variance(std::vector<double>::const_iterator xcbeg, std::vector<double>::const_iterator xcend,
                                    const double median_or_mean_initvalue) {

    if (std::distance(xcbeg, xcend) < 4 ) {
        std::cerr << "x is smaller than: " << 4 << std::endl;
        return statmap();
    }

    std::vector<double>::const_iterator xcbeg_save = xcbeg;
    this->d_n = (double)std::distance(xcbeg, xcend);
    this->n = (size_t)std::distance(xcbeg, xcend);



    if (median_or_mean_initvalue != 0.0) {
        for (; xcbeg != xcend; ++xcbeg) {
            this->d_sum += *xcbeg - median_or_mean_initvalue;
        }
        this->d_sum += this->d_n * median_or_mean_initvalue;
        this->d_mean = median_or_mean_initvalue;
    }

    else {
        for (; xcbeg != xcend; ++xcbeg) {
            this->d_sum += *xcbeg;
        }
        this->d_mean = this->d_sum / this->d_n;

    }

    double sdiffs = 0.0;
    double moment = 0.0;

    xcbeg = xcbeg_save;
    this->d_min = *xcbeg;
    this->d_max = *xcbeg;
    for (; xcbeg != xcend; ++xcbeg) {
        if (*xcbeg < this->d_min) this->d_min = *xcbeg;
        if (*xcbeg > this->d_max) this->d_max = *xcbeg;
        sdiffs = *xcbeg - this->d_mean;
        this->d_dmean += fabs(sdiffs);
        this->ep += sdiffs;
        this->d_variance += (moment = sdiffs * sdiffs); // m2
        this->d_skewness += (moment *= sdiffs);         // m3
        this->d_kurtosis += (moment *= sdiffs);         // m4
    }


    this->calc_all();

    return this->variance_data;


}

statmap two_pass_variance::variance_if(std::vector<double>::const_iterator xcbeg, std::vector<double>::const_iterator xcend,
                                       std::vector<bool>::const_iterator selected_cbeg, std::vector<bool>::const_iterator selected_cend,
                                       const double median_or_mean_initvalue) {

    math_vector mathv;

    if (!mathv.vector_size_if_min(xcbeg, xcend, selected_cbeg, selected_cend, 4)) {

        return statmap();
    }

    std::vector<double>::const_iterator xcbeg_save = xcbeg;
    std::vector<bool>::const_iterator   selected_cbeg_save = selected_cbeg;

    if (median_or_mean_initvalue != 0.0) {
        for (; xcbeg != xcend; ++xcbeg, ++selected_cbeg) {
            if(*selected_cbeg) {
                this->d_sum += *xcbeg - median_or_mean_initvalue;
                this->n++;
            }
        }
        this->d_n = (double)this->n;
        this->d_sum += this->d_n * median_or_mean_initvalue;
        this->d_mean = median_or_mean_initvalue;
    }

    else {
        for (; xcbeg != xcend; ++xcbeg, ++selected_cbeg) {
            if(*selected_cbeg) {

                this->d_sum += *xcbeg;
                this->n++;
            }

        }
        this->d_n = (double)this->n;

        this->d_mean = this->d_sum / this->d_n;

    }

    double sdiffs = 0.0;

    double moment = 0.0;
    xcbeg = xcbeg_save;
    this->d_min = *xcbeg;
    this->d_max = *xcbeg;
    selected_cbeg = selected_cbeg_save;
    for (; xcbeg != xcend; ++xcbeg, ++selected_cbeg) {
        if(*selected_cbeg) {
            if (*xcbeg < this->d_min) this->d_min = *xcbeg;
            if (*xcbeg > this->d_max) this->d_max = *xcbeg;
            sdiffs = *xcbeg - this->d_mean;
            this->d_dmean += fabs(sdiffs);
            this->ep += sdiffs;
            this->d_variance += (moment = sdiffs * sdiffs); // m2
            this->d_skewness += (moment *= sdiffs);         // m3
            this->d_kurtosis += (moment *= sdiffs);         // m4
        }
    }


    this->calc_all();

    return this->variance_data;

}

void two_pass_variance::clear()
{
    this->n = 0;
    this->d_n = 0.0;
    this->d_sum = 0.0;
    this->d_mean = 0.0;
    this->d_variance_population = 0.0;
    this->d_variance = 0.0;
    this->d_stddev_population = 0.0;
    this->d_stddev = 0.0;
    this->d_skewness = 0.0;
    this->d_kurtosis = 0.0;
    this->d_dmean = 0.0;
    this->ep = 0.0;
    this->d_min = 0.0;
    this->d_max = 0.0;
    this->d_range = 0.0;
}

void two_pass_variance::calc_all() {


    this->d_dmean /= this->d_n;
    this->d_range = this->d_max - this->d_min;

    this->d_variance_population = (this->d_variance - (ep * ep / this->d_n)) / (this->d_n);
    this->d_variance = (this->d_variance - (ep * ep / this->d_n)) / (this->d_n - 1.0);
    this->d_stddev_population = sqrt(this->d_variance_population);
    this->d_stddev = sqrt(this->d_variance);
    if (this->d_variance != 0.0) {
        this->d_skewness /= this->d_n * this->d_variance * this->d_stddev;
        this->d_kurtosis = (this->d_kurtosis /((this->d_n - 1.0)* this->d_variance * this->d_variance)) - 3.0;
    }
    else {
        this->d_skewness = 0;
        this->d_kurtosis = 0;
    }

    this->variance_data[g_stat::d_n_x] = this->d_n ;
    this->variance_data[g_stat::sum_x] = this->d_sum;
    this->variance_data[g_stat::min_x] = this->d_min;
    this->variance_data[g_stat::max_x] = this->d_max;
    this->variance_data[g_stat::mean_x] = this->d_mean;
    this->variance_data[g_stat::range_x] = this->d_range;
    this->variance_data[g_stat::variance_population_x] = this->d_variance_population;
    this->variance_data[g_stat::variance_x] = this->d_variance;
    this->variance_data[g_stat::stddev_population_x] = this->d_stddev_population;
    this->variance_data[g_stat::stddev_x] = this->d_stddev;
    this->variance_data[g_stat::skewness_x] = this->d_skewness;
    this->variance_data[g_stat::kurtosis_x] = this->d_kurtosis;
}

std::ostream& operator<<(std::ostream& out,
                         const two_pass_variance& a) {
    if (4 > a.d_n) {
        out << "two_pass_variance NOT VALID, size must be > 3" << std::endl;
        return out;

    }

    for ( auto it = a.variance_data.cbegin(); it != a.variance_data.cend(); ++it ) {
        if (it->second > 0) {
            out  << std::setw(22) << std::left <<  g_stat::gauss_names.at(it->first).toStdString() << "    " << it->second << std::endl;
        }
        else {
            out  << std::setw(22) << std::left <<  g_stat::gauss_names.at(it->first).toStdString() << "   " << it->second << std::endl;

        }
    }

    return out;
}


select_in_out_reg::select_in_out_reg()
{

}

select_in_out_reg::~select_in_out_reg()
{
    // qDebug() << "select_in_out_reg destroyed";
}

bool select_in_out_reg::mkselect_in_out_reg(std::vector<double>::const_iterator xcbeg, std::vector<double>::const_iterator xcend,
                                            std::vector<double>::const_iterator ycbeg, std::vector<double>::const_iterator ycend,
                                            std::vector<bool>::iterator selected_cbeg, std::vector<bool>::iterator selected_cend,
                                            const statmap &regression_data,
                                            const double center_x, const double center_y,
                                            const int slope_type_0s_1u_2l, double extra_offsety) {


    math_vector mathv;
    if (!mathv.vector_sizes_if_min(xcbeg, xcend, ycbeg, ycend, selected_cbeg, selected_cend, 4)) {

        return false;
    }

    this->d_n = regression_data.at(g_stat::d_n_x);
    this->d_slope = regression_data.at(g_stat::slope_xy);
    this->d_abscissa = regression_data.at(g_stat::abscissa_xy);
    this->d_lower_confidence = regression_data.at(g_stat::student_t_lower_confidence_xy);
    this->d_upper_confidence = regression_data.at(g_stat::student_t_upper_confidence_xy);
    this->d_varx = regression_data.at(g_stat::variance_x);
    this->d_student_t_inv = regression_data.at(g_stat::student_t_inv);
    this->d_meanx = regression_data.at(g_stat::mean_x);


    if ( (slope_type_0s_1u_2l == 1) || (slope_type_0s_1u_2l == 2) ) {
        for (; xcbeg != xcend; ++xcbeg, ++ycbeg, ++selected_cbeg) {
            double ymax = this->d_upper_confidence * (*xcbeg - center_x)  + center_y + extra_offsety;
            double ymin = this->d_lower_confidence * (*xcbeg - center_x)  + center_y + extra_offsety;
            std::cout << ymin << "  " << *ycbeg << "  " << ymax <<std::endl;
            if ((ymin < *ycbeg) && (*ycbeg < ymax) ) *selected_cbeg = true;
            else *selected_cbeg = false;
        }
        return true;
    }


    else if ( (slope_type_0s_1u_2l == 3) || (slope_type_0s_1u_2l == 4) ) {

        double dist_from_slope;
        double denom = (this->d_n - 1.0) * this->d_varx;
        double factor = this->d_student_t_inv * sqrt(this->d_abscissa) / sqrt((this->d_n - 2.0));

        for (; xcbeg != xcend; ++xcbeg, ++ycbeg, ++selected_cbeg) {
            dist_from_slope = factor * sqrt( ( (pow((*xcbeg - this->d_meanx), 2.0)/denom ) + (1.0/this->d_n) ))
                    + extra_offsety;
            double ymax = this->d_slope * *xcbeg + this->d_abscissa + dist_from_slope;
            double ymin = this->d_slope * *xcbeg + this->d_abscissa - dist_from_slope;
            std::cout << *xcbeg << "  " << ymin << "  " << *ycbeg << "  " << ymax <<std::endl;
            if ((ymin < *ycbeg) && (*ycbeg < ymax) ) *selected_cbeg = true;
            else *selected_cbeg = false;

            std::cout << *xcbeg << " " << dist_from_slope << std::endl;


        }
        return true;
    }



    return false;
}   // select_in_out_reg

two_pass_linreg::two_pass_linreg()
{
    this->S_xy = 0.0;
    this->n = 0;
    this->d_n = 0.0;
    this->d_covariance = 0.0;
    this->d_correlation = 0.0;
    this->d_slope = 0.0;
    this->d_abscissa = 0.0;
    this->d_sum_alldist_from_slope = 0.0;
    this->has_data = false;
    this->d_test_against_other_slope = DBL_MAX;
}

two_pass_linreg::~two_pass_linreg()
{
    //qDebug() << "two_pass_linreg destroyed";
}

statmap two_pass_linreg::linreg(std::vector<double>::const_iterator xcbeg, std::vector<double>::const_iterator xcend,
                                std::vector<double>::const_iterator ycbeg, std::vector<double>::const_iterator ycend,
                                const double median_or_mean_subtract_y) {



    size_t xdist = (size_t)std::distance(xcbeg, xcend);
    if (xdist < 4 ) {
        std::cerr << "x is smaller than: " << 4 << std::endl;
        return statmap();
    }
    size_t ydist = (size_t)std::distance(ycbeg, ycend);


    if (xdist != ydist) {
        std::cerr << "x and y not of same size: " << xdist << " " << ydist << std::endl;
        return statmap();
    }
    this->d_n = (double)std::distance(xcbeg, xcend);

    std::vector<double>::const_iterator xcbeg_save = xcbeg;
    std::vector<double>::const_iterator ycbeg_save = ycbeg;


    x.variance(xcbeg, xcend);
    y.variance(ycbeg, ycend, median_or_mean_subtract_y);


    xcbeg = xcbeg_save;
    ycbeg = ycbeg_save;
    for (; xcbeg != xcend; ++xcbeg, ++ycbeg) {

        this->S_xy += *xcbeg * *ycbeg;
    }


    if (!this->calc_all() ) {
        return statmap();
    }

    return this->regression_data;
}

statmap two_pass_linreg::linreg_if(std::vector<double>::const_iterator xcbeg, std::vector<double>::const_iterator xcend,
                                   std::vector<double>::const_iterator ycbeg, std::vector<double>::const_iterator ycend,
                                   std::vector<bool>::const_iterator selected_cbeg, std::vector<bool>::const_iterator selected_cend,
                                   const double median_or_mean_subtract_y) {

    math_vector mathv;
    if (!mathv.vector_sizes_if_min(xcbeg, xcend, ycbeg, ycend, selected_cbeg, selected_cend, 4)) {

        return statmap();
    }

    std::vector<double>::const_iterator xcbeg_save = xcbeg;
    std::vector<double>::const_iterator ycbeg_save = ycbeg;
    std::vector<bool>::const_iterator   selected_cbeg_save = selected_cbeg;


    x.variance_if(xcbeg, xcend, selected_cbeg, selected_cend);
    selected_cbeg = selected_cbeg_save;
    y.variance_if(ycbeg, ycend, selected_cbeg, selected_cend, median_or_mean_subtract_y);

    xcbeg = xcbeg_save;
    ycbeg = ycbeg_save;
    selected_cbeg = selected_cbeg_save;

    for (; xcbeg != xcend; ++xcbeg, ++ycbeg, ++selected_cbeg) {

        if (*selected_cbeg) {
            this->S_xy += *xcbeg * *ycbeg;
            this->n++;
        }
    }

    this->d_n = (double) this->n;

    if (!this->calc_all() ) {
        return statmap();
    }

    return this->regression_data;
}

bool two_pass_linreg::calc_all()
{
    math_vector mathv;
    this->regression_data = mathv.mk_regression_data(this->x.variance_data, this->y.variance_data, this->is_ok);
    if (!this->is_ok) {
        std::cerr << "two_pass_linreg::linerar regression NOT VALID, can not join x & y" << std::endl;
        return false;
    }


    this->S_xy = (this->S_xy - this->x.d_sum * this->y.d_sum /this->d_n)  / (this->d_n - 1.0);

    this->d_slope = this->S_xy / this->x.d_variance;
    this->d_abscissa = this->y.d_mean - this->d_slope * this->x.d_mean;
    this->d_covariance = this->S_xy;
    this->d_correlation = this->S_xy / ( (this->x.d_stddev * this->y.d_stddev) );
    this->d_sum_alldist_from_slope = (this->d_n - 1.0) * (this->y.d_variance - pow(this->d_slope, 2.0) * this->x.d_variance);


    this->regression_data[g_stat::covariance_xy] =  this->d_covariance;
    this->regression_data[g_stat::correlation_xy] =  this->d_correlation;
    this->regression_data[g_stat::slope_xy] =  this->d_slope;
    this->regression_data[g_stat::abscissa_xy] =  this->d_abscissa;
    this->regression_data[g_stat::sum_alldist_from_slope_xy] =  this->d_sum_alldist_from_slope;

    this->has_data = true;

    return true;

}

double two_pass_linreg::test_against_other_slope(const double& other_slope, statmap &result) {
    if (this->is_ok && this->has_data) {

        this->d_test_against_other_slope =
                sqrt(this->x.d_variance * (this->d_n - 1.0) * (this->d_n - 2.0) ) *
                (this->d_slope - other_slope) / sqrt(this->d_sum_alldist_from_slope);


        this->regression_data[g_stat::student_t_test_against_other_slope] = this->d_test_against_other_slope;
        result[g_stat::student_t_test_against_other_slope] = this->d_test_against_other_slope;
        return this->d_test_against_other_slope;

    }

    else return DBL_MAX;
}


two_pass_variance_cplx::two_pass_variance_cplx()
{

}

two_pass_variance_cplx::~two_pass_variance_cplx()
{
    // qDebug() << "two_pass_variance_cplx destroyed";
}

statmap two_pass_variance_cplx::variance(std::vector<std::complex<double>>::const_iterator xcbeg, std::vector<std::complex<double>>::const_iterator xcend,
                                         const std::complex<double> median_or_mean_initvalue) {

    if (std::distance(xcbeg, xcend) < 4 ) {
        std::cerr << "x is smaller than: " << 4 << std::endl;
        return statmap();
    }

    std::vector<std::complex<double>>::const_iterator xcbeg_save = xcbeg;
    this->d_n = (double)std::distance(xcbeg, xcend);
    this->n = (size_t)std::distance(xcbeg, xcend);



    if (median_or_mean_initvalue != 0.0) {
        for (; xcbeg != xcend; ++xcbeg) {
            this->d_sum += *xcbeg - median_or_mean_initvalue;
        }
        this->d_sum += this->d_n * median_or_mean_initvalue;
        this->d_mean = median_or_mean_initvalue;
    }

    else {
        for (; xcbeg != xcend; ++xcbeg) {
            this->d_sum += *xcbeg;
        }
        this->d_mean = this->d_sum / this->d_n;

    }

    std::complex<double> sdiffs = std::complex<double>(0.0, 0.0);
    std::complex<double> moment = std::complex<double>(0.0, 0.0);

    xcbeg = xcbeg_save;
    this->d_min = *xcbeg;
    this->d_max = *xcbeg;
    for (; xcbeg != xcend; ++xcbeg) {
        if (std::abs(*xcbeg) < std::abs(this->d_min)) this->d_min = std::abs(*xcbeg);
        if (std::abs(*xcbeg) > std::abs(this->d_max)) this->d_max = std::abs(*xcbeg);
        sdiffs = *xcbeg - this->d_mean;
        this->d_dmean += std::abs(sdiffs);
        this->ep += sdiffs;
        this->d_variance += (moment = sdiffs * sdiffs); // m2
        this->d_skewness += (moment *= sdiffs);         // m3
        this->d_kurtosis += (moment *= sdiffs);         // m4
    }


    this->calc_all();

    return this->variance_data;


}

statmap two_pass_variance_cplx::variance_if(std::vector<std::complex<double>>::const_iterator xcbeg, std::vector<std::complex<double>>::const_iterator xcend,
                                            std::vector<bool>::const_iterator selected_cbeg, std::vector<bool>::const_iterator selected_cend,
                                            const std::complex<double> median_or_mean_initvalue) {

    math_vector mathv;

    if (!mathv.vector_size_if_min_cplx(xcbeg, xcend, selected_cbeg, selected_cend, 4)) {

        return statmap();
    }

    std::vector<std::complex<double>>::const_iterator xcbeg_save = xcbeg;
    std::vector<bool>::const_iterator   selected_cbeg_save = selected_cbeg;

    if (median_or_mean_initvalue != 0.0) {
        for (; xcbeg != xcend; ++xcbeg, ++selected_cbeg) {
            if(*selected_cbeg) {
                this->d_sum += *xcbeg - median_or_mean_initvalue;
                this->n++;
            }
        }
        this->d_n = (double)this->n;
        this->d_sum += this->d_n * median_or_mean_initvalue;
        this->d_mean = median_or_mean_initvalue;
    }

    else {
        for (; xcbeg != xcend; ++xcbeg, ++selected_cbeg) {
            if(*selected_cbeg) {

                this->d_sum += *xcbeg;
                this->n++;
            }

        }
        this->d_n = (double)this->n;

        this->d_mean = this->d_sum / this->d_n;

    }

    std::complex<double> sdiffs = std::complex<double>(0.0, 0.0);

    std::complex<double> moment = std::complex<double>(0.0, 0.0);
    xcbeg = xcbeg_save;
    this->d_min = *xcbeg;
    this->d_max = *xcbeg;
    selected_cbeg = selected_cbeg_save;
    for (; xcbeg != xcend; ++xcbeg, ++selected_cbeg) {
        if(*selected_cbeg) {
            if (std::abs(*xcbeg) < std::abs(this->d_min)) this->d_min = std::abs(*xcbeg);
            if (std::abs(*xcbeg) > std::abs(this->d_max)) this->d_max = std::abs(*xcbeg);
            sdiffs = *xcbeg - this->d_mean;
            this->d_dmean += std::abs(sdiffs);
            this->ep += sdiffs;
            this->d_variance += (moment = sdiffs * sdiffs); // m2
            this->d_skewness += (moment *= sdiffs);         // m3
            this->d_kurtosis += (moment *= sdiffs);         // m4
        }
    }


    this->calc_all();

    return this->variance_data;

}

void two_pass_variance_cplx::clear()
{
    this->n = 0;
    this->d_n = 0.0;
    this->d_sum = std::complex<double>(0.0, 0.0);
    this->d_mean = std::complex<double>(0.0, 0.0);
    this->d_variance_population = std::complex<double>(0.0, 0.0);
    this->d_variance = std::complex<double>(0.0, 0.0);
    this->d_stddev_population = std::complex<double>(0.0, 0.0);
    this->d_stddev = std::complex<double>(0.0, 0.0);
    this->d_skewness = std::complex<double>(0.0, 0.0);
    this->d_kurtosis = std::complex<double>(0.0, 0.0);
    this->d_dmean = std::complex<double>(0.0, 0.0);
    this->ep = std::complex<double>(0.0, 0.0);
    this->d_min = std::complex<double>(0.0, 0.0);
    this->d_max = std::complex<double>(0.0, 0.0);
    this->d_range = std::complex<double>(0.0, 0.0);
}

void two_pass_variance_cplx::calc_all() {


    this->d_dmean /= this->d_n;
    this->d_range = this->d_max - this->d_min;

    this->d_variance_population = (this->d_variance - (ep * ep / this->d_n)) / (this->d_n);
    this->d_variance = (this->d_variance - (ep * ep / this->d_n)) / (this->d_n - 1.0);
    this->d_stddev_population = sqrt(this->d_variance_population);
    this->d_stddev = sqrt(this->d_variance);
    if (this->d_variance != 0.0) {
        this->d_skewness /= this->d_n * this->d_variance * this->d_stddev;
        this->d_kurtosis = (this->d_kurtosis /((this->d_n - 1.0)* this->d_variance * this->d_variance)) - 3.0;
    }
    else {
        this->d_skewness = 0;
        this->d_kurtosis = 0;
    }

    this->variance_data[g_stat::d_n_x] = this->d_n ;
    this->variance_data[g_stat::sum_x] = std::abs(this->d_sum);
    this->variance_data[g_stat::min_x] = std::abs(this->d_min);
    this->variance_data[g_stat::max_x] = std::abs(this->d_max);
    this->variance_data[g_stat::mean_x] = std::abs(this->d_mean);
    this->variance_data[g_stat::range_x] = std::abs(this->d_range);
    this->variance_data[g_stat::variance_population_x] = std::abs(this->d_variance_population);
    this->variance_data[g_stat::variance_x] = std::abs(this->d_variance);
    this->variance_data[g_stat::stddev_population_x] = std::abs(this->d_stddev_population);
    this->variance_data[g_stat::stddev_x] = std::abs(this->d_stddev);
    this->variance_data[g_stat::skewness_x] = std::abs(this->d_skewness);
    this->variance_data[g_stat::kurtosis_x] = std::abs(this->d_kurtosis);
}

std::ostream& operator<<(std::ostream& out,
                         const two_pass_variance_cplx& a) {
    if (4 > a.d_n) {
        out << "two_pass_variance NOT VALID, size must be > 3" << std::endl;
        return out;

    }

    for ( auto it = a.variance_data.cbegin(); it != a.variance_data.cend(); ++it ) {
        if (it->second > 0) {
            out  << std::setw(22) << std::left <<  g_stat::gauss_names.at(it->first).toStdString() << "    " << it->second << std::endl;
        }
        else {
            out  << std::setw(22) << std::left <<  g_stat::gauss_names.at(it->first).toStdString() << "   " << it->second << std::endl;

        }
    }

    return out;
}



