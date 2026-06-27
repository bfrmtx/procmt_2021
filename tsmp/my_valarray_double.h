/***************************************************************************
                          my_valarray_double.h  -  description
                             -------------------
    begin                : Sun Nov 12 2000
    copyright            : (C) 2000 by B. Friedrichs
    email                : geo@metronix.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef MY_VALARRAY_DOUBLE_H
 #define MY_VALARRAY_DOUBLE_H

#include <iostream>
#include <valarray>
#include <cfloat> 			//DBL_MAX
#include "allinclude.h"

using namespace std;

namespace my_valarray_double {
// akima
// x and y are the original vectors, b, c, and d will be calculted.....
int aspline(const valarray<double>& x, const valarray<double>& y,
            valarray<double>& b, valarray<double>& c, valarray<double>& d);
// after spline or aspline was used
// u will be the value of the new x axis; seval returns the new y
double seval(const double u, const valarray<double>& x, const valarray<double>& y,
             const valarray<double>& b, const valarray<double>& c, const valarray<double>& d);

double seval_const_borders(double u, const valarray<double>& x, const valarray<double>& y,
                           const valarray<double>& b, const valarray<double>& c, const valarray<double>& d);


int spline(const valarray<double>& x, const valarray<double>& y,
           valarray<double>& b, valarray<double>& c, valarray<double>& d);

//running avarage
int runavg(const valarray<double>& x, const valarray<double>& y, valarray<double>& ax,
           valarray<double>& ay, unsigned int ilen);
// filter
size_t filx(const valarray<double>& in, valarray<double>& out, const unsigned int reduction,
            const valarray<double>& cadblFilterFac, const double& scale);


size_t resample(const valarray<double>& in, valarray<double>& out, const double old_D_f, const double new_D_f);



} // end of namespace my_valarray_double

#endif
