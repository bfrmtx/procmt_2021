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
#ifndef ATSHEADER72_H
#define ATSHEADER72_H

using namespace std;
#include "itsheader.h"
#include "string_utils.h"
#include <iostream>
#include <fstream>
#include "koordi.h"
#include "allinclude.h"

/**
@author Dr. Bernhard Friedrichs
*/
class ATSHeader72 : public ITSHeader
{
public:

    ATSHeader72();
   ~ATSHeader72();
    virtual size_t read_header(ifstream& ifs);
    virtual size_t write_header(ofstream& ofs);
    virtual size_t get_headersize() const;
    virtual void set_headersize(const size_t& hsize);

    virtual int    get_adc_offset() const {return 0;}
    virtual double get_adc_factor() const {return 1.;}


    virtual void clean_header();
    virtual void byteswap_header();

    virtual size_t set_samples(const size_t& samples);

    /*!
     return samples as shown in header
    */
    virtual size_t get_samples() const {return size_t (Samples);}


    double set_lsb(const double& lsbval);

    /*!
     return least significant bit
    */
    virtual double get_lsb() const {return LSBVal;}

    virtual void identify(string& headertype, string& headerversion, size_t& headersize, size_t& n_channels, size_t& storagebits) const;

    /*!
     return UNIX time stamp
    */
    virtual long int  get_datetime() const {return atsDateTime;}

    virtual void set_datetime(const long int& datetime);

    /*
     return correction for start time in seconds
    */
    virtual double get_delta_start() const {return 0.0;}
    virtual void   set_delta_start(const double& delta_start);

    /*
     return correction for stop time in seconds
    */
    virtual double get_delta_stop()const{return 0.0;}
    virtual void   set_delta_stop(const double& delta_stop);




    /*!
     returns sample freq from header as double
    */
    virtual double get_samplefreq() const {
//        cerr << "    ..........hhhhh ! sample ! " << SampleFreq << endl;
		return double (SampleFreq);}
    virtual double get_samplefreq_orig() const {
//        cerr << ".........hhhhh ! orig ! " << SampleFreqOrig << endl;
	return double (SampleFreqOrig);}

    virtual void set_samplefreq(const double& samplefreq);
    virtual void set_samplefreq_orig(const double& samplefreq_orig);		

    virtual string get_channel_type() const;
    virtual void   get_sensor_pos(double& x1, double& y1, double& z1, double& x2, double& y2, double& z2);
    virtual void   get_coordinates(int& lagra, int& lamin, double& lasec, string& LatH, int& logra, int& lomin, double& losec, string& LongH) const;
    virtual void   get_bits(size_t& adcbits, size_t& storagebits) const;
    virtual string get_sensor_calfile() const;
    virtual string get_system_calfile() const;
    virtual string get_system_name() const;
    virtual string get_system_id() const;
    virtual string get_channel_id() const;
    virtual string get_sensor_id() const;
    virtual string get_sensor_type() const;

    virtual double get_sensor_resistivity() const;
    virtual long   get_gmt_offset() const;
    virtual long   get_utc_to_gmt_offset() const;
    virtual int    get_is_gmt() const;
    virtual double get_dc_offset() const;
    virtual double get_gain() const;



    virtual string get_system_chopper() const;
    virtual string get_sensor_chopper() const;
    virtual double get_elevation() const;

    virtual string get_meastype() const;
    virtual string get_system_selftest_file() const;
    virtual string get_result_selftest() const;
    virtual string get_client() const;
    virtual string get_contractor() const;
    virtual string get_area() const;
    virtual string get_survey_id() const;
    virtual string get_system_operator() const;
    virtual string get_weather() const;
    virtual string get_comments() const;








