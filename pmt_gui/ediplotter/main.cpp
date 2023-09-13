#include "ediplotter.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    std::shared_ptr<msg_logger> msg = std::make_shared<msg_logger>();

    QStringList allarguments = a.arguments();
    QStringList alledifiles;

    for (int i = 1; i < allarguments.size(); ++i) {
      if (allarguments.at(i).endsWith(".edi",Qt::CaseInsensitive)) {
        QFileInfo qfi(allarguments.at(i));
        alledifiles << qfi.absoluteFilePath();
      }
    }

    ediplotter w;
    w.set_msg_logger(msg);

    if (alledifiles.size()) {
      for (const auto &file : alledifiles) {
        w.open_edi_file(file);

      }
    }

    w.show();

    return a.exec();
}
