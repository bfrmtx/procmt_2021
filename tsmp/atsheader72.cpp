
#include "atsheader72.h"



void ATSHeader72::mv_header_read(){
HeaderLength = header.HeaderLength;
HeaderVer = header.HeaderVer;
Samples = header.Samples;
SampleFreq = header.SampleFreq;
atsDateTime = header.atsDateTime;
LSBVal = header.LSBVal;
GMTOffset = header.GMTOffset;
// strncpy(skip1, header.skip1);
SampleFreqOrig = header.SampleFreqOrig;
SerNoADU = header.SerNoADU;
SerNoADB = header.SerNoADB;
ChanNo = header.ChanNo;
cSensorChopper = header.cSensorChopper;
strncpy(cChannelType, header.cChannelType, sizeof(this->cChannelType));
strncpy(cSensorType, header.cSensorType, sizeof(cSensorType));
SensorNo = header.SensorNo;
x1 = header.x1;
y1 = header.y1;
z1 = header.z1;
x2 = header.x2;
y2 = header.y2;
z2 = header.z2;
DipoleLength = header.DipoleLength;
Angle = header.Angle;
ProbeResistivity = header.ProbeResistivity;
DCOffset = header.DCOffset;
Gain = header.Gain;
Gain2 = header.Gain2;
//strncpy(skip2, header.skip2, sizeof(skip2));
atsLatitude = header.atsLatitude;
atsLongitude = header.atsLongitude;
atsElevation = header.atsElevation;
LatLon_Type = header.LatLon_Type;
Coord_Type = header.Coord_Type;
Ref_Meridian = header.Ref_Meridian;
x = header.x;
y = header.y;
Sync_Status = header.Sync_Status;
Accuracy = header.Accuracy;
UTCGPSOffset = header.UTCGPSOffset;

strncpy(cSystemType, header.cSystemType, sizeof(cSystemType));
strncpy(csurvey_header_fname, header.csurvey_header_fname, sizeof(csurvey_header_fname));
strncpy(meas_type, header.meas_type, sizeof(meas_type));
strncpy(clog_file_sys_self_test, header.clog_file_sys_self_test, sizeof(clog_file_sys_self_test));
strncpy(result_self_test, header.result_self_test, sizeof(result_self_test));

strncpy(skip4, header.skip4, sizeof(skip4));
num_cal_freq = header.num_cal_freq;
lofe = header.lofe;
vocf = header.vocf;
start_cal_info_head = header.start_cal_info_head;
strncpy(skip5, header.skip5, sizeof(skip5));
strncpy(cadu_cal_file_name, header.cadu_cal_file_name, sizeof(cadu_cal_file_name));
date_of_cal_adu = header.date_of_cal_adu;
strncpy(csensor_cal_file_name, header.csensor_cal_file_name, sizeof(csensor_cal_file_name));
date_of_cal_sensor = header.date_of_cal_sensor;
//strncpy(skip6, header.skip6, sizeof(skip6));
strncpy(cClient, header.cClient, sizeof(cClient));
strncpy(cContractor, header.cContractor, sizeof(cContractor));
strncpy(cArea, header.cArea, sizeof(cArea));
strncpy(cSurveyID, header.cSurveyID, sizeof(cSurveyID));
strncpy(cOperator, header.cOperator, sizeof(cOperator));
strncpy(ccoordinates_type, header.ccoordinates_type, sizeof(ccoordinates_type));
atsZoneNumber = header.atsZoneNumber;
atsLetterDesignator = header.atsLetterDesignator;
strncpy(atsemptystr, header.atsemptystr, sizeof(atsemptystr));
atsNorthing = header.atsNorthing;
atsEasting = header.atsEasting;
atsutm_meridian = header.atsutm_meridian;
strncpy(cReferenceEllipsoid, header.cReferenceEllipsoid, sizeof(cReferenceEllipsoid));
strncpy(cskip7, header.cskip7, sizeof(cskip7));
strncpy(Achxmlheader, header.Achxmlheader, sizeof(Achxmlheader));
strncpy(cComments, header.cComments, sizeof(cComments));
}


