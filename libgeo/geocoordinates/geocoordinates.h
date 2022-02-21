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

#ifndef GEOCOORDINATES_H
#define GEOCOORDINATES_H



// MSVC only supports c++11 behaviour for fabs() with integral types from 2015 upwards
//  otherwise you'll get a compilation error when using something like "fabs(-1);"
#ifdef _MSC_VER
    #if _MSC_VER >= 1900
        #define _CAN_INT_FABS
    #endif
#else // for now, assume all other compilers support integral fabs
    #define _CAN_INT_FABS
#endif

#include <QObject>
#include <QDebug>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QHash>

#include <climits>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <algorithm>

template <class T, class S> void rad2grad(const T& rad, S& gr, S& mi, T& se) {
  T h1, diff = 1.E-4;

  gr = int( (rad * 180.) /M_PI);      // INT SHOULD not ROUND UP ... 2.6 -> 2
  h1 = rad * 180./M_PI;
  h1 -= gr;                              // rest 0.6
  mi = int (h1 * 60);                 // we should get the fraction here
  se = h1 * 3600 - mi * 60;


      // round up if we get 12:59:59.99999 to 13:00:00
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
  ////--qDebug() << "templ" << gr << mi << (double)se;
}

template <class T, class S> void deg2QString(const S& gr, const S& mi, const T& se, QString& qsdeg) {
  double setmp = (double)se;

  qsdeg = QString::number(gr) + ":";
  if (mi < (S)10) qsdeg += "0";
  qsdeg += QString::number(mi) + ":";
  if (setmp < 10.) qsdeg += "0";
  qsdeg += QString::number(setmp);

}

template <class T, class S> void deg2QStringGoogle(const S& gr, const S& mi, const T& se, QString& qsdeg) {
  double setmp = (double)se;

  qsdeg = QString::number(gr) + QString::fromUtf8("° ");
  if (mi < (S)10) qsdeg += "0";
  qsdeg += QString::number(mi) + "' ";
  if (setmp < 10.) qsdeg += "0";
  qsdeg += QString::number(setmp) + "''";


}

template <class T, class S> T degrees2rad(const S& gr, const S& mi, const T& se) {
  T deg = gr + mi/60. + se/3600.;
  T rad = (deg * M_PI) / 180.;
  return rad;
}





/**
     @author B. Friedrichs <bfr@mtx>
     internal calculation will use -180 to 180 deg lon
                                  and -90 to 90 deg lat

    Routines for coordinate transformation <br>
    (first version K. Ivory)<br>
    getestet:<br>
    Longitude        Latitude         Meridian<br>
    10 42' 59.3215'' 48 26' 45.4355''  12<br>
    rechts: 4.405057628849e+06  hoch: 5.368263247964e+06<br>
    Ergebinis 25.6.97<br>

    Longitude         Latitude<br>
    10 31' 55.9''  52 15' 39.5''<br>
    Soll 4399796.57      5792799.64 rechts hoch<br>
    Ist  4399796.266583  5792800.787294<br>

Braunschweig St. Andreas Curch, tower top middle
"52:16:09.4416", "10:31:15.8414"

Gauss Krueger:  Rechts 3 603820.13   Hoch  5 793801.08   Meridian 9
                       4 399055.56         5 793741.52            12
UTM           Northing 5792079     Easting 603791    Zone 32U   Meridian 9

Loewenwall Braunschweig
52:15:39.5, 10:31:55.9
Gauss Krueger:  Rechts 4 399796.57   Hoch  5 792799.64   Meridian 12
WGS-80 Lat Lon 52:15:34 10:31:53


CN Tower Toronto:
43:38:33.24N 79:23:13.7W. Zone 17
             4833438    630084 from Wikipedia
UTM   17 "T" 4832413    630104  my software with -81 -81 Meridian
             4832409    630107  (internet tool)


The internal data is RADIANS longitude and latitude; all input must be converted into that.
From these longitude and latitude degrees and UTM are calculated.

 */

/*!
 * \brief The geocoordinates class calculates geographical coordinates ; LONG DOUBLE is used;
 */
