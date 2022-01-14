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

#ifndef MT_GENSIGN_H
#define MT_GENSIGN_H

#include <QDebug>
#include <QFileInfo>
#include <QList>
#include <QString>
#include <QMap>
#include <QListWidget>
#include <QComboBox>
#include <QMainWindow>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QSignalMapper>
#include <QAbstractSpinBox>
#include <QWidget>
#include <memory>
#include <mutex>
#include <vector>
#include <QStringList>
#include <thread>
#include <queue>


#include "atsheader.h"
#include "atsfile.h"
#include "mc_data.h"
#include "eqdatetime.h"

#include "adulib.h"
#include "adutable.h"
#include "adujoblist.h"

#include "gui_items.h"
#include "calibration.h"

#include <iostream>
#include <random>

class mt_gensign;

class generator : public QObject
{
    Q_OBJECT
public:
    generator(const size_t &slices, const size_t &slice, mt_gensign *master);

    void set_threadbuffer(std::shared_ptr<threadbuffer<double> > &buffer);
    virtual ~generator();

    std::vector<double> v;
    std::vector<double> v2;
    size_t slices;
    std::shared_ptr<threadbuffer<double>> buffer;

    // that is coming as input
    std::shared_ptr<threadbuffer<double>> unique_noise_buffer_1;
    std::shared_ptr<threadbuffer<double>> unique_noise_buffer_2;

    int noise_1_status;
    int noise_2_status;

    void linear();

    void sawtooth();

    void rect();

    void mt19937noise();

    void mt_noise_coils();

    void seed();

    void sine();


    double linear_mV = 100.0;

    double lower = -50.0;
    double upper = 50.0;

    double f_sample = 1024.0;



    QString function_name = "straight_100_mV";

    mt_gensign *master;

    size_t channel_type = SIZE_MAX;

    int channel_no = 99;


public slots:

    void slot_set_function(const QString &function_name);

    void slot_set_slices(const int slices);
    void slot_set_slice_wl(const int slice);

    void slot_set_sines(const double &f_sample, const std::vector<double> &fsine_f, std::vector<double> &fsine_a, std::vector<double> &fsine_p);

private:
    std::vector<double> coil_mixer;
    std::vector<double> fgs_mixer;
    std::vector<double> fsine_f;
    std::vector<double> fsine_a;
    std::vector<double> fsine_p;


};


namespace Ui {
class mt_gensign;
}

class mt_gensign : public QMainWindow
{
    Q_OBJECT

public:
    explicit mt_gensign(QWidget *parent = Q_NULLPTR);
    ~mt_gensign();


private slots:


    void on_start_datetime_dateTimeChanged(const QDateTime &dateTime);


    void on_select_sample_feq_currentIndexChanged(int index);


    void sensor_changed(const int);

    void on_run_pushButton_clicked();

    void on_slices_valueChanged(int arg1);

    void on_select_slice_length_currentIndexChanged(int index);

    void on_fsine_f1_valueChanged(double arg1);

    void on_fsine_a1_valueChanged(double arg1);

    void on_fsine_p1_valueChanged(double arg1);

    void on_fsine_f2_valueChanged(double arg1);

    void on_fsine_a2_valueChanged(double arg1);

    void on_fsine_p2_valueChanged(double arg1);

    void on_fsine_f3_valueChanged(double arg1);

    void on_fsine_a3_valueChanged(double arg1);

    void on_fsine_p3_valueChanged(double arg1);

    void on_fsine_f4_valueChanged(double arg1);

    void on_fsine_a4_valueChanged(double arg1);

    void on_fsine_p4_valueChanged(double arg1);

    void on_checkBox_use_seconds_stateChanged(int arg1);

private:

    void clear();

    void seed_two_noise_src();

    void create_unique_seed(bool createme = true);

    void create_xml();

    Ui::mt_gensign *ui;

    QFileInfo qfi_db;


    checkbox_list *check_list_waves;

    QList<QPushButton *> qpbl;
    QMap<QString, QDoubleSpinBox *> qdspins;

    QString outdir;                           //!< ouptud data

    //QSignalMapper *mapper;
    //#include "boost/generator_iterator.hpp"



    std::unique_ptr<frequencies_combo>  selectfreq = nullptr;
    //std::unique_ptr<windowlength_combo> selectwindow = nullptr;
    std::unique_ptr<single_column_combo> selectwindow = nullptr;

    std::vector<std::shared_ptr<atsfile>> atsfiles;


    std::vector<std::shared_ptr<generator>> generators;

    std::vector<std::shared_ptr<threadbuffer<double>> > buffers;

    std::vector<std::thread>   writerthreads;
    std::vector<std::thread>   generatorthreads;

    std::vector<std::thread> uni_seed_threads;

    std::unique_ptr<adulib> adujob;


    size_t wl = 1024;
    size_t slices = 64;
    double f_sample = 1024.0;
    bool is_init = false;

    const size_t channels = 5;

    // generate uniform noise for all channels
    std::vector<double> noise_1;
    std::vector<double> noise_2;
    std::vector<std::shared_ptr<threadbuffer<double>>> unique_noise_buffer_1;
    std::vector<std::shared_ptr<threadbuffer<double>>> unique_noise_buffer_2;
    std::vector<std::vector<double>> noise_copy_1;
    std::vector<std::vector<double>> noise_copy_2;

    std::vector<double> fsine_f;
    std::vector<double> fsine_a;
    std::vector<double> fsine_p;


    QString basedir;
    QString HWConfig;
    QString HwDatabase;
    QString HwStatus;
    QString ChannelConfig;
    QString outputjobsdir;

    QFileInfo *qfiHWConfig = nullptr;
    QUrl *qrlHWConfig = nullptr;
    QByteArray *qbaHWConfig = nullptr;

    QFileInfo *qfiHwDatabase = nullptr;
    QUrl *qrlHwDatabase = nullptr;
    QByteArray *qbaHwDatabase = nullptr;


    QFileInfo *qfiChannelConfig = nullptr;
    QUrl *qrlChannelConfig = nullptr;
    QByteArray *qbaChannelConfig = nullptr;

    QFileInfo *qfiHwStatus;

    eQDateTime eqdt_start;

    QList<QComboBox *> ch_combos;
    std::vector<std::unique_ptr<sensors_combo>> scboxes;

    QStringList cht;
    QDir setsm;
    QString deldir_string;



};


#endif // MT_GENSIGN_H
