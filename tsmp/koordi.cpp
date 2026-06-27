/***************************************************************************
                          koordi.cpp  -  description
                            -------------------
    begin                : Mon Apr 10 2000
    copyright            : (C) 2004 by B. Friedrichs
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
getestet:
Laenge        Breite         Meridian
10 42' 59.3215'' 48 26' 45.4355''  12
rechts: 4.405057628849e+06  hoch: 5.368263247964e+06
Ergebinis 25.6.97

Laenge         Breite
10 31' 55.9''  52 15' 39.5''
Soll 4399796.57      5792799.64 rechts hoch
Ist  4399796.266583  5792800.787294




*/
#include "koordi.h"

// latitude, longitude in rad !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

/***************************************************************************
*                                                                         *
*   first provide a function for clearing the class                       *
*                                                                         *
*   and a function for getting the lats and longs                         *
*                                                                         *
*                                                                         *
***************************************************************************/



void coordinates::set_z() {
    latitude = longitude = rechts = hoch = lat = lon = 0.;
    northing = easting = 0.;
    this->gr_lo = this->mi_lo = this->gr_la = this->mi_la = 0;
    this->se_lo =  this->se_la = 0.;
    elevation = 0.;
    // undefined
    gk_meridian = INT_MAX;
    this->utm_meridian = INT_MAX;
    ReferenceEllipsoid = INT_MAX;

    this->LatH = "x";
    this->LonH = "x";

    // 	latmsec = lonmsec = 0;

}


/***************************************************************************
*                                                                         *
*  constructor with latitudes and longitudes                              *
*              and meridians and ellipsoid                                *
*                                                                         *
*                                                                         *
*                                                                         *
***************************************************************************/


coordinates::coordinates() {
    set_z();

    // as default start up we take the WGS-84
    ReferenceEllipsoid = 23;
    my_name_is = ellipsoid[ReferenceEllipsoid].ellipsoidName;
}



int	coordinates::getlatlon(const double radlat, const double radlon) {

    latitude = radlat;
    lat = (latitude * 180.) / M_PI;

    longitude = radlon;
    lon = (longitude * 180.) / M_PI;

    if (longitude > M_PI ) {
        longitude -= (2.* M_PI);
        lon -=360.,
              this->LonH = "W";
    }
    else if (longitude < 0)
        this->LonH = "W";
    else
        this->LonH = "E";

    if (latitude < 0)
        this->LatH = "S";
    else
        this->LatH = "N";

    rad2grad(longitude, this->gr_lo, this->mi_lo, this->se_lo);
    rad2grad(latitude, this->gr_la, this->mi_la, this->se_la);
    flip();
   
    return 1;
}
int	coordinates::getlatlon(const long double& radlat, const long double& radlon) {

    latitude = radlat;
    lat = (latitude * 180.) / M_PI;

    longitude = radlon;
    lon = (longitude * 180.) / M_PI;


    if (longitude > M_PI ) {
        longitude -= (2.* M_PI);
        lon -=360.,
              this->LonH = "W";
    }
    else if (longitude < 0)
        this->LonH = "W";
    else
        this->LonH = "E";

    if (latitude < 0)
        this->LatH = "S";
    else
        this->LatH = "N";

    rad2grad(longitude, this->gr_lo, this->mi_lo, this->se_lo);
    rad2grad(latitude, this->gr_la, this->mi_la, this->se_la);
    flip();
    

    return 1;
}

int coordinates::getlatlon(const int msec_lat, const int msec_lon) {

    double radlat, radlon;
    

    radlat = (msec_lat * M_PI) /( 3600000. * 180.);
    radlon = (msec_lon * M_PI) /( 3600000. * 180.);

    getlatlon(radlat, radlon);


    return 1;
}



int coordinates::getlatlon(const string& slat, const string& slon) {

int lag;
int lam;
double las;
int log;
int lom;
double los;

// I do not use the this-> here because coordinates will be reset after reception!!
  d_t_str2num(slat, ":", lag, lam, las);
  d_t_str2num(slon, ":", log, lom, los);

  return coordinates::getlatlon(lag, lam, los, LatH, log, lom, los, LonH);

}

void coordinates::get_msecs(int& msec_lat, int& msec_lon){

   msec_lat = int ((latitude  * 3600000. * 180.) / M_PI);
   msec_lon = int ((longitude * 3600000. * 180.) / M_PI);


}

void coordinates::add(const int& lag, const int& lam, const double& las, const int& log, const int& lom, const double& los) {

    this->gr_la += lag;
    this->mi_la += lam;
    this->se_la += las;
    this->gr_lo += log;
    this->mi_lo += lom;
    this->se_lo += los;

}