class  geocoordinates : public QObject{


    Q_OBJECT
public:
    geocoordinates(QObject *parent = 0);
    geocoordinates(const geocoordinates &ma, QObject *parent = 0);

    ~geocoordinates();

    geocoordinates& operator =  ( const geocoordinates&);



public slots:

    /*!
      clear and re-set all variables, set init to false
    */
    void clear();

    /*!
     * \brief get_lat_rad one of the 3 base vars WGS-84 assumed
     * \return
     */
    long double get_lat_rad() const;

    /*!
     * \brief get_lon_rad one of the 3 base vars WGS-84 assumed
     * \return
     */
    long double get_lon_rad() const;

    /*!
     * \brief get_elevation one of the 3 base vars WGS-84 assumed
     * \return
     */
    long double get_elevation() const;

    /*!
     * \brief coordinates returns the coordinates as base values - best
     * \param radlat
     * \param radlon
     * \param m_elev
     */
    void coordinates(long double &radlat, long double &radlon, long double &m_elev) const;


    /*!
     * \brief distance on earth to other coordinates in m
     * \param radlon from other point
     * \param radlat from other point
     * \return
     */
    double distance(const long double &radlon, const long double &radlat) const;

    /*!
     * \brief distance to other geocoordinates
     * \param other
     * \return
     */
    double distance(const geocoordinates &other) const;



    /*!
      this is THE function to setup the geocoordinates; all other functions will sooner or later call this this function
      from the radians value all other values will be re-calculated;
      the long double is needed for interchange with other coordinates<br>
      THIS IS THE BEST MASTER and will also used internally form other ser routines

    */
    void set_lat_lon_rad(const long double &rad_lat, const long double &rad_lon, const long double &elevation = 0, const int &utm_meridian = INT_MAX, const int &gk_meridian = INT_MAX);


    /*!
     * \brief set_lat_lon set lat long from numbers. 42 15 0 S is assumed as -42 -15 -0
     * \param lag latitude grad
     * \param lam latitude minute
     * \param las latitude seconds
     * \param LatH designator such as "N" or "S" can be empty, "X" or so whatever in case you don't know; if "S" coordinates will be multiplied by -1 internally
     * \param log longitute grad
     * \param lom longitude minute
     * \param los longtitude seconds
     * \param LonH designator such as "W" or "E" - can be empty "X" or so whatever in case you don't know; if "W" coordinates will be multiplied by -1 internally
     * \param televation elevation in m
     * \param tutm_meridian UTM merdian (skip of not know, will be calculated)
     * \param tgk_meridian Gauss Krueger meridian (skip of not know, will be calculated)
     * \param min_sec_positive_only if the format is -42 15 13 make -42 -15 -13 out of it -> -42 15 is somewhat like -41 -45
     */
    void set_lat_lon(  const int &lag,   const int &lam,   const long double &las,   const QString &LatH,
                       const int &log,   const int &lom,   const long double &los,   const QString &LonH,
                       const long double &elevation = 0,   const int &utm_meridian = INT_MAX,   const int &gk_meridian = INT_MAX,  const bool &min_sec_positive_only = false);

    /*!
     * \brief get_lon_lat get latitude and longitude as numbers
     * \param log grad longitude
     * \param lom minute longitude
     * \param los seconds longitude
     * \param LonH designtor "W" or "E"
     * \param lag grad latitude
     * \param lam minute latitude
     * \param las seconds latitude
     * \param LatH designator "N" or "S"
     * \param televation elevation m
     * \param tutm_meridian UTM Meridian
     * \param tgk_meridian Gauss Krueger Meridian
     */

    void get_lat_lon(  int& lag,   int& lam,   long double& las,   QString& LatH,
                       int& log,   int& lom,   long double& los,   QString& LonH,
                       long double& elevation,   int& utm_meridian,   int& gk_meridian) const;






