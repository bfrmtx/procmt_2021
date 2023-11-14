#include "plot.h"
#include "ui_plot.h"

#include <QPrintDialog>
#include <QPrinter>

#include "GlobalIncludes.h"
#include "utils.h"

Plot::Plot(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Plot) {
  ui->setupUi(this);

  // restore geometry
  pclSettings->beginGroup(this->objectName());
  this->restoreGeometry(pclSettings->value("geometry").toByteArray());
  pclSettings->endGroup();

  // save geometry
  pclSettings->beginGroup(this->objectName());
  pclSettings->setValue("geometry", this->saveGeometry());
  pclSettings->endGroup();

  initRefCurves();
}

Plot::~Plot() {
  // save geometry
  pclSettings->beginGroup(this->objectName());
  pclSettings->setValue("geometry", this->saveGeometry());
  pclSettings->endGroup();

  delete ui;
}

void Plot::on_pbSave_clicked(void) {
  if (this->parent() != NULL) {
    ((MainWindow *)this->parent())->saveDataToTXT();
  }
}

#define C_DRAW_LIMIT_CURVES

bool Plot::showData(QVector<QVector<QVector<QString>>> &qvecTFHeader, QVector<QVector<QVector<QVector<double>>>> &qvecTFData, const bool bRawData) const {
  bool bRetValue = true;
  unsigned int uiSetCounter;
  unsigned int uiCHCounter;
  unsigned int uiCounter;

  QVector<double> qvecFreq;
  QVector<double> qvecMag;
  QVector<double> qvecPhase;

  QVector<double> qvecRevLimUp;
  QVector<double> qvecRevLimLow;
  QVector<double> qvecRevLimFreq;
  QVector<double> qvecRevMag;
  QVector<double> qvecRevPhase;

  QCustomPlot *pclMagPlot;
  QCustomPlot *pclPhasePlot;

  unsigned int uiSetSmallestCalFreq = 0;

  QString qstrTemp;
  QString qstrExportFileName;

  double dStopFreq = 1.0E-100;
  double dStartFreq = 1.0E+100;
  double dLimPercent = 3.0;
  double dLimDegree = 3.0;

  for (uiCHCounter = 0; uiCHCounter < qvecTFData[0].size(); uiCHCounter++) {
    qvecFreq.clear();
    qvecMag.clear();
    qvecPhase.clear();

    for (uiSetCounter = 0; uiSetCounter < qvecTFHeader.size(); uiSetCounter++) {
      for (uiCounter = 0; uiCounter < qvecTFData[uiSetCounter][uiCHCounter][0].size(); uiCounter++) {
        // frequency
        qvecFreq.push_back(qvecTFData[uiSetCounter][uiCHCounter][0][uiCounter]);
        if (dStartFreq > qvecFreq.last()) {
          dStartFreq = qvecFreq.last();
          uiSetSmallestCalFreq = uiSetCounter;
        }
        if (dStopFreq < qvecFreq.last()) {
          dStopFreq = qvecFreq.last();
        }

        // magnitude
        qvecMag.push_back(qvecTFData[uiSetCounter][uiCHCounter][1][uiCounter]);

        // phase
        qvecPhase.push_back(qvecTFData[uiSetCounter][uiCHCounter][2][uiCounter]);
      }
    }

    qstrTemp = qvecTFHeader[0][uiCHCounter][3];
    qstrTemp += " - ";
    qstrTemp += qvecTFHeader[0][uiCHCounter][2];
    qstrTemp += " - ";
    qstrTemp += qvecTFHeader[uiSetSmallestCalFreq][uiCHCounter][4];

    qstrExportFileName = qvecTFHeader[0][uiCHCounter][2];
    qstrExportFileName = qstrExportFileName.left(6);
    qstrExportFileName = qstrExportFileName + "_";
    qstrExportFileName = qstrExportFileName + qvecTFHeader[0][uiCHCounter][2].right(3);
    qstrExportFileName = qstrExportFileName + "_InSituCal.txt";

    switch (uiCHCounter) {
    case 0: {
      pclMagPlot = ui->plotMagHx;
      pclPhasePlot = ui->plotPhaseHx;
      ui->qtabPlots->setTabText(0, qstrTemp);
      break;
    }

    case 1: {
      pclMagPlot = ui->plotMagHy;
      pclPhasePlot = ui->plotPhaseHy;
      ui->qtabPlots->setTabText(1, qstrTemp);
      break;
    }

    case 2: {
      pclMagPlot = ui->plotMagHz;
      pclPhasePlot = ui->plotPhaseHz;
      ui->qtabPlots->setTabText(2, qstrTemp);
      break;
    }

    default: {
      pclMagPlot = NULL;
      pclPhasePlot = NULL;
      break;
    }
    }

    if (pclMagPlot != NULL) {
      pclMagPlot->clearGraphs();
      pclMagPlot->addGraph();
      pclMagPlot->graph(0)->setName("Computed Calibration\nFunction");
      pclMagPlot->graph(0)->addData(qvecFreq, qvecMag);
      pclMagPlot->graph(0)->setVisible(true);
      QPen clPen = QPen(Qt::blue);
      clPen.setWidth(3);
      pclMagPlot->graph(0)->setPen(clPen);
      pclMagPlot->graph(0)->setBrush(Qt::NoBrush);
      pclMagPlot->graph(0)->rescaleAxes(true);
      pclMagPlot->graph(0)->setBrush(QBrush(QColor(0, 255, 0, 0)));
      pclMagPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::blue, 10.0));
      pclMagPlot->xAxis->setScaleType(QCPAxis::stLogarithmic);
      pclMagPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
      // pclMagPlot->yAxis->setAutoSubTicks(true);
      pclMagPlot->repaint();
      pclMagPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
      pclMagPlot->rescaleAxes();

      qstrTemp = "Magnitude [V/(nT * Hz)] - ";
      qstrTemp += qvecTFHeader[uiSetSmallestCalFreq][uiCHCounter][2];
      qstrTemp += " - ";
      qstrTemp += qvecTFHeader[uiSetSmallestCalFreq][uiCHCounter][4];

      pclMagPlot->plotLayout()->insertRow(0);
      // dont know
      pclMagPlot->plotLayout()->addElement(0, 0, new QCPTextElement(pclMagPlot, qstrTemp, QFont("sans", 12, QFont::Light)));
      // pclMagPlot->plotLayout()->addElement(0, 0, new QCPPlotTitle(pclMagPlot, qstrTemp));

      pclMagPlot->legend->setVisible(true);
      pclMagPlot->legend->setFont(QFont("Helvetica", 9));
      pclMagPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));

      pclMagPlot->graph(0)->keyAxis()->setLabel("Frequency [Hz]");
      if (bRawData == false) {
        pclMagPlot->graph(0)->valueAxis()->setLabel("Magnitude [nT/(V*Hz)]");
      } else {
        pclMagPlot->graph(0)->valueAxis()->setLabel("Magnitude [1]");
      }
    }

    if (pclPhasePlot != NULL) {
      pclPhasePlot->clearGraphs();
      pclPhasePlot->addGraph();
      pclPhasePlot->graph(0)->setName("Computed Calibration\nFunction");
      pclPhasePlot->graph(0)->addData(qvecFreq, qvecPhase);
      pclPhasePlot->graph(0)->setVisible(true);
      QPen clPen = QPen(Qt::blue);
      clPen.setWidth(3);
      pclPhasePlot->graph(0)->setPen(clPen);
      pclPhasePlot->graph(0)->setBrush(Qt::NoBrush);
      pclPhasePlot->graph(0)->rescaleAxes(true);
      pclPhasePlot->graph(0)->setBrush(QBrush(QColor(0, 255, 0, 0)));
      pclPhasePlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::blue, 10.0));
      pclPhasePlot->xAxis->setScaleType(QCPAxis::stLogarithmic);
      pclPhasePlot->yAxis->setScaleType(QCPAxis::stLinear);
      // pclPhasePlot->yAxis->setAutoSubTicks(true);
      pclPhasePlot->repaint();
      pclPhasePlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
      pclPhasePlot->rescaleAxes();

      qstrTemp = "Phase [degree] - ";
      qstrTemp += qvecTFHeader[uiSetSmallestCalFreq][uiCHCounter][2];
      qstrTemp += " - ";
      qstrTemp += qvecTFHeader[uiSetSmallestCalFreq][uiCHCounter][4];

      pclPhasePlot->plotLayout()->insertRow(0);
      // dont know
      pclPhasePlot->plotLayout()->addElement(0, 0, new QCPTextElement(pclPhasePlot, qstrTemp, QFont("sans", 12, QFont::Light)));
      // pclPhasePlot->plotLayout()->addElement(0, 0, new QCPPlotTitle(pclPhasePlot, qstrTemp));

      pclPhasePlot->legend->setVisible(true);
      pclPhasePlot->legend->setFont(QFont("Helvetica", 9));
      pclPhasePlot->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));

      pclPhasePlot->graph(0)->keyAxis()->setLabel("Frequency [Hz]");
      pclPhasePlot->graph(0)->valueAxis()->setLabel("Phase [degree]");
    }

    QFile qfExport;
    QDir qdActDir;
    qstrExportFileName = qdActDir.currentPath() + "/" + qstrExportFileName;
    qfExport.setFileName(qstrExportFileName);
    if (qfExport.exists() == true) {
      qfExport.remove();
    }
    if (qfExport.open(QIODevice::WriteOnly) == true) {
      qDebug() << "==================================";
      QString qstrOut;
      qstrOut.clear();
      //   for (uiCounter = 0; uiCounter < pclMagPlot->graph(0)->data()->keys().size(); uiCounter++) {
      //     qstrOut += getFormatedDouble(pclMagPlot->graph(0)->data()->keys()[uiCounter]) + " " + getFormatedDouble(pclMagPlot->graph(0)->data()->values().at(uiCounter).value) + " " + getFormatedDouble(pclPhasePlot->graph(0)->data()->values().at(uiCounter).value) + "\n";
      //     qDebug() << getFormatedDouble(pclMagPlot->graph(0)->data()->keys()[uiCounter]) + " " + getFormatedDouble(pclMagPlot->graph(0)->data()->values().at(uiCounter).value) + " " + getFormatedDouble(pclPhasePlot->graph(0)->data()->values().at(uiCounter).value) + "\n";
      //   }

      QVector<double> keys;
      QVector<double> values;
      // QCPDataContainer<QCPGraphData> *data = pclMagPlot->graph(0)->data();
      auto mdata = pclMagPlot->graph(0)->data();
      auto pdata = pclPhasePlot->graph(0)->data();
      for (int i = 0; i < mdata->size(); i++) {
        qstrOut += getFormatedDouble(mdata->at(i)->key) + " " + getFormatedDouble(mdata->at(i)->value) + " " + getFormatedDouble(pdata->at(i)->value) + "\n";
        // qDebug() << getFormatedDouble(mdata->at(i)->key) + " " + getFormatedDouble(mdata->at(i)->value) + " " + getFormatedDouble(pdata->at(i)->value) + "\n";
      }

      qDebug() << "==================================";
      qfExport.write(qstrOut.toUtf8());
      qfExport.close();
    }

    if (bRawData == false) {
      // display reference curve
      qvecFreq.clear();
      qvecMag.clear();
      qvecPhase.clear();

      if ((qvecTFHeader[uiSetSmallestCalFreq][uiCHCounter][2].contains("MFS06") == true) ||
          (qvecTFHeader[uiSetSmallestCalFreq][uiCHCounter][2].contains("MFS10") == true)) {
        if (qvecTFHeader[uiSetSmallestCalFreq][uiCHCounter][4].contains("Chopper On") == true) {
          qvecFreq = this->qvecRefMFS06FreqChopperOn;
          qvecMag = this->qvecRefMFS06MagChopperOn;
          qvecPhase = this->qvecRefMFS06PhaseChooperOn;
        } else {
          qvecFreq = this->qvecRefMFS06FreqChopperOff;
          qvecMag = this->qvecRefMFS06MagChopperOff;
          qvecPhase = this->qvecRefMFS06PhaseChooperOff;
        }
      } else if ((qvecTFHeader[uiSetSmallestCalFreq][uiCHCounter][2].contains("MFS07") == true) ||
                 (qvecTFHeader[uiSetSmallestCalFreq][uiCHCounter][2].contains("MFS11") == true)) {
        if (qvecTFHeader[uiSetSmallestCalFreq][uiCHCounter][4].contains("Chopper On") == true) {
          qvecFreq = this->qvecRefMFS07FreqChopperOn;
          qvecMag = this->qvecRefMFS07MagChopperOn;
          qvecPhase = this->qvecRefMFS07PhaseChooperOn;
        } else {
          qvecFreq = this->qvecRefMFS07FreqChopperOff;
          qvecMag = this->qvecRefMFS07MagChopperOff;
          qvecPhase = this->qvecRefMFS07PhaseChooperOff;
        }
      }

      qvecRevLimUp.clear();
      qvecRevLimLow.clear();
      qvecRevLimFreq.clear();
      qvecRevMag.clear();
      qvecRevPhase.clear();

      for (uiCounter = 0; uiCounter < qvecFreq.size(); uiCounter++) {
        if ((qvecFreq[uiCounter] >= dStartFreq) &&
            (qvecFreq[uiCounter] <= dStopFreq)) {
          qvecRevLimUp.push_back(qvecMag[uiCounter] + (qvecMag[uiCounter] * dLimPercent / 100.0));
          qvecRevLimLow.push_back(qvecMag[uiCounter] - (qvecMag[uiCounter] * dLimPercent / 100.0));
          qvecRevLimFreq.push_back(qvecFreq[uiCounter]);

          qvecRevMag.push_back(qvecMag[uiCounter]);
          qvecRevPhase.push_back(qvecPhase[uiCounter]);
        }
      }

// draw reference curve for mag and phase (only scatter type)
#ifndef C_DRAW_LIMIT_CURVES
      pclMagPlot->addGraph();
      pclMagPlot->graph(1)->addData(qvecRevLimFreq, qvecRevMag);
      pclMagPlot->graph(1)->setVisible(true);
      pclMagPlot->graph(1)->setPen(QPen(Qt::red));
      pclMagPlot->graph(1)->setBrush(Qt::NoBrush);
      QPen clPen = QPen(Qt::red);
      clPen.setWidth(5);
      pclMagPlot->graph(1)->setPen(Qt::NoPen);
      pclMagPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCrossCircle, Qt::red, 12.0));

      pclPhasePlot->addGraph();
      pclPhasePlot->graph(1)->addData(qvecRevLimFreq, qvecRevPhase);
      pclPhasePlot->graph(1)->setVisible(true);
      pclPhasePlot->graph(1)->setPen(QPen(Qt::red));
      pclPhasePlot->graph(1)->setBrush(Qt::NoBrush);
      clPen.setWidth(5);
      pclPhasePlot->graph(1)->setPen(Qt::NoPen);
      pclPhasePlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCrossCircle, Qt::red, 12.0));
