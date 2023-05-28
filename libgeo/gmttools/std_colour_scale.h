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

#ifndef STD_COLOUR_SCALE_H
#define STD_COLOUR_SCALE_H


#include <QList>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QTextStream>

#include <cmath>
#include <iostream>


class std_colour_scale
{
public:
    std_colour_scale(const QString which = "", const bool extends_colour = true, const bool nodata_white = true, const bool revert = false);

    int create_cpt_scale(QTextStream *qtx,  const double min, const double max, const bool contour = false, const bool log = false, bool round = false);



    QList<double> vals;
    double min;
    double minl;
    double max;
    double malx;
    QStringList rgb;

    QTextStream cpttext;
    QTextStream cpttextlog;

    bool extends_colour;
    bool nodata_white;

    QString N;   /*! use colour for no data */
    QString B;   /*! use colour for out of range lower */
    QString F;   /*! use colour for out of range above */



};

#endif // STD_COLOUR_SCALE_H
