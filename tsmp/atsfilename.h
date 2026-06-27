#ifndef ATSFILENAME_H
#define ATSFILENAME_H

#include <QtCore>
#include <QString>
#include <QFileInfo>
#include <QFile>
#include <QMap>
#include <cstring>
#include "allinclude.h"



/**
  @author Dr. Bernhard Friedrichs <bfr@isi7>
*/

/**
@class atsfilename This class takes over the resonsibility of setting the correct filenames
 * for the ADU<br>
 * 008_V01_C00_R009_TEx_BL_32H.ATS<br>
 * nnnADU-serial _ Vnn file-version _Cnn channel _Rnnn run _Tcc channel-type _Bc board-type
 * nnnnnnSampling[H/S] .ats<br>
 * Internally we use 009 as run and not _R009; tokens and keys are automatically generated
 * during output and removed during intput;


*/
class atsfilename {
public:

    /**
     * constructs an atsfilename with a given string
     * @param name 
     */
    atsfilename(const QString& name);
    
//     atsfilename(const QString& name, const QString& out_dir);
//     
//     atsfilename(const string& name, const string& out_dir);
    
    /**
     * constructs an atsfilename with qfileinfo - best way
     * @param qfileinfo 
     */
    atsfilename(const QFileInfo& qfileinfo);

    
    /**
     * sets / constructs an atsfilename with qfileinfo - best way
     * @param qfileinfo
     */
    void setAtsfile(const QFileInfo& qfileinfo);
     void setupme();

    
    /**
     * empty constructor - do not use!<br>>
     * this constructor is needed for vector classes in order to write something like QVector<atsfilename> atsfilenames where only a default constructor can be used;<br>
     * use setAtsfile() a.s.a.p; serials and runs are set at least to 0 in order
     * to protect the class agins "inf"
     */
    atsfilename();
    
    /**
     * if the empty constructor was called at least this function must be called once, like
     * atsfilenames.append(atsfilename(cmds[(cmds.indexOf("-files")+1)]));
     * @param name filename  
     * @return 
     */
    bool setName(const QString& name);

    
    /**
     * destructor
     */
    ~atsfilename();

    
    /**
     * 
     * @param serial_str optional: in case a new serial number shall be used supply here
     * @return the newly constructed atfilename of acient style: 020C01XF.ats
     */
    QString get_adu06_filename_str(const QString& serial_str = "");

    
    /**
     * 
     * @return a name in ADU07 convention like 008_V01_C00_R000_TEx_BL_32H.ATS
     */
    QString get_adu07_filename_str();
    
    /**
     * 
     * @return a name as std::string in ADU07 convention like 008_V01_C00_R000_TEx_BL_32H.ATS
     */
    
    std::string get_adu07_filename_stdstr();

    
    /**
     * 
     * @param serial_list_qstrl a list of serial numbers seperated by space: 12 13 14 ...
     * in case given this list can be used from main by get serial - wherever you are in software
     * @return counts the serial numbers
     */
    qint32 set_serial_list(const QStringList& serial_list_qstrl);

    /**
     * 
     * @param index give list index like 1 and get the corresponding serial like 13 (see above)
     * @return returns a serial number string
     */
    QString get_serial_from_list_str(const qint32& index);

    /**
     * atsfile can rename the file
     * @param toadu06_or_toadu07 use toadu06 to generate old style, use toadu07 to generate new style
     * @return true if successful
     */
    bool rename_atsfile(const QString& toadu06_or_toadu07);

    /**
     * convieninece function - can I use the serial list or not
     * @return 0 if no list is available
     */
    qint32 has_list();


    /**
     * Often required, especially when ADU07 has measured with run 0 accidently
     * @param sruns 
     * @return 
     */
    
    bool set_run_06(const QString& srun);
    
    bool set_run_07(const QString& srun);
    bool set_run_07(const size_t& irun);

    bool set_chan_07(const QString& schan);
    bool set_chan_07(const size_t& ichan);
    
    bool set_serial_07(const QString& sser);
    bool set_serial_07(const size_t& iser);

    bool set_serial_06(const QString& sser);
    bool set_serial_06(const size_t& iser);

    
    
    bool set_version_07(const QString& sver);
    bool set_version_07(const size_t& iver);

    

    
    /**
     * sync all integers (run, channel etc) with the existing strings
     * @return 
     */
    bool sync_imap();

    
    /**
     * sync all strings with existing integers
     * @return 
     */
    bool sync_smap();

    
    /**
     * 
     * @param stype set type as Ex, Hx ...
     * @return 
     */
    bool set_type(const QString& stype);

    
    /**
     * 
     * @param dsample 
     * @return 
     */
    bool set_sample_freq(const double& dsample);
    
    /**
     * 
     * @param aduser 
     * @param version 
     * @param channel 
     * @param run 
     * @param stype 
     * @param sboard 
     * @param dsample_freq 
     * @return 
     */
    bool create_atsname(const size_t& aduser, const size_t& version, const size_t& channel, const size_t& run, const QString& stype, const QString& sboard, const double& dsample_freq);


    /**
     * set the board type
     * @param sboard e.g. H, L, M
     * @return 
     */
    bool set_board_07(const QString& sboard);

    /**
     * instead of using the channel try to map Ex as A and Ey as B an so on
     * @return 
     */
    bool adu06_channels_from_07type();

private:
    bool init();

    
    /**
     * parses an adu07 ats filename an sets serial, version, channel anr run numbers
     * @return 
     */
    bool bparse();
    
    /**
     * maps ADU07 channels to A,B,C,D,E syntax od ADU06 as far as possible<br>
     * and maps ADU06 channels to ADU07 in a meaningful way
     * @return 
     */
    bool bchannels();
    
    bool btypes();

    bool bruns();

    bool bserials();

    bool bbands();

    bool is_init;

    QFileInfo qf;
    QString dir;
    QString out_dir;
    QString ext;
    QString t_name;
    QString name07;
    QString name06;
    bool is_name07;

    //008_V01_C00_R000_TEx_BL_32H.ATS
   
    QString ser_06_str;
    QString chan_06_str;
    QString run_06_str;
    QString type_06_str;

 

  
   
    QString band_06_str;

    int sample_freq_h;
    int sample_freq_s;

    QStringList serial_list_qstrl;

    QString stmp;

    QMap<QString, int>        imap;
    QMap<QString, QString>    smap;
    QChar fill;




};

#endif
