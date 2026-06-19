// #include "mainwindow.h"
#include "mestplotlib.h"
#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  mest_plotlib w;
  w.show();
  // MainWindow w;
  // w.show();

  return a.exec();
}
