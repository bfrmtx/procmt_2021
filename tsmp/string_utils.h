/***************************************************************************
                          string_utils.h  -  description
                            -------------------
    begin                : Sat Nov 11 2000
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
#ifndef _STRING_UTILS_
  #define _STRING_UTILS_

#include <QtGlobal>

#include <climits>    // INT_MAX
#include <cfloat>       //DBL_MAX

#include <cstring>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <climits>
#include <ctime>
#include <cmath>
#include <vector>
#include "allinclude.h"

using namespace std;


string str2degrees(string& in);
int calsernum(const string& sernum, string& newfile, string& coilname);
int newext(const string& filename, string& newfile, const char *ext);


// create a new filename from an old one without an extension
int noext( const string& filename, string& newfile);

// get the actual subdirectory (/home/cpp/test/a.cpp -> test)
// noch mal berarbeiten ... brauscht zb. // anstatt / usw
int mydir( const string& filename, string& mydirc);

// return the extension only
string ext(const char* filename);

int newextccc(const char *filename, char* newfile, char *extension);
int noextccc( const char *filename, char* newfile);
int mydirccc( const char *filename, char* mydirc);

int newextcsc(const char *filename, string& newfile, char *ext);
int noextcsc( const char *filename, string& newfile);
int mydircsc( const char *filename, string& mydirc);

bool compare_strs(const string str, const vector<string>& compares, int comp_lower = 0, int comp_upper = 0);

int askyn();  //returns 1 for n N, 0 for RETURN and others
int askny(); //returns 1 for y Y RETURN


// search a string within a string;
int RdStrStr(ifstream& ifs, const string &s, string &strval, const char* delim, const int binary, const int holdposition);
// same as above, but scan area is limited to header size
int RdStrStr(ifstream& ifs, const string &s, string &strval, const char *delim, const int binary, const int holdposition, size_t headersize);

// date time string to number
// string can be time 12:03:59.5 or date 2001:08:01 or 2001.08.01
int d_t_str2num(const string &s, const char* sep, int& first, int& second, double& last);
// 09.08.2003-12:55:00
int d_t_str2numemi(const string &s, const char *sep, const char *sep2, const char *sep3,
                  int& first, int& second, int& third, int& fourth, int& fifth, double& last);

// cleans a binary string
// sometimes a string has to be read in complete; one does not know whether
// the string is terminated or not
size_t clean_b_str(string& s);

// set all elements to \x0
void string_z(char *chars, size_t size);


// two files concatenated by a seperator, and a new extension
int tfnewext(const string& filename, const string& filename2, string& newfile,
            const char *sep, const char *ext);


string GMTtime(const long int &time_secs);
string GMTISOtime(long int time_secs, const size_t counter, const double samplefreq, double extra_shift_sec = 0.0);

// returns all items tokenized - e.g. get the fields from a CSV table
vector<string> tokenize(string str, string delims);

// non template jus for getting behind a string
size_t FndStr(ifstream& ifs, const string &s, const int binary, const int holdposition, const size_t headersize);

// code from Schrader and Kuhlins

// remove following empty spaces from a string
inline string trim(const string& s) {
    return s.substr(0, s.find_last_not_of(' ') + 1);
}

// remove leading empty spaces from a string
inline string ltrim(const string& s) {
    return s.empty() ? s: s.substr(s.find_first_not_of(' '));
}

// remove following terminators from a string
inline string btrim(const string& s) {
    return s.substr(0, s.find_last_not_of('\x0') + 1);
}

// remove leading terminators  from a string
inline string bltrim(const string& s) {
    return s.empty() ? s: s.substr(s.find_first_not_of('\x0'));
}


// convert to lower case letters
inline string lower(string& s) {
    for( string::size_type i = 0; i < s.length();  i++)
        if (isupper(s[i]))
            s[i] = static_cast<char>(tolower(s[i]));
    return s;
}

// convert to upper case letters
inline string upper(string& s) {
    for( string::size_type i = 0; i < s.length();  i++)
        if (islower(s[i]))
            s[i] = static_cast<char>(toupper(s[i]));
    return s;
}

// convert a copy to lower case letters
inline string return_lower(const string& s) {
	  string sc(s);
    for( string::size_type i = 0; i < sc.length();  i++)
        if (isupper(sc[i]))
            sc[i] = static_cast<char>(tolower(s[i]));
    return sc;
}

// convert a copy to upper case letters
inline string return_upper(const string& s) {
	  string sc(s);
    for( string::size_type i = 0; i < sc.length();  i++)
        if (islower(sc[i]))
            sc[i] = static_cast<char>(toupper(s[i]));
    return sc;
}

/***************************************************************************
* #x- position 2345          // sensor position x  will NOT be detected   *
*  x- position 2345          // sensor position x  will be detected       *
*  x- position = 2345        // sensor position x  will be detected       *
*  x- position: 2345         // sensor position x  will be detected       *                                                                         *
*  header size limits the scan area; take ULONG_MAX for merciless scan    *
***************************************************************************/
template <class T>
T RdStrVal(ifstream& ifs, const string &s, T &val,
          const int binary, const int holdposition, const size_t headersize) {

    if (!ifs || ifs.fail() || ifs.bad() ) {
        cerr << "\nRdStrVal can not access ifstream\n";
        return 0;
    }
    streampos pos;
    string gtl;
    stringstream sconv;

    pos = ifs.tellg();                                      // remember pos
    while (!ifs.eof()) {                                  // search the file
        getline(ifs,gtl);

        // Achtung das schein grosser Mist zu sein... was will ich den l�che, warum = HEADERSIZE????
        if (binary)
            gtl.erase(headersize);            // neccessary for binary files!                                             //allow comment # in col 1 or 2 in text file
        if( (gtl.find(s) < gtl.npos) && (gtl.find("#") > 1 || gtl.find("#") == gtl.npos )) {
            // # is  0, 1 or not existing

            gtl = ltrim(gtl);                                    // remove leading whitespaces
            gtl.erase(0, (gtl.find(s) + s.size()));              // delete search string
            gtl = ltrim(gtl);                                    // clean again
            if (!gtl.find("="))
                gtl.erase(0, 1);      // remove "=" at position 0 if there is
            if (!gtl.find(":"))
                gtl.erase(0, 1);      // or ":" alternativly
            break;
        }
        if (ifs.tellg() > streampos(headersize))
            break;
    }

    if (ifs.eof()) {
        cerr << "\nRdStrVal -> string: \"" << s << "\" not found!" << endl;
        ifs.clear();
        ifs.seekg(pos);
        return 0;
    } else {
        sconv << gtl << ends;                             // use strtream for conversion
        sconv >> val;
        if (holdposition)
            ifs.seekg(pos);
        return val;
    }

}

