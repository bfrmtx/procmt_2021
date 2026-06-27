#include "atsfilename.h"
atsfilename::atsfilename() {

    this->setupme();

}

void atsfilename::setupme() {
    fill = '0';
    is_init = false;
    //  008_V01_C00_R000_TEx_BL_32H.ATS

    // protect me against "inf"
    imap["aduser"] = 0;
    imap["version"] = 0;
    imap["channel"] = 0;
    imap["run"] = 0;

    smap["aduser"] = "   ";
    smap["version"]=  "  ";
    smap["channel"]= "  ";
    smap["run"] =  "   ";
    smap["type"] = "  ";
    smap["board"] = " ";
    smap["sample"] = "      ";
    smap["sample_unit"] = " ";
}

atsfilename::atsfilename(const QString& name) {
    this->setupme();

    qf.setFile(name);
    this->init();


}

atsfilename::atsfilename(const QFileInfo& qfileinfo) {
    this->setupme();

    qf = qfileinfo;
    this->init();
}

void atsfilename::setAtsfile(const QFileInfo& qfileinfo) {
    this->setupme();

    qf = qfileinfo;
    this->init();
}

bool atsfilename::setName(const QString& name) {
    this->setupme();
    this->qf.setFile(name);
    return this->init();
}
atsfilename::~atsfilename() {

}

bool atsfilename::init() {
    this->is_init = false;
    this->serial_list_qstrl.clear();
    this->t_name.clear();
    this->name07.clear();
    this->name06.clear();
    this->dir = qf.absolutePath();
    this->t_name = qf.baseName();
    this->ext = qf.completeSuffix();
    this->sample_freq_s = this->sample_freq_h = 0;
    this->bparse();
    this->bserials();
    this->bchannels();
    this->bruns();
    this->btypes();
    this->bbands();
    this->is_init = true;
    return is_init;
}

bool atsfilename::bparse() {
    if (!is_init) {
        QStringList tokens;
        if (!t_name.length()) return false;

        if (this->t_name.indexOf("_C") != -1) {
            is_name07 = true;
            //  008_V01_C00_R000_TEx_BL_32H.ATS
            tokens = t_name.split("_");

            stmp = tokens[0];
            this->smap["aduser"] = stmp.mid(0);


            stmp = tokens[1];
            this->smap["version"] = stmp.mid(1);

            stmp = tokens[2];
            this->smap["channel"] = stmp.mid(1);

            stmp = tokens[3];
            this->smap["run"] = stmp.mid(1);

            stmp = tokens[4];
            this->smap["type"] = stmp.mid(1);

            stmp = tokens[5];
            this->smap["board"] = stmp.mid(1);



            // first grep the H Hertz or S Seconds
            this->smap["sample_unit"] = tokens[6].right(1);

            this->smap["sample"] = tokens[6].left(tokens[6].indexOf(smap["sample_unit"]));

            if (smap["sample_unit"].toUpper() == "H") {
                this->sample_freq_h = this->smap["sample"].toInt();
                this->sample_freq_s = 0;
            } else {
                this->sample_freq_s = this->smap["sample"].toInt();
                this->sample_freq_h = 0;
            }
            // set up integer map
            this->sync_imap();


        } else {
            this->is_name07 = false;

        }

        return true;
    }
    return is_init;
}

bool atsfilename::bchannels() {
    if (!is_init) {
        if (this->is_name07) {
            if (smap["channel"].toUpper() == "00")
                this->chan_06_str = "A";
            if (smap["channel"].toUpper() == "01")
                this->chan_06_str = "B";
            if (smap["channel"].toUpper() == "02")
                this->chan_06_str = "C";
            if (smap["channel"].toUpper() == "03")
                this->chan_06_str = "D";
            if (smap["channel"].toUpper() == "04")
                this->chan_06_str = "E";

            if (smap["channel"].toUpper() == "05")
                this->chan_06_str = "A";
            if (smap["channel"].toUpper() == "06")
                this->chan_06_str = "B";
            if (smap["channel"].toUpper() == "07")
                this->chan_06_str = "C";
            if (smap["channel"].toUpper() == "08")
                this->chan_06_str = "D";
            if (smap["channel"].toUpper() == "09")
                this->chan_06_str = "E";



        } else {
            if (chan_06_str.toUpper() == "A")
                this->smap["channel"] = "00";
            if (chan_06_str.toUpper() == "B")
                this->smap["channel"] = "01";
            if (chan_06_str.toUpper() == "C")
                this->smap["channel"] = "02";
            if (chan_06_str.toUpper() == "D")
                this->smap["channel"] = "03";
            if (chan_06_str.toUpper() == "E")
                this->smap["channel"] = "04";
            if (chan_06_str.toUpper() == "F")
                this->smap["channel"] = "05";
            if (chan_06_str.toUpper() == "G")
                this->smap["channel"] = "06";
            if (chan_06_str.toUpper() == "H")
                this->smap["channel"] = "07";
            this->imap["channel"] = this->smap["channel"].toInt();





        }
        return true;
    }
    return is_init;
}


