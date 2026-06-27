
#ifndef TSHEADER_H
#define TSHEADER_H


/**
@author Bernhard Friedrichs
*/

/*! \brief A header class for ALL time series
*
In this class everything what is needed for a header of ANY time series file is implemented.
But what about the REAL data files? With the function setheader(ITSHeader* tshead) we set a pointer
to a REAL time series header at RUNTIME; the ITSHeader in implemetation is a virual base class for all EXISTING
header files. Childrens are the the ats emimt24 header implementations.
*
*/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <valarray>
#include "atsfilename.h"

             
#include <QString>

#include "allinclude.h"
#include "itsheader.h"
#include "string_utils.h"
#include "koordi.h"

using namespace std;

class TSHeader : private coordinates {
public:

    TSHeader();
    virtual ~TSHeader();


TSHeader& operator = (const TSHeader& M) {
  if (this == &M) return *this;
  cerr << "TSHeader::HEADER COPY" << endl;
  this->my_name_is         =    M.my_name_is;
  this->my_datafile_is     =    M.my_datafile_is;
  this->my_dir_is          =    M.my_dir_is;
  this->my_out_dir_is      =    M.my_out_dir_is;
  this->my_run_number_is   =    M.my_run_number_is;
  this->band_index         =    M.band_index;
  this->my_sitenumber_is   =    M.my_sitenumber_is;

  this->adcbits            =    M.adcbits;
  this->samples            =    M.samples;
  this->samples_orig       =    M.samples_orig;
  this->samplefreq         =    M.samplefreq;
  this->samplefreq_orig    =    M.samplefreq_orig;
  this->storagebits        =    M.storagebits;
  this->byteorder          =    M.byteorder;
  this->lsbval             =    M.lsbval;
  this->adc_dcoffset       =    M.adc_dcoffset;
  this->adc_factor         =    M.adc_factor;


  this->datetime           =    M.datetime;
  this->datetime_orig      =    M.datetime_orig;
  this->stoptime           =    M.stoptime;
  this->stoptime_orig      =    M.stoptime_orig;
  this->utc_to_gps_offset  =    M.utc_to_gps_offset;
  this->is_gmt             =    M.is_gmt;
  this->gmt_offset         =    M.gmt_offset;
  this->sync_status        =    M.sync_status;
  this->sync_precision     =    M.sync_precision;
  this->delta_start        =    M.delta_start;
  this->delta_stop         =    M.delta_stop;

  this->x1                 =    M.x1;
  this->y1                 =    M.y1;
  this->z1                 =    M.z1;
  this->x2                 =    M.x2;
  this->y2                 =    M.y2;
  this->z2                 =    M.z2;
  this->elevation          =    M.elevation;


  this->x1_orig            =    M.x1_orig;
  this->y1_orig            =    M.y1_orig;
  this->z1_orig            =    M.z1_orig;
  this->x2_orig            =    M.x2_orig;
  this->y2_orig            =    M.y2_orig;
  this->z2_orig            =    M.z2_orig;



  this->manufacturer_logger=    M.manufacturer_logger;
  this->system_name        =    M.system_name;
  this->system_id          =    M.system_id;
  this->system_cal_date    =    M.system_cal_date;
  this->system_gain        =    M.system_gain;
  this->system_calfile     =    M.system_calfile;
  this->system_chopper     =    M.system_chopper;

  this->headertype         =    M.headertype;
  this->headerversion      =    M.headerversion;
  this->headerlength       =    M.headerlength;

  this->manufacturer_senor =    M.manufacturer_senor;
  this->sensor_type        =    M.sensor_type;
  this->sensor_id          =    M.sensor_id;
  this->sensor_cal_date    =    M.sensor_cal_date;
  this->sensor_gain        =    M.sensor_gain;
  this->sensor_chopper     =    M.sensor_chopper;

  this->sensor_status      =    M.sensor_status;
  this->sensor_calfile     =    M.sensor_calfile;
  this->sensor_resistivity =    M.sensor_resistivity;
  this->sensor_dc_offset   =    M.sensor_dc_offset;

  this->channel_type       =    M.channel_type;
  this->channel_no         =    M.channel_no;
  this->n_channels         =    M.n_channels;
  this->channel_id         =    M.channel_id;
  this->channel_status     =    M.channel_status;
  this->channel_calfie     =    M.channel_calfie;

  this->units              =    M.units;

  this->meas_type          =    M.meas_type;

  this->lower_bound        =    M.lower_bound;
  this->upper_bound        =    M.upper_bound;
  this->adc_plus           =    M.adc_plus;
  this->adc_minus          =    M.adc_minus;
  this->novalue            =    M.novalue;

  this->result_selftest    =    M.result_selftest;
  this->Client             =    M.Client;
  this->Contractor         =    M.Contractor;
  this->project            =    M.project;
  this->area               =    M.area;
  this->line               =    M.line;
  this->target             =    M.target;
  this->landmark           =    M.landmark;
  this->surveyID           =    M.surveyID;
  this->supervisor         =    M.supervisor;
  this->Operator           =    M.Operator;
  this->achxmlheader            =    M.achxmlheader;
  this->comments           =    M.comments;

  if (M.tslices.size()) {
      this->tslices.reserve(M.tslices.size());
      this->tslices.resize(M.tslices.size());
      for (size_t i = 0; i < M.tslices.size(); ++i) {
          this->tslices[i] = M.tslices.at(i);
      }
  }

  //this->tshead             =    M.tshead;
  this->coordinates::operator=(M);
  return *this;
}