void ATSHeader72::mv_header_write() {



header.HeaderLength = HeaderLength;
header.HeaderVer = HeaderVer;
header.Samples = Samples;
header.SampleFreq = SampleFreq;
header.atsDateTime = atsDateTime;
header.LSBVal = LSBVal;
header.GMTOffset = GMTOffset;
//strncpy(header.skip1, skip1);
header.SampleFreqOrig = SampleFreqOrig;
header.SerNoADU = SerNoADU;
header.SerNoADB = SerNoADB;
header.ChanNo = ChanNo;
header.cSensorChopper = cSensorChopper;
strncpy(header.cChannelType, cChannelType, sizeof(cChannelType));
strncpy(header.cSensorType, cSensorType, sizeof(cSensorType));
header.SensorNo = SensorNo;
header.x1 = x1;
header.y1 = y1;
header.z1 = z1;
header.x2 = x2;
header.y2 = y2;
header.z2 = z2;
header.DipoleLength = DipoleLength;
header.Angle = Angle;
header.ProbeResistivity = ProbeResistivity;
header.DCOffset = DCOffset;
header.Gain = Gain;
header.Gain2 = Gain2;

//strncpy(header.skip2, skip2, sizeof(skip2));
header.atsLatitude = atsLatitude;
header.atsLongitude = atsLongitude;
header.atsElevation = atsElevation;
header.LatLon_Type = LatLon_Type;
header.Coord_Type = Coord_Type;
header.Ref_Meridian = Ref_Meridian;
header.x = x;
header.y = y;
header.Sync_Status = Sync_Status;
header.Accuracy = Accuracy;
header.UTCGPSOffset = UTCGPSOffset;
strncpy(header.cSystemType, cSystemType, sizeof(cSystemType));
strncpy(header.csurvey_header_fname, csurvey_header_fname, sizeof(csurvey_header_fname));
strncpy(header.meas_type, meas_type, sizeof(meas_type));
strncpy(header.clog_file_sys_self_test, clog_file_sys_self_test, sizeof(clog_file_sys_self_test));
strncpy(header.result_self_test, result_self_test, sizeof(result_self_test));

strncpy(header.skip4, skip4, sizeof(skip4));
header.num_cal_freq = num_cal_freq;
header.lofe = lofe;
header.vocf = vocf;
header.start_cal_info_head = start_cal_info_head;
strncpy(header.skip5, skip5, sizeof(skip5));
strncpy(header.cadu_cal_file_name, cadu_cal_file_name, sizeof(cadu_cal_file_name));
header.date_of_cal_adu = date_of_cal_adu;
strncpy(header.csensor_cal_file_name, csensor_cal_file_name, sizeof(csensor_cal_file_name));
header.date_of_cal_sensor = date_of_cal_sensor;
//strncpy(header.skip6, skip6, sizeof(skip6));
strncpy(header.cClient, cClient, sizeof(cClient));
strncpy(header.cContractor, cContractor, sizeof(cContractor));
strncpy(header.cArea, cArea, sizeof(cArea));
strncpy(header.cSurveyID, cSurveyID, sizeof(cSurveyID));
strncpy(header.cOperator, cOperator, sizeof(cOperator));
strncpy(header.ccoordinates_type, ccoordinates_type, sizeof(ccoordinates_type));
header.atsZoneNumber = atsZoneNumber;
header.atsLetterDesignator = atsLetterDesignator;
strncpy(header.atsemptystr, atsemptystr, sizeof(atsemptystr));
header.atsNorthing = atsNorthing;
header.atsEasting = atsEasting;
header.atsutm_meridian = atsutm_meridian;
strncpy(header.cReferenceEllipsoid, cReferenceEllipsoid, sizeof(cReferenceEllipsoid));
strncpy(header.cskip7, cskip7, sizeof(cskip7));
strncpy(header.Achxmlheader, Achxmlheader, sizeof(Achxmlheader));
strncpy(header.cComments, cComments, sizeof(cComments));
}


ATSHeader72::~ATSHeader72()
{
}

ATSHeader72::ATSHeader72()
{
// empty all elements except ats header version
    char myenv[80]="TZ=GMT0GMT";
if (!putenv(myenv)) tzset();
  else cerr << "atsheader::no environment free; times will be interpreted wrong" << endl;
  
   ATSHeader72::clean_header();
   HeaderLength = 1024;

  #ifdef CLASSDBG
  cerr << "init Atsheader72 " << sizeof(ATSHeader72) << endl;
  #endif
}




/*!
    \fn ATSHeader72::read_header(ifstream& ifs)
    We have to make sure to skip the first byte of the Interface class!
    The ifstream must be opened before
 */
