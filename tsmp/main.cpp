/***************************************************************************
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/
// tsmp -ascii -tc -abst 0 -dateformat ISO -start_date 2009-07-02T14:49:48 -stop_date 2009-07-02T14:49:50 053C44XA.ats
//  tsmp -ascii -tc -abst 0 -dateformat ISO -isostart 2009-07-02T14:49:48 -isostop 2009-07-02T14:49:50 053C44XA.ats
//

/*! \mainpage TSMP - Time Series Manipulation
 *
 * \section intro_sec Introduction
 *
 *  A command line driven data maipulation program
 *
 * \section install_sec Installation

/.configure
make
 *
 * \subsection step1 Step 1: FFTW
 *
 * etc...
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QCoreApplication>
#include <cctype> // try to find out wheter your machine is
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <valarray>
#include <vector>
// little (INTEL) or big endian (motorola, hp)

using namespace std;

#include "adu06cal.h"
#include "adu07cal.h"
#include "atsheader80.h"
#include "efp05cal.h"
#include "emi_bfcal.h"
#include "emimt24header.h"
#include "emisyscal.h"
#include "fgs01cal.h"
#include "fgs02cal.h"
#include "fubheader.h"
#include "ltscal.h"
#include "mfs07cal.h"
#include "mtxcal.h"
#include "plaincal.h"
#include "tsdata.h"
#include "tsheader.h"

#include "eDateTime.h"

#include "my_valarray.h"
#include "string_utils.h"

#include "atmfile.h" // atm class for mapros
#include "coherency.h"
#include "date_utils.h"
#include "mtstd.h"   // ... maybe in the future....
#include "spectra.h" // my spectral classes

#ifdef myMySQL
#include "mysql_simple_vector.h"
#include </usr/include/mysql++/mysql++.h>

#include <atsfilename>
using namespace mysqlpp;
#endif

#ifdef myGSL
#include <errno.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_rng.h>
#endif

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);

  if (argc < 2) {
    cerr << "\nProgVer " << "1.01 Qt" << endl;
    // do some checks for the console message

#if SIZEOF_LONG == 4

    cerr << "size of long is ok (32 bit)" << endl;
#endif

#if SIZEOF_LONG == 8

    cerr << "size of  is NOT OK (64 bit instead of 32) " << endl;
    cerr << "atsview will not run" << endl;
#endif

#if SIZEOF_DOUBLE == 8

    cerr << "size of double is ok and 8 bytes" << endl;
#endif

#if SIZEOF_DOUBLE == 10

    cerr << "size of double is PERHAPS NOT OK! and 10 bytes" << endl;
#endif

#if SIZEOF_DOUBLE == 12

    cerr << "size of double is PERHAPS NOT! OK and 12 bytes" << endl;
#endif

#if SIZEOF_DOUBLE == 16

    cerr << "size of double is PERHAPS NOT! OK and 16 bytes" << endl;
#endif

#ifdef __CYGWIN__

    cerr << "using CYGWIN " << endl;
#endif

#ifdef __MINGW32__

    cerr << "using MINGW32 " << endl;
#endif

#ifdef __WIN32__

    cerr << "using WIN32 environment" << endl;
#endif

#if SIZEOF_LONG > 4

    cerr << "size of  is NOT OK (64 bit? instead of 32) " << endl;
    cerr << "program will not be able to read the 32 bit values from ats file" << endl;
    cerr << "using  int as type" << endl;
    return EXIT_FAILURE;
#endif

#if SIZEOF_DOUBLE > 8

    cerr << "size of double is not ok greater than 8 bytes" << endl;
#endif

    /***************************************************************************
     *                                                                         *
     *    display command line options                                         *
     *                                                                         *
     *                                                                         *
     *                                                                         *
     *                                                                         *
     ***************************************************************************/

    cerr << "\nusage: " << argv[0] << " arguments infile\n";
    cerr << "\nex.: " << argv[0] << " -start 4096 -use 1024 001C02XB.ats";
    cerr << "\nselects file 001C02XB.ats, skips first 4096 values(0...4095) and writes 1024 to disk\n";
    cerr << "\nex.: " << argv[0] << " 0 all 001C02XB.ats dumps the complete file\n";
    // cerr << " -all reads the complete file\n";
    cerr << " -chan reads a channel from multi channle file; -chan 0 2 4 reads the according channels\n";
    cerr << " -run nn provides a new run number; e.g. for conversion of time series or other output\n";
    cerr << " -add 5.2 adds 5.2 mV to atsfile \n";
    cerr << " -add mean removes the mean of the time series\n";
    cerr << " -mul 2.5 multiplies atsfiles with given value; can be used together with -corr -wrh or in ascii output without changing the ats file\n";
    cerr << "      if you use -mul -1 you will change the polarity of the electrical / magnetical field\n";
    cerr << " -mul mean devides the time series by its mean\n";
    cerr << "  use -corr -wrh -time_s 13 to make changes permanent in the atsfile\n";
    cerr << " -gensign generates artificial signal (undocumented yet)\n";
    cerr << " -gen_rand" << endl;
    cerr << " -gen_sin freq freq .. freq" << endl;
    cerr << "  [-with_rectpulse start stop dvalue] " << endl;
    cerr << " -gen_rand_gsl mix1 ... mix5; e.g. 1 2 11.31 22.61 1 for coil or 1 2 0.0014142136 0.0028284272 1 for a fluxgate" << endl;
    cerr << " -cut_int use this for reading 32bit integer; use -start -use/-stop -run xx -cut_int for shortening the timeseries without any processing" << endl;
    cerr << " -nodata  atsfile will not be converted into ascii, data section is not read\n";
    cerr << " -ascii writes ascii data of time series and converted timeseries (especially from despike, destep, stddev)" << endl;
    cerr << "   which will write their results into ascii files but ALSO posibbly to atm files" << endl;
    cerr << " -ts2mysql [truncate, truncate_first] will write time series into sql data base ... TESTING VERSION " << endl;
    cerr << " -ncal indicates wether this is the first call of tsmp or not; needed for truncate options and online processing" << endl;
    cerr << " -ascii -back -wl .. etc. writes the timeseries data after FFT application and NOT the raw data\n";
    cerr << " -nspw  data section is read in but not written to disk; useful if you calculate spectra and do not need conversion\n";
    cerr << " -start 4096 skips the first 4096 samples of the atsfile\n";
    cerr << " -slice 2 use second slice if available, the start sample 4 if the start sample of 2. slice in this case\n";
    cerr << " -isostart 2009-12-24T16:34:00.5 try to start reading at this time\n";
    cerr << " -use 16384 reads 16384 samples after given start point\n";
    cerr << " -stop 20000 reads from start to stop defined samples from the atsfile\n";
    cerr << " -isostop 2009-12-24T18:34:01.5 try to stop reading at this time\n";
    cerr << " -last 1024 reads the last 1024 points from the data file\n";
    cerr << " -samples 3000 -corr -wrh set samples inside header (better do not do this)\n";
    cerr << " -wl 2048 uses 2048 point for the FFT; activates spectra calculations automatically\n";
    cerr << "    -wl 1 will use all given points for the fft window\n";
    cerr << "    this can only be used together with -back in order to get inverted time series\n";
    cerr << "    the spectra itself is zero padded and is not a \"true\" spectrum";
    cerr << " -rect use a rectangular fft window instead of Hanning (default)\n";
    cerr << " -trf auto enable automatically reading of the transfer function of the used sensor as specified in the header\n";
    cerr << "      in the log file you find Sensor Type; if MFS05 the program tries to read mfs05.trf and so on\n";
    cerr << "      sensor cal file name \"SENSOR.CAL\" in the header is a template and  not a valid entry\n";
    cerr << "      otherwise the sensor cal file name will be read\n";
    cerr << " -trf theo uses the built in transfer functions of the spectra module\n";
    cerr << " -trf hw05on uses a measured transfer function with chopper on of the spectra module\n";
    cerr << " -trf hw05off uses a measured transfer function with chopper off of the spectra module\n";
    cerr << " -trf hw05auto uses a measured transfer function with chopper auto of the spectra module\n";
    cerr << " -trf test.trf reads the specified file with 3 columns; 1. frequency, 2. amplitude, 3. phase in degree (not radians)\n";
    cerr << " -trf1 a.trf -trf2 b.trf reads both file in the two files only mode (like with atsadd for example)\n";
    cerr << " -dump_trf writes a control file for sensor, system and other calibration to disk\n";
    cerr << " -latlon 52 09 12.4 N 9 45 22.6 E  -corr -wrh corrects the latitude and longitude\n";
    cerr << " -elev 56 -corr -wrh  corrects the elevation with +56 centimeters\n";
    cerr << " -lsbval [-corr -wrh] sets a new LSB \n";
    cerr << " -fliplsbval [-corr -wrh] multiplies the lsb by -1 = change of polarity of the channel\n";
    cerr << " -adbser 34 -corr -wrh sets the ADB board serial number to 34\n";
    cerr << " -aduser 14 -corr -wrh sets the ADU serial number to 14\n";
    cerr << " -sensser 114 -corr -wrh sets the Sensor serial number to 114\n";
    cerr << " -samplefreq 2 -corr -wrh set the sampling frequency to 2 Hz inside the atsheader\n";
    cerr << " -datetime 12346 -corr -wrh set the absolute UNIX time stamp of the header\n";
    cerr << " -time_s seconds shifts the start time in secons\n";
    cerr << " -gmtoff 3600 -corr -wrh corrects the UTC (former GMT) offset to your local time with +3600 s\n";
    cerr << " -utctogps offset between UTC and GPS clock (2015 = 16 secs)\n";
    cerr << " -sensorcal MFS07.trf -corr -wrh changes the sensor cal file name in the header; use the DOS 8.3 format\n";
    cerr << " -systemcal mylogger.trf -corr -wrh changes a possible calibration file for the datalogger\n";
    cerr << " -sensor_type EFP05 -corr -wrh changes the sensor name in the header; max. 6 char\n";
    cerr << " -system_name ADU06,  MMS03e (-corr -wrh) sets the system name \n";
    cerr << " -channel_type EX -corr -wrh changes the channel type in the header; atsview makes use of this information\n";
    cerr << " -corr activates the header correction mode \n";
    cerr << " -wrh writes the correction permant to the file; with -corr you can simulate the the corrections\n";
    cerr << " -diplen 100 -corr -wrh changes dipole length to 100 metersn";
    cerr << " -resis 1000 -corr -wrh changes the contact resistivity to 1000 ohm/m\n";
    cerr << " -angle  90 -corr -wrh changes the angle to 90 degress\n";
    cerr << " -spos x1 y1 z1 x2 y2 z2 -corr -wrh changes the coordinates of the sensor position\n";
    cerr << "   -spos 100 n n 0 n n -corr -wrh changes the coordinates of x1 and x2 only\n";
    cerr << " -mrd 9 changes the reference meridian to 9 deg East\n";
    cerr << " -tc activates the time column in the ascii output; if activated you get a 2 columne file with time and values\n";
    cerr << " -abst nn -tc activates the absolute time (UNIX format, secs. since 1970) in your time column\n";
    cerr << "      nn shifts this time; use -abst -1000000 for example to lower the numbers in your time column\n";
    cerr << "      when you simply want to display almost parallel time series and the absolute date is not needed in the time column\n";
    cerr << " -atsadd adds the given atsfile into the last given new atsfile\n";
    cerr << " -atssub s.o.\n";
    cerr << " -atsmul .. \n";
    cerr << " -atsdiv .. \n";
    cerr << " -spwadd nn adds a value to the spectral output file\n";
    cerr << " -swpmul nn multiplies value to the spectral output file\n";
    cerr << " -median nn returns a median spectra if nn = 0 and uses +-10% of all value AROUND the median if -median 10\n";
    cerr << " -median_coh nn returns median processing for coherencies; use small values like 0.1\n";
    cerr << " -rda sample_freq column year month day hour min sec" << endl;
    cerr << "      example : -rda 512 1 2000 12 24 9 15 0" << endl;
    cerr << "      for a file recorded at Christmas 2000 at breakfast time 9:15 " << endl;
    cerr << "      your 024C01XC.dat will be converted in 024C01XC.ats " << endl;
    cerr << "      hence that MAPROS et al using the filename!! to determine sample frequency, run number, E/H channel" << endl;
    cerr << " -atsoutfile output filename for ascii conversion to ats" << endl;
    cerr << " -merge merges all outputfiles in one ascii file" << endl;
    cerr << "      can not be used with parameters which expect an output filename like atsadd and so on or -nodata" << endl;
    cerr << " -out_dir /wrk/mt/ directs the output to this directory; useful if data is on a CDROM " << endl;
    cerr << " -s_scale [f, sqr(f), sqrt(f), 1/f, 1/sqr(f), 1/sqrt(f)] scales the foward fft; -scale f means spectra is divided by f!" << endl;
    cerr << " -back [scale_f || scale_f2] enables an inverse FFT [devide by f || devide by f square]" << endl;
    cerr << " -fil 32 [-raster -1] or -fil 2, 4, 8, 25 decimates the timeseries with given factor" << endl;
    cerr << " -filw 32 [-raster -1] or -filw 2, 4, 8, 25  decimates the timeseries with given factor and writes new atsfile" << endl;
    cerr << " -nmtx violates the metronix name convention and appends 32x or 4x and so on for the filtred bands" << endl;
    cerr << " -nosystrf disables the automatic calibration of ADB boards for HF or calibration in general" << endl;
    cerr << " -fn2 {1,2, -1, -2} " << endl;
    cerr << " -ll 5 -ul 20 cuts the first 5 (low) and last 20 (high) points of the spectal output" << endl;
    cerr << " -destep int step_len double step_height int relaxation  double step_skewness; refer to manual" << endl;
    cerr << "   tries to fix jumps in the eletrical field caused by poor connections; try 40 0.2 1 0.8 for example" << endl;
    cerr << " -despike shoulder_length spike_height relaxation skewness; refer to manual" << endl;
    cerr << "   tries to extenuate spike caused by electric fences etc.; try 10 0.1 6 1.2 for example" << endl;
    cerr << " -dstw or -dspw will destroy your data (overwrite) together with the destep or despike option" << endl;
    cerr << "    use here -nspw for avoiding ascii output and -tc to plot ascii and spike/step indices together" << endl;
    cerr << " -cat new_run_number <file1.ats> <file2.ats> concatenates 2 atsfiles to a third with increased run number" << endl;
    cerr << " -stddev min_fac max_fac ovr; ovr overlapping of windows in pts; try together with atm and mapros" << endl;
    cerr << " -atm new | merge | cat | use ; [-run nn] new creates or overwrites an existing atm file; merge puts an existing together" << endl;
    cerr << "   fil expects a file .atm.cat with an existing selection done by -cat option of this program\n";
    cerr << " -utm calculates UTM cooridates; togther -mrd n a certain meridian can be forced\n";
    cerr << " -gk calculates Gauss Krueger cooridates; togther -mrd n a certain meridian can be forced\n";
    cerr << " -sync automatically forces all files to use the same start time; can be important for remote referencing\n";
    cerr << " -sync2 .. same as above; stop time also synchronised\n";
    cerr << " -synctime 1002384003 forces all file to start at given .putdatetime(), UNIX time format required\n";
    cerr << " -detrend removes a trend of the time series\n";
    cerr << " -wtrend removes a trend for each given window (option -wl) and if -rect is given\n";
    cerr << " -dateformat ISO -tc -abst nn ; write ISO8601 for XMGRACE as x-axis; tick x-axis format inside xmgrace\n";
    cerr << " -overwrite overwrites existing atsfile " << endl;
    cerr << " -resample new_sample_freq  new sampling frequency " << endl;
    cerr << " -raster nn set start times of filtred band to nn second raster\n";
    cerr << " -calib xx generates a some calibration data, phase multiplied with xx\n";
    cerr << " -ccf cstartx cstarty cshifts tohether -wl; refer to manual" << endl;
    cerr << " -pipe pipes result to std out " << endl;
    cerr << " -lin_conv filter_length frequencies - try a linear convolution; try lin_conv 1024 16.666 50 150" << endl;
    cerr << " -sst n sub stacks in time dommian; -sst 4 -wl 1024 calculates 4 sub stacks of 1024 points in time domain\n";
    cerr << " -noscale_e turns off scaling for E-Field into mv/km\n";
    cerr << " -wbin [float] writes data as binary; output is double (8 byte); if float use 4 byte\n";
    cerr << " -write_trf writes all interpolated transfer functions to disk - avoid to use with -wl 1 (very big file sizes!)\n";
    cerr << " -ovr ovoerlapping in points\n";
    cerr << " -nogrid turns off automatic adjustment of start time to a grid time\n";
    cerr << " -mt for magnetotellurics\n";
    cerr << " -coh for calulating coherencies - best use with sync2 option!\n";
    //        cerr << " -conv2ats \n";
    cerr << " -prz 0.2 gives a parzen radius of 0.2; shoulb be 0.05 ... 0.1, 0.2 ... 0.5\n";
    cerr << " -firfil 471 300 800 0 filters a band pass \n";
    cerr << "    example for 4096Hz and 8193 coeff: 8193 0 1000 0 is low pass, 8193 100 4096 0 is high pass, 8193 100 1000 0 is bandpass, 8193 70 40 0 is notch / bandstop\n";
    cerr << " -online " << endl;
    cerr << " -db_database tsmp connects to MySQL database tsmp" << endl;
    cerr << " -db_host localhost connects to MySQL on the computer where tsmp is running" << endl;
    cerr << " -db_password secret connects to MySQL with password \"secret\"" << endl;
    cerr << " -db_user berni connects to MySQL database as user berni" << endl;
    cerr << " -extra_scale divides by [f, sqr(f), sqrt(f) , 1/f, 1/sqr(f), 1/sqrt(f)] " << endl;
    cerr << " -xmul -xadd : multiply or add a complex to the complex raw spectra; give TWO!! nums at command line!!" << endl;
    cerr << " -flist default.lst (give filename) use only frequencies from this file for spectra" << endl;
    cerr << " -ascii_sst write sub stacked time slices" << endl;
    cerr << " -set_xmlheader try to set a new xml filename for filtered files (not enabled yet)" << endl;
    cerr << " Version 14 August 2016, sclice version / 64bit possibly for ADU-06/07e" << endl;
    cerr << "\n\n";
    return EXIT_FAILURE;
  }

  // above we gave the info; here I must stop!

  // set ctime function to expect a  int which represents GMT or UTC without summer time

