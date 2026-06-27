/***************************************************************************
 *   Copyright (C) 2003 by Bernhard Friedrichs                             *
 *   bfriedr@isi2.metronix.de                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "emimt24header.h"




EMIMT24Header::~EMIMT24Header()
{

}

EMIMT24Header::EMIMT24Header()
{
 headersize = 4096;
}


size_t EMIMT24Header::read_header(ifstream& ifs) {
 string s;
 int h, m, se, ye, mo, dy;
 double dse;
 tm read_t;

 ifs.seekg(0);

 s = "AcqStartTime:";
 RdStrStr(ifs, s, StartTime, " ", 0, 1, headersize);

#ifdef EMIIMPORTCONTROL
   cerr << StartTime << endl;
#endif

   d_t_str2numemi(StartTime, ".", "-", ":", mo, dy, ye, h, m, dse);

#ifdef EMIIMPORTCONTROL
 cerr << mo << "m  " << dy << "d " << ye << "y  " << h << "h " << m << "m " << dse << "s" << endl;
#endif
 std:char myenv[80]="TZ=GMT0GMT";
  if (!putenv(myenv)) tzset();
  else cerr << "tsheader::no environment free; times will be interpreted wrong" << endl;
  // mktime will only take full seconds
  se = int(dse);
 	read_t.tm_year =  ye - 1900;
  read_t.tm_mon = mo - 1;
  read_t.tm_mday = dy;
  read_t.tm_hour = h;
  read_t.tm_min = m;
  read_t.tm_sec = se;
  read_t.tm_isdst = 0;
  // ansi standard??
  #ifndef _msvc
	  read_t.tm_gmtoff = 0;
  #endif
  // this is THE time for the ats format
  DateTime = mktime(&read_t);
//  cerr << "My date time " << DateTime << endl;
//  UTCGPSOffset = 0;  // not possible with EMI

   s = "AcqSmpFreq:";
  if (RdStrVal(ifs, s, SampleFreq, 0, 1, headersize)) {
//    cerr << "\n SampleFreq " << SampleFreq << endl;
  }

  if (SampleFreq == 50.)       system_cal_file_name = "mmthf50.srp";
  else if (SampleFreq == 500.) system_cal_file_name = "mmthf500.srp";
  else if (SampleFreq == 12.5) system_cal_file_name = "mmthf125.srp";
  else if (SampleFreq == 6.25) system_cal_file_name = "mmtlf625.srp";


  s = "AcqNumSmp:";
  if (RdStrVal(ifs, s, Samples, 0, 1, headersize)) {
//    cerr << "\n file contains " << Samples << endl;
  }
  s = "ChnGain:";
  if (RdStrVal(ifs, s, Gain, 0, 1, headersize)) {
//    cerr << "\n Gain set to " << Gain << endl;
  }

//  ChannelType that is idiot done inside EMI
  s = "ChannelType:";
  RdStrStr(ifs, s, tmps, "\n", 0, 1, headersize);

  if (tmps.find("H +X") != tmps.npos) ChannelType = "hx";
  if (tmps.find("H +Y") != tmps.npos) ChannelType = "hy";
  if (tmps.find("H +Z") != tmps.npos) ChannelType = "hz";
  if (tmps.find("E +X") != tmps.npos) ChannelType = "ex";
  if (tmps.find("E +Y") != tmps.npos) ChannelType = "ey";
  cerr << "EMI " << tmps << "  " << ChannelType << endl;

  //s = "SpatialSettings:";
  //x,y,z? angle?




//this will be only a question of time to fail!
  s = "Calibration by EMI File =";
  RdStrStr(ifs, s, sensor_cal_file_name, "\n", 0, 1, headersize);
  if (sensor_cal_file_name.size() < 2 || sensor_cal_file_name.size() > 256 ) {
    s = "Calibration by EMI File=";
    RdStrStr(ifs, s, sensor_cal_file_name, "\n", 0, 1, headersize);

  }
  lower(sensor_cal_file_name);

  s = "ADConv";
  if (RdStrVal(ifs, s, LSBVal, 0, 1, headersize)) {
//    cerr << "\n LSB "<< LSBVal << endl;
  }
  else cerr << "emi24::read -> Header scan failed " << endl;

  cerr << "emi24::read -> overriding LSBValue " << LSBVal;
  LSBVal = 4.9798746E-04;
  cerr << " -> " << LSBVal << endl;


// EMI time series contain a LSBValue which  does NOT include the GAIN!!
  LSBVal /= Gain;
  cerr << "emi24::read -> Gain included into LSBVal, Gain: " << Gain << endl;

  ifs.seekg(4096);


return ifs.tellg();
}


size_t EMIMT24Header::write_header(ofstream& ofs) {

return 0;
}

/*
get the minimum information about the header
*/
void EMIMT24Header::identify(string& headertype, string& headerversion, size_t& headersize, size_t& n_channels, size_t& storagebits, vector<ATSSliceHeader> &tslices) const {

  headertype = "emimt24";
  headerversion = "0.72";
  n_channels = 1;
  storagebits = 32;
  headersize = 4096;

}



