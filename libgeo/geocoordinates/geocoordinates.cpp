#include "geocoordinates.h"

#ifdef ANDROID
  #define sinl sin
  #define tanl tan
  #define cosl cos
  #define atanl atan
#endif

geocoordinates::geocoordinates ( QObject *parent ) : QObject(parent)
{

    this->set_internals();
    this->is_init = false;
}


geocoordinates::geocoordinates(const geocoordinates &ma, QObject *parent) : QObject(parent) {

    this->clear();
    this->set_internals();
    this->set_lat_lon_rad (ma.latitude, ma.longitude, ma.elevation,  ma.utm_meridian,  ma.gk_meridian);


}

geocoordinates &geocoordinates::operator = (const geocoordinates& ma) {

    if (this == &ma) return *this;
    this->clear();
    this->set_internals();
    this->set_lat_lon_rad (ma.latitude,  ma.longitude,  ma.elevation,  ma.utm_meridian,  ma.gk_meridian);
    return *this;
}

void geocoordinates::clear()
{
    this->latitude = this->longitude = this->rechts = this->hoch = this->deglat = this->deglon = 0.0L;
    this->northing = this->easting = 0.0L;
    this->deg_lo = this->mi_lo = this->deg_la = this->mi_la = 0;
    this->se_lo =  this->se_la = 0.0L;
    this->elevation = 0.;

    this->gk_meridian = INT_MAX;
    this->utm_meridian = INT_MAX;


    this->LatH = "empty";
    this->LonH = "empty";
    this->UTMzone = "empty";

    this->is_init = false;

}


long double geocoordinates::get_lat_rad() const {
    return this->latitude;
}
long double geocoordinates::get_lon_rad() const {
    return this->longitude;
}
long double geocoordinates::get_elevation() const {
    return this->elevation;
}

void geocoordinates::coordinates(long double &radlat, long double &radlon, long double &m_elev) const
{
    radlat = this->latitude;
    radlon = this->longitude;
    m_elev = this->elevation;
}

void geocoordinates::get_UTM_ZONE(long double &northing, long double &easting, QString &zone) const
{
    northing = this->northing;
    easting = this->easting;
    zone = this->UTMzone;
}

double geocoordinates::distance(const long double &radlon, const long double &radlat) const
{
    long double r = this->EquatorialRadius.value(this->ellipsoid);
    long double f = 0.5 * this->EccentricitySquared.value(this->ellipsoid);

    long double F = (this->latitude + radlat) / 2.0L;
    long double G = (this->latitude - radlat) / 2.0L;
    long double l = (this->longitude - radlon) / 2.0L;

    long double S = pow(sinl(G), 2.0L) * pow(cosl(l), 2.0L) + pow(cosl(F), 2.0L) *  pow(sinl(l), 2.0L);
    long double C = pow(cosl(G), 2.0L) * pow(cosl(l), 2.0L) + pow(sinl(F), 2.0L) *  pow(sinl(l), 2.0L);

    long double w = atanl(pow((S/C), 0.5L));
    long double D = 2.0L * w * r;


    long double R = pow(((S *C)/w), 0.5L);
    long double H1 = (3.L * R - 1.L) / ( 2.0L * C);
    long double H2 = (3.L * R + 1.L) / ( 2.0L * S);

    long double s = D * ( 1.0L + f * H1 *  pow(sinl(F), 2.0L) * pow(cosl(G), 2.0L) -
                          f * H2 * pow(cosl(F), 2.0L) * pow(sinl(G), 2.0L));

    // we din't use again, so double is ok
    return (double)s;
}

double geocoordinates::distance(const geocoordinates &other) const
{
    return this->distance(other.get_lon_rad(), other.get_lat_rad());
}


void geocoordinates::set_internals() {
    this->clear();

    myname = "geocoorinates";
    this->EquatorialRadius["Airy"]                  = 6377563.0L;
    this->EquatorialRadius["Australian National"]   = 6378160.0L;
    this->EquatorialRadius["Bessel 1841"]           = 6377397.0L;
    this->EquatorialRadius["Bessel 1841 (Nambia)"]  = 6377484.0L;
    this->EquatorialRadius["Clarke 1866"]           = 6378206.0L;
    this->EquatorialRadius["Clarke 1880"]           = 6378249.0L;
    this->EquatorialRadius["Everest"]               = 6377276.0L;
    this->EquatorialRadius["Fischer 1960 (Mercury)"]= 6378166.0L;
    this->EquatorialRadius["Fischer 1968"]          = 6378150.0L;
    this->EquatorialRadius["GRS 1967"]              = 6378160.0L;
    this->EquatorialRadius["GRS 1980"]              = 6378137.0L;
    this->EquatorialRadius["Helmert 1906"]          = 6378200.0L;
    this->EquatorialRadius["Hough"]                 = 6378270.0L;
    this->EquatorialRadius["International"]         = 6378388.0L;
    this->EquatorialRadius["Krassovsky"]            = 6378245.0L;
    this->EquatorialRadius["Modified Airy"]         = 6377340.0L;
    this->EquatorialRadius["Modified Everest"]      = 6377304.0L;
    this->EquatorialRadius["Modified Fischer 1960"] = 6378155.0L;
    this->EquatorialRadius["South American 1969"]   = 6378160.0L;
    this->EquatorialRadius["WGS 60"]                = 6378165.0L;
    this->EquatorialRadius["WGS 66"]                = 6378145.0L;
    this->EquatorialRadius["WGS-72"]                = 6378135.0L;
    this->EquatorialRadius["WGS-84"]                = 6378137.0L;

    this->EccentricitySquared["Airy"]                 = 0.00667054L;
    this->EccentricitySquared["Australian National"]  = 0.006694542L;
    this->EccentricitySquared["Bessel 1841"]          = 0.006674372L;
    this->EccentricitySquared["Bessel 1841 (Nambia)"] = 0.006674372L;
    this->EccentricitySquared["Clarke 1866"]          = 0.006768658L;
    this->EccentricitySquared["Clarke 1880"]          = 0.006803511L;
    this->EccentricitySquared["Everest"]              = 0.006637847L;
    this->EccentricitySquared["Fischer 1960 (Mercury)"]  = 0.006693422L;
    this->EccentricitySquared["Fischer 1968"]         = 0.006693422L;
    this->EccentricitySquared["GRS 1967"]             = 0.006694605L;
    this->EccentricitySquared["GRS 1980"]             = 0.00669438L;
    this->EccentricitySquared["Helmert 1906"]         = 0.006693422L;
    this->EccentricitySquared["Hough"]                = 0.00672267L;
    this->EccentricitySquared["International"]        = 0.00672267L;
    this->EccentricitySquared["Krassovsky"]           = 0.006693422L;
    this->EccentricitySquared["Modified Airy"]        = 0.00667054L;
    this->EccentricitySquared["Modified Everest"]     = 0.006637847L;
    this->EccentricitySquared["Modified Fischer 1960"]= 0.006693422L;
    this->EccentricitySquared["South American 1969"]  = 0.006694542L;
    this->EccentricitySquared["WGS 60"]               = 0.006693422L;
    this->EccentricitySquared["WGS 66"]               = 0.006694542L;
    this->EccentricitySquared["WGS-72"]               = 0.006694318L;
    this->EccentricitySquared["WGS-84"]               = 0.00669438L;

    this->ellipsoid = "WGS-84";

}

