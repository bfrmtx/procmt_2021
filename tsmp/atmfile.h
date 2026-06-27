/***************************************************************************
                          atmfile.h  -  description
                             -------------------
    begin                : Fri Jan 4 2002
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

#ifndef ATMFILE_H
#define ATMFILE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <bitset>
#include <cstdlib>
#include "allinclude.h"

using namespace std;


/**
  *@author B. Friedrichs
  */


/***************************************************************************
*                                                                         *
* ATM file is the binary tag file for mapros                              *
*                                                                         *
* used to select / deselect parts of the time series                      *
*                                                                         *
*                                                                         *
***************************************************************************/
  

// header defined as used in MAPROS
struct atmheader {
  short int siHeaderLength;
  short int siHeaderVers;
  unsigned int iSamples;
};


class atmfile {
public:
	
	atmfile();
	~atmfile();
	string fname;
	fstream myfile;

		// allow user to ask for status	
	size_t samples();
	bool file_open();
	bool file_new();
		// main tasks
		
		// always returns the samples inside the atmfile
  size_t open(const string& Filename);
	
	// allow cerr << atmfile.close() << " samples read/written" << endl;
	size_t close();
	size_t read(vector<bool>& vb);
	size_t write(const vector<bool>& vb);
	
private:

	size_t file_size;
	bool is_open, is_new;
	bitset<8> bs;
	atmheader ath;
};


const unsigned short int charbits = 8 * sizeof(char);
const unsigned short int AtmHeaderVer = 10;
const unsigned short int AtmHeaderLength = 8;

#endif
