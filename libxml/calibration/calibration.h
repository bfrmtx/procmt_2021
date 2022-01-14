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

#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QUrl>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QMap>


#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>


#include <memory>
#include <vector>
#include <complex>
#include <algorithm>    // std::transform
#include <functional>
#include "procmt_alldefines.h"

#include "measdocxml.h"
#include "sql_base_tools.h"
#include "db_sensor_query.h"
#include "tinyxmlwriter.h"
#include "math_vector.h"



// todo make 05 coil

// for a sythetic frequency list f can be generated
//f =  ( double(i) * (f_sample/fwl) );
inline void gen_trf_mfs06e_chopper_on_template(const double &f, std::complex<double> &p1, std::complex<double> &p2,
                                               std::complex<double> &p4, const std::complex<double> &im, std::complex<double> &trf) {
    p1 = (f / 4.) * im;         // 45 phase at 4 Hz
    p2 = (f / 8192.) * im;
    p4 = (f / 28300.0) * im;
    trf = 0.8 * (  (p1 / ( 1. + p1)) * (1. / (1. + p2)) * (1. / (1. + p4))  );
}

// always on
inline void gen_trf_mfs09u_chopper_on_template(const double &f, std::complex<double> &p1, std::complex<double> &p2,
                                               const std::complex<double> &im, std::complex<double> &trf) {
    p1 = (f / 2.5) * im;  // 45 phase at 2.8 Hz
    p2 = (f / 100.) * im;
    trf = 0.25 * (  (p1 / ( 1. + p1)) * (1. / (1. + p2)) );
}



inline void gen_trf_mfs06e_chopper_off_template(const double &f, std::complex<double> &p1, std::complex<double> &p2, std::complex<double> &p3,
                                                std::complex<double> &p4, const std::complex<double> &im, std::complex<double> &trf) {
    p1 = (f / 4.) * im;
    p2 = (f / 8192.) * im;
    p3 = (f / 0.72) * im;
    p4 = (f / 25000.0) * im;
    trf = 0.8 * (  (p1 / ( 1. + p1)) * (1. / (1. + p2)) * (p3 / (1. + p3)) * (1. / (1. + p4)) );
}

inline void gen_trf_mfs07e_chopper_on_template(const double &f, std::complex<double> &p1, std::complex<double> &p2,
                                               std::complex<double> &p4, const std::complex<double> &im, std::complex<double> &trf) {
    p1 = (f / 32.) * im;
    p2 = (f / 40000.) * im;
    p4 = (f / 50000.0) * im;
    trf = 0.64 * (  (p1 / ( 1. + p1)) * (1. / (1. + p2))  * (1. / (1. + p4)) );
}

inline void gen_trf_mfs07e_chopper_off_template(const double &f, std::complex<double> &p1, std::complex<double> &p2, std::complex<double> &p3,
                                                std::complex<double> &p4, const std::complex<double> &im, std::complex<double> &trf) {
    p1 = (f / 32.) * im;
    p2 = (f / 40000.) * im;
    p3 = (f / 0.72) * im;
    p4 = (f / 50000.0) * im;
    trf = 0.64 * (  (p1 / ( 1. + p1)) * (1. / (1. + p2)) * (p3 / (1. + p3))  * (1. / (1. + p4)) );
}

inline void gen_trf_mfs07_chopper_on_template(const double &f, std::complex<double> &p1, std::complex<double> &p2,
                                              std::complex<double> &p4, const std::complex<double> &im, std::complex<double> &trf) {
    p1 = (f / 32.) * im;
    p2 = (f / 45000.) * im;
    p4 = (f / 28300.0) * im;
    trf = 1. * 0.64 * (  (p1 / ( 1. + p1)) * (1. / (1. + p2))  * (1. / (1. + p4)) );
}

inline void gen_trf_mfs07_chopper_off_template(const double &f, std::complex<double> &p1, std::complex<double> &p2, std::complex<double> &p3,
                                               std::complex<double> &p4, const std::complex<double> &im, std::complex<double> &trf) {
    p1 = (f / 32.0) * im;
    p2 = (f / 45000.) * im;
    p3 = (f / 0.720) * im;
    p4 = (f / 28300.0) * im;
    trf = 0.64 * (  (p1 / ( 1. + p1)) * (1. / (1. + p2)) * (p3 / (1. + p3))  * (1. / (1. + p4)) );
}

// all fluxgates do not have a chopper
// Raklin Geomag-01
inline void gen_trf_fgs02_template(std::complex<double> &trf) {

    trf = std::complex<double>(7.5000E-04, 0.0);
}

