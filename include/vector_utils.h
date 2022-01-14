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

#ifndef VECTOR_UTILS_H
#define VECTOR_UTILS_H


#include <cstdlib>
#include <cmath>
#include <vector>
#include <complex>
#include <iostream>
#include <algorithm>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QTextStream>
#include <utility>
#include <QRegularExpression>
#include "iterator_templates.h"

/*! \brief sort_by_first_mod_second_only sorts by f BUT leaves f untouched - needed for multiple iterations!
 *
 */
template <typename T, typename S> bool sort_by_first_mod_second_only(const std::vector<T> &f, std::vector<S> &a) {

    if (f.size() != a.size()) return false;

    std::vector<std::pair<double, double>> v(f.size());
    size_t i;

    for (i = 0; i < f.size(); ++i) {
        v[i] = std::make_pair(f[i], a[i]);
    }

    std::sort(v.begin(), v.end(), iter::cmp_pair_first_lt<std::pair<double, double>>());

    for (i = 0; i < f.size(); ++i) {
        a[i] = v[i].second;
    }

    return true;
}

/*!
 * \brief vector_f_stepper_Hz
 * \param f creates a frequency vector
 * \param step_size 10 steps, 20 steps 40 steps per decade; ATTENTION 40 from 1 ... 10 means 0.25 Hz steps; FFT must be long enough
 * \return
 */
template <typename T>
size_t vector_f_stepper_Hz(std::vector<T> &f, const int &step_size) {

    if (!step_size) return 0;
    double s = double(step_size);

    // 6 decades
    for (int i = 0; i < 6; ++i ) {
        for (int j = 1; j < step_size; ++j) {
            f.push_back(10.0/s * (double(j)) * pow(10.0, double(i)));
        }
    }

    return f.size();

}


/*!
 * \brief create_nharmonics
 * \param base_frequency
 * \param n_harmonics if 0 the base frequency is returned, if three (for example) then base, 1st,2nd, 3rd - so four values
 * \param odd_only
 */
template <typename T>
std::vector<T> create_nharmonics(const double &base_frequency, const size_t &n_harmonics, const bool odd_only) {
    if (base_frequency < 0.0001) {
        std::vector<double> selected_freqs;
        return selected_freqs;
    }
    std::vector<double> selected_freqs(n_harmonics+1);
    if (n_harmonics == 0) {
        selected_freqs[0] = base_frequency;
    }
    else if (odd_only && n_harmonics > 0) {
        size_t k = 1;
        for (size_t i = 0; i < n_harmonics + 1; ++i) {
            selected_freqs[i] = double(k) * base_frequency;
            k += 2;
        }
    }
    else if (n_harmonics > 0) {
        for (size_t i = 0; i < n_harmonics + 1; ++i) {
            selected_freqs[i] = double(i+1) * base_frequency;
        }
    }
    else {
        selected_freqs.resize(0);
    }
    return selected_freqs;

}


/*!
 * \brief vector_f_select_n select n elements form a vector, if in.size() is 512 you take 40 equal spaced from it
 * \param in
 * \param f
 * \param n
 * \return
 */
template <typename T>
size_t vector_f_select_n(const std::vector<double> in, std::vector<T> &f, const size_t &n) {


    if (!n) return 0;
    if (n >= in.size()) {
        f = in;
        return  f.size();
    }

    double ds = double(in.size()/ double(n));
    size_t idx = 0;
    f.push_back(in[0]);
    for (size_t i = 1; i < in.size()-1; ++i) {
        idx = size_t(ds * double(i));
        if (idx < in.size()-1) f.push_back(in[idx]);
    }
    f.push_back(in[in.size()-1]);


    return f.size();

}



/*!
 * \brief vector_2_indices compares two vectors
 * \param in master (like fft freqs)
 * \param comp  compare (like target freqs)
 * \param result    selcted in (where in == comp)
 * \param indices   indices of in (where in == comp)
 */
