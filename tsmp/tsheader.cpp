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
#include "tsheader.h"


/*!
be sure that these variables are set to 0 or given by default

*/
TSHeader::TSHeader()
{

#ifdef CLASSDBG
    cerr << "TSHeader initialized" << endl;
#endif
    this->tshead = NULL;

    this->adcbits = 0;
    this->samples = 0;
    this->samples_orig = 0;
    this->samplefreq = 0;
    this->samplefreq_orig = 0;
    this->storagebits = 0;
    this->lsbval = 0;
    this->adc_dcoffset = 0;
    this->adc_factor = 0;



    this->datetime = 0;
    this->datetime_orig = 0;
    this->stoptime = 0;
    this->stoptime_orig = 0;
    this->utc_to_gps_offset = 0;
    this->is_gmt = 0;
    this->gmt_offset = 0;
    this->sync_precision = 0;
    this->delta_start = 0;
    this->delta_stop = 0;

    this->x1 = 0;
    this->y1 = 0;
    this->z1 = 0;
    this->x2 = 0;
    this->y2 = 0;
    this->z2 = 0;

    this->x1_orig = 0;
    this->y1_orig = 0;
    this->z1_orig = 0;
    this->x2_orig = 0;
    this->y2_orig = 0;
    this->z2_orig = 0;


    this->elevation = 0;
    this->system_cal_date = 0;
    this->system_gain = 0;
    this->headerlength = 0;
    this->sensor_cal_date = 0;
    this->sensor_gain = 0;


    this->sensor_resistivity = 0;
    this->sensor_dc_offset = 0;
    this->channel_no = 0;
    this->n_channels = 1;

    this->lower_bound = this->upper_bound = this->novalue = 0;
    this->adc_plus =  this->adc_minus = 0;

    is07 = false;



}

void TSHeader::set_filetype_07(bool is07) {

    this->is07 = is07;
}

TSHeader::~TSHeader()
{
}




/*!
    \fn TSHeader::set__adcbits(size_t& adcbits)
     Get the BITS of the ADC - e.g. 8, 16, 24, 32<br>
     This is NOT the bit size of the file format - 24 ADC values stored as 32 integer is common

 */
size_t TSHeader::set_adcbits(size_t& adcbits)
{
    if (adcbits == 8 || adcbits == 16 || adcbits == 24 || adcbits == 32 ) this->adcbits = adcbits;
    else { cerr << "TSHeader::set_adcbits error; adcbits must be of 8, 16, 24, 32; received: " << adcbits << endl;
        return this->adcbits;
    }
    return this->adcbits;
}


/*!
    \fn TSHeader::set_samples(size_t& samples)
    Get the total amount of samples of the file as specified in header.
 */
size_t TSHeader::set_samples(size_t& samples)
{
    if (samples != LONG_MAX) this->samples = samples;
    else cerr << "TSHeader::set__samples error: received " << samples << " samples" << endl;
    return samples;
}


/*!
    \fn TSHeader::set_storagebits(size_t& storagebits)
    Storage bits is the bit size which is used inside the file
    example: ADU uses 24 bits but stores these as 32 bit int
 */
size_t TSHeader::set_storagebits(size_t& storagebits)
{
    if (storagebits == 8 || storagebits == 16 || storagebits == 24 || storagebits == 32 ) this->storagebits = storagebits;
    else { cerr << "TSHeader::set_storagebits error; storagebits must be of 8, 16, 24, 32; received: " << storagebits << endl;
        return EXIT_FAILURE;
    }
    return this->storagebits;
}


/*!
    \fn TSHeader::set_is_gmt(bool is_gmt)
    true if time series was recorded in GMT instead of UTC (satellite time)
 */
int TSHeader::set_is_gmt(int is_gmt)
{
    if (is_gmt != INT_MAX) this->is_gmt = is_gmt;
    return this->is_gmt;
}


/*!
    \fn TSHeader::set_datetime(long& datetime)
    set_s the UNIX time stamp; seconds since 1970
 */
long TSHeader::set_datetime(long& datetime)
{
    if (datetime != LONG_MAX) this->datetime = datetime;
    return this->datetime;
}


/*!
    \fn TSHeader::set_utc_to_gps_offset(long& utc_to_gps_offset)
    UTC offset is the difference between satellite time (UTC) and GMT time; <br>
    actually in the year 2003 the difference is 13 seconds
 */
long TSHeader::set_utc_to_gps_offset(long& utc_to_gps_offset)
{
    if(utc_to_gps_offset != LONG_MAX)
        this->utc_to_gps_offset = utc_to_gps_offset;
    return this->utc_to_gps_offset;
}


/*!
    make sure to calculate the minimum set of variables
    needed - like stoptime!
    ifstream must be opened before
 */
