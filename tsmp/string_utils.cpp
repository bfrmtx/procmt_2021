/***************************************************************************
                          string_utils.cpp  -  description
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
#include "string_utils.h"

string str2degrees(string &in) {

  size_t pos;
  pos = in.rfind(".");

  if (pos == 0)
    in.insert(0, "00:00:00");
  else if (pos == 1)
    in.insert(0, "00:00:0");
  else if (pos == 2)
    in.insert(0, "00:00:");
  else if (pos == 3) {
    in.insert(pos - 2, ":");
    in.insert(0, "00:0");
  } else if (pos == 4) {
    in.insert(pos - 2, ":");
    in.insert(0, "00:");
  } else if (pos == 5) {
    in.insert(pos - 2, ":");
    in.insert(pos - 4, ":");
    in.insert(0, "0");
  }

  else if (pos >= 6) {
    in.insert(pos - 2, ":");
    in.insert(pos - 4, ":");
  }

  return in;
}

int calsernum(const string &filename, string &sernum, string &coilname) {

  string::size_type l;
  string tsernum;

  l = filename.rfind("mfs06");
  coilname = "mfs06";
  if (l != filename.npos)
    tsernum.assign(filename, l + 5, filename.size());
  else
    l = filename.rfind("mfs6");
  if (l != filename.npos)
    tsernum.assign(filename, l + 4, filename.size());
  coilname = "mfs07";
  if (l != filename.npos)
    tsernum.assign(filename, l + 5, filename.size());
  else
    l = filename.rfind("mfs7");
  if (l != filename.npos)
    tsernum.assign(filename, l + 4, filename.size());
  else {
    tsernum = filename;
    coilname = "";
  }

  noext(tsernum, sernum);
  // cerr << "calsernum: " << sernum << endl;

  return 1;
}

/***************************************************************************
 *                                                                         *
 *   string interface of file name utilities                               *
 *                                                                         *
 *   these will be used as well in the char* functions and mixed           *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

int newext(const string &filename, string &newfile, const char *ext) {

  string::size_type i;
  if (!filename.size()) {
    cerr << "\nnewext -> no input filename specified! -> \"" << filename
         << " \"\n";
    return 0;
  }
  string extension(ext);
  i = extension.find(".");
  noext(filename, newfile);
  if (i == extension.npos)
    // += does not work here
    newfile = newfile + "." + extension;
  else
    newfile += extension;
  return 1;
}

int noext(const string &filename, string &newfile) {

  string::size_type l;
  l = filename.rfind(".");
  if (l != filename.npos)
    newfile.assign(filename, 0, l);
  else
    newfile = filename;
  //  cerr << "noexct " << newfile << endl;
  return 1;
}

string ext(const char *filename) {
  string fn = filename;
  size_t l;

  l = fn.rfind(".");
  if (l != fn.npos) {
    fn.erase(0, l);
    return fn;
  } else {
    fn.erase();
    return fn;
  }
}

int mydir(const string &filename, string &mydirc) {
  string::size_type i, j;
  string t_string;
  if (!filename.size()) {
    cerr << "\nnewext -> no input filename specified! -> \"" << filename
         << " \"\n";
    return 0;
  }

  i = filename.rfind("/", 2);
  j = filename.rfind("/");

  // if (i != j && i != filename.npos && j != filename.npos) {
  //   mydirc.assign(filename, ++j, i - j - 1);
  // }

  // 2026:

  if (i != j && i != filename.npos && j != filename.npos) {
    j++;
    mydirc.assign(filename, j, i - j - 1);
  }

  /*
  // if ( i != j && i != t_string.npos && j != t_string.npos) {
    if ( i != j && i != t_string.npos) {
       if (j) my_dir_is.assign(t_string, ++j, i-j-1);
       else  my_dir_is.assign(t_string, j, i);
       i = my_dir_is.rfind("SITE");
       if (i == my_dir_is.npos) i = my_dir_is.rfind("site");
       if (i != my_dir_is.npos) {
       j =  my_dir_is.rfind("/");
       if (j) my_sitenumber_is.assign(my_dir_is, i+4, j-4);
     }
  */

  else {
    t_string = getenv("PWD");
    i = t_string.rfind("/");
    mydirc.assign(t_string, i, t_string.npos);
  }

  // else cerr << "please cd .. to obtain the directory info" << endl;

  return 1;
}

int askyn() {
  char ch, ix[80];
  int i;
  cin.get(ch); // get carriage return or char
  if (iscntrl(ch) || ch == 'n' || ch == 'N' || ch == '0')
    i = 1;
  else
    i = 0;
  if (!iscntrl(ch))
    cin.getline(ix, 80, '\n');
  return i;
}

//*****************************************************************
// default: return = yes

int askny() {
  char ch, ix[80];

  int i;
  cin.get(ch); // get carriage return or char
  if (iscntrl(ch) || ch == 'y' || ch == 'Y' || ch == '1')
    i = 1;
  else
    i = 0;
  if (!iscntrl(ch))
    cin.getline(ix, 80, '\n');
  return i;
}