bool atsfilename::adu06_channels_from_07type() {
  
  if (is_init) {
   if (is_name07) {
    // this->bchannels();
      if (smap["type"].toUpper() == "EX")
        this->chan_06_str = "A";
      if (smap["type"].toUpper() == "EY")
        this->chan_06_str = "B";
      if (smap["type"].toUpper() == "HX")
        this->chan_06_str = "C";
      if (smap["type"].toUpper() == "HY")
        this->chan_06_str = "D";
      if (smap["type"].toUpper() == "HZ")
        this->chan_06_str = "E";
      if (smap["type"].toUpper() == "REX")
        this->chan_06_str = "A";
      if (smap["type"].toUpper() == "REY")
        this->chan_06_str = "B";
      if (smap["type"].toUpper() == "RHX")
        this->chan_06_str = "C";
      if (smap["type"].toUpper() == "RHY")
        this->chan_06_str = "D";
      if (smap["type"].toUpper() == "RHZ")
        this->chan_06_str = "E";      
    }



    return true;
  }
  return is_init;

}

bool atsfilename::btypes() {
    if (!is_init) {
        if (is_name07) {

            if (smap["type"].toUpper() == "EX")
                type_06_str = "A";
            if (smap["type"].toUpper() == "EY")
                type_06_str = "B";
            if (smap["type"].toUpper() == "HX")
                type_06_str = "X";
            if (smap["type"].toUpper() == "HY")
                type_06_str = "Y";
            if (smap["type"].toUpper() == "HZ")
                type_06_str = "Z";
        }

        else {

            if (type_06_str.toUpper() == "A")
                smap["type"] = "EX";
            if (type_06_str.toUpper() == "B")
                smap["type"] = "EY";
            if (type_06_str.toUpper() == "X")
                smap["type"] = "HX";
            if (type_06_str.toUpper() == "Y")
                smap["type"] = "HY";
            if (type_06_str.toUpper() == "Z")
                smap["type"] = "HZ";

        }
        return true;
    }
    return is_init;
}


bool atsfilename::bruns() {
    if (!is_init) {
        if (is_name07) {
            this->run_06_str = smap["run"];
            this->run_06_str.remove(0,1);

        } else {
            this->smap["run"] = "0" + run_06_str;
            this->imap["run"] = this->smap["run"].toInt();
        }
        return true;
    }
    return is_init;
}


bool atsfilename::bserials() {
    if (!is_init) {
        if (is_name07) ser_06_str = smap["aduser"];
        else {
            this->smap["aduser"] = ser_06_str;
            this->imap["aduser"] = this->smap["aduser"].toInt();
        }
        return true;
    }
    return is_init;
}

bool atsfilename::bbands() {
    if (!is_init) {
        if (sample_freq_h > 4096.)
            band_06_str = "A";
        else if (sample_freq_h >= 1024. && sample_freq_h <= 4096.)
            band_06_str = "B";
        else if (sample_freq_h >= 256. && sample_freq_h < 1024.)
            band_06_str = "F";
        else if (sample_freq_h >= 64. && sample_freq_h < 256.)
            band_06_str = "C";
        else if (sample_freq_h > 1. && sample_freq_h < 64.)
            band_06_str = "D";

        else if (sample_freq_s >= 1. && sample_freq_s <= 16.)
            band_06_str = "E";
        else if (sample_freq_s > 16.)
            band_06_str = "G";

        return true;
    }
    return is_init;
}