template <typename T>
size_t vector_2_indices(const std::vector<T> &in, const std::vector<T> &comp, std::vector<T> &result, std::vector<size_t> &indices) {

    // no data
    if (!in.size() || !comp.size()) return 0;
    result.reserve(comp.size());
    indices.reserve(comp.size());
    size_t t = 0;
    for (size_t i = 0; i < in.size(); ++i) {
        for (size_t j = t; j < comp.size(); ++j) {
            if (in[i] == comp[j]) {
                indices.push_back(i);
                result.push_back(in[i]);
                t = j;
            }
        }
    }

    return indices.size();
}



/*!
 * \brief write_vector writes a vector to a filename or console
 * \param filename if == console, write to std::cout
 * \param v
 * \param skip_first
 * \param skip_last
 * \param append if 1 opens a file and appends
 * \return
 */
template <typename T>
size_t write_vector(const QString filename, const std::vector<T> &v,
                    const size_t skip_first = 0, const size_t skip_last = 0, const int append = 0) {

    if (!v.size()) return 0;
    typename std::vector<T>::const_iterator b = v.cbegin();
    typename std::vector<T>::const_iterator e = v.cend();

    if ((skip_first + skip_last) == v.size()) return 0;
    b += skip_first;
    e -= skip_last;

    size_t amount = (e - b);

    if (filename == "console") {

        while ( b != e ) {
            std::cout << *b << Qt::endl;
            ++b;
        }
    }

    else {
        bool opensuccess;

        QFile data(filename);
        if (append) opensuccess = data.open(QFile::WriteOnly | QFile::Append | QFile::Text);
        else opensuccess = data.open(QFile::WriteOnly | QFile::Truncate | QFile::Text);

        if (!opensuccess) return 0;

        if (opensuccess) {
            QTextStream out(&data);
            out.setRealNumberNotation(QTextStream::ScientificNotation);

            while (b != e) {
                out << *b  << Qt::endl;
                ++b;
            }

            data.close();
        }
    }
    return amount;
}


template <typename T>
size_t write_vector_qstring(const QString filename, const std::vector<QString> str, const std::vector<T> &v) {

    if (!v.size()) return 0;
    if (!str.size()) return 0;
    if (v.size() != str.size()) return 0;
    bool opensuccess;

    QFile data(filename);
    opensuccess = data.open(QFile::WriteOnly | QFile::Truncate | QFile::Text);

    if (!opensuccess) return 0;

    if (opensuccess) {
        QTextStream out(&data);
        out.setRealNumberNotation(QTextStream::ScientificNotation);

        for ( size_t i = 0; i < v.size(); ++i) {
            out << str[i] << " " << v[i] << Qt::endl;
        }


        data.close();
    }

    return v.size();
}


template <typename T, typename S> size_t write_vector(const QString filename, const std::vector<T> &v,  const std::vector<S> &w,
                                                      const size_t skip_first = 0, const size_t skip_last = 0, const int append = 0) {

    // vectors must have different sizes but not zero
    if ( v.size() != w.size() ) return 0;
    if ( (!v.size()) ||( !w.size())) return 0;

    typename std::vector<T>::const_iterator b = v.cbegin();
    typename std::vector<T>::const_iterator e = v.cend();

    typename std::vector<S>::const_iterator bs = w.cbegin();

    bool opensuccess;

    if ((skip_first + skip_last) == v.size()) return 0;
    b += skip_first;
    e -= skip_last;
    bs += skip_first;
    size_t amount = (e - b);


    if (filename == "console") {
        while ( b != e ) {
            std::cout << *b << "   " << *bs << Qt::endl;
            ++b; ++bs;
        }
    }

    else {

        QFile data(filename);
        if (append) opensuccess = data.open(QFile::WriteOnly | QFile::Append | QFile::Text);
        else opensuccess = data.open(QFile::WriteOnly | QFile::Truncate | QFile::Text);

        if (!opensuccess) return 0;

        if (opensuccess) {
            QTextStream out(&data);
            out.setRealNumberNotation(QTextStream::ScientificNotation);

            while (b != e) {
                out << *b << "   " << *bs << Qt::endl;
                ++b; ++bs;
            }

            data.close();
        }
    }

    return amount;
}