size_t TSHeader::read_headers(ifstream& ifs)
{
    size_t pos = 0;
    if (tshead)
    {
        int gr_la;                    /*!< Latitude/Longitude */
        int mi_la;                    /*!< Latitude/Longitude */
        double se_la;                 /*!< Latitude/Longitude */
        int gr_lo;                    /*!< Latitude/Longitude */
        int mi_lo;                    /*!< Latitude/Longitude */
        double se_lo;                 /*!< Latitude/Longitude */
        string LatH, LonH;


        pos = tshead->read_header(ifs);
        this->headerlength = tshead->get_headersize();
        this->samples = tshead->get_samples();
        this->samples_orig = this->samples;

        this->lsbval = tshead->get_lsb();
        this->system_gain = tshead->get_gain();

        this->channel_no = this->tshead->get_channel_no();

        this->samplefreq = tshead->get_samplefreq();
        this->samplefreq_orig = tshead->get_samplefreq_orig();

        //cerr << "read header " << this->samplefreq_orig << endl;
        if (!this->samplefreq_orig || this->samplefreq_orig == DBL_MAX) this->samplefreq_orig = this->samplefreq;

        //cerr << "read header " << this->samplefreq_orig << endl;

        tshead->identify(this->headertype, this->headerversion, this->headerlength, this->n_channels, this->storagebits, this->tslices);
#ifdef CLASSDBG
        cerr << "TSHeader::readheader read header for " << samples << " " << headertype << " " << headerversion << "  " << storagebits << endl;
#endif
        this->datetime = tshead->get_datetime();
        this->datetime_orig = this->datetime;
        this->gmt_offset = tshead->get_gmt_offset();
        this->is_gmt = tshead->get_is_gmt();
        this->utc_to_gps_offset = tshead->get_utc_to_gps_offset();
        this->delta_start = tshead->get_delta_start();
        this->delta_stop = tshead->get_delta_stop();
        // theoretical max

        this->stoptime = this->datetime + long(this->samples/this->samplefreq);
        this->stoptime_orig = this->stoptime;


        this->channel_type = tshead->get_channel_type();

        tshead->get_bits(this->adcbits, this->storagebits);

        this->channel_id = tshead->get_channel_id();




        this->sensor_calfile = tshead->get_sensor_calfile();


        this->sensor_id = tshead->get_sensor_id();

        this->sensor_type = tshead->get_sensor_type();



        this->sensor_resistivity = tshead->get_sensor_resistivity();
        this->sensor_dc_offset = tshead->get_dc_offset();
        this->sensor_chopper = tshead->get_sensor_chopper();




        this->system_name = tshead->get_system_name();

        // for ADU06 data this is simply the serial number;
        this->system_id = tshead->get_system_id();

        this->system_calfile = tshead->get_system_calfile();

        this->system_chopper = tshead->get_system_chopper();

        tshead->get_coordinates(gr_la, mi_la, se_la, LatH, gr_lo, mi_lo, se_lo, LonH);



        coordinates::getlatlon(gr_la, mi_la, se_la, LatH, gr_lo, mi_lo, se_lo, LonH);
        this->elevation = tshead->get_elevation();

        tshead->get_sensor_pos(this->x1, this->y1, this->z1, this->x2, this->y2, this->z2);

        this->x1_orig = this->x1;
        this->y1_orig = this->y1;
        this->z1_orig = this->z1;
        this->x2_orig = this->x2;
        this->y2_orig = this->y2;
        this->z2_orig = this->z2;


        this->adc_dcoffset = tshead->get_adc_offset();
        this->adc_factor = tshead->get_adc_factor();

        this->result_selftest = tshead->get_result_selftest();
        this->surveyID   = tshead->get_survey_id();
        this->meas_type  = tshead->get_meastype();
        this->Contractor = tshead->get_contractor();
        this->Client     = tshead->get_client();
        this->area       = tshead->get_area();
        this->Operator   = tshead->get_system_operator();
        this->achxmlheader    = tshead->get_xmlheader();
        this->comments   = tshead->get_comments();










    }
    else {
        cerr << "TSHeader::readheader no header class available! " << endl;
    }

    return pos;
}

/*!
    \fn TSHeader::writeheader(ifstream& ifs)
    make sure to calculate the minimum set of variables
    needed - like stoptime!
 */
