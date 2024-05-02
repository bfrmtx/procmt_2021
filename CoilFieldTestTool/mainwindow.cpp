#include "mainwindow.h"
#include "ui_mainwindow.h"

void startProcessingThread(MainWindow *pclParentClass) {
  pclParentClass->processingThreadFunc();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
  QString qstrTmp;

  this->cmdline = std::make_shared<prc_com>();

  ui->setupUi(this);

  this->acceptDrops();

  this->ui->tableTSFiles->setAlternatingRowColors(true);
  this->ui->tableTSFiles->resizeColumnToContents(0);
  this->ui->tableTSFiles->resizeColumnToContents(1);
  this->ui->tableTSFiles->resizeRowsToContents();
  this->ui->tableTSFiles->horizontalHeader()->setStretchLastSection(true);
  this->ui->frameProgress->setVisible(false);

  pclPlot = Q_NULLPTR;

  pclSettings = new QSettings("Metronix", "Coil Field Test Tool");

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

  qstrTmp = "Metronix CoilFieldTest Tool - ";
  qstrTmp += qstrVersion;

  this->ui->pbSave->hide();

  this->setWindowTitle(qstrTmp);

  QStringList fcombo;
  // fcombo << "FFT_freqs" << "auto_gen_tf_80" << "auto_gen_tf_40"  << "MT_Freqs" ;
  fcombo << "FFT_freqs"
         << "auto_gen_tf_80"
         << "auto_gen_tf_40";

  this->ui->cbFFTWindowLength->addItems(fcombo);

  QStringList cutcombo;
  cutcombo << "sharp"
           << "medium"
           << "smooth";

  this->ui->cbFFT_cuts->addItems(cutcombo);

  this->cmdline->insert("target_frequencies", this->ui->cbFFTWindowLength->currentText());

  this->on_cbFFT_cuts_currentIndexChanged(this->ui->cbFFTWindowLength->currentText());

  this->cmdline->insert_double("mt_site_xstddev", this->ui->xstddev_doubleSpinBox->value());

  // activate the parallel test part
  this->cmdline->insert("calib_lab_run", "parallel_test");
  this->cmdline->insert_int("skip_marked_parts", 1);
  // default
  this->cmdline->insert_double("fcut_upper", 0.5);
  this->cmdline->insert_double("fcut_lower", 0.01);

  QList<QString> qstrlFiles;
  qstrlFiles.append("HxHx_ampl.dat");
  qstrlFiles.append("HyHy_ampl.dat");
  qstrlFiles.append("HzHz_ampl.dat");
  QDir directory(QDir::home());
  QStringList qstrlFilesInDir = directory.entryList(qstrlFiles);
  if (qstrlFilesInDir.size() > 0) {
    QMessageBox::warning(this, "Overwrite Data", "The following files exist in the home directory: HxHx_ampl.dat, HyHy_ampl.dat, HzHz_ampl.dat");
  }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
  event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event) {

  QList<QUrl> urls = event->mimeData()->urls();
  if (urls.isEmpty())
    return;

  QString qfistr = urls.at(0).toLocalFile();
  QFileInfo qfi(qfistr);
  if (qfi.isDir()) {
    this->ui->tableTSFiles->setRowCount(0);

    this->qstrTSSourceDir = qfi.absoluteFilePath();
    this->on_pbOpenData_clicked();
  }
}

