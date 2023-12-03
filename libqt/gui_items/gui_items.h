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

#ifndef GUI_ITEMS_H
#define GUI_ITEMS_H

#include "gui_items_global.h"
#include <QObject>
#include "gui_items_global.h"
#include <QComboBox>
#include "gui_items_global.h"
#include <QtSql>
#include "gui_items_global.h"
#include <QSqlDatabase>
#include "gui_items_global.h"
#include <QCheckBox>
#include "gui_items_global.h"
#include <QSqlQuery>
#include "gui_items_global.h"
#include <QList>
#include "gui_items_global.h"
#include <QDebug>
#include "gui_items_global.h"
#include <QVariant>
#include "gui_items_global.h"
#include <QFileInfo>
#include "gui_items_global.h"
#include <QApplication>
#include "gui_items_global.h"
#include <QListWidget>
#include "gui_items_global.h"
#include <QSpinBox>
#include "gui_items_global.h"
#include <QLabel>
#include "gui_items_global.h"
#include <QDoubleSpinBox>
#include "gui_items_global.h"
#include <QRadioButton>
#include "gui_items_global.h"
#include <QPushButton>
#include "gui_items_global.h"
#include <QLayout>
#include "gui_items_global.h"
#include <QHBoxLayout>
#include "gui_items_global.h"
#include <QVBoxLayout>
#include "gui_items_global.h"
#include <vector>
#include "gui_items_global.h"
#include <cmath>

#include "gui_items_global.h"
#include <QScrollArea>
#include "gui_items_global.h"
#include <QVBoxLayout>

#include "gui_items_global.h"
#include <QUrl>
#include "gui_items_global.h"
#include <QDesktopServices>

#include "gui_items_global.h"
#include "statmaps.h"
#include "gui_items_global.h"
#include "qtx_templates.h"

#include "gui_items_global.h"
#include <memory>


/*!
 * \brief The checkbox_list class implements a true signal mapper (QSignalMapper signalmapper)
 */
class checkbox_list : public QObject {

    Q_OBJECT
public:
    checkbox_list(QObject *parent = Q_NULLPTR);

    void append(QCheckBox *box);

    std::vector<QCheckBox *> vboxes;
    std::vector<QString> vbox_names;

public slots:

    void slot_re_check();
signals:

    void btn_name_checked(const QString &name);

};

/*!
 * \brief The checkbox_list class implements a true signal mapper (QSignalMapper signalmapper)
 */
class radiobutton_list : public QObject {

    Q_OBJECT
public:
    radiobutton_list(QObject *parent = Q_NULLPTR);

    void append(QRadioButton *radio);

    std::vector<QRadioButton *> vradios;
    std::vector<QString> vradio_names;

public slots:

    void slot_re_check();
signals:

    void btn_name_checked(const QString &name);

};


/*!
 * \brief The frequencies_combo class
 */
class frequencies_combo : public QObject
{
    Q_OBJECT

public:

    /*!
     * \brief frequencies_combo; you may need to protect the first init and a currentIndexChanged with if (this->selectfreq == nullptr) return;
     * \param dbname e.g. info.sql3
     * \param box previously UI created QComboBox to connect with
     */
    frequencies_combo(const QString& dbname, QComboBox *box);


    double current_index_freq(const int &current_index) const;
    QString current_index_name(const int &current_index) const;


    double set_frequency(const double &f);

    double get_frequency() const;



private :

    QComboBox *box = Q_NULLPTR;
    QString dbname;
    std::unique_ptr<QSqlDatabase> db = Q_NULLPTR;
    QList<QString> names;
    QList<QVariant> values;

    double freq = 0;
};

/*!
 * \brief The window length class fetches from the frequency db the window lengths down to 64
 * you may need to protect the first init and a currentIndexChanged with if (this->selectwindow == nullptr) return;
 */
class windowlength_combo : public QObject
{
    Q_OBJECT

public:

    /*!
     * \brief windowlength_combo
     * \param dbname e.g. info.sql3
     * \param box previously UI created QComboBox to connect with
     */
    windowlength_combo(const QString& dbname, QComboBox *box);


    int current_index_wl(const int &current_index) const;
    int set_window_length(const int &wl);


private :

    int lower_limit = 64;
    QComboBox *box = Q_NULLPTR;
    QString dbname;
    std::unique_ptr<QSqlDatabase> db = nullptr;
    QList<QString> names;
    QList<QVariant> values;
};