size_t TSHeader::write_headers(ofstream& ofs,  const unsigned short int header_ver, const unsigned short int header_length)
{


    if (!ofs) {
        cerr << "TSHeader::write_headers -> ofstream not open" << endl;
        return 0;
    }


    size_t pos = 0;
    if (tshead)
    {
        int gr_la;                    /*!< Latitude/Longitude */
        int mi_la;                    /*!< Latitude/Longitude */
        double se_la;                 /*!< Latitude/Longitude */
        int gr_lo;                    /*!< Latitude/Longitude */
        int mi_lo;                    /*!< Latitude/Longitude */
        double se_lo;                 /*!< Latitude/Longitude */
        string LatH, LonH;
        tshead->clean_header();

        if (header_length) {
            tshead->set_headersize(header_length);
            this->headerlength = header_length;
        }
        else tshead->set_headersize(this->headerlength);

        if (header_ver) {

            tshead->set_headerversion(header_ver);
            this->headerversion = header_ver;
        }
        // else tshead->set_headersize(this->headerversion);

        tshead->set_samplefreq(this->samplefreq);
        tshead->set_samplefreq_orig(this->samplefreq_orig);
        tshead->set_samples(this->samples);
        tshead->set_datetime(this->datetime);
        tshead->set_gmt_offset(this->gmt_offset);
        tshead->set_is_gmt(this->is_gmt);
        tshead->set_utc_to_gps_offset(this->utc_to_gps_offset);

        tshead->set_lsb(this->lsbval);
        tshead->set_delta_start(this->delta_start);
        tshead->set_delta_stop(this->delta_stop);
        tshead->set_channel_type(this->channel_type);
        tshead->set_channel_id(this->channel_id);
        tshead->set_sensor_calfile(this->sensor_calfile);
        tshead->set_sensor_type(this->sensor_type);
        tshead->set_sensor_id(this->sensor_id);
        tshead->set_system_id(this->system_id);
        tshead->set_system_name(this->system_name);
        tshead->set_system_calfile(this->system_calfile);

        tshead->set_system_chopper(this->system_chopper);
        tshead->set_sensor_chopper(this->sensor_chopper);

        tshead->set_channel_no(this->channel_no);

        coordinates::get_lat_lon(gr_la, mi_la, se_la, LatH, gr_lo, mi_lo, se_lo, LonH);
        tshead->set_coordinates(gr_la, mi_la, se_la, LatH, gr_lo, mi_lo, se_lo, LonH);
        tshead->set_elevation(this->elevation);
        tshead->set_sensor_pos(this->x1, this->y1, this->z1, this->x2, this->y2, this->z2);

        tshead->set_sensor_resistivity(this->sensor_resistivity);
        tshead->set_dc_offset(this->sensor_dc_offset);
        tshead->set_gain(this->system_gain);


        tshead->set_survey_id(this->surveyID);
        tshead->set_meastype(this->meas_type );
        tshead->set_contractor(this->Contractor);
        tshead->set_client(this->Client);
        tshead->set_area(this->area);
        tshead->set_system_operator(this->Operator);
        tshead->set_xmlheader(this->achxmlheader);
        tshead->set_comments(this->comments);
        tshead->set_tslices(this->tslices);




        pos = tshead->write_header(ofs);

        if (pos != headerlength) cerr << "TSHeader::write_header possible error writing the header!!!! pos: " << pos << " headerlength: " << headerlength << endl;
    }


    return pos;
}

/*!
    \fn TSHeader::set_lsb(double& lsbval)
 */
void TSHeader::set_lsb(double& lsbval)
{
    if ( lsbval != DBL_MAX ) this->lsbval = lsbval;

}




/*!
    \fn TSHeader::dipole_length()
    return E-Field length
 */
double TSHeader::dipole_length() const
{

    double tx, ty, tz;
    tx = this->x2 - this->x1;
    ty = this->y2 - this->y1;
    tz = this->z2 - this->z1;
    return sqrt(tx * tx  + ty * ty + tz * tz);
}


/*!
    \fn TSHeader::e_angle()
    return E field e_angle() in degrees from North to East
 */
double TSHeader::e_angle() const
{
    double tx, ty, tz;
    tx = this->x2 - this->x1;
    ty = this->y2 - this->y1;
    tz = this->z2 - this->z1;
    return  180. / M_PI * atan2(ty,tx);

}


/*!
    \fn TSHeader::diptopos(double length, double angle)
    after scaling the electric field we change the
    E-Field postions ...because we might use them later
 */
void TSHeader::diptopos(double length, double angle)
{
    cerr << "TSHeader::change_spos -> changing sensor positions according from dipole length and e_angle() from " << endl;
    cerr   << "\x9" << this->x1 << "  " << this->y1 << "  " << this->z1 << "  " << this->x2 << "  " << this->y2 << "  " << this->z2
           << "\x9to\x9";
    if (!length) {
        this->x1 = -cos(M_PI / 180. * e_angle()) * dipole_length() / 2.;
        this->x2 =  cos(M_PI / 180. * e_angle()) * dipole_length() / 2.;
        this->y1 = -sin(M_PI / 180. * e_angle()) * dipole_length() / 2.;
        this->y2 =  sin(M_PI / 180. * e_angle()) * dipole_length() / 2.;

    }

    else {
        this->x1 = -cos(M_PI / 180. * angle) * length / 2.;
        this->x2 =  cos(M_PI / 180. * angle) * length / 2.;
        this->y1 = -sin(M_PI / 180. * angle) * length / 2.;
        this->y2 =  sin(M_PI / 180. * angle) * length / 2.;


    }
    // clean values from numeriocal artefacts
    if  (abs(this->x1) <  assume_zero_length) this->x1 = 0.;
    if  (abs(this->x2) <  assume_zero_length) this->x2 = 0.;
    if  (abs(this->y1) <  assume_zero_length) this->y1 = 0.;
    if  (abs(this->y2) <  assume_zero_length) this->y2 = 0.;
    if  (abs(this->z1) <  assume_zero_length) this->z1 = 0.;
    if  (abs(this->z2) <  assume_zero_length) this->z2 = 0.;

    cerr   << this->x1 << "  " << this->y1 << "  " << this->z1 << "  "
           << this->x2 << "  " << this->y2 << "  " << this->z2 << "  " << endl;
    cerr << "tsheader::change_spos -> used e_angle() was " << e_angle() << "[deg]" << endl;
}



