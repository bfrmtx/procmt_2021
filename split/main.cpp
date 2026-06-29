#include "mainwindow.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  app.setDesktopFileName("split");
  QGuiApplication::setDesktopFileName("split");
  app.setWindowIcon(QIcon(":/icons/split.png"));

  MainWindow window;
  window.show();

  return app.exec();
}
