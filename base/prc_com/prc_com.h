/***************************************************************************
**                                                                        **
**  ProcMT and modules                                                    **
**  Copyright (C) 2017-2021 metronix GmbH                                 **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
** Parts of the software are linked against the Open Source Version of Qt **
** Qt source code copy is available under https://www.qt.io               **
** Qt source code modifications are non-proprietary,no modifications made **
** https://www.qt.io/licensing/open-source-lgpl-obligations               **
****************************************************************************
**           Author: metronix geophysics                                  **
**  Website/Contact: www.metronix.de                                      **
**                                                                        **
**                                                                        **
****************************************************************************/

#ifndef PRC_COM_H
#define PRC_COM_H

#include "prc_com_global.h"
#include <iostream>
#include <QDebug>
#include <QVariant>
#include <QVariantList>
#include <QString>
#include <QStringList>
#include <QMap>
#include <mutex>

#include <cfloat>
#include <climits>
#ifdef _msvc
#include <limits>
#endif

#include "qtx_templates.h"


/////////////// MUST NOT INCLUDE Q_OBJECT //////////////////////////////


/*!
 * \brief The prc_com class maintains setting for a QMap with additional functions like max/min and options
 * the class takes care of string, double, int, uint and espeically size_t (aka quint64).<br>
 * the class is for "settings and data transport" but not designed for math and calculations;
 * this should only be done where you set/get values in slow functions<br>
 * My major concern is to PRE-DEFINE variable & type in ADVANCE and FORCE conversion into this WHILE SET vars<br>
 * Qt goes the opposite way, stores undfined and tries to convert when GET the var<br>
 * size_t will be stored as quint64 aka ULongLong internally
 *
 * The smart way:<br>
 * com.insert("samples", (quint64) 0);          reserve a number (can be used as size_t)
 * com.insert("pos_x2", DBL_MAX);               reserve a double - which can be removed if not set
 * com.insert("observatory", "empty");          reserve a string - which can be removed if not set
 *
 * We want good types so do
 * set_key_size_t("samples", 1024);             since this was done already 1024 will be stored as quint64
 * all = sztvalue("samples");                   this is guaranteed now
 * set_key_size_t("samples", "1024");           should not work because 1024 is a string here
 *
 * set_key("pos_x2", 22);                       set a number or QVariant as double type!!! because it was defined above
 * x = dvalue("pos_x2");                        returns a double (and that should be possible now)
 *
 *  strip class_name
 */
class prc_com : public QMap<QString, QVariant>
{


protected:

    mutable std::recursive_mutex mutex;
    QString myname;                                         //!< define my class name

    bool threaded = false;                                 //!< controls if a lock is needed
public:
    prc_com(const bool threaded = false);

    prc_com(const prc_com &rhs);

    prc_com(const QMap<QString, QVariant> &rhs, const bool threaded = false);


    prc_com& operator = (prc_com const &rhs);

    virtual ~prc_com() = default;

    QString myname_is() const;



    void set_class_name(const QString &name, const bool threaded = false);

    /*!
       \brief set_data sets a complete hash into the class - IF they keys already EXISTSs otherwise use insert_data
       \param in_hash
       \param strip_classname when using a prc_com for all classes you prepend either classname_ or global_, if your class is adb and you use adb_samplefreq in the all classes, only the adb class will fish out samplefreq, if use global_ all classes will try to fetch samplefreq (if they have this key)
       \param check_options
     */
    void set_data(const QMap<QString, QVariant>&  in_map,
                  const bool strip_classname,  const bool check_options = false);

    /*!
       \brief instert_data sets a complete hash into the class - keys will be newly created in case
       \param in_hash
       \param strip_classname when using a prc_com for all classes you prepend either classname_ or global_, if your class is adb and you use adb_samplefreq in the all classes, only the adb class will fish out samplefreq, if use global_ all classes will try to fetch samplefreq (if they have this key)

     */
    void insert_data(const QMap<QString, QVariant>&  in_map,
                  const bool strip_classname = true);


