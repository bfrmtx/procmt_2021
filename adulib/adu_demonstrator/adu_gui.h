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

#ifndef ADU_GUI_H
#define ADU_GUI_H

#include <QMainWindow>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QDrag>
#include <QDropEvent>
#include <QMimeData>
#include <QSql>
#include <QUrl>

#include <QSqlDatabase>
#include <QSqlQuery>

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QButtonGroup>
#include <QRadioButton>
#include <QTableView>
#include <QTableWidget>

#include "adu_system_defs.h"
#include "adu_json.h"
#include <memory>
#include <vector>
#include <climits>
#include <cfloat>

#include "recording.h"


QT_BEGIN_NAMESPACE
namespace Ui { class adu_gui; }
QT_END_NAMESPACE

class adu_gui : public QMainWindow
{
    Q_OBJECT

public:
    adu_gui(QWidget *parent = nullptr);
    ~adu_gui();

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private slots:

    void on_pb_create_bare_bone_clicked();

    void on_pb_save_JSON_clicked();
    void on_pb_load_JSON_clicked();

    void on_pb_load_XML_clicked();
    void on_pb_save_XML_clicked();



    void update_adu_values(const uint& varmap, const uint& channel, const QString& key, QVariant& value);

    void update_adu_double_values(const double& value_dbl);
    void update_adu_string_values(const QString& value_str);
    void update_adu_string_values_from_idx(const int& index);

    void update_adu_int_values(const int& value_int);


    void channel_type_changed(const bool& checked);


    void clear_gui();

    void assemble_gui(const bool is_barebone);

    void get_doubles_from_adu();
    void get_strings_edit_from_adu();


    void get_ints_from_adu();




private:
    Ui::adu_gui *ui;

    std::unique_ptr<adu_json> adu;

    /*!
     * \brief create_double_spinbox creates atswriter++pos_x1++1  so QVariantMap_name++key++channel - that will be split later again
     * \param channel 1
     * \param QVariantMap_name like atswriter
     * \param key pos_x1
     * \param parent
     */
    void create_double_spinbox(const adu_defs::adu_defs varmap, const uint& channel, const QString& key, QWidget *parent = nullptr);

    void create_sensor_combo(const adu_defs::adu_defs varmap, const uint& channel, const QString& key, QWidget *parent = nullptr);
    void create_line_edit_channel(const adu_defs::adu_defs varmap, const uint& channel, const QString& key, QWidget *parent = nullptr);


    QVariant get_label(const adu_defs::adu_defs varmap, const QString& key);
    void get_sensor_types();


    std::vector<QDoubleSpinBox*> dbl_spbxs;
    std::vector<QLabel*> dbl_spbxs_labels;

    std::vector<QSpinBox*> spbxs;
    std::vector<QLabel*>   spbxs_labels;

    std::vector<QComboBox*> cbboxs;
    std::vector<QLabel*> cbboxs_labels;
    std::vector<QLineEdit*> lineetds;

    std::vector<QRadioButton*> rb_click_E;  //!< having a bare boe you want to set the channel type
    std::vector<QRadioButton*> rb_click_H;   //!< having a bare boe you want to set the channel type

    void set_default_ranges(const adu_defs::adu_defs varmap, const QString& token_2, double min, double max);

    adu_row_struct adu_row;

    std::unique_ptr<QSqlDatabase> db;
    std::unique_ptr<QSqlQuery> query;

    void create_bare_bone(uint chans);
    void gen_sensor_pos();
    void gen_sensor_types();
    void gen_str_results();

    QStringList E_sensors;
    QStringList H_sensors;
    QStringList H_sensors_atsheader;
    QStringList E_sensors_atsheader;

    QStringList E_channel_types;
    QStringList H_channel_types;

    QList<std::unique_ptr<adu_json>> jobs;

    void assemble_from_existing(const bool is_barebone = false);

    std::unique_ptr<recording> rec;



};
#endif // ADU_GUI_H
