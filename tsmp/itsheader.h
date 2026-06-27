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
#ifndef ITSHEADER_H
#define ITSHEADER_H

#include <vector>
#include "allinclude.h"
using namespace std;

#define C_ATS_CEA_NUM_HEADERS   1023     ///< add a maximum number of 1023 headers to the ATS file.


// std header = 1024, cea header 1024 + 1023 * ATSSliceHeader, should be 33760

/**
* Slice Header for CEA<br>
* <br>
* IMPORTANT: slice header must always be n * 4 byte (n * size of 1 sample)<br>
* using the slice header we have  (sizeof (ATSHeader) +  C_ATS_CEA_NUM_HEADERS * sizeof (ATSSliceHeader)))    ///< CEA Header is much bigger
*/
typedef struct ATSSliceHeader_s
{
    unsigned int   iSamples;                   //!< 0000h: number of samples for this slice
    unsigned int   iStartDateTime;             //!< 0004h: startdate/time as UNIX timestamp (UTC)
    double    dbDCOffsetCorrValue;        //!< 0008h: DC offset correction value in mV
    float     rPreGain;                   //!< 0010h: originally used pre-gain (GainStage1) - LSB is the same for all slices
    float     rPostGain;                  //!< 0014h: originally used post-gain (GainStage2) - LSB is the same for all slices
    char     DCOffsetCorrOn;             //!< 0018h: DC offset correction was on/off for this slice
    char     reserved[7];                //!< 0020h: reserved bytes to get to word / dword boundary
} ATSSliceHeader;



#include <iostream>
#include <cstdlib>


/**
@author Bernhard Friedrichs
\brief Interface class for various time series files
*
*This is the mother of invention; a virtual base class with no implementation (no functions in the cpp file); all functions are pure virtual.
A method like size_t get_samples() decribes what we want and need. Assuming that header file A uses int for samples and header file B uses double
than inside the derived classes get_samples will return size_t (samples) and make sure that the different types are converted to the whised type.
This becomes more important when you come to date function if A implements 12334 as unix time and B implements 24-12-2006-12:35:01 as a string; here
each derived class will amke sure that reading and writing will correctly interface to the super class.
*
*/
class ITSHeader{
public:


    ~ITSHeader();
    virtual size_t read_header(ifstream& ifs) = 0;
    virtual size_t write_header(ofstream& ofs) = 0;

    virtual size_t get_headersize() const = 0;
    virtual void   set_headersize(const size_t& hsize) = 0;

    virtual void   set_headerversion(const size_t& hver) = 0;

    virtual int    get_adc_offset() const = 0;
    virtual double get_adc_factor() const = 0;

    virtual void   clean_header() = 0;

    virtual void   byteswap_header() = 0;

    virtual size_t set_samples(const size_t& samples) = 0;
    virtual size_t get_samples() const = 0;

    virtual double set_lsb(const double& lsbval) = 0;
    virtual double get_lsb() const = 0;

    virtual void   identify(string& headertype, string& headerversion, size_t& headersize, size_t& n_channels, size_t& storagebits,
                            vector<ATSSliceHeader> &tslices) const = 0;

    virtual long int  get_datetime() const = 0;
    virtual void   set_datetime(const long int& datetime) = 0;

    virtual double get_delta_start() const = 0;
    virtual void   set_delta_start(const double& delta_start) = 0;

    virtual double get_delta_stop() const = 0;
    virtual void   set_delta_stop(const double& delta_stop) = 0;

    virtual double get_samplefreq() const = 0;
    virtual void   set_samplefreq(const double& samplefreq) = 0;
    virtual double get_samplefreq_orig() const = 0;
    virtual void   set_samplefreq_orig(const double& samplefreq_orig) = 0;