size_t ATSHeader72::read_header(ifstream& ifs)
{

  ifs.seekg(0);


  ifs.read((char*) &header, sizeof(header));
  ATSHeader72::mv_header_read();



  #ifdef CLASSDBG
  cerr << "HeaderLength " << HeaderLength << endl;
  cerr << "HeaderVer " << HeaderVer << endl;
  cerr << "Samples " << Samples << endl;
  cerr << "GMT2UTC " << UTCGPSOffset << endl;
  //cerr << "sizes its ats" << sizeof(ITSHeader) << "  " << sizeof(ATSHeader72) << endl;
  #endif
 cerr << this->cSensorChopper <<  "chopper " << header.cSensorChopper << endl;
  //cerr << sizeof(ATSHeader72) << " sizes " <<  sizeof(ITSHeader) << endl;
  #ifdef swap_ats
  //  atsbswap();
  #endif
  // ITSHEader is the virtual base clas which has a size of a byte
 // ifs.seekg(sizeof(ATSHeader72) - sizeof(ITSHeader));

// cerr << "position" <<  ifs.tellg();
cerr << endl << "HEADER IN " << this->SampleFreq << "  " << this->SampleFreqOrig << endl;





return ifs.tellg();
}


/*!
    \fn ATSHeader72::write_header(ofstream& ofs)
    We have to make sure to skip the first byte of the Interface class!
 */
size_t ATSHeader72::write_header(ofstream& ofs){


#ifdef swap_ats
  ATSHeader72::byteswap_header
#endif

  ofs.seekp(0);
// cerr << "start " << ofs.tellp() << endl;

  ATSHeader72::mv_header_write();
  ofs.write((char*) &header, sizeof(header));

cerr << "\nwrite " << this->cSensorChopper <<  "chopper " << header.cSensorChopper << endl;
cerr << "\nwrite " << this->cSensorChopper <<  "chopper " << header.cSensorChopper << endl;
return ofs.tellp();


}



size_t ATSHeader72::get_headersize() const {

 if (HeaderLength > 1022) {  // we might have read a file here before
  return size_t (HeaderLength);
 }
  // what do we do if we init a new one... take this!
 else return size_t (sizeof(ATSHeader72) - sizeof(ITSHeader));

}

void ATSHeader72::set_headersize(const size_t& hsize){
HeaderLength = (unsigned short int) (hsize);

}





/*!
    \fn ATSHeader72::set_samples(size_t& samples)
    set_ the total samples to be written
 */
size_t ATSHeader72::set_samples(const size_t& samples)
{

    if (samples != LONG_MAX) this->Samples = (unsigned int) samples;
    return 0;

}



/*!
    \fn ATSHeader72::set_lsb(double& lsbval)
    set_ lsb for writing
 */
double ATSHeader72::set_lsb(const double& lsbval)
{
     if (lsbval != DBL_MAX) this->LSBVal = lsbval;
     return LSBVal;
}



/*!
get the minimum information about the header always needed
*/
void ATSHeader72::identify(string& headertype, string& headerversion, size_t& headersize, size_t& n_channels, size_t& storagebits, vector<ATSSliceHeader> &tslices) const {

  headertype = "ats";
  headerversion = "0.72";
  n_channels = 1;
  storagebits = 32;
  headersize = 1024;

}

/*!

channel_type Ex..Hz
*/
string ATSHeader72::get_channel_type() const {



  string channel_type = this->cChannelType;
  channel_type = channel_type.substr(0,2);
  clean_b_str(channel_type);
  return channel_type;

}

/*!
    \fn ATSHeader72::set_datetime(const long int& datetime)
    get UNIX time stamp
 */
void ATSHeader72::set_datetime(const long int& datetime)
{
   this->atsDateTime = datetime;
}



/*
  get correction for start time in seconds
  .. which is not impelemented in atsheader
*/

void ATSHeader72::set_delta_start(const double& delta_start) {

}

/*
  get correction for stop time in seconds
  .. which is not impelemented in atsheader
*/

void ATSHeader72::set_delta_stop(const double& delta_stop){


}



/*!
    \fn ATSHeader72::set_samplefreq(const double& samplefreq)
    get sample freq for writing
 */
void ATSHeader72::set_samplefreq(const double& samplefreq)
{
   if (samplefreq != DBL_MAX) this->SampleFreq = float(samplefreq);
}

/*!
e.g. after filtering the sample frequency is different.
This can have consquences in using transfer functions; e.g. the transfer functions
of the original sampling frequency must be used;
only if the original frequency was 0 or DBL_MAX this value may be overwritten - otherwise NOT;
check this somewhere, here at this point I don't know

*/
void ATSHeader72::set_samplefreq_orig(const double& samplefreq_orig)
{
   if (samplefreq_orig != DBL_MAX) this->SampleFreqOrig = float(samplefreq_orig);
}



