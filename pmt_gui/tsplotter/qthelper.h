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

#ifndef QTHELPER_H
#define QTHELPER_H

#include <QKeyEvent>
#include <QDir>

namespace qthelper {
    int get_numeric_value_for_number_key(int qtkey);
    bool is_functional_key(int qtkey);

    /**
     * @brief create_survey_subs
     * replacement for pmt::create_survey_subdirs. If the base_dir does not exist it
     * will create the folder/path to it.
     * @param base_dir path to the target directory
     * @return returns true if all directories were created or already existed
     */
    bool create_survey_subs(QDir const & base_dir);
}

#endif // QTHELPER_H