template <typename T> size_t write_vector(const QString filename, const std::vector<T> &v,  const std::vector<T> &w, const std::vector<T> &err,
                                          const size_t skip_first = 0, const size_t skip_last = 0, const int append = 0) {

    // vectors must have different sizes but not zero
    if ( v.size() != w.size() ) return 0;
    if ( v.size() != err.size() ) return 0;

    if ( (!v.size()) ||( !w.size()) || (!err.size())) return 0;


    typename std::vector<T>::const_iterator b = v.cbegin();
    typename std::vector<T>::const_iterator e = v.cend();

    typename std::vector<T>::const_iterator bs = w.cbegin();
    typename std::vector<T>::const_iterator es = err.cbegin();

    bool opensuccess;

    if ((skip_first + skip_last) == v.size()) return 0;
    b += skip_first;
    e -= skip_last;
    bs += skip_first;
    es + skip_first;
    size_t amount = (e - b);


    if (filename == "console") {
        while ( b != e ) {
            std::cout << *b << "   " << *bs << "   " << *es << Qt::endl;
            ++b; ++bs; ++es;
        }
    }

    else {

        QFile data(filename);
        if (append) opensuccess = data.open(QFile::WriteOnly | QFile::Append | QFile::Text);
        else opensuccess = data.open(QFile::WriteOnly | QFile::Truncate | QFile::Text);

        if (!opensuccess) return 0;

        if (opensuccess) {
            QTextStream out(&data);
            out.setRealNumberNotation(QTextStream::ScientificNotation);

            while (b != e) {
                out << *b << "   " << *bs << "   " << *es << Qt::endl;
                ++b; ++bs; ++es;
            }

            data.close();
        }
    }

    return amount;
}







/*!
   \brief create a string from complex number <br>
     real || imag || complex || abs || phase_rad || phase_deg || <br>
       1      2        3         4      5              6 <br>
    \param complex number
    \param which (as above)

*/
template <typename T> QString cplx_to_reals(const T c, const int which) {
    if (which == 1) return  QString::number(real(c), 'E', 8);
    if (which == 2) return  QString::number(imag(c), 'E', 8);
    if (which == 3)  {

        QString real_imag;
        real_imag +=  QString::number(real(c), 'E', 8);
        real_imag += " ";
        real_imag +=  QString::number(imag(c), 'E', 8);
        return real_imag;

    }
    if (which == 4) return  QString::number(abs(c), 'E', 8);
    if (which == 5) return  QString::number(arg(c), 'E', 8);
    if (which == 6) return  QString::number( ((arg(c) * 180.) / M_PI), 'E', 8 );

    return "failure";
}