#endif

#ifdef C_DRAW_LIMIT_CURVES
      if (qvecRevLimFreq.size() > 0) {
        QPen clLimCurvePen((QPen(Qt::red)));
        clLimCurvePen.setStyle(Qt::DotLine);
        clLimCurvePen.setWidth(1);
        pclMagPlot->addGraph();
        pclMagPlot->addGraph();
        pclMagPlot->graph(1)->setName("Tolerance:\nReference Curve +/- 3.0 %");
        pclMagPlot->graph(1)->addData(qvecRevLimFreq, qvecRevLimUp);
        pclMagPlot->graph(1)->setVisible(true);
        pclMagPlot->graph(1)->setPen(clLimCurvePen);
        pclMagPlot->graph(1)->setBrush(QBrush(QColor(255, 50, 30, 40)));
        pclMagPlot->graph(2)->addData(qvecRevLimFreq, qvecRevLimLow);
        pclMagPlot->graph(2)->setVisible(true);
        pclMagPlot->graph(2)->setPen(clLimCurvePen);
        pclMagPlot->graph(2)->setBrush(Qt::NoBrush);
        pclMagPlot->graph(1)->setChannelFillGraph(pclMagPlot->graph(2));
        pclMagPlot->legend->removeItem(pclMagPlot->legend->itemCount() - 1);
      }
#endif

      qvecRevLimUp.clear();
      qvecRevLimLow.clear();
      qvecRevLimFreq.clear();

      for (uiCounter = 0; uiCounter < qvecFreq.size(); uiCounter++) {
        if ((qvecFreq[uiCounter] >= dStartFreq) &&
            (qvecFreq[uiCounter] <= dStopFreq)) {
          qvecRevLimUp.push_back(qvecPhase[uiCounter] + dLimDegree);
          qvecRevLimLow.push_back(qvecPhase[uiCounter] - dLimDegree);
          qvecRevLimFreq.push_back(qvecFreq[uiCounter]);
        }
      }

#ifdef C_DRAW_LIMIT_CURVES
      if (qvecRevLimFreq.size() > 0) {
        QPen clLimCurvePen((QPen(Qt::red)));
        clLimCurvePen.setStyle(Qt::DotLine);
        clLimCurvePen.setWidth(1);
        pclPhasePlot->addGraph();
        pclPhasePlot->addGraph();
        pclPhasePlot->graph(1)->setName("Tolerance:\nReference Curve +/- 3.0 degree");
        pclPhasePlot->graph(1)->addData(qvecRevLimFreq, qvecRevLimUp);
        pclPhasePlot->graph(1)->setVisible(true);
        pclPhasePlot->graph(1)->setPen(clLimCurvePen);
        pclPhasePlot->graph(1)->setBrush(QBrush(QColor(255, 50, 30, 40)));
        pclPhasePlot->graph(2)->addData(qvecRevLimFreq, qvecRevLimLow);
        pclPhasePlot->graph(2)->setVisible(true);
        pclPhasePlot->graph(2)->setPen(clLimCurvePen);
        pclPhasePlot->graph(2)->setBrush(Qt::NoBrush);
        pclPhasePlot->graph(1)->setChannelFillGraph(pclPhasePlot->graph(2));
        pclPhasePlot->legend->removeItem(pclPhasePlot->legend->itemCount() - 1);
      }
#endif
    }

    pclMagPlot->rescaleAxes();
    pclPhasePlot->rescaleAxes();
  }

  return (bRetValue);
}