int coordinates::getlatlon(const int& lag, const int& lam, const double& las, const string& LatH, const int& log, const int& lom, const double& los, const string& LonH) {

if ( (lag == INT_MAX) ||  (lam == INT_MAX) || (las == DBL_MAX) || (log == INT_MAX) ||  (lom == INT_MAX) || (los == DBL_MAX)) {
 cerr << "coordinates::getlatlon-> at least on of the latitude / longitude entries is not valid " << endl;
      cerr << lag << ":"  << lam  << ":" << las << "     " << log << ":" << lom << ":" << los << endl;

  return 0;
}



    set_z();

    this->gr_la = lag;
    this->mi_la = lam;
    this->se_la = las;
    this->gr_lo = log;
    this->mi_lo = lom;
    this->se_lo = los;
    
    this->LatH = LatH;
    this->LonH = LonH;

    latitude = grad2rad(this->gr_la, this->mi_la, this->se_la);
    lat = lag + lam / 60. + las / 3600.;

    longitude = grad2rad(this->gr_lo, this->mi_lo, this->se_lo);
    lon = log + lom / 60. + los / 3600.;

    // map 190 deg to 360 - 190 =    -170 deg
    // and indicate hemisphere
if (LatH == "x" && LonH == "x") {
    if (longitude > M_PI ) {
        longitude -= (2* M_PI);
        lon -=360.,
              this->LonH = "W";
    }
    else if (longitude < 0)
        this->LonH = "W";
    else
        this->LonH = "E";

    if (latitude < 0)
        this->LatH = "S";
    else
        this->LatH = "N";
}
    rad2grad(longitude, this->gr_lo, this->mi_lo, this->se_lo);
    rad2grad(latitude, this->gr_la, this->mi_la, this->se_la);
   
    flip();


    return 1;

}



// next
int coordinates::getlatlon(const int lag, const int lam, const double las, const int log, const int lom,
                           double los, const int utmmer, const int gkmer, const int ell) {

if ( (lag == INT_MAX) ||  (lam == INT_MAX) || (las == DBL_MAX) || (log == INT_MAX) ||  (lom == INT_MAX) || (los == DBL_MAX)) {
 cerr << "coordinates::getlatlon-> at least on of the latitude / longitude entries is not valid " << endl;
      cerr << lag << ":"  << lam  << ":" << las << "     " << log << ":" << lom << ":" << los << endl;

  return 0;
}


    set_z();

    this->gr_la = lag;
    this->mi_la = lam;
    this->se_la = las;
    this->gr_lo = log;
    this->mi_lo = lom;
    this->se_lo = los;

    latitude = grad2rad(this->gr_la, this->mi_la, this->se_la);
    lat = lag + lam / 60. + las / 3600.;

    longitude = grad2rad(this->gr_lo, this->mi_lo, this->se_lo);
    lon = log + lom / 60. + los / 3600.;

    // map 190 deg to 360 - 190 =    -170 deg
    // and indicate hemisphere
    if (LatH == "x" && LonH == "x") {
    if (longitude > M_PI ) {
        longitude -= (2. * M_PI);
        lon -=360.,
              this->LonH = "W";
    }
    else if (longitude < 0)
        this->LonH = "W";
    else
        this->LonH = "E";

    if (latitude < 0)
        this->LatH = "S";
    else
        this->LatH = "N";
    }
    rad2grad(longitude, this->gr_lo, this->mi_lo, this->se_lo);
    rad2grad(latitude, this->gr_la, this->mi_la, this->se_la);


    flip();
    getmeridian_ell(utmmer, gkmer, ell);
   
    return 1;

}

/***************************************************************************
*                                                                         *
*  constructor with latitudes and longitude                               *
*              and meridians and ellipsoid                                *
*                                                                         *
*               ns should be N or S                                          *
*               ew should be E or W                                       *
***************************************************************************/


