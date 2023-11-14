#include "mainwindow.h"
#include "GlobalIncludes.h"
#include "atsfile.h"
#include "measdocxml.h"
#include "ui_mainwindow.h"
#include "utils.h"

#include "QDateTime"
#include "QDebug"
#include "QFileDialog"
#include "QMessageBox"
#include "QProgressDialog"

#include "cmath"
#include "complex.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288419716939937510
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
  QString qstrTmp;

  ui->setupUi(this);

  pclPlot = NULL;

  pclSettings = new QSettings("Metronix", "In-Situ Coil Calibration Tool");

  // restore geometry
  pclSettings->beginGroup(this->objectName());
  this->restoreGeometry(pclSettings->value("geometry").toByteArray());
  this->restoreState(pclSettings->value("state").toByteArray());
  pclSettings->endGroup();

  // load last TS data dir
  pclSettings->beginGroup("TSData");
  this->qstrLastTSDir = pclSettings->value("LastTSDir").toString();
  pclSettings->endGroup();

  qstrVersion = QString::number(C_VERSION_MAIN);
  qstrVersion += ".";
  qstrVersion += QString::number(C_VERSION_SUB);
  qstrVersion += ".";
  qstrVersion += QString::number(C_VERSION_CUSTOMER);
  qstrVersion += ".";
  qstrVersion += QString::number(C_VERSION_KM_MAIN);
  qstrVersion += ".";

  qstrTmp = C_VERSION_KM_SUB;
  qstrTmp = qstrTmp.right(qstrTmp.size() - (qstrTmp.indexOf(":") + 2));
  qstrTmp = qstrTmp.left(qstrTmp.size() - 1);

  qstrVersion += qstrTmp;

  qstrTmp = "Metronix In-Situ Coil Calibration Tool - ";
  qstrTmp += qstrVersion;

  this->setWindowTitle(qstrTmp);

  this->ui->pbSave->setVisible(false);
}

MainWindow::~MainWindow() {
  if (pclPlot != NULL) {
    delete (pclPlot);
  }

  // save geometry
  pclSettings->beginGroup(this->objectName());
  pclSettings->setValue("geometry", this->saveGeometry());
  pclSettings->setValue("state", this->saveState());
  pclSettings->endGroup();

  // save last TS die
  pclSettings->beginGroup("TSData");
  pclSettings->setValue("LastTSDir", this->qstrLastTSDir);
  pclSettings->endGroup();

  if (this->fft_object != nullptr)
    delete this->fft_object;
  delete ui;
}

