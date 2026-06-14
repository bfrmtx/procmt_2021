#include "ediplotter.h"
#include <QApplication>
#include <QEvent>
#include <QFileOpenEvent>
#include <QTimer>

class EdiFileOpenEventHandler : public QObject {
public:
  explicit EdiFileOpenEventHandler(ediplotter *plotter, QObject *parent = nullptr) : QObject(parent), m_plotter(plotter) {
  }

protected:
  bool eventFilter(QObject *watched, QEvent *event) override {
    Q_UNUSED(watched)

    if (event->type() == QEvent::FileOpen && this->m_plotter != nullptr) {
      auto *fileOpenEvent = static_cast<QFileOpenEvent *>(event);
      const auto fileName = fileOpenEvent->file();

      if (!fileName.isEmpty() && fileName.endsWith(".edi", Qt::CaseInsensitive)) {
        QFileInfo qfi(fileName);
        const auto absolutePath = qfi.absoluteFilePath();

        if (!this->m_pendingFiles.contains(absolutePath))
          this->m_pendingFiles.append(absolutePath);

        if (!this->m_flushScheduled) {
          this->m_flushScheduled = true;
          QTimer::singleShot(0, this, [this]() {
            this->flushPendingFiles();
          });
        }

        return true;
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

    for (const auto &file : filesToOpen)
      this->m_plotter->open_edi_file(file);

    this->m_plotter->show();
    this->m_plotter->raise();
    this->m_plotter->activateWindow();
  }

  ediplotter *m_plotter = nullptr;
  QStringList m_pendingFiles;
  bool m_flushScheduled = false;
};

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  std::shared_ptr<msg_logger> msg = std::make_shared<msg_logger>();

  QStringList allarguments = a.arguments();
  QStringList alledifiles;

  for (int i = 1; i < allarguments.size(); ++i) {
    if (allarguments.at(i).endsWith(".edi", Qt::CaseInsensitive)) {
      QFileInfo qfi(allarguments.at(i));
      alledifiles << qfi.absoluteFilePath();
    }
  }

  ediplotter w;
  w.set_msg_logger(msg);

  EdiFileOpenEventHandler fileOpenHandler(&w);
  a.installEventFilter(&fileOpenHandler);

  w.show();

  if (alledifiles.size()) {
    for (const auto &file : alledifiles) {
      w.open_edi_file(file);
    }
  }

  return a.exec();
}