    size_t set_adcbits(size_t& adcbits);

   /*!
        \fn TSHeader::get_adcbits()
        return bit size of ADC converter, should be 8, 16, 24, 32
     */
    size_t get__adcbits() const {return adcbits;}

    size_t set_samples(size_t& samples);

   /*!
        \fn TSHeader::get_samples()
        return amount of samples used     */
    size_t get_samples() const {return samples;}

   /*!
        \fn TSHeader::get_samples()
        return total amount of samples as specified in header
     */
    size_t get_samples_orig() const {return samples_orig;}

   /*!
        \fn TSHeader::get_samples()
        return sample frequency
     */
    double get_samplefreq() const {return samplefreq;}

    double get_samplefreq_orig() const {return samplefreq_orig;}


    size_t set_storagebits(size_t& storagebits);

    /*!
        \fn TSHeader::get_storagebits()
        return bit size of integer data to be read in data section of the file<br>
        bit size  can be 32 bit even though the ADC has 16 or 24 bits<br>
        can also be 16 bit for a 24 bit ADC if the file was compressed
     */
    size_t get_storagebits() const {return storagebits; }
    int set_is_gmt(int istrue);
    long set_datetime(long& datetime);
    long set_utc_to_gps_offset(long& utc_to_gps_offset);


    /*!
        \fn TSHeader::get_is_gmt()
        true if datetime is GMT - else datetime is UTC
     */
    int get_is_gmt() const { return is_gmt; }

    /*!
        \fn TSHeader::get_datetime()
        returns UNIX time stamp
     */
    long get_datetime() const {return datetime;}

    /*!
        \fn TSHeader::get_stoptime()
        returns UNIX time stamp
     */
    long get_stoptime() const {return stoptime;}

    /*!
        \fn TSHeader::get_utcoffset()
        returns difference between UTC and GMT (13 seconds in the year 2003)
     */
    long get_utcoffset() const {return utc_to_gps_offset; }

    string get_channel_type() const {return channel_type;}


    /*!
      \fn TSHeader::get_sensor_calfile(string& sensor_calfile)
    */
    string get_sensor_calfile()  const {return sensor_calfile;}


    /*!
        \fn TSHeader::get_system_calfile(string& system_calfile)
    */
    string get_system_calfile() const {return this->system_calfile;}

    /*!
        \fn ITSHeader::get_system_name()
    */
    string get_system_name() const {return this->system_name;}