void MainWindow::on_pbOpenData_clicked(void) {
  // declaration of variables
  QString qstrTSSourceDir;
  QStringList qstrlTSSourceDirs;
  QStringList qstrlATSFiles;
  QStringList qstrlXMLFiles;
  QDir qdirTSSourceDir;
  unsigned int uiDirCount;
  unsigned int uiATSCount;
  QString qstrTemp;

  qstrTSSourceDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), qstrLastTSDir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  // now search for TS directories inside this directory.
  qdirTSSourceDir.setPath(qstrTSSourceDir);
  qstrlTSSourceDirs.clear();
  qstrlTSSourceDirs.append("meas_*");
  qstrlTSSourceDirs = qdirTSSourceDir.entryList(qstrlTSSourceDirs);

  qDebug() << "[" << __PRETTY_FUNCTION__ << "] TS dirs:" << qstrlTSSourceDirs;

  if (qstrlTSSourceDirs.size() == 0) {
    QMessageBox::warning(this, "Open TS", "No Time Series data found inside this directory!");
  } else {
    qstrLastTSDir = qstrTSSourceDir;

    // now go through all the directories and search for TS data files.
    for (uiDirCount = 0; uiDirCount < qstrlTSSourceDirs.size(); uiDirCount++) {
      qstrlTSSourceDirs[uiDirCount] = qstrTSSourceDir + "/" + qstrlTSSourceDirs[uiDirCount];

      qdirTSSourceDir.setPath(qstrlTSSourceDirs[uiDirCount]);
      qstrlATSFiles.append("*.ats");
      qstrlATSFiles = qdirTSSourceDir.entryList(qstrlATSFiles);

      qDebug() << "[" << __PRETTY_FUNCTION__ << "] ATS inside:" << qstrlTSSourceDirs[uiDirCount];
      qDebug() << "[" << __PRETTY_FUNCTION__ << "] " << qstrlATSFiles;

      ui->tableTSFiles->insertRow(uiDirCount);

      ui->tableTSFiles->setItem(uiDirCount, 0, new QTableWidgetItem(""));
      ui->tableTSFiles->item(uiDirCount, 0)->setCheckState(Qt::Checked);
      ui->tableTSFiles->setItem(uiDirCount, 8, new QTableWidgetItem(qstrlTSSourceDirs[uiDirCount]));

      // get calibration frequency from XML file (is only stored inside XML file)
      qstrlXMLFiles.append("*.xml");
      qstrlXMLFiles = qdirTSSourceDir.entryList(qstrlXMLFiles);
      qstrTemp = qstrlTSSourceDirs[uiDirCount];
      qstrTemp += "/";
      qstrTemp += qstrlXMLFiles[0];
      MeasDocXml clMeasDocXml(qstrTemp);
      if (clMeasDocXml.isOpen() == true) {
        ui->tableTSFiles->setItem(uiDirCount, 2, new QTableWidgetItem(QString::number(clMeasDocXml.getCalFreq())));
      }

      for (uiATSCount = 0; uiATSCount < qstrlATSFiles.size(); uiATSCount++) {
        qstrTemp = qstrlTSSourceDirs[uiDirCount];
        qstrTemp += "/";
        qstrTemp += qstrlATSFiles[uiATSCount];
        ATSFile clATSFile(qstrTemp);
        if (clATSFile.isOpen() == true) {
          qstrTemp = clATSFile.getSensorType();
          qstrTemp += "/" + clATSFile.getSensorSerial();

          if (qstrlATSFiles[uiATSCount].contains("Ex") == true) {
            ui->tableTSFiles->setItem(uiDirCount, 1, new QTableWidgetItem(QString::number(clATSFile.clATSHeader.rSampleFreq)));
            ui->tableTSFiles->setItem(uiDirCount, 3, new QTableWidgetItem(qstrTemp));
          }

          if (qstrlATSFiles[uiATSCount].contains("Ey") == true) {
            ui->tableTSFiles->setItem(uiDirCount, 4, new QTableWidgetItem(qstrTemp));
          }

          if (qstrlATSFiles[uiATSCount].contains("Hx") == true) {
            ui->tableTSFiles->setItem(uiDirCount, 5, new QTableWidgetItem(qstrTemp));
          }

          if (qstrlATSFiles[uiATSCount].contains("Hy") == true) {
            ui->tableTSFiles->setItem(uiDirCount, 6, new QTableWidgetItem(qstrTemp));
          }

          if (qstrlATSFiles[uiATSCount].contains("Hz") == true) {
            ui->tableTSFiles->setItem(uiDirCount, 7, new QTableWidgetItem(qstrTemp));
          }
        }
      }
    }
  }
}