QString  atsfilename::get_adu06_filename_str(const QString& serial_str) {

    if (this->is_init) {
        if (serial_str.size()) ser_06_str = serial_str;
//   qDebug() << ser_06_str << chan_06_str << run_06_str << type_06_str << band_06_str << ".ats";
        this->name06 = ser_06_str + chan_06_str + run_06_str + type_06_str + band_06_str + "." + ext;
        return this->name06;
    }
    return "not initialized";

}

QString atsfilename::get_adu07_filename_str() {
    if (is_init) {
        this->name07 = smap["aduser"] + "_V" + smap["version"] + "_C" + smap["channel"] + "_R" + smap["run"] + "_T" + smap["type"] +
                       "_B" + smap["board"] + "_" + smap["sample"] + smap["sample_unit"] + "." + ext;
        return this->name07;
    }
    return "not initialized";

}
std::string atsfilename::get_adu07_filename_stdstr() {
  QString qstrmp = this->get_adu07_filename_str();
  return  qstrmp.toStdString();
 
 }
 
int atsfilename::set_serial_list(const QStringList& serial_list_qstrl) {
    if (is_init) {

        if (serial_list_qstrl.size()) this->serial_list_qstrl = serial_list_qstrl;
        return this->serial_list_qstrl.size();
    }
    return 0;
}

int atsfilename::has_list() {
    if (is_init) {

        return serial_list_qstrl.size();
    }
    return 0;
}

QString atsfilename::get_serial_from_list_str(const qint32& index) {
    if (is_init) {
        QString tmp;
        if (index < serial_list_qstrl.size()) {

            if (serial_list_qstrl[index].size() > 3) return QString("999");
            if (serial_list_qstrl[index].size() == 3) return  serial_list_qstrl[index] ;
            if (serial_list_qstrl[index].size()  == 2) return QString("0") + serial_list_qstrl[index] ;
            if (serial_list_qstrl[index].size() == 1) return QString("00") + serial_list_qstrl[index] ;


        } else return QString("999");
        return QString("atsfilename:get_serial_from_list::error");
    }
    return "not initialized";
}

bool atsfilename::rename_atsfile(const QString& toadu06_or_toadu07) {
    if (!is_init) return false;
    QDir::setCurrent(dir);
    QFile file;
    // this is a bug; the file.setFileName should do this - but it does not!
    this->get_adu06_filename_str();
    this->get_adu07_filename_str();
    if (toadu06_or_toadu07 == "toadu06")  file.setFileName (this->get_adu07_filename_str());
    if (toadu06_or_toadu07 == "toadu07")  file.setFileName (this->get_adu06_filename_str());
    if (!file.exists()) qDebug() << " rename_atsfile " << file.fileName() << " does not exits";

    if (toadu06_or_toadu07 == "toadu06")  return file.rename(name06);
    if (toadu06_or_toadu07 == "toadu07") return file.rename(name07);
    return false;
}

bool atsfilename::set_run_06(const QString& srun) {

    if (!is_init) return false;

    this->run_06_str = srun.right(2);

    return true;


}



bool atsfilename::set_run_07(const QString& srun) {
 
  if (srun.size() == 1) this->smap["run"] = "00" + srun.right(3);
  if (srun.size() == 2) this->smap["run"] = "0" + srun.right(3);
  else   this->smap["run"] = srun.right(3);
    this->imap["run"] = this->smap["run"].toInt();
    if (this->imap["run"] < 1000) return true;
    else return false;
}


bool atsfilename::set_run_07(const size_t& irun) {
    
    if (irun < 1000) {
        this->imap["run"] = irun;
        this->smap["run"] = QString("%1").arg(imap["run"], 3, 10, fill);
        return true;
    } else return false;


}

bool atsfilename::set_chan_07(const QString& schan) {
  
  if (schan.size() == 1) this->smap["channel"] = "0" + schan.right(2);
  else this->smap["channel"] = schan.right(2);
  this->imap["channel"] = this->smap["channel"].toInt();

  if (this->imap["channel"] < 100) return true;
  else return false;
}


bool atsfilename::set_chan_07(const size_t& ichan) {
    

    if (ichan < 100) {
        this->imap["channel"] = ichan;
        this->smap["channel"]= QString("%1").arg(imap["channel"], 2, 10, fill);

        return true;
    } else return false;
}