    /*!
        \fn itsheader::get_sensor_type()
    */
    string get_sensor_type() const {return this->sensor_type;}
    string get_sensor_id() const {return this->sensor_id;}

    double get_system_gain() const {return this->system_gain;}

    string get_system_chopper() const {return this->system_chopper;}
    string get_sensor_chopper() const {return this->sensor_chopper;}
    string get_result_selftest() const {return this->result_selftest;}




    virtual size_t read_headers(ifstream& ifs);
    virtual size_t write_headers(ofstream& ofs, const unsigned short int header_ver = 80, const unsigned short int header_length = 1024);



/*!
    \fn TSHeader::setheader(ITSHeader* tshead)
    This is the most important step!
 */
    void setheader(ITSHeader* ptshead){ 
        this->tshead = ptshead;
        this->headerlength = tshead->get_headersize();
        this->tshead->clean_header();
        cerr << " setting header pointer      " << this->headerlength << endl;
        //cerr << "       " << endl;
        
        // without calling this or cerr above setheader crashes ... 
//        size_t whyis = this->headerlength;
    }

    void set_lsb(double& lsbval);

    /*!
    \fn TSHeader::get_lsb()
    */
    double get_lsb() const {return lsbval;}

    double dipole_length()const;
    double e_angle() const;
    void diptopos(double length = 0.0, double angle = 0.0);


/**************************************************************************
*                                                                         *
*                                                                         *
*    changing filename getting my own name etc                       *
*                                                                         *
*                                                                         *
*                                                                         *
**************************************************************************/
    int change_run(const string& run);
    int my_name(const char *name, const string& outget_dir);
    int my_datafile(const char *name);
    string return_outputfilename(const char* extension);
    string return_bandindex_mtx(double frequency);
    string return_atsfilename(string& run);
    string return_atmfilename(const string& run = "");
    int shift_bandindex(const int set_xmlheader);
    void   write_logfile(string filename);
    void   write_logfile();