    /*!
     * \brief set_lon_lat as above as STRING version; here you MUST specify "N"/"S" and "W"/"E" and ALL strings positive as 42 15 S for southern hemisphere
     * \param log grad longitude
     * \param lom minute longitude
     * \param los seconds longitude
     * \param LonH designtor "W" or "E"
     * \param lag grad latitude
     * \param lam minute latitude
     * \param las seconds latitude
     * \param LatH designator "N" or "S"
     */
    void set_lat_lon_strs( const QString &lag,  const QString &lam,  const QString &las,  const QString &LatH,
                      const QString &log,  const QString &lom,  const QString &los,  const QString &LonH,
                      const QString &elevation = "");


    bool fetch_arguments(QStringList& qargs );

    /*!
     * \brief set_lon_lat_msec_qstr some GPS provide latitude longitude as milli seconds and cm
     * \param qslon
     * \param qslat
     * \param qs_m_elevation
     * \return
     */
    QString set_lat_lon_msec_qstr(const QString &qslat, const QString &qslon, const QString &qs_m_elevation);

    /*!
     * \brief set_lon_lat_elev_qstr 10:23:12.1 52:44:33.1 129 - short version of \ref geocoordinates::set_lon_lat
     * \param qslon
     * \param qslat
     * \param qs_meter_elevation
     */
    void set_lat_lon_elev_qstr( const QString qslat, const QString qslon, const QString qs_meter_elevation);


    /*!
     * \brief set_lat_lon_msec set longitude and latitude as milliseconds - as some GPS modules deliver
     * \param msec_lat
     * \param msec_lon
     * \param m_elevation
     */
    void set_lat_lon_msec(const int &msec_lat,  const int &msec_lon, const long double &m_elevation);

    /*!
     * \brief get_lon_lat_msec get longitude and latitude as milli seconds and elevation as m - as some GPS modules deliver
     * \param msec_lon
     * \param msec_lat
     * \param m_elevation
     */
    void get_lat_lon_msec(int& msec_lat, int& msec_lon, long double &m_elevation) const;


    /*!
     * \brief get_lon_lat_str returns in the form of "N 52:44:33.1 E 10:23:12.1"
     * \param qslon set to 10:23:12.1 (for example)
     * \param qslat set to 52:44:33.1 (for example)
     * \return "N 52:44:33.1 E 10:23:12.1" (for example)
     */
    QString get_lat_lon_str( QString& qslat, QString& qslon) const;

    /*!
     * \brief get_lat_lon_str_EDI get_lon_lat_str returns in the form of "52:44:33.1 -10:23:12.1" like above BUT with sign
     * \param qslat
     * \param qslon
     */
    void get_lat_lon_str_EDI( QString& qslat, QString& qslon) const;

    /*!
     * \brief get_lon_lat_str_google_maps returns a string readable by google maps
     * \return N 40° 04' 53.928'' E 98° 48' 54.876''   (for example)
     */
    QString get_lat_lon_str_google_maps() const;



    /*!
     * \brief get_easting_northing
     * \param merdian
     * \return
     */
    QString get_easting_northing() const;

    int get_northing_meter() const;
    int get_easting_meter() const;
    long double get_northing() const;
    long double get_easting() const;


    /*!
     * \brief set_utm
     * \param northing
     * \param easting
     * \param UTM_ZONE like 32U
     * \param recalc_lat_lon
     */

    void set_utm(const double &northing, const long double &easting, const QString UTM_ZONE, const bool recalc_lat_lon);

    void get_UTM_ZONE(long double &northing, long double &easting, QString &zone) const;


    QString get_elevation_str(const bool append_unit = true) const;
    QString get_utm_meridian_str(const bool append_unit = true) const;
    QString get_utm_zone(const bool append_unit = true) const;
    QString get_gk_meridian_str(const bool append_unit =true ) const;

    QChar UTMLetterDesignator();


    /*!
     * \brief geo2UTM ATTENTION: calculates explicitly UTM coordinates, if tutm_meridian is NOT SET TO INT_MAX the function tries to calculate with the given meridian; UTM coordinates are automatically calculated when coordinates are set with auto merdian
     * \param easting
     * \param northing
     * \param utm_meridian set INT_MAX for auto calculation, otherwise 3, 9, 15 ....
     * \param UTMzone
     * \return
     */
    QString geo2UTM(long double &northing,  long double &easting, int& utm_meridian, QString& UTMzone);

