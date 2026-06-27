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
#ifndef ATSHEADER75_H
#define ATSHEADER75_H

#include "itsheader.h"
#include <QtCore/QtCore>
#include "allinclude.h"


typedef struct ATSComments75_s {
  char achClient      [16];               //!< 000 h
  char achContractor  [16];               //!< 010 h
  char achArea        [16];               //!< 020 h
  char achSurveyID    [16];               //!< 030 h
  char achOperator    [16];               //!< 040 h
  char achReserved   [112];               //!< 050 h
  char achXmlHeader   [64];               //!< 0C0 h
  char achComments   [512];               //!< 100 h
} C_ATSComments75;



typedef struct ATSHeader75_s {
  qint16    siHeaderLength;               //!< 000h
  qint16    siHeaderVers;                 //!< 002h

// This information can be found in the ChannelTS datastructure
  quint32   iSamples;                     //!< 004h amount of samples (each is a 32bit int) in the file
  float     rSampleFreq;                  //!< 008h sampling frequency in Hz
  qint32    iStartDateTime;               //!< 00Ch
  double    dblLSBMV;                     //!< 010h
  qint32    iGMTOffset;                   //!< 018h
  float     rOrigSampleFreq;              //!< 01Ch sampling frequency in Hz as ORIGINALLY recorded; this value should NOT change (for example after filtering)

//The required data could probably found in the HardwareConfig
  qint16    siADUSerNum;                  //!< 020h
  qint16    siADCSerNum;                  //!< 022h
  char      byChanNo;                     //!< 024h
  char      byChopper;                    //!< 025h   Chopper On/Off

// Data from XML Job-specification
  char      abyChanType   [2];            //!< 026h
  char      abySensorType [6];            //!< 028h
  qint16    siSensorSerNum;               //!< 02Eh

  float     rPosX1;                       //!< 030h
  float     rPosY1;                       //!< 034h
  float     rPosZ1;                       //!< 038h
  float     rPosX2;                       //!< 03Ch
  float     rPosY2;                       //!< 040h
  float     rPosZ2;                       //!< 044h
  float     rDipLength;                   //!< 048h
  float     rAngle;                       //!< 04Ch

// Data from Selftest ?
  float     rProbeRes;                    //!< 050h
  float     rDCOffset;                    //!< 054h
  float     rPreGain;                     //!< 058h
  float     rPostGain;                    //!< 05Ch

// Data from status information ?
  qint32    iLatitude;                    //!< 060h
  qint32    iLongitude;                   //!< 064h
  qint32    iElevation;                   //!< 068h
  char      byLatLongType;                //!< 06Ch  U, G user, GPS
  char      byAddCoordType;               //!< 06Dh  U = UTM, G = Gauss Krueger
  qint16    siGaussRefMeridian;           //!< 06Eh
  double    dblHochwert;                  //!< 070h
  double    dblRechtswert;                //!< 078h
  char      byGPSStat;                    //!< 080h
  char      byGPSAccuracy;                //!< 081h
  qint16    iUTCOffset;                   //!< 082h
  char      abySystemType[12];            //!< 084h

// Data from XML-Job specification
  char      abySurveyHeaderName [12];     //!< 090h
  char      abyMeasType          [4];     //!< 09Ch

//TODO[OKH]
// Next three fields will not be supported any more.
  char      abyLogFileName   [12];        //!< 0A0h
  char      abySelfTestResult [2];        //!< 0ACh
  char      abyReserved5      [2];        //!< 0AEh

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
  float    rCSAMTFreq;                    //!< 0F0h
  qint16   siCSAMTBlocks;                 //!< 0F4h
  qint16   siCSAMTStacksPBlock;           //!< 0F6h
  qint32   iCSAMTBlockLength;             //!< 0F8h
  char     abyADBBoardType[4];            //!< 0FCh

 //!< Comes from XML-Job spec.
  C_ATSComments75 tscComment;             //!< 100h
} C_ATSHeader75;










/**
@author Bernhard Friedrichs
*/
class ATSHeader75 : public ITSHeader
{
public:


    ~ATSHeader75();
     virtual size_t readheader(ifstream& ifs);

};

#endif