bool MainWindow::computeTF(QVector<QVector<QVector<QString>>> &qvecTFHeader, QVector<QVector<QVector<QVector<double>>>> &qvecTFData,
                           const QString qstrATSRef, const QString qstrATSCal, const QString qstrSensorType, const QString qstrInput,
                           const double dSampleFreq, const double dCalFreq, const double dNumHarmonics,
                           const unsigned int uiSetIndex, const unsigned int uiCHIndex) {
  bool bRetValue = true;
  bool bEOL;

  ATSFile qfATSRef(qstrATSRef);
  ATSFile qfATSCal(qstrATSCal);

  double dWindowSize;
  double dNumSamples;

  QVector<double> qvecTSRef;
  QVector<double> qvecTSCal;

  QVector<double> qvecFFTRefMag;
  QVector<double> qvecFFTRefPhase;
  QVector<double> qvecFFTRefFreq;

  QVector<double> qvecFFTCalMag;
  QVector<double> qvecFFTCalPhase;
  QVector<double> qvecFFTCalFreq;

  QVector<double> qvecHarmRefMag;
  QVector<double> qvecHarmRefPhase;
  QVector<double> qvecHarmRefFreq;

  QVector<double> qvecHarmCalMag;
  QVector<double> qvecHarmCalPhase;
  QVector<double> qvecHarmCalFreq;

  QVector<double> qvecResultFreq;
  QVector<double> qvecResultMag;
  QVector<double> qvecResultPhase;

  QVector<QString> qvecHeader;

  double dStackCount;

  QString qstrTemp;

  unsigned int uiCounter;
  unsigned int uiHarmCount;

  double dFreq;

  // add header information
  qvecHeader.clear();
  qvecHeader.push_back(QString::number(dSampleFreq) + "Hz");
  qvecHeader.push_back(QString::number(dCalFreq) + "Hz");
  qvecHeader.push_back(qstrSensorType);
  qvecHeader.push_back(qstrInput);

  // open the 2 ATS files (ref and cal)
  if (qfATSRef.isOpen() == false) {
    bRetValue = false;
  }

  if (qfATSCal.isOpen() == false) {
    bRetValue = false;
  } else {
    if (qfATSCal.clATSHeader.byChopper == 0) {
      qvecHeader.push_back("Chopper Off");
    } else {
      qvecHeader.push_back("Chopper On");
    }
  }
  qvecTFHeader[uiSetIndex][uiCHIndex] = qvecHeader;

  if (qfATSCal.clATSHeader.iSamples != qfATSRef.clATSHeader.iSamples) {
    bRetValue = false;
  }

  if (bRetValue == true) {
    // compute window size and number of samples
    if (dCalFreq < 1.0) {
      dWindowSize = (1.0 / dCalFreq) * 2.0 * dSampleFreq;
    } else {
      dWindowSize = dSampleFreq * 4.0;
    }
    dNumSamples = qfATSRef.clATSHeader.iSamples;

    qstrTemp = "Computing TF - ";
    qstrTemp += qstrSensorType + " - Calibration Frequency: ";
    qstrTemp += QString::number(dCalFreq) + "Hz";

    QProgressDialog clProgress(qstrTemp, "Abort", 0, 0, NULL);
    clProgress.setWindowModality(Qt::WindowModal);
    clProgress.setMaximum((floor(dNumSamples / dWindowSize)));
    clProgress.setMinimum(0);
    clProgress.setValue(0);
    clProgress.setFocus();
    clProgress.setFixedWidth(350);
    clProgress.setVisible(true);

    bEOL = false;
    qvecResultFreq.clear();
    qvecResultMag.clear();
    qvecResultPhase.clear();
    for (dStackCount = 0; dStackCount < (floor(dNumSamples / dWindowSize)); dStackCount = dStackCount + 1.0) {
      clProgress.setValue(dStackCount);

      this->repaint();
      clProgress.repaint();

      if (qfATSRef.getNextTSBuffer(dWindowSize, qvecTSRef) == false) {
        bEOL = true;
        break;
      }

      if (qfATSCal.getNextTSBuffer(dWindowSize, qvecTSCal) == false) {
        bEOL = true;
        break;
      }

      /*
      dumpTextFile ("./BufferRef.txt", qvecTSRef);
      dumpTextFile ("./BufferCal.txt", qvecTSCal);
      */

      if (bEOL == false) {
        // now compute FFT
        if (this->computeFFT(qvecTSRef, qvecFFTRefMag, qvecFFTRefPhase, qvecFFTRefFreq, (unsigned int)dWindowSize, dSampleFreq) == false) {
          bRetValue = false;
        }

        if (this->computeFFT(qvecTSCal, qvecFFTCalMag, qvecFFTCalPhase, qvecFFTCalFreq, (unsigned int)dWindowSize, dSampleFreq) == false) {
          bRetValue = false;
        }

        /*
        dumpTextFile ("./FFTRefMag.txt",   qvecFFTRefFreq, qvecFFTRefMag);
        dumpTextFile ("./FFTRefPhase.txt", qvecFFTRefFreq, qvecFFTRefPhase);

        dumpTextFile ("./FFTCalMag.txt",   qvecFFTCalFreq, qvecFFTCalMag);
        dumpTextFile ("./FFTCalPhase.txt", qvecFFTCalFreq, qvecFFTCalPhase);
        */

        if (bRetValue == true) {
          // reduce the FFT spectra to the calibration frequency and hamronics
          qvecHarmRefMag.clear();
          qvecHarmRefPhase.clear();
          qvecHarmRefFreq.clear();

          qvecHarmCalMag.clear();
          qvecHarmCalPhase.clear();
          qvecHarmCalFreq.clear();

          uiHarmCount = 0;
          dFreq = dCalFreq;
          for (uiCounter = 0; uiCounter < qvecFFTRefFreq.size(); uiCounter++) {
            if (dFreq == qvecFFTRefFreq[uiCounter]) {
              if (uiHarmCount < dNumHarmonics) {
                qvecHarmRefMag.push_back(qvecFFTRefMag[uiCounter]);
                qvecHarmRefPhase.push_back(qvecFFTRefPhase[uiCounter]);
                qvecHarmRefFreq.push_back(qvecFFTRefFreq[uiCounter]);

                qvecHarmCalMag.push_back(qvecFFTCalMag[uiCounter]);
                qvecHarmCalPhase.push_back(qvecFFTCalPhase[uiCounter]);
                qvecHarmCalFreq.push_back(qvecFFTCalFreq[uiCounter]);

                dFreq = dCalFreq * (3 + (uiHarmCount * 2));
                uiHarmCount++;
              } else {
                break;
              }
            }
          }

          /*
          dumpTextFile ("./FFTRefMag.txt",   qvecHarmRefFreq, qvecHarmRefMag);
          dumpTextFile ("./FFTRefPhase.txt", qvecHarmRefFreq, qvecHarmRefPhase);

          dumpTextFile ("./FFTCalMag.txt",   qvecHarmCalFreq, qvecHarmCalMag);
          dumpTextFile ("./FFTCalPhase.txt", qvecHarmCalFreq, qvecHarmCalPhase);
          */

          if (qvecResultFreq.size() < qvecHarmRefFreq.size()) {
            qvecResultFreq.resize(qvecHarmRefFreq.size());
            qvecResultMag.resize(qvecHarmRefFreq.size());
            qvecResultPhase.resize(qvecHarmRefFreq.size());
          }

          for (uiCounter = 0; uiCounter < qvecHarmRefFreq.size(); uiCounter++) {
            double dAmpl;
            double dPhase;

            dAmpl = qvecHarmCalMag[uiCounter] / qvecHarmRefMag[uiCounter];
            dAmpl = computeTFAmpl(dAmpl, qvecHarmRefFreq[uiCounter], qstrSensorType);

            if (qvecHarmCalPhase[uiCounter] >= qvecHarmRefPhase[uiCounter]) {
              dPhase = qvecHarmCalPhase[uiCounter] - qvecHarmRefPhase[uiCounter];
            } else {
              dPhase = qvecHarmCalPhase[uiCounter] - qvecHarmRefPhase[uiCounter] + M_PI;
            }
            dPhase = (dPhase / (2.0 * M_PI)) * 360.0;

            qvecResultFreq[uiCounter] = qvecHarmRefFreq[uiCounter];
            qvecResultMag[uiCounter] += dAmpl;
            qvecResultPhase[uiCounter] += dPhase;
          }
        }
      }
    }

    for (uiCounter = 0; uiCounter < qvecResultFreq.size(); uiCounter++) {
      qvecResultMag[uiCounter] /= dStackCount;
      qvecResultPhase[uiCounter] /= dStackCount;

      // correct phase shift (range is +/- 180°
      if (uiCounter >= 1) {
        if ((qvecResultPhase[uiCounter] - qvecResultPhase[uiCounter - 1]) >= 100.0) {
          qvecResultPhase[uiCounter] -= 180.0;
        } else {
          if ((qvecResultFreq[uiCounter] > 100.0) && (qvecResultPhase[uiCounter] > 100.0)) {
            qvecResultPhase[uiCounter] -= 180.0;
          }
        }
      } else {
        if ((qvecResultFreq[uiCounter] > 100.0) && (qvecResultPhase[uiCounter] > 100.0)) {
          qvecResultPhase[uiCounter] -= 180.0;
        }
      }
    }

    qstrTemp = ui->tableTSFiles->item(uiSetIndex, 8)->text() + "/../TFMag_" + qstrInput + "_" + QString::number(uiSetIndex) + "_" + QString::number(dSampleFreq) + "Hz.txt";
    dumpTextFile(qstrTemp, qvecResultFreq, qvecResultMag);

    qstrTemp = ui->tableTSFiles->item(uiSetIndex, 8)->text() + "/../TFPhase_" + qstrInput + "_" + QString::number(uiSetIndex) + "_" + QString::number(dSampleFreq) + "Hz.txt";
    dumpTextFile(qstrTemp, qvecResultFreq, qvecResultPhase);

    qvecTFData[uiSetIndex][uiCHIndex][0] = qvecResultFreq;
    qvecTFData[uiSetIndex][uiCHIndex][1] = qvecResultMag;
    qvecTFData[uiSetIndex][uiCHIndex][2] = qvecResultPhase;
  }

  return (bRetValue);
}