    virtual void set_channel_type(const string& channel_type);
    virtual void set_sensor_pos(const double& x1, const double& y1, const double& z1, const double& x2, const double& y2, const double& z2);
    virtual void set_coordinates(const int& lagra, const int& lamin, const double& lasec, const string& LatH, const int& logra, const int& lomin, const double& losec, const string& LonH);
    virtual void set_bits(const size_t& adcbits, const size_t& storagebits);
    virtual void set_sensor_calfile(const string& sensor_calfile);
    virtual void set_system_calfile(const string& system_calfile);
    virtual void set_system_name(const string& system_name);
    virtual void set_system_id(const string& system_id);
    virtual void set_channel_id(const string& channel_id);
    virtual void set_sensor_id(const string& sensor_id);
    virtual void set_sensor_type(const string& sensor_type);

    virtual void set_sensor_resistivity(const double& sensor_resistivity);
    virtual void set_gmt_offset(const long& gmt_offset);
    virtual void set_utc_to_gmt_offset(const long int& utc_to_gmt_offset);
    virtual void set_is_gmt(const int& is_gmt);
    virtual void set_dc_offset(const double& dc_offset);
    virtual void set_gain(const double& gain);


    virtual void set_system_chopper(const string& system_chopper);
    virtual void set_sensor_chopper(const string& sensor_chopper);
    virtual void set_elevation(const double& elevation);

    virtual void set_meastype(const string& meastype);
    virtual void set_system_selftest_file(const string& system_selftest_file);
    virtual void set_result_selftest(const string& result_selftest);
    virtual void set_client(const string& client);
    virtual void set_contractor(const string& contractor);
    virtual void set_area(const string& area);
    virtual void set_survey_id(const string& survey_id);
    virtual void set_system_operator(const string& system_operator);
    virtual void set_weather(const string& weather);
    virtual void set_comments(const string& comments);





    // this is exactly the description of the atsheader
    // all variable are in this oreder stored inside the binary header
protected:
#ifdef __CYGWIN__
  #define __obj_err_
#endif
#ifdef __MINGW32__
  #define __obj_err_
#endif

  
#define __obj_err_

  
#ifdef __obj_err_
 virtual void mv_header_read();
 virtual void mv_header_write();
struct xheader {
  unsigned short int  HeaderLength;                             /*!< ref. to manual*/
  unsigned short int  HeaderVer;                                /*!< ref. to manual*/
  unsigned int Samples;                                         /*!< ref. to manual*/
  float  SampleFreq;                                            /*!< ref. to manual*/
  int atsDateTime;                                              /*!< ref. to manual*/
  double LSBVal;                                                /*!< ref. to manual*/
  int GMTOffset;                                                /*!< ref. to manual*/
// change 2007
//  char skip1[4];                                                /*!< ref. to manual*/
  float  SampleFreqOrig;                                            /*!< ref. to manual*/
  unsigned short int SerNoADU;                                  /*!< ref. to manual*/
  unsigned short int SerNoADB;                                  /*!< ref. to manual*/
  unsigned char ChanNo;                                		/*!< ref. to manual*/
  unsigned char cSensorChopper;
// eigentlich m�sste ein byte gelesen und konvertiert werden; geht aber mit 2 byte
//  ifs.read(a.skip, 1);                                        /*!< ref. to manual*/
  char cChannelType[2];                                         /*!< ref. to manual*/
  char cSensorType[6];                                          /*!< ref. to manual*/
  unsigned short int SensorNo;                                  /*!< ref. to manual*/
  float x1;                                                     /*!< position 1*/
  float y1;                                                     /*!< ref. to manual*/
  float z1;                                                     /*!< ref. to manual*/
  float x2;                                                     /*!< ref. to manual*/
  float y2;                                                     /*!< ref. to manual*/
  float z2;                                                     /*!< ref. to manual*/
  float DipoleLength;                                           /*!< ref. to manual*/
  float Angle;                                                  /*!< ref. to manual*/
  float ProbeResistivity;                                       /*!< ref. to manual*/
  float DCOffset;                                               /*!< ref. to manual*/
  float Gain;                                                   /*!< gain amplification; 1 or 30 with ADB board */
  char skip2[4];                                                /*!< ref. to manual*/
  int atsLatitude;                                              /*!< [ms] milli seconds */
  int atsLongitude;                                             /*!< [ms] */
  int atsElevation;                                             /*!< [cm] */
  char LatLon_Type;                                             /*!< ref. to manual*/
  char Coord_Type;                                              /*!< ref. to manual*/
  unsigned short int Ref_Meridian;                              /*!< ref. to manual*/
  double x;          /*!< reserved for Gauss Krueger or UTM coordinates */
  double y;                                                     /*!< ref. to manual*/
  char Sync_Status;                                             /*!< ref. to manual*/
  //following variable is Byte
  char Accuracy;                                                /*!< ref. to manual*/

