#include "spectra_plotter_pro.h"
#include <QApplication>
#include <stdint.h>
#include <cfloat>

#include <QDebug>

int main(int argc, char *argv[]) {
    // ### IMPORTANT NOTICE! ### //
    // TO WORK CORRECTLY YOU HAVE TO TO ENABLE OPENGL-CONTEXT SHARING!

    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(16);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setVersion(4, 1);
    QSurfaceFormat::setDefaultFormat(fmt);
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);


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
