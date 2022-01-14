#include "tsdata_header.h"
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
  //  MainWindow w;
 //   w.show();

    QFileInfo qfi("/home/bfr/cpp/sw/mtx_devel/procmt/data/samtex/kap103as.ts");
    tsdata_header tsdata(qfi);
    tsdata.open();
    size_t i = tsdata.read();


    qDebug() << "read" << i << "lines";

    tsdata.guess_values();

    tsdata.create_ats_files();

    exit (0);
   // return a.exec();
}