    /*!
     * \brief geo2gauss calculates the Gauss Krueger coordinates; if meridian is NOT GIVEN the UTM meridian will be used; merdian can be 0,3,6,9 ...357, UTM can be 3, 9, 15 ....
     * \param rechts
     * \param hoch
     * \param gk_meridian set INT_MAX if you want auto, set 0,3,6,9 .... for explicit
     * \return
     */
    QString geo2gauss (double& rechts, double& hoch, int& gk_meridian);

    void gauss2geo();
    void UTM2geo() ;

    friend bool complat(const geocoordinates& lhs, const geocoordinates& rhs);
    friend bool complon(const geocoordinates& lhs, const geocoordinates& rhs);
    friend bool compelev(const geocoordinates& lhs, const geocoordinates& rhs);



//signals:
//    void get_lon_lat_qstr(QString& QSLongitude, QString& QSLatitude);
//    void utm_calculated(double& teasting, double& tnorthing,  int& tutm_meridian, QString& tUTMzone);

private:

    bool is_init = false;
    /*!
     * \brief flips the degrees (not radians) and sets to W and S in case
     */
    void flip();

    /*!
     * \brief geo2UTM_internal calculates the UTM coordinates
     * \param utm_meridian
     */
    void geo2UTM_internal(const int utm_meridian = INT_MAX);

    /*!
     * \brief set_internals calls clear and initializes all variables;
     */
    void set_internals();

    long double latitude;      /*! latitude  in RAD   this is the base of all*/
    long double longitude;     /*! longitude in RAD   this is the base of all*/
    long double deglat;        /*! latitude  in DEG - can hold data as above BUT with sign reversal if "S" is LatH */
    long double deglon;        /*! longitude in DEG - can hold data as above BUT with sign reversal if "W" is LatH */
    long double rechts;        /*! Gauss Krueger */
    long double hoch;          /*! Gauss Krueger */
    long double northing;      /*! UTM coordianate */
    long double easting;       /*! UTM coordinates */
    long double elevation;     /*! elevation sea (NN) in meter - eg. WGS84*/

    int deg_lo;                /*! readable coodinates, grad */
    int mi_lo;                 /*! readable coodinates, grad */
    int deg_la;                /*! readable coodinates, grad */
    int mi_la;                 /*! readable coodinates, grad */

    long double se_lo;         /*! readable seconds longitude DEG */
    long double se_la;         /*! readable seconds longitude DEG */

    int gk_meridian;           /*! meridian for Gauss Krueger  in DEG */
    int utm_meridian;          /*! meridian for UTM   in DEG */

    QString UTMzone;           /*! UTM zone */
    QString  LatH;             /*! N or S designator */
    QString  LonH;             /*! E or W designator */
    QString ellipsoid;         /*! store the Ellispoid name like WGS-84 */
    QString QSLatitude;        /*! temp */
    QString QSLongitude;       /*! temp */
    QHash<QString, long double> EquatorialRadius;
    QHash<QString, long double> EccentricitySquared;
    long double eqradius;      /*! used Equatorial Radius */
    long double eccSquared;    /*! used Eccentricity Squared */


    QString myname;            /*!  use for debug and other messages */
    QString myfunc;            /*!  use for debug and other messages */



};

bool complat(const geocoordinates &lhs, const geocoordinates &rhs);

bool complon(const geocoordinates &lhs, const geocoordinates &rhs);

bool compelev(const geocoordinates &lhs, const geocoordinates &rhs);




#endif // GEOCOORDINATES_H

