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

#ifndef MTX_TMATH_H
#define MTX_TMATH_H

#include <cmath>


template<typename T>
T roundMultiple( T value, T multiple )
{
    if (multiple == 0) return value;
    return static_cast<T>(round(static_cast<double>(value)/static_cast<double>(multiple))*static_cast<double>(multiple));
}

template<typename T>
T roundCeilMultiple( T value, T multiple )
{
    if (multiple == 0) return value;
    return static_cast<T>(std::ceil(static_cast<double>(value)/static_cast<double>(multiple))*static_cast<double>(multiple));
}

template<typename T>
T roundFloorMultiple( T value, T multiple )
{
    if (multiple == 0) return value;
    return static_cast<T>(std::floor(static_cast<double>(value)/static_cast<double>(multiple))*static_cast<double>(multiple));
}

template<typename T>
T autoMultiple(T multiple )
{
    if (multiple == 0) return multiple;
    T logs = floor(log10(multiple));


    return pow(10, logs-1) ;
}


#endif // MTX_TMATH_H
