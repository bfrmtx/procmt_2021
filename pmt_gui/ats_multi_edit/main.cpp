#include "mainwindow.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  a.setDesktopFileName("ats_multi_edit");
  QGuiApplication::setDesktopFileName("ats_multi_edit");
  a.setWindowIcon(QIcon(":/icons/ats_multi_edit.png"));

  QString sqlbd = (procmt_homepath("info.sql3"));
  QString dbname = sqlbd;
  MainWindow w(dbname);
  w.show();

  return a.exec();
}