int TSHeader::change_run(const string& run) {


    if (is07) {
        QString qtmp = run.c_str();
        std::cerr << "new run *******" << run << endl;
        name07.set_run_07(qtmp);
        my_name_is = name07.get_adu07_filename_stdstr();
        my_run_number_is = run;
        cerr << "07**********"<< my_name_is << endl;
    }
    else {
        cerr << "06**********"<< my_name_is << endl;
        if (run.size() > 2) {
            cerr << "tsheader::change_run -> run must be 01 ..99; given: " << run << endl;
            return 0;
        }


        my_name_is.replace(4, 2, run);
    }
    my_run_number_is = run;
    return 1;
}


int TSHeader::my_datafile(const char *name) {
    string t_string = name;
    string::size_type i  = t_string.rfind("/");
    string::size_type j = t_string.rfind("/", 2);

    // working in actual directory or a "./" is not given
    if (j == t_string.npos) j = 0;


    if (i != t_string.npos) my_datafile_is.assign(t_string, ++i, t_string.npos);
    else my_datafile_is = name;

    return 1;
}



/*
 quite essential function: which file was read, which directory and where to go!
 also makes site_number, run_number etc.


*/
int TSHeader::my_name(const char *name, const string& outget_dir) {
    string t_string = name;
    string::size_type i  = t_string.rfind("/");
    string::size_type j = t_string.rfind("/", 2);
    string::size_type k = t_string.rfind("../");

    // working in actual directory or a "./" is not given
    if (j == t_string.npos) j = 0;


    if (i != t_string.npos) my_name_is.assign(t_string, ++i, t_string.npos);
    else my_name_is = name;

    name07.setName(my_name_is.c_str());

    // if ( i != j && i != t_string.npos && j != t_string.npos) {
    if ( i != j && i != t_string.npos) {
        //if (j && k) my_dir_is.assign(t_string, ++j, i-j-1);       // k should be npos
        if (j && k) {
            ++j;
            my_dir_is.assign(t_string, j, i-j-1);
        }     // k should be npos
        else if (!k) my_dir_is.assign(t_string, k, i);            // ../ in first place
        else  my_dir_is.assign(t_string, j, i);
        i = my_dir_is.rfind("SITE");
        if (i == my_dir_is.npos) i = my_dir_is.rfind("site");
        if (i != my_dir_is.npos) {
            j =  my_dir_is.rfind("/");
            if (j) my_sitenumber_is.assign(my_dir_is, i + 4, j - (i + 4) );
        }
    }

    my_out_dir_is.assign(outget_dir);


    if (!my_out_dir_is.size()) my_out_dir_is = my_dir_is;
    if (my_out_dir_is.size() && my_out_dir_is[my_out_dir_is.size()-1] != '/') my_out_dir_is.append("/");



    if (my_name_is.size() > 6) my_run_number_is.assign(my_name_is, 4, 2);

    if (!isdigit(my_run_number_is[0]) || !isdigit(my_run_number_is[1]) ) my_run_number_is = "00";


    cerr << endl << "dir: " << my_dir_is << ", name: " << my_name_is << ", out_dir: "
         << my_out_dir_is << ", site: " << my_sitenumber_is << ", run: " << my_run_number_is << endl;

    return 1;
}

string TSHeader::return_outputfilename(const char* extension) {

    string fname;
    // assign atm file name to channel a

    // working in the directoy where the files are
    if (!my_out_dir_is.size())  {
        //        cerr << my_dir_is << " " << my_dir_is[my_dir_is.size()-1] << endl;
        if (my_dir_is.size() && my_dir_is[my_dir_is.size()-1] == '/')
            newext( (my_dir_is + my_name_is), fname, extension);
        //        else  newext( (my_dir_is + "/" + my_name_is), fname, extension);
        else  newext( (my_name_is), fname, extension);
    }
    // we have a different output directory
    else newext( (my_out_dir_is + my_name_is), fname, extension);

    if (fname.size() < 8) {
        cerr << "tsheader::return_outputfilename -> error? filename is: " << fname << endl;
    }
    //  cerr << "fname: " << fname << " " << fname.c_str() << eg constantndl;
    return fname;
}