void MainWindow::on_pbComputeTF_clicked(void) {
  unsigned int uiTSSetCounter;
  QString qstrTemp1;
  QString qstrTemp2;
  QStringList qstrlATSFiles;
  QDir qdirTargetTSDir;

  QVector<QVector<double>> qvec1CHData;
  QVector<QVector<QVector<double>>> qvecMultiCHData;

  QVector<QVector<QString>> qvec1SetHeader;

  this->qvecTFData.clear();

  qvec1CHData.resize(3);
  qvecMultiCHData.push_back(qvec1CHData);
  qvecMultiCHData.push_back(qvec1CHData);
  qvecMultiCHData.push_back(qvec1CHData);

  this->qvecTFHeader.clear();
  qvec1SetHeader.resize(3);

  for (uiTSSetCounter = 0; uiTSSetCounter < ui->tableTSFiles->rowCount(); uiTSSetCounter++) {
    this->qvecTFData.push_back(qvecMultiCHData);
    this->qvecTFHeader.push_back(qvec1SetHeader);

    if (ui->tableTSFiles->item(uiTSSetCounter, 0)->checkState() == Qt::Checked) {

      // compute Hx transfer function
      qdirTargetTSDir.setPath(ui->tableTSFiles->item(uiTSSetCounter, 8)->text());
      qstrlATSFiles.clear();
      qstrlATSFiles.append("*Ex*.ats");
      qstrlATSFiles = qdirTargetTSDir.entryList(qstrlATSFiles);
      qstrTemp1 = ui->tableTSFiles->item(uiTSSetCounter, 8)->text();
      qstrTemp1 += "/" + qstrlATSFiles[0];

      qstrlATSFiles.clear();
      qstrlATSFiles.append("*Hx*.ats");
      qstrlATSFiles = qdirTargetTSDir.entryList(qstrlATSFiles);
      qstrTemp2 = ui->tableTSFiles->item(uiTSSetCounter, 8)->text();
      qstrTemp2 += "/" + qstrlATSFiles[0];

      this->computeTF(this->qvecTFHeader, this->qvecTFData, qstrTemp1, qstrTemp2,
                      ui->tableTSFiles->item(uiTSSetCounter, 5)->text(), "Hx",
                      ui->tableTSFiles->item(uiTSSetCounter, 1)->text().toDouble(),
                      ui->tableTSFiles->item(uiTSSetCounter, 2)->text().toDouble(),
                      (double)ui->spNumHarmonics->value(), uiTSSetCounter, 0);

      // compute Hy transfer function
      qdirTargetTSDir.setPath(ui->tableTSFiles->item(uiTSSetCounter, 8)->text());
      qstrlATSFiles.clear();
      qstrlATSFiles.append("*Ex*.ats");
      qstrlATSFiles = qdirTargetTSDir.entryList(qstrlATSFiles);
      qstrTemp1 = ui->tableTSFiles->item(uiTSSetCounter, 8)->text();
      qstrTemp1 += "/" + qstrlATSFiles[0];

      qstrlATSFiles.clear();
      qstrlATSFiles.append("*Hy*.ats");
      qstrlATSFiles = qdirTargetTSDir.entryList(qstrlATSFiles);
      qstrTemp2 = ui->tableTSFiles->item(uiTSSetCounter, 8)->text();
      qstrTemp2 += "/" + qstrlATSFiles[0];

      this->computeTF(this->qvecTFHeader, this->qvecTFData, qstrTemp1, qstrTemp2,
                      ui->tableTSFiles->item(uiTSSetCounter, 6)->text(), "Hy",
                      ui->tableTSFiles->item(uiTSSetCounter, 1)->text().toDouble(),
                      ui->tableTSFiles->item(uiTSSetCounter, 2)->text().toDouble(),
                      (double)ui->spNumHarmonics->value(), uiTSSetCounter, 1);

      // compute Hz transfer function
      qdirTargetTSDir.setPath(ui->tableTSFiles->item(uiTSSetCounter, 8)->text());
      qstrlATSFiles.clear();
      qstrlATSFiles.append("*Ex*.ats");
      qstrlATSFiles = qdirTargetTSDir.entryList(qstrlATSFiles);
      qstrTemp1 = ui->tableTSFiles->item(uiTSSetCounter, 8)->text();
      qstrTemp1 += "/" + qstrlATSFiles[0];

      qstrlATSFiles.clear();
      qstrlATSFiles.append("*Hz*.ats");
      qstrlATSFiles = qdirTargetTSDir.entryList(qstrlATSFiles);
      qstrTemp2 = ui->tableTSFiles->item(uiTSSetCounter, 8)->text();
      qstrTemp2 += "/" + qstrlATSFiles[0];

      this->computeTF(this->qvecTFHeader, this->qvecTFData, qstrTemp1, qstrTemp2,
                      ui->tableTSFiles->item(uiTSSetCounter, 7)->text(), "Hz",
                      ui->tableTSFiles->item(uiTSSetCounter, 1)->text().toDouble(),
                      ui->tableTSFiles->item(uiTSSetCounter, 2)->text().toDouble(),
                      (double)ui->spNumHarmonics->value(), uiTSSetCounter, 2);
    }
  }

  // show data on plot
  if (pclPlot != NULL) {
    delete (pclPlot);
  }
  pclPlot = new Plot(this);
  qstrTemp1 = "Metronix In-Situ Coil Calibration Tool - ";
  qstrTemp1 += qstrVersion;
  pclPlot->setWindowTitle(qstrTemp1);
  pclPlot->showData(this->qvecTFHeader, this->qvecTFData, ui->cbRawTransferFunction->isChecked());
  pclPlot->show();

  qDebug() << "finished";
}

