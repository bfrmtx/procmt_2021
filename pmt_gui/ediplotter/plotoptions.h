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

#ifndef PLOTOPTIONS_H
#define PLOTOPTIONS_H

#include <QCheckBox>
#include <QPushButton>
#include <QSpacerItem>

#include <QDialog>
#include <QMap>

namespace Ui {
class PlotOptions;
}

class PlotOptions : public QWidget
{
    Q_OBJECT

public:
    explicit PlotOptions(QWidget *parent = nullptr);
    ~PlotOptions();

private:
    void remove_entry(QString const & name);

public slots:
    void slot_on_new_edi_plot(QString name);
    void slot_on_all_edi_plots_removed();

private:
    Ui::PlotOptions *ui;
    QMap<QString, std::pair<QCheckBox*, QPushButton*>> m_content;
    QSpacerItem * m_spacer = nullptr;

signals:
    void remove_plot(QString const & name);
    void plot_visiblity_change(QString const & name, bool value);
};

#endif // PLOTOPTIONS_H
