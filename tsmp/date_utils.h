/***************************************************************************
                          date_utils.h  -  description
                             -------------------
    begin                : Tue Dec 10 2002
    copyright            : (C) 2002 by B. Friedrichs
    email                : bernhard.friedrichs@coopertools.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef DATE_UTILS_H
 #define DATE_UTILS_H


#include <iostream>
#include "string_utils.h"
#include <vector>
#include <algorithm>

using namespace std;
#ifdef __CYGWIN__
 #define floorl floor
#endif
#include "allinclude.h"

/***************************************************************************
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
***************************************************************************/



int find_sync_times(vector<long int>& DateTimes,  const vector<size_t> total_smpls, vector<size_t>& skips,
                    vector<size_t>& smpls, const double smpl_f, const size_t& wdl, const int synctype);

int find_grid_time(const long int& DateTime, size_t& skip, const size_t& smpls, const double& smpl_f,
                   const int& reduct, const int& filter_length, const int& raster );










#endif