MainWindow::~MainWindow() {
  if (pclPlot != Q_NULLPTR) {
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

  // check the existence of HxHx_ampl.dat HyHy_ampl.dat and HzHz_ampl.dat
  // if they exist, we need a message box to inform the user that the files exist
  QList<QString> qstrlFiles;
  qstrlFiles.append("HxHx_ampl.dat");
  qstrlFiles.append("HyHy_ampl.dat");
  qstrlFiles.append("HzHz_ampl.dat");
  QDir directory(QDir::home());
  QStringList qstrlFilesInDir = directory.entryList(qstrlFiles);
  if (qstrlFilesInDir.size() > 0) {
    QMessageBox::information(this, "Saved Data", "The following files exist in the home directory: HxHx_ampl.dat, HyHy_ampl.dat, HzHz_ampl.dat");
  }

  delete ui;
}

void MainWindow::on_pbOpenData_clicked(void) {

  if (!this->qstrTSSourceDir.size()) {
    qstrTSSourceDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), qstrLastTSDir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
  }

  // declaration of variables
  QStringList qstrlTSSourceDirs;
  QStringList qstrlATSFiles;
  QDir qdirTSSourceDir;
  int uiDirCount;
  int uiATSCount;
  QString qstrTemp;

  this->qvecCalChopperOnFreq.clear();
  this->qvecCalChopperOnAmpl.clear();
  this->qvecCalChopperOffFreq.clear();
  this->qvecCalChopperOffAmpl.clear();

  // now search for TS directories inside this directory.
  qdirTSSourceDir.setPath(qstrTSSourceDir);
  qstrlTSSourceDirs.clear();
  qstrlTSSourceDirs.append("meas_*");
  qstrlTSSourceDirs = qdirTSSourceDir.entryList(qstrlTSSourceDirs);

  qDebug() << "[ on_pbOpenData_clicked ] TS dirs:" << qstrlTSSourceDirs;

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

      // qDebug () << "[" << __PRETTY_FUNCTION__ << "] ATS inside:" << qstrlTSSourceDirs [uiDirCount];
      // qDebug () << "[" << __PRETTY_FUNCTION__ << "] " << qstrlATSFiles;

      ui->tableTSFiles->insertRow(uiDirCount);

      ui->tableTSFiles->setItem(uiDirCount, 0, new QTableWidgetItem(""));
      ui->tableTSFiles->item(uiDirCount, 0)->setCheckState(Qt::Checked);
      ui->tableTSFiles->setItem(uiDirCount, 8, new QTableWidgetItem(qstrlTSSourceDirs[uiDirCount]));

      // get calibration frequency from XML file (is only stored inside XML file)

      qstrTemp = qstrlTSSourceDirs[uiDirCount];
      qstrTemp += "/";

      for (uiATSCount = 0; uiATSCount < qstrlATSFiles.size(); uiATSCount++) {
        QFileInfo qfi(qdirTSSourceDir.absolutePath() + "/" + qstrlATSFiles.at(uiATSCount));

        std::unique_ptr<atsheader> atsh = std::make_unique<atsheader>(qfi, 0, this);

        qstrTemp = qstrlTSSourceDirs[uiDirCount];
        qstrTemp += "/";
        qstrTemp += qstrlATSFiles[uiATSCount];
        size_t itest(atsh->scan_header_close());
        if ((itest != SIZE_MAX) && (itest != 0)) {
          qstrTemp = QString::number(atsh->ivalue("sensor_sernum"));
          while (qstrTemp.size() < 3) {
            qstrTemp.prepend('0');
          }
          qstrTemp.prepend('#');
          qstrTemp = atsh->svalue("sensor_type") + " " + qstrTemp;

          if (qstrlATSFiles[uiATSCount].contains("Ex") == true) {
            ui->tableTSFiles->setItem(uiDirCount, 1, new QTableWidgetItem(QString::number(atsh->dvalue("sample_freq"))));
            ui->tableTSFiles->setItem(uiDirCount, 2, new QTableWidgetItem("0"));
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
        atsh.reset();
      }
    }
  }

  qstrTemp.clear();
}

void MainWindow::processingThreadFunc(void) {
  // create a process for this dir
  std::unique_ptr<calib_lab> process = std::make_unique<calib_lab>();
  // set the data output
  process->set_storage(this->data.last());

  this->cmdline->insert("calibration", this->ui->cbCalType->currentText());

  // result of process shall arrive in this->data.last()
  this->cmdline->show_my_map();
  process->run(*this->cmdline, this->qfis);
  process.reset();
  this->qfis.clear();

  this->atomProcessingThreadRunning.store(false);
}