/*!
  write f --- and:  real || imag || abs || phase_rad || phase_deg || complex
*/
template <typename T, typename S> size_t write_vector_real_cplx(const QString filename, const QString type, const std::vector<T> &v,  const std::vector<S> &w,
                                                                const size_t skip_first = 0, const size_t skip_last = 0, const int append = 0) {

    // vectors must have different sizes but not zero
    if ( v.size() != w.size() ) return 0;
    if ( (!v.size()) ||( !w.size())) return 0;

    typename std::vector<T>::const_iterator b = v.cbegin();
    typename std::vector<T>::const_iterator e = v.cend();

    typename std::vector<S>::const_iterator bs = w.cbegin();
    typename std::vector<S>::const_iterator es = w.cend();

    bool opensuccess;

    if ((skip_first + skip_last) == v.size()) return 0;
    b += skip_first;
    e -= skip_last;
    bs += skip_first;
    es -= skip_last;
    size_t amount = (e - b);

    int which;

    //    if ( typeid(z) != typeid(S) ) {
    //        qWarning() << " write_vector_real_cplx --. please use double and complex<double> ";
    //        return;
    //    }

    if (type == "real") which = 1;
    if (type == "imag") which = 2;
    if (type == "complex") which = 3;
    if (type == "abs") which = 4;
    if (type == "phase_rad") which = 5;
    if (type == "phase_deg") which = 6;

    b += skip_first;
    bs += skip_first;

    e -= skip_last;
    es -= skip_last;

    if ( (e - b) != (es - bs)  ) {
        qWarning() << "write_vector_real_cplx vector size are different!" << (e-b) << (es-bs);
        return 0;
    }

    if (filename == "console") {
        while ( b != e ) {
            //--qDebug() << QString::number(*b, 'E', 8) << "   " << cplx_to_reals<S>(*bs, which);
            std::cout << *b << "  " << cplx_to_reals<S>(*bs, which).toStdString() << Qt::endl;
            ++b; ++bs;
        }
    }

    else {

        QFile data(filename);

        if (append) opensuccess = data.open(QFile::WriteOnly | QFile::Append | QFile::Text);
        else opensuccess = data.open(QFile::WriteOnly | QFile::Truncate | QFile::Text);


        if (opensuccess) {
            QTextStream out(&data);
            out.setRealNumberNotation(QTextStream::ScientificNotation);


            while (b != e) {
                out << *b << "   " << cplx_to_reals<S>(*bs, which) << Qt::endl;
                ++b; ++bs;
            }

            data.close();
        }

    }

    return amount;
}

// complex vector to real and imag
template <typename T>
size_t cplx2ri(const std::vector<std::complex<T> >& in,
               std::vector<T>& treal, std::vector<T>& timag) {

    if (in.size() != treal.size())
        treal.resize(in.size());
    if (in.size() != timag.size())
        timag.resize(in.size());

    for (size_t i = 0; i < in.size(); i++) {
        treal[i] = real(in[i]);
        timag[i] = imag(in[i]);
    }

    return treal.size();
}


template <typename T>
std::vector<std::complex<T>> read_xy_columns_as_complex(QFileInfo *qfi) {

    QFile data(qfi->absoluteFilePath());
    std::vector<std::complex<T>> z;
    z.reserve(10000);
    if (!data.open(QFile::ReadOnly | QFile::Text)) return z;
    QTextStream qtx(&data);
    T re, im;
    while(!qtx.atEnd()) {
        qtx >> re;
        qtx >> im;
        z.emplace_back(std::complex<T>(re, im));
    }

    data.close();
    z.shrink_to_fit();
    return z;


}
template <typename T>
size_t index_of(const std::vector<T> &v, const T elem) {

    return find(v.begin(), v.end(), elem) - v.begin();


}

template <typename T>
size_t read_x_column (QFileInfo *qfi, std::vector<T>& x, const size_t colx = 0, const bool check_isnumber = true, const size_t size_hint = 0) {

    QFile data(qfi->absoluteFilePath());
    size_t idx = 0;
    bool opensuccess = data.open(QFile::ReadOnly | QFile::Text);

    if (opensuccess) {
        QTextStream in(&data);
        QString line_in;
        QString line;
        QStringList items;

        if (size_hint) {
            x.reserve(size_hint);
        }

        // check the first line if enough coulmns are available
        bool ok;
        double d;
        while(!in.atEnd()) {
            line_in = in.readLine();
            line = line_in.simplified();
            items = line.split(QRegularExpression("\\s+"));
            //qDebug() << items;
            if (!idx) {
                if ( ((size_t)items.size()-1 < colx) ) return 0;
            }
            d = items.at(colx).toDouble(&ok);
            if(ok) {
                x.push_back(d);
                ++idx;
            }
            else return idx;
        }
        data.close();
    }
    return idx;
}