/***************************************************************************
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
***************************************************************************/
template <class T>
size_t FndStr(ifstream& ifs, const string &s, T &val, const int binary, const int holdposition, const size_t headersize) {

    if (!ifs || ifs.fail() || ifs.bad() ) {
        cerr << "\nFndStr can not access ifstream\n";
        return 0;
    }
    streampos pos;
    string gtl;
    stringstream sconv;

    pos = ifs.tellg();                                      // remember pos
    while (!ifs.eof()) {                                  // search the file
        getline(ifs,gtl);
        // Achtung das schein grosser Mist zu sein... was will ich denn l�chen, warum = HEADERSIZE????
        if (binary)
            gtl.erase(headersize);            // neccessary for binary files!
                                             //allow comment # in col 1 or 2 in text file
        if( (gtl.find(s) < gtl.npos) && (gtl.find("#") > 1 || gtl.find("#") == gtl.npos )) {
            // # is  0, 1 or not existing
           val = ltrim(gtl);
           break;
        }
        if (ifs.tellg() > streampos(headersize)){

            break;
        }

    }

    if (ifs.eof()) {
        cerr << "\nFndStr -> string: \"" << s << "\" not found!" << endl;
        ifs.clear();
        ifs.seekg(pos);
        return 0;
    } else {
        return ifs.tellg();
    }

}


/***************************************************************************
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/


template <class T>
string num2str(const T& num) {
    stringstream convert_num;
    string cut;

    convert_num << num << ends;
    convert_num >> cut;

    return btrim(cut);
}


//SerNoADU = str2num<int>(system_id);
template <class T>
T str2num(const string& cut) {
    T num;
    stringstream convert_num;
    convert_num << cut << ends;
    convert_num >> num;

    return num;
}


template <class T>
string num2str(const T& num, string& cut) {
    stringstream convert_num;

    convert_num << num;
    convert_num >> cut;

    return cut;
}


template <class T>
T str2num(const string& cut, T& num) {
    stringstream convert_num;
    convert_num << cut << ends;
    convert_num >> num;

    return num;
}


// number = 12; size = 4 -> result is 0012
template <class T>
string num2str_fill(const T& num, const size_t& size) {
    string cut = num2str(num);
    string pre;
    if  (size > cut.size()) {
      pre.assign( (size - cut.size()), '0');
      return pre + cut;
    }
    else return cut;

  //return cut;
}

template <class T > size_t hollerith2num(const string& instr, const int chars_per_num, vector<T>& numbers) {


    if (!numbers.size()) {
      cerr << "hollerith2num -> output vector must have the size of the amount of numbers in each line" << endl;
      return 0;
    }


    if (instr.size() < (chars_per_num * numbers.size())) {

      cerr << "hollerith2num -> inputstring.size(), size, n :" << instr.size() << "  " << chars_per_num << "  " << numbers.size() << endl;
      return 0;
    }


    string s;
    size_t i;

    for (i = 0; i < numbers.size(); i++) {
      s = instr.substr((i * chars_per_num), chars_per_num );
    //  cerr << "s->" << s << "<- s " << endl;
      str2num(s, numbers[i]);

    }


    return numbers.size();
}




/*
template <class T> T strdivt(const T& numer const T& denom,
    string& snumer, string& sdenom) {

string sres;
double res = double(numer) / double(denom);



}
*/
#endif