/*!
    \fn ITSHeader::get_sensor_pos(double& x1, double& y1, double& z1, double& x2, double& y2, double& z2)
    regardless whether you have dipole length or ange and so on the interface class uses 3Dim setup
 */
void ATSHeader72::get_sensor_pos(double& dx1, double& dy1, double& dz1, double& dx2, double& dy2, double& dz2)
{

  // clean
  double assume_zero_length = 0.00001;

  if (abs(this->x1) < assume_zero_length) this->x1 = 0.0;
  if (abs(this->x2) < assume_zero_length) this->x2 = 0.0;
  if (abs(this->y1) < assume_zero_length) this->y1 = 0.0;
  if (abs(this->y2) < assume_zero_length) this->y2 = 0.0;
  if (abs(this->z1) < assume_zero_length) this->z1 = 0.0;
  if (abs(this->z2) < assume_zero_length) this->z2 = 0.0;

  // it can happen that dipole was used instead of x,y,z
  if ( (x1 == x2) && (y1 == y2) && (abs(DipoleLength) > assume_zero_length) ) {

    this->x1 = -cos(M_PI / 180. * Angle) * DipoleLength / 2.;
    this->x2 =  cos(M_PI / 180. * Angle) * DipoleLength / 2.;
    this->y1 = -sin(M_PI / 180. * Angle) * DipoleLength / 2.;
    this->y2 =  sin(M_PI / 180. * Angle) * DipoleLength / 2.;
          // clean values from numeriocal artefacts
    if  (abs(this->x1) <  assume_zero_length) this->x1 = 0.;
    if  (abs(this->x2) <  assume_zero_length) this->x2 = 0.;
    if  (abs(this->y1) <  assume_zero_length) this->y1 = 0.;
    if  (abs(this->y2) <  assume_zero_length) this->y2 = 0.;
    if  (abs(this->z1) <  assume_zero_length) this->z1 = 0.;
    if  (abs(this->z2) <  assume_zero_length) this->z2 = 0.;


  }

    dx1 = double(this->x1);
    dx2 = double(this->x2);
    dy1 = double(this->y1);
    dy2 = double(this->y2);
    dz1 = double(this->z1);
    dz2 = double(this->z2);
}


/*!
    \fn ATSHeader72::get_coordinates(int& lagra, int& lamin, double& lasec, int& logra, int& lomin, double& losec)
 */
void ATSHeader72::get_coordinates(int& lagra, int& lamin, double& lasec, string& LatH, int& logra, int& lomin, double& losec, string& LongH) const
{
    coordinates coordi;
    coordi.getlatlon(this->atsLatitude, this->atsLongitude);

    
    coordi.get_lat_lon(lagra, lamin, lasec, LatH, logra, lomin, losec, LongH);
   
}

/*!
    \fn ATSHeader72::get_bits(size_t& adcbits, size_t& storagebits)
    fixed values for ats version 0.72
 */
void ATSHeader72::get_bits(size_t& adcbits, size_t& storagebits) const
{
    adcbits = 24;
    storagebits = 32;
}



/*!
    \fn ATSHeader72::get_sensor_calfile()
    mostly we have SENSOR.CAL which is not a cal file; therfore
    we generate a calibration file from Sensortype and serialnumber
 */
