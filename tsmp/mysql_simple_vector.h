/***************************************************************************
                          mysql_simple_vector.h  -  description
                             -------------------
    begin                : Sun Apr 31 2006
    copyright            : (C) 2006 by
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

#ifndef MYSQL_SIMPLE_VECTOR_H
 #define MYSQL_SIMPLE_VECTOR_H

 #include <vector>
 #include <valarray>
 #include <string>
 #include <iostream>
 #include <fstream>
 #include <complex>
#include "allinclude.h"


using namespace std;
#include </usr/include/mysql++/mysql++.h>
using namespace std;
using namespace mysqlpp;

namespace mysql_simple_vector {



// this vor valarry AND vector new
// also with pre-set amount of values!!! fist middle, last!
/*! get all from table into my vector
 push_back is used - so I can only use vector here
*/
template<class T> size_t select_all(Query& query, string sql_col, Result& res, Row& row, Row::size_type& ri, vector<T>& v, size_t begin_row = 0, size_t end_row = INT_MAX) {

  query << "select * from " << sql_col << " limit " << begin_row << ", " << end_row;

  res = query.store();
  if (res) {
    ri = 0;
    while (row = res.at(ri)) {
     ++ri;
     v.push_back( (T) (row[sql_col.c_str()]));
    }
    query.reset();
   return v.size();
  }
  else {
    cerr << "Failed to get item list: " << query.error() << endl;
    query.reset();
    return 0;
  }

 }


/*! get all from table into my vector
 push_back is used - so I can only use vector here
*/
template<class T> size_t select_all(Query& query, string sql_col, Result& res, Row& row, Row::size_type& ri, T* beg, const T* end,
size_t begin_row = 0, size_t end_row = INT_MAX) {

size_t i = end - beg;
  query << "select * from " << sql_col << " limit " << begin_row << ", " << end_row;
  res = query.store();
  if (res) {
   ri = 0;
   while ((row = res.at(ri)) && (beg != end)) {
//    for (ri = 0; row = res.at(ri); ++ri) {         // recom. in examples by mysql
     ++ri;
     *beg = (double (row[sql_col.c_str()]));
     ++beg;
    }
    if (res.size() != i ) {
      cerr << "select_all returned " << res.size() << " rows, data vector/array was only " << i << " , " << i << " values read " << endl;

    }
    query.reset();
   return i;
  }
  else {
    cerr << "Failed to get item list: " << query.error() << endl;
    query.reset();
    return 0;
  }

 }






template<class T> size_t truncate_vector(Query& query, string sql_col, int also_truncate_vector, vector<T>& v ) {
  query << "truncate table " << sql_col << ";";
  query.execute();
  if (also_truncate_vector) v.resize(0);
  query.reset();



}
template<class T> size_t truncate_vector(Query& query, string sql_col, int also_truncate_vector, valarray<T>& v ) {
  query << "truncate table " << sql_col << ";";
  query.execute();
  if (also_truncate_vector) v.resize(0);
  query.reset();



}

/*
insert valarray of vector into database; changing beg or end will write a subset only!
*/
template<class T> size_t insert_all(Query& query, string sql_table, string sql_col, T* beg, const T* end ) {

query.precision(8);
query.setf(ios_base::scientific);
query << "insert into " << sql_table << "( " << sql_col << " )" << " values ";
size_t i = end - beg;

 while (beg != end - 1) {
  query << "(" << *beg << "), ";
  ++beg;
}
  query << "(" << *beg << "); ";
  query.execute();
  query.reset();

 return i;

}

} // end of namespace vector

#endif