/*!
 * \brief The prc_com_check_box class is a check box connected to prc_com via signal <b>signal_current_variant_changed</b>
 */
class prc_com_check_box : public QObject {
    Q_OBJECT

public:

    /*!
     * \brief prc_com_check_box constructor should have a parent
     * \param parent e.g.this
     * \param box say Q_NULLPTR (default, the check box will be created)
     * \param prc_com_key to connect with in case
     * \param label_text a check box label
     */
    prc_com_check_box(QWidget *parent,  QCheckBox *box,
                      const QString prc_com_key, const QString label_text);

    /*!
     * \brief add_to_gui adds the button to a layout
     * \param layout
     * \param map_scc inserts <prc_com_key, vector_position> in oreder to use vector[map_scc.value(prc_com_key)]
     */
    void add_to_gui(QBoxLayout *layout, QMap<QString, int> &map_scc, const bool set_checked);


    QCheckBox *box = Q_NULLPTR;

    /*!
     * \brief set_data does not emit, if you need signals, use name->box->setChecked()
     * \param value (0, 1 will be processed)
     */
    void set_data(const QVariant &value);

    QString get_key() const {return this->key;}
    QVariant get_value() const {return this->value;}


signals:

    /*!
     * \brief signal_current_variant_changed
     * \param key e.g. prc_com Qstring
     * \param value value for setting
     */
    void signal_current_variant_changed(const QString &key, const QVariant &value);

private slots:

    void stateChanged(int state);

private :

    QWidget *parent = Q_NULLPTR;
    QString dbname;
    QString key = "";
    bool is_local_checkbox = false;
    QVariant value;



};

/*!
 * \brief The single_column_combo class fetches data from (e.g.) info.sql3 with select * from a table with ONE COLUMN
 */
class single_column_combo : public QObject
{
    Q_OBJECT

public:

    /*!
     * \brief single_column_combo
     * \param dbname e.g. info.sql3, data in these tables are TEXT
     * \param box previously UI created QComboBox to connect with
     */
    single_column_combo(const QString& dbname, const QString &table_name, const QString &double_int_string,
                        QWidget *parent = Q_NULLPTR,  QComboBox *box = Q_NULLPTR,
                        const QString prc_com_key = "", const QString label_text = "", const bool init_database = true);

    void add_to_gui(QBoxLayout *layout, const QString horizontal_vertical, QMap<QString, int> &map_scc);


    ~single_column_combo();

    QVariant current_index_qvariant(const int &current_index) const;
    QVariant set_qvariant_to_current_index(const QVariant &value);

    /*!
     * \brief set_data does NOT emit
     * \param value
     */
    void set_data(const QVariant &value);

    QString get_key() const {return this->key;}
    QVariant get_value() const {return this->value;}

    QComboBox *box = Q_NULLPTR;
    QLabel *label = Q_NULLPTR;

    QVBoxLayout *vtl = Q_NULLPTR;
    QHBoxLayout *hzl = Q_NULLPTR;

signals:

    void signal_current_variant_changed(const QString &key, const QVariant &value);

protected slots:

    void currentIndexChanged(int index);


protected :

    QWidget *parent = Q_NULLPTR;
    QString dbname;
    QString key = "";
    std::unique_ptr<QSqlDatabase> db = nullptr;
    QList<QString> names;
    QList<QVariant> values;
    QVariant value;
    bool is_local_combo = false;

    void init_data_base(const QString& dbname, const QString &table_name, const QString &double_int_string);

};

class wl_combo : public single_column_combo
{

public:
    wl_combo(const QString& dbname, const QString &table_name, const QString &double_int_string,
             QWidget *parent = Q_NULLPTR,  QComboBox *box = Q_NULLPTR,
             const QString prc_com_key = "", const QString label_text = "", const bool init_database = false);

    int lower_limit = 64;

protected:

    void init_data_base(const QString& dbname, const QString &table_name, const QString &double_int_string);

};

/*!
 * \brief The dual_column_combo class the FIRST column of the database contains the display in the combobox like "90%" and the second the value 0.9
 */
class dual_column_combo : public single_column_combo
{

public:
    dual_column_combo(const QString& dbname, const QString &table_name, const QString &double_int_string,
                      QWidget *parent = Q_NULLPTR,  QComboBox *box = Q_NULLPTR,
                      const QString prc_com_key = "", const QString label_text = "", const bool init_database = false);

protected:
    void init_data_base(const QString& dbname, const QString &table_name, const QString &double_int_string);
};

