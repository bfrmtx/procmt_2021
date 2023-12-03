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

#ifndef PHOENIX_JSON_H
#define PHOENIX_JSON_H

#include "phoenix_json_global.h"
#include <QObject>
#include "phoenix_json_global.h"
#include <QJsonDocument>
#include "phoenix_json_global.h"
#include <QJsonArray>
#include "phoenix_json_global.h"
#include <QJsonObject>
#include "phoenix_json_global.h"
#include <QJsonValue>
#include "phoenix_json_global.h"
#include <QByteArray>
#include "phoenix_json_global.h"
#include <QDebug>
#include "phoenix_json_global.h"
#include <QFile>
#include "phoenix_json_global.h"
#include <QFileInfo>
#include "phoenix_json_global.h"
#include <QTextStream>
#include "phoenix_json_global.h"
#include <QByteArray>


#include "phoenix_json_global.h"
#include <QMap>
#include "phoenix_json_global.h"
#include <QVariant>

#include "phoenix_json_global.h"
#include <atsfile.h>
#include "phoenix_json_global.h"
#include <vector>
#include "phoenix_json_global.h"
#include <thread>
#include "phoenix_json_global.h"
#include <future>
#include "phoenix_json_global.h"
#include <mutex>
#include "phoenix_json_global.h"
#include <condition_variable>
#include "phoenix_json_global.h"
#include <chrono>

#include "phoenix_json_global.h"
#include <calibration.h>


class Phoenix_json : public QObject
{
    Q_OBJECT
public:
    Phoenix_json(QObject *parent = Q_NULLPTR);


    bool from_JSON_scal(const QFileInfo *qfi = nullptr);
    bool from_JSON_rxcal(const QFileInfo *qfi = nullptr);


    /*!
     * \brief ts_header_reader reads the header of a Phoenix timeseries json file; HENCE that startim will be converted extra from string because JSON assumes double
     * \param qfi
     * \return
     */
    bool ts_header_reader(const QFileInfo &qfi, int &nerrors);
    bool ts_block_reader(const QFileInfo &qfi, std::vector<std::shared_ptr<atsfile>> &atsfiles,
                         const QMap<size_t, QString> &chan_str_mtx, const QMap<size_t, QString> &chan_str_phoenix);


    QMap<QString, QVariant> phoenix_tags;

    QMap<QString, QVariant> phoenix_results;

    int save_calfiles();

    /*!
     * \brief iterate_json recursive JSON parser
     * \param qjd
     */
    void iterate_json(const QJsonObject &qjd);

    std::vector<std::shared_ptr<calibration>> calfiles;

public slots:

    /*!
     * \brief slot_sensor_changed
     * \param sensortype like MFS-06e
     */
    void slot_sensortype_changed(const QString& sensortype);



signals:

    void signal_lines_processed(QString nlines_proc);

    void signal_sensor_changed(const QString& sensortype, const int& sernum);

private:

    QMap<size_t, QString> chan_str_mtx;
    QMap<size_t, QString> chan_str_phoenix;

    QFileInfo info_db;                                                  //!< set during opening to get sensor types
    QFileInfo myfile;

    void special_values_coils(QJsonObject::const_iterator &first);
    void special_values_system_ts(QJsonObject::const_iterator &first);
    void special_values_system(QJsonObject::const_iterator &first);
    QStringList active_channel_str;



    int what = 0;      // 1 ts, 2 coil, 3 system
    int nerrors = 0;



    std::vector<std::vector<double>> freqs_coil;
    std::vector<std::vector<double>> magnitude_coil;
    std::vector<std::vector<double>> phs_deg_coil;

    std::vector<std::vector<std::vector<double>>> freqs_system;
    std::vector<std::vector<std::vector<double>>> magnitude_system;
    std::vector<std::vector<std::vector<double>>> phs_deg_system;


    std::vector<std::future<bool>> async_runs;
    std::vector<size_t> async_has_lines;

    std::vector<std::future<size_t>> async_runs_lsb;
    std::vector<std::future<QDataStream::Status>> async_runs_file_write;



};




#endif // PHOENIX_JSON_H

/*
 * // concurrent-queue.h
#ifndef CONCURRENT_QUEUE_H_
#define CONCURRENT_QUEUE_H_

#include "phoenix_json_global.h"
#include <queue>
#include "phoenix_json_global.h"
#include <thread>
#include "phoenix_json_global.h"
#include <mutex>
#include "phoenix_json_global.h"
#include <condition_variable>

template <typename T>
class ConcurrentQueue {
 public:
  T pop() {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty()) {
      cond_.wait(mlock);
    }
    auto val = queue_.front();
    queue_.pop();
    mlock.unlock();
    cond_.notify_one();
    return val;
  }

  void pop(T& item) {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty()) {
      cond_.wait(mlock);
    }
    item = queue_.front();
    queue_.pop();
    mlock.unlock();
    cond_.notify_one();
  }

  void push(const T& item) {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.size() >= BUFFER_SIZE) {
       cond_.wait(mlock);
    }
    queue_.push(item);
    mlock.unlock();
    cond_.notify_one();
  }
  ConcurrentQueue()=default;
  ConcurrentQueue(const ConcurrentQueue&) = delete;            // disable copying
  ConcurrentQueue& operator=(const ConcurrentQueue&) = delete; // disable assignment

 private:
  std::queue<T> queue_;
  std::mutex mutex_;
  std::condition_variable cond_;
  const static unsigned int BUFFER_SIZE = 10;
};

#endif

// producer-consumer.cc
#include "phoenix_json_global.h"
#include "concurrent-queue.h"
#include "phoenix_json_global.h"
#include <iostream>
#include "phoenix_json_global.h"
#include <thread>

void produce(ConcurrentQueue<int>& q) {
  for (int i = 0; i< 10000; ++i) {
    std::cout << "Pushing " << i << "\n";
    q.push(i);
  }
}

void consume(ConcurrentQueue<int>& q, unsigned int id) {
  for (int i = 0; i< 2500; ++i) {
    auto item = q.pop();
    std::cout << "Consumer " << id << " popped " << item << "\n";
  }
}

int main() {
  ConcurrentQueue<int> q;

  using namespace std::placeholders;

  // producer thread
  std::thread prod1(std::bind(produce, std::ref(q)));

  // consumer threads
  std::thread consumer1(std::bind(&consume, std::ref(q), 1));
  std::thread consumer2(std::bind(&consume, std::ref(q), 2));
  std::thread consumer3(std::bind(&consume, std::ref(q), 3));
  std::thread consumer4(std::bind(&consume, std::ref(q), 4));

  prod1.join();
  consumer1.join();
  consumer2.join();
  consumer3.join();
  consumer4.join();
  return 0;
}

// makefile
STD :=c++0x
CPPFLAGS := -Iinclude
CXXFLAGS := -Wall -Wextra -pedantic-errors -std=$(STD) -O2 -pthread

producer_consumer : concurrent-queue.h

*/