// bartington mag-03 old and new version as fgs-03/4(e), low noise, 100 000 nT DEFAULT !

inline void gen_trf_fgs100nt_template(std::complex<double> &trf) {

    trf = std::complex<double>(1.0000E-04, 0.0);
}

// bartington mag-03 new version as fgs-05e, low noise, 70 000 nT, never sold yet

inline void gen_trf_fgs70nt_template(std::complex<double> &trf) {

    trf = std::complex<double>(1.4300E-04, 0.0);
}




/*!
 * \brief gen_trf_adb_08e_hf_xcalib calibration for using with external measurments where gains can appear
 * \param f
 * \param p1
 * \param p2
 * \param p3
 * \param p4
 * \param high_pass
 * \param gain_1
 * \param im
 * \param trf
 */
inline void gen_trf_adb_08e_hf_xcalib(const double &f, std::complex<double> &p1, std::complex<double> &p2, std::complex<double> &p3,
                                      std::complex<double> &p4, const int &hp_filter,
                                      const double &gain_1, const std::complex<double> &im, std::complex<double> &trf) {
    p1 = (f / 338.0E3) * im;
    p2 = ((f * gain_1) / 100.0E6) * im;
    p3 = (f / 1.59E6) *im;

    if (hp_filter == cal::adu08e_hp500hz_on) {
        p4 = (f / 482.0) * im;
        trf = gain_1 *  (1. / (1. + p1)) *  (1. / (1. + p2)) * (1. / (1. + p3)) *  (p4 / ( 1. + p4));
    }
    else if (hp_filter == cal::adu08e_hp1hz_on) {
        p4 = (f / 1.0) * im;
        trf = gain_1 *  (1. / (1. + p1)) *  (1. / (1. + p2)) * (1. / (1. + p3)) *  (p4 / ( 1. + p4)) ;
    }
    else {
        trf = gain_1 *  (1. / (1. + p1)) *  (1. / (1. + p2)) * (1. / (1. + p3));
    }
}

/*!
 * \brief gen_trf_adb_08e_hf generate transfer function for the HF board to be folded with the sensor
 * \param f
 * \param p1
 * \param p2
 * \param p3
 * \param p4
 * \param high_pass
 * \param im
 * \param trf
 */
inline void gen_trf_adb_08e_hf(const double &f, std::complex<double> &p1, std::complex<double> &p2, std::complex<double> &p3,
                               std::complex<double> &p4, const int &hp_filter,
                               const std::complex<double> &im, std::complex<double> &trf) {
    p1 = (f / 338.0E3) * im;
    p2 = (f / 100.0E6) * im;
    p3 = (f / 1.59E6) *im;

    if (hp_filter == cal::adu08e_hp500hz_on) {
        p4 = (f / 482.0) * im;
        trf = (1. / (1. + p1)) *  (1. / (1. + p2)) * (1. / (1. + p3)) *  (p4 / ( 1. + p4)) ;
    }
    else if (hp_filter == cal::adu08e_hp1hz_on) {
        p4 = (f / 1.0) * im;
        trf = (1. / (1. + p1)) *  (1. / (1. + p2)) * (1. / (1. + p3)) *  (p4 / ( 1. + p4)) ;
    }
    else {
        trf = (1. / (1. + p1)) *  (1. / (1. + p2)) * (1. / (1. + p3));
    }
}

/*!
 * \brief gen_trf_adb_07e_hf generate transfer function for the HF board to be folded with the sensor
 * \param f
 * \param p1
 * \param p2
 * \param p3
 * \param high_pass
 * \param gain_1
 * \param gain_2
 * \param im
 * \param trf
 */
inline void gen_trf_adb_07e_hf(const double &f, std::complex<double> &p1, std::complex<double> &p2, std::complex<double> &p3,
                               const int &hp_filter, const double &gain_1, const double &gain_2,
                               const std::complex<double> &im, std::complex<double> &trf) {
    p1 = (f / 7.7E6) * im;
    p2 = (f / 7.7E6) * im;

    std::complex<double> f_1 = std::complex<double>(1.0, 0.0);
    std::complex<double> f_2 = std::complex<double>(1.0, 0.0);
    std::complex<double> f_3 = std::complex<double>(1.0, 0.0);

    if (gain_1 > 1.1) {
        f_1 = (1. / (1. + p1));
    }
    if (gain_2 > 1.1) {
        f_1 = (1. / (1. + p2));
    }

    if (hp_filter == cal::adu07e_hp1hz_on) {
        p3 = (f / 1.0) * im;
        f_3 = (p3 / ( 1. + p3)) ;
    }

    trf = f_1 * f_2 * f_3;
}