void MainWindow::on_pbComputeTF_clicked(void) {

  if (!ui->tableTSFiles->rowCount())
    return;
  unsigned int uiTSSetCounter;
  unsigned int uiCHCounter;
  unsigned int uiSetIndex;
  unsigned int uiProgress;

  QString qstrTemp1;
  QStringList qstrlATSFiles;
  QDir qdirTargetTSDir;
  QVector<QString> qvecHeader;

  QVector<QVector<double>> qvec1CHData;
  QVector<QVector<QVector<double>>> qvecMultiCHData;
  QVector<QVector<QString>> qvec1SetHeader;

  // prepare data and header vector for processing results
  this->qvecTFData.clear();

  qvec1CHData.resize(6);
  qvecMultiCHData.push_back(qvec1CHData);
  qvecMultiCHData.push_back(qvec1CHData);
  qvecMultiCHData.push_back(qvec1CHData);

  this->qvecTFHeader.clear();
  qvec1SetHeader.resize(3);

  // compute parallel test results for all selected data sets
  uiSetIndex = 0;
  for (uiTSSetCounter = 0; uiTSSetCounter < ui->tableTSFiles->rowCount(); uiTSSetCounter++) {
    if (ui->tableTSFiles->item(uiTSSetCounter, 0)->checkState() == Qt::Checked) {
      int iAmplIndex = 0;
      int iCohIndex = 0;
      int iCounter;

      this->qvecTFData.push_back(qvecMultiCHData);
      this->qvecTFHeader.push_back(qvec1SetHeader);

      // fetch all H-channel ATS file names
      qdirTargetTSDir.setPath(ui->tableTSFiles->item(uiTSSetCounter, 8)->text());
      qstrlATSFiles.clear();
      qstrlATSFiles.append("*_TH*.ats");
      qfis = qdirTargetTSDir.entryInfoList(qstrlATSFiles);

      // append data for this dir
      this->data.append(std::make_shared<QMap<QString, parallel_test_coherency>>());

      // start processing in separate thread to have recative GUI
      this->atomProcessingThreadRunning.store(true);

      std::thread clProcessingThread(startProcessingThread, this);

      this->ui->frameProgress->setVisible(true);
      this->ui->progress->setRange(0, 0);
      this->ui->progress->setValue(0);
      this->ui->progress->setTextVisible(true);
      this->ui->labProgress->setText("Processing Set: " + ui->tableTSFiles->item(uiTSSetCounter, 1)->text() + "Hz  ");
      this->update();

      while (this->atomProcessingThreadRunning.load() == true) {
        this->ui->progress->setValue(uiProgress);
        uiProgress++;
        this->update();
        QThread::msleep(1);
      }
      this->ui->frameProgress->setVisible(false);

      clProcessingThread.join();

      // copy processing results into local data containers for display
      auto iter = data.last()->constBegin();
      while (iter != data.last()->constEnd()) {
        if (iter.key().contains("ampl")) {
          // add header information
          qvecHeader.clear();
          qvecHeader.push_back(ui->tableTSFiles->item(uiTSSetCounter, 1)->text() + "Hz");
          qvecHeader.push_back(ui->tableTSFiles->item(uiTSSetCounter, 2)->text() + "Hz");
          qvecHeader.push_back(ui->tableTSFiles->item(uiTSSetCounter, 5 + iAmplIndex)->text());
          qvecHeader.push_back("Input");
          qvecHeader.push_back("Unknown Chopper");
          qvecTFHeader[uiSetIndex][iAmplIndex] = qvecHeader;

          for (iCounter = 0; iCounter < iter.value().ampl.size(); iCounter++) {
            this->qvecTFData[uiSetIndex][iAmplIndex][Frequency].push_back((double)iter.value().f[iCounter]);
            this->qvecTFData[uiSetIndex][iAmplIndex][Amplitude].push_back((double)iter.value().ampl[iCounter]);
            this->qvecTFData[uiSetIndex][iAmplIndex][Error].push_back((double)iter.value().err[iCounter]);
          }
          iAmplIndex++;
        }

        if (iter.value().cal_f.size() && iter.value().chopper == 1) {
          qDebug() << "get chopper on >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> : " << iter.value().cal_f.size() << iter.value().cal_f.at(0) << iter.value().cal_ampl.at(0) << iter.value().cal_phase.at(0);
          this->qvecCalChopperOnFreq.push_back(fromStdVector(iter.value().cal_f));
          this->qvecCalChopperOnAmpl.push_back(fromStdVector(iter.value().cal_ampl));
          this->qvecCalChopperOnPhase.push_back(fromStdVector(iter.value().cal_phase));
        }

        if (iter.value().cal_f.size() && iter.value().chopper == 0) {
          qDebug() << "get chopper off >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> : " << iter.value().cal_f.size() << iter.value().cal_f.at(0) << iter.value().cal_ampl.at(0) << iter.value().cal_phase.at(0);
          this->qvecCalChopperOffFreq.push_back(fromStdVector(iter.value().cal_f));
          this->qvecCalChopperOffAmpl.push_back(fromStdVector(iter.value().cal_ampl));
          this->qvecCalChopperOffPhase.push_back(fromStdVector(iter.value().cal_phase));
        }

        if (iter.key().contains("coh")) {
          for (iCounter = 0; iCounter < iter.value().coh.size(); iCounter++) {
            this->qvecTFData[uiSetIndex][iCohIndex][Coherency].push_back((double)iter.value().coh[iCounter]);
          }
          iCohIndex++;
        }

        ++iter;
      }

      // now for this set compute the noise function
      // skip first and last values as they border values are indifferent
      for (uiCHCounter = 0; uiCHCounter < this->qvecTFData[uiSetIndex].size(); uiCHCounter++) {
        for (iCounter = 0; iCounter < this->qvecTFData[uiSetIndex][uiCHCounter][Coherency].size(); iCounter++) {
          // noise = SpectraAmpl * sqrt (1 - coherency)
          this->qvecTFData[uiSetIndex][uiCHCounter][Noise].push_back(this->qvecTFData[uiSetIndex][uiCHCounter][Amplitude].at(iCounter) * (sqrt(1 - this->qvecTFData[uiSetIndex][uiCHCounter][Coherency].at(iCounter))));
        }
      }

      uiSetIndex++;
    }
  }

  // show data on plot
  if (pclPlot != Q_NULLPTR) {
    delete (pclPlot);
  }

  pclPlot = new Plot(this->info_db, this->mastercal_db, this);
  qstrTemp1 = "Metronix Coil Field Test Tool - ";
  qstrTemp1 += qstrVersion;
  pclPlot->setWindowTitle(qstrTemp1);
  pclPlot->showData(this->qvecTFHeader, this->qvecTFData,
                    this->qvecCalChopperOnFreq, this->qvecCalChopperOnAmpl, this->qvecCalChopperOnPhase,
                    this->qvecCalChopperOffFreq, this->qvecCalChopperOffAmpl, this->qvecCalChopperOffPhase);
  pclPlot->show();

  qDebug() << "finished";
}

