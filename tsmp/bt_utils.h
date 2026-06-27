/***************************************************************************
                          bt_utils.h  -  description
                            -------------------
    begin                : Sat Jul 14 2001
    copyright            : (C) 2001 by 
    email                : 
***************************************************************************/

/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/
#ifndef BT_UTILS_H
#define BT_UTILS_H

/*! 
\file bt_utils.h
*/


/*! \brief A template collection basic utilities

\author B. Friedrichs
\date 24.02.2005

*/


#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "allinclude.h"

using namespace std;




/**
 * slow byte swap - but works fine
 * @param v will be swapped
 * @return returns the size of the swapped template
 */
template <class T> T tbswap (T& v) {
  unsigned char dst[sizeof(T)];
  const unsigned char *p = (const unsigned char*) &v;
  for (unsigned int j = 0; j < (int)sizeof(T); j++) {
   dst[j] = p[sizeof(T)-1-j];
    }
  v = *((T*)dst);
return sizeof(T);
}


/**
 * sets va = 1 if vb is one; else va == va
 * @param va boolean vector to be merged
 * @param vb 2nd boolean vector
 * @return returns number of merged elements
 */
template <class T> size_t merge_bool(vector<T>& va, const vector<T>& vb) {

  if (va.size() != vb.size() ) {
    cerr << "merge_bool -> vectors have different lengths; return" << endl;
    return 0;
  }
  
  for (size_t i = 0; i < va.size(); i++) {
    if (vb[i]) va[i] = 1;
  }
  

return va.size();
}


/**
 * find out how many entries are set true  
 * @param vb 
 * @return returns the number of elements true (or != 0)
 */
template <class T> size_t check_true(const vector<T>& vb) {

size_t truth = 0;
  for (size_t i = 0; i < vb.size(); i++) if (vb[i]) truth++;
return truth;
}


/**
 * find out how many elements are false
 * @param vb 
 * @return returns the number of !vb elements
 */
template <class T> size_t check_false(const vector<T>& vb) {

size_t isfalse = 0;
  for (size_t i = 0; i < vb.size(); i++) if (!vb[i]) isfalse++;
return isfalse;
}


/***************************************************************************
*                                                                         *
*returns a int / float /double from a char* which is not termninated      *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
***************************************************************************/

/**
 * converts a character*  to a number
 * @param num resulting number
 * @param str char* string
 * @param nchars number of chars to be used for conversion
 * @return returns the number
 */
template <class T> T char2num(T& num, char* str,  int nchars) {
string cut(str, 0, nchars);
stringstream convert_num;
  
  convert_num << cut << ends;
  convert_num >> num;
return num;
}



/***************************************************************************
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
***************************************************************************/
template <class T> T grad2num (T gr, T mi, T se) {
return gr + mi/60.+ se/3600.;
}


/***************************************************************************
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
***************************************************************************/

template <class T> T double2grad(T deg_grad, int& gr, int& mi, T& se) {

long double h1;
      gr = int(deg_grad);                // INT SHOULD not ROUND UP ... 2.6 -> 2
      h1 = deg_grad - gr;                // rest 0.6
      mi = int (h1 * 60);                // we should get the fraction here
      se = h1 * 3600 - mi * 60;
return gr;
}


/***************************************************************************
*                                                                         *
*                                                                         *
* radians into degrees minutes and seconds                                                                        *
*                                                                         *
*                                                                         *
*                                                                         *
***************************************************************************/
template <class T> T rad2grad(const T rad, int& gr, int& mi, T& se) {
long double h1, diff = 1.E-4;

      gr = int( (rad * 180.) /M_PI);        // INT SHOULD not ROUND UP ... 2.6 -> 2
      h1 = rad * 180./M_PI;
      h1 -= gr;                         // rest 0.6
      mi = int (h1 * 60);               // we should get the fraction here
      se = h1 * 3600 - mi * 60;


      // rount up if we get 12:59:59.99999 to 13:00:00
      if (gr >= 0) {
        if (se <  diff) se = +0;
        h1 = 60. - se;  
        if (h1 < diff) {
          se = 0.;
          if (mi != 59) mi++;
          else {
            mi = 0;
            gr++;
          }
        }
      }

      if (gr < 0) {
        if  ( -1. * se <  diff) se = -0.;
        h1 = 60. + se;  
        if (h1 < diff) {
          se = 0.;
          if (mi != -59) mi--;
          else {
            mi = 0;
            gr--;
          }
        }
      }


return gr;
}

/***************************************************************************
*                                                                         *
* degrees minutes and seconds into radians                                *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
***************************************************************************/
template <class T> T grad2rad(int gr, int mi, T se) {

return (gr + mi/60.+ se/3600.) * M_PI/180.;
}

#endif