    virtual string get_channel_type() const = 0;
    virtual void   get_sensor_pos(double& x1, double& y1, double& z1, double& x2, double& y2, double& z2) = 0;
    virtual void   get_coordinates(int& lagra, int& lamin, double& lasec, string& LatH, int& logra, int& lomin, double& losec, string& LonH) const = 0;
    virtual void   get_bits(size_t& adcbits, size_t& storagebits) const = 0;
    virtual string get_sensor_calfile() const = 0;
    virtual string get_system_calfile() const = 0;
    virtual string get_system_name() const = 0;
    virtual string get_system_id() const = 0;
    virtual string get_channel_id() const = 0;

    virtual size_t get_channel_no() const = 0;

    virtual string get_sensor_id() const = 0;
    virtual string get_sensor_type() const = 0;

    virtual double get_sensor_resistivity() const = 0;
    virtual long   get_gmt_offset() const = 0;
    virtual long   get_utc_to_gps_offset() const = 0;
    virtual int    get_is_gmt() const = 0;
    virtual double get_dc_offset() const = 0;
    virtual double get_gain() const = 0;



    virtual string get_system_chopper() const = 0;
    virtual string get_sensor_chopper() const = 0;
    virtual double get_elevation() const = 0;

    virtual string get_meastype() const = 0;
    virtual string get_system_selftest_file() const = 0;
    virtual string get_result_selftest() const = 0;
    virtual string get_client() const = 0;
    virtual string get_contractor() const = 0;
    virtual string get_area() const = 0;
    virtual string get_survey_id() const = 0;
    virtual string get_system_operator() const = 0;
    virtual string get_xmlheader() const = 0;
    virtual string get_comments() const = 0;







    virtual void set_channel_type(const string& channel_type) = 0;
    virtual void set_sensor_pos(const double& x1, const double& y1, const double& z1, const double& x2, const double& y2, const double& z2) = 0;
    virtual void set_coordinates(const int& lagra, const int& lamin, const double& lasec, const string& LatH, const int& logra, const int& lomin, const double& losec, const string& LonH)=0;
    virtual void set_bits(const size_t& adcbits, const size_t& storagebits) = 0;
    virtual void set_sensor_calfile(const string& sensor_calfile) = 0;
    virtual void set_system_calfile(const string& system_calfile) = 0;
    virtual void set_system_name(const string& system_name) = 0;
    virtual void set_system_id(const string& system_id) = 0;
    virtual void set_channel_id(const string& channel_id) = 0;
    virtual void set_sensor_id(const string& sensor_id) = 0;
    virtual void set_sensor_type(const string& sensor_type) = 0;

    virtual void set_channel_no(const size_t& channel_no) = 0;


    virtual void set_sensor_resistivity(const double& sensor_resistivity) = 0;
    virtual void set_gmt_offset(const long& gmt_offset) = 0;
    virtual void set_utc_to_gps_offset(const long int& utc_to_gps_offset) = 0;
    virtual void set_is_gmt(const int& is_gmt) = 0;
    virtual void set_dc_offset(const double& dc_offset) = 0;
    virtual void set_gain(const double& gain) = 0;


    virtual void set_system_chopper(const string& system_chopper) = 0;
    virtual void set_sensor_chopper(const string& sensor_chopper) = 0;
    virtual void set_elevation(const double& elevation) = 0;

    virtual void set_meastype(const string& meastype) = 0;
    virtual void set_system_selftest_file(const string& system_selftest_file) = 0;
    virtual void set_result_selftest(const string& result_selftest) = 0;
    virtual void set_client(const string& client) = 0;
    virtual void set_contractor(const string& contractor) = 0;
    virtual void set_area(const string& area) = 0;
    virtual void set_survey_id(const string& survey_id) = 0;
    virtual void set_system_operator(const string& system_operator) = 0;
    virtual void set_xmlheader(const string& achxmlheader) = 0;
    virtual void set_comments(const string& comments) = 0;

    virtual void set_tslices(const vector<ATSSliceHeader>& tslices) = 0;









};

#endif