int coordinates::getlatlon(const int lag, const int lam, const double las, const char* ns,
                           const int log, const int lom, const double los, const char* ew, const int utmmer, const int gkmer, const int ell) {

if ( (lag == INT_MAX) ||  (lam == INT_MAX) || (las == DBL_MAX) || (log == INT_MAX) ||  (lom == INT_MAX) || (los == DBL_MAX)) {
 cerr << "coordinates::getlatlon-> at least on of the latitude / longitude entries is not valid " << endl;
 cerr << lag << ":"  << lam  << ":" << las << "     " << log << ":" << lom << ":" << los << endl;

  return 0;
}


    set_z();
    this->gr_la = lag;
    this->mi_la = lam;
    this->se_la = las;
    this->gr_lo = log;
    this->mi_lo = lom;
    this->se_lo = los;

    this->LonH.assign(ew, 1);
    this->LatH.assign(ns, 1);

    upper(this->LonH);
    upper(this->LatH);

    latitude = grad2rad(this->gr_la, this->mi_la, this->se_la);
    lat = lag + lam / 60. + las / 3600.;

    longitude = grad2rad(this->gr_lo, this->mi_lo, this->se_lo);
    lon = log + lom / 60. + los / 3600.;

    if (longitude > M_PI && this->LonH == "E") {
        longitude = (2. * M_PI) - longitude;
        lon = 360. -lon;
        this->LonH = "W";
    }




    if (this->LonH == "W") {
        longitude *= -1.;
        lon *= -1.;
    }

    if (this->LatH == "S") {
        latitude *= -1.;
        lat *= -1.;
    }

    rad2grad(longitude, this->gr_lo, this->mi_lo, this->se_lo);
    rad2grad(latitude, this->gr_la, this->mi_la, this->se_la);

    flip();

    getmeridian_ell(utmmer, gkmer, ell);

    return 1;
}


/***************************************************************************
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
***************************************************************************/


int coordinates::getmeridian_ell(const int& utmmer, const int& gkmer, const int& ell = 23) {

if (utmmer == INT_MAX) {
  cerr << "coordinates::getmeridian_ell -> at least utm meridian has to be 0, 6, 12 ..." << endl;
  return 0;

}

    if(utmmer != INT_MAX) {
        if ( ( (utmmer + 3) % 6 ) == 0) {
            this->utm_meridian = utmmer;
            if (this->utm_meridian >  180)
                this->utm_meridian -= 360;
        } else
            cerr << "coordinates::getmeridian -> UTM Merdian not valid, using auto calculation" << endl;

    }

    if (gkmer == INT_MAX) gk_meridian = this->utm_meridian;

    if ( ( gkmer % 3 ) == 0)
        gk_meridian = gkmer;
    else
        cerr << "coordinates::getmeridian -> Gauss Krueger Merdian not valid, using auto calculation" << endl;
    if (gk_meridian < 0)
        gk_meridian = 360 + gk_meridian;


    if ( (ell != INT_MAX) && (ell < 24) && (ell > 0) )
        ReferenceEllipsoid = ell;
    else
        ReferenceEllipsoid = 23;
    my_name_is = ellipsoid[ReferenceEllipsoid].ellipsoidName;

    return 1;
}







/***************************************************************************
*                                                                         *
*                                                                         *
* acknoledge                                                              *
*                                                                         *
*                                                                         *
*                                                                         *
***************************************************************************/
void coordinates::akn() {

    ios::fmtflags oldopts = cerr.flags();

    cerr << "lat\x9\x9: " << setfill(' ') << setw(3) << this->gr_la << " " << setfill('0') << setw(2)
    << this->mi_la << " " <<  setfill('0') << setw(6) << this->se_la << "  " << this->LatH;
    cerr <<  "  grad / rad : " <<  lat << "  " << latitude << endl;

    cerr << endl;



    cerr << "lon\x9\x9: " << setfill(' ') << setw(3) << this->gr_lo << " " << setfill('0') << setw(2)
    << this->mi_lo <<" " << setfill('0') << setw(6) << this->se_lo << "  " << this->LonH;

    cerr << "  grad / rad: " << lon << "  " << longitude << endl;
    cerr << endl;
    cerr << "UTM and Gauss Krueger Merdian: " << this->utm_meridian << "  " << gk_meridian << endl;
    cerr << "Reference Ellipsoid          : " << my_name_is << endl;
    cerr << endl;
    cerr.flags(oldopts);
}

/***************************************************************************
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
***************************************************************************/

void coordinates::display_utm() {
    ios::fmtflags oldopts = cerr.flags();
    cerr.setf(ios::showpoint);
    cerr.precision(10);
    cerr << "UTM Northing\x9: " << northing << endl;
    cerr << "UTM Easting\x9: " << easting << endl;
    cerr << "UTM Zone\x9: " << UTMzone << endl;
    cerr << "UTM Meridian\x9: " << this->utm_meridian << endl;
    cerr.flags(oldopts);
}

/***************************************************************************
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
***************************************************************************/

void coordinates::display_gauss() {
    ios::fmtflags oldopts = cerr.flags();
    cerr.setf(ios::showpoint);
    cerr.precision(10);
    cerr << "GK Hochwert\x9: " << hoch << endl;
    cerr << "GK Rechtswert\x9: " << rechts << endl;
    cerr << "GK Meridian\x9: " << gk_meridian << endl;
    cerr.flags(oldopts);
}