geocoordinates::~geocoordinates()
{
    this->is_init = false;
    //--qDebug() << "coordinates deleted";
}




void geocoordinates::set_lat_lon_rad (const long double &rad_lat, const long double &rad_lon, const long double &elevation, const int &utm_meridian, const int &gk_meridian)
{

    this->myfunc = "geocoordinates::set_lat_lon_rad ";
    this->latitude = rad_lat;
    this->deglat = ( this->latitude * 180.L ) / M_PI;

    this->longitude = rad_lon;
    this->deglon = ( this->longitude * 180.L ) / M_PI;


    if ( this->longitude > M_PI )
    {
        this->longitude -= ( 2.L * M_PI );
        this->deglon -=360.L;
        this->LonH = "W";
    }
    else if ( this->longitude < 0 )
        this->LonH = "W";
    else
        this->LonH = "E";

    if ( this->latitude < 0 )
        this->LatH = "S";
    else
        this->LatH = "N";

    rad2grad<long double, int> ( this->latitude, this->deg_la, this->mi_la, this->se_la );
    rad2grad<long double, int> ( this->longitude, this->deg_lo, this->mi_lo, this->se_lo );
    this->flip();


    this->deglat = ( this->latitude * 180.L ) / M_PI;
    this->deglon = ( this->longitude * 180.L ) / M_PI;

    //--qDebug() << ( double ) deglon << ( double ) deglat;

    this->elevation = elevation;


    if ( utm_meridian != INT_MAX )
    {
        if ( ( ( utm_meridian + 3 ) % 6 ) == 0 )
        {
            this->utm_meridian = utm_meridian;
            if ( this->utm_meridian >  180 )
                this->utm_meridian -= 360;
        }
        else {
            //qDebug() << "UTM Merdian using auto calculation";
            this->utm_meridian = INT_MAX;
        }


    }
    if ( this->gk_meridian == INT_MAX ) this->gk_meridian = this->utm_meridian;

    if ( ( gk_meridian % 3 ) == 0 ) {
        this->gk_meridian = gk_meridian;
    }
//    else {
//        qDebug() <<  "Gauss Krueger Merdian usinlg auto calculation" << endl;
//    }
    if ( this->gk_meridian < 0 ) {
        this->gk_meridian += 360;
    }


    //--qDebug() << "class values " << deg_lo << mi_lo << (double)se_lo << " <lo la>  " << deg_la << mi_la << (double)se_la;
    //--qDebug() << LonH << "  " << LatH;

    this->is_init = true;

    this->utm_meridian  = INT_MAX;

    this->geo2UTM_internal();

}

void geocoordinates::flip()
{

    if ( this->latitude < 0 )
    {
        this->deg_la *= -1;
        this->mi_la *= -1;
        this->se_la *= -1.0L;
        this->LatH = "S";

    }

    if ( this->longitude < 0 )
    {
        this->deg_lo *= -1;
        this->mi_lo *= -1;
        this->se_lo *= -1.0L;
        this->LonH = "W";

    }

}

QString geocoordinates::set_lat_lon_msec_qstr (const QString &qslat, const QString &qslon, const QString &qs_m_elevation )
{
    this->latitude = ( ( ( long double ) ( qslat.toLong() ) ) * M_PI ) / ( 3600000.L * 180.L );
    this->longitude = ( ( ( long double ) ( qslon.toLong() ) ) * M_PI ) / ( 3600000.L * 180.L );
    this->elevation = ( ( long double ) ( qs_m_elevation.toLong() ) );
    this->set_lat_lon_rad ( this->latitude, this->longitude, this->elevation, INT_MAX, INT_MAX);
    this->get_lat_lon_str ( this->QSLatitude, this->QSLongitude );
    return this->QSLatitude + "  " + this->QSLongitude;
}

void geocoordinates::set_lat_lon_elev_qstr(const QString qslat, const QString qslon,  const QString qs_meter_elevation)
{

    QStringList lons = qslon.split(":");
    QStringList lats = qslat.split(":");


    if (lats.size() == 3 && lons.size() == 3)
        this->set_lat_lon_strs(lats.at(0), lats.at(1), lats.at(2),  "", lons.at(0), lons.at(1), lons.at(2), "", qs_meter_elevation);
    else {
        qDebug()  << "geocoordinates::set_lon_lat_elev_qstr -> no valid coorindates" << lats << lons;
    }
}


