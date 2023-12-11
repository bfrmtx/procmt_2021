#ifndef ATSFILE_H
#define ATSFILE_H

#include <QtGlobal>
#include <QString>


/*! @todo
 UTM Zone number
 UTM letter
 Tx times six double at least
 Tx base freq

*/
typedef struct ATSComments80_s {
  char achClient      [16];               //!< 000 h
  char achContractor  [16];               //!< 010 h
  char achArea        [16];               //!< 020 h
  char achSurveyID    [16];               //!< 030 h
  char achOperator    [16];               //!< 040 h
  char achReserved   [112];               //!< 050 h
  char achXmlHeader   [64];               //!< 0C0 h
  char achComments   [512];               //!< 100 h
} C_ATSComments80;



typedef struct ATSHeader80_s {
  qint16    siHeaderLength;               //!< 000h
  qint16    siHeaderVers;                 //!< 002h

// This information can be found in the ChannelTS datastructure
  quint32   iSamples;                     //!< 004h amount of samples (each is a 32bit int) in the file
  float     rSampleFreq;                  //!< 008h sampling frequency in Hz
  quint32   uiStartDateTime;              //!< 00Ch unix TIMESTAMP (changed to unsigned!)
  double    dblLSBMV;                     //!< 010h
  qint32    iGMTOffset;                   //!< 018h
  float     rOrigSampleFreq;              //!< 01Ch sampling frequency in Hz as ORIGINALLY recorded; this value should NOT change (for example after filtering)

//The required data could probably found in the HardwareConfig
  qint16    siADUSerNum;                  //!< 020h
  qint16    siADCSerNum;                  //!< 022h
  char      byChanNo;                     //!< 024h
  char      byChopper;                    //!< 025h Chopper On/Off

// Data from XML Job-specification
  char      abyChanType   [2];            //!< 026h
  char      abySensorType [6];            //!< 028h
  qint16    siSensorSerNum;               //!< 02Eh

  float     rPosX1;                       //!< 030h e.g South negative
  float     rPosY1;                       //!< 034h e.g West negative
  float     rPosZ1;                       //!< 038h e.g bottom
  float     rPosX2;                       //!< 03Ch e.g.North positive
  float     rPosY2;                       //!< 040h e.g.East positive
  float     rPosZ2;                       //!< 044h e.g. top
  float     rDipLength;                   //!< 048h e.g. to be calculated; should not be used
  float     rAngle;                       //!< 04Ch e.g. to be calculated; should not be used

// Data from Selftest ?
  float     rProbeRes;                    //!< 050h
  float     rDCOffset;                    //!< 054h
  float     rPreGain;                     //!< 058h e.g. Gain Stage 1
  float     rPostGain;                    //!< 05Ch e.g. Gain Stage 2

// Data from status information ?
  qint32    iLatitude;                    //!< 060h
  qint32    iLongitude;                   //!< 064h
  qint32    iElevation;                   //!< 068h
  char      byLatLongType;                //!< 06Ch U, G user, GPS
  char      byAddCoordType;               //!< 06Dh U = UTM, G = Gauss Krueger
  qint16    siGaussRefMeridian;           //!< 06Eh
  double    dblHochwert;                  //!< 070h also xcoord
  double    dblRechtswert;                //!< 078h also ycoord
  char      byGPSStat;                    //!< 080h
  char      byGPSAccuracy;                //!< 081h
  qint16    iUTCOffset;                   //!< 082h
  char      abySystemType[12];            //!< 084h

// Data from XML-Job specification
  char      abySurveyHeaderName [12];     //!< 090h
  char      abyMeasType          [4];     //!< 09Ch

//TODO[OKH]
// Next three fields will not be supported any more.
  double    DCOffsetCorrValue   ;         //!< 0A0h DAC offset double
  qint8     DCOffestCorrOn;               //!< 0A8h DC offset was switched on (1) or off(0)
  qint8     InputDivOn;                   //!< 0A9h inputput divider on(1) off(0); e.g when coil was connected
  qint16    not_used_var;                 //!< 0AAh
  char      abySelfTestResult [2];        //!< 0ACh
  char      abyReserved5      [2];        //!< 0AEh

  //qint16 calentries // max 128 entries

//TODO[OKH]
// Were the following fields ever used ?
  qint16    siCalFreqs;                   //!< 0B0h
  qint16    siCalEntryLength;             //!< 0B2h
  qint16    siCalVersion;                 //!< 0B4h
  qint16    siCalStartAddress;            //!< 0B6h
  char      abyLFFilters [8];             //!< 0B8h

  char      abyADU06CalFilename  [12];    //!< 0C0h
  qint32    iADUCalTimeDate;              //!< 0CCh
  char      abySensorCalFilename [12];    //!< 0D0h
  qint32    iSensorCalTimeDate;           //!< 0DCh

  float     rPowerlineFreq1;              //!< 0E0h
  float     rPowerlineFreq2;              //!< 0E4h
  char      abyHFFilters[8];              //!< 0E8h


  // Unused ?
  double   OriginalLSBMV;                 //!< 0F0h orig lsb from selftest without gains; used for ADC values
  qint32    unsed_var;                    //!< 0F8h
  char     abyADBBoardType[4];            //!< 0FCh LF HF or MF

 //!< Comes from XML-Job spec.
  C_ATSComments80 tscComment;             //!< 100h
} C_ATSHeader80;


class ATSFile
{
    public:
        ATSFile(const QString qstrTargetFileName);

        bool isOpen (void) const;

        C_ATSHeader80 clATSHeader;

        QString getSensorType   (void) const;
        QString getSensorSerial (void) const;

        void resetPos (void);

        bool getNextTSBuffer (const unsigned int uiNumSamples, QVector<double>& qvecTSData) const;

    private:

        QString qstrTargetFile;

        bool bFileOpen;

        unsigned int uiActPos;

        bool readHeader ();
};

#endif // ATSFILE_H
