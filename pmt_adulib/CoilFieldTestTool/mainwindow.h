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
#include <QSettings>
#include <QUrl>
#include <QFileInfo>
#include <QDrag>
#include <QDir>
#include <QDirIterator>
#include <QDropEvent>
#include <QMimeData>
#include <QDragEnterEvent>

#include <atomic>
#include <thread>

#include "plot.h"
#include "calib_lab.h"
#include "atsheader.h"
#include "atsfile.h"


#include "QFileDialog"
#include "QDebug"
#include "QMessageBox"
#include "QProgressDialog"

#include "GlobalIncludes.h"


#ifdef _msvc
#define _USE_MATH_DEFINES
#endif
/**
 * Version Number
 */
#define C_VERSION_MAIN      0
#define C_VERSION_SUB       1
#define C_VERSION_CUSTOMER  100000
#define C_VERSION_KM_MAIN    1
#define C_VERSION_KM_SUB    "$Revision: 42 $"


typedef enum
{
    Frequency  = 0,
    Amplitude  = 1,
    Phase      = 2,
    Coherency  = 3,
    Noise      = 4,
    Error      = 5
} t_VectorData;


namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:

        explicit MainWindow(QWidget *parent = Q_NULLPTR);

        ~MainWindow();

        void processingThreadFunc (void);

        void dragEnterEvent(QDragEnterEvent *event);

        void dropEvent(QDropEvent *event);


    private slots:
        void on_pbOpenData_clicked (void);

        void on_pbCloseData_clicked (void);

        void on_pbComputeTF_clicked (void);

        void on_pbShowPlot_clicked (void);


        void on_cbFFTWindowLength_currentIndexChanged(const QString &arg1);


        void on_cbFFT_cuts_currentIndexChanged(const QString &arg1);

        void on_xstddev_doubleSpinBox_valueChanged(double arg1);

private:

        Ui::MainWindow *ui;

        QString      qstrTSSourceDir;  //!< dir to open or slot

        /**
         * 1: TS index from table
         * 2: Channel index  (Hx, Hy, Hz)
         * 3: Header Strings (Sensor Type, Cal Frequency, ...)
         */
        QVector<QVector<QVector<QString> > > qvecTFHeader;

        /**
         * 1: TS index from table
         * 2: Channel index (Hx, Hy, Hz)
         * 3: TF Data (0: Frequency, 1: Magnitude, 2: Phase)
         * 4: Data
         */
        QVector<QVector<QVector<QVector<double> > > > qvecTFData;

        QVector<QVector<double>> qvecCalChopperOnFreq;
        QVector<QVector<double>> qvecCalChopperOnAmpl;
        QVector<QVector<double>> qvecCalChopperOnPhase;

        QVector<QVector<double>> qvecCalChopperOffFreq;
        QVector<QVector<double>> qvecCalChopperOffAmpl;
        QVector<QVector<double>> qvecCalChopperOffPhase;

        Plot* pclPlot;

        quint32 uiwsize = 0;

        QString qstrLastTSDir;

        QString qstrVersion;

        QList<std::shared_ptr<QMap<QString, parallel_test_coherency>>> data;    //!< list of data, each list item == direcorty
        std::shared_ptr<prc_com> cmdline;                                       //!< all options will be collected here
        QList<QFileInfo> qfis;                                                  //!< for example 2 * (Hx, Hy, Hz) from two runs ats files
        QFileInfo info_db = QFileInfo("info.sql3");                                                      //!< database file to open
        QFileInfo mastercal_db = QFileInfo("master_calibration.sql3");                                                 //!< database file to open

        std::atomic<bool> atomProcessingThreadRunning;

};

#endif // MAINWINDOW_H
