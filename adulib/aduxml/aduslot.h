#ifndef ADUSLOT_H
#define ADUSLOT_H

#include <QString>
#include <QDebug>

#include <QVariant>
#include <QMap>

#include "tinyxml2.h"
#include "qtx_templates.h"


namespace adudefs {


/*!
 * \brief The ukn_hwcfg_db_jobfile enum have I data from HWConfig, database or jobfile
 */
enum adulib_source {

    unknown = 0,
    hwcfg =   1,
    database= 2,
    jobfile = 3,
    measdoc = 4
};
}


/*!
   \brief The aduslot class masks channel 0-4 to 5-9 for 07e; the name however in the exportet XML is channel again
   slot in normal order<br>
   LF HF LF HF LF HF LF HF LF HF<br>
   00 05 01 06 03 07 03 08 04 09<br>
   more details in adbboard
 */
class aduslot
{


public:

    /*!
       \brief aduslot
       \param num we need a physical slot. In ADU08 it is same like channel, in ADU07 we use 0,5 to switch internally from LF to HF
     */
    aduslot(const int &num);


    /*!
       \brief operator = copies the slot number only; the data/prvdata will be copied in the channel only!
       \param rhs
       \return
     */
    aduslot& operator = (const aduslot &rhs);

    /*!
       \brief operator == compares the slot number only - it is a HW comparision
       \param rhs
       \return
     */
    bool operator == (const aduslot &rhs);


    /*!
       \brief get variables with no special cases simply get: derived classes pipe through if they don't have - or set and return
       \param what
       \return
     */
    virtual QVariant get(const QString &what) const;

    QVariant get_selftest(const QString &what) const;

    /*!
       \brief set - variables with no special cases are simply set: derived classes pipe through if they don't have - or set and return; \ref adulib::set()
       \param what
       \param value
       \return
     */
    bool set(const QString &what, const QVariant &value);


    /*!
       \brief prvget private data
       \param what
       \return
     */
    QVariant prvget(const QString &what) const;


    /*!
       \brief prvset this is for laboratory tests only - can be called only befor the job submits
       \param what
       \param value
       \return
     */
    QVariant prvset(const QString &what, const QVariant &value);



    /*!
       \brief is_active
       \return if the board will / can be used
     */
    bool is_active() const;

    int get_slot() const;

    /*!
       \brief append_xml_data write data below <channel id= > ; VARIABLE NAME IS NODE NAME!!
       \param jbchannel
       \param jbdata
       \param what
       \return
     */
    bool append_xml_data(tinyxml2::XMLDocument &xmldoc, tinyxml2::XMLElement *jbchannel, tinyxml2::XMLElement *jbdata, const QString &what);


    /*!
       \brief get_data_size check how big is the container size of data
       \return
     */
    int get_data_size() const;

    /*!
       \brief has_set_error fetch and reset the error status of the last set
       \return
     */
    int has_no_set_error();



protected:



    QMap<QString, QVariant> data;                //!< contains the data to be set and get
    QMap<QString, QVariant> selftest_result;     //!< contains the selftest result for this channel

    QMap<QString, QVariant> prvdata;             //!< contains the private data  - not for regular GUI

    /*!
       \brief set_error 0: error, 1: no error, 2: auto corrected contains a possible error during set;
        will be changed after next set,
        use !set_error to handle, set_error to continue or == 2 to check the auto-corrected

     */
    int no_set_error = 1;




};


//////// not needed now


/*!
   \brief The anyboard class - a helper class for unkown boards or boards without slot
 */
class anyboard {

public:

    anyboard();

    anyboard(const anyboard& rhs);

    anyboard& operator = (const anyboard &rhs);

    bool operator == (const anyboard& rhs) {
        return ( (GMS == rhs.GMS) && (RevMain == rhs.RevMain) && (RevSub == rhs.RevSub)
                 && (Type == rhs.Type));
    }


protected:

    int Type = 0;                           //!< database ID
    int Serial = 0;                         //!< database ID
    int RevMain = 0;                        //!< database ID
    int RevSub = 0;                         //!< database ID
    int GMS = 0;                            //!< database ID
    QString Name;                           //!< database ID - readable name

};


#endif // ADUSLOT_H
