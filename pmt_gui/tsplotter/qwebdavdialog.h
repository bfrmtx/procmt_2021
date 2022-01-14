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

#ifndef QWEBDAVDIALOG_H
#define QWEBDAVDIALOG_H

#include "ui_qwebdavdialog.h"
#include "tsnetworkhandler.h"

#include <array>
#include <iostream>
#include <QIcon>
#include <QMessageBox>
#include <QFileDialog>
#include <QStack>

struct QWebDavDialogItemData {
    QWebDavDialogItemData() = default;
    QWebDavDialogItemData(QString uri, webdav::Response data);
    ~QWebDavDialogItemData() = default;
    QWebDavDialogItemData(const QWebDavDialogItemData &) = default;
    QWebDavDialogItemData &operator=(const QWebDavDialogItemData &) = default;

    QString uri;
    webdav::Response data;
};

class QWebDavDialog : public QDialog, private Ui::QWebDavDialog {
    Q_OBJECT

signals:
    void files_selected(QStringList const & URIs, bool complete_files);

public:
    ATSNetworkManager * networkManager() { return m_network_manager; }
    explicit QWebDavDialog(QString const & host_address, QWidget *parent = nullptr);
    void show();

private:
    void timer_triggered();
    void requestFolder(QString const & uri, QTreeWidgetItem * current = nullptr);
    void requestHeader(QString const & uri);
    void downloadFiles(QString const & target, QStringList const & URIs);
    void close_and_select(QStringList const & URIs);
    void enableButtons(bool value);
    const QStringList currentURIs();

private slots:
    void on_button_goto_clicked();
    void on_button_cancel_clicked();
    void on_button_select_clicked();
    void on_edit_url_textEdited(QString const & text);
    void timer_timeout();
    void on_tree_view_itemClicked(QTreeWidgetItem *item, int column);

    void on_button_download_clicked();

private:
    ATSNetworkManager * m_network_manager;
    uint64_t m_pending_requests = 0;
    uint64_t m_requested_bytes = 0;
    QStack<QString> m_links;
    QString m_current_uri;
    QString m_root;
    int m_edit_time = 0;
    QTimer * m_edit_timer;
};

Q_DECLARE_METATYPE(QWebDavDialogItemData*)

#endif // QWEBDAVDIALOG_H
