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

#ifndef MSG_LOGGER_H
#define MSG_LOGGER_H

#include <QDebug>
#include <QVariant>
#include <QString>
#include <QStringList>
#include <QObject>
#include <queue>
#include <iostream>
#include <mutex>




/*!
 * \brief The msg_logger class makes a 0,1,2,3 column log of messages.
 * It can be displayed on a command line or send to a tableWidget
 * msg_logger uses a mutex and can be accessed from several threads
 * Since this is expensive in time, think of what you want to log
 */
class msg_logger : public QObject
{
    Q_OBJECT

public:
    msg_logger(QObject *parent = Q_NULLPTR);

    /*!
     * \brief show_and_empty displays all and empties
     */
    void show_and_empty();

    /*!
     * \brief size size of the msg_logger
     * \return number of lines (each with 4 qvariant) == table rows
     */
    int size() ;

    /*!
     * \brief get_items
     * \return a single sline (4 items) of the msg_logger
     */
    QStringList get_items();

    /*!
     * \brief get_all_as_single_lines
     * \return all fields as one line, until empty
     */
    QStringList get_all_as_single_lines();

    /*!
     * \brief isEmpty Qt style test if the container is empty
     * \return true if is empty
     */
    bool isEmpty();

    /*!
     * \brief clear clears / empties the msg_logger
     */
    void clear();


public slots:

    void parzen_radius_changed(const double &old_prz, const double &prz);

    /*!
     * \brief rx_cal_message simplifed interface for calibration messages
     * \param channel which channel of ADU was used
     * \param slot thread queue slot
     * \param message message
     */
    void rx_cal_message(const int &channel, const int &slot, const QString &message);

    /*!
     * \brief rx_adu_msg simplifed interface for the ADU
     * \param system_serial_number
     * \param channel which channel of ADU was used
     * \param slot thread queue slot
     * \param message message
     */
    void rx_adu_msg(const int &system_serial_number,  const int &channel, const int &slot, const QString &message);


    void slot_general_msg(const QString &sender, const QString &component, const QString &message);

    void slot_general_msg_nums(const QString &sender_and_msg, const QVariant num_1, const QString &num_seperator, const QVariant num_2);


    void slot_general_msg_4strs(const QString &sender_and_msg, const QString &field2, const QString &field3, const QString &field4);


private:


    std::queue<QVariant> c0;        //!< e.g. we expect the sender's name here
    std::queue<QVariant> c1;        //!< for MT data we expect the channel 0 ...9 here
    std::queue<QVariant> c2;        //!< e.g. the thread queue number
    std::queue<QVariant> c3;        //!< e.g. the message or value to be displayed


    void add_message(const QVariant &qc0, const QVariant &qc1, const QVariant &qc2, const QVariant &qc3);


    std::mutex lock;

};

#endif // MSG_LOGGER_H
