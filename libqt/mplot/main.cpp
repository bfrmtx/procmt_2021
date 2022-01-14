//#include "mainwindow.h"
#include <QApplication>
#include "mestplotlib.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

     mest_plotlib w;
     w.show();
    //MainWindow w;
    //w.show();

    return a.exec();
}