/***************************************************************************
 *                                                                         *
 *   provide same functionality for the classical chars                    *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

int newextccc(const char *filename, char *newfile, char *extension) {

  string f = filename, n = newfile;
  newext(f, n, extension);
  strcpy(newfile, n.c_str());
  return 1;
}
int noextccc(const char *filename, char *newfile) {
  string f = filename, n = newfile;
  noext(f, n);
  strcpy(newfile, n.c_str());
  return 1;
}

int mydirccc(const char *filename, char *mydirc) {
  string f = filename, n = mydirc;
  mydir(f, n);
  strcpy(mydirc, n.c_str());
  return 1;
}

/***************************************************************************
 *                                                                         *
 *   use the same for mixed: chars as input and string as new files        *
 *                                                                         *
 *  useful if char* argv is used as first arg                              *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

int newextcsc(const char *filename, string &newfile, char *extension) {

  string f = filename;
  newext(f, newfile, extension);
  return 1;
}
int noextcsc(const char *filename, string &newfile) {
  string f = filename;
  noext(f, newfile);
  return 1;
}

int mydircsc(const char *filename, string &mydirc) {
  string f = filename;
  mydir(f, mydirc);
  return 1;
}

bool compare_strs(const string str, const vector<string> &compares,
                  int comp_lower, int comp_upper) {

  size_t i;

  for (i = 0; i < compares.size(); i++) {
    if (str == compares[i])
      return true;
  }
  return false;
}
int tfnewext(const string &filename, const string &filename2, string &newfile,
             const char *sep, const char *ext) {

  string temp;

  noext(filename, newfile);
  temp = newfile + sep;
  newfile.erase();
  noext(filename2, newfile);
  newfile = temp + newfile + ext;
  return 1;
}

string GMTtime(const long &time_secs) {

  struct tm *gm;
#ifdef _msvc
  long long int times = time_secs;
#else
  long int times = time_secs;
#endif
  gm = gmtime(&times);
  return asctime(gm);
}

string GMTISOtime(long int time_secs, size_t const counter,
                  const double samplefreq, double extra_shift_sec) {
  // return date in ISO 8601
  // works only if sample freq is n*2 or 1/(n*2) Hz
  string iso;
  stringstream stmp;
  double tadd;
  // add extra shift - e.g. to correct the filter length
  double extra_add_full_secs = 0.0;
  double extra_sub_sec = 0.0;

  if (extra_shift_sec != 0.0 && extra_shift_sec != DBL_MAX) {
    extra_sub_sec = modf(extra_shift_sec, &extra_add_full_secs);
    if (extra_sub_sec < 0.0) {
      --time_secs;
      extra_sub_sec += 1.0;
    }
  }
  double ftmp = counter / samplefreq;
  struct tm *gm;

  ftmp = modf(ftmp, &tadd);
  time_secs += long(tadd) + long(extra_add_full_secs);
#ifdef _msvc
  long long int times = time_secs;
#else
  long int times = time_secs;
#endif
  gm = gmtime(&times);

  stmp << gm->tm_year + 1900 << "-" << gm->tm_mon + 1 << "-" << gm->tm_mday
       << "T" << gm->tm_hour << ":" << gm->tm_min << ":"
       << (gm->tm_sec + ftmp + extra_sub_sec);

  stmp >> iso;
  return iso;
}

size_t clean_b_str(string &s) {

  s = btrim(s);
  s = trim(s);
  s = bltrim(s);
  s = ltrim(s);

  return s.size();
}

void string_z(char *chars, size_t size) {

  size_t i;
  for (i = 0; i < size; i++)
    chars[i] = '\x0';
}

int RdStrStr(ifstream &ifs, const string &s, string &strval, const char *delim,
             const int binary, const int holdposition, size_t headersize) {

  if (!ifs || ifs.fail() || ifs.bad()) {
    cerr << "\nRdStrStr can not access ifstream\n";
    return 0;
  }
  streampos pos;
  string gtl;

  pos = ifs.tellg();   // remember pos
  while (!ifs.eof()) { // search the file
    getline(ifs, gtl);

    if (binary)
      gtl.erase(headersize); // neccessary for binary files!
    // allow comment # in col 1 or 2 in text file
    if ((gtl.find(s) < gtl.npos) &&
        (gtl.find("#") > 1 || gtl.find("#") == gtl.npos)) {
      // # is  0, 1 or not existing
      //        cerr << "lfs" << endl;
      gtl = ltrim(gtl);                       // remove leading whitespaces
      gtl.erase(0, (gtl.find(s) + s.size())); // delete search string
      gtl = ltrim(gtl);                       // clean again
      if (!gtl.find("="))
        gtl.erase(0, 1); // remove "=" at position 0 if there is
      if (!gtl.find(":"))
        gtl.erase(0, 1); // or ":" alternativly
      gtl = ltrim(gtl);
      break;
    }

    if (ifs.tellg() > headersize)
      break;
  }

  if (ifs.eof()) {
    cerr << "\nRdStrStr -> string: \"" << s << "\" not found!" << endl;
    ifs.clear();
    ifs.seekg(pos);
    return 0;
  } else {
    size_t zz;
    zz = gtl.find(delim);

    if (gtl.find(delim) < gtl.npos) {
      strval = gtl.substr(0, gtl.find(delim));
    } else
      strval = gtl;
    if (holdposition)
      ifs.seekg(pos);
    return 1;
  }
}

int RdStrStr(ifstream &ifs, const string &s, string &strval, const char *delim,
             const int binary, const int holdposition) {

  return RdStrStr(ifs, s, strval, delim, binary, holdposition, ULONG_MAX);
}

int d_t_str2num(const string &s, const char *sep, int &first, int &second,
                double &last) {

  string cut;
  stringstream convert_num;

  cut = s.substr(0, s.find(sep));

  convert_num << cut << ends;
  convert_num >> first;

  convert_num.clear();
  convert_num.seekg(0);
  convert_num.seekp(0);

  cut = s.substr(s.find(sep) + 1, s.find_last_of(sep) - s.find(sep) - 1);
  convert_num << cut << ends;
  convert_num >> second;
  convert_num.clear();
  convert_num.seekg(0);
  convert_num.seekp(0);

  cut = s.substr(s.find_last_of(sep) + 1, s.size());
  convert_num << cut << ends;
  convert_num >> last;

  return 1;
}

/***************************************************************************
 *                                                                         *
 * If you have a dateformat like                                           *
 * 09.08.2003-12:55:00 *
 *                                                                         *
 * good for you - but not for the computer                                 *
 *                                                                         *
 ***************************************************************************/