    /*!
     * \brief append_data_from_other get a prc_com from other class and append to mine (no check for overwrite!
     * \param in_map e.g. class->get_data()
     * \param prepend_classname_ like "mt_site_" - when I fill it and send back I use the strip_classname option for insert_data
     */
    void append_data_from_other(const QMap<QString, QVariant>&  in_map, const QString prepend_classname_);


    /*!
     * \brief create_from
     * \param key key a string
     * \param value value - to be converted from type which is e.g. text when coming from a database
     * \param type such as int, double quint
     * \return
     */
    QMap<QString, QVariant>::iterator create_from(const QString& key, const QVariant &value, const QString &type);


    /*!
     * \brief get_data returns the internal QMap
     * \return
     */
    QMap<QString, QVariant> get_data() const;

    /*!
       \brief set_key_value set a key value pair - accepted only if EXIST; otherwise maps will blow up
       \param key
       \param value
       \param check_options if true it will be check if inside min/max or list
       \return false in case key does not exist; use insert instead
     */
    bool set_key_value(const QString& key, const QVariant& qvalue, const bool check_options = false);
    bool set_key_size_t(const QString& key, const size_t &qvalue, const bool check_options = false);
    bool set_key_size_t(const QString& key, const QVariant& qvalue, const bool check_options = false);





    /*!
       \brief insert inserts a new key value pair
       \param key
       \param value
       \return
     */
    QMap<QString, QVariant>::iterator insert(const QString& key, const QVariant& value);

    /*!
     * \brief insert_szt size_t is handeled as quint64 - also for 32bit systems
     * \param key
     * \param value
     * \return
     */
    QMap<QString, QVariant>::iterator insert_szt(const QString& key, const size_t &value);

    /*!
     * \brief insert_szt  handeled as quint64 - also for 32/64bit systems
     * \param key
     * \param value
     * \return
     */
    QMap<QString, QVariant>::iterator insert_szt(const QString& key, const QVariant& value);


    /*!
     * \brief insert_double make sure that value arrives as double immedeately
     * \param key
     * \param value
     */
    QMap<QString, QVariant>::iterator insert_double(const QString& key, const QVariant &value);

    /*!
     * \brief insert_int make sure that value arrives as int immedeately
     * \param key
     * \param value
     */
    QMap<QString, QVariant>::iterator insert_int(const QString& key, const QVariant &value);

    /*!
     * \brief insert_uint64 make sure that value arrives as int immedeately
     * \param key
     * \param value
     */
    QMap<QString, QVariant>::iterator insert_uint64(const QString& key, const QVariant &value);

    /*!
     * \brief insert_int64 make sure that value arrives as int immedeately
     * \param key
     * \param value
     */
    QMap<QString, QVariant>::iterator insert_int64(const QString& key, const QVariant &value);




    /*!
      show the content of my map; for transportation you can merge the result into a stringlist
    */
    int show_my_map(const bool show_values = true,  const bool prepend_leading_minus = false) const;


    /*!
       \brief myHashes_toQstringList
       \param prepend_leading_minus add a minus sign before key: useful in case you wnat to show as "options" like -samples 2000
       \return
     */
    QStringList myHashes_toQstringList (bool prepend_leading_minus= false) const;


    /*!
       \brief map_prc_com_to_ClassMembers copies hash or parts of it into local class
     */
    virtual void prc_com_to_classmembers();

    /*!
       \brief classmembers_to_prc_com copy class members to prc_com - for example when class or threaded
       finishes
     */
    virtual void classmembers_to_prc_com();

    /*!
     * \brief value returns a QVariant
     * \param key key of map
     * \return
     */
    QVariant    value(const QString key) const;


    /*!
     * \brief value returns a integer
     * \param key key of map
     * \return
     */
    int         ivalue(const QString key) const;


    /*!
     * \brief value returns a double
     * \param key key of map
     * \return
     */
    double      dvalue(const QString key) const;