void geocoordinates::get_lat_lon(int& lag,   int& lam,   long double& las,   QString& LatH,
                                 int& log,   int& lom,   long double& los,   QString& LonH,
                                 long double &elevation,   int& utm_meridian,   int& gk_meridian) const {

    log =  this->deg_lo;             /*! readable coodinates, grad */
    lom =  this->mi_lo;              /*! readable coodinates, grad */
    lag =  this->deg_la;             /*! readable coodinates, grad */
    lam =  this->mi_la;              /*! readable coodinates, grad */




    los = this->se_lo;         /*! readable seconds longitude DEG */
    las = this->se_la;         /*! readable seconds longitude DEG */

    LonH = this->LonH;
    LatH = this->LatH;
    elevation = this->elevation;
    utm_meridian = this->utm_meridian;
    gk_meridian = this->gk_meridian;


}


QString geocoordinates::get_lat_lon_str ( QString& qslat, QString& qslon ) const
{

    deg2QString ( this->deg_la, this->mi_la, this->se_la, qslat );
    deg2QString ( this->deg_lo, this->mi_lo, this->se_lo, qslon );
    return this->LatH + " " + qslat + "  " +  this->LonH + " " + qslon;

}

void geocoordinates::get_lat_lon_str_EDI(QString &qslat, QString &qslon) const
{
    deg2QString ( this->deg_la, this->mi_la, this->se_la, qslat );
    deg2QString ( this->deg_lo, this->mi_lo, this->se_lo, qslon );

    if (!LatH.compare("S")) qslat.prepend("-");
    if (!LonH.compare("W")) qslon.prepend("-");
}

QString geocoordinates::get_lat_lon_str_google_maps() const {

    QString mylat, mylon;
    deg2QStringGoogle ( this->deg_la, this->mi_la, this->se_la, mylat );
    deg2QStringGoogle ( this->deg_lo, this->mi_lo, this->se_lo, mylon );

    return this->LatH + " " + mylat + " " + this->LonH + " " + mylon;

}

QString geocoordinates::get_easting_northing() const
{

    return QString::number((double)this->easting, 'E', 12) + " " + QString::number((double)this->northing, 'E', 12);
}

int geocoordinates::get_northing_meter() const {

    return (int) this->northing;

}
int geocoordinates::get_easting_meter() const {

    return (int) this->easting;
}

long double geocoordinates::get_northing() const
{
    return this->northing;
}

long double geocoordinates::get_easting() const
{
    return this->easting;
}

QString geocoordinates::get_elevation_str (const bool append_unit) const
{


    QString  selevation;
    selevation.setNum ( ( double ) this->elevation );

    if (append_unit) return selevation + " m";
    return selevation;

}

QString geocoordinates::get_utm_meridian_str (const bool append_unit ) const
{

    QString sutm_meridian;
    sutm_meridian.setNum ( this->utm_meridian );

    if (append_unit) return sutm_meridian + " UTM Meridian";
    return sutm_meridian;


}

QString geocoordinates::get_utm_zone(const bool append_unit) const
{

    if (append_unit) return this->UTMzone + " UTMZone";
    return this->UTMzone;
}



QString geocoordinates::get_gk_meridian_str (const bool append_unit) const
{

    QString sgk_meridian;
    sgk_meridian.setNum ( this->gk_meridian );

    if (append_unit) return sgk_meridian + " Gauss Krueger Meridian";
    return sgk_meridian;
}


void geocoordinates::set_lat_lon ( const int &lag,   const int &lam,   const long double &las,   const QString &LatH,
                                   const int &log,   const int &lom,   const long double &los,   const QString &LonH,
                                   const long double &elevation ,   const int &utm_meridian,  const int &gk_meridian,
                                   const bool &min_sec_positive_only )
{

    long double rad_lon;
    long double rad_lat;
    if (min_sec_positive_only) {

        if (log < 0) {
            rad_lon = degrees2rad<long double, int> ( log, -1 * lom, -1.L * los );
        }

        if (lag < 0) {

            rad_lat = degrees2rad<long double, int> ( lag, -1 * lam, -1.L * las );
        }


    }
    else {
        rad_lon = degrees2rad<long double, int> ( log, lom, los );
        rad_lat = degrees2rad<long double, int> ( lag, lam, las );
    }
    if ( LonH == "W" ) rad_lon *= -1.L;
    if ( LatH == "S" ) rad_lat *= -1.L;
    this->set_lat_lon_rad ( rad_lat, rad_lon, elevation, utm_meridian, gk_meridian );



}