size_t EMIMT24Header::get_headersize() const {


return size_t (headersize);

}

void EMIMT24Header::set_headersize(const size_t& hsize){
  headersize = hsize;

}

void EMIMT24Header::set_headerversion(const size_t &hver)
{

}


int EMIMT24Header::get_adc_offset() const {

if (SampleFreq == 6.25) {
 return 0;
}

else return 8388608;  // 800000h
}


/*!
    \fn EMIMT24Header::set_samples(size_t& samples)
    set_ the total samples to written
 */
size_t EMIMT24Header::set_samples(const size_t& samples)
{
    Samples = (unsigned int) samples;
    return 0;
}



/*!
    \fn EMIMT24Header::set_lsb(double& lsbval)
    set_ lsb for writing
 */
double EMIMT24Header::set_lsb(const double& lsbval)
{
     LSBVal = lsbval;
     return LSBVal;
}




/*!
also most essential e.g. if you need transfer functions later
channel_type Ex..Hz

*/
string EMIMT24Header::get_channel_type() const {


  return this->ChannelType;

}

/*!
    \fn EMIMT24Header::set_datetime(const long int& datetime)
    get UNIX time stamp
 */
void EMIMT24Header::set_datetime(const long int& datetime)
{
   this->DateTime = datetime;
}



/*
  get correction for start time in seconds
  .. which is not impelemented in atsheader
*/

void EMIMT24Header::set_delta_start(const double& delta_start) {

}

/*
  get correction for stop time in seconds
  .. which is not impelemented in atsheader
*/

void EMIMT24Header::set_delta_stop(const double& delta_stop){

}






/*!
    \fn EMIMT24Header::set_samplefreq(const double& samplefreq)
    get sample freq for writing
 */
void EMIMT24Header::set_samplefreq(const double& samplefreq)
{
   this->SampleFreq = samplefreq;
}




/*!
    \fn ITSHeader::get_sensor_pos(double& x1, double& y1, double& z1, double& x2, double& y2, double& z2)
    regardless whether you have dipole length or ange and so on the interface class uses 3Dim setup
 */
void EMIMT24Header::get_sensor_pos(double& dx1, double& dy1, double& dz1, double& dx2, double& dy2, double& dz2)
{
/*
  // clean
  double assume_zero_length = 0.00001;

  if (abs(this->x1) < assume_zero_length) this->x1 = 0.0;
  if (abs(this->x2) < assume_zero_length) this->x2 = 0.0;
  if (abs(this->y1) < assume_zero_length) this->y1 = 0.0;
  if (abs(this->y2) < assume_zero_length) this->y1 = 0.0;
  if (abs(this->z1) < assume_zero_length) this->z1 = 0.0;
  if (abs(this->z2) < assume_zero_length) this->z2 = 0.0;

  // it can happen that dipole was used instead of x,y,z
  if ( (x1 == x2) && (y1 == y2) && (abs(DipoleLength) > assume_zero_length) ) {

    this->x1 = -cos(M_PI / 180. * Angle) * DipoleLength / 2.;
    this->x2 =  cos(M_PI / 180. * Angle) * DipoleLength / 2.;
    this->y1 = -sin(M_PI / 180. * Angle) * DipoleLength / 2.;
    this->y2 =  sin(M_PI / 180. * Angle) * DipoleLength / 2.;
          // clean values from numeriocal artefacts
    if  (abs(this->x1) <  assume_zero_length) x1 = 0.;
    if  (abs(this->x2) <  assume_zero_length) x2 = 0.;
    if  (abs(this->y1) <  assume_zero_length) y1 = 0.;
    if  (abs(this->y2) <  assume_zero_length) y2 = 0.;
    if  (abs(this->z1) <  assume_zero_length) z1 = 0.;
    if  (abs(this->z2) <  assume_zero_length) z2 = 0.;


  }

    dx1 = double(this->x1);
    dx2 = double(this->x2);
    dy1 = double(this->y1);
    dy2 = double(this->y2);
    dz1 = double(this->z1);
    dz2 = double(this->z2);

*/
}


/*!
    \fn EMIMT24Header::get_coordinates(int& lagra, int& lamin, double& lasec, int& logra, int& lomin, double& losec)
 */