void Plot::initRefCurves(void) {
  // clear all arrays
  qvecRefMFS06FreqChopperOn.clear();
  qvecRefMFS06MagChopperOn.clear();
  qvecRefMFS06PhaseChooperOn.clear();

  qvecRefMFS06FreqChopperOff.clear();
  qvecRefMFS06MagChopperOff.clear();
  qvecRefMFS06PhaseChooperOff.clear();

  qvecRefMFS07FreqChopperOn.clear();
  qvecRefMFS07MagChopperOn.clear();
  qvecRefMFS07PhaseChooperOn.clear();

  qvecRefMFS07FreqChopperOff.clear();
  qvecRefMFS07MagChopperOff.clear();
  qvecRefMFS07PhaseChooperOff.clear();

  // MFS06, MFS06e, MFS10e with Chopper On
  qvecRefMFS06FreqChopperOn << 1.0000E-05;
  qvecRefMFS06FreqChopperOn << 1.2589E-05;
  qvecRefMFS06FreqChopperOn << 1.5849E-05;
  qvecRefMFS06FreqChopperOn << 1.9953E-05;
  qvecRefMFS06FreqChopperOn << 2.5119E-05;
  qvecRefMFS06FreqChopperOn << 3.1623E-05;
  qvecRefMFS06FreqChopperOn << 3.9811E-05;
  qvecRefMFS06FreqChopperOn << 5.0119E-05;
  qvecRefMFS06FreqChopperOn << 6.3095E-05;
  qvecRefMFS06FreqChopperOn << 7.9430E-05;
  qvecRefMFS06FreqChopperOn << 1.0000E-04;
  qvecRefMFS06FreqChopperOn << 1.2589E-04;
  qvecRefMFS06FreqChopperOn << 1.5849E-04;
  qvecRefMFS06FreqChopperOn << 1.9953E-04;
  qvecRefMFS06FreqChopperOn << 2.5119E-04;
  qvecRefMFS06FreqChopperOn << 3.1623E-04;
  qvecRefMFS06FreqChopperOn << 3.9811E-04;
  qvecRefMFS06FreqChopperOn << 5.0119E-04;
  qvecRefMFS06FreqChopperOn << 6.3095E-04;
  qvecRefMFS06FreqChopperOn << 7.9430E-04;
  qvecRefMFS06FreqChopperOn << 1.0000E-03;
  qvecRefMFS06FreqChopperOn << 1.2589E-03;
  qvecRefMFS06FreqChopperOn << 1.5849E-03;
  qvecRefMFS06FreqChopperOn << 1.9953E-03;
  qvecRefMFS06FreqChopperOn << 2.5119E-03;
  qvecRefMFS06FreqChopperOn << 3.1623E-03;
  qvecRefMFS06FreqChopperOn << 3.9811E-03;
  qvecRefMFS06FreqChopperOn << 5.0119E-03;
  qvecRefMFS06FreqChopperOn << 6.3095E-03;
  qvecRefMFS06FreqChopperOn << 7.9430E-03;
  qvecRefMFS06FreqChopperOn << 1.0000E-02;
  qvecRefMFS06FreqChopperOn << 1.2589E-02;
  qvecRefMFS06FreqChopperOn << 1.5849E-02;
  qvecRefMFS06FreqChopperOn << 1.9953E-02;
  qvecRefMFS06FreqChopperOn << 2.5119E-02;
  qvecRefMFS06FreqChopperOn << 3.1623E-02;
  qvecRefMFS06FreqChopperOn << 3.9811E-02;
  qvecRefMFS06FreqChopperOn << 5.0119E-02;
  qvecRefMFS06FreqChopperOn << 6.3095E-02;
  qvecRefMFS06FreqChopperOn << 7.9430E-02;
  qvecRefMFS06FreqChopperOn << 1.0000E-01;
  qvecRefMFS06FreqChopperOn << 1.2589E-01;
  qvecRefMFS06FreqChopperOn << 1.5849E-01;
  qvecRefMFS06FreqChopperOn << 1.9953E-01;
  qvecRefMFS06FreqChopperOn << 2.5119E-01;
  qvecRefMFS06FreqChopperOn << 3.1623E-01;
  qvecRefMFS06FreqChopperOn << 3.9811E-01;
  qvecRefMFS06FreqChopperOn << 5.0119E-01;
  qvecRefMFS06FreqChopperOn << 6.3095E-01;
  qvecRefMFS06FreqChopperOn << 7.9430E-01;
  qvecRefMFS06FreqChopperOn << 1.0000E+00;
  qvecRefMFS06FreqChopperOn << 1.2589E+00;
  qvecRefMFS06FreqChopperOn << 1.5849E+00;
  qvecRefMFS06FreqChopperOn << 1.9952E+00;
  qvecRefMFS06FreqChopperOn << 2.5119E+00;
  qvecRefMFS06FreqChopperOn << 3.1623E+00;
  qvecRefMFS06FreqChopperOn << 3.9810E+00;
  qvecRefMFS06FreqChopperOn << 5.0118E+00;
  qvecRefMFS06FreqChopperOn << 6.3095E+00;
  qvecRefMFS06FreqChopperOn << 7.9430E+00;
  qvecRefMFS06FreqChopperOn << 1.0000E+01;
  qvecRefMFS06FreqChopperOn << 1.2589E+01;
  qvecRefMFS06FreqChopperOn << 1.5849E+01;
  qvecRefMFS06FreqChopperOn << 1.9952E+01;
  qvecRefMFS06FreqChopperOn << 2.5119E+01;
  qvecRefMFS06FreqChopperOn << 3.1622E+01;
  qvecRefMFS06FreqChopperOn << 3.9810E+01;
  qvecRefMFS06FreqChopperOn << 5.0118E+01;
  qvecRefMFS06FreqChopperOn << 6.3095E+01;
  qvecRefMFS06FreqChopperOn << 7.9430E+01;
  qvecRefMFS06FreqChopperOn << 1.0000E+02;
  qvecRefMFS06FreqChopperOn << 1.2589E+02;
  qvecRefMFS06FreqChopperOn << 1.5849E+02;
  qvecRefMFS06FreqChopperOn << 1.9952E+02;
  qvecRefMFS06FreqChopperOn << 2.5118E+02;
  qvecRefMFS06FreqChopperOn << 3.1622E+02;
  qvecRefMFS06FreqChopperOn << 3.9810E+02;
  qvecRefMFS06FreqChopperOn << 5.0118E+02;
  qvecRefMFS06FreqChopperOn << 6.3094E+02;
  qvecRefMFS06FreqChopperOn << 7.9430E+02;
  qvecRefMFS06FreqChopperOn << 1.0000E+03;
  qvecRefMFS06FreqChopperOn << 1.2589E+03;
  qvecRefMFS06FreqChopperOn << 1.5849E+03;
  qvecRefMFS06FreqChopperOn << 1.9952E+03;
  qvecRefMFS06FreqChopperOn << 2.5118E+03;
  qvecRefMFS06FreqChopperOn << 3.1622E+03;
  qvecRefMFS06FreqChopperOn << 3.9810E+03;
  qvecRefMFS06FreqChopperOn << 5.0117E+03;
  qvecRefMFS06FreqChopperOn << 6.3094E+03;
  qvecRefMFS06FreqChopperOn << 7.9430E+03;
  qvecRefMFS06FreqChopperOn << 9.9995E+03;
  qvecRefMFS06FreqChopperOn << 1.0000E+04;

  qvecRefMFS06MagChopperOn << 2.0000E-06;
  qvecRefMFS06MagChopperOn << 2.5110E-06;
  qvecRefMFS06MagChopperOn << 3.1612E-06;
  qvecRefMFS06MagChopperOn << 3.9798E-06;
  qvecRefMFS06MagChopperOn << 5.0102E-06;
  qvecRefMFS06MagChopperOn << 6.3075E-06;
  qvecRefMFS06MagChopperOn << 7.9407E-06;
  qvecRefMFS06MagChopperOn << 9.9967E-06;
  qvecRefMFS06MagChopperOn << 1.2585E-05;
  qvecRefMFS06MagChopperOn << 1.5843E-05;
  qvecRefMFS06MagChopperOn << 1.9946E-05;
  qvecRefMFS06MagChopperOn << 2.5110E-05;
  qvecRefMFS06MagChopperOn << 3.1612E-05;
  qvecRefMFS06MagChopperOn << 3.9798E-05;
  qvecRefMFS06MagChopperOn << 5.0102E-05;
  qvecRefMFS06MagChopperOn << 6.3075E-05;
  qvecRefMFS06MagChopperOn << 7.9407E-05;
  qvecRefMFS06MagChopperOn << 9.9967E-05;
  qvecRefMFS06MagChopperOn << 1.2585E-04;
  qvecRefMFS06MagChopperOn << 1.5843E-04;
  qvecRefMFS06MagChopperOn << 1.9946E-04;
  qvecRefMFS06MagChopperOn << 2.5110E-04;
  qvecRefMFS06MagChopperOn << 3.1612E-04;
  qvecRefMFS06MagChopperOn << 3.9798E-04;
  qvecRefMFS06MagChopperOn << 5.0102E-04;
  qvecRefMFS06MagChopperOn << 6.3075E-04;
  qvecRefMFS06MagChopperOn << 7.9407E-04;
  qvecRefMFS06MagChopperOn << 9.9967E-04;
  qvecRefMFS06MagChopperOn << 1.2585E-03;
  qvecRefMFS06MagChopperOn << 1.5843E-03;
  qvecRefMFS06MagChopperOn << 1.9946E-03;
  qvecRefMFS06MagChopperOn << 2.5110E-03;
  qvecRefMFS06MagChopperOn << 3.1612E-03;
  qvecRefMFS06MagChopperOn << 3.9798E-03;
  qvecRefMFS06MagChopperOn << 5.0102E-03;
  qvecRefMFS06MagChopperOn << 6.3075E-03;
  qvecRefMFS06MagChopperOn << 7.9407E-03;
  qvecRefMFS06MagChopperOn << 9.9967E-03;
  qvecRefMFS06MagChopperOn << 1.2585E-02;
  qvecRefMFS06MagChopperOn << 1.5843E-02;
  qvecRefMFS06MagChopperOn << 1.9946E-02;
  qvecRefMFS06MagChopperOn << 2.5118E-02;
  qvecRefMFS06MagChopperOn << 3.1616E-02;
  qvecRefMFS06MagChopperOn << 3.9790E-02;
  qvecRefMFS06MagChopperOn << 5.0072E-02;
  qvecRefMFS06MagChopperOn << 6.2958E-02;
  qvecRefMFS06MagChopperOn << 7.9116E-02;
  qvecRefMFS06MagChopperOn << 9.9366E-02;
  qvecRefMFS06MagChopperOn << 1.2437E-01;
  qvecRefMFS06MagChopperOn << 1.5558E-01;
  qvecRefMFS06MagChopperOn << 1.9406E-01;
  qvecRefMFS06MagChopperOn << 2.4025E-01;
  qvecRefMFS06MagChopperOn << 2.9497E-01;
  qvecRefMFS06MagChopperOn << 3.5828E-01;
  qvecRefMFS06MagChopperOn << 4.2883E-01;
  qvecRefMFS06MagChopperOn << 5.0160E-01;
  qvecRefMFS06MagChopperOn << 5.7402E-01;
  qvecRefMFS06MagChopperOn << 6.3860E-01;
  qvecRefMFS06MagChopperOn << 6.9322E-01;
  qvecRefMFS06MagChopperOn << 7.3528E-01;
  qvecRefMFS06MagChopperOn << 7.6673E-01;
  qvecRefMFS06MagChopperOn << 7.8859E-01;
  qvecRefMFS06MagChopperOn << 8.0261E-01;
  qvecRefMFS06MagChopperOn << 8.1346E-01;
  qvecRefMFS06MagChopperOn << 8.1971E-01;
  qvecRefMFS06MagChopperOn << 8.2356E-01;
  qvecRefMFS06MagChopperOn << 8.2638E-01;
  qvecRefMFS06MagChopperOn << 8.2825E-01;
  qvecRefMFS06MagChopperOn << 8.2724E-01;
  qvecRefMFS06MagChopperOn << 8.3020E-01;
  qvecRefMFS06MagChopperOn << 8.3061E-01;
  qvecRefMFS06MagChopperOn << 8.3038E-01;
  qvecRefMFS06MagChopperOn << 8.3099E-01;
  qvecRefMFS06MagChopperOn << 8.3034E-01;
  qvecRefMFS06MagChopperOn << 8.2907E-01;
  qvecRefMFS06MagChopperOn << 8.3020E-01;
  qvecRefMFS06MagChopperOn << 8.3004E-01;
  qvecRefMFS06MagChopperOn << 8.3000E-01;
  qvecRefMFS06MagChopperOn << 8.2943E-01;
  qvecRefMFS06MagChopperOn << 8.2901E-01;
  qvecRefMFS06MagChopperOn << 8.2726E-01;
  qvecRefMFS06MagChopperOn << 8.2508E-01;
  qvecRefMFS06MagChopperOn << 8.2226E-01;
  qvecRefMFS06MagChopperOn << 8.1632E-01;
  qvecRefMFS06MagChopperOn << 8.0800E-01;
  qvecRefMFS06MagChopperOn << 7.8416E-01;
  qvecRefMFS06MagChopperOn << 7.4692E-01;
  qvecRefMFS06MagChopperOn << 6.7803E-01;
  qvecRefMFS06MagChopperOn << 6.0426E-01;
  qvecRefMFS06MagChopperOn << 5.7523E-01;
  qvecRefMFS06MagChopperOn << 5.4638E-01;
  qvecRefMFS06MagChopperOn << 5.4642E-01;
  for (unsigned int uiCounter = 0; uiCounter < qvecRefMFS06MagChopperOn.size(); uiCounter++) {
    qvecRefMFS06MagChopperOn[uiCounter] = qvecRefMFS06MagChopperOn[uiCounter] / qvecRefMFS06FreqChopperOn[uiCounter];
  }

  qvecRefMFS06PhaseChooperOn << 9.0000E+01;
  qvecRefMFS06PhaseChooperOn << 8.9999E+01;
  qvecRefMFS06PhaseChooperOn << 8.9999E+01;
  qvecRefMFS06PhaseChooperOn << 8.9999E+01;
  qvecRefMFS06PhaseChooperOn << 8.9999E+01;
  qvecRefMFS06PhaseChooperOn << 8.9999E+01;
  qvecRefMFS06PhaseChooperOn << 8.9999E+01;
  qvecRefMFS06PhaseChooperOn << 8.9999E+01;
  qvecRefMFS06PhaseChooperOn << 8.9999E+01;
  qvecRefMFS06PhaseChooperOn << 8.9999E+01;
  qvecRefMFS06PhaseChooperOn << 8.9999E+01;
  qvecRefMFS06PhaseChooperOn << 8.9998E+01;
  qvecRefMFS06PhaseChooperOn << 8.9998E+01;
  qvecRefMFS06PhaseChooperOn << 8.9998E+01;
  qvecRefMFS06PhaseChooperOn << 8.9998E+01;
  qvecRefMFS06PhaseChooperOn << 8.9996E+01;
  qvecRefMFS06PhaseChooperOn << 8.9994E+01;
  qvecRefMFS06PhaseChooperOn << 8.9993E+01;
  qvecRefMFS06PhaseChooperOn << 8.9991E+01;
  qvecRefMFS06PhaseChooperOn << 8.9989E+01;
  qvecRefMFS06PhaseChooperOn << 8.9986E+01;
  qvecRefMFS06PhaseChooperOn << 8.9982E+01;
  qvecRefMFS06PhaseChooperOn << 8.9977E+01;
  qvecRefMFS06PhaseChooperOn << 8.9971E+01;
  qvecRefMFS06PhaseChooperOn << 8.9964E+01;
  qvecRefMFS06PhaseChooperOn << 8.9954E+01;
  qvecRefMFS06PhaseChooperOn << 8.9942E+01;
  qvecRefMFS06PhaseChooperOn << 8.9927E+01;
  qvecRefMFS06PhaseChooperOn << 8.9909E+01;
  qvecRefMFS06PhaseChooperOn << 8.9885E+01;
  qvecRefMFS06PhaseChooperOn << 8.9855E+01;
  qvecRefMFS06PhaseChooperOn << 8.9818E+01;
  qvecRefMFS06PhaseChooperOn << 8.9770E+01;
  qvecRefMFS06PhaseChooperOn << 8.9711E+01;
  qvecRefMFS06PhaseChooperOn << 8.9636E+01;
  qvecRefMFS06PhaseChooperOn << 8.9542E+01;
  qvecRefMFS06PhaseChooperOn << 8.9423E+01;
  qvecRefMFS06PhaseChooperOn << 8.9273E+01;
  qvecRefMFS06PhaseChooperOn << 8.9085E+01;
  qvecRefMFS06PhaseChooperOn << 8.8848E+01;
  qvecRefMFS06PhaseChooperOn << 8.8550E+01;
  qvecRefMFS06PhaseChooperOn << 8.8187E+01;
  qvecRefMFS06PhaseChooperOn << 8.7751E+01;
  qvecRefMFS06PhaseChooperOn << 8.7180E+01;
  qvecRefMFS06PhaseChooperOn << 8.6446E+01;
  qvecRefMFS06PhaseChooperOn << 8.5578E+01;
  qvecRefMFS06PhaseChooperOn << 8.4410E+01;
  qvecRefMFS06PhaseChooperOn << 8.3076E+01;
  qvecRefMFS06PhaseChooperOn << 8.1221E+01;
  qvecRefMFS06PhaseChooperOn << 7.9120E+01;
  qvecRefMFS06PhaseChooperOn << 7.6389E+01;
  qvecRefMFS06PhaseChooperOn << 7.3119E+01;
  qvecRefMFS06PhaseChooperOn << 6.9230E+01;
  qvecRefMFS06PhaseChooperOn << 6.4385E+01;
  qvecRefMFS06PhaseChooperOn << 5.8937E+01;
  qvecRefMFS06PhaseChooperOn << 5.2803E+01;
  qvecRefMFS06PhaseChooperOn << 4.6413E+01;
  qvecRefMFS06PhaseChooperOn << 3.9785E+01;
  qvecRefMFS06PhaseChooperOn << 3.3455E+01;
  qvecRefMFS06PhaseChooperOn << 2.7736E+01;
  qvecRefMFS06PhaseChooperOn << 2.2636E+01;
  qvecRefMFS06PhaseChooperOn << 1.8341E+01;
  qvecRefMFS06PhaseChooperOn << 1.4602E+01;
  qvecRefMFS06PhaseChooperOn << 1.1685E+01;
  qvecRefMFS06PhaseChooperOn << 9.2718E+00;
  qvecRefMFS06PhaseChooperOn << 7.3040E+00;
  qvecRefMFS06PhaseChooperOn << 5.7130E+00;
  qvecRefMFS06PhaseChooperOn << 3.9318E+00;
  qvecRefMFS06PhaseChooperOn << 3.3278E+00;
  qvecRefMFS06PhaseChooperOn << 2.3737E+00;
  qvecRefMFS06PhaseChooperOn << 1.5691E+00;
  qvecRefMFS06PhaseChooperOn << 9.4260E-01;
  qvecRefMFS06PhaseChooperOn << 4.0868E-01;
  qvecRefMFS06PhaseChooperOn << -3.8592E-01;
  qvecRefMFS06PhaseChooperOn << -1.0626E+00;
  qvecRefMFS06PhaseChooperOn << -1.7242E+00;
  qvecRefMFS06PhaseChooperOn << -2.5209E+00;
  qvecRefMFS06PhaseChooperOn << -3.3489E+00;
  qvecRefMFS06PhaseChooperOn << -4.4832E+00;
  qvecRefMFS06PhaseChooperOn << -5.8666E+00;
  qvecRefMFS06PhaseChooperOn << -7.5230E+00;
  qvecRefMFS06PhaseChooperOn << -9.6234E+00;
  qvecRefMFS06PhaseChooperOn << -1.2226E+01;
  qvecRefMFS06PhaseChooperOn << -1.5489E+01;
  qvecRefMFS06PhaseChooperOn << -1.9974E+01;
  qvecRefMFS06PhaseChooperOn << -2.5325E+01;
  qvecRefMFS06PhaseChooperOn << -3.1913E+01;
  qvecRefMFS06PhaseChooperOn << -3.8839E+01;
  qvecRefMFS06PhaseChooperOn << -4.2453E+01;
  qvecRefMFS06PhaseChooperOn << -4.6510E+01;
  qvecRefMFS06PhaseChooperOn << -5.4379E+01;
  qvecRefMFS06PhaseChooperOn << -5.4386E+01;

  /**
   * MFS06, MFS16e, MFS10e Chopper Off
   */
  qvecRefMFS06FreqChopperOff << 1.0000E+00;
  qvecRefMFS06FreqChopperOff << 1.2589E+00;
  qvecRefMFS06FreqChopperOff << 1.5849E+00;
  qvecRefMFS06FreqChopperOff << 1.9953E+00;
  qvecRefMFS06FreqChopperOff << 2.5119E+00;
  qvecRefMFS06FreqChopperOff << 3.1623E+00;
  qvecRefMFS06FreqChopperOff << 3.9811E+00;
  qvecRefMFS06FreqChopperOff << 5.0119E+00;
  qvecRefMFS06FreqChopperOff << 6.3095E+00;
  qvecRefMFS06FreqChopperOff << 7.9430E+00;
  qvecRefMFS06FreqChopperOff << 1.0000E+01;
  qvecRefMFS06FreqChopperOff << 1.2589E+01;
  qvecRefMFS06FreqChopperOff << 1.5849E+01;
  qvecRefMFS06FreqChopperOff << 1.9952E+01;
  qvecRefMFS06FreqChopperOff << 2.5119E+01;
  qvecRefMFS06FreqChopperOff << 3.1623E+01;
  qvecRefMFS06FreqChopperOff << 3.9810E+01;
  qvecRefMFS06FreqChopperOff << 5.0118E+01;
  qvecRefMFS06FreqChopperOff << 6.3095E+01;
  qvecRefMFS06FreqChopperOff << 7.9430E+01;
  qvecRefMFS06FreqChopperOff << 1.0000E+02;
  qvecRefMFS06FreqChopperOff << 1.2589E+02;
  qvecRefMFS06FreqChopperOff << 1.5849E+02;
  qvecRefMFS06FreqChopperOff << 1.9952E+02;
  qvecRefMFS06FreqChopperOff << 2.5119E+02;
  qvecRefMFS06FreqChopperOff << 3.1622E+02;
  qvecRefMFS06FreqChopperOff << 3.9810E+02;
  qvecRefMFS06FreqChopperOff << 5.0118E+02;
  qvecRefMFS06FreqChopperOff << 6.3095E+02;
  qvecRefMFS06FreqChopperOff << 7.9430E+02;
  qvecRefMFS06FreqChopperOff << 1.0000E+03;
  qvecRefMFS06FreqChopperOff << 1.2589E+03;
  qvecRefMFS06FreqChopperOff << 1.5849E+03;
  qvecRefMFS06FreqChopperOff << 1.9952E+03;
  qvecRefMFS06FreqChopperOff << 2.5118E+03;
  qvecRefMFS06FreqChopperOff << 3.1622E+03;
  qvecRefMFS06FreqChopperOff << 3.9810E+03;
  qvecRefMFS06FreqChopperOff << 5.0118E+03;
  qvecRefMFS06FreqChopperOff << 6.3094E+03;
  qvecRefMFS06FreqChopperOff << 7.9430E+03;
  qvecRefMFS06FreqChopperOff << 1.0000E+04;

  qvecRefMFS06MagChopperOff << 1.8514E-01;
  qvecRefMFS06MagChopperOff << 2.4828E-01;
  qvecRefMFS06MagChopperOff << 3.2123E-01;
  qvecRefMFS06MagChopperOff << 4.0127E-01;
  qvecRefMFS06MagChopperOff << 4.8392E-01;
  qvecRefMFS06MagChopperOff << 5.6295E-01;
  qvecRefMFS06MagChopperOff << 6.3176E-01;
  qvecRefMFS06MagChopperOff << 6.9034E-01;
  qvecRefMFS06MagChopperOff << 7.3436E-01;
  qvecRefMFS06MagChopperOff << 7.6720E-01;
  qvecRefMFS06MagChopperOff << 7.8931E-01;
  qvecRefMFS06MagChopperOff << 8.0430E-01;
  qvecRefMFS06MagChopperOff << 8.1508E-01;
  qvecRefMFS06MagChopperOff << 8.1949E-01;
  qvecRefMFS06MagChopperOff << 8.2493E-01;
  qvecRefMFS06MagChopperOff << 8.2713E-01;
  qvecRefMFS06MagChopperOff << 8.2892E-01;
  qvecRefMFS06MagChopperOff << 8.3391E-01;
  qvecRefMFS06MagChopperOff << 8.3027E-01;
  qvecRefMFS06MagChopperOff << 8.3084E-01;
  qvecRefMFS06MagChopperOff << 8.3087E-01;
  qvecRefMFS06MagChopperOff << 8.3158E-01;
  qvecRefMFS06MagChopperOff << 8.3065E-01;
  qvecRefMFS06MagChopperOff << 8.3160E-01;
  qvecRefMFS06MagChopperOff << 8.3247E-01;
  qvecRefMFS06MagChopperOff << 8.3128E-01;
  qvecRefMFS06MagChopperOff << 8.3124E-01;
  qvecRefMFS06MagChopperOff << 8.3121E-01;
  qvecRefMFS06MagChopperOff << 8.3065E-01;
  qvecRefMFS06MagChopperOff << 8.3060E-01;
  qvecRefMFS06MagChopperOff << 8.2861E-01;
  qvecRefMFS06MagChopperOff << 8.2637E-01;
  qvecRefMFS06MagChopperOff << 8.2364E-01;
  qvecRefMFS06MagChopperOff << 8.1757E-01;
  qvecRefMFS06MagChopperOff << 8.0845E-01;
  qvecRefMFS06MagChopperOff << 7.8603E-01;
  qvecRefMFS06MagChopperOff << 7.4831E-01;
  qvecRefMFS06MagChopperOff << 6.7860E-01;
  qvecRefMFS06MagChopperOff << 6.0407E-01;
  qvecRefMFS06MagChopperOff << 5.7499E-01;
  qvecRefMFS06MagChopperOff << 5.4592E-01;
  for (unsigned int uiCounter = 0; uiCounter < qvecRefMFS06MagChopperOff.size(); uiCounter++) {
    qvecRefMFS06MagChopperOff[uiCounter] = qvecRefMFS06MagChopperOff[uiCounter] / qvecRefMFS06FreqChopperOff[uiCounter];
  }

  qvecRefMFS06PhaseChooperOff << 1.1420E+02;
  qvecRefMFS06PhaseChooperOff << 1.0368E+02;
  qvecRefMFS06PhaseChooperOff << 9.2973E+01;
  qvecRefMFS06PhaseChooperOff << 8.2101E+01;
  qvecRefMFS06PhaseChooperOff << 7.1399E+01;
  qvecRefMFS06PhaseChooperOff << 6.1087E+01;
  qvecRefMFS06PhaseChooperOff << 5.1333E+01;
  qvecRefMFS06PhaseChooperOff << 4.2626E+01;
  qvecRefMFS06PhaseChooperOff << 3.4915E+01;
  qvecRefMFS06PhaseChooperOff << 2.8264E+01;
  qvecRefMFS06PhaseChooperOff << 2.2748E+01;
  qvecRefMFS06PhaseChooperOff << 1.8167E+01;
  qvecRefMFS06PhaseChooperOff << 1.4563E+01;
  qvecRefMFS06PhaseChooperOff << 1.1506E+01;
  qvecRefMFS06PhaseChooperOff << 9.1149E+00;
  qvecRefMFS06PhaseChooperOff << 7.1679E+00;
  qvecRefMFS06PhaseChooperOff << 5.5927E+00;
  qvecRefMFS06PhaseChooperOff << 4.2288E+00;
  qvecRefMFS06PhaseChooperOff << 3.2362E+00;
  qvecRefMFS06PhaseChooperOff << 2.3172E+00;
  qvecRefMFS06PhaseChooperOff << 1.5404E+00;
  qvecRefMFS06PhaseChooperOff << 9.1719E-01;
  qvecRefMFS06PhaseChooperOff << 2.2373E-01;
  qvecRefMFS06PhaseChooperOff << -3.6768E-01;
  qvecRefMFS06PhaseChooperOff << -1.0853E+00;
  qvecRefMFS06PhaseChooperOff << -1.7337E+00;
  qvecRefMFS06PhaseChooperOff << -2.3647E+00;
  qvecRefMFS06PhaseChooperOff << -3.3640E+00;
  qvecRefMFS06PhaseChooperOff << -4.5164E+00;
  qvecRefMFS06PhaseChooperOff << -5.8733E+00;
  qvecRefMFS06PhaseChooperOff << -7.5504E+00;
  qvecRefMFS06PhaseChooperOff << -9.6240E+00;
  qvecRefMFS06PhaseChooperOff << -1.2208E+01;
  qvecRefMFS06PhaseChooperOff << -1.5487E+01;
  qvecRefMFS06PhaseChooperOff << -1.9946E+01;
  qvecRefMFS06PhaseChooperOff << -2.5354E+01;
  qvecRefMFS06PhaseChooperOff << -3.1982E+01;
  qvecRefMFS06PhaseChooperOff << -3.8948E+01;
  qvecRefMFS06PhaseChooperOff << -4.2551E+01;
  qvecRefMFS06PhaseChooperOff << -4.6559E+01;
  qvecRefMFS06PhaseChooperOff << -5.4419E+01;

  /**
   * MFS07, MFS07e, MFS11e Chopper On
   */
  qvecRefMFS07FreqChopperOn << 5.0000E-05;
  qvecRefMFS07FreqChopperOn << 6.2946E-05;
  qvecRefMFS07FreqChopperOn << 7.9240E-05;
  qvecRefMFS07FreqChopperOn << 9.9759E-05;
  qvecRefMFS07FreqChopperOn << 1.2559E-04;
  qvecRefMFS07FreqChopperOn << 1.5811E-04;
  qvecRefMFS07FreqChopperOn << 1.9905E-04;
  qvecRefMFS07FreqChopperOn << 2.5059E-04;
  qvecRefMFS07FreqChopperOn << 3.1548E-04;
  qvecRefMFS07FreqChopperOn << 3.9716E-04;
  qvecRefMFS07FreqChopperOn << 5.0000E-04;
  qvecRefMFS07FreqChopperOn << 6.2946E-04;
  qvecRefMFS07FreqChopperOn << 7.9240E-04;
  qvecRefMFS07FreqChopperOn << 9.9759E-04;
  qvecRefMFS07FreqChopperOn << 1.2559E-03;
  qvecRefMFS07FreqChopperOn << 1.5811E-03;
  qvecRefMFS07FreqChopperOn << 1.9905E-03;
  qvecRefMFS07FreqChopperOn << 2.5059E-03;
  qvecRefMFS07FreqChopperOn << 3.1548E-03;
  qvecRefMFS07FreqChopperOn << 3.9716E-03;
  qvecRefMFS07FreqChopperOn << 5.0000E-03;
  qvecRefMFS07FreqChopperOn << 6.2946E-03;
  qvecRefMFS07FreqChopperOn << 7.9240E-03;
  qvecRefMFS07FreqChopperOn << 9.9759E-03;
  qvecRefMFS07FreqChopperOn << 1.2559E-02;
  qvecRefMFS07FreqChopperOn << 1.5811E-02;
  qvecRefMFS07FreqChopperOn << 1.9905E-02;
  qvecRefMFS07FreqChopperOn << 2.5059E-02;
  qvecRefMFS07FreqChopperOn << 3.1548E-02;
  qvecRefMFS07FreqChopperOn << 3.9716E-02;
  qvecRefMFS07FreqChopperOn << 5.0000E-02;
  qvecRefMFS07FreqChopperOn << 6.2946E-02;
  qvecRefMFS07FreqChopperOn << 7.9240E-02;
  qvecRefMFS07FreqChopperOn << 9.9759E-02;
  qvecRefMFS07FreqChopperOn << 1.2559E-01;
  qvecRefMFS07FreqChopperOn << 1.5811E-01;
  qvecRefMFS07FreqChopperOn << 1.9905E-01;
  qvecRefMFS07FreqChopperOn << 2.5059E-01;
  qvecRefMFS07FreqChopperOn << 3.1548E-01;
  qvecRefMFS07FreqChopperOn << 3.9716E-01;
  qvecRefMFS07FreqChopperOn << 5.0000E-01;
  qvecRefMFS07FreqChopperOn << 6.2946E-01;
  qvecRefMFS07FreqChopperOn << 7.9240E-01;
  qvecRefMFS07FreqChopperOn << 9.9759E-01;
  qvecRefMFS07FreqChopperOn << 1.2559E+00;
  qvecRefMFS07FreqChopperOn << 1.5811E+00;
  qvecRefMFS07FreqChopperOn << 1.9905E+00;
  qvecRefMFS07FreqChopperOn << 2.5059E+00;
  qvecRefMFS07FreqChopperOn << 3.1548E+00;
  qvecRefMFS07FreqChopperOn << 3.9716E+00;
  qvecRefMFS07FreqChopperOn << 5.0000E+00;
  qvecRefMFS07FreqChopperOn << 6.2946E+00;
  qvecRefMFS07FreqChopperOn << 7.9240E+00;
  qvecRefMFS07FreqChopperOn << 9.9760E+00;
  qvecRefMFS07FreqChopperOn << 1.2559E+01;
  qvecRefMFS07FreqChopperOn << 1.5811E+01;
  qvecRefMFS07FreqChopperOn << 1.9905E+01;
  qvecRefMFS07FreqChopperOn << 2.5059E+01;
  qvecRefMFS07FreqChopperOn << 3.1548E+01;
  qvecRefMFS07FreqChopperOn << 3.9716E+01;
  qvecRefMFS07FreqChopperOn << 4.9999E+01;
  qvecRefMFS07FreqChopperOn << 6.2946E+01;
  qvecRefMFS07FreqChopperOn << 7.9240E+01;
  qvecRefMFS07FreqChopperOn << 9.9760E+01;
  qvecRefMFS07FreqChopperOn << 1.2559E+02;
  qvecRefMFS07FreqChopperOn << 1.5811E+02;
  qvecRefMFS07FreqChopperOn << 1.9905E+02;
  qvecRefMFS07FreqChopperOn << 2.5059E+02;
  qvecRefMFS07FreqChopperOn << 3.1547E+02;
  qvecRefMFS07FreqChopperOn << 3.9716E+02;
  qvecRefMFS07FreqChopperOn << 4.9999E+02;
  qvecRefMFS07FreqChopperOn << 6.2945E+02;
  qvecRefMFS07FreqChopperOn << 7.9240E+02;
  qvecRefMFS07FreqChopperOn << 9.9760E+02;
  qvecRefMFS07FreqChopperOn << 1.2559E+03;
  qvecRefMFS07FreqChopperOn << 1.5811E+03;
  qvecRefMFS07FreqChopperOn << 1.9905E+03;
  qvecRefMFS07FreqChopperOn << 2.5059E+03;
  qvecRefMFS07FreqChopperOn << 3.1547E+03;
  qvecRefMFS07FreqChopperOn << 3.9716E+03;
  qvecRefMFS07FreqChopperOn << 4.9999E+03;
  qvecRefMFS07FreqChopperOn << 6.2945E+03;
  qvecRefMFS07FreqChopperOn << 7.9245E+03;
  qvecRefMFS07FreqChopperOn << 9.9760E+03;
  qvecRefMFS07FreqChopperOn << 1.2559E+04;
  qvecRefMFS07FreqChopperOn << 1.5811E+04;
  qvecRefMFS07FreqChopperOn << 1.9905E+04;
  qvecRefMFS07FreqChopperOn << 2.5058E+04;
  qvecRefMFS07FreqChopperOn << 3.1547E+04;
  qvecRefMFS07FreqChopperOn << 3.9715E+04;
  qvecRefMFS07FreqChopperOn << 4.9999E+04;
  qvecRefMFS07FreqChopperOn << 6.0000E+04;

  qvecRefMFS07MagChopperOn << 9.9950E-07;
  qvecRefMFS07MagChopperOn << 1.2583E-06;
  qvecRefMFS07MagChopperOn << 1.5840E-06;
  qvecRefMFS07MagChopperOn << 1.9942E-06;
  qvecRefMFS07MagChopperOn << 2.5105E-06;
  qvecRefMFS07MagChopperOn << 3.1606E-06;
  qvecRefMFS07MagChopperOn << 3.9790E-06;
  qvecRefMFS07MagChopperOn << 5.0093E-06;
  qvecRefMFS07MagChopperOn << 6.3064E-06;
  qvecRefMFS07MagChopperOn << 7.9392E-06;
  qvecRefMFS07MagChopperOn << 9.9950E-06;
  qvecRefMFS07MagChopperOn << 1.2583E-05;
  qvecRefMFS07MagChopperOn << 1.5840E-05;
  qvecRefMFS07MagChopperOn << 1.9942E-05;
  qvecRefMFS07MagChopperOn << 2.5105E-05;
  qvecRefMFS07MagChopperOn << 3.1606E-05;
  qvecRefMFS07MagChopperOn << 3.9790E-05;
  qvecRefMFS07MagChopperOn << 5.0093E-05;
  qvecRefMFS07MagChopperOn << 6.3064E-05;
  qvecRefMFS07MagChopperOn << 7.9392E-05;
  qvecRefMFS07MagChopperOn << 9.9950E-05;
  qvecRefMFS07MagChopperOn << 1.2583E-04;
  qvecRefMFS07MagChopperOn << 1.5840E-04;
  qvecRefMFS07MagChopperOn << 1.9942E-04;
  qvecRefMFS07MagChopperOn << 2.5105E-04;
  qvecRefMFS07MagChopperOn << 3.1606E-04;
  qvecRefMFS07MagChopperOn << 3.9790E-04;
  qvecRefMFS07MagChopperOn << 5.0093E-04;
  qvecRefMFS07MagChopperOn << 6.3064E-04;
  qvecRefMFS07MagChopperOn << 7.9392E-04;
  qvecRefMFS07MagChopperOn << 9.9950E-04;
  qvecRefMFS07MagChopperOn << 1.2583E-03;
  qvecRefMFS07MagChopperOn << 1.5840E-03;
  qvecRefMFS07MagChopperOn << 1.9942E-03;
  qvecRefMFS07MagChopperOn << 2.5105E-03;
  qvecRefMFS07MagChopperOn << 3.1606E-03;
  qvecRefMFS07MagChopperOn << 3.9790E-03;
  qvecRefMFS07MagChopperOn << 5.0093E-03;
  qvecRefMFS07MagChopperOn << 6.3064E-03;
  qvecRefMFS07MagChopperOn << 7.9392E-03;
  qvecRefMFS07MagChopperOn << 9.9950E-03;
  qvecRefMFS07MagChopperOn << 1.2577E-02;
  qvecRefMFS07MagChopperOn << 1.5846E-02;
  qvecRefMFS07MagChopperOn << 1.9953E-02;
  qvecRefMFS07MagChopperOn << 2.5138E-02;
  qvecRefMFS07MagChopperOn << 3.1616E-02;
  qvecRefMFS07MagChopperOn << 3.9673E-02;
  qvecRefMFS07MagChopperOn << 5.0008E-02;
  qvecRefMFS07MagChopperOn << 6.2919E-02;
  qvecRefMFS07MagChopperOn << 7.8979E-02;
  qvecRefMFS07MagChopperOn << 9.9480E-02;
  qvecRefMFS07MagChopperOn << 1.2375E-01;
  qvecRefMFS07MagChopperOn << 1.5266E-01;
  qvecRefMFS07MagChopperOn << 1.9127E-01;
  qvecRefMFS07MagChopperOn << 2.3553E-01;
  qvecRefMFS07MagChopperOn << 2.8588E-01;
  qvecRefMFS07MagChopperOn << 3.4219E-01;
  qvecRefMFS07MagChopperOn << 4.0092E-01;
  qvecRefMFS07MagChopperOn << 4.5950E-01;
  qvecRefMFS07MagChopperOn << 5.1158E-01;
  qvecRefMFS07MagChopperOn << 5.5624E-01;
  qvecRefMFS07MagChopperOn << 5.9340E-01;
  qvecRefMFS07MagChopperOn << 6.1886E-01;
  qvecRefMFS07MagChopperOn << 6.3624E-01;
  qvecRefMFS07MagChopperOn << 6.4822E-01;
  qvecRefMFS07MagChopperOn << 6.5614E-01;
  qvecRefMFS07MagChopperOn << 6.6232E-01;
  qvecRefMFS07MagChopperOn << 6.6569E-01;
  qvecRefMFS07MagChopperOn << 6.6536E-01;
  qvecRefMFS07MagChopperOn << 6.6663E-01;
  qvecRefMFS07MagChopperOn << 6.6734E-01;
  qvecRefMFS07MagChopperOn << 6.6697E-01;
  qvecRefMFS07MagChopperOn << 6.6745E-01;
  qvecRefMFS07MagChopperOn << 6.6931E-01;
  qvecRefMFS07MagChopperOn << 6.6907E-01;
  qvecRefMFS07MagChopperOn << 6.7028E-01;
  qvecRefMFS07MagChopperOn << 6.6974E-01;
  qvecRefMFS07MagChopperOn << 6.6968E-01;
  qvecRefMFS07MagChopperOn << 6.6899E-01;
  qvecRefMFS07MagChopperOn << 6.6782E-01;
  qvecRefMFS07MagChopperOn << 6.6579E-01;
  qvecRefMFS07MagChopperOn << 6.5721E-01;
  qvecRefMFS07MagChopperOn << 6.4390E-01;
  qvecRefMFS07MagChopperOn << 6.2176E-01;
  qvecRefMFS07MagChopperOn << 5.8334E-01;
  qvecRefMFS07MagChopperOn << 5.2847E-01;
  qvecRefMFS07MagChopperOn << 4.4299E-01;
  qvecRefMFS07MagChopperOn << 3.8000E-01;
  qvecRefMFS07MagChopperOn << 3.4064E-01;
  qvecRefMFS07MagChopperOn << 2.9209E-01;
  qvecRefMFS07MagChopperOn << 2.2496E-01;
  qvecRefMFS07MagChopperOn << 1.6416E-01;
  for (unsigned int uiCounter = 0; uiCounter < qvecRefMFS07MagChopperOn.size(); uiCounter++) {
    qvecRefMFS07MagChopperOn[uiCounter] = qvecRefMFS07MagChopperOn[uiCounter] / qvecRefMFS07FreqChopperOn[uiCounter];
  }

  qvecRefMFS07PhaseChooperOn << 9.0000E+01;
  qvecRefMFS07PhaseChooperOn << 9.0000E+01;
  qvecRefMFS07PhaseChooperOn << 9.0000E+01;
  qvecRefMFS07PhaseChooperOn << 9.0000E+01;
  qvecRefMFS07PhaseChooperOn << 9.0000E+01;
  qvecRefMFS07PhaseChooperOn << 9.0000E+01;
  qvecRefMFS07PhaseChooperOn << 9.0000E+01;
  qvecRefMFS07PhaseChooperOn << 9.0000E+01;
  qvecRefMFS07PhaseChooperOn << 9.0000E+01;
  qvecRefMFS07PhaseChooperOn << 9.0000E+01;
  qvecRefMFS07PhaseChooperOn << 9.0000E+01;
  qvecRefMFS07PhaseChooperOn << 9.0000E+01;
  qvecRefMFS07PhaseChooperOn << 9.0000E+01;
  qvecRefMFS07PhaseChooperOn << 9.0000E+01;
  qvecRefMFS07PhaseChooperOn << 9.0000E+01;
  qvecRefMFS07PhaseChooperOn << 9.0000E+01;
  qvecRefMFS07PhaseChooperOn << 9.0000E+01;
  qvecRefMFS07PhaseChooperOn << 9.0000E+01;
  qvecRefMFS07PhaseChooperOn << 9.0000E+01;
  qvecRefMFS07PhaseChooperOn << 9.0000E+01;
  qvecRefMFS07PhaseChooperOn << 9.0000E+01;
  qvecRefMFS07PhaseChooperOn << 8.9999E+01;
  qvecRefMFS07PhaseChooperOn << 8.9986E+01;
  qvecRefMFS07PhaseChooperOn << 8.9982E+01;
  qvecRefMFS07PhaseChooperOn << 8.9977E+01;
  qvecRefMFS07PhaseChooperOn << 8.9971E+01;
  qvecRefMFS07PhaseChooperOn << 8.9963E+01;
  qvecRefMFS07PhaseChooperOn << 8.9954E+01;
  qvecRefMFS07PhaseChooperOn << 8.9942E+01;
  qvecRefMFS07PhaseChooperOn << 8.9926E+01;
  qvecRefMFS07PhaseChooperOn << 8.9907E+01;
  qvecRefMFS07PhaseChooperOn << 8.9882E+01;
  qvecRefMFS07PhaseChooperOn << 8.9852E+01;
  qvecRefMFS07PhaseChooperOn << 8.9813E+01;
  qvecRefMFS07PhaseChooperOn << 8.9764E+01;
  qvecRefMFS07PhaseChooperOn << 8.9703E+01;
  qvecRefMFS07PhaseChooperOn << 8.9626E+01;
  qvecRefMFS07PhaseChooperOn << 8.9529E+01;
  qvecRefMFS07PhaseChooperOn << 8.9407E+01;
  qvecRefMFS07PhaseChooperOn << 8.9253E+01;
  qvecRefMFS07PhaseChooperOn << 8.9059E+01;
  qvecRefMFS07PhaseChooperOn << 8.8862E+01;
  qvecRefMFS07PhaseChooperOn << 8.8571E+01;
  qvecRefMFS07PhaseChooperOn << 8.8215E+01;
  qvecRefMFS07PhaseChooperOn << 8.7799E+01;
  qvecRefMFS07PhaseChooperOn << 8.7242E+01;
  qvecRefMFS07PhaseChooperOn << 8.6523E+01;
  qvecRefMFS07PhaseChooperOn << 8.5672E+01;
  qvecRefMFS07PhaseChooperOn << 8.4559E+01;
  qvecRefMFS07PhaseChooperOn << 8.3204E+01;
  qvecRefMFS07PhaseChooperOn << 8.1436E+01;
  qvecRefMFS07PhaseChooperOn << 7.9418E+01;
  qvecRefMFS07PhaseChooperOn << 7.6590E+01;
  qvecRefMFS07PhaseChooperOn << 7.3388E+01;
  qvecRefMFS07PhaseChooperOn << 6.9384E+01;
  qvecRefMFS07PhaseChooperOn << 6.4671E+01;
  qvecRefMFS07PhaseChooperOn << 5.9207E+01;
  qvecRefMFS07PhaseChooperOn << 5.3229E+01;
  qvecRefMFS07PhaseChooperOn << 4.6762E+01;
  qvecRefMFS07PhaseChooperOn << 4.0266E+01;
  qvecRefMFS07PhaseChooperOn << 3.5228E+01;
  qvecRefMFS07PhaseChooperOn << 2.7858E+01;
  qvecRefMFS07PhaseChooperOn << 2.2671E+01;
  qvecRefMFS07PhaseChooperOn << 1.8264E+01;
  qvecRefMFS07PhaseChooperOn << 1.4244E+01;
  qvecRefMFS07PhaseChooperOn << 1.1132E+01;
  qvecRefMFS07PhaseChooperOn << 8.5497E+00;
  qvecRefMFS07PhaseChooperOn << 6.5208E+00;
  qvecRefMFS07PhaseChooperOn << 4.6370E+00;
  qvecRefMFS07PhaseChooperOn << 3.0720E+00;
  qvecRefMFS07PhaseChooperOn << 1.6702E+00;
  qvecRefMFS07PhaseChooperOn << 4.1585E-01;
  qvecRefMFS07PhaseChooperOn << -7.7685E-01;
  qvecRefMFS07PhaseChooperOn << -2.2416E+00;
  qvecRefMFS07PhaseChooperOn << -3.6284E+00;
  qvecRefMFS07PhaseChooperOn << -5.4565E+00;
  qvecRefMFS07PhaseChooperOn << -7.5384E+00;
  qvecRefMFS07PhaseChooperOn << -9.9007E+00;
  qvecRefMFS07PhaseChooperOn << -1.2903E+01;
  qvecRefMFS07PhaseChooperOn << -1.6637E+01;
  qvecRefMFS07PhaseChooperOn << -2.1420E+01;
  qvecRefMFS07PhaseChooperOn << -2.7512E+01;
  qvecRefMFS07PhaseChooperOn << -3.4888E+01;
  qvecRefMFS07PhaseChooperOn << -4.4198E+01;
  qvecRefMFS07PhaseChooperOn << -5.5569E+01;
  qvecRefMFS07PhaseChooperOn << -6.8816E+01;
  qvecRefMFS07PhaseChooperOn << -8.1129E+01;
  qvecRefMFS07PhaseChooperOn << -9.2812E+01;
  qvecRefMFS07PhaseChooperOn << -1.0720E+02;
  qvecRefMFS07PhaseChooperOn << -1.3006E+02;
  qvecRefMFS07PhaseChooperOn << -1.5621E+02;
  qvecRefMFS07PhaseChooperOn << -1.7546E+02;

  /**
   * MFS07, MFS07e, MFS11e Chopper Off
   */
  qvecRefMFS07FreqChopperOff << 5.0000E-01;
  qvecRefMFS07FreqChopperOff << 6.2946E-01;
  qvecRefMFS07FreqChopperOff << 7.9240E-01;
  qvecRefMFS07FreqChopperOff << 9.9759E-01;
  qvecRefMFS07FreqChopperOff << 1.2559E+00;
  qvecRefMFS07FreqChopperOff << 1.5811E+00;
  qvecRefMFS07FreqChopperOff << 1.9905E+00;
  qvecRefMFS07FreqChopperOff << 2.5059E+00;
  qvecRefMFS07FreqChopperOff << 3.1548E+00;
  qvecRefMFS07FreqChopperOff << 3.9716E+00;
  qvecRefMFS07FreqChopperOff << 5.0000E+00;
  qvecRefMFS07FreqChopperOff << 6.2946E+00;
  qvecRefMFS07FreqChopperOff << 7.9240E+00;
  qvecRefMFS07FreqChopperOff << 9.9760E+00;
  qvecRefMFS07FreqChopperOff << 1.2559E+01;
  qvecRefMFS07FreqChopperOff << 1.5811E+01;
  qvecRefMFS07FreqChopperOff << 1.9905E+01;
  qvecRefMFS07FreqChopperOff << 2.5059E+01;
  qvecRefMFS07FreqChopperOff << 3.1548E+01;
  qvecRefMFS07FreqChopperOff << 3.9716E+01;
  qvecRefMFS07FreqChopperOff << 4.9999E+01;
  qvecRefMFS07FreqChopperOff << 6.2946E+01;
  qvecRefMFS07FreqChopperOff << 7.9240E+01;
  qvecRefMFS07FreqChopperOff << 9.9760E+01;
  qvecRefMFS07FreqChopperOff << 1.2559E+02;
  qvecRefMFS07FreqChopperOff << 1.5811E+02;
  qvecRefMFS07FreqChopperOff << 1.9905E+02;
  qvecRefMFS07FreqChopperOff << 2.5059E+02;
  qvecRefMFS07FreqChopperOff << 3.1547E+02;
  qvecRefMFS07FreqChopperOff << 3.9716E+02;
  qvecRefMFS07FreqChopperOff << 4.9999E+02;
  qvecRefMFS07FreqChopperOff << 6.2945E+02;
  qvecRefMFS07FreqChopperOff << 7.9240E+02;
  qvecRefMFS07FreqChopperOff << 9.9760E+02;
  qvecRefMFS07FreqChopperOff << 1.2559E+03;
  qvecRefMFS07FreqChopperOff << 1.5811E+03;
  qvecRefMFS07FreqChopperOff << 1.9905E+03;
  qvecRefMFS07FreqChopperOff << 2.5059E+03;
  qvecRefMFS07FreqChopperOff << 3.1547E+03;
  qvecRefMFS07FreqChopperOff << 3.9716E+03;
  qvecRefMFS07FreqChopperOff << 4.9999E+03;
  qvecRefMFS07FreqChopperOff << 6.2945E+03;
  qvecRefMFS07FreqChopperOff << 7.9245E+03;
  qvecRefMFS07FreqChopperOff << 9.9760E+03;
  qvecRefMFS07FreqChopperOff << 1.2559E+04;
  qvecRefMFS07FreqChopperOff << 1.5811E+04;
  qvecRefMFS07FreqChopperOff << 1.9905E+04;
  qvecRefMFS07FreqChopperOff << 2.5058E+04;
  qvecRefMFS07FreqChopperOff << 3.1547E+04;
  qvecRefMFS07FreqChopperOff << 3.9715E+04;
  qvecRefMFS07FreqChopperOff << 4.9999E+04;
  qvecRefMFS07FreqChopperOff << 6.0000E+04;

  qvecRefMFS07MagChopperOff << 5.9985E-03;
  qvecRefMFS07MagChopperOff << 8.7180E-03;
  qvecRefMFS07MagChopperOff << 1.2346E-02;
  qvecRefMFS07MagChopperOff << 1.6983E-02;
  qvecRefMFS07MagChopperOff << 2.2890E-02;
  qvecRefMFS07MagChopperOff << 3.0169E-02;
  qvecRefMFS07MagChopperOff << 3.9267E-02;
  qvecRefMFS07MagChopperOff << 5.0599E-02;
  qvecRefMFS07MagChopperOff << 6.4490E-02;
  qvecRefMFS07MagChopperOff << 8.1553E-02;
  qvecRefMFS07MagChopperOff << 1.0204E-01;
  qvecRefMFS07MagChopperOff << 1.3049E-01;
  qvecRefMFS07MagChopperOff << 1.6124E-01;
  qvecRefMFS07MagChopperOff << 2.0013E-01;
  qvecRefMFS07MagChopperOff << 2.4667E-01;
  qvecRefMFS07MagChopperOff << 2.9883E-01;
  qvecRefMFS07MagChopperOff << 3.5558E-01;
  qvecRefMFS07MagChopperOff << 4.1445E-01;
  qvecRefMFS07MagChopperOff << 4.7231E-01;
  qvecRefMFS07MagChopperOff << 5.2318E-01;
  qvecRefMFS07MagChopperOff << 5.5474E-01;
  qvecRefMFS07MagChopperOff << 5.9650E-01;
  qvecRefMFS07MagChopperOff << 6.2175E-01;
  qvecRefMFS07MagChopperOff << 6.3806E-01;
  qvecRefMFS07MagChopperOff << 6.4836E-01;
  qvecRefMFS07MagChopperOff << 6.5394E-01;
  qvecRefMFS07MagChopperOff << 6.6057E-01;
  qvecRefMFS07MagChopperOff << 6.6617E-01;
  qvecRefMFS07MagChopperOff << 6.6643E-01;
  qvecRefMFS07MagChopperOff << 6.6775E-01;
  qvecRefMFS07MagChopperOff << 6.6849E-01;
  qvecRefMFS07MagChopperOff << 6.6892E-01;
  qvecRefMFS07MagChopperOff << 6.6961E-01;
  qvecRefMFS07MagChopperOff << 6.7033E-01;
  qvecRefMFS07MagChopperOff << 6.7080E-01;
  qvecRefMFS07MagChopperOff << 6.7137E-01;
  qvecRefMFS07MagChopperOff << 6.7128E-01;
  qvecRefMFS07MagChopperOff << 6.7103E-01;
  qvecRefMFS07MagChopperOff << 6.7034E-01;
  qvecRefMFS07MagChopperOff << 6.6921E-01;
  qvecRefMFS07MagChopperOff << 6.6724E-01;
  qvecRefMFS07MagChopperOff << 6.5897E-01;
  qvecRefMFS07MagChopperOff << 6.4557E-01;
  qvecRefMFS07MagChopperOff << 6.2292E-01;
  qvecRefMFS07MagChopperOff << 5.8357E-01;
  qvecRefMFS07MagChopperOff << 5.2755E-01;
  qvecRefMFS07MagChopperOff << 4.4145E-01;
  qvecRefMFS07MagChopperOff << 3.7863E-01;
  qvecRefMFS07MagChopperOff << 3.4001E-01;
  qvecRefMFS07MagChopperOff << 2.9237E-01;
  qvecRefMFS07MagChopperOff << 2.2576E-01;
  qvecRefMFS07MagChopperOff << 1.6498E-01;
  for (unsigned int uiCounter = 0; uiCounter < qvecRefMFS07MagChopperOff.size(); uiCounter++) {
    qvecRefMFS07MagChopperOff[uiCounter] = qvecRefMFS07MagChopperOff[uiCounter] / qvecRefMFS07FreqChopperOff[uiCounter];
  }

  qvecRefMFS07PhaseChooperOff << 1.4395E+02;
  qvecRefMFS07PhaseChooperOff << 1.3746E+02;
  qvecRefMFS07PhaseChooperOff << 1.3064E+02;
  qvecRefMFS07PhaseChooperOff << 1.2388E+02;
  qvecRefMFS07PhaseChooperOff << 1.1743E+02;
  qvecRefMFS07PhaseChooperOff << 1.1169E+02;
  qvecRefMFS07PhaseChooperOff << 1.0631E+02;
  qvecRefMFS07PhaseChooperOff << 1.0157E+02;
  qvecRefMFS07PhaseChooperOff << 9.7255E+01;
  qvecRefMFS07PhaseChooperOff << 9.3201E+01;
  qvecRefMFS07PhaseChooperOff << 8.9289E+01;
  qvecRefMFS07PhaseChooperOff << 8.5241E+01;
  qvecRefMFS07PhaseChooperOff << 8.1136E+01;
  qvecRefMFS07PhaseChooperOff << 7.6663E+01;
  qvecRefMFS07PhaseChooperOff << 7.1633E+01;
  qvecRefMFS07PhaseChooperOff << 6.6083E+01;
  qvecRefMFS07PhaseChooperOff << 5.9868E+01;
  qvecRefMFS07PhaseChooperOff << 5.3183E+01;
  qvecRefMFS07PhaseChooperOff << 4.6352E+01;
  qvecRefMFS07PhaseChooperOff << 3.9289E+01;
  qvecRefMFS07PhaseChooperOff << 3.3168E+01;
  qvecRefMFS07PhaseChooperOff << 2.7121E+01;
  qvecRefMFS07PhaseChooperOff << 2.2051E+01;
  qvecRefMFS07PhaseChooperOff << 1.7779E+01;
  qvecRefMFS07PhaseChooperOff << 1.3900E+01;
  qvecRefMFS07PhaseChooperOff << 1.0846E+01;
  qvecRefMFS07PhaseChooperOff << 8.3592E+00;
  qvecRefMFS07PhaseChooperOff << 6.3756E+00;
  qvecRefMFS07PhaseChooperOff << 4.5833E+00;
  qvecRefMFS07PhaseChooperOff << 3.0286E+00;
  qvecRefMFS07PhaseChooperOff << 1.6566E+00;
  qvecRefMFS07PhaseChooperOff << 3.5741E-01;
  qvecRefMFS07PhaseChooperOff << -8.5764E-01;
  qvecRefMFS07PhaseChooperOff << -2.2476E+00;
  qvecRefMFS07PhaseChooperOff << -3.8996E+00;
  qvecRefMFS07PhaseChooperOff << -5.4530E+00;
  qvecRefMFS07PhaseChooperOff << -7.4840E+00;
  qvecRefMFS07PhaseChooperOff << -9.9130E+00;
  qvecRefMFS07PhaseChooperOff << -1.2903E+01;
  qvecRefMFS07PhaseChooperOff << -1.6627E+01;
  qvecRefMFS07PhaseChooperOff << -2.1404E+01;
  qvecRefMFS07PhaseChooperOff << -2.7512E+01;
  qvecRefMFS07PhaseChooperOff << -3.4940E+01;
  qvecRefMFS07PhaseChooperOff << -4.4289E+01;
  qvecRefMFS07PhaseChooperOff << -5.5670E+01;
  qvecRefMFS07PhaseChooperOff << -6.8863E+01;
  qvecRefMFS07PhaseChooperOff << -8.1182E+01;
  qvecRefMFS07PhaseChooperOff << -9.2706E+01;
  qvecRefMFS07PhaseChooperOff << -1.0697E+02;
  qvecRefMFS07PhaseChooperOff << -1.2979E+02;
  qvecRefMFS07PhaseChooperOff << -1.5601E+02;
  qvecRefMFS07PhaseChooperOff << -1.7535E+02;
}