int d_t_str2numemi(const string &s, const char *sep, const char *sep2,
                   const char *sep3, int &first, int &second, int &third,
                   int &fourth, int &fifth, double &last) {
  string cut, ts;
  stringstream convert_num;
  // size_t occ, occt;

  cut = s.substr(0, s.find(sep));
  convert_num << cut << ends;
  convert_num >> first;
  convert_num.clear();
  convert_num.seekg(0);
  convert_num.seekp(0);

  ts = s.substr(s.find(sep) + 1, s.size());
  cut = ts.substr(0, ts.find(sep));
  convert_num << cut << ends;
  convert_num >> second;
  convert_num.clear();
  convert_num.seekg(0);
  convert_num.seekp(0);

  ts = ts.substr(ts.find(sep) + 1, s.size());
  cut = ts.substr(0, ts.find(sep));
  convert_num << cut << ends;
  convert_num >> third;
  convert_num.clear();
  convert_num.seekg(0);
  convert_num.seekp(0);

  ts = ts.substr(ts.find(sep2) + 1, s.size());
  cut = ts.substr(0, ts.find(sep3));
  convert_num << cut << ends;
  convert_num >> fourth;
  convert_num.clear();
  convert_num.seekg(0);
  convert_num.seekp(0);

  ts = ts.substr(ts.find(sep3) + 1, s.size());
  cut = ts.substr(0, ts.find(sep3));
  convert_num << cut << ends;
  convert_num >> fifth;
  convert_num.clear();
  convert_num.seekg(0);
  convert_num.seekp(0);

  /*
   cut = s.substr(s.find(sep) + 1, s.find_last_of(sep) - s.find(sep) - 1 );
   convert_num  << cut << ends;
   convert_num  >> second;
   convert_num.clear();
   convert_num.seekg(0);
   convert_num.seekp(0);

  */

  cut = s.substr(s.find_last_of(sep3) + 1, s.size());
  convert_num << cut << ends;
  convert_num >> last;

  return 1;
}

// http://www.geocities.com/spur4444/prog/tokenizer.cpp.txt
vector<string> tokenize(string str, string delims) {
  string::size_type start_index, end_index;
  vector<string> ret;

  // Skip leading delimiters, to get to the first token
  start_index = str.find_first_not_of(delims);

  // While found a beginning of a new token
  //
  while (start_index != string::npos) {
    // Find the end of this token
    end_index = str.find_first_of(delims, start_index);

    // If this is the end of the string
    if (end_index == string::npos)
      end_index = str.length();

    ret.push_back(str.substr(start_index, end_index - start_index));

    // Find beginning of the next token
    start_index = str.find_first_not_of(delims, end_index);
  }

  return ret;
}

size_t FndStr(ifstream &ifs, const string &s, const int binary,
              const int holdposition, const size_t headersize) {

  if (!ifs || ifs.fail() || ifs.bad()) {
    cerr << "\nFndStr can not access ifstream\n";
    return 0;
  }
  streampos pos;
  string gtl;
  stringstream sconv;

  pos = ifs.tellg();   // remember pos
  while (!ifs.eof()) { // search the file
    getline(ifs, gtl);
    // Achtung das schein grosser Mist zu sein... was will ich denn l�chen,
    // warum = HEADERSIZE????
    if (binary)
      gtl.erase(headersize); // neccessary for binary files!
                             // allow comment # in col 1 or 2 in text file
    if ((gtl.find(s) < gtl.npos) &&
        (gtl.find("#") > 1 || gtl.find("#") == gtl.npos)) {
      // # is  0, 1 or not existing
      break;
    }
    if (ifs.tellg() > streampos(headersize)) {

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