/*

/// @brief The usual PI/180 constant
static const double DEG_TO_RAD = 0.017453292519943295769236907684886;
/// @brief Earth's quatratic mean radius for WGS-84
static const double EARTH_RADIUS_IN_METERS = 6372797.560856;

** @brief Computes the arc, in radian, between two WGS-84 positions.
  *
  * The result is equal to <code>Distance(from,to)/EARTH_RADIUS_IN_METERS</code>
  *    <code>= 2*asin(sqrt(h(d/EARTH_RADIUS_IN_METERS )))</code>
  *
  * where:<ul>
  *    <li>d is the distance in meters between 'from' and 'to' positions.</li>
  *    <li>h is the haversine function: <code>h(x)=sin²(x/2)</code></li>
  * </ul>
  *
  * The haversine formula gives:
  *    <code>h(d/R) = h(from.lat-to.lat)+h(from.lon-to.lon)+cos(from.lat)*cos(to.lat)</code>
  *
  * @sa http://en.wikipedia.org/wiki/Law_of_haversines


*
double ArcInRadians(const Position& from, const Position& to) {
    double latitudeArc  = (from.lat - to.lat) * DEG_TO_RAD;
    double longitudeArc = (from.lon - to.lon) * DEG_TO_RAD;
    double latitudeH = sin(latitudeArc * 0.5);
    latitudeH *= latitudeH;
    double lontitudeH = sin(longitudeArc * 0.5);
    lontitudeH *= lontitudeH;
    double tmp = cos(from.lat*DEG_TO_RAD) * cos(to.lat*DEG_TO_RAD);
    return 2.0 * asin(sqrt(latitudeH + tmp*lontitudeH));
}

* @brief Computes the distance, in meters, between two WGS-84 positions.
  *
  * The result is equal to <code>EARTH_RADIUS_IN_METERS*ArcInRadians(from,to)</code>
  *
  * @sa ArcInRadians




double DistanceInMeters(const Position& from, const Position& to) {
    return EARTH_RADIUS_IN_METERS*ArcInRadians(from, to);
}

Approximate distance in miles:

    sqrt(x * x + y * y)

where x = 69.1 * (lat2 - lat1)
and y = 53.0 * (lon2 - lon1)

You can improve the accuracy of this approximate distance calculation by adding the cosine math function:

Improved approximate distance in miles:

    sqrt(x * x + y * y)

where x = 69.1 * (lat2 - lat1)
and y = 69.1 * (lon2 - lon1) * cos(lat1/57.3)

If you need greater accuracy, you can use the Great Circle Distance Formula. This formula requires use of spherical geometry and a high level of floating point mathematical accuracy - about 15 digits of accuracy (sometimes called "double-precision"). In order to use this formula properly make sure your software application or programming language is capable of double-precision floating point calculations. In addition, the trig math functions used in this formula require conversion of the latitude and longitude values from decimal degrees to radians. To convert latitude or longitude from decimal degrees to radians, divide the latitude and longitude values in this database by 180/pi, or approximately 57.29577951. The radius of the Earth is assumed to be 6,378.8 kilometers, or 3,963.0 miles.

If you convert all latitude and longitude values in the database to radians before the calculation, use this equation:

Great Circle Distance Formula using radians:

    3963.0 * arccos[sin(lat1) *  sin(lat2) + cos(lat1) * cos(lat2) * cos(lon2 - lon1)]

If you do NOT first convert the latitude and longitude values in the database to radians, you must include the degrees-to-radians conversion in the calculation. Substituting degrees for radians, the formula becomes:

Great Circle Distance Formula using decimal degrees:

    3963.0 * arccos[sin(lat1/57.2958) * sin(lat2/57.2958) + cos(lat1/57.2958) * cos(lat2/57.2958) *  cos(lon2/57.2958 -lon1/57.2958)]

OR

    r * acos[sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(lon2 - lon1)]

Where r is the radius of the earth in whatever units you desire.
r=3437.74677 (nautical miles)
r=6378.7 (kilometers)
r=3963.0 (statute miles)

If the software application or programming language you are using has no arccosine function, you can calculate the same result using the arctangent function, which most applications and languages do support. Use the following equation:

    3963.0 * arctan[sqrt(1-x^2)/x]

where

    x = [sin(lat1/57.2958) * sin(lat2/57.2958)] + [cos(lat1/57.2958) * cos(lat2/57.2958) * cos(lon2/57.2958 - lon1/57.2958)]




*/


