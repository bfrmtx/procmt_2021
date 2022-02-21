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

#ifndef PLOT_H
#define PLOT_H

#include <QDialog>
#include <QButtonGroup>
#include <QGroupBox>
#include <QPrinter>
#include <QPrintDialog>
#include <QPalette>
#include <QVector>
#include "qtx_templates.h"
#include "GlobalIncludes.h"
#include <cstdlib>
#include "qcustomplot.h"
#include "calibration.h"

#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

typedef struct {
    QCPItemLine *hLine;
    QCPItemLine *vLine;
} QCPCursor;

namespace Ui {
class Plot;
}

class Plot : public QDialog
{
    Q_OBJECT

public:
    explicit Plot(const QFileInfo &in_info_db, QFileInfo &in_master_cal_db,  QWidget *parent = Q_NULLPTR);
    ~Plot();

    bool showData (QVector<QVector<QVector<QString> > >& qvecTFHeader, QVector<QVector<QVector<QVector<double> > > >& qvecTFData, QVector<QVector<double> > &qvecCalChopperOnFreq, QVector<QVector<double> > &qvecCalChopperOnAmpl, QVector<QVector<double> > &qvecCalChopperOnPhase, QVector<QVector<double> > &qvecCalChopperOffFreq, QVector<QVector<double> > &qvecCalChopperOffAmpl, QVector<QVector<double> > &qvecCalChopperOffPhase);

public slots:
    void on_pbAutoScale_clicked (void);

    void on_pbPrint_clicked (void);

    void on_pbShowCurveGraph1_clicked (void);
    void on_pbShowCurveGraph2_clicked (void);
    void on_pbShowCurveGraph3_clicked (void);

    void on_pbShowDatapointsGraph1_clicked (void);
    void on_pbShowDatapointsGraph2_clicked (void);
    void on_pbShowDatapointsGraph3_clicked (void);

    void mouseMoveMagPlot   (QMouseEvent* event);
    void mouseMoveCohPlot   (QMouseEvent* event);
    void mouseMoveNoisePlot (QMouseEvent* event);

    void mouseMoveCalChopperOnAmplPlot  (QMouseEvent* event);
    void mouseMoveCalChopperOnPhasePlot (QMouseEvent* event);

    void mouseMoveCalChopperOffAmplPlot  (QMouseEvent* event);
    void mouseMoveCalChopperOffPhasePlot (QMouseEvent* event);

    void slotMouseMove(QMouseEvent* event, QCustomPlot* pclPlot, const QString qstrUnit);


signals:
    void sigMouseMove (QMouseEvent* event, QCustomPlot* pclPlot, const QString qstrUnit);

private:
    Ui::Plot *ui;

//    QVector<double> qvecRefMFS06FreqChopperOn;
//    QVector<double> qvecRefMFS06MagChopperOn;
//    QVector<double> qvecRefMFS06PhaseChooperOn;

//    QVector<double> qvecRefMFS06FreqChopperOff;
//    QVector<double> qvecRefMFS06MagChopperOff;
//    QVector<double> qvecRefMFS06PhaseChooperOff;

//    QVector<double> qvecRefMFS07FreqChopperOn;
//    QVector<double> qvecRefMFS07MagChopperOn;
//    QVector<double> qvecRefMFS07PhaseChooperOn;

//    QVector<double> qvecRefMFS07FreqChopperOff;
//    QVector<double> qvecRefMFS07MagChopperOff;
//    QVector<double> qvecRefMFS07PhaseChooperOff;

    QVector<QPushButton*> qvechowCurveButtons;
    QVector<QPushButton*> qvechowPointsButtons;

    QVector<QColor> qvecColours;

    QVector<QGroupBox*> qvecGraphButtons;

    QVector<QCPErrorBars*> qvecErrorBars;

    void on_pbShowDatapoints_clicked (int iID);
    void on_pbShowCurve_clicked      (int iID);

    QVector<QCPCursor> qvecCursorMagPlot;
    QVector<QCPCursor> qvecCursorCohPlot;
    QVector<QCPCursor> qvecCursorNoisePlot;

    QVector<QCPCursor> qvecCursorCalChopperOnAmplPlot;
    QVector<QCPCursor> qvecCursorCalChopperOnPhasePlot;

    QVector<QCPCursor> qvecCursorCalChopperOffAmplPlot;
    QVector<QCPCursor> qvecCursorCalChopperOffPhasePlot;

    unsigned int uiMousePosX;

    qint64 qiLastCursourUpdate;

    std::atomic<bool> atomCoursorUpdateActive;


    // databases for default calibration values
    QFileInfo info_db;
    QFileInfo master_cal_db;


    void initRefCurves (void);

    QSqlDatabase master_calibration_db;

};

#endif // PLOT_H
