#include "mainwindow.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  app.setDesktopFileName("unsplit");
  QGuiApplication::setDesktopFileName("unsplit");
  app.setWindowIcon(QIcon(":/icons/unsplit.png"));

  MainWindow window;
  window.show();

  return app.exec();
}