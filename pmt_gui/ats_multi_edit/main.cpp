#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString sqlbd = (procmt_homepath("info.sql3"));
    QString dbname = sqlbd;
    MainWindow w(dbname);
    w.show();

    return a.exec();
}
