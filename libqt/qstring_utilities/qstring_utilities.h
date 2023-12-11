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

#ifndef QSTRING_UTILITIES_H
#define QSTRING_UTILITIES_H

#include "qstring_utilities_global.h"
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QDateTime>
#include <QDebug>
#include <QMap>
#include <QPair>


/*!
 * \brief round_digits
 * \param ndig_9_0 search for ddd.000x or ddd.999x and make  next number of it 1.9999 -> 2, 6.0001 -> 6
 * \param in
 * \return
 */

QString round_digits(const int ndig_9_0, const QString &in);

/*!
 * \brief round_digits
 * \param ndig_9_0 search for .000x or .999x and make  next number of it 1.9999 -> 2, 6.0001 -> 6
 * \param in
 * \return
 */
QString round_digits(const int ndig_9_0, const double &in);

QStringList xmls_in_site(const QString &survey_base_dir, const QString &site_dir);

QList<QList<QFileInfo>> ats_in_site(const QString &survey_base_dir, const QString &site_dir);

QStringList sites_in_survey_basedir(const QString &survey_base_dir);

int xml_measdoc_overlap_time(QStringList &local, QStringList &remote, QList<qint64> &overlap_secs);

QPair<double, QString> xml_measdoc_to_f_s(const QString &measdoc);

QMap <QString, QVariant> atsfilename_values(const QString &filename);

QMap <QString, QVariant> atsfilename_site_survey(const QString &absloute_filename);



void xml_measdoc_to_start_stop(const QString &measdoc, QDateTime &start, QDateTime &stop, QString &fstr);

QStringList measdocs_and_frequencies(const QStringList &allmeasdocs, const std::vector<double> &freqs);

#endif // QSTRING_UTILITIES_H