string ATSHeader72::get_sensor_calfile() const
{


    string sensor_calfile_used;

    // get info like MFS EFP
    string SensorType = this->get_sensor_type();
    // get the serial num like 123
    string SensorNum = this->get_sensor_id();
    // get the calibration file entry - which unfortunately mostly SENSOR.CAL
    string sensor_calfile(&this->csensor_cal_file_name[0], sizeof(this->csensor_cal_file_name));

    // clean an lower all strings
    clean_b_str(sensor_calfile);
    sensor_calfile_used = SensorType;                     // should be mfs06 or mfs05 etc

    stringstream convert_num;

          // sensor.cal is a empty template
  if (  (return_lower(sensor_calfile) == "sensor.cal")
        && (return_lower(SensorType) != "efp05") && (return_lower(SensorType) != "efp06")    ) {        // no file is given; template no electrodes
        if (this->SensorNo < 10)       sensor_calfile_used = trim(sensor_calfile_used) + "00"; // mfs0600 now
        else if (this->SensorNo < 100) sensor_calfile_used = trim(sensor_calfile_used) + "0";  // mfs060 now
        else                           sensor_calfile_used = trim(sensor_calfile_used);        // mfs06
        sensor_calfile_used += (num2str(this->SensorNo) + ".txt");

        sensor_calfile =  sensor_calfile_used;
        #ifdef CLASSDBG
        cerr << "ATSHeader72::get_sensor_calfile-> " << sensor_calfile << endl;
        #endif

  }

        // check that out
  else if ( (return_lower(sensor_calfile) != "sensor.cal")                                   // has a cal file name
        && ( return_lower(SensorType) != "efp05") &&   ( return_lower(SensorType) != "efp06")    ) {             // also provide to use electrodes

        #ifdef CLASSDBG
        cerr << "ATSHeader72::get_sensor_calfile-> " << sensor_calfile << endl;
        #endif
  }

  else if ( (return_lower(SensorType) == "efp05") ||  (return_lower(SensorType) == "efp06") || 
						 (return_lower(SensorType) == "unkn_e")  || (return_lower(SensorType) == "linear"))  {
        #ifdef CLASSDBG
        cerr << "ATSHeader72::get_sensor_calfile-> no transfer function used for " << SensorType << ": linear" << endl;
        #endif
        sensor_calfile = "";
  }


  else  {
    cerr << "ATSHeader72::get_sensor_calfile-> fail to make a name returning " << sensor_calfile << endl;
  }

  return sensor_calfile;

}


/*!
    \fn ATSHeader72::get_system_calfile(string& system_calfile)
 */
string ATSHeader72::get_system_calfile() const
{

    // do not use aduxxx.txt that is a fake
    // adu has no calibration
    string tmp(&this->cadu_cal_file_name[0], sizeof(this->cadu_cal_file_name));
    //lower(tmp);
    clean_b_str(tmp);
    // avoid to return ADU..TXT !!!
    if ((tmp.find("adu") != tmp.npos) || (tmp.find("ADU") != tmp.npos)) return "";
    else return tmp;
}


/*!
    \fn ITSHeader::get_system_name()
 */
string ATSHeader72::get_system_name() const
{
   string check = this->cSystemType;
   clean_b_str(check);
   lower(check);

   if (check == "adu06" || check == "adu-06") return "ADU06";
   if (check == "adu07" || check == "adu-07") return "ADU07";
   if (check == "emimt24" || check == "emi-mt24"|| check == "emi mt24") return "EMIMT24";
   if (check == "goerap" || check == "rap") return "GOERAP";
   if (check == "linear") return "linear";


   else return "ADU06";
}

string ATSHeader72::get_system_id() const {

return num2str(this->SerNoADU);

}


void ATSHeader72::set_channel_no(const size_t& channel_no) {

    this->ChanNo = (char) channel_no;
//    cerr << "got -----------------------------------" <<  this->ChanNo << channel_no << endl;
}

size_t ATSHeader72::get_channel_no() const {

//    cerr << "put -----------------------------------" <<  this->ChanNo << size_t(this->ChanNo) << endl;
    return this->ChanNo;
}


string ATSHeader72::get_channel_id() const {

return num2str(this->SerNoADB);


}

string ATSHeader72::get_sensor_id() const {
return num2str(this->SensorNo);


}

/*!
    \fn ITSHeader::get_sensor_type()
    something MFS
 */
string ATSHeader72::get_sensor_type() const
{

   string tmp(&this->cSensorType[0], sizeof(this->cSensorType));
   
   clean_b_str(tmp);
   return tmp;

}




double ATSHeader72::get_sensor_resistivity() const{
return double(this->ProbeResistivity);
}

long ATSHeader72::get_gmt_offset() const{
return long(this->GMTOffset);

}

long ATSHeader72::get_utc_to_gps_offset() const {

 return long (UTCGPSOffset);
}


int ATSHeader72::get_is_gmt() const{
return 1;

}

double ATSHeader72::get_dc_offset() const{

return double(this->DCOffset);
}

double ATSHeader72::get_gain() const{

return double(this->Gain);

}


/*
this has to be change asap - not stored in header yet

*/
string ATSHeader72::get_sensor_chopper() const{

  if (this->cSensorChopper == 1) return "on";
  else if (this->cSystemChopper == 0) return "off";
  else return "off";


}


string ATSHeader72::get_system_chopper() const {


  if (this->cSystemChopper == 0) return "on";
  else return "off";

}



/*
elevation is in cm defined in ATS format
*/
double ATSHeader72::get_elevation() const {

return double (this->atsElevation / 100.);

}