// this->scbox = std::make_unique<sensors_combo>( this->qfi_db.absoluteFilePath(), 'H', this->ui->sensor_box);


class sensors_combo : public QObject
{
    Q_OBJECT

public:
    sensors_combo(const QString& dbname, const QChar E_or_H, QComboBox *box);

    /*!
     * \brief set_atsheader_sensor_name MFS06e (or even MFS-06e) and return MFS-06e the offical name
     * \param inatsheadername MFS06e or MFS06 or MFS07e or even MFS-06e
     * \return offical name: MFS-06e or MFS-07e in this example
     */
    QString set_atsheader_sensor_name(const QString& inatsheadername);

    /*!
     * \brief set_name MFS-06e or MFS-07e the offival name; also wrong MFS06e possibly
     * \param inname  MFS-06e MFS-07e MFS06e and so on
     * \return ats header sensor_name: MFS06e MFS07e MFS06e and so on
     */
    QString set_name(const QString &inname);

    /*!
     * \brief get_name
     * \return official name
     */
    QString get_name() const;

    /*!
     * \brief get_atsheader_sensor_name
     * \return atsheader sensor_name (6 chars only)
     */
    QString get_atsheader_sensor_name() const;

    QString get_current_index_atsheader_sensor_name(const int &current_index) const;

private :

    QComboBox *box = Q_NULLPTR;
    QString dbname;
    std::unique_ptr<QSqlDatabase> db = Q_NULLPTR;
    QList<QString> names;
    QList<QString> atsheadernames;
    QString name;
    QString atsheadername;
    QChar E_or_H = QChar(' ');
};


class message_window : public QListWidget
{
    Q_OBJECT
public:
    message_window();
    virtual ~message_window();



public slots:

    void add_message(const QString& message);
    void add_header(const QString& header_string);
    void add_QMap_dbl(const QMap<QString, double>& qmp);
    void add_statmap(const statmap& smp);
    // void add_QMap_dbl_shared(const QList<std::shared_ptr<QMap<QString, double>>> &qmps);


};

class hz_s_spinbox : public QWidget {
    Q_OBJECT

public:
    hz_s_spinbox( bool is_lowpass, QWidget *parent = Q_NULLPTR);
    ~hz_s_spinbox();

    void set_initValue(const double f);
    void set_f_min_max(const double fmin, const double fmax);


    QSpinBox *fsp = Q_NULLPTR;            //!< frequency spinbox
    QPushButton *Hz_s_btn = Q_NULLPTR;    //!< changes from frequency to period

    double f;                             //!< act frequency

private slots:

    void on_Hz_s_btn_clicked(bool b);

    void on_fsp_valueChanged(int d);


private:

    void toggle_btn(const bool is_hz);


    bool is_hz;
    bool is_lowpass;
    int fmax, fmin, pmax, pmin;

};


class check_boxes_scoll_area : public QScrollArea
{
    Q_OBJECT
public:
    check_boxes_scoll_area(QWidget *parent = Q_NULLPTR);
    virtual ~check_boxes_scoll_area();

    std::vector<QCheckBox *> check_boxes;

    const char* prop_frequency = "Hz";
    const char* prop_count = "counter";

public slots:

    void check_all(const bool checked);

    void add_f_checkbox(const double &frequency);

    void clear();

signals:

    void clicked_freqs(const std::vector<double> &fs);

private slots:

    void clicked_box();


private:

    QVBoxLayout *vl;
};



class push_buttons_scoll_area : public QScrollArea
{
    Q_OBJECT
public:
    push_buttons_scoll_area(QWidget *parent = Q_NULLPTR);
    virtual ~push_buttons_scoll_area();

    std::vector<QPushButton *> push_buttons;

    const char* prop_frequency = "Hz";
    const char* prop_count = "counter";

public slots:


    void add_f_push_button(const double &frequency);

    void clear();

signals:

    void clicked_freqs(const std::vector<double> &fs);

private slots:

    void clicked_button();


private:

    QVBoxLayout *vl;
};



class pmt_open_help : public QObject
{
    Q_OBJECT

public:
    pmt_open_help(QObject *parent = Q_NULLPTR);
    ~pmt_open_help();

public slots:
    void help(const QString which_page_main, const QString which_page_sub, int local_1_both_2_online_3 = 2, const QString lang = "en");
};

#endif // GUI_ITEMS_H
