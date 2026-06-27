/***************************************************************************
                          my_vector.h  -  description
                             -------------------
    begin                : Tue Sep 14 2004
    copyright            : (C) 2004 by
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

/***************************************************************************
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
***************************************************************************/

#ifndef MY_VECTOR_H
 #define MY_VECTOR_H

 #include <vector>
 #include <valarray>
 #include <string>
 #include <iostream>
 #include <fstream>
 #include <complex>
#include "allinclude.h"


using namespace std;


namespace my_vector {

/*!
compares two vectors; if one of them contains a value defined as empty the cooresponding
element is deleted in both vectors
*/
template<class T> size_t check_empty(vector<T>& f, vector<T>& z, const T& empty ) {

  if (f.size() != z.size()) {
    cerr << "check_empty:: vectors have different length " << f.size() << "  " << z.size() << endl;
    return 0;
  }

  vector<T>::iterator i = f.begin();
  vector<T>::iterator j = z.begin();

  while(i != f.end()) {

    if (( *i == empty) || (*j == empty) ) {
        cerr << "de ";
        f.erase(i); z.erase(j);
    }
    ++i;
    ++j;

  }





}













} // end of namespace vector

#endif