/*!
 * \brief gen_trf_adb_08e_lf_xcalib  calibration for using with external measurments where gains can appear
 * \param f
 * \param p1
 * \param p2
 * \param p3
 * \param p4
 * \param gain_1
 * \param gain_2
 * \param resistance
 * \param input_div
 * \param radio_filter
 * \param lp_filter
 * \param im
 * \param trf
 */
inline void gen_trf_adb_08e_lf_xcalib(const double &f, std::complex<double> &p1, std::complex<double> &p2, std::complex<double> &p3, std::complex<double> &p4,
                                      const double &gain_1, const double &gain_2, const double &resistance,
                                      const int &input_div, const int &radio_filter, const int &lp_filter,
                                      const std::complex<double> &im, std::complex<double> &trf) {
    p1 = (f / 318.0E3) * im;
    p2 = ((f * gain_2) / 2.0E6) * im;



    if      ((radio_filter == cal::adu08e_rf_1) && (input_div == cal::div_8)) p4 = (f / 30.0E3) * im;
    else if ((radio_filter == cal::adu08e_rf_2) && (input_div == cal::div_8)) p4 = (f / 10.5E3) * im;
    else if ((radio_filter == cal::adu08e_rf_1) && (input_div == cal::div_1)) p4 = (f / ( 0.159 / ((resistance + 200.) * 470.0E-12) ) ) * im;
    else if ((radio_filter == cal::adu08e_rf_2) && (input_div == cal::div_1)) p4 = (f / ( 0.159 / ((resistance + 200.) * 7.27E-9) ) ) * im;



    // the fixed gain of 2  is invisible in the time series an calibrated in the LSB
    trf = gain_1 * gain_2 * (1. / (1. + p1)) *  (1. / (1. + p2)) *  (1. / ( 1. + p4));

    //    }
    if (lp_filter == cal::adu08e_lp4hz_on) {
        p3 = (f / 4.) * im;
        trf *= (1. / (1. + 1.41421356237 * p3 + (p3 * p3) ));

    }
}

/*!
 * \brief gen_trf_adb_08e_lf generate transfer function for the LF board to be folded with the sensor
 * \param f
 * \param p1
 * \param p2
 * \param p3
 * \param p4
 * \param resistance
 * \param input_div
 * \param radio_filter
 * \param lp_filter
 * \param im
 * \param trf
 */
inline void gen_trf_adb_08e_lf(const double &f, std::complex<double> &p1, std::complex<double> &p2, std::complex<double> &p3, std::complex<double> &p4,
                               const double &resistance,
                               const int &input_div, const int &radio_filter, const int &lp_filter,
                               const std::complex<double> &im, std::complex<double> &trf) {
    p1 = (f / 318.0E3) * im;
    p2 = (f / 2.0E6) * im;

    trf = (1. / (1. + p1)) *  (1. / (1. + p2)) *  (1. / ( 1. + p4));


    if      ((radio_filter == cal::adu08e_rf_1) && (input_div == cal::div_8)) p4 = (f / 30.0E3) * im;
    else if ((radio_filter == cal::adu08e_rf_2) && (input_div == cal::div_8)) p4 = (f / 10.5E3) * im;
    else if ((radio_filter == cal::adu08e_rf_1) && (input_div == cal::div_1)) p4 = (f / ( 0.159 / ((resistance + 200.) * 470.0E-12) ) ) * im;
    else if ((radio_filter == cal::adu08e_rf_2) && (input_div == cal::div_1)) p4 = (f / ( 0.159 / ((resistance + 200.) * 7.27E-9) ) ) * im;

    if (lp_filter == cal::adu08e_lp4hz_on) {
        p3 = (f / 4.) * im;
        trf *= (1. / (1. + 1.41421356237 * p3 + (p3 * p3) ));

    }
}


/*!
 * \brief gen_trf_adb_07e_lf generate transfer function for the LF board to be folded with the sensor
 * \param f
 * \param p1
 * \param p2
 * \param p3
 * \param p4
 * \param resistance
 * \param input_div
 * \param radio_filter
 * \param lp_filter
 * \param im
 * \param trf
 */