void geocoordinates::set_lat_lon_strs ( const QString &lag,  const QString &lam,  const QString &las,  const QString &LatH ,
                                        const QString &log,  const QString &lom,  const QString &los,  const QString &LonH,
                                        const QString &elevation)
{

    bool ok = true;
    int errors = 0;
    this->myfunc = this->myname+"::set_lon_lat";
    int tlog = log.toInt ( &ok, 10 );
    if ( !ok ) {qDebug()  << this->myfunc << "can't convert"<< log << " to number"; errors++;}
    ok = true;
    int tlom = lom.toInt ( &ok, 10 );
    if ( !ok ) {qDebug()  << this->myfunc << "can't convert"<< lom << " to number"; errors++;}
    ok = true;
    double  tlos = los.toDouble ( &ok );
    if ( !ok ) {qDebug()  << this->myfunc << "can't convert"<< los << " to number"; errors++;}
    ok = true;
    int tlag = lag.toInt ( &ok, 10 );
    if ( !ok ) {qDebug()  << this->myfunc << "can't convert"<< lag << " to number"; errors++;}
    ok = true;
    int tlam = lam.toInt ( &ok, 10 );
    if ( !ok ) {qDebug()  << this->myfunc << "can't convert"<< lam << " to number"; errors++;}
    ok = true;
    double  tlas = las.toDouble ( &ok );
    if ( !ok ) {qDebug()  << this->myfunc << "can't convert"<< las << " to number"; errors++;}
    ok = true;

    double  telev = 0;
    if (elevation.size()) {
        telev= elevation.toDouble ( &ok );
        if ( !ok ) {qDebug()  << this->myfunc << "can't convert"<< telev << " to number"; errors++;}
        ok = true;
    }


    if ( errors )
    {
        qFatal ( "could not initialize data" );
    }
    long double rad_lon = degrees2rad<long double, int> ( tlog, tlom, (long double)tlos );
    long double rad_lat = degrees2rad<long double, int> ( tlag, tlam, (long double)tlas );
    if ( LonH == "W" ) rad_lon *= -1.L;
    if ( LatH == "S" ) rad_lat *= -1.L;
    this->set_lat_lon_rad ( rad_lat, rad_lon, telev );


}

bool geocoordinates::fetch_arguments (  QStringList& qargs )
{


    this->myfunc = this->myname+"::fetch_arguments";
    int cmdpos;
    quint64 tutm_meridian = INT_MAX;
    quint64 tgk_meridian  = INT_MAX;
    long double televation  = 0.L;
    quint64 tlog = 0;
    quint64 tlom = 0;
    double  tlos = 0.;
    quint64 tlag = 0;
    quint64 tlam = 0;
    double  tlas = 0.;
    long double rad_lon = 0.L;
    long double rad_lat = 0.L;


    if ( ( ( cmdpos = qargs.indexOf ( "-deg" ) ) > 0 ) && ( qargs.size() > cmdpos + 8 ) )
    {
        tlog = qargs[cmdpos+1].toLong ( &this->is_init, 10 );
        if ( !this->is_init ) qDebug()  << this->myfunc << "can't convert"<< qargs[cmdpos+1] << " to number";

        tlom = qargs[cmdpos+2].toLong ( &this->is_init, 10 );
        if ( !this->is_init ) qDebug()  << this->myfunc << "can't convert"<< qargs[cmdpos+2] << " to number";

        tlos = qargs[cmdpos+3].toDouble ( &this->is_init );
        if ( !this->is_init ) qDebug()  << this->myfunc << "can't convert"<< qargs[cmdpos+3] << " to number";

        this->LonH = qargs[cmdpos+4];
        tlag = qargs[cmdpos+5].toLong ( &this->is_init, 10 );
        if ( !this->is_init ) qDebug()  << this->myfunc << "can't convert"<< qargs[cmdpos+4] << " to number";

        tlam = qargs[cmdpos+6].toLong ( &this->is_init, 10 );
        if ( !this->is_init ) qDebug()  << this->myfunc << "can't convert"<< qargs[cmdpos+5] << " to number";

        tlas = qargs[cmdpos+7].toDouble ( &this->is_init );
        if ( !this->is_init ) qDebug()  << this->myfunc << "can't convert"<< qargs[cmdpos+6] << " to number";

        this->LatH = qargs[cmdpos+8];

    }
    else if (( cmdpos = qargs.indexOf ( "-deg" ) ) > 0 ) {
        qFatal ( "fetch_arguments could not initialize data, to few arguments likely" ) ;
        this->is_init = false;
        return this->is_init;
    }

    if ( this->is_init )
    {
        rad_lon = degrees2rad<long double, int> ( tlog, tlom, (long double)tlos );
        rad_lat = degrees2rad<long double, int> ( tlag, tlam, (long double)tlas );
        if ( this->LonH == "W" ) rad_lon *= -1.L;
        if ( this->LatH == "S" ) rad_lat *= -1.L;
    }


    if ( ( cmdpos = qargs.indexOf ( "-utm_meridian" ) ) > 0 )
    {

        tutm_meridian= qargs[cmdpos+1].toLong ( &this->is_init, 10 );
        if ( !this->is_init ) qDebug()  << this->myfunc << "can't convert"<< qargs[cmdpos+1] << " to number";

    }

    if ( ( cmdpos = qargs.indexOf ( "-gk_meridian" ) ) > 0 )
    {
        tgk_meridian = qargs[cmdpos+1].toLong ( &this->is_init, 10 );
        if ( !this->is_init ) qDebug()  << this->myfunc << "can't convert"<< qargs[cmdpos+1] << " to number";
    }
    if ( ( cmdpos = qargs.indexOf ( "-elevation" ) ) > 0 )
    {
        televation = qargs[cmdpos+1].toDouble ( &this->is_init );
        if ( !this->is_init ) qDebug()  << myfunc << "can't convert"<< qargs[cmdpos+1] << " to number";
    }

    if ( this->is_init ) set_lat_lon_rad ( rad_lat, rad_lon, (long double)televation, tutm_meridian, tgk_meridian );

    return this->is_init;
}


void geocoordinates::set_lat_lon_msec (const int &msec_lat, const int &msec_lon, const long double &m_elevation )
{
    long double rad_lat = ( msec_lat * M_PI ) / ( 3600000.L * 180.L );
    long double rad_lon = ( msec_lon * M_PI ) / ( 3600000.L * 180.L );
    this->set_lat_lon_rad ( rad_lat, rad_lon, m_elevation );
}

void geocoordinates::get_lat_lon_msec (int& msec_lat, int& msec_lon, long double &m_elevation) const
{
    msec_lat = (int) (this->latitude * ( 3600000.L * 180.L )) / M_PI;
    msec_lon = (int) (this->longitude * ( 3600000.L * 180.L )) / M_PI;
    m_elevation = this->elevation;
}