// bool MainWindow::getArg (const std::complex<double> clCmplxValue, double& dArg) const
//{
//     // declaration of variables
//     bool bRetValue = true;

//    dArg = atan (imag (clCmplxValue) / real (clCmplxValue));

//    return (bRetValue);
//}

void MainWindow::on_pbCloseData_clicked(void) {
  unsigned int uiCounter;

  for (uiCounter = 0; uiCounter < ui->tableTSFiles->rowCount(); uiCounter++) {
    ui->tableTSFiles->removeRow(0);
  }
  ui->tableTSFiles->removeRow(0);
  ui->tableTSFiles->removeRow(0);

  ui->tableTSFiles->clearContents();
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

      pclPlot = new Plot(this->info_db, this->mastercal_db, this);
      qstrTemp = "Metronix Coil Field-Test Tool - ";
      qstrTemp += qstrVersion;
      pclPlot->setWindowTitle(qstrTemp);
      pclPlot->showData(this->qvecTFHeader, this->qvecTFData,
                        this->qvecCalChopperOnFreq, this->qvecCalChopperOnAmpl, this->qvecCalChopperOnPhase,
                        this->qvecCalChopperOffFreq, this->qvecCalChopperOffAmpl, this->qvecCalChopperOffPhase);
      pclPlot->show();

    } else {
      QMessageBox::warning(this, "Show Plot", "No data was processed yet to be plotted!");
    }
  }
}

void MainWindow::on_cbFFTWindowLength_currentTextChanged(const QString &arg1)
// void MainWindow::on_cbFFTWindowLength_currentIndexChanged(const QString &arg1)
{
  this->cmdline->insert("target_frequencies", arg1);
  std::cout << "target freqs changed " << arg1.toStdString() << std::endl;
  if (arg1 == "MT_Freqs") {
    this->cmdline->insert_double("fcut_upper", 0.5);
    this->cmdline->insert_double("fcut_lower", 0.15);
  }
}

void MainWindow::on_cbFFT_cuts_currentIndexChanged(const QString &arg1) {
  if (arg1 == "smooth") {
    this->cmdline->insert_double("fcut_upper", 0.45);
    this->cmdline->insert_double("fcut_lower", 0.15);
  }
  if (arg1 == "medium") {
    this->cmdline->insert_double("fcut_upper", 0.15);
    this->cmdline->insert_double("fcut_lower", 0.05);
  }
  if (arg1 == "sharp") {
    this->cmdline->insert_double("fcut_upper", 0.5);
    this->cmdline->insert_double("fcut_lower", 0.01);
  }
}

void MainWindow::on_xstddev_doubleSpinBox_valueChanged(const double arg1) {
  this->cmdline->insert_double("mt_site_xstddev", arg1);
  std::cout << "stdev changed " << arg1 << std::endl;
}

void MainWindow::on_cbCalType_currentTextChanged(const QString &arg1) {
  this->cmdline->insert("calibration", arg1);
  std::cout << "calibration changed " << arg1.toStdString() << std::endl;
}
