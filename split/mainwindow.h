#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileInfo>
#include <QMainWindow>
#include <QMap>
#include <QString>

class QButtonGroup;
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
  void chooseMeasdoc();
  void splitMeasdoc();

private:
  void setMeasdoc(const QString &path);
  QString droppedMeasdoc(const QDropEvent *event) const;
  qint64 selectedChunkBytes() const;

  QLabel *dropLabel = nullptr;
  QLabel *measdocLabel = nullptr;
  QPushButton *splitButton = nullptr;
  QTextEdit *logText = nullptr;
  QButtonGroup *sizeButtons = nullptr;

  QFileInfo measdocFile;
  QMap<int, qint64> splitSizeByButtonId;
};

#endif // MAINWINDOW_H
