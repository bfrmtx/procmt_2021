#include "mk_master_cal.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mk_master_cal w;
    w.show();

    return a.exec();
}
