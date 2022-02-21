#include "spectra_plotter.h"
#include <QApplication>
#include <stdint.h>
#include <cfloat>

#include <QDebug>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    spectra_plotter w;
    w.show();

    if(argc > 1) {
        QString specfile = QString::fromStdString(argv[argc - 1]);
        while(specfile.front() == '-') {
            specfile = specfile.mid(1);
        }
        if(QFileInfo::exists(specfile)) {
            w.load_file(specfile);
        }
    }

    return a.exec();
}