void Plot::on_pbAutoScale_clicked(void) {
  ui->plotMagHx->rescaleAxes();
  ui->plotMagHy->rescaleAxes();
  ui->plotMagHz->rescaleAxes();

  ui->plotPhaseHx->rescaleAxes();
  ui->plotPhaseHy->rescaleAxes();
  ui->plotPhaseHz->rescaleAxes();

  ui->plotMagHx->replot();
  ui->plotMagHy->replot();
  ui->plotMagHz->replot();

  ui->plotPhaseHx->replot();
  ui->plotPhaseHy->replot();
  ui->plotPhaseHz->replot();
}

void Plot::on_pbPrint_clicked(void) {
  int iRetValue;

  int iTop;
  int iLeft;
  int iHeight;
  int iWidth;

  QRectF clrect;

  QPrintDialog *pclPrintDialog = new QPrintDialog();

  iRetValue = pclPrintDialog->exec();
  if (iRetValue == 1) {
    clrect = pclPrintDialog->printer()->pageRect(QPrinter::Unit::Millimeter);
    iTop = clrect.top();
    iLeft = clrect.left();

    iHeight = clrect.height() - 100;
    iWidth = clrect.width();

    QPainter painter;
    painter.begin(pclPrintDialog->printer());
    painter.drawPixmap(iLeft, iTop, ui->plotMagHx->toPixmap(iWidth, iHeight / 2));
    painter.drawPixmap(iLeft, iTop + (iHeight / 2) + 5, ui->plotPhaseHx->toPixmap(iWidth, iHeight / 2));
    painter.drawText(iLeft, iTop + (iHeight / 2) * 2 + 10, "   " + this->windowTitle());

    pclPrintDialog->printer()->newPage();

    painter.drawPixmap(iLeft, iTop, ui->plotMagHy->toPixmap(iWidth, iHeight / 2));
    painter.drawPixmap(iLeft, iTop + (iHeight / 2) + 5, ui->plotPhaseHy->toPixmap(iWidth, iHeight / 2));
    painter.drawText(iLeft, iTop + (iHeight / 2) * 2 + 10, "   " + this->windowTitle());

    pclPrintDialog->printer()->newPage();

    painter.drawPixmap(iLeft, iTop, ui->plotMagHz->toPixmap(iWidth, iHeight / 2));
    painter.drawPixmap(iLeft, iTop + (iHeight / 2) + 5, ui->plotPhaseHz->toPixmap(iWidth, iHeight / 2));
    painter.drawText(iLeft, iTop + (iHeight / 2) * 2 + 10, "   " + this->windowTitle());

    painter.end();
  }

  delete (pclPrintDialog);
}