bool MainWindow::computeFFT(const QVector<double> qvecInputData, QVector<double> &qvecOutputMag, QVector<double> &qvecOutputPhase, QVector<double> &qvecOutputFreqList, const unsigned int uiWindowSize, const double dSampleFreq) {
  // declaration of variables
  bool bRetValue = true;
  unsigned int uiCounter;
  double dAmpl;
  double dPhase;
  std::complex<double> cmplTemp;

  qvecOutputMag.clear();
  qvecOutputPhase.clear();
  qvecOutputFreqList.clear();

  if (this->uiwsize != qvecInputData.size()) {
    if (this->fft_object != nullptr)
      delete this->fft_object;
    this->fft_object = new ffft::FFTReal<double>(qvecInputData.size());
    this->halfcomplex.resize(qvecInputData.size());
    this->uiwsize = qvecInputData.size();
  }

  this->halfcomplex.assign(halfcomplex.size(), 0.0);

  // create input vector from qvecInputData
  std::vector<double> input(qvecInputData.size());
  std::copy(qvecInputData.begin(), qvecInputData.end(), input.begin());

  // input.reserve(qvecInputData.size());
  // for (const auto &v : qvecInputData)
  //   input.push_back(v);

  this->fft_object->do_fft(&halfcomplex[0], &input[0]);

  // now compute frequency and amplitude for all frequencies

  qvecOutputMag.reserve(uiWindowSize / 2);
  qvecOutputPhase.reserve(uiWindowSize / 2);
  qvecOutputFreqList.reserve(uiWindowSize / 2);

  for (uiCounter = 0; uiCounter < (uiWindowSize / 2); uiCounter++) {
    // cmplTemp = std::complex<double> (pOutputData [uiCounter] [0], pOutputData [uiCounter] [1]);
    cmplTemp = std::complex<double>(halfcomplex[uiCounter], -1.0 * halfcomplex[uiCounter + uiWindowSize / 2]);
    dAmpl = (double)(sqrt(pow(real(cmplTemp), 2.0) + pow(imag(cmplTemp), 2.0)));
    if (getArg(cmplTemp, dPhase) == false) {
      bRetValue = false;
    }

    qvecOutputMag.push_back(dAmpl);
    qvecOutputPhase.push_back(dPhase);
    qvecOutputFreqList.push_back((dSampleFreq / uiWindowSize) * uiCounter);
  }
  return (bRetValue);
}