template <typename T>
size_t read_five_columns (QTextStream *qts, std::vector<T>& x, std::vector<T>& y, std::vector<T>& a, std::vector<T>& b, std::vector<T>& c,
                          std::vector<bool>& bx, std::vector<bool>& by, std::vector<bool>& ba, std::vector<bool>& bb, std::vector<bool>& bc,
                          double &maxx, double &minx, double &maxy, double &miny, double &maxa, double &mina,
                          double &maxb, double &minb, double &maxc, double &minc,
                          const double &gt_not_number, const double &gt_number_replace, const bool check_isnumber = true , const size_t size_hint = 0) {

    if (size_hint) {
        x.reserve(size_hint);
        y.reserve(size_hint);
        a.reserve(size_hint);
        b.reserve(size_hint);
        c.reserve(size_hint);
        bx.reserve(size_hint);
        by.reserve(size_hint);
        ba.reserve(size_hint);
        bb.reserve(size_hint);
        bc.reserve(size_hint);

    }

    QString line_in, line;
    QStringList items;
    size_t idx = 0;

    QString idiot_line;
    QStringList idiot_items;

    bool okx, oky, oka, okb, okc;
    double da, db, dc, dx, dy;

    while(!qts->atEnd()) {
        line_in = qts->readLine();
        line = line_in.simplified();
        items = line.split(QRegularExpression("\\s+"));
        if ((size_t)items.size() < 5) {
            if(!qts->atEnd()) {
                idiot_line =  qts->readLine();
                idiot_line = idiot_line.simplified();
                idiot_items = idiot_line.split(QRegularExpression("\\s+"));
                items.append(idiot_items);

                if ((size_t)items.size() != 5) {
                    qDebug() << "failure - not five channels!";
                    qDebug() << line;
                    return idx;
                }
            }
            else return idx;

        }
        dx = items.at(0).toDouble(&okx);
        dy = items.at(1).toDouble(&oky);
        da = items.at(2).toDouble(&oka);
        db = items.at(3).toDouble(&okb);
        dc = items.at(4).toDouble(&okc);

        if (oka && okb && okc && okx && oky) {

            if ( (da >= gt_not_number) || (db >= gt_not_number) || (dc >= gt_not_number) ||  (dx >= gt_not_number) || (dy >= gt_not_number) ) {
                x.push_back(gt_number_replace);
                y.push_back(gt_number_replace);
                a.push_back(gt_number_replace);
                b.push_back(gt_number_replace);
                c.push_back(gt_number_replace);
                bx.push_back(true);
                by.push_back(true);
                ba.push_back(true);
                bb.push_back(true);
                bc.push_back(true);

            }
            else {
                x.push_back(dx);
                y.push_back(dy);
                a.push_back(da);
                b.push_back(db);
                c.push_back(dc);

                if (!idx) {
                    minx = dx;
                    maxx = dx;
                    miny = dy;
                    maxy = dy;
                    mina = da;
                    maxa = da;
                    minb = db;
                    maxb = db;
                    minc = dc;
                    maxc = dc;
                }
                else {
                    if (dx < minx) minx = dx;
                    if (dy < miny) minx = dy;
                    if (da < mina) minx = da;
                    if (db < minb) minx = db;
                    if (dc < minc) minx = dc;

                    if (dx > maxx) maxx = dx;
                    if (dy > maxy) maxy = dy;
                    if (da > maxa) maxa = da;
                    if (db > maxb) maxb = db;
                    if (dc > maxc) maxc = dc;
                }

                bx.push_back(false);
                by.push_back(false);
                ba.push_back(false);
                bb.push_back(false);
                bc.push_back(false);


            }
            ++idx;
        }
        else {
            qDebug() << "failure" << line;
            return idx;
        }

    }

    qDebug() << x.size();
    return idx;

}