QChar geocoordinates::UTMLetterDesignator()
{
    //This routine determines the correct UTM letter designator for the given latitude
    //returns 'Z' if latitude is outside the UTM limits of 80N to 80S
    //Written by Chuck Gantz- chuck.gantz@globalstar.com
    QChar LetterDesignator;

    if ( ( 80 >= this->deglat ) && ( this->deglat > 72 ) )
        LetterDesignator = 'X';
    else if ( ( 72 >= this->deglat ) && ( this->deglat > 64 ) )
        LetterDesignator = 'W';
    else if ( ( 64 >= this->deglat ) && ( this->deglat > 56 ) )
        LetterDesignator = 'V';
    else if ( ( 56 >= this->deglat ) && ( this->deglat > 48 ) )
        LetterDesignator = 'U';
    else if ( ( 48 >= this->deglat ) && ( this->deglat > 40 ) )
        LetterDesignator = 'T';
    else if ( ( 40 >= this->deglat ) && ( this->deglat > 32 ) )
        LetterDesignator = 'S';
    else if ( ( 32 >= this->deglat ) && ( this->deglat > 24 ) )
        LetterDesignator = 'R';
    else if ( ( 24 >= this->deglat ) && ( this->deglat > 16 ) )
        LetterDesignator = 'Q';
    else if ( ( 16 >= this->deglat ) && ( this->deglat > 8 ) )
        LetterDesignator = 'P';
    else if ( ( 8 >= this->deglat ) && ( this->deglat >= 0 ) )
        LetterDesignator = 'N';
    else if ( ( 0 >= this->deglat ) && ( this->deglat > -8 ) )
        LetterDesignator = 'M';
    else if ( ( -8>= this->deglat ) && ( this->deglat > -16 ) )
        LetterDesignator = 'L';
    else if ( ( -16 >= this->deglat ) && ( this->deglat > -24 ) )
        LetterDesignator = 'K';
    else if ( ( -24 >= this->deglat ) && ( this->deglat > -32 ) )
        LetterDesignator = 'J';
    else if ( ( -32 >= this->deglat ) && ( this->deglat > -40 ) )
        LetterDesignator = 'H';
    else if ( ( -40 >= this->deglat ) && ( this->deglat > -48 ) )
        LetterDesignator = 'G';
    else if ( ( -48 >= this->deglat ) && ( this->deglat > -56 ) )
        LetterDesignator = 'F';
    else if ( ( -56 >= this->deglat ) && ( this->deglat > -64 ) )
        LetterDesignator = 'E';
    else if ( ( -64 >= this->deglat ) && ( this->deglat > -72 ) )
        LetterDesignator = 'D';
    else if ( ( -72 >= this->deglat ) && ( this->deglat > -80 ) )
        LetterDesignator = 'C';
    else
        LetterDesignator = 'Z'; //This is here as an error flag to show that the Latitude is outside the UTM limits

    return LetterDesignator;
}

QString geocoordinates::geo2UTM (long double& northing, long double& easting, int& utm_meridian, QString& UTMzone ) {


    this->geo2UTM_internal();

    QString strret, str;

    easting = this->easting;
    northing = this->northing;
    utm_meridian = this->utm_meridian;
    UTMzone = this->UTMzone;
    strret = "Easting: ";
    strret +=  str.setNum ( ( double ) this->easting, 'f', 2 );
    strret += " Northing: ";
    strret +=  str.setNum ( ( double ) this->northing, 'f', 2 );
    strret += " Zone: ";
    strret +=  this->UTMzone;
    strret += " Meridian ";
    strret +=  str.setNum ( this->utm_meridian );
    return  strret;
}

void geocoordinates::geo2UTM_internal(const int utm_meridian)