void EMIMT24Header::get_coordinates(int& lagra, int& lamin, double& lasec, string& LatH, int& logra, int& lomin, double& losec, string& LonH) const
{

  //no coorinates inside haeder
  // migh be in a external file
  lagra = lamin = logra = lomin = 0;
  lasec = losec = 0;
/*
    coordinates coordi;
    coordi.getlatlon(atsLatitude, atsLongitude);
    coordi.get_lat_lon(lagra, lamin, lasec, logra, lomin, losec);

*/
}

/*!
    \fn EMIMT24Header::get_bits(size_t& adcbits, size_t& storagebits)
    fixed values for ats version 0.72
 */
void EMIMT24Header::get_bits(size_t& adcbits, size_t& storagebits) const
{
    adcbits = 24;
    storagebits = 32;
}



/*!
    \fn EMIMT24Header::get_sensor_calfile(string& sensor_calfile)
 */
string EMIMT24Header::get_sensor_calfile() const
{

  return sensor_cal_file_name;

}


/*!
    \fn EMIMT24Header::get_system_calfile(string& system_calfile)
 */
string EMIMT24Header::get_system_calfile() const
{
    return system_cal_file_name;
}


/*!
    \fn ITSHeader::get_system_name()
 */
string EMIMT24Header::get_system_name() const
{
   return "EMIMT24";
}


/*!
    \fn ITSHeader::get_sensor_type()
 */
string EMIMT24Header::get_sensor_type() const
{


   return this->sensor_cal_file_name.substr(0, sensor_cal_file_name.find("-"));
}




double EMIMT24Header::get_sensor_resistivity() const{
return 0;
}

long EMIMT24Header::get_gmt_offset() const{

return 0;
}

long EMIMT24Header::get_utc_to_gps_offset() const {
 return 0;
}


int EMIMT24Header::get_is_gmt() const{
return 1;

}

double EMIMT24Header::get_dc_offset() const{

return 0;
}

double EMIMT24Header::get_gain() const{

return double(Gain);

}






void EMIMT24Header::clean_header(){


}

void EMIMT24Header::byteswap_header(){
// byteswap of the header


//tbswap(HeaderLength);
//tbswap(HeaderVer);
// Header is ASCII

}


void EMIMT24Header::set_channel_type(const string& channel_type) {


}

/*
get something like MFS06 HX 123
*/
void EMIMT24Header::set_sensor_pos(const double& x1, const double& y1, const double& z1, const double& x2, const double& y2, const double& z2) {




}
void EMIMT24Header::set_coordinates(const int& lagra, const int& lamin, const double& lasec, const string& LatH, const int& logra, const int& lomin, const double& losec, const string& LonH) {



}


void EMIMT24Header::set_bits(const size_t& adcbits, const size_t& storagebits) {

  // not defined in ats ver. 0.72; default 24bit ADC and 32bit int

}

void EMIMT24Header::set_sensor_calfile(const string& sensor_cal_file)  {


}

void EMIMT24Header::set_system_calfile(const string& system_calfile)  {

 system_cal_file_name = system_calfile;
}


void EMIMT24Header::set_system_name(const string& data_logger_name)  {

}


void EMIMT24Header::set_channel_id(const string& channel_id)  {



}



void EMIMT24Header::set_sensor_id(const string& sensor_id)  {




}

void EMIMT24Header::set_system_id(const string& system_id) {

}
void EMIMT24Header::set_sensor_type(const string& sensor_type)  {

}


void EMIMT24Header::set_sensor_resistivity(const double& sensor_resistivity){


}

void EMIMT24Header::set_gmt_offset(const long& gmt_offset){

}

void EMIMT24Header::set_utc_to_gps_offset(const long int& utc_to_gps_offset) {

}


void EMIMT24Header::set_is_gmt(const int& is_gmt){

}

void EMIMT24Header::set_dc_offset(const double& dc_offset){

}

void EMIMT24Header::set_gain(const double& gain){
  Gain = gain;
}


void EMIMT24Header::set_system_chopper(const string& system_chopper) {

}


void EMIMT24Header::set_sensor_chopper(const string& sensor_chopper){
}



void EMIMT24Header::set_elevation(const double& elevation) {



}

void EMIMT24Header::set_meastype(const string& meastype){}
void EMIMT24Header::set_system_selftest_file(const string& system_selftest_file){}
void EMIMT24Header::set_result_selftest(const string& result_selftest){}
void EMIMT24Header::set_client(const string& client){}
void EMIMT24Header::set_contractor(const string& contractor){}
void EMIMT24Header::set_area(const string& area){}
void EMIMT24Header::set_survey_id(const string& survey_id){}
void EMIMT24Header::set_system_operator(const string& system_operator){}
void EMIMT24Header::set_xmlheader(const string& achxmlheader){}
void EMIMT24Header::set_comments(const string& comments){}

void EMIMT24Header::set_tslices(const vector<ATSSliceHeader> &tslices)
{
}




