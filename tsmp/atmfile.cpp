/***************************************************************************
                          atmfile.cpp  -  description
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

#include "atmfile.h"

atmfile::atmfile() {

    ath.siHeaderLength = 0;
    ath.siHeaderVers = 0;
    ath.iSamples = 0;
    is_open = 0;
    is_new = 0;
}

atmfile::~atmfile() {
    if (is_open)
        myfile.close();
}


size_t atmfile::samples() {
    return ath.iSamples;
}
bool atmfile::file_open() {
    return is_open;
}
bool atmfile::file_new() {
    return is_new;
}


size_t atmfile::close() {
    if (myfile)
        myfile.close();
    is_open = 0;
    is_new = 0;
    return ath.iSamples;
}

/***************************************************************************
 * try to read the header                                                  *
 * if file does not exist or has zero length prepare for create            *
 * else ckeck integrety of header (read or create)                         *
 * if header is ok ckeck the corresponding file size                       *
 * if file size is also ok file is ready to read, otherwise ready to write *
 * new                                                                     *
 ***************************************************************************/


size_t atmfile::open(const string& Fname) {
    // save the filename for messages
    fname = Fname;
    myfile.close(); // need to FIX this BUG
    myfile.clear();
    // forget the ios::noreplace option and open read mode
    myfile.open(fname.c_str(), ios::binary | ios::in | ios:: out);
    if (myfile) {
        is_open = 1;
        // read binary header
        myfile.read((char*) &ath, sizeof(ath));
        // can not read a header; prepare for new
        if (myfile.eof() || myfile.fail() ) {
            cerr << "atmfile::open -> file "<< fname << " does not contain a header; prepare for open a new file" << endl;
            is_new = 1;
            is_open = 1;
            myfile.close();
            myfile.clear();
            myfile.open(fname.c_str(), ios::binary | ios::out);
            return 0;    // zero samples
        }
        // else check Header entries
        if (ath.siHeaderVers <= 10 && ath.siHeaderLength == AtmHeaderLength) {
            // check file size
            myfile.seekg(0L, ios::end);
            file_size = myfile.tellg();
            myfile.seekg(0L, ios::beg);

            // file can not be smaller than iSamples*8 or bigger than iSamples*8 + 8 + header?
            if (((file_size - sizeof(ath)) * 8) < ath.iSamples )  {
                //						||	(ath.iSamples < (((file_size - sizeof(ath)) * 8) - 64) )) {
                cerr << "atmfile::open -> file "<< fname << "  has different size as defined by samples " << endl;
                cerr << "atmfile::open -> (file_size -header)* 8: " << (file_size -sizeof(ath))* 8 << "  samples:" << ath.iSamples << "  header:" << sizeof(ath) << endl;
                cerr << "atmfile::open -> file "<< fname << "  can be used for write only!" << endl;

                myfile.close();
                myfile.open(fname.c_str(), ios::binary | ios::in | ios::out);
                is_new = 1;		// new file can not be read in the read function
                is_open = 1;
                return 0;
            }

            // so file size seems ok an we take the samples given in the header
            // this is it as it always should be if atm file exists on hdd
            else {
                myfile.close();
                // prepare for read write
                myfile.open(fname.c_str(), ios::binary | ios::in | ios::out);
                return ath.iSamples;
            }
        } else {
            cerr << "atmfile::open -> file "<< fname << "  has wrong header??" << endl;
            exit(0);
        }
    } else {
        // file does not exist
        myfile.close();
        myfile.clear();
        // do not try to open ios::in as well
        myfile.open(fname.c_str(), ios::binary | ios::out);
        if (myfile) {
            cerr << "atmfile::open -> creating file " << fname.c_str() << endl;
            is_open = 1;
            is_new = 1;
            return 0;	// 0 samples inside the file yet
        } else {
            // does not exist and can not be created
            cerr << "atmfile::open -> hooops! can neither read top 	nor create " << fname << "  atmfile!!!" << endl;
            cerr << "   possibly file locked by other process - MAPROS??" << endl;
            return EXIT_FAILURE;
        }
    }

    // we never reach this line here
    //return ath.iSamples;

}


/***************************************************************************
 *                                                                         *
 *   read bits from atm file                                               *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/


size_t atmfile::read(vector<bool>& vb) {

    size_t i, j;
    char ch;
    if (!is_open) {
        cerr << "atmfile::read -> atm file "<< fname << "  is not open yet. Please use the atm.open(filename) function" << endl;
        return 0;
    }

    if (is_new) {
        cerr << "atmfile::read -> atm file "<< fname << "  is NEW. Please use the write function" << endl;
        return 0;
    }

    // prevent read error by a read vector of wrong dimension
    if (vb.size() != ath.iSamples) {
        cerr << "atmfile::read -> can not read more samples than inside the file "<< fname << " " << endl;
        cerr << "file: " << ath.iSamples << " vector size: " << vb.size() << endl;
        return 0;
    }

    // make sure we start at the right position
    myfile.seekg(sizeof(ath));
    myfile.seekp(sizeof(ath));

    for ( i = 0; i < vb.size() / charbits; i++) {
        myfile.read((char*) &ch, sizeof(char));
        bs = ch;
        for (j = 0; j < charbits; j++)
            vb[charbits * i + j] = bs[j];
    }

    if (vb.size() % charbits) {
        bs.reset();
        myfile.read((char*) &ch, sizeof(char));
        bs = ch;
        for (j = 0; j < vb.size() % charbits; j++)
            vb[charbits * i + j] = bs[j];
    }
    myfile.clear();

    return vb.size();
}


/***************************************************************************
 *                                                                         *
 *      write BITS to disk                                                 *
 *      does NOT return an error if file was opened                        *
 *      in READ mode                                                       *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

size_t atmfile::write(const vector<bool>& vb) {

    size_t i, j;
    char ch;
    if (!is_open || !myfile) {
        cerr << "atmfile::write -> atm file "<< fname << "  is not open yet. Please use the atm.open(filename) function" << endl;
        return 0;
    }

    // go to the beginning of the file
    myfile.seekp(0L, ios::beg);

    // write the new header
    if (is_new) {
        ath.siHeaderLength = sizeof(ath);
        ath.siHeaderVers = AtmHeaderVer;
        // this info we get from the bool vector; no extra init neccessary
        ath.iSamples = vb.size();
        myfile.write((char*) &ath, sizeof(ath));
        myfile << flush;
    }

    // if we write into existing files check if we do not take the wrong amount of samples
    if (vb.size() != ath.iSamples) {
        cerr << "atmfile::write -> atm file "<< fname << "  has different samples from what you are writing now; return" << endl;
        return 0;
    }


    myfile.seekp(sizeof(ath));

    for ( i = 0; i < vb.size() / charbits; i++) {
        for (j = 0; j < charbits; j++)
            bs[j] = vb[charbits * i + j];
        ch = char(bs.to_ulong());
        myfile.write((char*) &ch, sizeof(char));
    }

    if (vb.size() % charbits) {
        bs.reset();
        for (j = 0; j < vb.size() % charbits; j++)
            bs[j] = vb[charbits * i + j];
        ch = char(bs.to_ulong());
        myfile.write((char*) &ch, sizeof(char));
    }
    
    close();
    is_new = 0;			// now file can be read again
    is_open = 0;			// close after write access!
    cerr << "atmfile::write -> closed" << endl;
    return vb.size();
}
