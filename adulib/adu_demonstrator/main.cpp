#include "adu_gui.h"
#include "adu_json.h"

#include <QApplication>
#include <QDebug>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    adu_gui w;
    w.show();
    return a.exec();
}
