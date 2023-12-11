#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>

#include "FFTReal.h"
#include "plot.h"
#include <complex>
#include <cstddef>
#include <vector>

/**
 * Version Number
 */
#define C_VERSION_MAIN 0
#define C_VERSION_SUB 1
#define C_VERSION_CUSTOMER 100000
#define C_VERSION_KM_MAIN 1
#define C_VERSION_KM_SUB "$Revision: 17 $"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  bool saveDataToTXT(void);

private slots:
  void on_pbOpenData_clicked(void);

  void on_pbCloseData_clicked(void);

  void on_pbComputeTF_clicked(void);

  void on_pbShowPlot_clicked(void);

private:
  Ui::MainWindow *ui;

  /**
   * 1: TS index from table
   * 2: Channel index  (Hx, Hy, Hz)
   * 3: Header Strings (Sensor Type, Cal Frequency, ...)
   */
  QVector<QVector<QVector<QString>>> qvecTFHeader;

  /**
   * 1: TS index from table
   * 2: Channel index (Hx, Hy, Hz)
   * 3: TF Data (0: Frequency, 1: Magnitude, 2: Phase)
   * 4: Data
   */
  QVector<QVector<QVector<QVector<double>>>> qvecTFData;

  Plot *pclPlot;

  ffft::FFTReal<double> *fft_object = nullptr;
  std::vector<double> halfcomplex;
  quint32 uiwsize = 0;

  QString qstrLastTSDir;

  QString qstrVersion;

  bool computeTF(QVector<QVector<QVector<QString>>> &qvecTFHeader, QVector<QVector<QVector<QVector<double>>>> &qvecTFData,
                 const QString qstrATSRef, const QString qstrATSCal, const QString qstrSensorType, const QString qstrInput,
                 const double dSampleFreq, const double dCalFreq, const double dNumHarmonics, const unsigned int uiSetIndex, const unsigned int uiCHIndex);

  bool computeFFT(const QVector<double> qvecInputData, QVector<double> &qvecOutputMag, QVector<double> &qvecOutputPhase, QVector<double> &qvecOutputFreqList, const unsigned int uiWindowSize, const double dSampleFreq);

  bool getArg(const std::complex<double> clCmplxValue, double &dArg) const;

  double computeTFAmpl(const double dSpectraAmpl, const double dFreq, const QString qstrSensorType) const;
};

#endif // MAINWINDOW_H