    /*!
     * \brief value returns a QString
     * \param key key of map
     * \return
     */
    QString     svalue(const QString key) const;


    /*!
     * \brief value returns a unsigned int
     * \param key key of map
     * \return
     */
    uint        uivalue(const QString key) const;


    /*!
     * \brief value returns a size_t - avoid if you can
     * \param key key of map
     * \return size_t
     */
    size_t      sztvalue(const QString key) const;


    /*!
     * \brief value returns a int64_t type
     * \param key key of map
     * \return
     */
    qint64      qint64value(const QString key) const;


    /*!
     * \brief value returns a uint64_t
     * \param key key of map
     * \return
     */
    quint64     quint64value(const QString key) const;



    /*!
     * \brief svalue_rformat returns a value as right formatted string such as 002
     * \param key
     * \param width
     * \param fill
     * \return
     */
    QString svalue_rformat(const QString key, const uint width, QChar fill) const;


    /*!
       \brief scan_cmdline  and compare with the map;
       \param
       \param files return a list of files after "-files"
       \param atsfiles return a list of atsfiles: this works without "-files " option
       \param measdocs return a list of XML files automatically if the arg list contains the metronix XML patern", this works without -files " option
     */

    /*!
     * \brief scan_cmdline scans the command line; MUST have -files at the END if you read files
     * \param argues argues from the main (e.g. a.arguments). LIMITATION: file names can NOT start with "-"
     * \param files all files found (but NOT ats and measdoc files from ADU)
     * \param atsfiles all ats files detected - they do NOT need -files at the end of the command line in order to be found
     * \param measdocs all measdoc.xml  detected - they do NOT need -files at the end of the command line in order to be found
     * \param remove_leading_minus -set_x 12 -make_std full will be set into map as <set_x, 12> <make_std, full>
     * \param check_and_replace this is a good TRICK: declare what you want in ADVANCE! So the map can NOT contain unknown <key, value>
     * \param auto_remove removes key value parirs with DBL_MAX and "empty" as value
     */
    void scan_cmdline(const QStringList &argues, QStringList& files, QStringList &atsfiles, QStringList& measdocs, const bool remove_leading_minus = true, const bool check_and_replace = true, const bool auto_remove = false);

    friend prc_com create_prc_com_from_diff(const prc_com &original, const prc_com &changed);
};

/*!
 * \brief create_prc_com_from_diff compare two prc_com and create a new one where values are different
 * \param original
 * \param changed
 * \return
 */
inline prc_com create_prc_com_from_diff(const prc_com &original, const prc_com &changed)
{
    prc_com result;

    auto iter_ob = original.constBegin();
    auto iter_oe = original.constEnd();
    auto iter_cb = changed.constBegin();
    auto iter_ce = changed.constEnd();

    while (iter_cb != iter_ce) {

            while (iter_ob != iter_oe) {
                if (iter_ob.key() == iter_cb.key()) {
                    qDebug() << "+++" << iter_cb.key() << iter_cb.value();
                    qDebug() << "---" << iter_ob.key() << iter_ob.value();

                    QVariant test(get_native(iter_cb.value(), original.value(iter_cb.key())));

                    if (iter_ob.value() != test) {
                        result.insert(iter_cb.key(), test);
                        qDebug() << " --------------------------- changed" << iter_cb.key() << iter_cb.value() << iter_ob.value() << result.value(iter_cb.key());
                    }

                }
                ++iter_ob;
            }
            iter_ob = original.constBegin();
            ++iter_cb;
        }


    return result;

}


//class create_from_diff {

//public:
//    create_from_diff() {}
//    /*!
//     * \brief create_from_diff creates a prc_com (internal QMap) with the differences and tries to set native types of the originals - a table view for example always returns strings!
//     * \param original
//     * \param changed
//     */
//     prc_com diff(const prc_com &original, const prc_com& changed);

//};

#endif // PRC_COM_H