string TSHeader::return_bandindex_mtx(double frequency) {

    string idx;
    if (!frequency) frequency = samplefreq;

    if (frequency > 4096.) idx = "A";
    else if (frequency == 4096.) idx = "B";
    else if (frequency > 100. && frequency < 4096.) idx = "F";
    else if (frequency > 30. && frequency < 100.) idx = "C";
    else if (frequency > 1. && frequency < 4.) idx = "D";
    else if (frequency > 0.004 && frequency < 0.08) idx = "E";
    else if (frequency > 0.001 && frequency < 0.004) idx = "G";
    else cerr << "TSHeader::return_bandindex_mtx -> could not generate band index for " << frequency << endl;


    return idx;

}

string TSHeader::return_atmfilename(const string& run) {

    string fname;
    // assign atm file name to channel a

    // working in the directory where the files are
    if (!my_out_dir_is.size()) newext( (my_dir_is + my_name_is), fname, "atm");
    // we have a different output directory
    else newext( (my_out_dir_is + my_name_is), fname, "atm");

    if (is07) {

        QString qstr, qrstr;
        qstr = QString::fromStdString(fname);

        if (run.size()) {
            this->change_run(run);

        }

        qstr.replace(QString("_THx_"), QString("_TEx_"));
        qstr.replace(QString("_THy_"), QString("_TEx_"));
        qstr.replace(QString("_THz_"), QString("_TEx_"));
        qstr.replace(QString("_TEy_"), QString("_TEx_"));

        qstr.replace(QString("_C00_"), QString("_C01_"));
        qstr.replace(QString("_C02_"), QString("_C01_"));
        qstr.replace(QString("_C03_"), QString("_C01_"));
        qstr.replace(QString("_C04_"), QString("_C01_"));
        qstr.replace(QString("_C05_"), QString("_C01_"));


        fname = qstr.toStdString();


        return fname;

    }

    if (isupper(fname[fname.size() - 6])) fname[fname.size() - 6] = 'A';
    else  fname[fname.size() - 6] = 'a';
    if (isupper(fname[fname.size() - 9])) fname[fname.size() - 9] = 'A';
    else fname[fname.size() - 9] = 'a';

    if (fname.size() < 8) {
        cerr << "TSHeader::return_atmfilename -> ERROR?? filename is: " << fname << endl;
    }

    return fname;
}

string TSHeader::return_atsfilename(string& run) {

    string name, snum;
    int serial;
    unsigned short int SerNoADU;

    SerNoADU = str2num<int>(system_id);

    if (SerNoADU == USHRT_MAX) {
        name = "999";
        SerNoADU = atoi(name.c_str());
    }


    else name = num2str(SerNoADU);

    if (name.size() == 1) name = "00" + name;
    else if (name.size() == 2) name = "0" + name;


    snum = channel_type;
    lower(snum);

    if (snum == "ex") name += "A";
    if (snum == "ey") name += "B";
    if (snum == "hx") name += "C";
    if (snum == "hy") name += "D";
    if (snum == "hz") name += "E";

    if (run.size() && (atoi(run.c_str()) < 99) ) {
        name += run;
        my_run_number_is = run;
    }
    else {
        cerr << "TSHeader::return_atsfilename -> can not set correct run number ";
        cerr << "given number " << run << " set to 99" << endl;
        name += "99";
        my_run_number_is = "99";
    }

    if (snum == "ex") name += "A";
    if (snum == "ey") name += "B";
    if (snum == "hx") name += "X";
    if (snum == "hy") name += "Y";
    if (snum == "hz") name += "Z";

    //name += "D.ats";
    name += return_bandindex_mtx(samplefreq);
    name += ".ats";

    //  cerr << " TSHeader::return_atsfilename -> " << name << endl;

    return name;
}

int TSHeader::shift_bandindex(const int set_xmlheader) {

    if (is07) {


        QString tmps;
        name07.set_sample_freq(this->samplefreq);
        tmps = name07.get_adu07_filename_str();
        my_name_is = tmps.toStdString();


        return 1;
    }



    if ((my_name_is.size() == 12) && my_name_is[8] != '.') {
        cerr << "tsheader::shift_bandindex -> " << my_name_is << " seems not to be an ats file" << endl;
        return 0;
    }

    if (my_name_is[7] == 'D') my_name_is[7] = 'E';
    else if (my_name_is[7] == 'd') my_name_is[7] = 'e';
    else if (my_name_is[7] == 'C') my_name_is[7] = 'D';
    else if (my_name_is[7] == 'c') my_name_is[7] = 'd';
    else if (my_name_is[7] == 'B') my_name_is[7] = 'C';
    else if (my_name_is[7] == 'b') my_name_is[7] = 'c';
    else if (my_name_is[7] == 'A') my_name_is[7] = 'B';
    else if (my_name_is[7] == 'a') my_name_is[7] = 'b';
    else if (my_name_is[7] == 'E') my_name_is[7] = 'G';
    else if (my_name_is[7] == 'e') my_name_is[7] = 'g';
    else if (my_name_is[7] == 'f') my_name_is[7] = 'c';
    else if (my_name_is[7] == 'F') my_name_is[7] = 'C';
    else {


        // try CEA

        QString ceaname;
        ceaname = QString::fromStdString(my_name_is);
        QStringList splitcea = ceaname.split("_");
        if (splitcea.size() == 5 && splitcea.at(1).startsWith(QChar('2')) && splitcea.at(4).endsWith(".ats", Qt::CaseInsensitive)) {
            std::cerr << "\nassuming CEA, try... \n";

            QString changefreq;
            if (this->samplefreq >= 1.0) {
                changefreq = QString("%1").arg(((int)(this->samplefreq)) );
                changefreq += "H";

            }
            else {

                changefreq = QString("%1").arg(((int)(1./this->samplefreq)));
                changefreq += "S";
            }

            splitcea[3] = changefreq;

            ceaname = splitcea.join("_");

            my_name_is = ceaname.toStdString();

            return 1;

        }







        string::size_type l;
        l = my_name_is.rfind(".");
        if (l != my_name_is.npos) my_name_is.erase(l);
        my_name_is += "_fil.ats";
    }




    return 1;
}