{
    //converts lat/long to UTM coords.  Equations from USGS Bulletin 1532
    //East Longitudes are positive, West longitudes are negative.
    //North latitudes are positive, South latitudes are negative
    //Lat and Long are in decimal degrees
    //Does not take into account thespecial UTM zones between 0 degrees and
    //36 degrees longitude above 72 degrees latitude and a special zone 32
    //between 56 degrees and 64 degrees north latitude
    //Written by Chuck Gantz- chuck.gantz@globalstar.com


    if ( !this->is_init )
    {

        qDebug() <<  "geocoordinates::geo2UTM no values defined";
    }

    this->utm_meridian = utm_meridian;


    this->eqradius = this->EquatorialRadius.value(this->ellipsoid);
    this->eccSquared = this->EccentricitySquared.value(this->ellipsoid);

    long double k0 = 0.9996L;

    int LongOrigin;
    long double eccPrimeSquared;
    long double N, T, C, A, M;


    long double LongOriginRad;

    if ( this->deglon > -6 && this->deglon < 0 )
        LongOrigin = -3;
    else if ( this->deglon < 6 && this->deglon >= 0 )
        LongOrigin = 3; //arbitrarily set origin at 0 longitude to 3W
    else if ( this->deglon == -180 || this->deglon == 180 )
        LongOrigin = -177;   // also forces at 180 deg to 177 W
    else
    #ifdef _CAN_INT_FABS
        LongOrigin = int ( this->deglon/6.L ) *  6 + 3 * int ( this->deglon/6.L) /  std::abs ( int (this->deglon / 6.L ) );
    #else
        LongOrigin = int ( this->deglon/6.L ) *  6 + 3 * int ( this->deglon/6.L) /  fabs ( double ( int (this->deglon / 6.L ) ) );
    #endif
    // if meridian was defined
    if ( this->utm_meridian != INT_MAX )
        LongOrigin = this->utm_meridian;
    else
        this->utm_meridian = LongOrigin ;

    // set also Gauss Krueger meridian if not defined before
    if ( this->gk_meridian == INT_MAX )
        this->gk_meridian = this->utm_meridian;
    if ( this->gk_meridian < 0 )
        this->gk_meridian += 360;

    LongOriginRad = LongOrigin * M_PI / 180.L;
    //  char* utmzone;
    //compute the UTM Zone from the latitude and longitude
    //  sprintf(UTMzone, "%d%c", int((deglon + 180)/6) + 1, UTMLetterDesignator());

    long ut = long ( ( ( this->deglon + 180 ) /6 ) + 1 );
    this->UTMzone.setNum ( ut );
    this->UTMzone += UTMLetterDesignator();


    eccPrimeSquared = ( this->eccSquared ) / ( 1.L - this->eccSquared );

    N = this->eqradius / sqrt ( 1.L - this->eccSquared * sinl ( this->latitude ) * sinl ( this->latitude ) );
    T = tanl ( this->latitude ) * tanl ( this->latitude );
    C = eccPrimeSquared * cosl ( this->latitude ) * cosl ( this->latitude );
    A = cosl ( this->latitude ) * ( this->longitude - LongOriginRad );

    M = this->eqradius * ( ( 1.L  - this->eccSquared/4.L - 3.L * this->eccSquared * this->eccSquared/64.
                             - 5.L * this->eccSquared * this->eccSquared * this->eccSquared/256.L ) * this->latitude
                           - ( 3.L * this->eccSquared/8.L + 3.L * this->eccSquared * this->eccSquared/32.L
                               + 45.L * this->eccSquared * this->eccSquared * this->eccSquared / 1024.L ) * sinl ( 2.L * this->latitude )
                           + ( 15.L * this->eccSquared * this->eccSquared/256.L + 45.L * this->eccSquared * this->eccSquared * this->eccSquared/1024.L )
                           * sinl ( 4.L * this->latitude )
                           - ( 35.L * this->eccSquared * this->eccSquared * this->eccSquared/3072.L ) * sinl ( 6.L * this->latitude ) );

    this->easting = ( long double ) ( k0 * N * ( A + ( 1.L - T + C ) * A * A * A/6.L
                                                 + ( 5.L - 18.L * T + T * T + 72.L * C - 58.L * eccPrimeSquared ) * A * A * A * A * A/120.L )
                                      + 500000.0L );

    this->northing = ( long double ) ( k0 * ( M + N * tanl ( this->latitude ) * ( A * A/2.L + ( 5.L -T +9.L * C + 4.L * C *C ) * A * A * A *A/24.L
                                                                                  + ( 61.L - 58.L * T + T * T + 600.L * C - 330.L * eccPrimeSquared ) * A * A * A * A * A * A/720.L ) ) );

    if ( this->deglat < 0 )
        this->northing += 10000000.0L; //10000000 meter offset for southern hemisphere
}

QString  geocoordinates::geo2gauss (double& rechts, double& hoch, int& gk_meridian )
{
    /*
  Die Laender der Bundesrepublik Deutschland verwenden als amtliche
  Bezugsflaeche das Bessel-Ellipsoid (s. /SCH81/, S.2).
  BESSEL-Ellipsoidkonstanlten:
  a : grosse Halbachse                    a  = 6 377 397,155 m
  b : kleine Halbachse                    b  = 6 356 078,963 m
  c : Polkruemmungsradius                 c  = sqr(a)/b
  e': zweite numerische Exzentrizitaet    e2 := e'2 = (a2-b2)/b2

  internationales Ellipsoid anders definiert, etwa
  a = 6378388
  b = 6356911.946
  f = 1/127 usw.

  */

    if ( !this->is_init )
    {

        return "geocoordinates::geo2gauss no values defined";
    }

    long double a, b, c, e2, Q, ef2, ef4, ef6,
            G, eta2, N, y,  cL, dL, c2, t2;


    if (gk_meridian != INT_MAX) this->gk_meridian = gk_meridian;

    // fix the problem that this algorithm works with 0...360 deg instead of
    // -180 .. 180
    // this applies also for the Meridian

    long double temp_long = this->longitude;
    long double three = 3.0;
    if ( this->longitude < 0 )
        temp_long = 2.L * M_PI + this->longitude;


    a = 6377397.15508L;
    b = 6356078.96290L;
    c =  ( a * a) / b;
    e2= ( ( a * a) -  ( b * b ) ) / ( b * b );

    /*  Abgeleitete Konstanten, s. /SCH81/, A.3.9, S.14
  Q : meridianbogenlaenge vom Aequator bis zum Pol (meridianviertelkreis)
  */
    Q   =  c * ( 1.L -3.L /4.L * e2 * ( 1.L -15.L /16.L * e2 * ( 1.L -35./36.L * e2 * ( 1.L - 63.L /64.L * e2 ) ) ) );
    ef2  =  - c * 3.L /8.L * e2 * ( 1.L -5.L /4.L * e2 * ( 1.L -35.L /32.L * e2 * ( 1.L - 21.L /20.L *e2 ) ) );
    ef4  =  c * 15.L /256.L * e2 * e2 * ( 1.L -7.L /4.L * e2 * ( 1.L -21.L /16.L * e2 ) );
    ef6  =  - c * 35.L /3072.L * pow ( e2, three ) * ( 1.L -9.L /4.L * e2 );




    //   G : meridianbogenlaenge zur gegebenen geographischen Breite latitude
    G = Q * this->latitude + ef2 * sinl ( 2.L * this->latitude ) + ef4 * sinl ( 4.L * this->latitude ) +
            ef6 * sinl ( 6.L * this->latitude );
    c2  =  ( cosl ( this->latitude )) * ( cosl ( this->latitude ));
    t2  =  ( tanl ( this->latitude ) ) * ( tanl ( this->latitude ) );
    eta2  =  e2 * c2;

    //  N : Querkruemmungsradius, s. /SCH81/, A.3.6, S.11
    N  =  c / sqrt ( 1. + eta2 );
    dL  =  temp_long * 180.L / M_PI - this->gk_meridian;
    cL  =  pow( ( dL * M_PI / 180.L ), 2.L ) * c2/2.L ;
    y  =  M_PI / 180.L * N * cosl ( this->latitude ) * dL * ( 1.L +cL/3.L * ( 1.L - t2 + eta2 + cL/10.L * ( 5.L - t2 * ( 18.L - t2 ) ) ) );

    this->rechts  =  this->gk_meridian / 3E-6 + 5E+5 + y;
    this->hoch  =  G + N * tanl ( this->latitude ) * cL * ( 1.L + cL/6.L * ( 5.- t2 + 9.L * eta2 ) );

    rechts = this->rechts;
    hoch = this->hoch;
    gk_meridian = this->gk_meridian;

    QString strret, str;

    strret = "Rechtswert: ";
    strret +=  str.setNum ( ( double ) this->rechts , 'f', 2);
    strret += " Hochwert: ";
    strret +=  str.setNum ( ( double ) this->hoch, 'f', 2 );
    strret += " Gauss Krueger Meridian: ";
    strret +=  str.setNum ( this->gk_meridian );
    return  strret;

}

