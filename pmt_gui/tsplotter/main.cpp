
#include "tsplotter.h"
#include <QApplication>
#include <QEvent>
#include <QFileInfo>
#include <QFileOpenEvent>
#include <QIcon>
#include <QStringList>
#include <QTimer>

class TsFileOpenEventHandler : public QObject {
public:
  explicit TsFileOpenEventHandler(TS_NAMESPACE_NAME::tsplotter *plotter, QObject *parent = nullptr) : QObject(parent), m_plotter(plotter) {
  }

protected:
  bool eventFilter(QObject *watched, QEvent *event) override {
    Q_UNUSED(watched)

    if (event->type() == QEvent::FileOpen && this->m_plotter != nullptr) {
      auto *fileOpenEvent = static_cast<QFileOpenEvent *>(event);
      const auto fileName = fileOpenEvent->file();

      if (!fileName.isEmpty()) {
        QFileInfo qfi(fileName);
        if (qfi.exists() && (qfi.isDir() || fileName.endsWith(".ats", Qt::CaseInsensitive) || fileName.endsWith(".xml", Qt::CaseInsensitive))) {
          if (!this->m_pendingFiles.contains(qfi))
            this->m_pendingFiles.append(qfi);

          if (!this->m_flushScheduled) {
            this->m_flushScheduled = true;
            QTimer::singleShot(0, this, [this]() {
              this->flushPendingFiles();
            });
          }

          return true;
        }
      }
    }

    return QObject::eventFilter(watched, event);
  }

private:
  void flushPendingFiles() {
    this->m_flushScheduled = false;

    if (this->m_plotter == nullptr || this->m_pendingFiles.isEmpty())
      return;

    const auto filesToOpen = this->m_pendingFiles;
    this->m_pendingFiles.clear();

    this->m_plotter->open_file_list(filesToOpen);
    this->m_plotter->show();
    this->m_plotter->raise();
    this->m_plotter->activateWindow();
  }

  TS_NAMESPACE_NAME::tsplotter *m_plotter = nullptr;
  QList<QFileInfo> m_pendingFiles;
  bool m_flushScheduled = false;
};

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  a.setDesktopFileName("tsplotter");
  QGuiApplication::setDesktopFileName("tsplotter");
  a.setWindowIcon(QIcon(":/icons/tsplotter.png"));

  TS_NAMESPACE_NAME::tsplotter *tsplot = new TS_NAMESPACE_NAME::tsplotter();

  QStringList allarguments = a.arguments();
  QList<QFileInfo> allatsfiles;

  for (int i = 1; i < allarguments.size(); ++i) {
    if (allarguments.at(i).endsWith(".ats", Qt::CaseInsensitive) || allarguments.at(i).endsWith(".xml", Qt::CaseInsensitive)) {
      QFileInfo qfi(allarguments.at(i));
      if (qfi.exists())
        allatsfiles.append(qfi);
      else {
        qDebug() << qfi.absoluteFilePath() << "not there";
      }
    } else {
      QFileInfo qfi(allarguments.at(i));
      if (qfi.isDir())
        allatsfiles.append(qfi);
    }
  }

  TsFileOpenEventHandler fileOpenHandler(tsplot);
  a.installEventFilter(&fileOpenHandler);

  if (allatsfiles.size())
    tsplot->open_file_list(allatsfiles);

  tsplot->show();

  return a.exec();
}