void TSHeader::write_logfile() {

    string filename;
    newext(this->my_dir_is + this->my_name_is, filename, "log");
    TSHeader::write_logfile(filename);
}


void TSHeader::write_logfile(string filename = "") {

    ofstream ofs;

    int gr_la;                    /*!< Latitude/Longitude */
    int mi_la;                    /*!< Latitude/Longitude */
    double se_la;                 /*!< Latitude/Longitude */
    int gr_lo;                    /*!< Latitude/Longitude */
    int mi_lo;                    /*!< Latitude/Longitude */
    double se_lo;                 /*!< Latitude/Longitude */
    string NS, EW;

    coordinates::get_lat_lon(gr_la, mi_la, se_la, NS, gr_lo, mi_lo, se_lo, EW);
    //coordinates::get_NS_EW(NS, EW);
    if (!filename.size()) newext(this->my_name_is, filename, "log");
    ofs.open(filename.c_str());

    if (!ofs) cerr << "TSHeader::wite_logfile -> can not write " << filename << endl;


    else {

        cerr << "TSHeader::wite_logfile "<< filename << endl;

        ofs << "adcbits:                  " <<          adcbits << endl;
        ofs << "samples:                  " <<          samples << endl;
        ofs << "samples_orig:             " <<     samples_orig << endl;
        ofs << "samplefreq:               " <<       samplefreq << " [Hz] " << endl;
        ofs << "samplefreq_orig:          " <<  samplefreq_orig << " [Hz] " << endl;
        ofs << "storagebits:              " <<      storagebits << endl;
        ofs << "byteorder:                " <<        byteorder << endl;
        ofs << "lsbval:                   " <<           lsbval << " [mV] " << endl;
        ofs << "adc_dcoffset:             " <<     adc_dcoffset << endl;
        ofs << "adc_factor:               " <<       adc_factor << endl;



        ofs << "datetime:                 " <<         datetime << " [s] " << GMTtime(datetime) << endl;
        ofs << "datetime_orig:            " <<    datetime_orig << " [s] " << GMTtime(datetime_orig) << endl;
        ofs << "stoptime:                 " <<         stoptime << " [s] " << GMTtime(stoptime) << endl;
        ofs << "stoptime_orig:            " <<    stoptime_orig << " [s] " << GMTtime(stoptime_orig) << endl;
        ofs << ",oostring                ," <<    system_id << "," << datetime << "," << stoptime << endl;
        ofs << "utc_to_gps_offset:        " <<    utc_to_gps_offset << endl;
        ofs << "is_gmt:                   " <<           is_gmt << endl;
        ofs << "gmt_offset:               " <<       gmt_offset << endl;
        ofs << "sync_status:              " <<      sync_status << endl;
        ofs << "sync_precision:           " <<   sync_precision << endl;
        ofs << "delta_start:              " <<      delta_start << endl;
        ofs << "delta_stop:               " <<       delta_stop << endl;

        ofs << "latitude:                 " << gr_la << ":" << mi_la << ":" << se_la << " " << NS << endl;
        ofs << "longitude:                " << gr_lo << ":" << mi_lo << ":" << se_lo << " " << EW << endl;
        ofs << "latitude:                 " << gr_la << "° " << mi_la << "' " << se_la << "'' " << NS << endl;
        ofs << "longitude:                " << gr_lo << "° " << mi_lo << "' " << se_lo << "'' " << EW << endl;

        ofs << "x1:                       " <<               x1 << endl;
        ofs << "y1:                       " <<               y1 << endl;
        ofs << "z1:                       " <<               z1 << endl;
        ofs << "x2:                       " <<               x2 << endl;
        ofs << "y2:                       " <<               y2 << endl;
        ofs << "z2:                       " <<               z2 << endl;
        ofs << "elevation:                " <<        elevation << endl;

        ofs << "x1_orig:                  " <<          x1_orig << endl;
        ofs << "y1_orig:                  " <<          y1_orig << endl;
        ofs << "z1_orig:                  " <<          z1_orig << endl;
        ofs << "x2_orig:                  " <<          x2_orig << endl;
        ofs << "y2_orig:                  " <<          y2_orig << endl;
        ofs << "z2_orig:                  " <<          z2_orig << endl;


        ofs << "manufacturer_logger:      " << manufacturer_logger << endl;
        ofs << "system_name:              " <<         system_name << endl;
        ofs << "system_id:                " <<           system_id << endl;
        ofs << "system_cal_date:          " <<     system_cal_date << endl;
        ofs << "system_gain:              " <<         system_gain << endl;
        ofs << "system_calfile:           " <<      system_calfile << endl;
        ofs << "system_chopper:           " <<      system_chopper << endl;

        ofs << "headertype:               " <<          headertype << endl;
        ofs << "headerversion:            " <<       headerversion << endl;
        ofs << "headerlength:             " <<        headerlength << endl;


        ofs << "manufacturer_senor :      " << manufacturer_senor  << endl;
        ofs << "sensor_type:              " <<         sensor_type << endl;
        ofs << "sensor_id:                " <<           sensor_id << endl;
        ofs << "sensor_cal_date:          " <<     sensor_cal_date << endl;
        ofs << "sensor_gain:              " <<         sensor_gain << endl;
        ofs << "sensor_chopper:           " <<      sensor_chopper << endl;

        ofs << "sensor_status:            " <<       sensor_status << endl;
        ofs << "sensor_calfile:           " <<      sensor_calfile << endl;
        ofs << "sensor_resistivity:       " <<  sensor_resistivity << endl;
        ofs << "sensor_dc_offset:         " <<    sensor_dc_offset << endl;


        ofs << "channel_type:             " <<        channel_type << endl;
        ofs << "channel_no:               " <<          channel_no << endl;
        ofs << "n_channels:               " <<          n_channels << endl;
        ofs << "channel_id:               " <<          channel_id << endl;
        ofs << "channel_status:           " <<      channel_status << endl;




        ofs << "units:                    " <<               units << endl;

        ofs << "meas_type:                " <<           meas_type << endl;
        ofs << "result_selftest:          " <<           result_selftest << endl;

        ofs << "Client:                   " <<              Client << endl;
        ofs << "Contractor:               " <<          Contractor << endl;
        ofs << "project:                  " <<             project << endl;
        ofs << "area:                     " <<                area << endl;
        ofs << "line:                     " <<                line << endl;
        ofs << "target:                   " <<              target << endl;
        ofs << "landmark:                 " <<            landmark << endl;
        ofs << "surveyID:                 " <<            surveyID << endl;
        ofs << "supervisor:               " <<          supervisor << endl;
        ofs << "Operator:                 " <<            Operator << endl;
        ofs << "achxmlheader:                  " <<             achxmlheader << endl;
        ofs << "comments:                 " <<            comments << endl;

    }
    ofs.close();
    ofs.clear();




}





