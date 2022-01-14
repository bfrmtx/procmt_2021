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

#ifndef DIR_DIALOG_H
#define DIR_DIALOG_H

#include <QDialog>
#include <QTreeView>
#include <QHeaderView>
#include <QFileInfo>
#include <QModelIndex>
#include <QFileSystemModel>
#include <QDir>
#include <QPushButton>

namespace Ui {
class dir_dialog;
}

class dir_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit dir_dialog(QWidget *parent, const QString &absolute_path, int target);
    ~dir_dialog();


signals:

    void selected_dir(const QString &absolute_path, const int &target);

private slots:
    void on_dir_treeView_clicked(const QModelIndex &index);

private:
    Ui::dir_dialog *ui;
    QFileSystemModel *qfsi  = Q_NULLPTR;
    QDir dir;
    int target = -1;
};

#endif // DIR_DIALOG_H
