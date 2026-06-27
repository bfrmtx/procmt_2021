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
#include "fubheader.h"

FUBHeader::FUBHeader()
 : ITSHeader()
{



}


FUBHeader::~FUBHeader()
{
}

/*!
    \fn FUBHeader::read_header(ifstream& ifs)
    We have to make sure to skip the first byte of the Interface class!
    The ifstream must be opened before
 */
size_t FUBHeader::read_header(ifstream& ifs)
{

  ifs.seekg(0);
  cerr << "here";
  SampleFreq = 2;
  RdStrVal(ifs, "Samp.Rate:", SampleSec, 0, 0,4096);
  SampleFreq = 1. / SampleSec;
     string line;
    do {
        getline(ifs, line, '\n');
        if ( (line.find("***") != line.npos))
            break;

    } while (ifs);

cerr << "SampleFreq " << SampleFreq << "  " << SampleSec << endl;

exit (0);
return ifs.tellg();
}


size_t FUBHeader::write_header(ofstream& ofs) {

return 0;
}

/*
get the minimum information about the header
*/
void FUBHeader::identify(string& headertype, string& headerversion, size_t& headersize, size_t& n_channels, size_t& storagebits, vector<ATSSliceHeader> &tslices) const {

  headertype = "goerap";
  headerversion = "0.72";
  n_channels = goe_nc;
  storagebits = 32;
 

}

void FUBHeader::set_headersize(const size_t& hsize){


}

void FUBHeader::set_headerversion(const size_t &hver)
{

}

size_t FUBHeader::get_headersize() const {


return 0;

}







/*!
    \fn FUBHeader::set_samples(size_t& samples)
    set_ the total samples to written
 */
size_t FUBHeader::set_samples(const size_t& samples)
{
    Samples = (unsigned int) samples;
    return 0;

}



/*!
    \fn FUBHeader::set_lsb(double& lsbval)
    set_ lsb for writing
 */
double FUBHeader::set_lsb(const double& lsbval)
{
     LSBVal = lsbval;
     return LSBVal;
}




/*!
also most essential e.g. if you need transfer functions later
channel_type Ex..Hz

*/
string FUBHeader::get_channel_type() const {


  return this->ChannelType;

}

/*!
    \fn FUBHeader::set_datetime(const long int& datetime)
    get UNIX time stamp
 */
void FUBHeader::set_datetime(const long int& datetime)
{
   this->atsDateTime = datetime;
}



/*
  get correction for start time in seconds
  .. which is not impelemented in atsheader
*/

void FUBHeader::set_delta_start(const double& delta_start) {

}

/*
  get correction for stop time in seconds
  .. which is not impelemented in atsheader
*/

void FUBHeader::set_delta_stop(const double& delta_stop){

}






/*!
    \fn FUBHeader::set_samplefreq(const double& samplefreq)
    get sample freq for writing
 */
void FUBHeader::set_samplefreq(const double& samplefreq)
{
   this->SampleFreq = samplefreq;
}




/*!
    \fn ITSHeader::get_sensor_pos(double& x1, double& y1, double& z1, double& x2, double& y2, double& z2)
    regardless whether you have dipole length or ange and so on the interface class uses 3Dim setup
 */
void FUBHeader::get_sensor_pos(double& dx1, double& dy1, double& dz1, double& dx2, double& dy2, double& dz2)
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
    \fn FUBHeader::get_coordinates(int& lagra, int& lamin, double& lasec, int& logra, int& lomin, double& losec)
 */
void FUBHeader::get_coordinates(int& lagra, int& lamin, double& lasec, string& LatH, int& logra, int& lomin, double& losec, string& LonH) const
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
    \fn FUBHeader::get_bits(size_t& adcbits, size_t& storagebits)
    fixed values for ats version 0.72
 */
void FUBHeader::get_bits(size_t& adcbits, size_t& storagebits) const
{
    adcbits = 24;
    storagebits = 32;
}