string ATSHeader72::get_meastype() const {

string tmp(&this->meas_type[0], sizeof(this->meas_type));
clean_b_str(tmp);
return tmp;


}


string ATSHeader72::get_system_selftest_file() const {
 string tmp(&this->clog_file_sys_self_test[0], sizeof(this->clog_file_sys_self_test));
 clean_b_str(tmp);
 return tmp;

}


string ATSHeader72::get_result_selftest() const {
 string tmp(&this->result_self_test[0], sizeof(this->result_self_test));
 clean_b_str(tmp);
 return tmp;

}


string ATSHeader72::get_client() const {
 string tmp(&this->cClient[0], sizeof(this->cClient));
 clean_b_str(tmp);
 return tmp;

}


string ATSHeader72::get_contractor() const {
 string tmp(&this->cContractor[0], sizeof(this->cContractor));
 clean_b_str(tmp);
 return tmp;

}


string ATSHeader72::get_area() const {
 string tmp(&this->cArea[0], sizeof(this->cArea));
 clean_b_str(tmp);
 return tmp;

}


string ATSHeader72::get_survey_id() const {
 string tmp(&this->cSurveyID[0], sizeof(this->cSurveyID));
 clean_b_str(tmp);
 return tmp;

}


string ATSHeader72::get_system_operator() const {
 string tmp(&this->cOperator[0], sizeof(this->cOperator));
 clean_b_str(tmp);
 return tmp;

}


string ATSHeader72::get_xmlheader() const {
 string tmp(&this->Achxmlheader[0], sizeof(this->Achxmlheader));
 clean_b_str(tmp);
 return tmp;

}


string ATSHeader72::get_comments() const {
 string tmp(&this->cComments[0], sizeof(this->cComments));
 clean_b_str(tmp);
 return tmp;

}






void ATSHeader72::clean_header(){

//HeaderVer = ATSHEADERVERSION;
this->HeaderVer = 73;
this->HeaderLength = this->SerNoADU = this->SerNoADB = this->ChanNo = this->SensorNo = this->Ref_Meridian = 0;
this->GMTOffset = this->atsLatitude = this->atsLongitude = this->atsElevation = 0;
this->UTCGPSOffset = 0;
this->SampleFreq = 0;
this->SampleFreqOrig = 0;
this->DipoleLength = this->ProbeResistivity = this->DCOffset = this->Gain = this->Gain2 = this->Angle =
        this->x1 = this->y1 = this->z1 = this->x2 = this->y2 = this->z2 = 0.;
this->Samples = 0;  this->atsDateTime = 0;
this->date_of_cal_adu = 0;  this->date_of_cal_sensor = 0;
this->LSBVal = this->x = this->y = 0.0;

this->num_cal_freq = this->lofe = this->vocf = this->start_cal_info_head = 0;

// new in 73 Version
this->atsZoneNumber = 0;

this->atsNorthing = this->atsEasting = 0;
this->atsutm_meridian = 0;

//this->cSensorChopper = 0;
//this->cSystemChopper = 0;
//this->Sync_Status = 0;
//this->LatLon_Type = 0;
//this->Coord_Type = 0;
//this->Accuracy = 0;

// empty all strings

//  string_z( this->skip1,  sizeof( this->skip1));
//  string_z( this->cChannelType,  sizeof( this->cChannelType));
//  string_z( this->cSensorType, sizeof( this->cSensorType));
  //string_z( this->skip2, sizeof( this->skip2));
//  string_z( this->cSystemType, sizeof( this->cSystemType));
//  string_z( this->csurvey_header_fname, sizeof( this->csurvey_header_fname));
//  string_z( this->meas_type, sizeof( this->meas_type));
//  string_z( this->clog_file_sys_self_test, sizeof( this->clog_file_sys_self_test));
//  string_z( this->result_self_test, sizeof( this->result_self_test));
//  string_z( this->skip4, sizeof( this->skip4));
//  string_z( this->skip5, sizeof( this->skip5));
//  string_z( this->cadu_cal_file_name, sizeof( this->cadu_cal_file_name));
//  string_z( this->csensor_cal_file_name, sizeof( this->csensor_cal_file_name));
//  strncpy( this->csensor_cal_file_name, " ",  sizeof(csensor_cal_file_name));
  //string_z( this->skip6, sizeof( this->skip6));

//  string_z( this->cClient, sizeof( this->cClient));
//  string_z( this->cContractor, sizeof( this->cContractor));
//  string_z( this->cArea, sizeof( this->cArea));
//  string_z( this->cSurveyID, sizeof( this->cSurveyID));
//  string_z( this->cOperator, sizeof( this->cOperator));
//  string_z( this->cskip7, sizeof( this->cskip7));
//  string_z( this->Achxmlheader, sizeof( this->Achxmlheader));
//  string_z( this->cComments, sizeof( this->cComments));

//  this->atsLetterDesignator = ' ';
  string_z( this->atsemptystr, sizeof(this->atsemptystr));
//  string_z( this->ccoordinates_type, sizeof( this->ccoordinates_type));
//  string_z( this->cReferenceEllipsoid, sizeof( this->cReferenceEllipsoid));

}

