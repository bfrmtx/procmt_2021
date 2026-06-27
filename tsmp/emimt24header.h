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
#ifndef EMIMT24HEADER_H
#define EMIMT24HEADER_H

#include "itsheader.h"
#include <string>
#include <iostream>
#include <fstream>
#include "string_utils.h"
#include "allinclude.h"


/**
@author Bernhard Friedrichs
*/
class EMIMT24Header : public ITSHeader
{
public:


    ~EMIMT24Header();
    EMIMT24Header();
    
    virtual size_t read_header(ifstream& ifs);
    virtual size_t write_header(ofstream& ofs);
    virtual size_t get_headersize() const;
    virtual void set_headersize(const size_t& hsize);
    virtual void   set_headerversion(const size_t& hver);


    
    virtual double get_adc_factor() const {return 1.;}
    virtual int get_adc_offset() const;
    
    virtual void clean_header();
    virtual void byteswap_header();

    virtual size_t set_samples(const size_t& samples);
    
    /*!
     return samples as shown in header
    */
    virtual size_t get_samples() const {return Samples;}    
    
    
    double set_lsb(const double& lsbval);
    
    /*!
     return least significant bit
    */
    virtual double get_lsb() const {return LSBVal;}
    
    virtual void identify(string& headertype, string& headerversion, size_t& headersize, size_t& n_channels, size_t& storagebits,
                          vector<ATSSliceHeader> &tslices) const;
    
    /*!
     return UNIX time stamp
    */
    virtual long int  get_datetime() const {return DateTime;}
    
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
    

    virtual void set_channel_no(const size_t& channel_no) {;}
    virtual size_t get_channel_no() const {return 0;}
    
    
    
    /*!
     returns sample freq from header as double
    */
    virtual double get_samplefreq() const {return double (SampleFreq);}
    
    virtual void set_samplefreq(const double& samplefreq);

    virtual double get_samplefreq_orig() const {return double (0);}
    virtual void set_samplefreq_orig(const double& samplefreq) {;};
    
    virtual string get_channel_type() const;
    virtual void   get_sensor_pos(double& x1, double& y1, double& z1, double& x2, double& y2, double& z2);
    virtual void   get_coordinates(int& lagra, int& lamin, double& lasec, string& LatH, int& logra, int& lomin, double& losec, string& LonH) const;
    virtual void   get_bits(size_t& adcbits, size_t& storagebits) const;
    virtual string get_sensor_calfile() const;
    virtual string get_system_calfile() const;
    virtual string get_system_name() const;
    virtual string get_system_id() const {return "";}
    virtual string get_channel_id() const {return "";}
    virtual string get_sensor_id() const {return "";}
    virtual string get_sensor_type() const;
    
    virtual double get_sensor_resistivity() const;
    virtual long   get_gmt_offset() const;
    virtual long   get_utc_to_gps_offset() const;
    virtual int    get_is_gmt() const;
    virtual double get_dc_offset() const;
    virtual double get_gain() const;
    
    
    
    virtual string get_system_chopper() const {return "";}
    virtual string get_sensor_chopper() const {return "";}
    virtual double get_elevation() const {return 0.;}
    
    virtual string get_meastype() const {return "MT";}
    virtual string get_system_selftest_file() const {return "";}
    virtual string get_result_selftest() const {return "ok";}
    virtual string get_client() const {return "";}
    virtual string get_contractor() const {return "";}
    virtual string get_area() const {return "";}
    virtual string get_survey_id() const {return "";}
    virtual string get_system_operator() const {return "";}
    virtual string get_xmlheader() const {return "";}
    virtual string get_comments() const {return "";}

    

    
    
    
    
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
    virtual void set_utc_to_gps_offset(const long int& utc_to_gps_offset);
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
    virtual void set_xmlheader(const string& achxmlheader);
    virtual void set_comments(const string& comments);
    virtual void set_tslices(const vector<ATSSliceHeader>& tslices);

   
    
    

  
    // this is exactly the description of the atsheader
private:
 size_t Samples, usehfadc;
  double	SampleFreq;
  double LSBVal,                  // EMI calls this ADConv
         ADCFactor;               // do we need this??
  int adc_offset;
  float Gain;                     // will not be used
  long DateTime;
  string StartTime,               // try to read from file ... why not UNIX times stamp!
         ChannelType,
         sensor_cal_file_name,    // try to read calibratio file name from time series file
         system_cal_file_name,   // the sam for system cal file MT24
         tmps;



size_t headersize;			// set by constructor		  

};

#endif