inline void gen_trf_adb_07e_lf(const double &f, std::complex<double> &p1, std::complex<double> &p2, std::complex<double> &p3, std::complex<double> &p4,
                               const double &resistance,
                               const int &input_div, const int &radio_filter, const int &lp_filter,
                               const std::complex<double> &im, std::complex<double> &trf) {
    p1 = (f / 4.0E3) * im;
    p2 = (f / 21.2E3) * im;

    trf = (1. / (1. + p2)) *  (1. / (1. + p2));

    //!< @todo RADIO FILTER missing
    // Radio Filter not respected yet
    //    if      ((radio_filter == cal::adu07e_rf_1) && (input_div == cal::div_8)) p4 = (f / 30.0E3) * im;
    //    else if ((radio_filter == cal::adu07e_rf_2) && (input_div == cal::div_8)) p4 = (f / 10.5E3) * im;
    //    else if ((radio_filter == cal::adu07e_rf_1) && (input_div == cal::div_1)) p4 = (f / ( 0.159 / ((resistance + 200.) * 470.0E-12) ) ) * im;
    //    else if ((radio_filter == cal::adu07e_rf_2) && (input_div == cal::div_1)) p4 = (f / ( 0.159 / ((resistance + 200.) * 7.27E-9) ) ) * im;

    if (lp_filter == cal::adu07e_lp4hz_on) {
        p3 = (f / 4.) * im;
        trf *= (1. / (1. + 1.41421356237 * p3 + (p3 * p3) ));

    }
}




inline void gen_trf_adb_07e_mf(const double &f, const double &sample_freq, std::complex<double> &p1, std::complex<double> &p2, std::complex<double> &p3, std::complex<double> &p4,
                               const double &resistance, const int &input_div,
                               const int &radio_filter, const int &lp_filter, const int &hp_filter,
                               const std::complex<double> &im, std::complex<double> &trf) {
    if (sample_freq > (65536 - 1) )      p1 = (f / 48.1E3) * im;
    else if (sample_freq > (16384 - 1) ) p1 = (f / 15.9E3) * im;
    else if (sample_freq > (4096 - 1) )  p1 = (f / 3.7E3)  * im;
    else if (sample_freq > (128 - 1) )   p1 = (f / 159.)   * im;



    trf = (1. / (1. + p1));

    //!< @todo RADIO FILTER missing
    // Radio Filter not respected yet
    //    if      ((radio_filter == cal::adu07e_rf_1) && (input_div == cal::div_8)) p4 = (f / 30.0E3) * im;
    //    else if ((radio_filter == cal::adu07e_rf_2) && (input_div == cal::div_8)) p4 = (f / 10.5E3) * im;
    //    else if ((radio_filter == cal::adu07e_rf_1) && (input_div == cal::div_1)) p4 = (f / ( 0.159 / ((resistance + 200.) * 470.0E-12) ) ) * im;
    //    else if ((radio_filter == cal::adu07e_rf_2) && (input_div == cal::div_1)) p4 = (f / ( 0.159 / ((resistance + 200.) * 7.27E-9) ) ) * im;

    if (lp_filter == cal::adu07e_lp4hz_on) {
        p3 = (f / 4.) * im;
        trf *= (1. / (1. + 1.41421356237 * p3 + (p3 * p3) ));

    }
    if (hp_filter == cal::adu07e_hp500hz_on) {
        p4 = (f / 482.0) * im;
        trf *= (p4 / ( 1. + p4)) ;
    }


}




// should reach E = 50mV / nT at 10 kHz
inline void gen_trf_shft02e_template(const double &f, std::complex<double> &p1,  const std::complex<double> &im, std::complex<double> &trf) {

    p1 = (f / 3.0E5) * im;
    trf = 1. * 0.05 * (1./ (1. + p1));
}

inline void gen_trf_adb_07e_1Hz(const double &f, std::complex<double> &p1, const std::complex<double> &im, std::complex<double> &trf) {
    // 1 Hz HP from ADB board
    // im should be  std::complex<double> im(0.0,1.0);


    //    Michael´s ADU hat einen 1 Hz Hochpass - ganz normal.
    //    Der hat einen Einfluss bei Frequenzen kleiner 10 Hz. Besonders in dewr
    //    Phase, bei kleinen Frequenzen auch in der Amplitude. Es handelt sich
    //    hier um ein 1 poliges Filter. Die Formel ist P/(1-P) mit P=j*f/1Hz,
    //    also hier P=j*f.
    //    Die Absenkung der Amplitude ist bei 1Hz 0.707, die Phase dann +45°.
    //    Der Phaseneinfluss bei 10 Hz beträgt dann 90°-atan(10Hz/1Hz)= +5,71°.
    //    Natürlich dreht auch die Spule die Phase. Dies wird aber bei richtig
    //    angewendeter Kalibrierfunktion berücksichtigt. Er muss natürlich die
    //    für Chopper off verwenden.


    p1 = (f * im);
    trf = (p1 / ( 1. + p1));
}

/*!
 * \brief The plt enum defines what to plot in case; some values are repeated from calibration / plot repository - for independency
 */