  short int UTCGPSOffset;                                       /*!< ref. to manual*/
  char cSystemType[12];                                          /*!< ref. to manual*/
  char csurvey_header_fname[12];                                /*!< ref. to manual*/
  char meas_type[4];                                            /*!< ref. to manual*/

  char clog_file_sys_self_test[12];                             /*!< ref. to manual*/
  char result_self_test[2];                                     /*!< ref. to manual*/
  unsigned char cSystemChopper;
  char skip4[1];                                                /*!< ref. to manual*/

  unsigned short int num_cal_freq;                              /*!< number of cal frequencies  in file */
  unsigned short int lofe;                                      /*!< length of frequency entry (byte) */
  unsigned short int vocf;                                      /*!< version of calibration format * 100 */
  unsigned short int start_cal_info_head;                       /*!< start address of calibration information */
  char skip5[8];                                                /*!< ref. to manual*/
  char cadu_cal_file_name[12];                                  /*!< ref. to manual*/
  int date_of_cal_adu;                                          /*!< ref. to manual*/
  char csensor_cal_file_name[12];                               /*!< ref. to manual*/
  int date_of_cal_sensor;                                       /*!< ref. to manual*/

  char skip6[32];                                               /*!< ref. to manual*/

  char cClient[16];                                             /*!< ref. to manual*/
  char cContractor[16];                                         /*!< ref. to manual*/
  char cArea[16];                                               /*!< ref. to manual*/
  char cSurveyID[16];                                           /*!< ref. to manual*/
  char cOperator[16];                                           /*!< ref. to manual*/

  char ccoordinates_type[8];                                    /*!< UTM */
  int atsZoneNumber;                                            /*!< ref. to manual*/
  char atsLetterDesignator;                                     /*!< ref. to manual*/
  /*!< I can skip these three chars in the code and set_ the same effect!! Why? */
  char atsemptystr[3];                                          /*!< ref. to manual*/
  double atsNorthing;                                           /*!< ref. to manual*/
  double atsEasting;                                            /*!< ref. to manual*/
  int atsutm_meridian;                                          /*!< ref. to manual*/
  char cReferenceEllipsoid[12];                                 /*!< ref. to manual*/


  char cskip7[63];                                              /*!< was 112 */
  char cWeather[64];                                            /*!< ref. to manual*/
  char cComments[512];                                          /*!< ref. to manual*/

} header;

#endif

  unsigned short int  HeaderLength;                             /*!< ref. to manual*/
  unsigned short int  HeaderVer;                                /*!< ref. to manual*/
  unsigned int Samples;                                         /*!< ref. to manual*/
  float  SampleFreq;                                            /*!< ref. to manual*/
  int atsDateTime;                                              /*!< ref. to manual*/
  double LSBVal;                                                /*!< ref. to manual*/
  int GMTOffset;                                                /*!< ref. to manual*/
// change 2007
//  char skip1[4];                                                /*!< ref. to manual*/
  float  SampleFreqOrig;                                            /*!< ref. to manual*/
  unsigned short int SerNoADU;                                  /*!< ref. to manual*/
  unsigned short int SerNoADB;                                  /*!< ref. to manual*/
  unsigned char ChanNo;                                    /*!< ref. to manual*/
  unsigned char cSensorChopper;
// eigentlich m�sste ein byte gelesen und konvertiert werden; geht aber mit 2 byte

