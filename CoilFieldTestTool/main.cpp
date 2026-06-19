#include "mainwindow.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setDesktopFileName("CoilFieldTestTool");
    QGuiApplication::setDesktopFileName("CoilFieldTestTool");
    a.setWindowIcon(QIcon(":/icons/CoilFieldTestTool.png"));

    MainWindow w;
    w.show();

    return a.exec();
}
