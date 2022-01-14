#include <QApplication>
#include <QFileInfo>

#include "xmlcallib.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFileInfo info_db("info.sql3"), master_cal_db("master_calibration.sql3");
    xmlcallib w(info_db, master_cal_db);
//    w.setWindowIcon(QIcon(":/myicons/glasses.svg"));

    if (a.arguments().size() > 1) {
        QFileInfoList qfis;
        for (auto &aa : a.arguments()) {
            if (aa.endsWith(".txt", Qt::CaseInsensitive) || aa.endsWith(".xml", Qt::CaseInsensitive)) qfis.append(QFileInfo(aa));
        }

        w.set_file(qfis);
    }

    w.show();

    return a.exec();
}