/***************************************************************************
*                                                                         *
*                                                                         *
*                                                                         *
*                          todo: implement me                             *
*                                                                         *
*                                                                         *
***************************************************************************/

void geocoordinates::gauss2geo()
{

    //rechtswert, hochwert, laenge & breite im Bogenmass als ergebnis



    /*
    Unterprogramm zur Umrechnung von Gauss-Krueger-Koordinaten (rechts,
    hoch) in geographische Koordinaten (latitude, longitude).
    Die geographischen Koordinaten latitude und longitude (Breite und Laenge)
    werden im Bogenmass berechnet.
    Im Wesentlichen geschieht die Berechnung nach SCHOEDLBAUER, Albert,
   'Rechenformeln und Rechenbeispiele zur Landesvermessung',
         Teil 1, Karlsruhe 1981: /SCH81/
     und Teil 2, Karlsruhe 1982: /SCH82/.

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
    c = ( a * a ) / b;
    e2 = ( ( a* a ) - ( b * b ) ) / ( b * b );


    /*
  c  Abgeleitete Konstanten, siehe SCHOEDL., Teil 1, S.15
  c  Q : meridianbogenlaenge vom Aequator bis zum Pol (meridianviertelkreis)
  */
    Q   =  c * ( 1.L -3./ 4.L * e2 * ( 1.L -15./ 16.L * e2 * ( 1.L - 35./ 36.L * e2 * ( 1.L -63.L / 64.L * e2 ) ) ) );
    f2  =  3.0E+0L / 8.L * e2 * ( 1.-e2 / 2.L * ( 1.L - 71.L /128.L * e2 ) );
    f4  =  21.L / 256.L * e2 * e2 * ( 1.L - e2 );
    f6  =  151.0L / 6144.0L * pow ( e2, three ); //{e2*e2*e2}

    //c  bf: Geographische Breite zur meridianbogenleange, s. /SCH81/, S.14f

    sigma = this->hoch / Q;
    bf = sigma + f2 * sinl ( 2.L * sigma ) + f4 * sinl ( 4.L * sigma ) + f6 * sinl ( 6.L * sigma );
    c2 = pow ( (cosl ( bf )) , 2.L );
    t2 = pow ( (tanl ( bf )) , 2.L );
    eta2 = c2 * e2;

    //c  N : Querkruemmungsradius (/SCH81/, A.3.6, S.11)
    N = c / ( sqrt ( 1.L + eta2 ) );
    if ( this->rechts < 0.0001 )
        y = 0; // numerical error
    //   else y = rechts - floor(rechts / 1E6) * 1E6 - 5E5; // ! vgl. /SCH82/, B.2.1, S.79}
    // floor rounds up?
    else
        y = this->rechts - long ( this->rechts / 1E6L ) * 1E6L - 5E5L; // ! vgl. /SCH82/, B.2.1, S.79}
    yN = pow ( (y/N) , 2.L ) /2.L;


    //(c  lh: Bezugsmeridian (in Bogenmass, s. /SCH82/, B.2.1, S.79
    //{     lh  = floor(rechts/1E6) * atanl(1E+0)/15;}
    //   lh  = floor(rechts/1E6) * 3. * M_PI/180.;
    // floor round up?
    lh  = long ( this->rechts/1E6L ) * 3.L * M_PI/180.L;


    this->latitude = bf - yN * tanl ( bf ) * ( 1.L + eta2 - yN/6.L * ( 5.L + 3.L * t2 + 6.L * eta2 * ( 1.L - t2 )
                                                                       - yN * ( 4.L + 3.L * t2 * ( 2.L + t2 ) ) ) );
    this->longitude = lh + y/N/cosl ( bf ) * ( 1.L - yN/3.L *
                                               ( 1.L + 2.L * t2 + eta2 - yN / 10.L * ( 5.L + t2 * ( 28.L + 24.L * t2 ) ) ) );


    if ( this->longitude >= M_PI )
        this->longitude -= ( 2.L * M_PI );
    //  lon = ( longitude * 180.L ) / M_PI;

    rad2grad<long double, int> ( this->latitude, this->deg_la, this->mi_la, this->se_la );
    rad2grad<long double, int> ( this->longitude, this->deg_lo, this->mi_lo, this->se_lo );
    flip();
}

