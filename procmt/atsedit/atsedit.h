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

#ifndef ATSEDIT_H
#define ATSEDIT_H

#include <QMainWindow>
#include <QTableView>
#include <QVariant>
#include <QDebug>
#include <QPushButton>
#include <QRadioButton>
#include <QList>
#include <QUrl>
#include <QFileInfo>
#include <QDrag>
#include <QDir>
#include <QDirIterator>
#include <QDropEvent>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QProgressBar>
#include <QButtonGroup>
#include <QDateTimeEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>

#include <memory>
#include <QDesktopServices>
#include <QUrl>

#include "prc_com.h"
#include "prc_com_table.h"
#include "atsheader.h"
#include "atsfile.h"
#include "qtx_templates.h"
#include "geocoordinates.h"
#include "gui_items.h"
#include "overwrite_dialog.h"

#include "measdocxml.h"


namespace Ui {
class atsedit;
}

class calcfilesize;



class atsedit : public QMainWindow
{
    Q_OBJECT

public:
    explicit atsedit(const QString& dbname, QWidget *parent = Q_NULLPTR);
    ~atsedit();

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

    void open_urls(const QList<QUrl> urls, const QString otherwise_absolute_path);

    Ui::atsedit *ui;


private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_doubleSpinBox_x2_valueChanged(double arg1);

    void on_doubleSpinBox_y1_valueChanged(double arg1);

    void on_doubleSpinBox_y2_valueChanged(double arg1);

    void on_doubleSpinBox_x1_valueChanged(double arg1);

    void on_doubleSpinBox_z1_valueChanged(double arg1);

    void on_doubleSpinBox_z2_valueChanged(double arg1);

    void on_comboBox_sensortype_editTextChanged(const QString &arg1);

    void on_lineEdit_sernum_textChanged(const QString &arg1);

    void on_lineEdit_xml_header_textChanged(const QString &arg1);

    void on_lineEdit_time_stamp_textChanged(const QString &arg1);

    void on_lineEdit_num_samples_textChanged(const QString &arg1);

    void on_dateTimeEdit_starts_dateTimeChanged(const QDateTime &dateTime);

    void on_spinBox_lat_deg_valueChanged(int arg1);

    void on_spinBox_lat_min_valueChanged(int arg1);

    void on_doubleSpinBox_lat_sec_valueChanged(double arg1);

    void on_spinBox_lon_deg_valueChanged(int arg1);

    void on_spinBox_lon_min_valueChanged(int arg1);

    void on_doubleSpinBox_lon_sec_valueChanged(double arg1);

    void on_doubleSpinBox_northing_valueChanged(double arg1);

    void on_spinBox_lon_msecs_valueChanged(int arg1);

    void on_doubleSpinBox_elev_m_valueChanged(double arg1);

    void on_doubleSpinBox_easting_valueChanged(double arg1);

    void on_spinBox_lat_msecs_valueChanged(int arg1);

    void on_comboBox_NS_currentIndexChanged(const QString &arg1);

    void on_comboBox_EW_currentIndexChanged(const QString &arg1);


    void on_selfreq_currentIndexChanged(int index);

    void on_spinBox_channels_valueChanged(int arg1);

    void on_dateTimeEdit_startsat_dateTimeChanged(const QDateTime &dateTime);

    void on_dateTimeEdit_stopsat_dateTimeChanged(const QDateTime &dateTime);

    void on_comboBox_sensortype_currentIndexChanged(const QString &arg1);

    void on_pushButton_openstreetmap_clicked();

    void on_pushButton_google_maps_clicked();

private:

    QString dbname;

    prc_com_table *all = nullptr;
    QTableView *jobtimetable = nullptr;

    QFileInfo qfi;
    std::unique_ptr<atsheader> atsh = nullptr;

    std::unique_ptr<prc_com_table> site_table = nullptr;
    QStringList site_table_includes;


    std::unique_ptr<prc_com_table> system_table = nullptr;
    QStringList system_table_excludes;

    QStringList excludes;

    std::unique_ptr<prc_com>  sens_pos_ser = nullptr;
    QFileInfo qfi_measdoc;

    eQDateTime starts;
    eQDateTime stops;

    bool time_updt = true;

    QMap<QString, QVariant> tmp_header;



    geocoordinates coord;

    void update_gui_vars();

    void update_coord_gui();

    void recalc_coordinates();

    /////////////////////   coordinates   /////////////////////////////////////////

    bool coo_update  = true;
    bool b_from_deg  = false;
    bool b_from_mesc = false;
    bool b_from_utm  = false;


    long double latitude = 0;      /*! latitude  in RAD   this is the base of all */
    long double longitude = 0;     /*! longitude in RAD   this is the base of all */
    long double northing = 0;      /*! UTM coordianate */
    long double easting = 0;       /*! UTM coordinates */
    long double elevation = 0;     /*! elevation sea (NN) in meter - eg. WGS84 */
    int msec_lon = 0;              /*! gps delivery */
    int msec_lat = 0;              /*! gsp delivery */

    int deg_lo = 0;                /*! readable coodinates, grad */
    int mi_lo = 0;                 /*! readable coodinates, grad */
    int deg_la = 0;                /*! readable coodinates, grad */
    int mi_la = 0;                 /*! readable coodinates, grad */

    long double se_lo = 0;         /*! readable seconds longitude DEG */
    long double se_la = 0;         /*! readable seconds longitude DEG */


    int utm_meridian = 0;          /*! meridian for UTM   in DEG */

    QString UTMzone;           /*! UTM zone */
    QString  LatH;             /*! N or S designator */
    QString  LonH;             /*! E or W designator */

    /////////////////////                   /////////////////////////////////////////

    std::unique_ptr<calcfilesize> calcf = nullptr;

    std::unique_ptr<sensors_combo> allsensors = nullptr;
    overwrite_dialog *ovr;

    std::unique_ptr<measdocxml> measdoc = nullptr;



};

class calcfilesize : public QObject
{
    Q_OBJECT
public:
    calcfilesize(const QString& dbname, QComboBox *box, atsedit* gui);

    quint64 calc_filesizes();

public slots:

    void start_time_changed(const QDateTime& startime);
    void stop_time_changed(const QDateTime& stoptime);
    void channels_value_changed(const int nchan);
    void frequency_index_changed(const int index);



private:

    QComboBox *box;
    std::unique_ptr<frequencies_combo>  selectfreq = nullptr;

    quint64 headersize = 1024;
    quint64 headersize_cea = 33760;
    quint64 headersize_used = 1024;
    quint64 sample_size = 4;

    quint64 total_size;
    quint64 samplesecs = 0;

    quint64 nchannels = 1;

    qint64 starts = 0;
    qint64 stops = 0;

    double freq = 0;

    atsedit *gui;

};

#endif // ATSEDIT_H