void coordinates::display_lat_lon() {

    cerr << "lat:\x9" << setfill(' ') << setw(3) << this->gr_la << ":" << setfill('0') << setw(2)
    << this->mi_la << ":" <<  setfill('0') << setw(2) << this->se_la << " " << this->LatH << endl;


    cerr << "lon:\x9" << setfill(' ') << setw(3) << this->gr_lo << ":" << setfill('0') << setw(2)
    << this->mi_lo << ":" << setfill('0') << setw(2) << this->se_lo << " " << this->LonH << endl;
}

/***************************************************************************
*                                                                         *
*                                                                         *
* make values available                                                   *
*                                                                         *
* missing check: did we already calculate?                                *
*                                                                         *
***************************************************************************/

void coordinates::get_gauss(double& drechts, double& dhoch, int& iref_med) {

    drechts = rechts;
    dhoch = hoch;
    iref_med = gk_meridian;

}

void coordinates::get_utm(double& dnorthing, double& deasting, int& izone,
                          int& iumeridian, char& cletter) {

    dnorthing = northing;
    deasting = easting;
    iumeridian = this->utm_meridian;
    izone = atoi(UTMzone.c_str());
    cletter = UTMzone[UTMzone.size()-1];
}


/***************************************************************************
*  if lat or lon are negative                                             *
*  set the this->gr_la and gra_lo values POSITIVE                               *
*  and indicate with S and W                                              *
*                                                                         *
*                                                                         *
*                                                                         *
***************************************************************************/