  char cChannelType[2];                                         /*!< ref. to manual*/
  char cSensorType[6];                                          /*!< ref. to manual*/
  unsigned short int SensorNo;                                  /*!< ref. to manual*/
  float x1;                                                     /*!< position 1*/
  float y1;                                                     /*!< ref. to manual*/
  float z1;                                                     /*!< ref. to manual*/
  float x2;                                                     /*!< ref. to manual*/
  float y2;                                                     /*!< ref. to manual*/
  float z2;                                                     /*!< ref. to manual*/
  float DipoleLength;                                           /*!< ref. to manual*/
  float Angle;                                                  /*!< ref. to manual*/
  float ProbeResistivity;                                       /*!< ref. to manual*/
  float DCOffset;                                               /*!< ref. to manual*/
  float Gain;                                                   /*!< gain amplification; 1 or 30 with ADB board */
  char skip2[4];                                                /*!< ref. to manual*/
  int atsLatitude;                                              /*!< [ms] milli seconds */
  int atsLongitude;                                             /*!< [ms] */
  int atsElevation;                                             /*!< [cm] */
  char LatLon_Type;                                             /*!< ref. to manual*/
  char Coord_Type;                                              /*!< ref. to manual*/
  unsigned short int Ref_Meridian;                              /*!< ref. to manual*/
  double x;          /*!< reserved for Gauss Krueger or UTM coordinates */
  double y;                                                     /*!< ref. to manual*/
  char Sync_Status;                                             /*!< ref. to manual*/
  //following variable is Byte
  char Accuracy;                                                /*!< ref. to manual*/

  short int UTCGPSOffset;                                       /*!< ref. to manual*/
  char cSystemType[12];                                          /*!< ref. to manual*/
  char csurvey_header_fname[12];                                /*!< ref. to manual*/
  char meas_type[4];                                            /*!< ref. to manual*/

  char clog_file_sys_self_test[12];                             /*!< ref. to manual*/
  char result_self_test[2];                                     /*!< ref. to manual*/
  unsigned char cSystemChopper;
  char skip4[1];                                                /*!< ref. to manual*/

  unsigned short int num_cal_freq;                              /*!< number of cal frequencies  in file */
  unsigned short int lofe;                                      /*!< length of frequency entry (byte) */
  unsigned short int vocf;                                      /*!< version of calibration format * 100 */
  unsigned short int start_cal_info_head;                       /*!< start address of calibration information */
  char skip5[8];                                                /*!< ref. to manual*/
  char cadu_cal_file_name[12];                                  /*!< ref. to manual*/
  int date_of_cal_adu;                                          /*!< ref. to manual*/
  char csensor_cal_file_name[12];                               /*!< ref. to manual*/
  int date_of_cal_sensor;                                       /*!< ref. to manual*/

  char skip6[32];                                               /*!< ref. to manual*/

  char cClient[16];                                             /*!< ref. to manual*/
  char cContractor[16];                                         /*!< ref. to manual*/
  char cArea[16];                                               /*!< ref. to manual*/
  char cSurveyID[16];                                           /*!< ref. to manual*/
  char cOperator[16];                                           /*!< ref. to manual*/

  char ccoordinates_type[8];                                    /*!< UTM */
  int atsZoneNumber;                                            /*!< ref. to manual*/
  char atsLetterDesignator;                                     /*!< ref. to manual*/
  /*!< I can skip these three chars in the code and set_ the same effect!! Why? */
  char atsemptystr[3];                                          /*!< ref. to manual*/
  double atsNorthing;                                           /*!< ref. to manual*/
  double atsEasting;                                            /*!< ref. to manual*/
  int atsutm_meridian;                                          /*!< ref. to manual*/
  char cReferenceEllipsoid[12];                                 /*!< ref. to manual*/


  char cskip7[63];                                              /*!< was 112 */
  char cWeather[64];                                            /*!< ref. to manual*/
  char cComments[512];                                          /*!< ref. to manual*/


};

#endif
