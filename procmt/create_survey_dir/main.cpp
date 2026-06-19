#include "procmt_alldefines.h"
#include <QCoreApplication>
int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);

  QStringList dirs(a.arguments());

  for (auto &str : dirs) {
    QDir dir(str);
    pmt::create_survey_subdirs<bool>(dir);
  }
  exit(0);
}
