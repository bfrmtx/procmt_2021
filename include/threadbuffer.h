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

#ifndef THREADBUFFER_H
#define THREADBUFFER_H

#include <chrono>
#include <iostream>
#include <memory>

#include <mutex>
#include <thread>
#include <condition_variable>
#include <complex>
#include <vector>
#include <cstdlib>
#include <typeinfo>
#include <QString>

#include "procmt_alldefines.h"

namespace threadbuffer_status {


enum threadbuffer_status : std::int32_t {
running = 0,           //!<
finished = 1,          //!< last buffer submitted, fetch it
terminated = 2,        //!< no last buffer available, nothing to fetch
fetch_error = 4,       //!<
deposit_error = 3,     //!<
};
}

/*!
  \brief threadbuffer a class to exchange data bewteen threads with C++11 swap/move
  This will only work fast with C++11 and above
  In the producer you may try to allocate the vector like v.reserve(1024), v.resize(1024); this may not be neccessary

  In the produce you <b>MUST use threadbuffer->set_stauts(pmt::threadbuffer_finished)</b> when
  exiting the producer loop or delete the producer.<br>
  The consumer thread typically will use do { ...} while (buffer_status == pmt::threadbuffer_running);<br>

  The vecor size may change during runtime - I tested that only for the last buffer <br>

  TO TERMINATE the process send an empty buffer this->spcbuffer->deposit(this->spcdata, threadbuffer_status::terminated); <br>
  so the vector spcdata is zero size; the receiver thread must wake and swap finally!


 */
