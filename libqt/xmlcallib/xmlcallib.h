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

#ifndef XMLCALLIB_H
#define XMLCALLIB_H

#include "xmlcallib_global.h"
#include <QMainWindow>
#include "xmlcallib_global.h"
#include <QFile>
#include "xmlcallib_global.h"
#include <QFileInfo>
#include "xmlcallib_global.h"
#include <QDrag>
#include "xmlcallib_global.h"
#include <QDropEvent>
#include "xmlcallib_global.h"
#include <QMimeData>
#include "xmlcallib_global.h"
#include <QDragEnterEvent>
#include "xmlcallib_global.h"
#include <QList>
#include "xmlcallib_global.h"
#include <QMap>
#include "xmlcallib_global.h"
#include <QSql>
#include "xmlcallib_global.h"
#include <QDockWidget>
#include "xmlcallib_global.h"
#include <QListWidget>
#include "xmlcallib_global.h"
#include <QVector>

#include "xmlcallib_global.h"
#include <memory>
#include "xmlcallib_global.h"
#include "qcustomplot.h"

#include "xmlcallib_global.h"
#include "procmt_alldefines.h"
#include "xmlcallib_global.h"
#include "qtx_templates.h"

#include "xmlcallib_global.h"
#include "plotlib.h"
#include "xmlcallib_global.h"
#include "measdocxml.h"
#include "xmlcallib_global.h"
#include "calibration.h"
#include "xmlcallib_global.h"
#include "db_sensor_query.h"
#include "xmlcallib_global.h"
#include "select_master_dialog.h"
#include "xmlcallib_global.h"
#include "manual_scales.h"
#include "xmlcallib_global.h"
#include "add_range_labels.h"

#include "xmlcallib_global.h"
#include <QSharedPointer>


//tesinng
#include "xmlcallib_global.h"
#include "sql_vector.h"


namespace Ui {
class xmlcallib;
}

/*!
 * \brief The xmlcallib class is a graphical container for coil calibration data
 */
class xmlcallib : public QMainWindow
{

   Q_OBJECT

public:
    explicit xmlcallib(const QFileInfo &in_info_db, const QFileInfo &in_master_cal_db, QWidget *parent = Q_NULLPTR);

    /*!
     * \brief set_file opens either a XML file or a standard metronix text cal file
     * \param qfx
     */
    void set_file(const QFileInfoList &qfx);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

    void rename_ipl_button(const QString newname);
    void clear(bool caldata_also = false);


    ~xmlcallib();

public slots:

    /*!
     * \brief slot_set_cal set calibration on the fly; MAKE SURE THAT THE MASTERCAL was loaded!
     * \param cal_channel
     * \param nooverlay
     */

    void slot_set_cal(std::shared_ptr<calibration> cal_channel, const bool nooverlay = true);

    /*!
     * \brief slot_set_newfreqs
     * \param ff vector of new (e.g.) FFT frequencies; this automatically interpolates for chopper on AND off
     */
    void slot_set_newfreqs(const std::vector<double> &ff);

private slots:

    void xrange_changed(const QCPRange &range);
    void ayrange_changed(const QCPRange &range);
    void pyrange_changed(const QCPRange &range);

    void rx_cal_message(const int &channel, const int &slot, const QString &message);

    void on_pushButton_rescale_clicked();

    void on_pushButton_Chopper_Off_clicked();

    void on_pushButton_Chopper_On_clicked();

    void on_pushButton_Chopper_Both_clicked();

    void on_pushButton_show_master_cal_clicked();

    void on_pushButton_ipl_clicked();

    void on_pushButton_theo_clicked();

    void on_actionMTX_Format_triggered();

    void on_actionXML_Format_triggered();

    void on_actionCSV_Format_triggered();


    void on_actionExtendFull_triggered();

    void on_checkBox_clicked(bool checked);

    void replot_all();
    void on_checkBox_un_clicked();

    void on_actionload_master_triggered();

    void set_potential_master_cal(const QString &sensor_name);

    void load_master_only();
    void slot_x_y_pos(const double &x, const double &y);


    void on_pushButton_map_to_zero_clicked();

    void on_pushButton_map_to_zero_2_clicked();

    void on_actiondiv_Gain_1_triggered();

    void on_actiondiv_Gain_2_triggered();


    void on_actionManual_Scales_triggered();

    void slot_range_label_updated();



    void xyaxis_clicked(QCPAxis *axis, QCPAxis::SelectablePart part, QMouseEvent *event);

    void on_actionremove_fixed_2x_Gain_triggered();

    void on_actionremove_DIV_8_triggered();

    void on_actionJSON_Format_triggered();

signals:

    void scale_me();

private:
    Ui::xmlcallib *ui;

    //std::unique_ptr<calibration> cal;

    // the emplace_back later into THREADS seems only to accept shared not unique
    std::vector<std::shared_ptr<calibration>> channels;                 //!<  each file or XML entry as treated as channel
    // we use auto plt = ... later - so we need shared ptr for convinience
    std::vector<std::shared_ptr<plotlib>> amplplots;
    std::vector<std::shared_ptr<plotlib>> phaseplots;


    QFileInfo info_db;                                                  //!< set during opening to get sensor types
    QFileInfo master_cal_db;                                            //!< master calibration - e.g. when no calibratrion can be found

    QList<QPushButton *> pblist_ampl;
    QList<QPushButton *> pblist_phs;
    QMap<QPushButton *, QString> pbmap;

    QList<QUrl> urls;

    int add_master_plot = 0;
    int add_theo_plot = 0;
    int add_ipl_plot = 0;

    void pblcear(const QString btn_name);


    void append_plots();




    QDockWidget *qdw;
    QListWidget *qlw;

    select_master_dialog *sel;
    QString sensor_for_master_str;

    manual_scales *manual;
    add_range_labels *rlabels;

    QVector<double>  dnums_labels;
    QVector<QString> strnums_labels;
    int label_calls = 0;




};

#endif // XMLCALLIB_H