void geocoordinates::UTM2geo() {
    //converts UTM coords to lat/long.  Equations from USGS Bulletin 1532
    //East Longitudes are positive, West longitudes are negative.
    //North latitudes are positive, South latitudes are negative
    //Lat and Long are in decimal degrees.
    //Does not take into account the special UTM zones between 0 degrees
    //and 36 degrees longitude above 72 degrees latitude and a special
    //zone 32 between 56 degrees and 64 degrees north latitude
    //Written by Chuck Gantz- chuck.gantz@globalstar.com

    long double k0 = 0.9996;

    this->eqradius = this->EquatorialRadius[this->ellipsoid];
    this->eccSquared = this->EccentricitySquared[this->ellipsoid];

    long double eccPrimeSquared;
    long double e1 = (1-sqrt(1 - this->eccSquared))/(1+sqrt(1 - this->eccSquared));
    long double N1, T1, C1, R1, D, M;
    long double LongOrigin;
    long double mu, phi1Rad;
    long double x, y;
    long double one_point_five = 1.5L;
    int ZoneNumber;
    QString qsZoneNumber;
    char ZoneLetter;
    QChar qZoneLetter;
    int NorthernHemisphere = 2; //1 for northern hemispher, 0 for southern

    x = this->easting - 500000.0L; //remove 500,000 meter offset for longitude
    y = this->northing;

    qsZoneNumber =   this->UTMzone.left(2);
    ZoneNumber = qsZoneNumber.toInt();
    // ZoneNumber = atoi(UTMzone.c_str());

    qZoneLetter = this->UTMzone[this->UTMzone.size()-1];
    ZoneLetter = qZoneLetter.toLatin1();

    //  cerr << ZoneNumber << " " << ZoneLetter << endl;
    //--qDebug() << "PLEASE CHECK ME";

    if((ZoneLetter - 'N') >= 0)
        NorthernHemisphere = 1; //point is in northern hemisphere

    else {
        NorthernHemisphere = 0; //point is in southern hemisphere
        y -= 10000000.0;        //remove 10,000,000 meter offset used for southern hemisphere
    }
    if (NorthernHemisphere == 2) {
        std::cerr << "could not detect Northern or Southern hemisphere !!!" << std::endl;
    }

    LongOrigin = (long double)(((ZoneNumber - 1) * 6 - 180) + 3);  // +3 puts origin in middle of zone

    eccPrimeSquared = (this->eccSquared)/(1.L - this->eccSquared);

    M = y / k0;
    mu = M / (this->eqradius * (1.L- this->eccSquared/4.L -3.L * this->eccSquared*eccSquared/64.L-
                                5.0L * this->eccSquared * this->eccSquared * this->eccSquared/256.L));

    phi1Rad = mu  + (3.L * e1/2.L -27.L *e1 * e1 * e1/32.)*sinl(2.L * mu)
            + (21.L * e1 * e1/16.L - 55.L * e1 * e1 * e1 * e1/32.L)*sinl(4.L * mu)
            +(151.L * e1 * e1 * e1/96.L)*sinl(6.L * mu);

    N1 = this->eqradius / sqrt(1.L - this->eccSquared * sinl(phi1Rad) * sinl(phi1Rad));
    T1 = tanl(phi1Rad) * tanl(phi1Rad);
    C1 = eccPrimeSquared * cosl(phi1Rad) * cosl(phi1Rad);
    R1 = this->eqradius * (1.L - this->eccSquared) / pow((1.L - this->eccSquared * sinl(phi1Rad) * sinl(phi1Rad)), one_point_five);
    D = x/(N1 * k0);

    this->latitude = phi1Rad - (N1*tanl(phi1Rad)/R1)
            * (D * D/2.L - (5.L + 3.L * T1 + 10.L * C1 - 4.L * C1 * C1 - 9.L * eccPrimeSquared)
               * D * D * D * D/24.
               + (61.L + 90.L * T1 + 298.L * C1 + 45.L * T1 * T1 - 252.L * eccPrimeSquared - 3.L * C1 * C1)
               * D * D * D * D * D * D/720.L);


    this->longitude = (D - ( 1.L + 2.L * T1 + C1) * D * D * D/6.L
                       + (5.L - 2.L * C1 + 28.L * T1 - 3.L * C1 * C1 + 8.L *eccPrimeSquared + 24.L * T1 * T1)
                       * D * D * D * D * D/120.L)/ cosl(phi1Rad);

    long double xlon = LongOrigin + this->longitude * 180.L / M_PI;
    this->longitude = (xlon * M_PI) / 180.L;


    this->set_lat_lon_rad(this->latitude, this->longitude, this->elevation, this->utm_meridian, this->gk_meridian);
    //rad2grad<long double, int> ( latitude, this->deg_la, this->mi_la, this->se_la );
    //rad2grad<long double, int> ( longitude, this->deg_lo, this->mi_lo, this->se_lo );

    //--qDebug() << "PLEASE CHECK ME";
    //flip();

}



void geocoordinates::set_utm(const double &northing, const long double &easting, const QString UTM_ZONE, const bool recalc_lat_lon) {


    this->northing = northing;
    this->easting = easting;
    this->UTMzone = UTM_ZONE;

    if (recalc_lat_lon) this->UTM2geo();

}

#ifdef ANDROID
  #undef sinl
  #undef tanl
  #undef cosl
  #undef atanl
#endif

bool complat(const geocoordinates &lhs, const geocoordinates &rhs)
{
    return ( lhs.latitude < rhs.latitude);
}

bool complon(const geocoordinates &lhs, const geocoordinates &rhs)
{
    return ( lhs.longitude < rhs.longitude);
}

bool compelev(const geocoordinates &lhs, const geocoordinates &rhs)
{
    return ( lhs.elevation < rhs.elevation);
}