template<typename T> class threadbuffer {


    std::vector<std::vector<T> > buffer;

    size_t capacity;        //!< n vectors of vector_size
    size_t vector_size;     //!< vector of size
    atsfileout  buffer_type;       //!< see alldefines atsfileout
    size_t id = 0;          //!< set a buffer id for debugging
    size_t front;           //!< front counter of cyclic buffer
    size_t rear;            //!< rear counter of cyclic buffer
    size_t count;           //!< increased by deposit, decreased by fetch

    std::mutex lock;        //!< locker bewtween deposit and fetch
    int runstatus;          //!< threadbufferStatus

    std::condition_variable not_full;
    std::condition_variable not_empty;




public:


    threadbuffer(size_t capacity, size_t vector_size, const atsfileout buffer_type, const size_t id) :  capacity(capacity), vector_size(vector_size),
        buffer_type(buffer_type), id(id), front(0), rear(0), count(0) {
        buffer.resize(capacity);

        // for the C++11 swap we try to have the exact capacity and size!
        for (auto &v : buffer) {
            v.reserve(vector_size);
            v.resize(vector_size);
        }
        // this->vector_size = vector_size;
        this->runstatus = threadbuffer_status::running;
#ifndef QT_NO_DEBUG_OUTPUT
        std::cerr << "threadbuffer created, capacity: " << this->get_capacity() << " vector size: " << this->get_vector_size() << " status:" << this->get_status() << ", id:" << this->get_id() << std::endl;
#endif
    }

    /*!
       \brief get_capacity: ref get_vector_size()
       \return number of vectors
     */
    size_t get_capacity() const {
        return this->capacity;
    }

    /*!
       \brief get_buffer_type
       \return return if time series, complex or stacked and so on
     */
    atsfileout get_buffer_type() const {
        return this->buffer_type;
    }

    /*!
       \brief get_id
       \return my id
     */
    size_t get_id() const {
        return this->id;
    }

    /*!
       \brief get_vector_size
       \return size of each vector; ref get_capacity()
     */
    size_t get_vector_size() const {
        return this->vector_size;
    }

    /*!
       \brief ~threadbuffers doe nothing; all data is NOT created with "new" and will be deleted
        when this class goes out of scope
    */
    ~threadbuffer(){

#ifndef QT_NO_DEBUG_OUTPUT
        std::cerr << "threadbuffer[" << this->id << "] released and deleted" << std::endl;
#endif
    }

    /*!
       \brief set_status set the statust to pmt::threadbuffer_finished when the producer thread finishes
        at least check this when the class is deleted (deleter / de-constructor )
       \param status
     */
    void set_status(const int status) {
        this->runstatus = status;
    }

    /*!
       \brief get_status can be used to terminate the consumer thread
       \return pmt::threadbuffer_XXXX
     */
    int get_status() const {
        return this->runstatus;
    }


    /*!
     * \brief deposit
     * \param data
     * \param runstatus
     */

    void deposit(std::vector<double> &data, const int runstatus){
        std::unique_lock<std::mutex> l(lock);
        // wait that not full anymore and I can deposit (count is != that is less)
        not_full.wait(l, [this](){return (count != capacity) ; });
        std::swap(buffer[rear], data);
        rear = (rear + 1) % capacity;
        ++count;
        this->runstatus = runstatus;
        not_empty.notify_one();
    }

    void deposit(std::vector<std::complex<double>> &data, const int runstatus){
        std::unique_lock<std::mutex> l(lock);
        not_full.wait(l, [this](){return (count != capacity) ; });
        std::swap(buffer[rear], data);
        rear = (rear + 1) % capacity;
        ++count;
        // std::cerr <<"deposit " << count << std::endl;
        this->runstatus = runstatus;
        not_empty.notify_one();
    }

    void deposit(std::vector<int> &data, const int runstatus){
        std::unique_lock<std::mutex> l(lock);
        // wait that not full anymore and I can deposit (count is != that is less)
        not_full.wait(l, [this](){return (count != capacity) ; });
        std::swap(buffer[rear], data);
        rear = (rear + 1) % capacity;
        ++count;
        this->runstatus = runstatus;
        not_empty.notify_one();
    }

    void deposit(std::vector<QString> &data, const int runstatus){
        std::unique_lock<std::mutex> l(lock);
        // wait that not full anymore and I can deposit (count is != that is less)
        not_full.wait(l, [this](){return (count != capacity) ; });
        std::swap(buffer[rear], data);
        rear = (rear + 1) % capacity;
        ++count;
        this->runstatus = runstatus;
        not_empty.notify_one();
    }


    /*!
       \brief fetch swaps the vector data; check  while (buffer_status == pmt::threadbuffer_running);
       \param data
       \return int status pmt::threadbuffer_XXXX
     */

    void fetch(std::vector<double> &data, int &buffer_status){
        std::unique_lock<std::mutex> l(lock);
        // buffer is not empty and I can fetch
        not_empty.wait(l, [this](){return count != 0; });
        std::swap(buffer[front], data);
        front = (front + 1) % capacity;
        --count;
        // always empty the buffer then check in the deposit process is finished
        if (!count) buffer_status = this->runstatus;
        // always empty the buffer first
        // now return the status from the producer
        else buffer_status =  threadbuffer_status::running;
        not_full.notify_one();
    }

    void fetch(std::vector<std::complex<double>> &data, int &buffer_status){
        std::unique_lock<std::mutex> l(lock);
        not_empty.wait(l, [this](){return count != 0; });
        std::swap(buffer[front], data);
        front = (front + 1) % capacity;
        --count;
        // std::cerr <<"fetch " << count << std::endl;

        // always empty the buffer then check in the deposit process is finished
        if (!count) buffer_status = this->runstatus;
        // always empty the buffer first
        // now return the status from the producer
        else buffer_status =  threadbuffer_status::running;

        not_full.notify_one();

    }

    void fetch(std::vector<int> &data, int &buffer_status){
        std::unique_lock<std::mutex> l(lock);
        // buffer is not empty and I can fetch
        not_empty.wait(l, [this](){return count != 0; });
        std::swap(buffer[front], data);
        front = (front + 1) % capacity;
        --count;
        // always empty the buffer then check in the deposit process is finished
        if (!count) buffer_status = this->runstatus;
        // always empty the buffer first
        // now return the status from the producer
        else buffer_status =  threadbuffer_status::running;
        not_full.notify_one();
    }

    void fetch(std::vector<QString> &data, int &buffer_status){
        std::unique_lock<std::mutex> l(lock);
        // buffer is not empty and I can fetch
        not_empty.wait(l, [this](){return count != 0; });
        std::swap(buffer[front], data);
        front = (front + 1) % capacity;
        --count;
        // always empty the buffer then check in the deposit process is finished
        if (!count) buffer_status = this->runstatus;
        // always empty the buffer first
        // now return the status from the producer
        else buffer_status =  threadbuffer_status::running;
        not_full.notify_one();
    }
    /*!
     * \brief terminate set buffer state and wake; fetcher process must decide what to to
     */
    void terminate_deposit() {
      this->runstatus = threadbuffer_status::deposit_error;
      count = 1; // so fetch will retun the run status as given above
      not_empty.notify_one();

    }
    void terminate_fetch() {
      this->runstatus = threadbuffer_status::fetch_error;
      count = 0;
      not_full.notify_one();

    }



};

#endif // THREADBUFFER_H
