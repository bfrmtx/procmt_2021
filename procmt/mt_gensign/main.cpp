#include "mt_gensign.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mt_gensign w;
    w.show();

    return a.exec();
}