bool MainWindow::getArg(const std::complex<double> clCmplxValue, double &dArg) const {
  // declaration of variables
  bool bRetValue = true;

  dArg = atan(imag(clCmplxValue) / real(clCmplxValue));

  return (bRetValue);
}

void MainWindow::on_pbCloseData_clicked(void) {
  unsigned int uiCounter;

  for (uiCounter = 0; uiCounter < ui->tableTSFiles->rowCount(); uiCounter++) {
    ui->tableTSFiles->removeRow(0);
  }
  ui->tableTSFiles->removeRow(0);
  ui->tableTSFiles->removeRow(0);

  ui->tableTSFiles->clearContents();
}

double MainWindow::computeTFAmpl(const double dSpectraAmpl, const double dFreq, const QString qstrSensorType) const {
  double dRetValue;
  double dCoilCalSensitivity;
  bool bUseCalCoil;

  if ((qstrSensorType.contains("MFS06") == true) ||
      (qstrSensorType.contains("MFS10") == true)) {
    dCoilCalSensitivity = 4.0;
    bUseCalCoil = true;
  } else if ((qstrSensorType.contains("MFS07") == true) ||
             (qstrSensorType.contains("MFS11") == true)) {
    dCoilCalSensitivity = 1.6;
    bUseCalCoil = true;
  } else {
    dCoilCalSensitivity = 1.0;
    bUseCalCoil = false;
  }

  if (this->ui->cbRawTransferFunction->isChecked() == true) {
    dCoilCalSensitivity = 1.0;
    bUseCalCoil = false;
  }

  /**
   * Formula created by MA according to ADU-07e input circuitry.
   * Result is V/(nT * Hz)
   */
  if (bUseCalCoil == true) {
    dRetValue = dSpectraAmpl * (1.428E3 / 1.0001E6) * (1.0 / dCoilCalSensitivity) * (1.0 / dFreq) * 8.8;
    // dRetValue  = dSpectraAmpl * (1.428E3 / 1.0001E6) * (1.0 / dCoilCalSensitivity) /** (1.0 / dFreq)*/ * 8.8;
    // dRetValue *= 2.707;
  } else {
    dRetValue = dSpectraAmpl;
  }

  return (dRetValue);
}

