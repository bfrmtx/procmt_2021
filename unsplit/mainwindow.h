#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileInfo>
#include <QFileInfoList>
#include <QFutureWatcher>
#include <QList>
#include <QMainWindow>
#include <QString>

class QDragEnterEvent;
class QDropEvent;
class QEvent;
class QLabel;
class QPushButton;
class QTextEdit;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

protected:
  bool eventFilter(QObject *watched, QEvent *event) override;
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dropEvent(QDropEvent *event) override;

private slots:
  void chooseDirectory();
  void scanDirectory();
  void unsplitDirectory();
  void swapFinally();
  void scanFinished();

private:
  void setDirectory(const QString &directoryPath);
  QString droppedDirectory(const QDropEvent *event) const;
  static bool splitXmlRunName(const QFileInfo &xmlFile, QString &startNumber, QString &endingPattern);

  QLabel *dropLabel = nullptr;
  QLabel *directoryLabel = nullptr;
  QPushButton *scanButton = nullptr;
  QPushButton *unsplitButton = nullptr;
  QPushButton *swapFinallyButton = nullptr;
  QTextEdit *logText = nullptr;
  QString directoryPath;

  struct run {
    QFileInfo xmlfile;
    QFileInfoList ats_files;
  };

  class runs {
  public:
    runs() = default;
    runs(const QString &startNumber, const QString &endingPattern) : m_startNumber(startNumber), m_endingPattern(endingPattern) {}
    QFileInfo targetXmlFile;
    void addRun(const run &run);
    bool unsplit_ats(QString *errorMessage = nullptr);
    bool unsplit_xml(QString *errorMessage = nullptr);
    const QList<run> &list() const { return m_runs; }
    const QString &startNumber() const { return m_startNumber; }
    const QString &endingPattern() const { return m_endingPattern; }

  private:
    static int runNumber(const run &run);

    QString m_startNumber;
    QString m_endingPattern;
    QList<run> m_runs;
  };

  static QList<runs> scanXmlFiles(const QString &scanDirectory);

  QList<runs> allRuns;
  QFutureWatcher<QList<runs>> scanWatcher;
};
#endif // MAINWINDOW_H