/*!
    \fn FUBHeader::get_sensor_calfile(string& sensor_calfile)
 */
string FUBHeader::get_sensor_calfile() const
{

  return sensor_cal_file_name;

}


/*!
    \fn FUBHeader::get_system_calfile(string& system_calfile)
 */
string FUBHeader::get_system_calfile() const
{
    return system_cal_file_name;
}


/*!
    \fn ITSHeader::get_system_name()
 */
string FUBHeader::get_system_name() const
{
   return "GOERAP";
}


/*!
    \fn ITSHeader::get_sensor_type()
 */
string FUBHeader::get_sensor_type() const
{


   return this->sensor_cal_file_name.substr(0, sensor_cal_file_name.find("-"));
}




double FUBHeader::get_sensor_resistivity() const{
    return 0;

}

long FUBHeader::get_gmt_offset() const{

    return 0;

}

long FUBHeader::get_utc_to_gps_offset() const {
 return 0;
}


int FUBHeader::get_is_gmt() const{
return 1;

}

double FUBHeader::get_dc_offset() const{

return 0;
}

double FUBHeader::get_gain() const{

return double(Gain);

}






void FUBHeader::clean_header(){


}

void FUBHeader::byteswap_header(){
// byteswap of the header


//tbswap(HeaderLength);
//tbswap(HeaderVer);
// Header is ASCII

}


void FUBHeader::set_channel_type(const string& channel_type) {


}

/*
get something like MFS06 HX 123
*/
void FUBHeader::set_sensor_pos(const double& x1, const double& y1, const double& z1, const double& x2, const double& y2, const double& z2) {




}
void FUBHeader::set_coordinates(const int& lagra, const int& lamin, const double& lasec, const string& LatH, const int& logra, const int& lomin, const double& losec, const string& LonH) {



}


void FUBHeader::set_bits(const size_t& adcbits, const size_t& storagebits) {

  // not defined in ats ver. 0.72; default 24bit ADC and 32bit int

}

void FUBHeader::set_sensor_calfile(const string& sensor_cal_file)  {


}

void FUBHeader::set_system_calfile(const string& system_calfile)  {

 system_cal_file_name = system_calfile;
}


void FUBHeader::set_system_name(const string& data_logger_name)  {

}


void FUBHeader::set_channel_id(const string& channel_id)  {



}



void FUBHeader::set_sensor_id(const string& sensor_id)  {




}

void FUBHeader::set_system_id(const string& system_id) {

}
void FUBHeader::set_sensor_type(const string& sensor_type)  {

}


void FUBHeader::set_sensor_resistivity(const double& sensor_resistivity){


}

void FUBHeader::set_gmt_offset(const long& gmt_offset){

}

void FUBHeader::set_utc_to_gps_offset(const long int& utc_to_gps_offset) {

}


void FUBHeader::set_is_gmt(const int& is_gmt){

}

void FUBHeader::set_dc_offset(const double& dc_offset){

}

void FUBHeader::set_gain(const double& gain){
  Gain = gain;
}


void FUBHeader::set_system_chopper(const string& system_chopper) {

}


void FUBHeader::set_sensor_chopper(const string& sensor_chopper){
}



void FUBHeader::set_elevation(const double& elevation) {



}

void FUBHeader::set_meastype(const string& meastype){}
void FUBHeader::set_system_selftest_file(const string& system_selftest_file){}
void FUBHeader::set_result_selftest(const string& result_selftest){}
void FUBHeader::set_client(const string& client){}
void FUBHeader::set_contractor(const string& contractor){}
void FUBHeader::set_area(const string& area){}
void FUBHeader::set_survey_id(const string& survey_id){}
void FUBHeader::set_system_operator(const string& system_operator){}
void FUBHeader::set_xmlheader(const string& achxmlheader){}
void FUBHeader::set_comments(const string& comments){}

void FUBHeader::set_tslices(const vector<ATSSliceHeader> &tslices)
{
}







