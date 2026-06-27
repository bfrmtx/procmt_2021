/***************************************************************************
                          koordi.h  -  description
                            -------------------
    begin                : Mon Apr 10 2000
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
/*  C++ (z. B. fuer Conv)

Routinen zur Koordinatentransformation
nach einem Fortran Program von K. Ivory
*/
#ifndef _KOORDI_
#define _KOORDI_

#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <climits>
#include <string>
#include "bt_utils.h"
#include "string_utils.h"
#include "allinclude.h"


using namespace std;

class Ellipsoid {
public:
  Ellipsoid();
  Ellipsoid(int Id, const char* name, double radius, double ecc) {
    id = Id; ellipsoidName = name;
    EquatorialRadius = radius; eccentricitySquared = ecc;
  }

  int id;
  const char* ellipsoidName;
  double EquatorialRadius;
  double eccentricitySquared;

};

//setting up possible types
static Ellipsoid ellipsoid[] = {
//  id, Ellipsoid name, Equatorial Radius, square of eccentricity
  Ellipsoid( -1, "Placeholder", 0, 0),   //placeholder only, To allow array indices to match id numbers
  Ellipsoid( 1,  "Airy", 6377563, 0.00667054),
  Ellipsoid( 2,  "Australian National", 6378160, 0.006694542),
  Ellipsoid( 3,  "Bessel 1841", 6377397, 0.006674372),
  Ellipsoid( 4,  "Bessel 1841 (Nambia) ", 6377484, 0.006674372),
  Ellipsoid( 5,  "Clarke 1866", 6378206, 0.006768658),
  Ellipsoid( 6,  "Clarke 1880", 6378249, 0.006803511),
  Ellipsoid( 7,  "Everest", 6377276, 0.006637847),
  Ellipsoid( 8,  "Fischer 1960 (Mercury) ", 6378166, 0.006693422),
  Ellipsoid( 9,  "Fischer 1968", 6378150, 0.006693422),
  Ellipsoid( 10, "GRS 1967", 6378160, 0.006694605),
  Ellipsoid( 11, "GRS 1980", 6378137, 0.00669438),
  Ellipsoid( 12, "Helmert 1906", 6378200, 0.006693422),
  Ellipsoid( 13, "Hough", 6378270, 0.00672267),
  Ellipsoid( 14, "International", 6378388, 0.00672267),
  Ellipsoid( 15, "Krassovsky", 6378245, 0.006693422),
  Ellipsoid( 16, "Modified Airy", 6377340, 0.00667054),
  Ellipsoid( 17, "Modified Everest", 6377304, 0.006637847),
  Ellipsoid( 18, "Modified Fischer 1960", 6378155, 0.006693422),
  Ellipsoid( 19, "South American 1969", 6378160, 0.006694542),
  Ellipsoid( 20, "WGS 60", 6378165, 0.006693422),
  Ellipsoid( 21, "WGS 66", 6378145, 0.006694542),
  Ellipsoid( 22, "WGS-72", 6378135, 0.006694318),
  Ellipsoid( 23, "WGS-84", 6378137, 0.00669438)
};




class coordinates {


/* internal calculation will use -180 to 180 deg lon
                              and -90 to 90 deg lat

*/

private:
  long double latitude, longitude,   // latitude, longitude in rad!
              lat, lon,              // lat lon in grad
              rechts, hoch,          // Gauss Krueger
              se_lo, se_la,          // readable coodinates, grad
              northing, easting,     // UTM coordinates
              elevation;             // level above sea (NN) in meter
  int gr_lo, mi_lo, gr_la, mi_la;    // readable coodinates, grad
  int ReferenceEllipsoid;            // select one of the ellipsoids above
//  int latmsec, lonmsec;            // lat lon in msec from motorola clock
  int gk_meridian;                   // meridian for Gauss Krueger  in DEG
  int utm_meridian;                  // meridian for UTM   in DEG

  string UTMzone, LatH, LonH,        // UTM zone, LatH can be N or S, LonH can be E or W
        my_name_is;                  // store the Ellispoid name like WGS-84

  

public:
  void geo2gauss();
  void gauss2geo();

  void akn();
  void display_utm();
  void display_gauss();
  void display_lat_lon();

  void geo2UTM();
  void UTM2geo();
  
 

  void get_gauss(double& drechts, double& dhoch, int& iref_med);
  long double get_rechts() const {return rechts;}
  long double get_hoch() const {return hoch;}
  int get_gk_meridian() const {return gk_meridian;}

  void get_utm(double& dnorthing, double& deasting, int& izone, int& iumeridian, char& cletter);
  long double get_northing() const {return northing;}
  long double get_easting() const {return easting;}

  string get_utm_zone() const {return UTMzone;}


  coordinates();



  // get lat lon in radians
  int getlatlon(const double radlat, const double radlon);
  int getlatlon(const long double& radlat, const long double& radlon);
  int getlatlon(const int msec_lat, const int msec_lon);
  int getlatlon(const string& slat, const string& slon);

  int getlatlon(const int& lag, const int& lam, const double& las, const string& LatH, const int& log, const int& lom, const double& los, const string& LonH);
  // set Gauss Krueger and /or UTM merdian to INT_MAX if you want
  // automatic calculation
  int getlatlon(const int lag, const int lam, const double las, const int log, const int lom, const double los,
      const int utmmer, const int gkmer, const int ell);

  int getlatlon(const int lag, const int lam, const double las, const char* ns, const int log, const int lom, const double los,
      const char* ew, const int utmmer, const int gkmer, const int ell);

  int getmeridian_ell(const int& utmmer, const int& gkmer, const int& ell);
  void get_lat_lon(int& lag, int& lam, double& las, string& LatH, int& log, int& lom, double &los, string& LonH) const;
  void get_lat_lon(string& slat, string& slon);
  void get_lat_lon_h_m(string& slat, string& slon, const int& add_min);
  void get_NS_EW(string& NS, string& EW) const;
  void get_msecs(int& msec_lat, int& msec_lon);

  void add(const int& lag, const int& lam, const double& las, const int& log, const int& lom, const double& los);

  coordinates& operator -= (const coordinates& cdn);
  coordinates& operator += (const coordinates& cdn);
  coordinates& operator = (const coordinates& cdn);






private:

  void set_z();                              // zero all components

  char UTMLetterDesignator();

  long double sqr(const long double &sq) {return sq * sq;}   // relict from ancient times

  void flip();                                    // if lat or lon are negative... se text in .cpp


};

#endif