/*!
    \fn TSHeader::set_sensor_pos(vector<double>& spos)
 */
void TSHeader::set_sensor_pos(valarray<double>& spos)
{

    if (spos.size() < 6 ) {
        cerr << "TSHeader::set_sensor_pos -> you mus provide sox positions x1, y1, z1, x2, y2, z2" << endl;

    }
    else {
        if (spos[0] != DBL_MAX) this->x1 = spos[0];
        if (spos[1] != DBL_MAX) this->y1 = spos[1];
        if (spos[2] != DBL_MAX) this->z1 = spos[2];
        if (spos[3] != DBL_MAX) this->x2 = spos[3];
        if (spos[4] != DBL_MAX) this->y2 = spos[4];
        if (spos[5] != DBL_MAX) this->z2 = spos[5];


    }
}


/*!
    \fn TSHeader::set_lat_lon(const int& lag, const int& lam, const double& las, const int& log, const int& lom, const double& los)
 */
void TSHeader::set_lat_lon(const int& lag, const int& lam, const double& las, const string& LatH, const int& log, const int& lom, const double& los, const string& LonH)
{

    if ( (lag == INT_MAX) &&  (lam == INT_MAX) && (las == DBL_MAX) && (log == INT_MAX) &&  (lom == INT_MAX) && (los == DBL_MAX)) {

    }


    else if ( (lag == INT_MAX) ||  (lam == INT_MAX) || (las == DBL_MAX) || (log == INT_MAX) ||  (lom == INT_MAX) || (los == DBL_MAX)) {
        cerr << "TSHeader::set_lat_lon-> at least on of the latitude / longitude entries is not valid, no problem, skipping " << endl;
        cerr << lag << ":"  << lam  << ":" << las << "     " << log << ":" << lom << ":" << los << endl;

    }
    else {
        coordinates::getlatlon(lag, lam, las, LatH, log, lom, los, LonH);
    }

}


/*!
    \fn TSHeader::set_ref_meridian(const int &meridian)
 */