/*!
 * \brief The calibration class
 *
 *   f_on.size() after a complete and successful initialisation
 *   chopper_f is != 0 and != -1 (fail) ... like 512 1024
 *   the int chooper variale is a switch - not and indicator that the coil has a chopper<br>
 *   THIS CLASS CAN NOT BE COPIED! (no copy constructor AND class contain a database object), use<br>
 *   std::shared_ptr<calibration> myptr = std::make_shared<calibration>(other->get_dbname_info(), other->get_channel_no(), this)); <br>
 *   myptr->clone_caldata(other); <br>
 *   we use TEXT only because double is cut off sqlite or SQLITE interface
 */

class calibration : public db_sensor_query
{
    Q_OBJECT

public:



    /*!
     * \brief calibration conatins all about the calibration
     * \param dbname_info see  \ref db_sensor_query::db_sensor_query
     * \param channo \ref db_sensor_query::db_sensor_query
     * \param open_db \ref db_sensor_query::db_sensor_query
     * \param connection_name \ref db_sensor_query::db_sensor_query
     * \param parent  in case NOT Q_NULLPTR parent MUST HAVE a SLOT  rx_cal_message(const int &channel, const QString &message)
     */
    calibration(const QFileInfo &dbname_info,  const int channo = -1, const int slot = -1,
                const bool open_db = true, const QString &connection_name = "db_sensor_query",
                QObject *parent = Q_NULLPTR, std::shared_ptr<msg_logger> msg = nullptr);

    ~calibration();

    void clone_caldata(std::shared_ptr<calibration> rhs);


    /*!
     * \brief read_std_txt_file reads a standard metronix text calibration file;
     *  will check serial number and calibration date; coiltype from filename
     * \param qfi
     * \return chopper on + chopper of size; 0 if fails
     */
    size_t read_std_txt_file(const int &channel, const QFileInfo &qfi);


    bool operator == (const calibration& rhs);


    void open_master_cal(const QFileInfo &master_calibration_db_name, const QString &connection_name_master = "master_calibration");


    bool open_create_db(const QString &db_name, const QDir &directory, const QString &connection_name = "all_sensor_db");

    bool prepare_cal_db();

    bool prepare_info_table();

    /*!
     * \brief insert_into_cal_db inserts actual chopper on / off into the database with 15 digits prcisions
     * \return
     */
    int insert_into_cal_db();

    int coil_exist(const bool insert = false);

    void create_type_and_date(const QString &sensortype, const int &sernum, const QDateTime &caltime );
    void set_f_a_p_fromVector_chopper_on(const std::vector<double> &f_on, const std::vector<double> &ampl_on,const std::vector<double> &phase_grad_on);
    void set_f_a_p_fromVector_chopper_off(const std::vector<double> &f_off, const std::vector<double> &ampl_off,const std::vector<double> &phase_grad_off);



    /*!
     * \brief select_coil selects a coil FROM `cal mfs-06e` WHERE ; sensortype will be correct from MFS06e or MFS-06e
     * \param sensortype mfs-06e mfs06e and so on
     * \param sernum existing sernum
     * \param b_get_cal
     * \return
     */
    bool select_coil(const QString &sensortype, const int &sernum, bool b_get_cal = true);

    QString search_coil_dir(const QDir where, const QString &sensortype, const int &sernum,
                            const int ichan = -1, bool b_get_cal = true );

    /*!
     * \brief get_master_cal sets this master to master from SQL
     * \param no_cal_found true: throw a warning that no calibration was found an we use the master cal instead
     * \return size of f
     */


    size_t get_master_cal(const bool no_cal_found = false);

    void set_chopper(const cal::cal chop);

    cal::cal get_chopper() const;

    void master_cal_to_caldata();


    QMultiMap<QString, int> all_sensors_in_db();


    /*!
     * \brief write_std_txt_file creatinit_calibration a text file in metronix format; you MUST have called \ref select_coil first
     * \param qfi qfileinfor for output; precision is 6 digits
     * \return size of f_on and f_off
     */
    size_t write_std_txt_file(const QFileInfo &qfi);

    /*!
     * \brief write_csv_table data a serial, chopper, f, a, p, stddev a, stddev p ; stddev may be empty
     * \param qfi
     * \return lines written
     */
    size_t write_csv_table(const QFileInfo &qfi) const;

    QString create_xlmstr() const;


    size_t write_xml(const QFileInfo &qfi) const;

    size_t write_json(const QFileInfo &qfi);

