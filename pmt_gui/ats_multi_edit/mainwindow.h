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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QVariant>
#include <QDebug>
#include <QPushButton>
#include <QRadioButton>
#include <QList>
#include <QUrl>
#include <QFileInfo>
#include <QDialog>
#include <QMessageBox>
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

#include "prc_com.h"
#include "prc_com_table.h"
#include "atsheader.h"
#include "atsfile.h"
#include "qtx_templates.h"
#include "geocoordinates.h"
#include "gui_items.h"

#include "measdocxml.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString& dbname, QWidget *parent = nullptr);
    ~MainWindow();

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

    void open_urls(const QList<QUrl> urls);


private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_pushButton_accept_clicked();

private:
    Ui::MainWindow *ui;

    QString dbname;

    void clean_up(const bool setWT = true);

    void set_table();

    bool items_checked();

    std::unique_ptr<prc_com_table> system_table = nullptr;

    QFileInfo qfi_xml;
    std::unique_ptr<measdocxml> measdoc = nullptr;


    QList<QFileInfo> qfis;
    std::vector<std::unique_ptr<atsheader>> atshs;

    prc_com master_data;
    prc_com diff_data;
    prc_com changed_data;
    QMap<QString, QVariant> tmp_header;
    QStringList excludes;

    bool can_exec = false;
    bool can_proceed = false;



};

#endif // MAINWINDOW_H