void ATSHeader72::byteswap_header(){
    // byteswap of the header


    //tbswap(HeaderLength);
    //tbswap(HeaderVer);

    if (tbswap(this->Samples) != 4) {
      cerr << " int has not 4 bytes, exit" << endl;
      exit(0);

    }

    if (tbswap(this->SampleFreq) != 4) {
      cerr << "float has not 4 bytes, exit" << endl;
      exit(0);
    }

    if (tbswap(this->SampleFreqOrig) != 4) {
      cerr << "float has not 4 bytes, exit" << endl;
      exit(0);
    }

    tbswap(this->atsDateTime);
    if (tbswap(this->LSBVal) != 8) {
      cerr << "double has not 8 bytes, exit" << endl;
      exit(0);
    }
    tbswap(this->GMTOffset);

    tbswap(this->DipoleLength); tbswap( this->ProbeResistivity);
    tbswap( this->DCOffset); tbswap( this->Gain); tbswap( this->Gain2); tbswap( this->Angle);
    tbswap( this->x1); tbswap( this->y1); tbswap( this->z1);
    tbswap( this->x2); tbswap( this->y2); tbswap( this->z2);

    tbswap( this->x); tbswap( this->y);



    tbswap( this->atsLatitude);
    tbswap( this->atsLongitude);
    tbswap( this->atsElevation);
    tbswap( this->date_of_cal_adu);
    tbswap( this->date_of_cal_sensor);

    tbswap( this->atsNorthing);
    tbswap( this->atsEasting);

}

/*
get something like 123 as serial number;
ats can only store a number
*/

void ATSHeader72::set_channel_type(const string& channel_type) {

   string_z(this->cChannelType, sizeof(this->cChannelType));
   channel_type.copy(&this->cChannelType[0], sizeof(this->cChannelType));


}

void ATSHeader72::set_sensor_pos(const double& x1, const double& y1, const double& z1, const double& x2, const double& y2, const double& z2) {


  if (x1 != DBL_MAX) this->x1 = float(x1);
  if (y1 != DBL_MAX) this->y1 = float(y1);
  if (z1 != DBL_MAX) this->z1 = float(z1);
  if (x2 != DBL_MAX) this->x2 = float(x2);
  if (y2 != DBL_MAX) this->y2 = float(y2);
  if (z2 != DBL_MAX) this->z2 = float(z2);



    double tx, ty, tz;
    tx = this->x2 - this->x1;
    ty = this->y2 - this->y1;
    tz = this->z2 - this->z1;
    DipoleLength = float (sqrt(tx * tx  + ty * ty + tz * tz));
    Angle = float(  180. / M_PI * atan2(ty,tx));



}
void ATSHeader72::set_coordinates(const int& lagra, const int& lamin, const double& lasec, const string& LatH, const int& logra, const int& lomin, const double& losec, const string& LonH) {

coordinates coordi;
    coordi.getlatlon(lagra, lamin, lasec, LatH, logra, lomin, losec, LonH);
    coordi.get_msecs(atsLatitude, atsLongitude);

}


void ATSHeader72::set_bits(const size_t& adcbits, const size_t& storagebits) {

  // not defined in ats ver. 0.72; default 24bit ADC and 32bit int

}

void ATSHeader72::set_sensor_calfile(const string& sensor_cal_file)  {

  string_z(this->csensor_cal_file_name, sizeof(this->csensor_cal_file_name));
  sensor_cal_file.copy(&this->csensor_cal_file_name[0], sizeof(this->csensor_cal_file_name));
}

void ATSHeader72::set_system_calfile(const string& system_calfile)  {

 string_z(cadu_cal_file_name, sizeof(cadu_cal_file_name));
 system_calfile.copy(&this->cadu_cal_file_name[0], sizeof(this->cadu_cal_file_name));
}