template <typename T>
size_t read_xy_columns (QFileInfo *qfi, std::vector<T>& x, std::vector<T>& y, const size_t colx = 0, const size_t coly = 1, const bool check_isnumber = true , const size_t size_hint = 0) {

    QFile data(qfi->absoluteFilePath());
    size_t idx = 0;
    bool opensuccess = data.open(QFile::ReadOnly | QFile::Text);

    if (opensuccess) {
        QTextStream in(&data);
        QString line_in;
        QString line;
        QStringList items;

        if (size_hint) {
            x.reserve(size_hint);
            y.reserve(size_hint);
        }

        // check the first line if enough coulmns are available

        bool oka, okb;
        double a, b;
        while(!in.atEnd()) {
            line_in = in.readLine();
            line = line_in.simplified();
            items = line.split(QRegularExpression("\\s+"));
            //qDebug() << items;
            if (!idx) {
                if ( ((size_t)items.size()-1 < colx) || ((size_t)items.size()-1 < coly) ) return 0;
            }
            a = items.at(colx).toDouble(&oka);
            b = items.at(coly).toDouble(&okb);
            if (oka && okb) {
                x.push_back(a);
                y.push_back(b);
                ++idx;
            }
            else return idx;
        }
        data.close();
    }
    return idx;
}

/*!
 * \brief read_xy_columns_min_max_f reads x and y from a (at least 3 column) file and checks if f in min_f max_f range
 * \param qfi
 * \param min_f
 * \param max_f
 * \param x
 * \param y
 * \param colx
 * \param coly
 * \param colf
 * \param check_isnumber
 * \param size_hint
 * \return
 */
template <typename T>
size_t read_xy_columns_min_max_f (QFileInfo *qfi, const double min_f, const double max_f, std::vector<T>& x, std::vector<T>& y,
                                  const size_t colx, const size_t coly , const size_t colf, const bool check_isnumber = true,
                                  const size_t size_hint = 0) {

    QFile data(qfi->fileName());
    size_t idx = 0;

    bool opensuccess = data.open(QFile::ReadOnly | QFile::Text);

    if (opensuccess) {
        QTextStream in(&data);
        QString line_in;
        QString line;
        QStringList items;

        if (size_hint) {
            x.reserve(size_hint);
            y.reserve(size_hint);
        }

        bool oka, okb, okf;
        double a, b, f;
        // check the first line if enough coulmns are available
        while(!in.atEnd()) {
            line_in = in.readLine();
            line = line_in.simplified();
            items = line.split(QRegularExpression("\\s+"));
            //qDebug() << items;

            if (!idx) {
                if ( ((size_t)items.size()-1 < colx) || ((size_t)items.size()-1 < coly) || ((size_t)items.size()-1 < colf) ) return 0;
            }

            a = items.at(colx).toDouble(&oka);
            b = items.at(coly).toDouble(&okb);
            f = items.at(colf).toDouble(&okf);
            if (oka && okb && okf) {
                if ( ( f >= min_f) && (f <= max_f) ) {
                    x.push_back(a);
                    y.push_back(b);
                    ++idx;
                }
            }
            else {
                return idx;
            }
        }
        data.close();
    }
    return idx;
}



template <typename T>
size_t remove_if_y(std::vector<T>& x, std::vector<T>& y, const T& compare, const T &plusminus) {

    std::vector<T> a(x);
    std::vector<T> b(y);

    x.clear();
    y.clear();
    x.reserve(a.size());
    y.reserve(b.size());

    size_t i, idx = 0;

    for (i = 0; i < a.size(); ++i) {

        if ( (b.at(i) < compare + plusminus ) && (b.at(i) > compare - plusminus) ) {
            x.push_back(a.at(i));
            y.push_back(b.at(i));

            ++idx;

        }

    }

    return idx;

}



/*!
 * \brief values_and_indices
 * \param values list of actual frequencies
 * \param vals_to_find list of all possible target frequencies
 * \param found subset of possible frequencies
 * \return vector with iterator position of actual frequencies
 */
template <typename T>

std::vector<T*> values_and_indices(const std::vector<T> &values, const std::vector<T> &vals_to_find) {
    std::vector<T*> positions;
    T* pos;
    T* last = values.end();



    for (size_t i = 0; i < vals_to_find.size(); ++i) {
        pos = find(values.begin(), values.end(), vals_to_find[i]);
        if (pos != last) {
            positions.push_back(pos);
        }
    }

    return positions;
}

#endif // VECTOR_UTILS_H
