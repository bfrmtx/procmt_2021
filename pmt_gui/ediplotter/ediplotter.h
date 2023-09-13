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

#ifndef EDIPLOTTER_H
#define EDIPLOTTER_H

#include <QMainWindow>
#include <QDropEvent>
#include <QMimeData>
#include <QDragEnterEvent>

#include <QFileInfo>
#include <QListWidget>
#include <QDockWidget>

#include <qcustomplot.h>

#include <memory>
#include "edi_file.h"
#include "msg_logger.h"
#include "plotoptions.h"
#include "buckets.h"

namespace Ui {
class ediplotter;
}

enum class EdiPlotterOpenMode {
    ADD,
    OVERLAY,
    REPLACE
};

class ediplotter : public QMainWindow
{
    Q_OBJECT

public:
    explicit ediplotter(QWidget *parent = nullptr);
    ~ediplotter();

    /*!
     * \brief set_msg_logger fetch messages from reading EDI files and show on demand
     * \param msg
     */
    void set_msg_logger(std::shared_ptr<msg_logger> msg);

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void open_edi_file(QString const & filename);


signals:
    void new_edi_plot_added(QString const & name);
    void all_edi_files_removed();
    void map_phase(const bool phase_is_mapped);

public slots:
    void slot_set_x_axis_label(const QString& label_x_axis);
    void slot_set_y_axis_label(const QString& label_y_axis);


private slots:
    void phi_button_clicked();
    void rho_button_clicked();

    void on_button_group_open_mode_buttonClicked(QAbstractButton * button);

    void on_action_quit_triggered();
    void on_action_open_triggered();

    void on_button_rho_fit_clicked();
    void on_button_phi_fit_clicked();

    void on_button_actions_toggle_map_phase_clicked(bool checked);

    void on_dial_active_plot_valueChanged(int value);

    void on_action_export_ascii_triggered();

private:
    void set_rho_range(double min, double max);
    void set_phi_range(double min, double max);
    void add_edi_data(const QString & name, mt_data_res<std::complex<double>> const & data);
    void add_coh_plot(QString const & name, mt_data_res<std::complex<double>> const & data);
    void add_anisotropy_plot(QString const & name, mt_data_res<std::complex<double>> const & data);
    void add_tipper_plot(QString const & name, mt_data_res<std::complex<double>> const & data);
    void add_strike_plot(QString const & name, mt_data_res<std::complex<double>> const & data);
    void remove_edi_file(QString const & name);
    void remove_all_edi_files();
    void change_edi_visibility(QString const & name, bool value);

private:
    Ui::ediplotter *ui;

    QMap<QString, edi_file *> m_edi_files;

    std::shared_ptr<msg_logger> msg;

    QDockWidget *qdw;                       //!< show messages on demand
    QListWidget *qlw;                       //!< show messages on demand, strings here

    QHash<QString, QVector<QCPGraph*>> m_graphs;

    bool m_sidemenu_visible = false;

    bool phase_is_mapped = true;

    EdiPlotterOpenMode m_open_mode = EdiPlotterOpenMode::OVERLAY;
};

#endif // EDIPLOTTER_H