void ATSHeader72::set_system_name(const string& data_logger_name)  {
 string_z(cSystemType, sizeof(cSystemType));
 data_logger_name.copy(&this->cSystemType[0], sizeof(this->cSystemType));



}

void ATSHeader72::set_channel_id(const string& channel_id)  {

  unsigned short int tmp = str2num<unsigned short int>(channel_id);
  if (tmp != USHRT_MAX)

  this->SerNoADB = str2num<unsigned short int>(channel_id);


}

/*!
ATS uses a number only
*/
void ATSHeader72::set_sensor_id(const string& sensor_id)  {
  unsigned short int tmp = str2num<unsigned short int>(sensor_id);
  if (tmp != USHRT_MAX)
  this->SensorNo = str2num<unsigned short int>(sensor_id);


}

/*!
ATS uses a number only
*/
void ATSHeader72::set_system_id(const string& system_id)  {
  unsigned short int tmp = str2num<unsigned short int>(system_id);
  if (tmp != USHRT_MAX)
  this->SerNoADU = str2num<unsigned short int>(system_id);

}


void ATSHeader72::set_sensor_type(const string& sensor_type)  {
  string_z(this->cSensorType, sizeof(this->cSensorType));
  sensor_type.copy(&this->cSensorType[0], sizeof(this->cSensorType));
}


void ATSHeader72::set_sensor_resistivity(const double& sensor_resistivity){
  if (sensor_resistivity != DBL_MAX) this->ProbeResistivity = float(sensor_resistivity);

}

void ATSHeader72::set_gmt_offset(const long& gmt_offset){
 if (gmt_offset != USHRT_MAX) this->GMTOffset = int(gmt_offset);
}


void ATSHeader72::set_utc_to_gps_offset(const long int& utc_to_gps_offset) {
  this->UTCGPSOffset = (short int) utc_to_gps_offset;

}

void ATSHeader72::set_is_gmt(const int& is_gmt){

}

void ATSHeader72::set_dc_offset(const double& dc_offset){
  if (dc_offset != DBL_MAX) this->DCOffset = float(dc_offset);
}

void ATSHeader72::set_gain(const double& gain){
  if (gain != DBL_MAX) this->Gain = float(gain);
}


void ATSHeader72::set_system_chopper(const string& system_chopper) {
if (system_chopper == "on") this->cSystemChopper = 1;
else this->cSystemChopper = 0;
}



void ATSHeader72::set_sensor_chopper(const string& sensor_chopper){
if (sensor_chopper == "on") this->cSensorChopper = 1;
else this->cSensorChopper = 0;
}


/*
elevation is in cm defined in ATS format
*/
void ATSHeader72::set_elevation(const double& elevation) {

 if (elevation != DBL_MAX) this->atsElevation = int (elevation * 100.);

}

void ATSHeader72::set_meastype(const string& meastype) {
  meastype.copy(&this->meas_type[0], sizeof(this->meas_type));
}

void ATSHeader72::set_system_selftest_file(const string& system_selftest_file) {
  system_selftest_file.copy(&this->clog_file_sys_self_test[0], sizeof(this->clog_file_sys_self_test));
}

void ATSHeader72::set_result_selftest(const string& result_selftest) {
 result_selftest.copy(&this->result_self_test[0], sizeof(this->result_self_test));
}

void ATSHeader72::set_client(const string& client) {
 client.copy(&this->cClient[0], sizeof(this->cClient));
}

void ATSHeader72::set_contractor(const string& contractor) {
 contractor.copy(&this->cContractor[0], sizeof(this->cContractor));
}

void ATSHeader72::set_area(const string& area) {
 area.copy(&this->cArea[0], sizeof(this->cArea));
}

void ATSHeader72::set_survey_id(const string& survey_id) {
 survey_id.copy(&this->cSurveyID[0], sizeof(this->cSurveyID));
}

void ATSHeader72::set_system_operator(const string& system_operator) {
 system_operator.copy(&this->cOperator[0], sizeof(this->cOperator));
}

void ATSHeader72::set_xmlheader(const string& achxmlheader) {
  achxmlheader.copy(&this->Achxmlheader[0], sizeof(this->Achxmlheader));
}

void ATSHeader72::set_comments(const string& comments) {
  comments.copy(&this->cComments[0], sizeof(this->cComments));
}


 void ATSHeader72::set_tslices(const vector<ATSSliceHeader>& tslices) {

}