    QByteArray toJsonByteArray(QJsonDocument::JsonFormat format = QJsonDocument::Indented, cal::cal chopper = cal::cal::chopper_undef);

    size_t fromQBytearray(const QByteArray &json_bytes);

    size_t from_JSON(const int channel, const QFileInfo *qfi = nullptr, const QByteArray *qba = nullptr);

    void iterate_json(const QJsonObject &qjd);

    void init_calinfo();

    void update_calinfo();



    /*!
     * \brief get_has_chopper_f returns the frequency <= chopper should be on, so 512, 256 is on if 512 returned; 0 == no chopper, -1 invalid/false/fail
     * \return
     */
    double get_has_chopper_f();


    QMap<QString, QString> get_sensor_types_tables() const;


    /*!
     * \brief sensors_summary fills the selectable sensors string list wich contains the lower case names of all sensors in the database
     * \return
     */
    int sensors_summary();

    int f_index(const std::vector<double> &vf, const double freq) const;
    /*!
     * \brief gen_theo_cal generates a theoretical tranfer function
     * \param fin_f frequencies in
     * \param dad   delta amplitude as uncertainy - that is mostly less than 3% so 0.03, IF 0 DELTA WILL NOT BE GENERATED
     * \param dpg   delta phase as uncertainy - that should be 0.5 or 1 deg, IF 0 DELTA WILL NOT BE GENERATED
     * \param limit_fcal_high cut at highest frequency (e.g.cal or master cal) that is f_on of f_off; for MT limit_fcal_high is TRUE
     * \return size of generated transfer function (on or off)
     */
    size_t gen_theo_cal(const std::vector<double> &fin_f, const double dad = 0.0, const double dpg = 0.0, const bool limit_fcal_high = false);

    /*!
     * \brief gen_adb_pre_cal this calibration INCLUDES extra gains and div which are hidden in the ATS files; use for Magnetsrode
     * \param fin_f
     * \return
     */
    size_t gen_adb_pre_cal(const std::vector<double> &fin_f, const double dad = 0.0, const double dpg = 0.0, const bool limit_fcal_high = false);

    /*!
     * \brief theo_cal_to_active in case theo shall be used (forced) the gen_tho_cal will be copied to f, cap
     * \return
     */
    size_t theo_cal_to_active();


    /*!
     * \brief gen_cal_freqs generates a logarithmic equidistant vect
     * \param start
     * \param stop
     * \param steps_per_decade
     * \param channel
     * \return
     */
    std::vector<double> gen_cal_freqs(const double start, const double stop, const int steps_per_decade);


    /*!
     * \brief get_selectable_sensors get the result from calibration::sensors_summary
     * \return
     */
    QStringList get_selectable_sensors() const;

    // fetch data without object functions - used for base data copy
    int sernum = 0;                                                                 //!< sensor sernum as int
    QString sensortype;
    cal::cal chopper = cal::chopper_undef;                                          //!< 1 = chopper on, 0 = chopper off, -1 chopper undefined

    QDateTime caltime;
    //!< sensortype as used in all docs: MFS-06e, not mfs06e, not alias
    std::vector<double> freqs;                                                      //!< contains e.g. the FFT frequencies

    std::vector<double> f_on, phase_grad_on, ampl_on;                               //!< data from the xml or txt file: the input calibration for chopper on
    std::vector<double> f_off, phase_grad_off, ampl_off;                            //!< data from the xml or txt file: the input calibration for chopper off
    // if calculated
    std::vector<double> phase_grad_on_stddev, ampl_on_stddev;
    std::vector<double> phase_grad_off_stddev, ampl_off_stddev;

    std::vector<double> phase_grad_on_master_stddev, ampl_on_master_stddev;
    std::vector<double> phase_grad_off_master_stddev, ampl_off_master_stddev;

    std::vector<double> f_on_theo, phase_grad_on_theo, ampl_on_theo ;               //!< data from the theoretical: theo function for chopper on
    std::vector<double> f_off_theo, phase_grad_off_theo, ampl_off_theo;             //!< data from the theoretical: theo function for chopper on
    std::vector<double> phase_grad_on_theo_stddev, ampl_on_theo_stddev;             //!< data from the theoretical: theo function for chopper on with added error stddev
    std::vector<double> phase_grad_off_theo_stddev, ampl_off_theo_stddev;           //!< data from the theoretical: theo function for chopper off wirh added error stddev

    std::vector<double> f_on_master, phase_grad_on_master, ampl_on_master ;         //!< data from the SQL database: master function for chopper on
    std::vector<double> f_off_master, phase_grad_off_master, ampl_off_master;       //!< data from the SQL database: master function for chopper on