void Plot::on_pbShowDatapoints_clicked(void) {
  if (ui->pbShowDatapoints->isChecked() == true) {
    ui->plotMagHx->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::blue, 10.0));
    ui->plotMagHy->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::blue, 10.0));
    ui->plotMagHz->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::blue, 10.0));

    ui->plotPhaseHx->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::blue, 10.0));
    ui->plotPhaseHy->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::blue, 10.0));
    ui->plotPhaseHz->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::blue, 10.0));
  } else {
    ui->plotMagHx->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone, Qt::blue, 10.0));
    ui->plotMagHy->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone, Qt::blue, 10.0));
    ui->plotMagHz->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone, Qt::blue, 10.0));

    ui->plotPhaseHx->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone, Qt::blue, 10.0));
    ui->plotPhaseHy->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone, Qt::blue, 10.0));
    ui->plotPhaseHz->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone, Qt::blue, 10.0));
  }

  ui->plotMagHx->replot();
  ui->plotMagHy->replot();
  ui->plotMagHz->replot();

  ui->plotPhaseHx->replot();
  ui->plotPhaseHy->replot();
  ui->plotPhaseHz->replot();
}

void Plot::on_pbShowCurve_clicked(void) {
  if (ui->pbShowCurve->isChecked() == true) {
    ui->plotMagHx->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->plotMagHy->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->plotMagHz->graph(0)->setLineStyle(QCPGraph::lsLine);

    ui->plotPhaseHx->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->plotPhaseHy->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->plotPhaseHz->graph(0)->setLineStyle(QCPGraph::lsLine);
  } else {
    ui->plotMagHx->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->plotMagHy->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->plotMagHz->graph(0)->setLineStyle(QCPGraph::lsNone);

    ui->plotPhaseHx->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->plotPhaseHy->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->plotPhaseHz->graph(0)->setLineStyle(QCPGraph::lsNone);
  }

  ui->plotMagHx->replot();
  ui->plotMagHy->replot();
  ui->plotMagHz->replot();

  ui->plotPhaseHx->replot();
  ui->plotPhaseHy->replot();
  ui->plotPhaseHz->replot();
}