    void set_sensor_pos(valarray<double>& spos);
    void set_lat_lon(const int& lag, const int& lam, const double& las, const string& LatH, const int& log, const int& lom, const double& los, const string& LonH);
    void set_ref_meridian(const int& utmmer, const int& gkmer, const int& ell);
    void set_elevation(const double& elevation);
    void set_diplen_angle(const double& diplen, const double& angle);
    void set_sensor(const string& sensor_type, const string& sensor_id, const string& sensor_calfile);
    void set_sensor_calfile( const string& sensor_calfile) {this->sensor_calfile = sensor_calfile;};
    void set_channel(const string& channel_type, const string& channel_id, const string& channel_calfile);
    void set_datetime(const long int& datetime, const long int& gmt_offset);
    void set_datetime_plus(const long int& time_sec);
    void set_data_info(const size_t& samples, const double& samplefreq);
    void set_system(const string& system_name, const string& system_id);
    void set_systemcalfile(const string& system_calfile);

    
    string gen_calfile_name();










string my_name_is;
string my_datafile_is;
string my_dir_is;
string my_out_dir_is;
string my_run_number_is;
string band_index;
string my_sitenumber_is;

// adu 
//029_V01_C04_R002_THz_BL_1024H.ats
bool is07;
void set_filetype_07(bool is07);
atsfilename name07;





protected:

size_t adcbits;               /*!< resolution of the ADC 8, 16, 24, 32 bit*/
size_t samples;               /*!< total samples stored in file*/
size_t samples_orig;          /*!< orig. total samples stored in file - e.g. before filtering*/
double samplefreq;            /*!< sampling frequency*/
double samplefreq_orig;       /*!< original sampling freq - e.g. before applying a filter*/
size_t storagebits;           /*!< bitsize of intergers stored inside the file*/
string byteorder;             /*!< hi byte low byte*/
double lsbval;                /*!< use INT * lsbval to get the "real" measured value*/
double adc_dcoffset;          /*!< DC offset of the ADC (SUBTRACT immedeatly after int)*/
double adc_factor;            /*!< multiply together with lsb*/



long datetime;                /*!< UNIX Time Stamp; seconds since 1970*/
long datetime_orig;           /*!< ... as it was at the beginning - e.g. before using a filter*/
long stoptime;                /*!<  will be calculated form datetime, samples and samplefreg */
long stoptime_orig;           /*!<  save a copy before filtering etc. */
long utc_to_gps_offset;       /*!< difference between UTC and GMT - 2004 13 seconds*/
int  is_gmt;                  /*!< true if GMT time is used; false if UTC is used*/
long gmt_offset;              /*!< offset of local time in seconds to GMT time, Germany Berlin Winter: 3600s*/
string sync_status;           /*!< GPS, internal*/
double sync_precision;        /*!< should be something like 10E-9*/
double delta_start;           /*!< could be internal run time between start impuls and first ADC value in seconds*/
double delta_stop;            /*!< drift of internal clock until stop time in seconds */

double x1;                    /*!< Sensor position in 3D redundant information like dipole length and angle will be calculated [m] */
double y1;                    /*!< Sensor position in 3D redundant information like dipole length and angle will be calculated [m] */
double z1;                    /*!< Sensor position in 3D redundant information like dipole length and angle will be calculated [m] */
double x2;                    /*!< Sensor position in 3D redundant information like dipole length and angle will be calculated [m] */
double y2;                    /*!< Sensor position in 3D redundant information like dipole length and angle will be calculated [m] */
double z2;                    /*!< Sensor position in 3D redundant information like dipole length and angle will be calculated [m] */
double elevation;             /*!< elevation in metres of MT site*/

double x1_orig;               /*!< Sensor position copy [m] */
double y1_orig;               /*!< Sensor position copy [m] */
double z1_orig;               /*!< Sensor position copy [m] */
double x2_orig;               /*!< Sensor position copy [m] */
double y2_orig;               /*!< Sensor position copy [m] */
double z2_orig;               /*!< Sensor position copy [m] */




string manufacturer_logger;   /*!< like EMI, Metronix, Phoenix*/
string system_name;           /*!< like MMS03e or ADU06 or MT24*/
string system_id;             /*!< e.g. the serialnumber*/
long   system_cal_date;       /*!< date of calibration of data logger*/
double system_gain;           /*!< amplifaction before ADC*/
string system_calfile;
string system_chopper;        /*!< should be on or off or none*/

string headertype;            /*!< ats, emimt24, bgrf1, goe_rap*/
string headerversion;
size_t headerlength;          /*!< headersize in byte*/


string manufacturer_senor ;   /*!< like EMI, Metronix, Phoenix*/
string sensor_type;            /*!< like MFS06 or BF7*/
string sensor_id;             /*!< e.g. serial number*/
long   sensor_cal_date;       /*!< date of calibration of sensor*/
double sensor_gain;           /*!< gain of internal pre amplifier*/
string sensor_chopper;        /*!< should be on or off or none*/

string sensor_status;
string sensor_calfile;
double sensor_resistivity;    /*!< e.g. DC resistivity of the electrodes*/
double sensor_dc_offset;      /*!< e.g. DC offset (potemtial) of the electrodes*/


string channel_type;            /*!< e.g. Ex, Ey, Hx, Hy, Hz or temperature*/
size_t channel_no;            /*!< 0, 1, 2, ... n*/
size_t n_channels;            /*!< total channels*/
string channel_id;              /*!< e.g. ADC board*/
string channel_status;
string channel_calfie;




string units;                 /*!< like mV or V or nT or mV/km or C and so on*/

string meas_type;             /*!< MT or CSAMT or source or Temperature*/

double lower_bound, upper_bound, novalue; /*!< values outside specifications*/
double adc_plus, adc_minus;               /*!< physical values of the converter*/


string result_selftest;
string Client;
string Contractor;
string project;
string area;
string line;
string target;
string landmark;
string surveyID;
string supervisor;
string Operator;
string achxmlheader;
string comments;




ITSHeader* tshead;         /*!< pointer to Interface class */



ATSSliceHeader tslice;
std::vector<ATSSliceHeader> tslices;








};

#endif