    std::vector<double> f_on_ipl, phase_grad_on_ipl, ampl_on_ipl;                   //!< interploated data chopper on  - MUST have same size as freqs - otherwise useless
    std::vector<double> f_off_ipl, phase_grad_off_ipl, ampl_off_ipl;                //!< interploated data chopper off - MUST have same size as freqs - otherwise useless
    // during processing one ats file is either on or off

    ////////// FINAL FINAL //////
    std::vector<double> f;                                                          //!< final used; uses chopper you have asked for
    std::vector<std::complex<double>> cap;                                          //!< final used complex amplitude and phase; uses chopper you have asked for;

    QFileInfo input_filename;
    /*!
     * \brief sort_lower_to_upper_check_duplicates after reading (especially txt file make sure that they are sorted and have no duplicates
     * \param what calibration, master, interpolated, chopper: on, chopper of
     * \return size of survied frequencies
     */
    size_t sort_lower_to_upper_check_duplicates(const cal::cal &what, const cal::cal &chop);

    QString get_adb_name() const;

    void set_linear();                                                              //!< place holder for empty cal, to be replaced by other

    bool fold(const std::shared_ptr<calibration> &other_cal);


public slots:

    void read_measdoc(const int &channel, const cal::cal &chopper, const QFileInfo &qfi);

    /*!
     * \brief interploate interpolates the existing data onto the newfreqs
     * \param newfreqs frequencies as new x-axis
     * \param channel channel number
     * \param chopper on of off - which part of the calibration is to be interploated
     * \param overlap   overlap first and last value omto the theroretical values - smooth transition
     * \param extend_theo true if you want to go outside existing + master cal
     * \param limit_upper_to_cal - in most cases the UPPER theoretical does not fit so well; the lower MUST fit!
     * \return size of new calibration (on or off)
     */
    size_t interploate(const std::vector<double> &newfreqs, const int channel = -1, const cal::cal chopper = cal::chopper_undef,
                       const bool overlap = true, const bool extend_theo = true, const bool limit_upper_to_cal = true);

    QString fetch_single_item_from_sensortypes_table(const QString Name, const QString col_name);

    void eliminate_except_complex();


    /*!
     * \brief prepend_dc_part_f0_to_complex_final for a prepend if a DC part is needed, expensive! also prepends f[0] = 0.0
     * \param real_trf_at_0 real part of DC cap
     * \param imag_trf_at_0 imaginary part of cap
     */
    void prepend_dc_part_f0_to_complex_final(const double real_trf_at_0 = 1.0, const double imag_trf_at_0 = 0.0);

    /*!
     * \brief slot_gen_pure_theo_cal
     * \param psrc rerereimimim input from fftreal
     * \param f_sample sample freq
     * \param start_idx start index of fft ; will be incremented iff DC fft so we start min fft[1] here to avoid 0Hz DC;
     * \param stop_idx stop index of fft; stop index - so start and stop are a sub index for cut_upper and cut_lower
     * \param dc_ampl set the amplitude for DC e.g. zro for a coil or the const value of fluxgate
     * \param dc_phase_deg set DC phase; I set DC phase 0 for the coil that eliminates the DC for 0Hz DC part
     * \param mul_by should be 1000 to get mV what is needed for processing; say 1 for the mtx std calibration for other usage
     * \param mul_by_f say true for the scaled mtx format; say false if you want to use the result for processing
     * \param invert say true - the transfer finction is inverted and psrc is MULTIPLIED; old style was devide so I invert and multiply
     */
    void slot_gen_pure_theo_cal(std::vector<double> &psrc, const double &f_sample, const size_t &start_idx, const size_t &stop_idx,
                                const double &dc_ampl, const double &dc_phase_deg, const double &mul_by, const bool &mul_by_f, const bool &invert) const;


    void slot_set_chopper(const int c_0_off_1_on);

    /*!
     * \brief map_to_zero maps phase from pzx to 0 deg
     * \param pzx - phase from to zero
     */
    void map_to_zero(const double pzx = 359.);

    void div_gain_1();

    void div_gain_2();

    void mult_input_div();

    void remove_fixed_gain();

    void remove_div_8();

    /*!
     * \brief parse_filename_other_cal tries to understand other calibrations like TRF_ADU-08e_ADB08e-LF_DIV-1_Gain1-4_DAC-off_Gain2-1_LF-RF-2_LF-LP-off_HF-HP-off_SensorResistance-250.txt
     * \param qfi
     * \return
     */
    bool parse_filename_other_cal(const QFileInfo &qfi);

