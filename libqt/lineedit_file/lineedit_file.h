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

#ifndef LINEEDIT_FILE_H
#define LINEEDIT_FILE_H

#include "lineedit_file_global.h"
#include <QLineEdit>
#include "lineedit_file_global.h"
#include <QDrag>
#include "lineedit_file_global.h"
#include <QDropEvent>
#include "lineedit_file_global.h"
#include <QMimeData>
#include "lineedit_file_global.h"
#include <QFileInfo>
#include "lineedit_file_global.h"
#include <QUrl>
#include "lineedit_file_global.h"
#include <QDir>
#include "lineedit_file_global.h"
#include <QFile>
#include "lineedit_file_global.h"
#include <QFileInfo>
#include "lineedit_file_global.h"
#include <QMimeType>
#include "lineedit_file_global.h"
#include <QString>
#include "lineedit_file_global.h"
#include <QStringList>



class lineedit_file : public QLineEdit, public QFileInfo
{

    Q_OBJECT
public:
    /*!
     * \brief qlineedit_file MUST use set_file or set_dir later
     * \param what 0, 1, 2 for basename, filename and absolute filepath
     * \param parent
     */
    lineedit_file(QWidget *parent = Q_NULLPTR, const int what = 2);

    /*!
     * \brief qlineedit_file constructor for a file or directory
     * \param qfi
     * \param
     * \param what file_0_dir_1 sets: 0 file, 1 dir
     * \param parent
     */
    lineedit_file(const QFileInfo qfi, const int file_0_dir_1 = 0, const int what = 0, QWidget *parent = Q_NULLPTR);

    /*!
     * \brief qlineedit_file
     * \param dir
     * \param what
     * \param parent
     */
    lineedit_file(const QDir dir, const int what = 0,QWidget *parent = Q_NULLPTR);


    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void set_what(const int base0_filename1_absolute2);

    void set_dir(const QString str, const int what = 0);
    void set_dir(const QDir dir, const int what = 0);

    void set_file(const QString str, const int what = 0);
    void set_file(const QFileInfo qfi, const int what = 0);


    void set_check_subdir(const QString slash_subdir);
    void set_toplevel_dir(const QString &toplevel_dir);
    void set_toplevel_dir(const QFileInfo &qfi_toplevel_dir);

signals:
   void valid_file(const bool &ok);
   void valid_dir(const bool &ok);
   void valid_topdir(const bool &ok);

public slots:

    void lineedit_changed(const QString &str);
    void clear();


private:

   void set_text();

   int what = 0;       //!<  0 basename, 1 filename, 2 absloute

   QString slash_subdir;

   QFileInfo qfi_topdir; //!< limit to top

   bool clear_me = false;
   bool reset_me = false;

};

#endif // LINEEDIT_FILE_H