void coordinates::flip() {

    if (latitude < 0) {
        this->gr_la *= -1;
        this->mi_la *= -1;
        this->se_la *= -1.;
        this->LatH = "S";

    }

    if (longitude < 0) {
        this->gr_lo *= -1;
        this->mi_lo *= -1;
        this->se_lo *= -1.;
        this->LonH = "W";

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


void  coordinates::geo2gauss () {
    /*
    c  Die Laender der Bundesrepublik Deutschland verwenden als amtliche
    c  Bezugsflaeche das Bessel-Ellipsoid (s. /SCH81/, S.2).
    c  BESSEL-Ellipsoidkonstanlten:
    c  a : grosse Halbachse                    a  = 6 377 397,155 m
    c  b : kleine Halbachse                   b  = 6 356 078,963 m
    c  c : Polkruemmungsradius                 c  = sqr(a)/b
    c  e': zweite numerische Exzentrizitaet   e2 := e'2 = (a2-b2)/b2

    internationales Ellipsoid anders definiert, etwa
    a = 6378388
    b = 6356911.946
    f = 1/127 usw.

    */

    long double a, b, c, e2, Q, ef2, ef4, ef6,
    G, eta2, N, y,  cL, dL, c2, t2;




    // fix the problem that this algorithm works with 0...360 deg instead of
    // -180 .. 180
    // this applies also for the Meridian

    long double temp_long = longitude;
    long double three = 3.0;
    if (longitude < 0)
        temp_long = 2 * M_PI + longitude;


    a = 6377397.15508;
    b = 6356078.96290;
    c = sqr(a) / b;
    e2= (sqr(a) - sqr(b)) / sqr(b);

    /*  Abgeleitete Konstanten, s. /SCH81/, A.3.9, S.14
        Q : meridianbogenlaenge vom Aequator bis zum Pol (meridianviertelkreis)
    */
    Q   =  c * (1. -3./4. * e2 * (1. -15./16. * e2 * (1. -35./36. * e2 * (1.-63./64. * e2))));
    ef2  =  - c * 3./8. * e2 * (1. -5./4. * e2 * (1. -35./32. * e2 * (1.-21./20. *e2)));
    ef4  =  c * 15./256. * e2 * e2 * (1. -7./4. * e2 * (1. -21./16. * e2));
    ef6  =  - c * 35./3072. * pow(e2, three) * (1. -9./4. * e2);




    //   G : meridianbogenlAEnge zur gegebenen geographischen Breite latitude
    G = Q * latitude + ef2 * sin(2. * latitude) + ef4 * sin(4. * latitude) +
        ef6 * sin(6. * latitude);
    c2  =  sqr(cos(latitude));
    t2  =  sqr( tan(latitude));
    eta2  =  e2*c2;

    //c  N : Querkruemmungsradius, s. /SCH81/, A.3.6, S.11
    N  =  c / sqrt(1. + eta2);
    dL  =  temp_long * 180. / M_PI - gk_meridian;
    cL  =  sqr( (dL * M_PI / 180.) ) * c2/2.;
    y  =  M_PI / 180. * N * cos(latitude) * dL * (1. +cL/3. * (1. - t2 + eta2 + cL/10. * (5. - t2 * (18. - t2))));

    rechts  =  gk_meridian / 3E-6 + 5E+5 + y;
    hoch  =  G + N * tan(latitude) * cL *(1. + cL/6. * (5.- t2 + 9. * eta2));

}

/***********************************************************************************************/
/***************************************************************************
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
***************************************************************************/


void coordinates::gauss2geo() {
    //rechtswert, hochwert, laenge & breite im Bogenmass als ergebnis



    /*
    c  Unterprogramm zur Umrechnung von Gauss-Krueger-Koordinaten (rechts,
    c  hoch) in geographische Koordinaten (latitude, longitude).
    c  Die geographischen Koordinaten latitude und longitude (Breite und Laenge)
    c  werden im Bogenmass berechnet.
    c  Im Wesentlichen geschieht die Berechnung nach SCHOEDLBAUER, Albert,
    c  'Rechenformeln und Rechenbeispiele zur Landesvermessung',
    c      Teil 1, Karlsruhe 1981: /SCH81/
    c  und Teil 2, Karlsruhe 1982: /SCH82/.

      Konstanten  werden im Programm mit extended Precision berechnet
    CONST e2 = 6.719219E-3;
          c = 6.398786849E6;
          Q = 6366742.520261453;
          f2 = 2.5112734435E-3;
          f4 = 3.678654E-6;
          f6 = 7.45559E-9;
    s.o.
    */


    long double  a, b, c, e2, Q, eta2, N, y,  c2, t2,
    f2, f4, f6, lh, sigma, bf, yN;
    long double three = 3.0;


    a = 6377397.15508;
    b = 6356078.96290;
    c = sqr(a) / b;
    e2 = (sqr(a) - sqr(b)) / sqr(b);


    /*
    c  Abgeleitete Konstanten, siehe SCHOEDL., Teil 1, S.15
    c  Q : meridianbogenlaenge vom Aequator bis zum Pol (meridianviertelkreis)
    */
    Q   =  c * (1. -3./ 4. * e2 * (1. -15./ 16. * e2 * (1. - 35./ 36. * e2 * (1. -63./ 64. * e2))));
    f2  =  3E+0 / 8. * e2 * (1.-e2 / 2. * (1. -71./128. * e2));
    f4  =  21./ 256. * e2 * e2 * (1. - e2);
    f6  =  151.0 / 6144.0 * pow(e2, three); //{e2*e2*e2}

    //c  bf: Geographische Breite zur meridianbogenleange, s. /SCH81/, S.14f

    sigma = hoch / Q;
    bf = sigma + f2 * sin(2. * sigma) + f4 * sin(4. * sigma) + f6 * sin(6. * sigma);
    c2 = sqr(cos(bf));
    t2 = sqr(tan(bf));
    eta2 = c2 * e2;

    //c  N : Querkruemmungsradius (/SCH81/, A.3.6, S.11)
    N = c / (sqrt (1. + eta2));
    if (rechts < 0.0001)
        y = 0; // numerical error
    //   else y = rechts - floor(rechts / 1E6) * 1E6 - 5E5; // ! vgl. /SCH82/, B.2.1, S.79}
    // floor rounds up?
    else
        y = rechts - long(rechts / 1E6) * 1E6 - 5E5; // ! vgl. /SCH82/, B.2.1, S.79}
    yN = sqr(y/N)/2.;


    //(c  lh: Bezugsmeridian (in Bogenma�, s. /SCH82/, B.2.1, S.79
    //{     lh  = floor(rechts/1E6) * atanl(1E+0)/15;}
    //   lh  = floor(rechts/1E6) * 3. * M_PI/180.;
    // floor round up?
    lh  = long(rechts/1E6) * 3. * M_PI/180.;


    latitude = bf - yN * tan(bf) * (1. + eta2 - yN/6. * (5. + 3. * t2 + 6. * eta2 * (1. - t2)
                                    - yN * (4. + 3. * t2 * (2. + t2))));
    longitude = lh + y/N/cos(bf) * (1. - yN/3. *
                                    (1. + 2. * t2 + eta2 - yN / 10. * (5. + t2 * (28. + 24. * t2))));


    if (longitude >= M_PI)
        longitude -= (2 * M_PI);
    lon = (longitude * 180.) / M_PI;

    rad2grad(longitude, this->gr_lo, this->mi_lo, this->se_lo);
    rad2grad(latitude, this->gr_la, this->mi_la, this->se_la);
    flip();
}

/***********************************************************************************************/




/*Reference ellipsoids derived from Peter H. Dana's website-
  http://www.utexas.edu/depts/grg/gcraft/notes/datum/elist.html
  Department of Geography, University of Texas at Austin
  Internet: pdana@mail.utexas.edu
  3/22/95

  Source
  Defense Mapping Agency. 1987b. DMA Technical Report:
  Supplement to Department of Defense World Geodetic System
  1984 Technical Report. Part I and II. Washington, DC: Defense Mapping Agency
*/

/***************************************************************************
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
***************************************************************************/

void coordinates::geo2UTM() {
    //converts lat/long to UTM coords.  Equations from USGS Bulletin 1532
    //East Longitudes are positive, West longitudes are negative.
    //North latitudes are positive, South latitudes are negative
    //Lat and Long are in decimal degrees
    //Does not take into account thespecial UTM zones between 0 degrees and
    //36 degrees longitude above 72 degrees latitude and a special zone 32
    //between 56 degrees and 64 degrees north latitude
    //Written by Chuck Gantz- chuck.gantz@globalstar.com

    long double a = ellipsoid[ReferenceEllipsoid].EquatorialRadius;
    long double eccSquared = ellipsoid[ReferenceEllipsoid].eccentricitySquared;
    long double k0 = 0.9996;

    int LongOrigin;
    long double eccPrimeSquared;
    long double N, T, C, A, M;

    long double LatRad = latitude;
    long double LongRad = longitude;
    long double LongOriginRad;

    if(lon > -6 && lon < 0)
        LongOrigin = -3;
    else if(lon < 6 && lon >= 0)
        LongOrigin = 3; //arbitrarily set origin at 0 longitude to 3W
    else if (lon == -180 || lon == 180)
        LongOrigin = -177;   // also forces at 180 deg to 177 W
    else
        LongOrigin = int(lon/6) *  6 + 3 * int(lon/6) /  abs(int(lon / 6));

    // if meridian was defined
    if (this->utm_meridian != INT_MAX)
        LongOrigin = this->utm_meridian;
    else
        this->utm_meridian = LongOrigin;
    // set also Gauss Krueger meridian if not defined before
    if (gk_meridian == INT_MAX)
        gk_meridian = this->utm_meridian;
    if (gk_meridian < 0 )
        gk_meridian += 360;

    LongOriginRad = LongOrigin * M_PI / 180.;
    //  char* utmzone;
    //compute the UTM Zone from the latitude and longitude
    //	sprintf(UTMzone, "%d%c", int((lon + 180)/6) + 1, UTMLetterDesignator());

    long ut = long(((lon + 180)/6) + 1);
    UTMzone = num2str(ut);
    UTMzone += UTMLetterDesignator();


    eccPrimeSquared = (eccSquared)/(1-eccSquared);

    N = a / sqrt(1. - eccSquared * sin(LatRad) * sin(LatRad));
    T = tan(LatRad) * tan(LatRad);
    C = eccPrimeSquared * cos(LatRad) * cos(LatRad);
    A = cos(LatRad) * (LongRad - LongOriginRad);

    M = a * ((1.	- eccSquared/4. - 3. * eccSquared*eccSquared/64.
              - 5. * eccSquared * eccSquared * eccSquared/256.) * LatRad
             - (3. * eccSquared/8. + 3. * eccSquared * eccSquared/32.
                + 45. * eccSquared * eccSquared*eccSquared / 1024.) * sin(2. * LatRad)
             + (15. * eccSquared * eccSquared/256. + 45. * eccSquared * eccSquared * eccSquared/1024.)
             * sin(4. * LatRad)
             - (35. * eccSquared * eccSquared * eccSquared/3072.) * sin(6. * LatRad));

    easting = (long double)(k0 * N * (A + (1 - T + C) * A * A * A/6.
                                      + (5. - 18. * T + T * T + 72. * C - 58. * eccPrimeSquared) * A * A * A * A * A/120.)
                            + 500000.0);

    northing = (long double)(k0 * (M + N * tan(LatRad) * (A * A/2. + (5. -T +9. * C + 4. * C *C )* A * A * A *A/24.
                                   + (61.- 58. * T + T * T + 600. * C - 330. * eccPrimeSquared) * A * A * A * A * A * A/720.)));

    if(lat < 0)
        northing += 10000000.0; //10000000 meter offset for southern hemisphere
}



/***************************************************************************
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
***************************************************************************/



char coordinates::UTMLetterDesignator() {
    //This routine determines the correct UTM letter designator for the given latitude
    //returns 'Z' if latitude is outside the UTM limits of 80N to 80S
    //Written by Chuck Gantz- chuck.gantz@globalstar.com
    char LetterDesignator;

    if((80 >= lat) && (lat > 72))
        LetterDesignator = 'X';
    else if((72 >= lat) && (lat > 64))
        LetterDesignator = 'W';
    else if((64 >= lat) && (lat > 56))
        LetterDesignator = 'V';
    else if((56 >= lat) && (lat > 48))
        LetterDesignator = 'U';
    else if((48 >= lat) && (lat > 40))
        LetterDesignator = 'T';
    else if((40 >= lat) && (lat > 32))
        LetterDesignator = 'S';
    else if((32 >= lat) && (lat > 24))
        LetterDesignator = 'R';
    else if((24 >= lat) && (lat > 16))
        LetterDesignator = 'Q';
    else if((16 >= lat) && (lat > 8))
        LetterDesignator = 'P';
    else if(( 8 >= lat) && (lat >= 0))
        LetterDesignator = 'N';
    else if(( 0 >= lat) && (lat > -8))
        LetterDesignator = 'M';
    else if((-8>= lat) && (lat > -16))
        LetterDesignator = 'L';
    else if((-16 >= lat) && (lat > -24))
        LetterDesignator = 'K';
    else if((-24 >= lat) && (lat > -32))
        LetterDesignator = 'J';
    else if((-32 >= lat) && (lat > -40))
        LetterDesignator = 'H';
    else if((-40 >= lat) && (lat > -48))
        LetterDesignator = 'G';
    else if((-48 >= lat) && (lat > -56))
        LetterDesignator = 'F';
    else if((-56 >= lat) && (lat > -64))
        LetterDesignator = 'E';
    else if((-64 >= lat) && (lat > -72))
        LetterDesignator = 'D';
    else if((-72 >= lat) && (lat > -80))
        LetterDesignator = 'C';
    else
        LetterDesignator = 'Z'; //This is here as an error flag to show that the Latitude is outside the UTM limits

    return LetterDesignator;
}

/***************************************************************************
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
***************************************************************************/


void coordinates::UTM2geo() {
    //converts UTM coords to lat/long.  Equations from USGS Bulletin 1532
    //East Longitudes are positive, West longitudes are negative.
    //North latitudes are positive, South latitudes are negative
    //Lat and Long are in decimal degrees.
    //Does not take into account the special UTM zones between 0 degrees
    //and 36 degrees longitude above 72 degrees latitude and a special
    //zone 32 between 56 degrees and 64 degrees north latitude
    //Written by Chuck Gantz- chuck.gantz@globalstar.com

    long double k0 = 0.9996;
    long double a = ellipsoid[ReferenceEllipsoid].EquatorialRadius;
    long double eccSquared = ellipsoid[ReferenceEllipsoid].eccentricitySquared;
    long double eccPrimeSquared;
    long double e1 = (1-sqrt(1-eccSquared))/(1+sqrt(1-eccSquared));
    long double N1, T1, C1, R1, D, M;
    long double LongOrigin;
    long double mu, phi1, phi1Rad;
    long double x, y;
    long double one_point_five = 1.5;
    int ZoneNumber;
    char* ZoneLetter;
    int NorthernHemisphere; //1 for northern hemispher, 0 for southern

    x = easting - 500000.0; //remove 500,000 meter offset for longitude
    y = northing;

    //	ZoneNumber = strtoul(UTMzone.c_str(), &ZoneLetter, 10);
    ZoneNumber = atoi(UTMzone.c_str());
    ZoneLetter = &UTMzone[UTMzone.size()-1];

    //	cerr << ZoneNumber << " " << ZoneLetter << endl;
    if((*ZoneLetter - 'N') >= 0)
        NorthernHemisphere = 1; //point is in northern hemisphere

    else {
        NorthernHemisphere = 0;	//point is in southern hemisphere
        y -= 10000000.0;				//remove 10,000,000 meter offset used for southern hemisphere
    }

    LongOrigin = ((ZoneNumber - 1) * 6 - 180) + 3;  // +3 puts origin in middle of zone

    eccPrimeSquared = (eccSquared)/(1. - eccSquared);

    M = y / k0;
    mu = M / (a * (1.- eccSquared/4. -3. * eccSquared*eccSquared/64.-
                   5 * eccSquared * eccSquared * eccSquared/256.));

    phi1Rad = mu	+ (3. * e1/2. -27. *e1 * e1 * e1/32.)*sin(2. * mu)
              + (21. * e1 * e1/16. - 55. * e1 * e1 * e1 * e1/32.)*sin(4. * mu)
              +(151. * e1 * e1 * e1/96.)*sin(6. * mu);
    phi1 = phi1Rad * 180. / M_PI;

    N1 = a / sqrt(1. - eccSquared * sin(phi1Rad) * sin(phi1Rad));
    T1 = tan(phi1Rad) * tan(phi1Rad);
    C1 = eccPrimeSquared * cos(phi1Rad) * cos(phi1Rad);
    R1 = a * (1. - eccSquared) / pow((1. - eccSquared * sin(phi1Rad) * sin(phi1Rad)), one_point_five);
    D = x/(N1 * k0);

    latitude = phi1Rad - (N1*tan(phi1Rad)/R1)
               * (D * D/2. - (5. + 3. * T1 + 10. * C1 - 4. * C1 * C1 - 9. * eccPrimeSquared)
                  * D * D * D * D/24.
                  + (61. + 90. * T1 + 298. * C1 + 45.* T1 * T1 - 252. * eccPrimeSquared - 3. * C1 * C1)
                  * D * D * D * D * D * D/720.);
    lat = latitude * 180. / M_PI;

    longitude = (D - ( 1.+ 2. * T1 + C1) * D * D * D/6
                 + (5. - 2. * C1 + 28. * T1 - 3. * C1 * C1 + 8. *eccPrimeSquared + 24. * T1 * T1)
                 * D * D * D * D * D/120.)/ cos(phi1Rad);

    lon = LongOrigin + longitude * 180. / M_PI;
    longitude = (lon * M_PI) / 180.;

    rad2grad(longitude, this->gr_lo, this->mi_lo, this->se_lo);
    rad2grad(latitude, this->gr_la, this->mi_la, this->se_la);
    flip();

}


/*!
    \fn coordinates::get_lat_lon(int& lag, int& lam, double& las, int& log, int& lom, double &los)
*/
void coordinates::get_lat_lon(int& lag, int& lam, double& las, string& LatH, int& log, int& lom, double &los, string& LonH) const {
    lag = this->gr_la;
    lam = this->mi_la;
    las = this->se_la;

    log = this->gr_lo;
    lom = this->mi_lo;
    los = this->se_lo;
    LatH = this->LatH;
    LonH = this->LonH;
   
}

void coordinates::get_lat_lon(string& slat, string& slon) {

    stringstream sslat, sslon;

    sslat <<  this->gr_la << ":" << this->mi_la << ":" << this->se_la;
    sslat >> slat;
    sslon <<  this->gr_lo << ":" << this->mi_lo << ":" << this->se_lo;
    sslon >> slon;


}


void coordinates::get_lat_lon_h_m(string& slat, string& slon, const int& add_min) {

    stringstream sslat, sslon;

    sslat <<  this->gr_la << ":" << this->mi_la + add_min << ":00.0";
    sslat >> slat;
    sslon <<  this->gr_lo << ":" << this->mi_lo + add_min << ":00.0";
    sslon >> slon;


}


void coordinates::get_NS_EW(string& NS, string& EW) const {

  NS = this->LatH;
  EW = this->LonH;
  
}



/*!
    \fn coordinates::operator -= (const coordinates& cdn)
 */
coordinates& coordinates::operator -= (const coordinates& cdn)
{

    this->latitude  -= cdn.latitude;
    this->longitude -= cdn.longitude;
    this->elevation -= cdn.elevation;

    getlatlon(this->latitude, this->longitude);

    if (this->utm_meridian) {
     coordinates::geo2UTM();

    }
    if (this->gk_meridian) {
     coordinates::geo2gauss();

    }

    return *this;
}

/*!
    \fn coordinates::operator += (const coordinates& cdn)
 */
coordinates& coordinates::operator += (const coordinates& cdn)
{

    this->latitude  += cdn.latitude;
    this->longitude += cdn.longitude;
    this->elevation += cdn.elevation;

    getlatlon(this->latitude, this->longitude);

    if (this->utm_meridian) {
     coordinates::geo2UTM();

    }
    if (this->gk_meridian) {
     coordinates::geo2gauss();

    }
    return *this;
}

/*!
    \fn coordinates::operator = (const coordinates& cdn)
 */
coordinates& coordinates::operator = (const coordinates& cdn)
{

    this->latitude   = cdn.latitude;
    this->longitude  = cdn.longitude;

    this->lat        = cdn.lat;
    this->lon        = cdn.lon;

    this->elevation  = cdn.elevation;

    this->rechts     = cdn.rechts;
    this->hoch       = cdn.hoch;
    this->northing   = cdn.northing;
    this->easting    = cdn.easting;

    this->ReferenceEllipsoid = cdn.ReferenceEllipsoid;
    this->gk_meridian  = cdn.gk_meridian;
    this->utm_meridian = cdn.utm_meridian;

    this->my_name_is = cdn.my_name_is;
    this->UTMzone    = cdn.UTMzone;

    this->LatH       = cdn.LatH;
    this->LonH       = cdn.LonH;

    this->gr_lo      = cdn.gr_lo;
    this->gr_la      = cdn.gr_la;
    this->mi_lo      = cdn.mi_lo;
    this->mi_la      = cdn.mi_la;
    this->se_lo      = cdn.se_lo;
    this->se_la      = cdn.se_la;


    return *this;
}