std:
  char myenv[80] = "TZ=GMT0GMT";
  if (!putenv(myenv))
    tzset();
  else
    cerr << "no environment free; times will be interpreted wrong" << endl;

  /***************************************************************************
   *                                                                         *
   *         declare all variables                                           *
   *                                                                         *
   *                                                                         *
   *                                                                         *
   *                                                                         *
   ***************************************************************************/

  stringstream convert_num;

  unsigned int nargs, nfiles, nspw = 0;

  unsigned int l, nm = 0, i, j, loops = 1, reduction = 0;

  // vaiables for setting addition, subtraction, multiplication, division, concatenate,
  // correction, time cols, no ascii output and filename two active

  unsigned int lc_filter_len = 0;

  size_t fir_order = 0;     /*! order (length) of the FIR filter*/
  double cutlow = 0;        /*! cut off frequency low*/
  double cuthigh = DBL_MAX; /*! cut off frequency high*/

  // read ascii file

  unsigned int asc_col = 0;       // which columne of file - starting with 0
  unsigned short int chan_no = 0; // channel number

  // command line option control
  bool deselect_samples = false;
  bool select_samples = false;
  int corr = 0;                       /*! header correction on/off*/
  int write_header = 0;               /*! header correction writen to header -> permanent changes */
  int raster = 0;                     /*! sync time series to a given raster - e.g. lowest sampling period*/
  int nogrid = 0;                     /*! if nogrid == 1 filter regardless of start time */
  int dump_trf = 0;                   /*! writes ascii transfer functions to disk which have been applied to the spectra*/
  int gensign = 0, rda = 0;           /*! genrate signal, read ascii */
  double gen_rand = DBL_MAX;          /*! generate standard random singnal and scale */
  int gen_sin = 0;                    /*! generate sinus frequencies */
  int gen_rect = 0;                   /*! generate rectangulars */
  int gen_rand_gsl = 0;               /*! take gsl random generator */
  unsigned int ascii_sst = 0;         /*! wite sub stacked time slices */
  unsigned int ascii_sst_count = 0;   /*! wite sub stacked time slices */
  unsigned int destep = 0, dstw = 0;  /*! activate destep and destep with over-write*/
  unsigned int cut_int = 0;           /*! cut integers directly from ats file */
  unsigned int despike = 0, dspw = 0; /*! activate despike and despike with over-write*/
  unsigned int rmtrend = 0;           /*! use a global detrend (e.g. fluxgate data)*/
  unsigned int wdetrend = 0;          /*! use a local detrend, e.g. for each window together with -rect*/
  unsigned int back = 0;              /*! backwards FFT*/
  unsigned int calc_median = 0;       /*! activate median processing for FFT*/
  unsigned int median_coh = 0;        /*!      and coherency*/
  unsigned int lin_conv = 0;          /*! activate linear convolution*/
  unsigned int mc = 0;                /*! *.ats files will be merged into ONE .dat file*/
  unsigned int site = 0;              /*! read a complete ats set according to a site or 5 channel GOE file*/
  unsigned int conv = 0;              /*! convert GOE file into ats files*/
  unsigned int calc_utm = 0;          /*! calculate UTM coordinates (use refmed!)*/
  unsigned int calc_gk = 0;           /*! calculate Gauss Krueger must use meridian*/
  unsigned int last_century = 0;      /*! assume year 2000+ for goe files*/
  unsigned int allchan = 0;           /*! total number of channels used*/
  unsigned int sync = 0;              /*! read all files and start at  the same absolute time*/
  unsigned int sync2 = 0;             /*! same for stop times if required*/
  unsigned int overwrite = 0;         /*! allow atsview to overwrite existing data*/
  unsigned int do_ccf = 0;            /*! compute cross correlation (slow)*/
  unsigned int resample = 0;          /*! if sampling freq is > 1 Hz (e.g 64Hz) and you want 25 Hz*/
  unsigned int noscale_e = 0;         /*! otherwise E-Field set to mv/km (divided by dipole length)*/
  unsigned int usefloat = 0;          /*! try to use float as binary output instead of double*/
  unsigned int wbin = 0;              /*! write all channels subsequently a1 b1 c1 a2 b2 c2 ...*/
  unsigned int write_trf = 0;         /*! writes all transferfunctions to disk*/
  unsigned int mt = 0;                /*! magnetotellurics*/
  unsigned int coh = 0;               /*! coherency between two channels */
  unsigned int write_atm = 0;         /*! we can only write the atm file in total so -all must have been used */
  unsigned int firfil = 0;            /*! applies a finite response filter (lp, hp, bad pass band stop) to time series*/
  size_t firfil_reduct = 0;           /*! possible reduction with this fir filter */
  unsigned int use_flist = 0;         /*! write only these frequencies */
  int set_xmlheader = 0;              /*! try to write a new xml header name after filtering */
  long int UTCGPSOffset = 0;          /*! offset between UTC and GPS - mostly never used */

  size_t start_rectpulse = 0; /*! start sample of rect pulse (together with gensign) */
  size_t stop_rectpulse = 0;  /*! stop sample of rect pulse (together with gensign) */
  double rectpulse_val = 0.;  /*! rect pulse value (together with gensign) */

  string flist_name; /*! read frequenvies for spectra output */
  valarray<double> flist;
  int invfft_zeropadding = 0; /*! prepare for full inversion - e.g. to get real ts data from coils */

  vector<long int> sync_dates_start, sync_dates_stop; /*! contains the absolute start, stop time data*/
  vector<size_t> vstart, vsamples, vtotalsmpls;       /*! contains the individual start/used/total samples for sync option*/
  unsigned long int synctime = 0;                     /*! for given synctime; together with -sync*/
  vector<double> gensign_params;                      /*! list of frequencies for sinus generator */
  vector<double> noise_mix;                           /*! mix levers of noise for mt noise; e.g 4 numbers are required */
  vector<string> extra_scale;                         /*! strings with extra scales for the forward fft .. like f or sqrt(f) etc */

  vector<unsigned int> chan(0, 1);

  string run = "",       /*! read run number from command line*/
      dateformat,        /*! can be set to ISO 8601*/
      ascii_outget_type; /*! types are data, spikes, step, stddev*/
  // file handling
  int fn2 = 0;
  int last_file_is_result = 0; /*! if output results into a NEW ats file*/
  unsigned int do_atm = 0;     /*! 0 no,  1 create /overwrite; 2 merge; 3 merge from a file .atm.cat*/
  vector<bool> newselect;      /*! IO vector for atm files*/

  // values for changing header entries
  // general: where we have to SET a variable we use INT_MAX NOT to do it
  //          where we have to add, multiply or CHANGE we use 0 (ZERO) not to do it.
  // position
  int log = INT_MAX; /*! readable coodinates, grad */
  int lom = INT_MAX;
  int lag = INT_MAX;
  int lam = INT_MAX;
  double las = DBL_MAX;
  double los = DBL_MAX;
  string LatH = "x";
  string LonH = "x";

  int ref_med = INT_MAX; /*! INT_MAX means: don't want to change*/

  // time
  long time_sec = LONG_MAX;  /*! shift ascii file output with nn seconds;*/
  double iso_sec = DBL_MAX;  /*! shift ISO date - possible duble values!*/
  long datetime = LONG_MAX;  /*! set absolute time of header */
  size_t samples = LONG_MAX; /* will be used for changing samples inside header */
  // also used to shift header .putdatetime() entry
  // most common for the 13s problem
  long gmt_offset = LONG_MAX;                 /*! seconds from GMT; Berlin = 3600s at winter time*/
  double d_start = DBL_MAX, d_stop = DBL_MAX; /*! correction time for start and stop*/

  unsigned relative_time = 1; /*! do NOT use the long date in ascii time series output*/
  long time_offset = 0;       /*! together with -abst you can remove leading 10+e9*/
  // seconds and so on; abst sets relative_time to 0

  double parzen = 0.; /*! parzen radius */

  double SampleFreq = DBL_MAX;
  double rsmpl_freq = DBL_MAX; /*! new sampling frequency from resample option*/
  double elevation = DBL_MAX;  /*! new elevation in meter  */
  double lsbval = DBL_MAX;     /*! new lsb value */
  bool fliplsbval = false;     /*! multiply LSB with -1 */

  tm read_t;                  /*! for using  time functions in main*/
  time_t ats_time = LONG_MAX; /*! for using  time functions in main and read ascii*/

  // E-field

  double angle = DBL_MAX, diplen = DBL_MAX; /*! DBL_MAX means: don't want to change*/
  double eprobe_res = DBL_MAX, dcoffset = DBL_MAX;

  valarray<double> senspos(DBL_MAX, 6);
  // x1, y1, z1, x2, y2, z2; senspos[i] =  DBL_MAX means no changes

  // ADU specific
  string SerNoADU;     /*! that is the system_id */
  string SerNoADB;     /*! that is the board_id  */
  string SensorNo;     /*! that is the sensor id */
  string achxmlheader; /*! info for haeder */
  string comments;     /*! comments */
  string meas_type;    /*! MT, AMT, CSAMT, CSEMM etc. */
  string atsoutfile;   /*! ats filename used for conversion from ascii to ats */

  // used for changing header entries
  string channel_type, sensor_type; /*! Hx, Hy, Hz, Ex, Ey; MFS06, FGS01 etc.*/
  string system_name;               /*! want to change the system name ADU06, MMS03e */
  // string are empty and shoud stay so except you want
  // change header entries
  // output
  int nodata = 0, ascii = 0; /*! no data section, no ascii output*/
  int ts2mysql = 0;          /*! write time series to mysql db*/
  int ncall = 0;             /*! counter for external calls - eg. fro truncate options */
  int truncate = 0;          /*! truncate corresponding tables*/
  int tc = 0;                /*! time column in ascii output on/off*/
  int nspc = 0;              /*! supress all spectr calculation */

  // database handling
  string db_database = "tsdata";     /*! database name */
  string db_host = "localhost";      /*! database host; eg. localhost */
  string db_user = "tsdata";         /*! database login */
  string db_password = "tsmptsdata"; /*! database password */

  // data handling
  size_t start = 0, stop = 0, used_samples = 0, last_samples = 0, slice = 0;
  QString isostart;
  QString isostop;

  eDateTime edate;
  eDateTime start_date;
  eDateTime stop_date;

  // data functions  (also for activating the function if value is not 0
  int atsadd = 0, atssub = 0, atsmul = 0, atsdiv = 0, atscat = 0;         /*! adding ... ats files TOGETHER*/
  int spectra_add = 0, spectra_sub = 0, spectra_mul = 0, spectra_div = 0; //!< adding spectra together
  double add = 0, mul = 0;                                                /*! adding a number to the data vector*/

  // transfer functions
  unsigned int auto_trf = 0, theo_trf = 0, hw_trf = 0;
  double calib = 0;
  string sensorcalfile; /*! leave empty*/
  string systemcalfile; /*! leave empty*/

  // filtering
  string nmtx, bandstr; /*! nmtx will violate the mtx naming convention*/
  // banstring will attach a 4 or 32
  unsigned int write_ats = 0; /*! write filtred values to disk*/
  int filter_length = 0;      /*! filter length will cause time offset after filtering*/

  // data handling and FFT
  unsigned int hanning = 1;    /*! hanning window is defaultb instead of recangular*/
  unsigned int no_dc_fft = 1;  /*! skip the dc-part of FFT; do not use with FFT-BACK*/
  unsigned int nosystrf = 0;   /*! 0 = activate internal transfer function for ADU-06*/
  int inverse_trf = 0;         /*! instead of dividing multiply with transfer function */
  unsigned int wl = 0;         /*! window length of FFT; also used for activating FFT section*/
  unsigned int ul = 0, ll = 0; /*! upper limit lower limit of ascii ouput of FFT*/
  /*! ll, ul 20 = skip the first, last 20 points from output*/
  unsigned int sst = 0;                  /*! sub stacks in time domain*/
  unsigned int scale_f = 0;              /*! scale spectra by /f before backwards time series conversion*/
  size_t ccf_startx = 0, ccf_starty = 0; /*! starting points for cross correlation*/
  size_t ccf_shifts = 0;                 /*! how many ccf's will be calculted */

  double range = 0, range_coh = 0; /*! ranges around median*/
  double spwadd = 0, spwmul = 1;   /*! adding and multiplying spectral OUTPUT files (not the data itself!)*/
  complex<double> xadd = complex<double>(0, 0);
  complex<double> xmul = complex<double>(0, 0);

  // spwadd -90 subtracts 90 degrees for expample from phase
  // spwmul  180./M_PI makes rad to grad if phase was selected
  // hence that this applies to all spectra files....

  // time series processing, destep, despike, std. dev. proc

  size_t ovr = 0; /*! overlay of windows for std deviation */
  unsigned int step_len = 0, shoulder_len = 0, relaxation = 0, do_stddev = 0;
  string new_run_number;
  double step_skewness = 0, step_height = 0, spike_height = 0, spike_skewness = 0,
         min_stddev = DBL_MAX, max_stddev = DBL_MAX;
  double max_min = DBL_MAX; /*! used for max min analysis */
  size_t max_min_length;    /*! length of segment to test */

  valarray<double> vx, vy, vz, ccfxy;
  string fname, f2name, temp_str, out_dir, my_dir;
  string trf_file, trf_file1, trf_file2;
  vector<double> lc_target_f;

  /***************************************************************************
   *                                                                         *
   *                                                                         *
   *       scan comand line options                                          *
   *                                                                         *
   *                                                                         *
   *                                                                         *
   ***************************************************************************/

  l = 1;
  while (argc > 1 && (l < unsigned(argc)) && *argv[l] == '-') {

    if (!strcmp(argv[l], "-all")) {
      used_samples = UINT_MAX;
    } else if (!strcmp(argv[l], "-overwrite")) {
      overwrite = 1;
    } else if (!strcmp(argv[l], "-chan")) {
      chan.resize(0);
      ++l;
      // isdigit
      while (!strstr(argv[l], "-") && !strstr(argv[l], ".")) {
        chan.push_back(atoi(argv[l]));
        ++l;
      }
      --l;
    } else if (!strcmp(argv[l], "-conv")) {
      conv++;
    } else if (!strcmp(argv[l], "-max_min")) {
      max_min = atof(argv[++l]);
      max_min_length = atoi(argv[++l]);
    } else if (!strcmp(argv[l], "-run")) {
      run = argv[++l];
      //        } else if (!strcmp(argv[l], "-mt")) {
      //            mt = 1;
      //        } else if (!strcmp(argv[l], "-last_century")) {
      //            last_century = 1;
    } else if (!strcmp(argv[l], "-raster")) {
      raster = atoi(argv[++l]);
    } else if (!strcmp(argv[l], "-nogrid")) {
      nogrid = 1;
    } else if (!strcmp(argv[l], "-calib")) {
      calib = atof(argv[++l]);
    } else if (!strcmp(argv[l], "-cut_int")) {
      cut_int = 1;
      noscale_e = 1;
    } else if (!strcmp(argv[l], "-select")) {
      select_samples = true;
    } else if (!strcmp(argv[l], "-deselect")) {
      deselect_samples = true;
      //        } else if (!strcmp(argv[l], "-prz")) {
      //            parzen = atof(argv[++l]);
    } else if (!strcmp(argv[l], "-nspc")) {
      nspc = 1;
    } else if (!strcmp(argv[l], "-firfil")) {
      firfil = 1;
      fir_order = atoi(argv[++l]);
      cutlow = atof(argv[++l]);
      cuthigh = atof(argv[++l]);
      firfil_reduct = (unsigned)atoi(argv[++l]);
    } else if (!strcmp(argv[l], "-sync")) {
      sync = 1;
    } else if (!strcmp(argv[l], "-sync2")) {
      sync = 2;
      sync2 = 1;
    } else if (!strcmp(argv[l], "-synctime")) {
      sync = 1;
      synctime = atol(argv[++l]);
    } else if (!strcmp(argv[l], "-add")) {
      if (!strcmp(argv[++l], "mean"))
        add = DBL_MAX;
      else
        add = atof(argv[l]);
    } else if (!strcmp(argv[l], "-mul")) {
      if (!strcmp(argv[++l], "mean"))
        mul = DBL_MAX;
      else
        mul = atof(argv[l]);
    } else if (!strcmp(argv[l], "-time_s")) {
      time_sec = atol(argv[++l]);

    } else if (!strcmp(argv[l], "-iso_s")) {
      iso_sec = atof(argv[++l]);

    } else if (!strcmp(argv[l], "-spwadd")) {
      spwadd = atof(argv[++l]);

    } else if (!strcmp(argv[l], "-spwmul")) {
      spwmul = atof(argv[++l]);
    } else if (!strcmp(argv[l], "-utm")) {
      calc_utm = 1;
      //        } else if (!strcmp(argv[l], "-gk")) {
      //            calc_gk = 1;
    } else if (!strcmp(argv[l], "-ascii_sst")) {
      ascii_sst = 1;
    } else if (!strcmp(argv[l], "-gensign")) {
      gensign = 1;
    } else if (!strcmp(argv[l], "-gen_rand")) {
      gen_rand = atof(argv[++l]);
    } else if (!strcmp(argv[l], "-s_scale")) {
      l++;
      while ((l < unsigned(argc)) && (*argv[l] != '-') && !strstr(argv[l], ".ats")) {
        cout << "getting  " << argv[l] << endl;
        extra_scale.push_back(argv[l++]);
      }
      l--;
    } else if (!strcmp(argv[l], "-gen_rand_gsl")) {
      gen_rand_gsl = 1;
      l++;
      while ((l < unsigned(argc)) && (*argv[l] != '-') && !strstr(argv[l], ".ats")) {
        cout << "getting  " << argv[l] << endl;
        noise_mix.push_back(atof(argv[l++]));
        // Ex Ey Hx Hy Hz
      }
      l--;

    } else if (!strcmp(argv[l], "-gen_sin")) {
      gen_sin = 1;
      l++;
      while ((l < unsigned(argc)) && (*argv[l] != '-') && !strstr(argv[l], ".ats")) {
        cout << "getting  " << argv[l] << endl;
        gensign_params.push_back(atof(argv[l++]));
      }
      l--;

    } else if (!strcmp(argv[l], "-gen_rect")) {
      gen_rect = 1;
      l++;
      while ((l < unsigned(argc)) && (*argv[l] != '-') && !strstr(argv[l], ".ats")) {
        cout << "getting  " << argv[l] << endl;
        gensign_params.push_back(atof(argv[l++]));
      }
      l--;

    } else if (!strcmp(argv[l], "-with_rectpulse")) {
      start_rectpulse = atoi(argv[++l]);
      stop_rectpulse = atoi(argv[++l]);
      rectpulse_val = atof(argv[++l]);

    } else if (!strcmp(argv[l], "-rect")) {
      hanning = 0;
    } else if (!strcmp(argv[l], "-nodata")) {
      nodata = 1;
      // data section of ats is not read
    } else if (!strcmp(argv[l], "-ascii")) {
      ascii = 1; // data section is written but most ascii output supressed
                 // data section of ats is not read
    } else if (!strcmp(argv[l], "-ts2mysql")) {
      ts2mysql = 1;
      if (!strcmp(argv[l + 1], "truncate")) {
        truncate = 1; // clean tables
        ++l;
      }
      if (!strcmp(argv[l + 1], "truncate_first")) {
        truncate = 2; // clean tables
        ++l;
      }
      // solution for PHP and using select drop down boxes -> ts2mysql must have a pre-set value
      if (!strcmp(argv[l + 1], "off")) {
        ts2mysql = 0;
        ++l;
      }
      if (!strcmp(argv[l + 1], "on")) {
        ts2mysql = 1;
        ++l;
      }

    } else if ((!strcmp(argv[l], "-ncall"))) {
      ncall = atoi(argv[++l]);

    } else if ((!strcmp(argv[l], "-utctogps"))) {
      UTCGPSOffset = atoi(argv[++l]);

    } else if ((!strcmp(argv[l], "-lsbval"))) {
      lsbval = atof(argv[++l]);

    } else if ((!strcmp(argv[l], "-set_xmlheader"))) {
      set_xmlheader = 1;
      ++l;

    } else if ((!strcmp(argv[l], "-fliplsbval"))) {
      fliplsbval = true;

    } else if (!strcmp(argv[l], "-nspw")) {
      nspw = 1; // if spectra calculation is activated
                // data section is read but not written
                // to disk
    } else if (!strcmp(argv[l], "-back")) {
      back = 1;
      ++l; // backward FFT
      if (!strcmp(argv[l], "scale_f"))
        scale_f = 1; // scale back spectra by /f before conversion
      else if (!strcmp(argv[l], "scale_f2"))
        scale_f = 2; // scale back spectra by /(f*f) before conversion
      else
        --l;
      //        } else if (strstr(argv[l], "evaluated iso_s")) {
      //            start = atoi(argv[++l]);
    } else if (strstr(argv[l], "-start")) {
      start = atoi(argv[++l]);

    } else if (strstr(argv[l], "-slice")) {
      slice = atoi(argv[++l]);

    } else if (!strcmp(argv[l], "-use")) {
      used_samples = atoi(argv[++l]);
    } else if (!strcmp(argv[l], "-last")) {
      last_samples = atoi(argv[++l]);
    } else if (!strcmp(argv[l], "-stop")) {
      stop = unsigned(atoi(argv[++l]));
      stop++; // because of C notation!
    } else if (strstr(argv[l], "-isostart")) {
      isostart = argv[++l];
      start_date.set_ISOTime(isostart);
    } else if (strstr(argv[l], "-isostop")) {
      isostop = argv[++l];
      stop_date.set_ISOTime(isostop);
    } else if (!strcmp(argv[l], "-wtrend")) {
      wdetrend = 1;
    } else if (!strcmp(argv[l], "-wl")) {
      wl = unsigned(atoi(argv[++l]));
    } else if (!strcmp(argv[l], "-ovr")) {
      ovr = unsigned(atoi(argv[++l]));
    } else if (!strcmp(argv[l], "-trf")) { // use strcmp because strstr returns true if argv was trf1
      if (!strcmp(argv[l + 1], "auto")) {
        auto_trf = 1; // automatic detection from atsheader, read from file
        ++l;
      } else if (!strcmp(argv[l + 1], "theo")) {
        theo_trf = 1; // automatic detection from atsheader,
        ++l;          //  theoretical cal functio is used
      } else if (!strcmp(argv[l + 1], "hw")) {
        hw_trf = 1; // use cal function of MFS-05 No. 143
        ++l;        // or
      } else
        trf_file = argv[++l];
      // cerr << trf_file << "!!" << endl;
      //        } else if (!strcmp(argv[l], "-trf1")) {
      //            trf_file1 =  argv[++l];
      //            auto_trf = 0;
      //        } else if (!strcmp(argv[l], "-trf2")) {
      //            trf_file2 =  argv[++l];
      //            auto_trf = 0;

    } else if (!strcmp(argv[l], "-dump_trf")) {
      dump_trf = 1;

    } else if (!strcmp(argv[l], "-out_dir")) {
      out_dir = argv[++l];
    } else if (!strcmp(argv[l], "-latlon")) {
      lag = atoi(argv[++l]);
      lam = atoi(argv[++l]);
      las = atof(argv[++l]);

      LatH = argv[++l];

      log = atoi(argv[++l]);
      lom = atoi(argv[++l]);
      los = atoi(argv[++l]);

      LonH = argv[++l];

    } else if (!strcmp(argv[l], "-mrd")) {
      ref_med = atoi(argv[++l]);
    } else if (!strcmp(argv[l], "-elev")) {
      elevation = atof(argv[++l]);
    } else if (!strcmp(argv[l], "-datetime")) {
      datetime = atoi(argv[++l]);
    } else if (!strcmp(argv[l], "-samples")) {
      samples = atoi(argv[++l]);
    } else if (!strcmp(argv[l], "-gmtoff")) {
      gmt_offset = atoi(argv[++l]);
    } else if (!strcmp(argv[l], "-angle")) {
      angle = atof(argv[++l]);
    } else if (!strcmp(argv[l], "-diplen")) {
      diplen = atof(argv[++l]);
    } else if (!strcmp(argv[l], "-resis")) {
      eprobe_res = atof(argv[++l]);
    } else if (!strcmp(argv[l], "-dcoffset")) {
      dcoffset = atof(argv[++l]);
    }

    // use this for spectra
    else if (!strcmp(argv[l], "-median")) {
      calc_median = 1;
      range = atof(argv[++l]);
    } else if (!strcmp(argv[l], "-median_coh")) {
      median_coh = 1;
      range_coh = atof(argv[++l]);
    } else if (!strcmp(argv[l], "-sensorcal")) {
      sensorcalfile = argv[++l];
      if (sensorcalfile.size() > 12) {
        cerr << "\nsorry, inside ats file format only 12 chars (8.3) are allowed for cal file name\n";
        return EXIT_FAILURE;
      }
    } else if (!strcmp(argv[l], "-systemcal")) {
      systemcalfile = argv[++l];
      if (systemcalfile.size() > 12) {
        cerr << "\nsorry, inside ats file format only 12 chars (8.3) are allowed for cal file name\n";
        return EXIT_FAILURE;
      }
    } else if (!strcmp(argv[l], "-sensor_type")) {
      sensor_type = argv[++l];
      if (sensor_type.size() > 6) {
        cerr << "\nsorry, inside ats file format only 6 chars are allowed for Sensor type\n";
        return EXIT_FAILURE;
      }
    } else if (!strcmp(argv[l], "-channel_type")) {
      channel_type = argv[++l];
      if (channel_type.size() > 2) {
        cerr << "\nsorry, inside ats file format only 2 chars are allowed for ChannelType (Hx, ..Ey)\n";
        return EXIT_FAILURE;
      }
    } else if (!strcmp(argv[l], "-system_name")) {
      system_name = argv[++l];
    } else if (!strcmp(argv[l], "-channelno")) {
      chan_no = (unsigned short)(atoi(argv[++l]));
    } else if (!strcmp(argv[l], "-adbser")) {
      SerNoADB = argv[++l];
    } else if (!strcmp(argv[l], "-aduser")) {
      SerNoADU = argv[++l];
    } else if (!strcmp(argv[l], "-sensser")) {
      SensorNo = argv[++l];
    } else if (!strcmp(argv[l], "-samplefreq")) {
      SampleFreq = atof(argv[++l]);
    } else if (!strcmp(argv[l], "-corr")) {
      corr = 1; // correct header
    } else if (!strcmp(argv[l], "-wrh")) {
      write_header = 1; // correct header permanently

    } else if (!strcmp(argv[l], "-tc")) {
      tc = 1; // write time and values in file

    } else if (!strcmp(argv[l], "-ccf")) { // cross correlation

      ccf_startx = abs(atoi(argv[++l]));
      ccf_starty = abs(atoi(argv[++l]));
      ccf_shifts = abs(atoi(argv[++l]));
      do_ccf = 1;
    } else if (!strcmp(argv[l], "-lin_conv")) {
      lin_conv = 1;
      lc_filter_len = (unsigned int)atoi(argv[++l]);
      l++;
      while ((l < unsigned(argc)) && (*argv[l] != '-') && !strstr(argv[l], ".ats")) {
        lc_target_f.push_back(atof(argv[l++]));
      }
      l--;
    } else if (!strcmp(argv[l], "-merge")) {
      mc = 1;
      last_file_is_result++;
    } else if (!strcmp(argv[l], "-atsadd")) {
      atsadd = 1;
      last_file_is_result++;
    } else if (!strcmp(argv[l], "-atssub")) {
      atssub = 1;
      last_file_is_result++;
    } else if (!strcmp(argv[l], "-atsmul")) {
      atsmul = 1;
      last_file_is_result++;
    } else if (!strcmp(argv[l], "-atsdiv")) {
      atsdiv = 1;
      last_file_is_result++;

    } else if (!strcmp(argv[l], "-spectra_add")) {
      spectra_add = 1;

    } else if (!strcmp(argv[l], "-spectra_sub")) {
      spectra_sub = 1;

    } else if (!strcmp(argv[l], "-spectra_mul")) {
      spectra_mul = 1;

    } else if (!strcmp(argv[l], "-spectra_div")) {
      spectra_div = 1;

    } else if (!strcmp(argv[l], "-spos")) {
      for (i = 0; i < senspos.size(); i++) {
        if (!strstr(argv[++l], "n"))
          senspos[i] = atof(argv[l]);
      }
    }
    // reads NOT in C notation that means first col is 1 !!!!
    else if (!strcmp(argv[l], "-rda")) {
      rda = 1;
      SampleFreq = atof(argv[++l]);
      asc_col = atoi(argv[++l]);
      read_t.tm_year = atoi(argv[++l]) - 1900;
      read_t.tm_mon = atoi(argv[++l]) - 1;
      read_t.tm_mday = atoi(argv[++l]);
      read_t.tm_hour = atoi(argv[++l]);
      read_t.tm_min = atoi(argv[++l]);
      read_t.tm_sec = atoi(argv[++l]);

#ifndef _msvc
      read_t.tm_gmtoff = 0;
#endif
      read_t.tm_isdst = 0;
      ats_time = mktime(&read_t);
      if (ats_time == -1) {
        cerr << "format must be: -rda sample_freq column year month day hour min sec" << endl;
        cerr << "example : -rda 512 1 2000 12 24 9 15 0" << endl;
        cerr << "for a file recorded at Christmas 2000 at breakfast time 9:15 with 512 Hz sampling rate" << endl;
        cerr << "your column 1 of 024C01XF.dat will be converted in 024C01XF.ats " << endl;
        cerr << "hence that MAPROS et al using the filename!! to determin sample frequency," << endl;
        cerr << "run number, E/H channel" << endl;
        return EXIT_FAILURE;
      }

    } else if (!strcmp(argv[l], "-atsoutfile")) {
      atsoutfile = argv[++l];

    } else if (!strcmp(argv[l], "-fil")) {
      reduction = atoi(argv[++l]);
      bandstr = argv[l];                    // the 4 or 32 will be saved here
    } else if (!strcmp(argv[l], "-filw")) { // overwites the F band!!!
      reduction = atoi(argv[++l]);
      bandstr = argv[l]; // the 4 or 32 will be saved here
      write_ats = 1;
    } else if (!strcmp(argv[l], "-nmtx")) {
      nmtx = argv[l];
    } else if (!strcmp(argv[l], "-nosystrf")) {
      nosystrf = 1;
    } else if (!strcmp(argv[l], "-inverse_trf")) {
      inverse_trf = 1;
    } else if (!strcmp(argv[l], "-fn2")) {
      fn2 = atoi(argv[++l]);
    } else if (!strcmp(argv[l], "-ul")) {
      ul = (unsigned)atoi(argv[++l]);
    } else if (!strcmp(argv[l], "-ll")) {
      ll = (unsigned)atoi(argv[++l]);
    } else if (!strcmp(argv[l], "-destep")) {

      step_len = (unsigned)atoi(argv[++l]);
      step_height = atof(argv[++l]);
      relaxation = (unsigned)atoi(argv[++l]);
      step_skewness = atof(argv[++l]);
      destep = 1;
    } else if (!strcmp(argv[l], "-despike")) {
      shoulder_len = (unsigned)atoi(argv[++l]);
      spike_height = atof(argv[++l]);
      relaxation = (unsigned)atoi(argv[++l]);
      spike_skewness = atof(argv[++l]);
      despike = 1;
    } else if (!strcmp(argv[l], "-dstw")) {
      dstw = 1;
    } else if (!strcmp(argv[l], "-dspw")) {
      dspw = 1;
    } else if (!strcmp(argv[l], "-cat")) {
      new_run_number = argv[++l];
      if (new_run_number.size() < 2) {
        // cerr << "use -cat new_run_number; run_number > 0" << endl;
        return EXIT_FAILURE;
      }
      atscat = 1;
    } else if (!strcmp(argv[l], "-abst")) {
      relative_time = 0;
      time_offset = atol(argv[++l]);
    } else if (!strcmp(argv[l], "-dcfft")) {
      no_dc_fft = 0; // including the DC parts mean for i = no_dc_fft; i < ...; i++
    } else if (!strcmp(argv[l], "-stddev")) {
      do_stddev = 1;
      min_stddev = atof(argv[++l]);
      max_stddev = atof(argv[++l]);
      ovr = atoi(argv[++l]);
    } else if (!strcmp(argv[l], "-detrend")) {
      rmtrend = 1;
    } else if (!strcmp(argv[l], "-dateformat")) {
      dateformat = argv[++l];
    } else if (!strcmp(argv[l], "-resample")) {
      resample = 1;
      rsmpl_freq = atof(argv[++l]);
    } else if (!strcmp(argv[l], "-sst")) {
      sst = atoi(argv[++l]);
    } else if (!strcmp(argv[l], "-atm")) {
      ++l;
      if (!strcmp(argv[l], "new"))
        do_atm = 1;
      if (!strcmp(argv[l], "merge"))
        do_atm = 2;
      if (!strcmp(argv[l], "cat"))
        do_atm = 3;
      if (!strcmp(argv[l], "use"))
        do_atm = 4;
      else {
        cerr << "\nunrecognized option -atm " << argv[l] << endl;
        return EXIT_FAILURE;
      }
    } else if (!strcmp(argv[l], "-noscale_e")) {
      noscale_e = 1;

    } else if (!strcmp(argv[l], "-write_trf")) {
      write_trf = 1;

    } else if (!strcmp(argv[l], "-wbin")) {
      wbin = 1;
      if (!strcmp(argv[++l], "float"))
        usefloat = 1;
      else
        --l;
      last_file_is_result = 1;

      //        } else if (!strcmp(argv[l], "-db_user")) {
      //            db_user = argv[++l];
      //        } else if (!strcmp(argv[l], "-db_password")) {
      //            db_password = argv[++l];
      //        } else if (!strcmp(argv[l], "-db_database")) {
      //            db_database = argv[++l];
      //        } else if (!strcmp(argv[l], "-db_host")) {
      //            db_host = argv[++l];

    } else if (!strcmp(argv[l], "-flist")) {
      use_flist = 1;
      flist_name = argv[++l];

    } else if (strstr(argv[l], "-")) {
      cerr << "\nunrecognized option " << argv[l] << endl;
      return EXIT_FAILURE;
    }

    l++;
  }

  // wemn l = 1 ...? ohne argumente alles konvertieren !!!!!!!!!!!!!!!!
  nargs = l - 1; // counter for the arguments

  nfiles = argc - l; // counter for files including possible result file
  string last_file_result;
  if (last_file_is_result) {

    last_file_result = argv[argc - 1];
    if (last_file_result.size() < 2) {
      cerr << "main-> I need a final output file! Please provide a name with at least two characters" << endl;
      exit(0);
    }
  }
  int ubfiles = 0, gfiles = 0, afiles = 0, xfiles = 0, efiles = 0;

  /***************************************************************************
   *                                                                         *
   * end command line options                                                *
   *                                                                         *
   *                                                                         *
   * try to set up the correct channels                                      *
   *                                                                         *
   ***************************************************************************/

  // count ATS, Goettingen and EMI files
  for (l = nargs + 1; l < unsigned(argc); l++) {
    if (strstr(argv[l], ".goe") || strstr(argv[l], ".GOE"))
      gfiles++;
    else if (strstr(argv[l], ".r") || strstr(argv[l], ".R"))
      ubfiles++;
    else if (strstr(argv[l], ".ats") || strstr(argv[l], ".ATS"))
      afiles++;
    else if (strstr(argv[l], ".t02") || strstr(argv[l], ".T02"))
      efiles++;
    else
      xfiles++;
  }

  // only Goettingen is assumed to have more than one challe per file (like 03e format
  if (chan.empty())
    allchan = afiles + xfiles + efiles;
  else
    allchan = afiles + gfiles * chan.size() + ubfiles * chan.size() + xfiles;

  // and inside a multi channel file you MUST provide a channel number - starting with 0
  if (chan.empty() && gfiles) {
    cerr << "together with Goettingen format files (.goe, .GOE, .rnn) you MUST provide a channel number" << endl;
    cerr << " e.g. -chan 0 or -chan 0 1 2 for the first three channels; max 7! .. exit!" << endl;
    exit(0);
  }
  if (chan.empty() && ubfiles) {
    cerr << "together with Goettingen format files (.goe, .GOE, .rnn) you MUST provide a channel number" << endl;
    cerr << " e.g. -chan 0 or -chan 0 1 2 for the first three channels; max 7! .. exit!" << endl;
    exit(0);
  }

  // check the inegrety of the command line args

  // read ascii
  if (rda) {

    if (!atsoutfile.size())
      cerr << "main ->  -rda option need -atsoufile " << endl;
    if (!corr)
      cerr << "main -> -rda option needs -corr " << endl;
    if (!write_header)
      cerr << "main -> -rda option needs -wrh " << endl;

    if (!write_header || !corr || !atsoutfile.size())
      exit(0);
  }

  if (mt && !parzen) {
    cerr << "\nif you want MT (-mt) you must apply at least -prz 0.01 for parzen radius";
    exit(0);
  }
  if ((used_samples == UINT_MAX) && stop) {
    cerr << "\nuse either -start -use or -start -stop or -start -all\n";
    return EXIT_FAILURE;
  }
  if (((last_samples) && stop) || ((last_samples) && start) || ((last_samples) && start) || ((last_samples) && used_samples)) {
    cerr << "\nuse either -start -use or -start -stop or -start -all OR -last\n";
    return EXIT_FAILURE;
  }

  if ((auto_trf || theo_trf) && (trf_file1.size() || trf_file2.size())) {
    cerr << "\nuse either -trf auto, -trf theo, -trf filename,  no parameter or -trf1 filename -trf2 filename" << endl;
    return EXIT_FAILURE;
  }

  if ((trf_file1.size() || trf_file2.size()) && nfiles > 2) {
    cerr << "the option -trf1 -trf2 is made for reading in two files only" << endl;
    return EXIT_FAILURE;
  }

  if (reduction && nodata) {
    cerr << "-fil and -nodata not possible; use -fil xx -nspw if you want to create new ats file only" << endl;
    return EXIT_FAILURE;
  }

  if (wl && nodata) {
    cerr << "spectra and -nodata not possible; use -nspw if don not you want to write ascii time series" << endl;
    return EXIT_FAILURE;
  }

  if (despike && destep) {
    cerr << "use either destep or despike" << endl;
    return EXIT_FAILURE;
  }
  if (do_stddev && !wl) {
    cerr << "if you use standard deviation analysis you must provide a window length (-wl) as well" << endl;
    cerr << "usage -stdev 0.3 4 64 -wl 128 for 128 windows and 64 samples overlapping " << endl;
    return EXIT_FAILURE;
  }

  if (do_stddev) {
    if (min_stddev == DBL_MAX || max_stddev == DBL_MAX || ovr > wl) {
      cerr << "usage -stdev 0.3 4 64 -wl 128 for 128 windows and 64 samples overlapping " << endl;
      cerr << "you must give also a min and a max of used standard deviations" << endl;
      cerr << "overlay of windows can not be bigger than window" << endl;
      cerr << "parameter: " << min_stddev << "  " << max_stddev << " " << ovr << "  " << wl << endl;
      return EXIT_FAILURE;
    }
  }
  if (out_dir.size() && out_dir[out_dir.size() - 1] != '/')
    out_dir.append("/");

  if (nfiles > 1 && fn2) {
    cerr << "use the -fn2 option only together with one file," << endl;
    cerr << "example -fn2 1 024C01XD.ats will read 024C01XD.ats AND  024D01YD.ats " << endl;
    cerr << "only useful together with the UNIX find command" << endl;
    return EXIT_FAILURE;
  }

  if (run.size() && run.size() != 2) {

    cerr << "run must be exactly between 01, 02 ..91 - please use two digits!" << endl;

    return EXIT_FAILURE;
  }

  if (SerNoADU.size() && SerNoADU.size() != 3) {

    cerr << "serial number of ADU must be exactly between 001, 02 ..999 - please use three digits!" << endl;
    return EXIT_FAILURE;
  }

  if (!run.size() && conv) {

    cerr << "for file conversion you muste provide a run number exactly between 01, 02 ..99";
    return EXIT_FAILURE;
  }

  /***************************************************************************
   *                                                                         *
   *                                                                         *
   *   extending the command line argument with a generated                  *
   *   new file named fn2                                                    *
   *                                                                         *
   *                                                                         *
   ***************************************************************************/
  else if (nfiles == 1 && fn2) {
    nfiles = 2;
    argc++;
    string mytempxstr(argv[l]);
    char *ptr;
    ptr = new char[mytempxstr.size()];
    argv[l + 1] = ptr;
    strcpy(argv[l + 1], argv[l]);
    mytempxstr = argv[l + 1];
    mytempxstr[mytempxstr.size() - 6] += fn2;
    mytempxstr[mytempxstr.size() - 9] += fn2;
    strcpy(argv[l + 1], mytempxstr.c_str());
  }

  // assume that several operations result in one file only, like atsadd
  // needed for excluding this last file in loops where input files are opened

  if (last_file_is_result >= 1) {
    last_file_is_result = 1;
    allchan--;
  }

  cerr << "\n\n"
       << argv[0] << ": handling " << nfiles << " files **************************************";
  cerr << " total input channels: " << allchan << endl;
  if (fn2)
    cerr << "add file from -fn2 option: " << argv[l + 1] << endl;

  if (extra_scale.size()) {
    if (extra_scale.size() != allchan) {
      cerr << "main: you must supply for each channel a spectra scaling seperatly " << endl;
      exit(0);
    }
  } else
    extra_scale.resize(allchan, "nan");

  fstream correction; // used for correction; provide file reset
  ifstream inputfile, inputfile2;
  ofstream logfile, outputfile;
  atmfile read_atmfile;

  string sysexe, leer_string = " ";
  char rdkb;

  nm = 0; // set counter for ats files to zero

  TSData atd_result; // needed for atsadd and other
  // TSData atd[allchan];
  // atsfilename atf[allchan];
  std::vector<TSData> atd;
  std::vector<atsfilename> atf;

  atd.resize(allchan);
  atf.resize(allchan);

  ATSHeader80 ats80;
  EMIMT24Header emi24;
  FUBHeader fub;

  // my_name and my_datafile is set here
  // header and data are read here!

  if (gensign) {

    int newfiles = unsigned(nargs + 1);
    valarray<double> data(samples);

    valarray<double> data2;
    valarray<double> s0;
    valarray<double> s1;
    valarray<double> s2;
    valarray<double> s3;
    valarray<double> s4;

    if (gen_rand != DBL_MAX) {
      cerr << "main::gensign->generate random standard" << endl;
      for (i = 0; i < data.size(); i++)
        data[i] = rand() * gen_rand;
    }
#ifdef myGSL
    else if (gen_rand_gsl) {
      const gsl_rng_type *T;
      gsl_rng *r;
      gsl_rng_env_setup();
      T = gsl_rng_default;
      r = gsl_rng_alloc(T);
      gsl_rng_set(r, 0);
      // double f1 = 1./707.1;   // flux
      // double f2 = 2./707.1;   // flux
      double f1 = 11.31; // coil
      double f2 = 22.61; // coil
      double f3 = 1.;    // e field
      double f4 = 2.;    // e field

      if (noise_mix.size() < 5) {
        cerr << " main-> please supply 5 values for noise mixer; like 1 2 11.31 22.61 or 1 2 1./701.1 2./707.1" << endl;
        cerr << "using default coil " << endl;
      } else if (noise_mix.size() == 5) {
        cerr << "main  getting mixer values ";
        for (size_t im = 0; i < noise_mix.size(); i++)
          cerr << noise_mix[i] << " ";
        cerr << endl;
      }

      if (noise_mix.size() != 5) {
        noise_mix.push_back(f3);
        noise_mix.push_back(f4);
        noise_mix.push_back(f1);
        noise_mix.push_back(f2);
      }

      if (noise_mix.size() == 4)
        noise_mix.push_back(1.);
      for (i = 0; i < noise_mix.size(); i++)
        cerr << noise_mix[i] << endl;
      cerr << "main::gensign->generate random " << gsl_rng_name(r) << endl;
      for (i = 0; i < data.size(); i++)
        data[i] = gsl_rng_uniform(r);
      if (mt) {
        data2.resize(samples);
        s0.resize(samples);
        s1.resize(samples);
        s2.resize(samples);
        s3.resize(samples);
        s4.resize(samples);
        gsl_rng_set(r, 1471805546);
        for (i = 0; i < data.size(); i++)
          data2[i] = gsl_rng_uniform(r);

        for (i = 0; i < data.size(); i++) {
          s0[i] = data[i] * noise_mix[0] + data2[i] * noise_mix[1];         // Ex
          s1[i] = data[i] * noise_mix[1] + data2[i] * noise_mix[0];         // Ey
          s2[i] = data[i] * noise_mix[3] + data2[i] * noise_mix[2];         // Hx
          s3[i] = (data[i] * noise_mix[2] + data2[i] * noise_mix[3]) * -1.; // Hy
          s4[i] = data[i];
        }
      }
      gsl_rng_free(r);
    }

#endif

    else if (gen_sin) {
      if (!gensign_params.size()) {
        cerr << "main::gensign-> missing parameters!!" << endl;
        exit(0);
      }
      cerr << "main::gensign->generate sinus standard" << endl;
      for (i = 0; i < data.size(); i++) {
        // data[i] = 1.0012 * i + 13.;
        // data[1] += sin(1 * M_PI * 2. * i / SampleFreq);
        for (j = 0; j < gensign_params.size(); j++)
          data[i] += (2. / sqrt(2.)) * sin(gensign_params[j] * M_PI * 2. * i / SampleFreq);
      }

    } else if (gen_rect) {
      while (i < data.size() - gensign_params[0] - gensign_params[2]) {
        for (size_t ii = 0; ii < gensign_params[0]; ii++)
          data[i++] = gensign_params[1];
        for (size_t ii = 0; ii < gensign_params[2]; ii++)
          data[i++] = (-1.) * gensign_params[3];
      }

    } else {
      cerr << "main::gensign->generate quasi linear standard" << endl;
      for (i = 0; i < data.size(); i++)
        data[i] = 1.0012 * i + 13.;
    }

    if ((start_rectpulse < stop_rectpulse) && (stop_rectpulse < data.size())) {
      for (i = start_rectpulse; i < stop_rectpulse; i++)
        data[i] = rectpulse_val;
    }

    /***************************************************************************
     *                                                                         *
     *                                                                         *
     *   here I use all filenames given at command line for genrating         *
     *   new files                                                             *
     *                                                                         *
     *                                                                         *
     ***************************************************************************/
    for (nm = 0; nm < allchan; nm++) {

      cerr << "main::gensign-> setting channels " << argv[newfiles] << "  " << nm << endl;
      atd[nm].setheader(&ats80);

      if (!mt)
        atd[nm].set_data(data);
      else {
        if (nm == 0) {
          atd[nm].set_data(s0);
          atd[nm].set_diplen_angle(1000., 0.);
        } else if (nm == 1) {
          atd[nm].set_data(s1);
          atd[nm].set_diplen_angle(1000., 90.);
        } else if (nm == 2) {
          atd[nm].set_data(s2);
        } else if (nm == 3) {
          atd[nm].set_data(s3);
        } else {
          atd[nm].set_data(s4);
        }
      }

      //            if (strstr(argv[i], "_V") || strstr(argv[i], "_v")) {
      //                atd[nm+l].set_filetype_07(1);
      //            }
      atd[nm].my_name(argv[newfiles], out_dir);
      atf[nm].setName(argv[newfiles]);

      atd[nm].set_sensor(sensor_type, SensorNo, sensorcalfile);
      atd[nm].set_sensor_pos(senspos);
      atd[nm].set_lat_lon(lag, lam, las, LatH, log, lom, los, LonH);
      atd[nm].set_ref_meridian(ref_med, ref_med, 23);
      atd[nm].set_elevation(elevation);
      atd[nm].set_diplen_angle(diplen, angle);

      atd[nm].set_utc_to_gps_offset(UTCGPSOffset);

      atd[nm].set_channel(channel_type, SerNoADB, "");
      atd[nm].set_datetime(datetime, gmt_offset);
      atd[nm].set_data_info(samples, SampleFreq);
      atd[nm].set_system(system_name, SerNoADU);
      atd[nm].set_systemcalfile(systemcalfile);
      atd[nm].set_datetime_plus(time_sec);

      if (!out_dir.size())
        temp_str = atd[nm].my_dir_is + atd[nm].my_name_is;
      else
        temp_str = out_dir + atd[nm].my_name_is;
      cerr << "main -> writing " << temp_str;

      // missing 14. Oct 02
      outputfile.open(temp_str.c_str(), ios::binary | ios::out);

      atd[nm].write_headers(outputfile);
      atd[nm].write(outputfile);
      outputfile.close();
      cerr << " done " << endl;

      newfiles++;
    }
    exit(0);
  }

  /***************************************************************************
   *                                                                         *
   * from this point on we will use allchan as counter                       *
   * for all channels - that might be more than files at command line!!!     *
   *                                                                         *
   *                                                                         *
   *                                                                         *
   ***************************************************************************/

  /***************************************************************************
   *                                                                         *
   *   start with the data section                                           *
   *                                                                         *
   *   data vector initialized before gensign section                         *
   *                                                                         *
   *                                                                         *
   ***************************************************************************/

  // if we import ascii files we can not read headers, so completely skip this section!
  if (!rda) {
    nm = 0; // is the loop counter; make sure that nm is zero
    // when we calculate the channels here do not use nm++ but use nm = nm +l
    // as you will see below; mtx has one channel in a file, Goettingen 5 channels!

    // counter i ok for reading files - we might get MORE channel than files
    for (i = unsigned(nargs + 1); i < unsigned(argc - last_file_is_result); i++) {

      // create a log file only - allow to create in in a different directory
      if (!nargs) {
        cerr << "no arguments given; using -nodata to create a log file for you" << endl;
        nodata = 1;
        cerr << "repeat with -all or -all -tc if you want to see the data without/with time column" << endl;
      }

      if ((!strstr(argv[i], ".ats") && !strstr(argv[i], ".ATS")) &&
          (!strstr(argv[i], ".x") && !strstr(argv[i], ".GOE")) &&
          (!strstr(argv[i], ".t0") && !strstr(argv[i], ".T0")) &&
          (!strstr(argv[i], ".bgrf") && !strstr(argv[i], ".BGRF"))) {
        cerr << "the name of your file does not end with ats ... might crash soon" << endl;
      }

      inputfile.open(argv[i], ios::binary | ios::in);
      if (!inputfile) {
        cerr << "\nmain (no rda) -> can not open " << argv[i] << " for input, exit\n";
        return EXIT_FAILURE;
      }

      // reading various header types
      // loops will be active if -chan 0 1 2 has been given for
      // multi channel files like .goe format

      //    if ( strstr(argv[i], ".ats") || strstr(argv[i], ".ATS")) loops = 1;
      if (strstr(argv[i], ".goe") || strstr(argv[i], ".GOE"))
        loops = chan.size();
      //    cerr << "loops " << loops << endl;

      for (l = 0; l < loops; l++) {

        // read multi channel file again and again from beginning
        inputfile.seekg(0);

        if (strstr(argv[i], ".ats") || strstr(argv[i], ".ATS")) {

          atd[nm + l].setheader(&ats80);
          atd[nm + l].read_headers(inputfile);
          if (strstr(argv[i], "_V") || strstr(argv[i], "_v")) {
            atd[nm + l].set_filetype_07(1);
          }
        }

        if (strstr(argv[i], ".t0") || strstr(argv[i], ".T0")) {
          atd[nm + l].setheader(&emi24);
          atd[nm + l].read_headers(inputfile);
        }

        if (strstr(argv[i], ".r") || strstr(argv[i], ".r")) {
          atd[nm + l].setheader(&fub);
          atd[nm + l].read_headers(inputfile);
        }

        if (last_samples) {
          start = atd[nm + l].get_samples() - last_samples;
          used_samples = last_samples;
        }
        if (time_sec != 0 && time_sec != LONG_MAX) {
          if (relative_time) {
            int sh;
            sh = (int)(time_sec * atd[nm + l].get_samplefreq());
            cerr << "main -> shifting start value with " << sh << " samples, corresponding with " << time_sec << "[s]" << endl;
            vstart.push_back(start + sh);
            if (!loops && stop)
              stop += sh;
          } else
            cerr << "main -> together with absolute time (-abst) start samples will not be shifted but date output (-tc)  ... ignored " << endl;
        }
        if (isostart.size() && isostop.size() && (time_sec == LONG_MAX)) {
          edate.set_UTC_TimeStamp(atd[nm + l].get_datetime());
          edate.set_sample_freq(atd[nm + l].get_samplefreq());
          start = samples_to_StartTime(edate, start_date);
          stop = samples_to_StartTime(edate, stop_date);
          vstart.push_back(start);
          used_samples = stop - start;
          cerr << "Setting ISO" << start << " " << stop << " " << used_samples << endl;
        }

        else
          vstart.push_back(start); // make start times individual

        vtotalsmpls.push_back(atd[nm + l].get_samples());
        atd[nm + l].my_datafile(argv[i]);
        atd[nm + l].my_name(argv[i], out_dir); // tell me who I am
        atf[nm + l].setName(argv[i]);
        // give a check
        cerr << "\nmain -> open " << atd[nm + l].my_name_is << " (type: " << atd[nm + l].get_channel_type()
             << "  @" << atd[nm + l].get_samplefreq() << " Hz sample freq.); " << atd[nm + l].get_samples()
             << " samples " << endl;

        // set the ats filename

        if (strstr(argv[i], ".t0") || strstr(argv[i], ".T0")) {
          if (SerNoADU.size()) {
            string xstr;
            atd[nm].set_system(system_name, SerNoADU);
            atd[nm + l].change_run(run);
            atd[nm + l].my_name_is = atd[nm + l].return_atsfilename(run);
            xstr = atd[nm + l].my_name_is;
            atf[nm + l].setName(xstr.c_str());
            cerr << "main -> converting to ats file naming convention:  " << atd[nm + l].my_name_is << endl;
          }
        }

        // after reading the header we check command line paramters in detail
        if ((!start && !stop && !used_samples && !nargs && !last_samples) || nodata) {
          atd[nm + l].set_samples(0);
        }
        // no data means that onyl a log file is created like with "atsview *.ats"
        if (!nodata) {

          // get start and stop time from each file / channel
          if (sync) {
            sync_dates_start.push_back(atd[nm + l].get_datetime());
            sync_dates_stop.push_back(atd[nm + l].get_stoptime());
          }

          // check that out
          if (reduction) {
            if (reduction == 4)
              filter_length = 71;
            if (reduction == 32)
              filter_length = 471;
            if (reduction == 2)
              filter_length = 35;
            if (reduction == 25)
              filter_length = 501;
            if (reduction == 8)
              filter_length = 158;
            // grid time returns vstart
            if (!nogrid) {
              find_grid_time(atd[nm + l].get_datetime(), vstart[nm + l], used_samples, atd[nm + l].get_samplefreq(),
                             reduction, filter_length, raster);
            } else
              vstart[nm + l] = 0;
          }

          // -all or -start -all was given
          if ((used_samples == UINT_MAX) && (vstart[nm + l] < atd[nm + l].get_samples())) { // take the complete file
            atd[nm + l].set_samples(atd[nm + l].get_samples() - vstart[nm + l]);            // or from start on
            stop = atd[nm + l].get_samples();
          }

          // -start and -stop given
          else if (stop && (stop <= atd[nm + l].get_samples()) && (vstart[nm + l] < atd[nm + l].get_samples())) {
            atd[nm + l].set_samples(size_t(stop - vstart[nm + l]));
            cerr << vstart[nm + l] << "  -  " << stop << endl;
          }

          // start and used samples are given
          else if ((vstart[nm + l] < atd[nm + l].get_samples()) && used_samples && (used_samples <= atd[nm + l].get_samples() - vstart[nm + l])) {
            atd[nm + l].set_samples(used_samples);
            stop = vstart[nm + l] + used_samples;
          }

          // -start but to big -stop given
          else if (stop && stop > atd[nm + l].get_samples()) {
            cerr << "main -> sorry file contains only " << atd[nm + l].get_samples() << " samples, ";
            cerr << "main -> setting stop to " << atd[nm + l].get_samples() << endl;
            stop = atd[nm + l].get_samples();
            used_samples = atd[nm + l].set_samples(atd[nm + l].get_samples() - vstart[nm + l]);
          }

          // here I know all about possible start and stop
          if (sync) {
            vsamples.push_back(atd[nm + l].get_samples());
          } else {

            // here we read the data section finally   - and only if sync option was NOT given
            // wl == 1 : fft window has the same size like time series
            if (!sync && (wl == 1)) {
              wl = atd[nm + l].get_samples();
              rmtrend = 1;
              cerr << "main -> preparing full ts fft with zero padding" << endl;
              invfft_zeropadding = 1;
              if (!((wl % 2) == 0)) {
                --wl;
              }
              cerr << "main -> setting fft window length to used samples: " << wl << endl;
            }
            // if no sync or sync2 was required the data section is read here
            // allow also to read the INTGERS for special treatments
            if (!cut_int)
              atd[nm + l].read(inputfile, vstart[nm + l], atd[nm + l].get_samples(), slice);
            else
              atd[nm + l].read_int(inputfile, vstart[nm + l], atd[nm + l].get_samples(), slice);
          }

          // get opts for stddev
          if (do_stddev) {
            atd[nm].set_stddev_param(min_stddev, max_stddev, ovr);
          }

        } // !nodata
      } // end loop over channels
      inputfile.close();
      nm = nm + l;
    }

    /***************************************************************************
     *                                                                         *
     * all data headers have been read in now                                  *
     * we should know all start and stop times                                 *
     * all channel in total                                                    *
     *                                                                         *
     *                                                                         *
     ***************************************************************************/

    /***************************************************************************
     *                                                                         *
     *                                                                         *
     * we want to use same start (stop) time if requested                      *
     *                                                                         *
     *                                                                         *
     *                                                                         *
     ***************************************************************************/

    if (sync) {
      nm = 0;

      // starts, total size and used samples
      find_sync_times(sync_dates_start, vtotalsmpls, vstart, vsamples, atd[0].get_samplefreq(), wl, sync);

      for (i = unsigned(nargs + 1); i < unsigned(argc - last_file_is_result); i++) {

        inputfile.open(argv[i], ios::binary | ios::in);
        if (!inputfile) {
          cerr << "\nmain (sync) -> can not open " << argv[i] << " for input, exit\n";
          return EXIT_FAILURE;
        }

        // das problem n x 32 sekunden muss gel�t werden??

        for (l = 0; l < loops; l++) {

          if (!nodata) {
            inputfile.seekg(0);
            // for some reason I can not re-open header
            /*
            if ( strstr(argv[i], ".ats") || strstr(argv[i], ".ATS")) {
            cerr << "A  ";
            atd[nm+l].setheader(&ats80);
            cerr << "B  ";
            atd[nm+l].read_headers(inputfile);
            cerr << "C  ";
            }

            if ( strstr(argv[i], ".t02") || strstr(argv[i], ".T02")) {
            atd[nm+l].setheader(&emi24);
            atd[nm+l].read_headers(inputfile);
            }
            */
            atd[nm + l].my_datafile(argv[i]);
            atd[nm + l].my_name(argv[i], out_dir); // tell me who I am

            if (wl == 1) {

              cerr << "main -> setting fft window length to used samples is not possible for sync measurements" << endl;
              exit(0);
            }

            if (sync2)
              atd[nm + l].set_samples(vsamples[nm + l]); // start and stop
            else {

              if (atd[nm + l].get_samples() + vstart[nm + l] > atd[nm + l].get_samples()) {
                cerr << "main -> sync: reducing samples from " << atd[nm + l].get_samples();
                atd[nm + l].set_samples(atd[nm + l].get_samples() - vstart[nm + l]);
                cerr << " to " << atd[nm + l].get_samples() << endl;
              }
            }

            // data is read in here if sync or sync2 was required
            // also also to read the Integers for special treatment
            if (!cut_int)
              atd[nm + l].read(inputfile, vstart[nm + l], atd[nm + l].get_samples(), slice);
            else
              atd[nm + l].read_int(inputfile, vstart[nm + l], atd[nm + l].get_samples(), slice);

          } // !nodata
        } // end loop over channels
        inputfile.close();
        cerr << "close" << endl;
        nm = nm + l;
      }

    } // end sync
  }

  /***************************************************************************
   *                                                                         *
   *                                                                         *
   *                                                                         *
   *                                                                         *
   *                                                                         *
   *                                                                         *
   ***************************************************************************/

  /***************************************************************************
   *                                                                         *
   *                                                                         *
   *                                                                        *
   *     we start the data loop over all channels here                       *
   *     open atm files                                                      *
   *                                                                         *
   ***************************************************************************/

  /***************************************************************************
   *                                                                         *
   *   read ascii file section                                               *
   *   for conversion into ats format                                        *
   *                                                                         *
   *                                                                         *
   *                                                                         *
   ***************************************************************************/

  if (rda && corr) {

    if (atsoutfile.size() < 2) {
      cerr << "main -> rda option needs -atsoutfile option!" << endl;
      return EXIT_FAILURE;
    }
    nm = 0;
    // this loop counter is correct here - we are only counting ats files
    for (i = unsigned(nargs + 1); i < unsigned(argc - last_file_is_result); i++) {
      temp_str = argv[i];
      atd[nm].setheader(&ats80);
      atd[nm].read_ascii(temp_str, asc_col);
      atd[nm].my_name(atsoutfile.c_str(), out_dir);
      datetime = ats_time;

      nm++;
    }
    allchan = nm;
  }

  if (corr) {

    for (nm = 0; nm < allchan; nm++) {

      if (out_dir.size()) {
        cerr << "corrections on the file can only be done on the directory where the files are";
        return EXIT_FAILURE;
      } else
        cerr << "main-> corrections apply" << endl;

      string log_original;

      if (!rda) {
        newext(atd[nm].my_name_is, log_original, ".log_orig");
        // create original log file
        atd[nm].write_logfile(log_original);
      }

      if (nodata) {
        atd[nm].set_samples(atd[nm].get_samples_orig());
        cerr << "main::corr -> correcting header with -nodata option .... all original samples are used!! " << endl;
      }
      atd[nm].set_sensor(sensor_type, SensorNo, sensorcalfile);
      atd[nm].set_sensor_pos(senspos);
      atd[nm].set_lat_lon(lag, lam, las, LatH, log, lom, los, LonH);
      atd[nm].set_ref_meridian(ref_med, ref_med, 23);
      atd[nm].set_elevation(elevation);
      atd[nm].set_diplen_angle(diplen, angle);
      atd[nm].set_utc_to_gps_offset(UTCGPSOffset);

      if (fliplsbval) {
        double newlsbval = atd[nm].get_lsb();
        newlsbval *= -1.;
        atd[nm].set_lsb(newlsbval);

      } else
        atd[nm].set_lsb(lsbval);

      atd[nm].set_channel(channel_type, SerNoADB, "");
      atd[nm].set_datetime(datetime, gmt_offset);
      atd[nm].set_data_info(samples, SampleFreq);
      atd[nm].set_system(system_name, SerNoADU);
      atd[nm].set_systemcalfile(systemcalfile);
      atd[nm].set_datetime_plus(time_sec);

      /*
           if(calc_utm) {
           atd[nm].latlon2utm(ref_med, corr);
       }

           if(calc_gk) {
           atd[nm].latlon2gk(ref_med, corr);
       }
      */

      cerr << endl
           << "main -> correcting header of " << atd[nm].my_name_is << "   " << endl;

      if (write_header) {
        temp_str = atd[nm].my_dir_is + atd[nm].my_name_is;
        if (!rda)
          outputfile.open(temp_str.c_str(), ios::out | ios::in | ios::binary); // open fstream without resetting the file
        else {
          atd[nm].my_name(atsoutfile.c_str(), out_dir);
          cerr << "prepare " << atd[nm].my_name_is << endl;
          outputfile.open(atd[nm].my_name_is.c_str(), ios::binary);
        }
        if (!outputfile) {
          cerr << "can not re-write " << temp_str << "\n";
          cerr << "permanent outputfile of the headers will be done \"in place\" because data section is not touched" << endl;
        }
        // cerr << atd[nm].get_sensor_type();
        atd[nm].write_headers(outputfile);

        if (rda) {
          atd[nm].write(outputfile);
          cerr << "main writing data of ascii file to " << atd[nm].my_name_is << "\n";
        }
        outputfile.close();

        inputfile.open(temp_str.c_str(), ios::binary);
        if (!inputfile) {
          cerr << "\nmain -> fatal error can not open " << temp_str << " for input, after modification, exit\n";
          exit(1);
        } else {

          //    atd[nm].read_headers(inputfile);

          //  inputfile.close();
          //    atd[nm].write_logfile();
        }
        inputfile.close();
      }
    }
    if (write_header) {
      cerr << "main -> corrections written and finished!" << endl;
      exit(0);
    } else
      cerr << "main -> corrections NOT written and finished!" << endl;
  }

  /***************************************************************************
   *                                                                         *
   * here I implement the cut_int section                                     *
   * because all code is based on double I will exit after that              *
   * this section contains redundant code ... sorry                          *
   *                                                                         *
   *                                                                         *
   ***************************************************************************/
  if (cut_int) {
    for (nm = 0; nm < allchan; nm++) {

      if (!back && !reduction) {

        atd_result = atd[nm];

        atd_result.setheader(&ats80);

        string new_file_ats;
        if (overwrite) {
          cerr << "main::cut_int -> overwriting activated " << endl;
          atd_result.my_name_is = atd[nm].my_name_is;
        } else if (run.size() == 2) {
          if (atd[0].is07) {
            cerr << "try set run" << run;
            atd[nm].change_run(run);
            atd_result.my_name_is = atd[nm].my_name_is;
          } else {
            if (SerNoADU.size() == 3) {
              new_file_ats = SerNoADU;
              new_file_ats += atd[nm].my_name_is.substr(3, 1);
            } else
              new_file_ats = atd[nm].my_name_is.substr(0, 4);

            new_file_ats += run;
            new_file_ats += atd[nm].my_name_is.substr(6, 11);

            atd_result.my_name_is = new_file_ats;
          }
        } else {
          cerr << "main::cut_int -> either use overwrite or -run nn [-aduser] " << endl;
          exit(0);
        }
        if (!out_dir.size())
          temp_str = atd[nm].my_dir_is + atd_result.my_name_is;
        else
          temp_str = out_dir + atd_result.my_name_is;

        // I use the same file - only replacing the data vector
        atd_result.set_data_int(atd[nm].int_data());

        //        outputfile.open(temp_str.c_str(), ios::noreplace);
        outputfile.open(temp_str.c_str(), ios::binary | ios::out);
        if (outputfile) {
          cerr << "main -> writing ats (int) time series file " << temp_str << " ....";
          atd_result.write_headers(outputfile);
          atd_result.write_int(outputfile);
          outputfile.close();
          cerr << "done " << endl;
        } else {
          cerr << endl
               << "main -> " << temp_str << " already exists! Nothing done." << endl
               << endl;
        }
      }
    }
    exit(0);
  }
  /*************** end of cut_int section ****************/

  /***************************************************************************
   *                                                                         *
   *   start with the data interpretation                                    *
   *   data was read in in the section above                                 *
   *   FIRST we loop only for the log file; no data manipulatio here         *
   *   write log files                                                       *
   *                                                                         *
   ***************************************************************************/

  /***************************************************************************
   *                                                                         *
   *                                                                         *
   *                                                                        *
   *     we start the data loop over all channels here                       *
   *     open atm files                                                      *
   *                                                                         *
   ***************************************************************************/

  for (nm = 0; nm < allchan; nm++) {

    // start with the very first basic operations
    // read existing atmfile; prepare for merge if wished

    // if we later want to write the atm file the complete file must have been read in
    if (atd[nm].get_samples_orig() == atd[nm].get_samples())
      write_atm = 1;

    if (do_atm == 2 || do_atm == 4) {

      fname = atd[nm].return_atmfilename(run);
      if (!read_atmfile.open(fname)) { // open file  and returns samples , 0 if not existing
        read_atmfile.close();
        cerr << "main -> atmfile " << fname << " not found, trying ";
        // newext(atd[nm].return_atmfilename(), fname, "ATM");   // try upper case as well
        cerr << fname << endl;
        if (!read_atmfile.open(fname)) {
          cerr << "main -> atmfile not found" << endl;
          read_atmfile.close();
        }
      }

      // if existing file was opened
      // if was existing we should have the right filename now and NO create mode set
      if (read_atmfile.file_open() && !read_atmfile.file_new()) {
        newselect.resize(atd[nm].get_samples(), 0); // all SELECTED for proc
        read_atmfile.read(newselect);
        read_atmfile.close();
        atd[nm].set_select(newselect);
      }
    }

    if (!nodata) {

      // apply a filter (data reduction)
      if (reduction && atd[nm].get_samples()) {
        if (wl == atd[nm].get_samples())
          wl = atd[nm].filter(reduction);
        else
          atd[nm].filter(reduction);
      }

      if (fir_order && atd[nm].get_samples()) {
        atd[nm].fir_filter(fir_order, cutlow, cuthigh, firfil_reduct);
      }

      if (resample && atd[nm].get_samples()) {
        if (wl == atd[nm].get_samples())
          atd[nm].resample(rsmpl_freq);
        else
          atd[nm].resample(rsmpl_freq);
      }

      if (rmtrend && atd[nm].get_samples()) {
        cerr << "used bits before: " << atd[nm].usedbits() << endl;
        cerr << "max: " << atd[nm].max() << "  min: " << atd[nm].min() << " mean: " << atd[nm].mean() << " mV" << endl;
        atd[nm].detrend();
        cerr << atd[nm].my_name_is << "; trend removed" << endl;
        cerr << "used bits behind: " << atd[nm].usedbits() << endl;
        cerr << "max: " << atd[nm].max() << "  min: " << atd[nm].min() << " mean: " << atd[nm].mean() << " mV" << endl;
      }

      // put E field to mV/km; mV is ADU unit; m should be inside atsheader

      if (!noscale_e && atd[nm].get_samples())
        atd[nm].scale();

      // multiply with a factor if requested
      if (mul && atd[nm].get_samples()) {
        if (mul == DBL_MAX) {
          mul = 1. / atd[nm].mean();
          atd[nm] *= mul;
          cerr << atd[nm].my_name_is << " multiplied with " << mul << endl;
          mul = DBL_MAX; // reset for the next loop
        } else {
          atd[nm] *= mul;
          cerr << atd[nm].my_name_is << " multiplied with " << mul << endl;
        }
      }

      if (add && atd[nm].get_samples()) {
        if (add == DBL_MAX) {
          add = -1. * atd[nm].mean();
          atd[nm] += add;
          cerr << atd[nm].my_name_is << "; value added " << add
               << "mV" << endl;
          add = DBL_MAX; // reset for the next loop
        } else {
          atd[nm] += add;
          cerr << atd[nm].my_name_is << "; value added " << add
               << "mV" << endl;
        }
      }

      // or add values (shifting the amplitudes of time series)

      /***************************************************************************
       *                                                                         *
       * prepare spectral calculations if a window length wl is given            *
       *                                                                         *
       * also write ouput data                                                   *
       *                                                                         *
       *                                                                         *
       ***************************************************************************/

      if (wl) {
        atd[nm].set_window_length(wl, ovr); // copy wl of fft
      }

    } // nodata

    /***************************************************************************
     *                                                                         *
     *   section for writing output files                                      *
     *                                                                         *
     *                                                                         *
     *                                                                         *
     *                                                                         *
     ***************************************************************************/

    // ASCII
    if ((!nodata && ascii && !back)) { // if you not only want spectra .
      ascii_outget_type = "data";
      temp_str = atd[nm].return_outputfilename("dat"); // write ascii data
      outputfile.open(temp_str.c_str());               // converted files to disc...
      cerr << "main -> writing ascii file .. " << flush;
      atd[nm].write_ascii(outputfile, tc, relative_time, time_offset, dateformat,
                          0, ascii_outget_type, iso_sec);
      outputfile.close();
      cerr << atd[nm].get_samples() << " samples written to " << temp_str << endl;
    }

#ifdef myMySQL
    // xmgrace -datehint iso -source pipe "mysql tsdata -ss -e 'select iso8601.gmt, hy.hy from iso8601, hy where iso8601.idx = hy.idx;'"
    Connection con(false);
    Query query = con.query();
    //        con.connect(MY_DATABASE, MY_HOST, MY_USER, MY_PASSWORD);
    if (!con.connect(db_database.c_str(), db_host.c_str(), db_user.c_str(), db_password.c_str())) {
      cerr << "no con" << endl;
    } else {
      cerr << "connected" << endl;
      if (truncate == 1 && !nm || (truncate == 2 && !nm && !ncall)) {
        query << "truncate table hx;";
        query.execute();
        query.reset();
        query << "truncate table hy;";
        query.execute();
        query.reset();
        query << "truncate table hz;";
        query.execute();
        query.reset();
        query << "truncate table ex;";
        query.execute();
        query.reset();
        query << "truncate table ey;";
        query.execute();
        query.reset();
        query << "truncate table unix;";
        query.execute();
        query.reset();
        query << "truncate table iso8601;";
        query.execute();
        query.reset();
      }

      if ((!nodata && ts2mysql && !back)) { // if you not only want spectra .
        ascii_outget_type = "data";
        cerr << "main -> writing data to mysql .. " << flush;
        if (tc && !nm)
          atd[nm].write_mysql(query, tc, relative_time, time_offset, dateformat,
                              0, ascii_outget_type);
        // avoid multiple writing of date
        else
          atd[nm].write_mysql(query, 0, relative_time, time_offset, dateformat,
                              0, ascii_outget_type);

        cerr << atd[nm].get_samples() << " samples written to mysql" << endl;
      }

      con.close();
    }
#endif

    // ATS
    // run must be 01 or 02 .. 99  avoid to write here if we want back fft

    if (((run.size() == 2) || atsoutfile.size()) && !back && !reduction) {

      atd_result = atd[nm];
      atd_result.setheader(&ats80);

      string new_file_ats;

      if (atsoutfile.size() && (atsoutfile != atd[nm].my_name_is))
        atd_result.my_name(atsoutfile.c_str(), out_dir);

      else if (atd[0].is07) {

        atd[nm].change_run(run);
        atd_result.my_name_is = atd[nm].my_name_is;
        atd_result.set_filetype_07(1);

      } else {

        if (SerNoADU.size() == 3) {
          new_file_ats = SerNoADU;
          new_file_ats += atd[nm].my_name_is.substr(3, 1);
        } else
          new_file_ats = atd[nm].my_name_is.substr(0, 4);

        new_file_ats += run;
        new_file_ats += atd[nm].my_name_is.substr(6, 11);

        atd_result.my_name_is = new_file_ats;
      }
      if (!out_dir.size())
        temp_str = atd[nm].my_dir_is + atd_result.my_name_is;
      else
        temp_str = out_dir + atd_result.my_name_is;

      // I use the same file - only replacing the data vector
      atd_result.set_data(atd[nm].data());

      //        outputfile.open(temp_str.c_str(), ios::noreplace);
      outputfile.open(temp_str.c_str(), ios::binary | ios::out);
      if (outputfile) {
        cerr << "main -> writing new ats times series file " << temp_str << "....";
        atd_result.write_headers(outputfile);
        atd_result.write(outputfile);
        outputfile.close();
        cerr << "main->done " << endl;
      } else {
        cerr << endl
             << "main -> " << temp_str << " already exists! Nothing done." << endl
             << endl;
      }
    }

    if ((reduction || write_ats) && !back) {

      atd[nm].setheader(&ats80);
      // if we want to write ats files......change the name
      if (reduction && write_ats) {

        cerr << "main->filtred values of " << atd[nm].my_name_is << " written to ";
        // try metronix convention

        if (atsoutfile.size() && (atsoutfile != atd[nm].my_name_is))
          atd[nm].my_name(atsoutfile.c_str(), out_dir);

        else if (nmtx == "-nmtx") {
          noext(atd[nm].my_name_is, f2name);
          atd[nm].my_name_is = f2name + bandstr + "x.ats";

        } else if ((run.size() == 2) && (atd[nm].my_name_is[7] == 'f' || atd[nm].my_name_is[7] == 'F')) {
          temp_str = atd[nm].my_name_is.substr(0, 4);
          temp_str += run;
          temp_str += atd[nm].my_name_is.substr(6, 11);
          atd[nm].my_name_is = temp_str;

        }

        else {

          if (run.size() == 2)
            atd[nm].change_run(run);
          atd[nm].shift_bandindex(set_xmlheader);
        }
      }
      // append an output directory
      if (!out_dir.size())
        temp_str = atd[nm].my_dir_is + atd[nm].my_name_is;
      else
        temp_str = out_dir + atd[nm].my_name_is;

      cerr << "!!!!main -> overwriting ?!!! " << temp_str << "  ";
      outputfile.open(temp_str.c_str(), ios::binary | ios::out);
      atd[nm].write_headers(outputfile);
      atd[nm].write(outputfile);
      outputfile.close();
      cerr << "  (" << atd[nm].get_samples() << " samples) done " << endl;
    }

    atd[nm].write_logfile();
    if (atd[nm].get_samples()) {
      temp_str = atd[nm].return_outputfilename("log");
      logfile.open(temp_str.c_str(), ios::app); // append to existing log file
      logfile << endl
              << "evaluated samples: " << atd[nm].get_samples() << endl;
      logfile << "voltage and ratio peak to peak: " << atd[nm].max() - atd[nm].min() << "mV " << atd[nm].max() / atd[nm].min() << endl;
      logfile << "max: " << atd[nm].max() << "  min: " << atd[nm].min() << " mean: " << atd[nm].mean() << " mV" << endl;
      logfile << "used bits: " << atd[nm].usedbits() << endl;
      logfile << "over all standard deviation: " << atd[nm].stddev() << endl;
      logfile << "voltage peak peak / std deviation : " << (atd[nm].max() - atd[nm].min()) / atd[nm].stddev() << endl;
      logfile << "(ratio is: square wave 2, triangle wave sqrt(12) = 3.46, sine wave 2*sqrt(2) = 2.83, noise ca. 6-8;)" << endl;
      logfile.close();
      cerr << "main -> logfile written" << endl;
    }
  } // end data loop

  /***************************************************************************
   *                                                                         *
   *                                                                         *
   * sub stack in time domain                                                *
   *                                                                         *
   *                                                                         *
   *                                                                         *
   ***************************************************************************/

  if (sst && wl && atd[nm].get_samples()) {
    cerr << "main -> sub stacks in time domain for CSAMT" << endl;
    valarray<double> dsta, dsta_chop(wl);
    for (nm = 0; nm < allchan; nm++) {
      size_t new_stacks = atd[nm].get_stacks();
      dsta.resize(0);

      my_valarray::vwrite_first(atd[nm].data(), "in.dat", 0, 4096);

      my_valarray::substack2(dsta, atd[nm].data(), wl, 1, 1, sst, new_stacks);

      atd[nm].set_data(dsta);
      // check stack size! should be set new
      cerr << "main -> possible use of stacks: " << atd[nm].get_stacks() << endl;
      cerr << ascii_sst << endl;
      if (ascii_sst) {
        ascii_outget_type = "data";
        temp_str = atd[nm].return_outputfilename("dat"); // write ascii data
        string xxstr = num2str(ascii_sst_count);
        temp_str += "_" + xxstr;
        outputfile.open(temp_str.c_str()); // converted files to disc...
        cerr << "main -> writing ascii sub stack file .. " << flush;

        atd_result.cpheader(atd[nm]);
        for (size_t uui = 0; uui < wl; ++uui) {
          dsta_chop[uui] = dsta[uui];
        }
        atd_result.set_data(dsta_chop);

        atd_result.write_ascii(outputfile, tc, relative_time, time_offset, dateformat,
                               0, ascii_outget_type, iso_sec);
        outputfile.close();
        cerr << atd_result.get_samples() << " samples written to " << temp_str << endl;
        ++ascii_sst_count;
      }
    }
    dsta.resize(0);
    dsta_chop.resize(0);
  }
  /***************************************************************************
   *                                                                         *
   *                                                                         *
   *  cross correlation intime domain                                        *
   *  e.g. to find a oint where two time series are synchronius              *
   *                                                                         *
   *                                                                         *
   ***************************************************************************/

  if (do_ccf && (allchan == 2) && wl) { // calculate cross coherency
    cerr << "main -> calculating correlation" << endl;
    tfnewext((atd[0].my_out_dir_is + atd[0].my_name_is), atd[1].my_name_is, fname, "_", "_ccf.dat");
    my_valarray::CCF(ccfxy, atd[0].data(), atd[1].data(), ccf_startx, ccf_starty, wl, atd[0].get_samplefreq(), ccf_shifts);
    my_valarray::vwrite(ccfxy, fname, 0, 0);
    cerr << "main -> " << fname << " written" << endl;
    cerr << endl
         << "main->************finshed****cross correlation************************" << endl
         << endl;

    exit(0);
  }

  // if (do_stddev) return EXIT_SUCCESS;

  /***************************************************************************
   *                                                                         *
   *                                                                         *
   *                                                                         *
   *                                                                         *
   *                                                                         *
   *                                                                         *
   ***************************************************************************/

  /***************************************************************************
   *                                                                         *
   *                                                                         *
   *                                                                         *
   *                                                                         *
   *                                                                         *
   *                                                                         *
   ***************************************************************************/
  /***************************************************************************
   *                                                                         *
   *                                                                         *
   *                                                                         *
   *                                                                         *
   *  concatunate ats files                                                  *
   *                                                                         *
   ***************************************************************************/

  if (atscat) {
    nm = 1; // we cat to the first file
    string new_file_ats(atd[0].my_name_is, 0, 4);

    for (i = 1; i < allchan; i++) {
      atd[0].catats(atd[nm++]);
    }
    if (atd[0].is07) {
      atd[0].change_run(new_run_number);
    } else {
      // string new_file_ats(atd[0].my_name_is, 0, 4);
      new_file_ats += new_run_number;
      new_file_ats += atd[0].my_name_is.substr(6, 11);
      atd[0].my_name_is = new_file_ats;
    }
    if (!out_dir.size())
      temp_str = atd[0].my_dir_is + atd[0].my_name_is;
    else
      temp_str = out_dir + atd[0].my_name_is;

    //    outputfile.open(temp_str.c_str(), ios::noreplace);
    outputfile.open(temp_str.c_str(), ios::binary | ios::out);
    if (outputfile) {
      cerr << "main->writing " << temp_str << "....";
      atd[0].write_headers(outputfile);
      atd[0].write(outputfile);
      outputfile.close();
      cerr << "main->done " << endl;
    } else {
      cerr << "main -> " << temp_str << " already exists! Nothing done." << endl;
    }
    newext(temp_str, new_file_ats, "atm");
    if (isupper(new_file_ats[new_file_ats.size() - 6]))
      new_file_ats[new_file_ats.size() - 6] = 'A';
    else
      new_file_ats[new_file_ats.size() - 6] = 'a';
    if (isupper(new_file_ats[new_file_ats.size() - 9]))
      new_file_ats[new_file_ats.size() - 6] = 'A';
    else
      new_file_ats[new_file_ats.size() - 9] = 'a';

    exit(0);
  }

  if (atsadd || atssub || atsmul || atsdiv) {

    nm = 0;
    atd_result = atd[nm]; // first I initialize the file; += is not possible
    atd_result.setheader(&ats80);
    cerr << atd_result.data(10) << "  " << atd[nm].data(10) << "  " << atd[1].data(10);

    atd_result.my_name(argv[argc - 1], out_dir);
    string tmp_str = atd_result.return_outputfilename("ats");

    cerr << endl
         << "main->using header information of "
         << atd[nm].my_name_is << " for " << atd_result.my_name_is << endl;

    if (atsdiv)
      for (i = 1; i < allchan; i++) { // first file [0] has been used already
        atd_result *= atd[i];
        cerr << "\nmain->all values multplied *= to " << atd_result.my_name_is << endl;
      }
    if (atsmul)
      for (i = 1; i < allchan; i++) {
        atd_result /= atd[i];
        cerr << "\nmain->all values divided /= to " << atd_result.my_name_is << endl;
      }
    if (atsadd)
      for (i = 1; i < allchan; i++) {

        atd_result += atd[i];
        cerr << "\nmain->all values added += to " << atd_result.my_name_is << endl;
      }
    if (atssub)
      for (i = 1; i < allchan; i++) {
        atd_result -= atd[i];
        cerr << "\nmain->all values subtracted -= to " << atd_result.my_name_is << endl;
      }

    outputfile.open(tmp_str.c_str(), ios::binary | ios::out);
    atd_result.write_headers(outputfile);
    atd_result.write(outputfile);
    outputfile.close();
  }

  for (nm = 0; nm < allchan; nm++) {

    if (lin_conv) {
      cout << "lin_conv -> filter length = " << lc_filter_len << " pts" << endl;
      for (j = 0; j < lc_target_f.size(); j++) {
        cout << "lin_conv -> linear convolution of " << lc_target_f[j] << "Hz" << endl;
        atd[nm].linconv(lc_filter_len, lc_target_f[j]);
      }

      if (!out_dir.size())
        temp_str = atd[nm].my_dir_is + atd[nm].my_name_is;
      else
        temp_str = out_dir + atd[nm].my_name_is;

      outputfile.open(temp_str.c_str(), ios::binary | ios::out);
      atd[nm].write_headers(outputfile);
      atd[nm].write(outputfile);
      outputfile.close();
    }

    if (do_stddev) {

      // you might not believe - this line is the main routine!
      cerr << "main -> std dev analysed windows: " << atd[nm].stddevarray(wl, ovr, do_atm) << endl;

      if (ascii) { // write result to disk
        ascii_outget_type = "stddev";
        fname = atd[nm].return_outputfilename("stddev");
        cerr << "main -> writing " << fname << " ... " << endl;
        outputfile.open(fname.c_str());
        atd[nm].write_ascii(outputfile, tc, relative_time, time_offset, dateformat,
                            0, ascii_outget_type);

        outputfile.close();
        cerr << "main -> done" << endl;
      }

      if (ascii) { // write selection
        fname = atd[nm].return_outputfilename("stddev_used");
        outputfile.open(fname.c_str());
        cerr << "main -> writing " << fname << " ... " << endl;
        ascii_outget_type = "stddev_used";
        atd[nm].write_ascii(outputfile, tc, relative_time, time_offset, dateformat,
                            0, ascii_outget_type);
        outputfile.close();
      }

      if (do_atm && do_atm != 4) { // 4 = use but not manipulate; 2 merge will write
        // assign atm file name to channel a
        fname = atd[nm].return_atmfilename(run);
        read_atmfile.open(fname);
        cerr << "main -> " << fname << " binary data for " << read_atmfile.write(atd[nm].get_select()) << " samples written; "
             << atd[nm].tags() << " selections (points) excluded from processing " << endl;
      }

      cerr << "main -> stddev finished ************* " << endl;
    }

    if (max_min != DBL_MAX) {

      // you might not believe - this line is the main routine!
      atd[nm].maxmin_select(max_min_length, max_min);

      if (do_atm && do_atm != 4) { // 4 = use but not manipulate; 2 merge will write
        // assign atm file name to channel a
        fname = atd[nm].return_atmfilename(run);
        read_atmfile.open(fname);
        cerr << "main -> " << fname << " binary data for " << read_atmfile.write(atd[nm].get_select()) << " samples written; "
             << atd[nm].tags() << " selections (points) excluded from processing " << endl;
      }

      cerr << "main ->max_min finished ************* " << endl;
    }

    /***************************************************************************
     * try to remove steps from time series data                               *
     * these steps might be a result of poor electrode contacts                *
     *                                                                         *
     *                                                                         *
     *                                                                         *
     *                                                                         *
     ***************************************************************************/

    if (destep && !despike) {
      size_t nsteps = 0;

      nsteps = atd[nm].destep(step_len, step_height, relaxation, step_skewness, do_atm);

      if (ascii) {
        if (nsteps) {
          if (!out_dir.size())
            temp_str = atd[nm].my_dir_is + atd[nm].my_name_is;
          else
            temp_str = out_dir + atd[nm].my_name_is;
          newext(temp_str, fname, "steps");
          outputfile.open(fname.c_str());
          ascii_outget_type = "steps";
          atd[nm].write_ascii(outputfile, tc, relative_time, time_offset, dateformat,
                              0, ascii_outget_type);
          outputfile.close();
          cerr << "main -> index of steps written to " << fname << endl;
        }
      }

      // overwite ats data file here!

      if (dstw) {
        // atd[nm].set_samples(atd[nm].nsamples());
        if (!out_dir.size())
          temp_str = atd[nm].my_dir_is + atd[nm].my_name_is;
        else
          temp_str = out_dir + atd[nm].my_name_is;

        cerr << "main -> writing " << temp_str;
        outputfile.open(temp_str.c_str(), ios::binary | ios::out);
        atd[nm].write_headers(outputfile);
        atd[nm].write(outputfile);
        outputfile.close();
        cerr << " done " << endl;
      }

      // manipulation of atm file
      if (do_atm && do_atm != 4) { // 4 = use but not manipulate; 2 merge will write

        fname = atd[nm].return_atmfilename(run);
        read_atmfile.open(fname);
        cerr << "main -> " << fname << " binary data for " << fname << "  " << read_atmfile.write(atd[nm].get_select()) << " samples written; "
             << atd[nm].tags() << " selections made " << endl;
      }

      cerr << "main -> destep finished ************* " << endl;

    }

    /***************************************************************************
     * try to remove spike from time series data                               *
     * these spike might be a result of ????????                               *
     *                                                                         *
     *                                                                         *
     *                                                                         *
     *                                                                         *
     ***************************************************************************/

    else if (despike && !destep) {

      size_t nspikes = 0;

      if (do_atm == 2 || do_atm == 4) { // merge or use #
        cerr << "main->main (despike) -> merging /using  atmfile; old selections: " << atd[nm].tags() << "  " << endl;
      }
      nspikes = atd[nm].despike(shoulder_len, spike_height, relaxation, spike_skewness, do_atm);

      if (do_atm == 2 || do_atm == 4) { // merge or use
        cerr << "main->new tags : " << atd[nm].tags() << endl;
      }

      if (ascii) {
        if (nspikes) {
          if (!out_dir.size())
            temp_str = atd[nm].my_dir_is + atd[nm].my_name_is;
          else
            temp_str = out_dir + atd[nm].my_name_is;
          newext(temp_str, fname, "spikes");
          outputfile.open(fname.c_str());
          //  these are the orig spike values.
          ascii_outget_type = "spikes";
          atd[nm].write_ascii(outputfile, tc, relative_time, time_offset, dateformat,
                              0, ascii_outget_type);
          outputfile.close();
          cerr << "main->index of spikes written to " << fname << endl;
        }
      }

      // overwite ats data file here!!!
      if (dspw) {
        // atd[nm].set_samples(atd[nm].nsamples());
        if (!out_dir.size())
          temp_str = atd[nm].my_dir_is + atd[nm].my_name_is;
        else
          temp_str = out_dir + atd[nm].my_name_is;
        cerr << "main -> writing " << temp_str;

        // missing 14. Oct 02
        outputfile.open(temp_str.c_str(), ios::binary | ios::out);

        atd[nm].write_headers(outputfile);
        atd[nm].write(outputfile);
        outputfile.close();
        cerr << "main->done " << endl;
      }

      // manipulation of atm file
      if (do_atm && do_atm != 4) { // 4 = use but not manipulate; 2 merge will write
        fname = atd[nm].return_atmfilename(run);
        read_atmfile.open(fname);
        cerr << "main -> " << fname << " binary data for " << read_atmfile.write(atd[nm].get_select());
        cerr << " samples written " << atd[nm].tags() << " selections made " << endl;
      }

      cerr << "main -> despike finished ************* " << endl;
    }

  } // channel despike/destep

  /***************************************************************************
   *                                                                         *
   *                                                                         *
   *                                                                         *
   *                                                                         *
   *                                                                         *
   *                                                                         *
   ***************************************************************************/

  //************MERGE OUTPUT COLUMMNS
  if (mc && !nodata && !atsadd && !atsmul && !atsdiv && !atssub) {

    string myisodate;
    size_t j = 0;
    nm = 0;

    double read_time_col;

    if (strstr(argv[argc - 1], ".ats") || strstr(argv[argc - 1], ".ATS")) {
      cerr << "main->Do you really want to overwrite a possible input file.. last file should be a *.dat file? [n]\n";
      if (askyn()) {
        cerr << argv[argc - 1] << " NOT overwritten\n";
        return EXIT_FAILURE;
      }
    }

    // write ascii
    if (!wbin) {
      outputfile.open(argv[argc - 1]);
      if (tc) {

        outputfile.setf(ios_base::scientific);
        outputfile.precision(10);

        if (!dateformat.size()) {

          if (relative_time)
            cerr << "main->write relative time " << atd[0].get_samples() << " samples and " << allchan << " channels" << endl;
          else
            cerr << "main->write absolute time " << atd[0].get_samples() << " samples and " << allchan << " channels" << endl;
          for (j = 0; j < atd[0].get_samples(); j++) {
            outputfile << atd[0].time(j, relative_time, time_offset, dateformat, myisodate) << "\t";
            for (i = 0; i < allchan; i++)
              outputfile << atd[i].data(j) << "\t";
            outputfile << endl;
          }

        } else {
          cerr << "main->write ISO time " << atd[0].get_samples() << " samples and " << allchan << " channels" << endl;
          for (j = 0; j < atd[0].get_samples(); j++) {
            atd[0].time(j, relative_time, time_offset, dateformat, myisodate);
            outputfile << myisodate << "\t";
            for (i = 0; i < allchan; i++)
              outputfile << atd[i].data(j) << "\t";
            outputfile << endl;
          }
        }

      } else if (!tc) {

        if (relative_time)
          cerr << "main->write data " << atd[0].get_samples() << " samples and " << allchan << " channels" << endl;
        for (j = 0; j < atd[0].get_samples(); j++) {
          for (i = 0; i < allchan; i++)
            outputfile << atd[i].data(j) << "\t";
          ;
          outputfile << endl;
        }
      }
    }

    // binary
    //   ofs.write(static_cast<char *> (static_cast<void *>(&idata)), sizeof( int));
    else {
      outputfile.open(argv[argc - 1], ios::binary | ios::out);

      if (!usefloat) {
        if (tc) {

          if (!dateformat.size()) {
            double t_temp;
            if (relative_time)
              cerr << "main->write binary relative time " << atd[0].get_samples() << " samples and " << allchan << " channels " << endl;
            else
              cerr << "main->write binary absolute time " << atd[0].get_samples() << " samples and " << allchan << " channels" << endl;
            for (j = 0; j < atd[0].get_samples(); j++) {
              t_temp = atd[0].time(j, relative_time, time_offset, dateformat, myisodate);
#ifdef use_shift_to_swap

              bswap_32(t_temp);
#endif
#ifdef use_template_to_swap

              tbswap(t_temp);
#endif

              outputfile.write(static_cast<char *>(static_cast<void *>(&t_temp)), sizeof(double));
              for (i = 0; i < allchan; i++)
                atd[i].writedouble(outputfile, j);
            }

          } else {
            cerr << "main->write ISO time not supported for binary output" << endl;
          }

        } else if (!tc) {

          cerr << "main->write binary data (ntc) " << atd[0].get_samples() << " samples and " << allchan << " channels" << endl;
          for (j = 0; j < atd[0].get_samples(); j++) {
            for (i = 0; i < allchan; i++)
              atd[i].writefloat(outputfile, j);
          }
        }
      } // no use float

      if (usefloat) {
        if (tc) {

          if (!dateformat.size()) {
            float t_temp;
            if (relative_time)
              cerr << "main->write binary relative time (float) " << atd[0].get_samples() << " samples and " << allchan << " channels " << endl;
            else
              cerr << "main->write binary absolute time (float) " << atd[0].get_samples() << " samples and " << allchan << " channels" << endl;
            for (j = 0; j < atd[0].get_samples(); j++) {
              t_temp = float(atd[0].time(j, relative_time, time_offset, dateformat, myisodate));
#ifdef use_shift_to_swap

              bswap_32(t_temp);
#endif
#ifdef use_template_to_swap

              tbswap(t_temp);
#endif

              outputfile.write(static_cast<char *>(static_cast<void *>(&t_temp)), sizeof(float));
              for (i = 0; i < allchan; i++)
                atd[i].writefloat(outputfile, j);
            }

          } else {
            cerr << "main->write ISO time not supported for binary output" << endl;
          }

        } else if (!tc) {

          cerr << "main->write binary data (ntc, float)" << atd[0].get_samples() << " samples and " << allchan << " channels" << endl;
          for (j = 0; j < atd[0].get_samples(); j++) {
            for (i = 0; i < allchan; i++)
              atd[i].writefloat(outputfile, j);
          }
        }
      } // use float

    } // binary

    outputfile.close();

    cerr << argv[argc - 1] << " written\n";
  }
  // overwriting data  e.g.after manipulation
  if (overwrite) {

    for (nm = 0; nm < allchan; nm++) {

      if (run.size() == 2 || SerNoADU.size() == 3) {

        string new_file_ats;
        if (atd[0].is07) {
          atd[nm].change_run(new_run_number);
        } else if (SerNoADU.size() == 3) {
          new_file_ats = SerNoADU;
          new_file_ats += atd[nm].my_name_is.substr(3, 1);
        } else
          new_file_ats = atd[nm].my_name_is.substr(0, 4);

        if (run.size() == 2)
          new_file_ats += run;
        else
          new_file_ats += atd[nm].my_name_is.substr(4, 2);
        new_file_ats += atd[nm].my_name_is.substr(6, 11);

        if (!out_dir.size())
          temp_str = atd[nm].my_dir_is + new_file_ats;
        else
          temp_str = out_dir + new_file_ats;

        cerr << "main -> writing " << temp_str;
        outputfile.open(temp_str.c_str(), ios::binary | ios::out);
        atd[nm].write_headers(outputfile);
        atd[nm].write(outputfile);
        outputfile.close();
        cerr << "  (" << atd[nm].get_samples() << " samples) done " << endl;

      } // end new file .. else overwrite!!!!!!!!!!!!!!!
      else {

        atd[nm].set_samples(atd[nm].get_samples());
        if (!out_dir.size())
          temp_str = atd[nm].my_dir_is + atd[nm].my_name_is;
        else
          temp_str = out_dir + atd[nm].my_name_is;
        cerr << "!!!! main -> overwriting !!! " << temp_str;
        outputfile.open(temp_str.c_str(), ios::binary | ios::out);
        atd[nm].write_headers(outputfile);
        atd[nm].write(outputfile);
        outputfile.close();
        cerr << "  (" << atd[nm].get_samples() << " samples) done " << endl;
      }
    }
  }

  if (nspc) {
    cerr << endl
         << "************finshed****no spectra*******************************" << endl
         << endl;

    exit(0);
  }

  /***************************************************************************
   *                                                                         *
   *   start with the spectral analysis                                      *
   *                                                                         *
   *                                                                         *
   *                                                                         *
   *                                                                         *
   ***************************************************************************/

  if (wl && !nspc && atd[0].get_samples()) {
    string tmp_str;
    cerr << endl
         << " ******** start spectra ********" << endl;

    // init spectra
    // v_spectra assx[allchan];
    std::vector<v_spectra> assx;
    assx.resize(allchan);
    MTXCal mfs_coil;
    MFS07Cal mfs07_coil;
    LTSCal lts_squid;
    EFP05Cal efp05_elec;
    FGS01Cal fgs01_flux;
    FGS02Cal fgs02_flux;
    PLAINCal plain_cal;
    EMIMT24_sys emi24_sys;
    ADU06cal adu06_sys;
    ADU07cal adu07_sys;
    EMI_BF_cal emi_bf_coil;
    ifstream defaultfreqs;
    valarray<double> flist; // prepare for parzening

    if (parzen) {
      defaultfreqs.open("default.flst");
      if (defaultfreqs) {
        my_valarray::read_ukwn_1(defaultfreqs, flist);
        defaultfreqs.close();
      } else {
        cerr << "main -> can not open default.flst for reading parzen frequencies" << endl;
        exit(0);
      }
    }

    for (nm = 0; nm < allchan; nm++) {

      if (invfft_zeropadding)
        ovr = 0; // no overlapping with full ts inversion
      if (invfft_zeropadding)
        no_dc_fft = 1; // no overlapping with full ts inversion
      if (invfft_zeropadding)
        no_dc_fft = 1; // no overlapping with full ts inversion
      cerr << "  init(" << nm + 1 << ") of " << allchan << endl;
      assx[nm].init(atd[nm].my_name_is, wl, atd[nm].get_samplefreq(), atd[nm].get_samples(),
                    no_dc_fft, atd[nm].get_select(), ovr, invfft_zeropadding);

      /***************************************************************************
       *                                                                         *
       *   read the transfer function first                                      *
       *                                                                         *
       *                                                                         *
       *                                                                         *
       *                                                                         *
       ***************************************************************************/
      if (auto_trf || theo_trf) {
        cerr << "main::auto || theo" << endl;
        tmp_str = atd[nm].get_system_name();
        lower(tmp_str);
        if (tmp_str == "emimt24")
          assx[nm].set_calibsystem(&emi24_sys);
        if (tmp_str == "adu06")
          assx[nm].set_calibsystem(&adu06_sys);
        if (tmp_str == "adu07")
          assx[nm].set_calibsystem(&adu07_sys);

        // we might use orig_sample_freq ...?
        if (!nosystrf) {
          cerr << "main::reading system calibration of " << atd[nm].get_system_name() << endl;
          assx[nm].read_calib_system(atd[nm].get_system_calfile(), atd[nm].get_sensor_chopper(), atd[nm].get_samplefreq(), atd[nm].get_system_gain(), 0, 0, "", "");
        }

        tmp_str = atd[nm].get_sensor_calfile();
        // want always lower case filenames
        lower(tmp_str);

        if (!tmp_str.size()) {
          tmp_str = atd[nm].gen_calfile_name();
          lower(tmp_str);
          atd[nm].set_sensor_calfile(tmp_str);
        }

        cerr << "Sensor : " << tmp_str << endl;

        if (tmp_str.find("07") != tmp_str.npos)
          assx[nm].set_calibsensor(&mfs07_coil);

        else if (tmp_str.find("mfs06") != tmp_str.npos)
          assx[nm].set_calibsensor(&mfs_coil);

        else if (tmp_str.find("mfs05") != tmp_str.npos)
          assx[nm].set_calibsensor(&mfs_coil);

        else if (tmp_str.find("lts") != tmp_str.npos)
          assx[nm].set_calibsensor(&lts_squid);

        else if (tmp_str.find("fgs01") != tmp_str.npos)
          assx[nm].set_calibsensor(&fgs01_flux);

        else if (tmp_str.find("fgs02") != tmp_str.npos)
          assx[nm].set_calibsensor(&fgs02_flux);
        //        if (tmp_str.find("fgs01") != tmp_str.npos) assx[nm].set_calibsensor(&mfs_coil);
        else if (tmp_str.find("kim") != tmp_str.npos)
          assx[nm].set_calibsensor(&mfs_coil);

        else if (tmp_str.find("bf6") != tmp_str.npos)
          assx[nm].set_calibsensor(&plain_cal);

        else if (tmp_str.find("p30") != tmp_str.npos)
          assx[nm].set_calibsensor(&plain_cal);
        else if (tmp_str.find("p50") != tmp_str.npos)
          assx[nm].set_calibsensor(&plain_cal);

        else if (tmp_str.find("bf") != tmp_str.npos)
          assx[nm].set_calibsensor(&emi_bf_coil);

        else if (tmp_str.find("efp05") != tmp_str.npos)
          assx[nm].set_calibsensor(&efp05_elec);

        else if ((tmp_str.find("fgs01") != tmp_str.npos) && theo_trf) {
          assx[nm].read_calib_sensor("", atd[nm].get_sensor_chopper(), atd[nm].get_samplefreq(), atd[nm].get_system_gain(), 0, 0, "", "");
        }

        else if ((tmp_str.find("fgs02") != tmp_str.npos) && theo_trf) {
          assx[nm].read_calib_sensor("", atd[nm].get_sensor_chopper(), atd[nm].get_samplefreq(), atd[nm].get_system_gain(), 0, 0, "", "");
        }
        if (auto_trf) {
          cerr << "main::auto transfer function" << endl;
          tmp_str = atd[nm].get_sensor_calfile();
          lower(tmp_str);
          assx[nm].read_calib_sensor(tmp_str, atd[nm].get_sensor_chopper(), atd[nm].get_samplefreq(), atd[nm].get_system_gain(), 0, 0, "", "");
        }
        if (theo_trf) {
          cerr << "main::theroretical transfer function" << endl;
          assx[nm].theoretical("", atd[nm].get_sensor_chopper(), atd[nm].get_samplefreq(), atd[nm].get_system_gain(), 0, 0, "", "");
        }

      } else if (trf_file.size()) {
        cerr << "main::assigning manual transfer function NEEDS plain!!!!!!!!!!!" << endl;

        assx[nm].set_calibsensor(&fgs01_flux);
        assx[nm].read_calib_sensor(trf_file, atd[nm].get_sensor_chopper(), atd[nm].get_samplefreq(),
                                   atd[nm].get_system_gain(), 0, 0, "", "");
      }

      /***************************************************************************
       *                                                                         *
       *   calculate the spectra an remove the data vector                       *
       *                                                                         *
       *   from the timeseries file in order save memory                         *
       *                                                                         *
       *                                                                         *
       ***************************************************************************/
      assx[nm].calc_sp(hanning, wdetrend, atd[nm].data(), atd[nm].get_select(), extra_scale[nm], inverse_trf, dump_trf);
      atd[nm].kill_data();
    } // end of simple complex spectra which are ALWAYS needed

    if (spectra_add || spectra_sub || spectra_mul || spectra_div) {

      for (nm = 1; nm < allchan; nm++) {
        if (spectra_add)
          assx[0] += assx[nm];
        if (spectra_sub)
          assx[0] -= assx[nm];
        if (spectra_mul)
          assx[0] *= assx[nm];
        if (spectra_div)
          assx[0] /= assx[nm];
      }

      /*            cerr << "main-> results of spectral math written to ";

         assx[0].mean("complex");
         assx[0].write("complex", 1, 1, ul, ll, spwadd, spwmul, last_file_result);

         cerr << last_file_result;

         last_file_result += "ap";

         assx[0].write("trfmtx", 1, 1, ul, ll, spwadd, spwmul, last_file_result);

         cerr << "  and " << last_file_result << endl;

         exit(0);

      */
    }

    if (!back) {
      for (nm = 0; nm < allchan; nm++) {

        if (parzen) {
          assx[nm].set_flist_parzen(flist, parzen);
          assx[nm].mean("ampl_prz");
        }

        assx[nm].mean("ampl");
        assx[nm].mean("power");
        assx[nm].mean("phase");

        // neu heute
        if (calc_median) {
          assx[nm].single("ampl");
          assx[nm].sstat("ampl", "max", 0);
          assx[nm].sstat("ampl", "min", 0);
          assx[nm].sstat("ampl", "median", range);
        }

        if (mt) {

          assx[nm].single("conj");
        }
      }
    }

    cerr << endl
         << "main -> generating output files " << endl;

    for (nm = 0; nm < allchan; nm++) {

      // make sure that the complete file was done

      if (back) {
        valarray<double> time_series(assx[nm].get_size()); // size can be wl * stacks

        fname = atd[nm].return_outputfilename("_scaled.dat");
        assx[nm].calc_ts(time_series, scale_f);

        if (ascii) {
          cerr << "main -> writing " << fname << "   ";
          my_valarray::vwrite(time_series, fname, 0, 0);

          cerr << "done " << endl;
        }

        // run must be 01 or 02 .. 99
        if ((run.size() == 2) || atsoutfile.size()) {

          string new_file_ats;
          if (run.size() == 2) {
            if (atd[0].is07) {
              cerr << "try set run" << run;
              atd[nm].change_run(run);
              atd_result.my_name_is = atd[nm].my_name_is;
            } else {
              if (SerNoADU.size() == 3) {
                new_file_ats = SerNoADU;
                new_file_ats += atd[nm].my_name_is.substr(3, 1);
              } else
                new_file_ats = atd[nm].my_name_is.substr(0, 4);

              new_file_ats += run;
              new_file_ats += atd[nm].my_name_is.substr(6, 11);
              atd[nm].my_name_is = new_file_ats;
            }
          } else if (atsoutfile.size()) {
            atd[nm].my_name_is = atsoutfile;
          }

          if (!out_dir.size())
            temp_str = atd[nm].my_dir_is + atd[nm].my_name_is;
          else
            temp_str = out_dir + atd[nm].my_name_is;

          // I use the same file - only repalcing the data vector
          atd[nm].set_data(time_series);

          // putlsb efield coordinates ....sensor type and calfile

          //        outputfile.open(temp_str.c_str(), ios::noreplace);
          outputfile.open(temp_str.c_str(), ios::binary | ios::out);
          if (outputfile) {
            cerr << "main -> writing ats times series file (lsb: " << atd[nm].get_lsb() << ") " << temp_str << "....";
            atd[nm].setheader(&ats80);
            atd[nm].write_headers(outputfile);
            atd[nm].write(outputfile);
            outputfile.close();
            cerr << "done " << endl;
          } else {
            cerr << endl
                 << "main -> " << temp_str << " already exists! Nothing done." << endl
                 << endl;
          }
        }

      } // end backwards

      if (!nspw) {

        if (use_flist) {
          defaultfreqs.open(flist_name.c_str());
          if (defaultfreqs) {
            my_valarray::read_ukwn_1(defaultfreqs, flist);
            defaultfreqs.close();
          } else {
            cerr << "main -> can not open " << flist_name << "for reading frequencies" << endl;
            exit(0);
          }
        }

        fname = atd[nm].return_outputfilename("_ampl.dat");
        cerr << "main -> writing " << fname << "   ";
        assx[nm].write("ampl", 1, 1, ul, ll, spwadd, spwmul, fname, flist);
        cerr << "done " << endl;
        if (calib) {
          fname = atd[nm].return_outputfilename("_phase.dat");
          cerr << "main -> writing " << fname << "   ";
          assx[nm].write("phase", 1, 1, ul, ll, spwadd, calib, fname, flist);
          cerr << "done " << endl;

          fname = atd[nm].return_outputfilename("_cal.dat");
          cerr << "main -> writing " << fname << "   ";
          assx[nm].write("cal", 1, 1, ul, ll, spwadd, calib, fname, flist);
          cerr << "done " << endl;
        }
        //                fname = atd[nm].return_outputfilename("_power.dat");
        //                cerr << "main -> writing " << fname << "   ";
        //                assx[nm].write("power", 1, 1, ul, ll, spwadd, spwmul, fname, flist);
        //                cerr << "done " << endl;

        if (parzen) {
          fname = atd[nm].return_outputfilename("_ampl_prz.dat");
          cerr << "main -> writing " << fname << "   ";
          assx[nm].write("ampl parzen", 1, 1, ul, ll, spwadd, spwmul, fname, flist);
          cerr << "done " << endl;
        }
        // neu heute
        if (calc_median) {
          fname = atd[nm].return_outputfilename("_ampl_max.dat");
          cerr << "main -> writing " << fname << "   ";
          assx[nm].write("ampl max", 0, 1, ul, ll, spwadd, spwmul, fname, flist);
          cerr << "done " << endl;

          fname = atd[nm].return_outputfilename("_ampl_min.dat");
          cerr << "main -> writing " << fname << "   ";
          assx[nm].write("ampl min", 0, 1, ul, ll, spwadd, spwmul, fname, flist);
          cerr << "done " << endl;

          fname = atd[nm].return_outputfilename("_ampl_med.dat");
          cerr << "main -> writing " << fname << "   ";
          if (range)
            assx[nm].write("ampl median range", 0, 1, ul, ll, spwadd, spwmul, fname, flist);
          else
            assx[nm].write("ampl median", 0, 1, ul, ll, spwadd, spwmul, fname, flist);
          cerr << "done " << endl;
        }
      }
    }

    /***************************************************************************
     *                                                                         *
     *   use a different loop because 4-5 files are always needed              *
     *                                                                         *
     *   for calculating one MT site                                           *
     *                                                                         *
     *                                                                         *
     ***************************************************************************/

    // TRY FOR MT
    if (mt) {

      ifstream defaultfreqs;

      defaultfreqs.open("default.flst");
      cerr << "main->reading default frequency list for MT, default.flst" << endl;
      my_valarray::read_ukwn_1(defaultfreqs, flist);
      defaultfreqs.close();
      cerr << "done" << flush;
      string out_file;

      cerr << "main r phi1" << endl;

      //         size_t f_begin = 0, f_end = 0;

      mtstd mtres;
      mtres.init(assx[0], flist, parzen);
      cerr << "main r phi2" << endl;
      valarray<double> ra, rayx, ph, phyx;
      mtres.z(assx[0], assx[1], assx[2], assx[3], assx[4]);

      cerr << "main r phi" << endl;
      mtres.rhoa_phi(ra, ph, 0, 0, "zxy");
      mtres.rhoa_phi(rayx, phyx, 0, 0, "zyx");

      valarray<double> invflist(mtres.flist.size());
      invflist = mtres.flist;
      invflist = 1. / invflist;

      cerr << "main write" << endl;
      ra *= 1000000.;
      ph += 180.;
      rayx *= 1000000.;
      phyx += 180.;

      cerr << ra[2] << "  " << ra[3] << "  " << ra[4] << " ende" << endl;
      out_file = "test_raxy_sec_" + atd[0].return_bandindex_mtx(atd[0].get_samplefreq()) + ".dat";
      my_valarray::vwrite(invflist, ra, out_file, 1, 0);

      out_file = "test_rayx_sec_" + atd[0].return_bandindex_mtx(atd[0].get_samplefreq()) + ".dat";
      my_valarray::vwrite(invflist, rayx, out_file, 1, 0);

      out_file = "test_raxy_hz_" + atd[0].return_bandindex_mtx(atd[0].get_samplefreq()) + ".dat";
      my_valarray::vwrite(mtres.flist, ra, out_file, 1, 0);

      out_file = "test_rayx_hz_" + atd[0].return_bandindex_mtx(atd[0].get_samplefreq()) + ".dat";
      my_valarray::vwrite(mtres.flist, rayx, out_file, 1, 0);

      out_file = "test_ph" + atd[0].return_bandindex_mtx(atd[0].get_samplefreq()) + ".dat";
      my_valarray::vwrite(mtres.flist, ph, out_file, 1, 0);
    }

    /***************************************************************************
     *                                                                         *
     *   start with the coherency analysis                                     *
     *                                                                         *
     *   coherency and nois can only be calculated between two channles        *
     *                                                                         *
     *                                                                         *
     ***************************************************************************/

    if ((allchan == 2) && (assx[0].get_stacks() < 2) && coh)
      cerr << "main -> you need more than 2 stacks for coherencies" << endl;
    else if (allchan == 2) {

      // hier nicht nm ver�dern , keine unabh�gige Schleife
      coherency atsc;
      cerr << "main -> calculating coherencies" << endl
           << flush;
      atsc.init(assx[0]);

      if (!assx[0].is_mean_pwr)
        assx[0].mean("power");
      if (!assx[1].is_mean_pwr)
        assx[1].mean("power");

      cerr << "main -> calculating coherencies" << endl
           << flush;

      if (!median_coh)
        atsc.coh(1, 0, 0, assx[0], assx[1]);
      else
        atsc.coh(1, 1, range_coh, assx[0], assx[1]);

      //     cerr << atd[0].my_out_dir_is << atd[0].my_name_is << " " <<  atd[1].my_name_is << endl;
      tfnewext((atd[0].my_out_dir_is + atd[0].my_name_is), atd[1].my_name_is, fname, "_", "_coh.dat");
      cerr << "main -> writing " << fname << "   ";
      atsc.write("coh", 1, ul, ll, fname);
      if (median_coh) {
        tfnewext((atd[0].my_out_dir_is + atd[0].my_name_is), atd[1].my_name_is, fname, "_", "_coh_med.dat");
        cerr << "main -> writing " << fname << endl;
        atsc.write("median", 1, ul, ll, fname);

        if (1000 * range_coh) {
          tfnewext((atd[0].my_out_dir_is + atd[0].my_name_is), atd[1].my_name_is, fname, "_", "_coh_med_range.dat");
          cerr << "main -> writing " << fname << endl;
          atsc.write("medianrange", 1, ul, ll, fname);
        }
      }
      cerr << "  done" << endl;

      for (nm = 0; nm < allchan; nm++) {
        fname = atd[nm].return_outputfilename("_noise.dat");
        cerr << "main -> writing " << fname << "   ";

        outputfile.open(fname.c_str());
        for (l = 1 + ll; l < wl / 2 - ul; l++)
          outputfile << l * atd[nm].get_samplefreq() / wl <<
              //     "\x9" << (sqrt(1- pow(atsc.sqrt_coh[l],2.))) * assx[nm].smean_ampl[l]  << endl;
              "\x9" << (1 - atsc.sqrt_coh[l]) * assx[nm].smean_ampl[l] << endl;
        outputfile.close();

        cerr << "done " << endl;
      }

    } // nfiles == 2

  } // end spectra    }

  cerr << endl
       << "************finshed final***********************************" << endl
       << endl;

  // return EXIT_SUCCESS;  // cygwin does not like this
  exit(0);
}