void TSHeader::set_ref_meridian(const int& utmmer, const int& gkmer, const int& ell)
{
    if (utmmer == INT_MAX) {
        //cerr << "TSHeader::set_ref_meridian -> at least the UTM meridian has to be valid ...0, 6, 12, no problem, skipping" << endl;

    }
    else coordinates::getmeridian_ell(utmmer, gkmer, ell);
}


/*!
    \fn TSHeader::set_elevation(const double& elevation)
 */
void TSHeader::set_elevation(const double& elevation)
{
    if (elevation != DBL_MAX) this->elevation = elevation;
}


/*!
    \fn TSHeader::set_diplen_angle(const double& diplen, const double& angle
 */
void TSHeader::set_diplen_angle(const double& diplen, const double& angle)
{
    if (diplen != DBL_MAX && angle != DBL_MAX) TSHeader::diptopos(diplen, angle);
}


/*!
    \fn TSHeader::set_sensor(const string& sensor_type, const string& sensor_id, const string& sensor_calfile)
 */
void TSHeader::set_sensor(const string& sensor_type, const string& sensor_id, const string& sensor_calfile)
{
    if (sensor_type.size())    this->sensor_type = sensor_type;
    if (sensor_id.size())      this->sensor_id = sensor_id;
    if (sensor_calfile.size()) this->sensor_calfile = sensor_calfile;
    if (sensor_type.size() && sensor_id.size() && !sensor_calfile.size() ) {
        cerr << "TSHeader::set_sensor -> got type and serial but no calfile -> generating calibration file name " << endl;
        this->sensor_calfile = "";
        this->gen_calfile_name();
    }
}








/*!
    \fn TSHeader::set_channel(const string& channel_type, const string& channel_id, const string& channel_calfile)
 */
void TSHeader::set_channel(const string& channel_type, const string& channel_id, const string& channel_calfile)
{
    if (channel_type.size())    this->channel_type = channel_type;
    if (channel_id.size())      this->channel_id = channel_id;
    if (channel_calfile.size()) this->channel_calfie = channel_calfile;
}


/*!
    \fn TSHeader::set_datetime(const long int& datetime, const long int& gmt_offset)
 */
void TSHeader::set_datetime(const long int& datetime, const long int& gmt_offset)
{
    if (datetime != LONG_MAX)   this->datetime = datetime;
    if (gmt_offset != LONG_MAX) this->gmt_offset = gmt_offset;
}

void TSHeader::set_datetime_plus(const long int& time_sec)
{
    if (time_sec != LONG_MAX)   this->datetime += time_sec;
}


/*!
    \fn TSHeader::set_data_info(const size_t& samples, const double& samplefreq)
 */
void TSHeader::set_data_info(const size_t& samples, const double& samplefreq)
{
    if (samples != LONG_MAX)   this->samples = samples;
    if (samplefreq != DBL_MAX) this->samplefreq = samplefreq;
}


/*!
    \fn TSHeader::set_system(const string& system_name, const string& system_id)
 */
void TSHeader::set_system(const string& system_name, const string& system_id)
{
    if (system_name.size()) this->system_name = system_name;
    if (system_id.size())   this->system_id = system_id;
}

/*!
    \fn TSHeader::set_systemcalfile(const string& system_calfile)
 */

void TSHeader::set_systemcalfile(const string& system_calfile) {
    if (system_calfile.size()) this->system_calfile = system_calfile;
}


string TSHeader::gen_calfile_name() {

    stringstream convert_num;
    string sensor_calfile_used;
    // sensor.cal is a empty template
    if ( (return_lower(sensor_calfile) == "sensor.cal") || (sensor_calfile == "") ) {
        //        && (sensor_type != "efp05") && (sensor_type != "efp06")    )         // no file is given; template
        sensor_calfile_used = sensor_type;
        if (str2num<int>(sensor_id) < 10)   sensor_calfile_used = trim(sensor_calfile_used) + "00";
        else if (str2num<int>(sensor_id) < 100)   sensor_calfile_used = trim(sensor_calfile_used) + "0";
        else sensor_calfile_used = trim(sensor_calfile_used);
        convert_num << sensor_id << ".txt" << ends;
        sensor_calfile_used += convert_num.str();
        cerr << "generalized_header::gen_calfile_name-> " << sensor_calfile_used << endl;
    }

    // 2006: enable calibration of e-field
    /*
        // check that out
  else if ( (sensor_calfile != "SENSOR.CAL")                         // has a cal file name
        && ( sensor_type != "efp05") &&   ( sensor_type != "efp06")    ) {
        sensor_calfile_used = sensor_calfile;
        cerr << "generalized_header::gen_calfile_name-> " << sensor_calfile_used << endl;
  }
  
  else if ( (sensor_type  == "efp05") ||  (sensor_type  == "efp06") || (sensor_type  == "unkn_e") )  {

        cerr << "tsheader::gen_calfile_name-> no transfer function used for " << sensor_type << ": linear" << endl;
  }
  
*/


    return sensor_calfile_used;

}


