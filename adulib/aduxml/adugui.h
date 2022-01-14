#ifndef ADUGUI_H
#define ADUGUI_H

#include <QMainWindow>
#include <QTableView>
#include <QStandardItemModel>
#include <QUrl>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <memory>
#include "adulib.h"
#include "adutable.h"
#include "adujoblist.h"
#include "stoptime_dialog.h"
#include "num_keypad.h"
#include "db_sensor_query.h"
#include "select_items_from_db_sensors.h"

#include <cstdlib>
#include <cfloat>
#include <climits>
#include <cmath>



namespace Ui {
class mw_adugui;
}

// forward declaration
class pos_button;

class triple_button;

class adugui : public QMainWindow
{
    Q_OBJECT
public:
    explicit adugui(const QFileInfo &sqlfile, QWidget *parent = Q_NULLPTR);

    ~adugui();

    void edit_job(const int &index);
    void edit_job2(const int &index);


    // must be puplic because adujoblist contains QOBJECT

    std::shared_ptr<adujoblist> joblist = nullptr;
    std::shared_ptr<db_sensor_query> sdb = nullptr;
    bool exmode = false; // expert mode active

public slots:

    void on_update_from_xml();
    void on_job_zero_created();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_selectable_frequencies_changed(const std::vector<double> &selectable_frequencies);


    void on_JobTablepushButton_clicked();

    void on_mw_adugui_destroyed();

    void on_lineEdit_site_name_editingFinished();

    void on_lineEdit_site_name_rr_editingFinished();

    void on_lineEdit_site_name_emap_editingFinished();



    void on_set_exmode(bool set_true_false);


private :


    std::unique_ptr<adutable> single_jobtable = nullptr;
    std::unique_ptr<adutable> ssingle_jobtable = nullptr;

    QTableView *jobtimetable = nullptr;

    QFileInfo sqlfile;

    Ui::mw_adugui *ui;


    std::unique_ptr<num_keypad> keyp = nullptr;

    QList<pos_button*> posbtns;

    QList<triple_button*> chcfgbtns;

    QFileInfo qfi;

    qint64 grid_time;              //!< prefer a 64s raster for RR




};

class pos_button : public QObject
{
    Q_OBJECT
public:
    pos_button(QBoxLayout *layout, QString label_text, QString xml_name, int channel_no, bool needs_flip, adugui *gui);

    adugui *gui;
    bool needs_flip;
    int channel_no;
    QString xml_name;
    QString label_text;
    QBoxLayout *layout;

    QPushButton *btn;
    QLabel *lbl;

    std::unique_ptr<num_keypad> keyp = nullptr;
    QVariant value;


public slots:

    void on_clicked();
    void on_update_from_xml();


};

class triple_button : public QObject {
    Q_OBJECT

public:
    triple_button(QBoxLayout *layout, const QString channel_type, const QString sensor_type, const QString sensor_sernum, const int channel_no, std::shared_ptr<db_sensor_query> sdb, adugui *gui);

    adugui *gui;
    std::shared_ptr<db_sensor_query> sdb = nullptr;
    int channel_no;
    QString sensor_sernum;
    QString sensor_type;
    QString channel_type;
    QBoxLayout *layout;
    std::unique_ptr<select_items_from_db_sensors> sdb_select = nullptr;
    QPushButton *channel_type_btn;
    QPushButton *sensor_type_btn;
    QPushButton *sensor_sernum_btn;
    std::unique_ptr<num_keypad> keyp = nullptr;



    int get_channel_no() const;

public slots:

    void on_change_channel_type();
    void on_change_sensor_type();
    void on_change_sensor_sernum();
    void on_update_from_xml();

private:
    bool locked = true;

};


class mini_dialog : public QDialog
{
    Q_OBJECT

public:
    mini_dialog (const QString &text, QWidget *parent = Q_NULLPTR);
};


#endif // ADUGUI_H