void MainWindow::on_pbShowPlot_clicked(void) {
  QString qstrTemp;

  if (pclPlot != NULL) {
    if (this->qvecTFData.size() > 0) {
      pclPlot->show();
    } else {
      QMessageBox::warning(this, "Show Plot", "No data was processed yet to be plotted!");
    }

  } else {
    if (this->qvecTFData.size() > 0) {
      pclPlot = new Plot(this);
      qstrTemp = "Metronix In-Situ Coil Calibration Tool - ";
      qstrTemp += qstrVersion;
      pclPlot->setWindowTitle(qstrTemp);
      pclPlot->showData(this->qvecTFHeader, this->qvecTFData, ui->cbRawTransferFunction->isChecked());
      pclPlot->show();
    } else {
      QMessageBox::warning(this, "Show Plot", "No data was processed yet to be plotted!");
    }
  }
}

bool MainWindow::saveDataToTXT(void) {
  bool bRetValue = true;
  unsigned int uiCHCount;
  unsigned int uiTSCount;
  unsigned int uiSampleCount;
  QString qstrTemp;
  QVector<QMap<double, double>> qvecPhase;
  QVector<QMap<double, double>> qvecMag;
  QString qstrTargetDir;
  QString qstrTXT;

  if (this->qvecTFHeader.size() == 0) {
    QMessageBox::warning(this, "InSituCoilCal", "Data was not processed yet!");
  } else {
    QMap<double, double> qmapTemp;

    qvecMag.push_back(qmapTemp);
    qvecMag.push_back(qmapTemp);
    qvecMag.push_back(qmapTemp);

    qvecPhase.push_back(qmapTemp);
    qvecPhase.push_back(qmapTemp);
    qvecPhase.push_back(qmapTemp);

    qstrTargetDir = QFileDialog::getExistingDirectory(this);
    for (uiTSCount = 0; uiTSCount < this->qvecTFData.size(); uiTSCount++) {
      for (uiCHCount = 0; uiCHCount < this->qvecTFData[uiCHCount].size(); uiCHCount++) {
        for (uiSampleCount = 0; uiSampleCount < this->qvecTFData[uiTSCount][uiCHCount][0].size(); uiSampleCount++) {
          qvecPhase[uiCHCount].insert(this->qvecTFData[uiTSCount][uiCHCount][0][uiSampleCount], this->qvecTFData[uiTSCount][uiCHCount][2][uiSampleCount]);
          qvecMag[uiCHCount].insert(this->qvecTFData[uiTSCount][uiCHCount][0][uiSampleCount], this->qvecTFData[uiTSCount][uiCHCount][1][uiSampleCount]);
        }
      }
    }

    for (uiCHCount = 0; uiCHCount < this->qvecTFData[uiCHCount].size(); uiCHCount++) {
      QString qstrOutput;

      qstrOutput.clear();
      qstrOutput = "Calibration measurement using InSituCoilCal\n";
      qstrOutput += "Metronix GmbH, Kocherstr. 3, 38120 Braunschweig\n";
      qstrOutput += "\n";
      qstrTemp = this->qvecTFHeader[0][uiCHCount][2].left(this->qvecTFHeader[0][uiCHCount][2].indexOf('/'));
      qstrTemp += " " + this->qvecTFHeader[0][uiCHCount][2].right(4);
      qstrOutput += "Magnetometer: " + qstrTemp +
                    " Date: " + QDateTime::currentDateTime().toString("dd/MM/yy") +
                    " Time: " + QDateTime::currentDateTime().toString("hh:mm:ss") + "\n";
      qstrOutput += "\n";
      qstrOutput += "\n";
      qstrOutput += "FREQUENCY    MAGNITUDE    PHASE\n";
      qstrOutput += "Hz           V/(nT*Hz)    deg\n";
      qstrOutput += "Chopper On\n";

      for (uiSampleCount = 0; uiSampleCount < qvecPhase[uiCHCount].size(); uiSampleCount++) {
        qstrOutput += "+" + QString::number(qvecMag[uiCHCount].keys().at(uiSampleCount), 'e', 4);
        qstrTemp = QString::number(qvecMag[uiCHCount].values().at(uiSampleCount), 'e', 4);
        if (qvecMag[uiCHCount].values().at(uiSampleCount) >= 0.0) {
          qstrTemp.prepend("+");
        }
        qstrOutput += "  " + qstrTemp;
        qstrTemp = QString::number(qvecPhase[uiCHCount].values().at(uiSampleCount), 'e', 4);
        if (qvecPhase[uiCHCount].values().at(uiSampleCount) >= 0.0) {
          qstrTemp.prepend("+");
        }
        qstrOutput += "  " + qstrTemp;
        qstrOutput += "\n";
      }

      qstrOutput += "\n";
      qstrOutput += "\n";
      qstrOutput += "FREQUENCY    MAGNITUDE    PHASE\n";
      qstrOutput += "Hz           V/(nT*Hz)    deg\n";
      qstrOutput += "Chopper Off\n";

      for (uiSampleCount = 0; uiSampleCount < qvecPhase[uiCHCount].size(); uiSampleCount++) {
        if (qvecMag[uiCHCount].keys().at(uiSampleCount) > 30.0) {
          qstrOutput += "+" + QString::number(qvecMag[uiCHCount].keys().at(uiSampleCount), 'e', 4);
          qstrTemp = QString::number(qvecMag[uiCHCount].values().at(uiSampleCount), 'e', 4);
          if (qvecMag[uiCHCount].values().at(uiSampleCount) >= 0.0) {
            qstrTemp.prepend("+");
          }
          qstrOutput += "  " + qstrTemp;
          qstrTemp = QString::number(qvecPhase[uiCHCount].values().at(uiSampleCount), 'e', 4);
          if (qvecPhase[uiCHCount].values().at(uiSampleCount) >= 0.0) {
            qstrTemp.prepend("+");
          }
          qstrOutput += "  " + qstrTemp;
          qstrOutput += "\n";
        }
      }

      qstrTemp = this->qvecTFHeader[0][uiCHCount][2];
      qstrTemp = qstrTemp.remove('/');
      qstrTemp = qstrTemp.remove('#');
      qstrTemp += ".TXT";
      QFile qfOutput;
      qfOutput.setFileName(qstrTargetDir + "/" + qstrTemp);
      if (qfOutput.exists() == true) {
        qfOutput.remove();
      }
      if (qfOutput.open(QIODevice::WriteOnly) == false) {
        QMessageBox::warning(this, "InSituCoilCal", "Output file could not be opened:\n\n" + qfOutput.fileName());
      } else {
        qfOutput.write(qstrOutput.toStdString().c_str(), qstrOutput.size());
        qfOutput.close();
      }
    }
  }

  return (bRetValue);
}
