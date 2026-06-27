/***************************************************************************
                          date_utils.cpp  -  description
                             -------------------
    begin                : Tue Dec 10 2002
    copyright            : (C) 2002 by B. Friedrichs
    email                : bernhard.friedrichs@coopertools.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "date_utils.h"

int find_sync_times(vector<long int>& DateTimes, const vector<size_t> total_smpls, vector<size_t>& skips, vector<size_t>& smpls,
                        const double smpl_f, const size_t& wdl, const int synctype) {


long int start_a, stop_a, isample_s;
int myerror = 0;
size_t i;
string dinfo;
//char leer = " ";


vector<long int> NewDateTimes(DateTimes.size()), diffs(DateTimes.size()), shifts(DateTimes.size()),
                          EndTimes(DateTimes.size()), diff_ends(DateTimes.size());;


  isample_s = (long int)(1./smpl_f);                                        // sampling period in seconds
  for(i = 0; i < DateTimes.size(); i++) NewDateTimes[i] = DateTimes[i] + (long int)(skips[i] / smpl_f);
  start_a = *max_element(NewDateTimes.begin(), NewDateTimes.end());        // find latest start time


                                                                     // caluclate stop times directly
  for(i = 0; i < DateTimes.size(); i++)  EndTimes[i] = DateTimes[i] +  (long int)(total_smpls[i] / smpl_f);
  stop_a = *min_element(EndTimes.begin(), EndTimes.end());           // find earliest stop time


  if (start_a < stop_a +  int(wdl / smpl_f) )                           // check
    cerr << "find_sync_times -> first possible sync start at: " << GMTtime(start_a);
  else {
    cerr << "find_sync_times -> no overlapping time segment (including possible fft) " << endl;
    myerror++;
  }

  for(i = 0; i < DateTimes.size(); i++) {                           // show info (always)
    dinfo =  GMTtime(NewDateTimes[i]);
    dinfo[dinfo.size()-1] = ' ';
    dinfo += "- ";
    dinfo += GMTtime(EndTimes[i]);
    cerr << "find_sync_times info ->  " << dinfo;

  }
  if (myerror) exit(0);

  for(i = 0; i < DateTimes.size(); i++) {                             // find the offsets to the lates start time

    diffs[i] = start_a - NewDateTimes[i];

    // do we have to shift the latest start time also .... that will be impossible!!!

    if (smpl_f < 1.) {                        // for higher sampling rates we always fin a sync start
      shifts[i] = diffs[i] % isample_s;
      if (shifts[i]) {
          cerr << "find_sync_times -> remaining offset is " << shifts[i] << ", which can not be adjusted with "
          << isample_s << " sampling period " << endl;
          cerr << "solution: filter the higher band to a new start time (16, 32, 64, 512, 1024, 2048 sec grid e.g)" << endl;

          myerror++;
      }

      if (myerror) cerr << "find_sync_times -> error: differnce and remainer " << diffs[i] << ",  " << shifts[i] << endl;;
    }
  }

  if (myerror) exit(0);                                               // terminate here

  // sync start times
  myerror = 0;
  if (synctype) {
    cerr << "find_sync_times -> new skips: ";
                                                  // 1 sync start only
    for(i = 0; i < DateTimes.size(); i++) {
      cerr << skips[i] << " old -> new ";
      skips[i] += size_t (double (diffs[i]) * smpl_f);
      cerr <<  skips[i] << ";   ";

    }
    cerr << endl;
  }



  if (synctype == 2) {                                                    // sync start and stop -> same n_samples!
    for(i = 0; i < DateTimes.size(); i++) {

     smpls[i] = int((stop_a - start_a) * smpl_f);
     cerr << smpls[i] << endl;
     if (smpls[i] != smpls[0] ) {
        cerr << "find_sync_times (sync 2) -> I have calculated different samples ... that must be wrong; stopping!" << endl;
        exit(0);

     }

    }

    dinfo = GMTtime(stop_a);
    dinfo[dinfo.size()-1] = ' ';
    cerr << "find_sync_times (sync 2) -> set stop time to " << dinfo << " and samples to " << smpls[0] << endl;
  }





return 1;

}




int find_grid_time(const long int& DateTime, size_t& skip, const size_t& smpls,
                    const double& smpl_f, const int& reduct, const int& filter_length, const int& raster) {



if (raster == -1)  return 1;

double new_smpl_f = smpl_f / reduct;

size_t fil_offset = size_t(double(filter_length) /(2. * smpl_f));          // offset in s caused by filtering
  // assuming that the data logger can start onl to a full second;
if (new_smpl_f >= 1 ) {

  cerr << "find_grid_time -> new sampling is still above 1Hz! " << endl;

return 1;
}


 //  double full_second = (skip / smpl_f) - floorl((skip / smpl_f));

   if ((skip / smpl_f) - floorl((skip / smpl_f)) ) {

     cerr << "find_grid_time -> given skip samples must fit to a full second!" << endl;
     exit(0);

   }

   if (!raster) {

     int new_ismpl_s = int(1 / new_smpl_f);                            // new sampling period
     int old_ismpl_s = int(1. / smpl_f);                               // old sampling period
     long offset = (DateTime + int(skip / smpl_f) + fil_offset) % new_ismpl_s;
     long sec_to_grid = new_ismpl_s - offset;

     if (new_ismpl_s > 128) {
       cerr << "find_grid_time -> new sampling period is greater than 128s, here " << new_ismpl_s << endl;
     }

     if ( !offset ) return 1;
     else {

       skip += int(sec_to_grid * smpl_f);   // that is the final shift in pts of the unfiltred time series we need to the next grid point
       cerr << "find_grid_time -> filtred time series set to " << new_ismpl_s << "s grid";
       cerr << ", skipping " << skip << " samples" << endl;
//       cerr << DateTime << "  " << offset << "  " << sec_to_grid << "  " << skip << endl;


       // neu heute - warum will ich zweimal setzen?  ausgeklammert

/*       if ( old_ismpl_s && (sec_to_grid % old_ismpl_s)) {
         cerr << "find_grid_time -> error: raster from new sample rate is " << new_ismpl_s << "s and time difference: " << sec_to_grid;
         cerr << " and actual sampling rate " << old_ismpl_s << "s" << endl;
         cerr << "there will be no chance to synchronize - except re-filtring the higher band to a new raster or resampling" << endl;
         exit(0);
       }
*/
     }
   }

   else {
//    int new_ismpl_s = int(1/new_smpl_f);
    int old_ismpl_s = int(1./smpl_f);
    long offset = (DateTime + int(skip / smpl_f) + fil_offset ) % raster;
    long sec_to_grid = raster - offset;

    skip +=  int( sec_to_grid * smpl_f);

    cerr << "find_grid_time -> raster: skipping " << skip << " samples" << endl;
    if ( old_ismpl_s && (sec_to_grid % old_ismpl_s)) {
      cerr << "find_grid_time -> error: raster is " << raster << "s and time difference: " << sec_to_grid;
      cerr << " and actual sampling rate " << old_ismpl_s << "s" << endl;
      cerr << "there will be no chance to synchronize - except re-filtring the higher band to a new raster or resampling" << endl;
      exit(0);
    }

   }

return 1;
}