    /*!
     * \brief set_sample_freq this is needed for ADB board calibration - we want to skip certain low frequencies where we are absolute linear
     * \param sample_freq
     */
    void set_sample_freq(const double& sample_freq);


signals:

    void sensor_serial(const int &channel, const QString &sensor, const int &serial, const int &type);

    void cal_open_success(const int &channel, const size_t &n_chopper_on, const size_t &n_chopper_off);

    void cal_data(const int &channel, const int &chopper,
                  const std::vector<double> &f, const std::vector<double> &ampl, const std::vector<double> &phase_grad);

    void complex_cal_data(const int &channel, const cal::cal &chopper,
                          const std::vector<double> &f, std::vector<std::complex<double>> &cap);

    void master_cal(const int &channel, const cal::cal &chopper,
                    const std::vector<double> &f, const std::vector<double> &ampl, const std::vector<double> &phase_grad);

    void item_fetched_from_sensortypes_table(const QString &Name, const QString &col_name, const QString &result);




private:

    bool parse_filename(const QFileInfo &qfi);

    /*!
     * \brief parse_filename_other_cal can be called from text file, where the file name is stored; called "alone" does not set the filename, but the cal settings
     * \param qfi
     * \return
     */

    bool ukn_sensor = false;         //!< try to read unknown files
    size_t get_data();

    void clear();

    int get_cal();

    QString sensor_table;
    double chopper_f;

    int input_div = cal::div_1;            //!< for ADB measurements where the DIV from connector IF board is not inside the LSB; is 1 or 8
    double gain_1 = 1.0;                   //!< for ADB measurements where the gain is not inside the LSB 1, 4, 16
    double gain_2 = 1.0;                   //!< for ADB measurements where the gain is not inside the LSB 1, 4, 16
    bool dac_on = false;                   //!< DAC on or off;
    bool direct_mode = false;

    int adu08e_hp         = cal::adu08e_hp_off;  //!< ADU-08e High Pass 500Hz off
    int adu08e_rf_1_2     = cal::adu08e_rf_2;    //!< ADU-08e 0: RF-1 off, RF-2 off, 1: RF-1 on, RF-2 off, 2:  RF-1 off, RF-2 on
    int adu08e_lp         = cal::adu08e_lp_off;  //!< ADU-08e Low Pass 4Hz off

    int adu07e_hp         = cal::adu07e_hp_off;  //!< ADU-07e High Pass 500Hz off
    int adu07e_rf_1_2_3_4 = cal::adu07e_rf_2;    //!< ADU-07e 0: RF-1 off, RF-2 off, 1: RF-1 on, RF-2 off, 2:  RF-1 off, RF-2 on
    int adu07e_lp         = cal::adu07e_lp_off;  //!< ADU-07e Low Pass 4Hz off


    double sample_freq = 0.0;                    //!< for the boards we want decide for which sample frequencies ABOVE we want to apply the corrections
    double sensor_resistance = 250.;             //!< e.g. contact resistance of electrode

    bool is_linear_cal = false;                  //! for swapping an E field which has no cal with other - like ADB


    QMap<QString, QString> sensor_types_tables;

    std::unique_ptr<measdocxml> measdoc;                                            //!< measdoc - contains e.g. multiple calibrations

    math_vector mathv;


    QStringList selectable_sensors;

    QSqlDatabase sensor_db;
    QString sensor_db_connection_name;
    QSqlDatabase master_calibration_db;
    QString master_calibration_db_connection_name;


    std::unique_ptr<sql_base_tools> sqlbt;


    QMap<QString, QVariant> calinfo;                                                //!< calinfo from XML file or JSON

    QStringList csv_header;                                                         //!<  "serial" << "chopper" << "f" << "a" << "p" << "stddev a" << "stddev p"


    /*!
     * \brief zerof in case frequency is zero, replace it
     * \param ampl
     * \param phase
     * \param ampl_0
     * \param double_phase_0
     */

    inline void zerof(std::vector<double> &ampl, std::vector<double> &phase, const double &ampl_0, const double &phase_0);

    // SELECT printf("%i %E",idx, vals) FROM real where idx = 1;


    // ------------------- JSON Interface   ------------
    std::unique_ptr<QJsonObject> root_obj;
    std::unique_ptr<QJsonDocument> json_doc;
    bool read_json_chopper_on = false;  // else off or general (chopper unknown)






};

/*!
 * \brief calibration comperator - as a template
 */
template <typename T> struct comp_calser_lt
{
public:
    comp_calser_lt() { }
    bool operator()(const calibration& lhs, const calibration rhs) const
    {            return ( lhs.sernum < rhs.sernum);

    }
};



#endif // CALIBRATION_H