bool atsfilename::set_serial_07(const QString& sser) {
 
  if (sser.size() == 1) this->smap["aduser"] = "00" + sser.right(3);
  if (sser.size() == 2) this->smap["aduser"] = "0" + sser.right(3);
  else this->smap["aduser"] = sser.right(3);
  this->imap["aduser"] = this->smap["aduser"].toInt();

  if (this->imap["aduser"] < 1000) return true;
  else return false;
}


bool atsfilename::set_serial_07(const size_t& iser) {
  

  if (iser < 1000) {
    this->imap["aduser"] = iser;
    this->smap["aduser"]= QString("%1").arg(imap["aduser"], 3, 10, fill);

    return true;
  } else return false;
}



bool atsfilename::set_serial_06(const QString& sser) {
 

  this->ser_06_str = sser.right(3);
  //this->imap["aduser"] = this->smap["aduser"].toInt();

  return true;
  //if (this->imap["aduser"] < 100) return true;
  //else return false;
}


bool atsfilename::set_serial_06(const size_t& iser) {
  

  if (iser < 1000) {
    
    this->ser_06_str = QString("%1").arg(imap["aduser"], 3, 10, fill);

    return true;
  } else return false;
}








bool atsfilename::set_version_07(const QString& sver) {
 

  this->smap["aduser"] = sver.right(2);
  this->imap["version"] = this->smap["version"].toInt();

  if (this->imap["version"] < 100) return true;
  else return false;
}


bool atsfilename::set_version_07(const size_t& iver) {
  

  if (iver < 1000) {
    this->imap["version"] = iver;
    this->smap["version"]= QString("%1").arg(imap["version"], 3, 10, fill);

    return true;
  } else return false;
}



bool atsfilename::sync_imap() {

    this->imap["aduser"] = this->smap["aduser"].toInt();
    this->imap["version"] = this->smap["version"].toInt();
    this->imap["channel"] = this->smap["channel"].toInt();
    this->imap["run"] = this->smap["run"].toInt();

    return true;

}



bool atsfilename::sync_smap() {
    this->smap["aduser"] = QString("%1").arg(imap["aduser"], 3, 10, fill);
    this->smap["version"]= QString("%1").arg(imap["version"], 2, 10, fill);
    this->smap["channel"]= QString("%1").arg(imap["channel"], 2, 10, fill);
    this->smap["run"] = QString("%1").arg(imap["run"], 3, 10, fill);

    return true;
}




bool atsfilename::set_type(const QString& stype)
{
   this->smap["type"] = stype;
   return true;
}


bool atsfilename::set_sample_freq(const double& dsample)
{
  
   if (dsample >= 1.0) {
     this->sample_freq_h = (int)dsample;
     this->smap["sample"] = QString("%1").arg(this->sample_freq_h );
     this->smap["sample_unit"] = "H";
     this->sample_freq_s = 0;
   }
   else {
     this->sample_freq_h = 0;
     this->sample_freq_s = (int)(1./dsample);

     this->smap["sample"] = QString("%1").arg(this->sample_freq_s);
     this->smap["sample_unit"] = "S";


   }
   return true;
}




bool atsfilename::create_atsname(const size_t& aduser, const size_t& version, const size_t& channel, const size_t& run, const QString& stype, const QString& sboard, const double& dsample_freq)
{
  bool status = true;
  if (status) status = this->set_serial_07(aduser); else return status;
  if (status) status = this->set_version_07(version); else return status;
  if (status) status = this->set_chan_07(channel); else return status;
  if (status) status = this->set_run_07(run); else return status;
  if (status) status = this->set_type(stype); else return status;
  if (status) status = this->set_board_07(sboard); else return status;
  if (status) status = this->set_sample_freq(dsample_freq); else return status;
  this->sync_imap();
  this->name07 = smap["aduser"] + "_V" + smap["version"] + "_C" + smap["channel"] + "_R" + smap["run"] + "_T" + smap["type"] +
      "_B" + smap["board"] + "_" + smap["sample"] + smap["sample_unit"] + "." + "ats";

  // now go to the standard initialization
  return this->setName(this->name07);

   
}


/*!
    \fn atsfilename::set_board(const QString& sboard)
 */
bool atsfilename::set_board_07(const QString& sboard)
{
   this->smap["board"] = sboard;
   return true;
}
