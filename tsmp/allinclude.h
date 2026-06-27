#ifndef ALLINCLUDE_H
#define ALLINCLUDE_H


/***************************************************************************
* basically for the byte swap                                             *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
*                                                                         *
***************************************************************************/



#include <climits>    // INT_MAX
#include <cfloat> 			//DBL_MAX

#ifdef __CYGWIN__
 #define floorl floor
#endif



// uncomment to disable!!!
//#define CLASSDBG 0
//#define TRFCONTROL
#define GOEIMPORTCTRL
#define GEN_HEAD
#define FFTCHECK
#define PARZENCHK
#define EMIIMPORTCONTROL

#define ATSHEADERVERSION 73
#define UNITS_TIMESERIES            mV
#define UNITS_CALIBRATION_AMPLITUDE mV/nT
#define UNITS_CALIBRATION_PHASE     deg
#define UNITS_CALIBRATION_FREQUENCY Hz
#define UNITS_MAGNETIC_FIELD        nT
#define UNITS_ELECTRIC_FIELD        mV/km
#define UNITS_IMPEDANCE_TENSOR      m/s
#define UNITS_MAGNETIC_SPECTRA      nT/(sqrt(Hz)
#define UNITS_ELECTRIC_SPECTRA      mV/sqrt(Hz)

#define GOERECLENGTH  256		// 256 byte is one record in the GOE format
#define GOEDATASIZE		2		// 2 byte or 16 bit data in GOE format




#ifdef WORDS_BIGENDIAN
    #define swap_ats									// say that we do it in general
    #ifdef HAVE_BYTESWAP_H
    #define use_shift_to_swap         // use the gcc routine if possible
    #endif
    #ifndef HAVE_BYTESWAP_H						// typical user have odd compilers
    #define use_shift_to_swap
        #define bswap_32(x) \
     ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) |		      \
      (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

    #define bswap_16(x) \
     ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8))

    #endif

#endif


// uncomment these line if the  integer conversion does not work
// #undef use_shift_to_swap
// #define use_template_to_swap

/*end machine dependend part */



// empty double; also used for Tensor Elements
// pre defined in the EDI file format
const double empty_val = 1.000000000000E+32;

// used for setting e-field lenth to 0
const double assume_zero_length = 10E-9;

const double mue0 = 1.256637061E-6;

// comment this line if you do not want to use mysql and online processing
//#define myMySQL
//#define myGSL
// this file must be changed and submitted in order to execute the SVN macro below !!!!!!!!!!!!!!!!!!!

#endif // ALLINCLUDE_H

