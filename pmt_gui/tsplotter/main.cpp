
#include "tsplotter.h"
#include <QApplication>
#include <QStringList>
#include <QFileInfo>




int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TS_NAMESPACE_NAME::tsplotter* tsplot = new TS_NAMESPACE_NAME::tsplotter();

    QStringList allarguments = a.arguments();
    QList<QFileInfo> allatsfiles;

    for (int i = 1; i < allarguments.size(); ++i) {
        if (allarguments.at(i).endsWith(".ats",Qt::CaseInsensitive) || allarguments.at(i).endsWith(".xml",Qt::CaseInsensitive)  ) {
            QFileInfo qfi(allarguments.at(i));
            if (qfi.exists()) allatsfiles.append(qfi);
            else {
                qDebug() << qfi.absoluteFilePath() << "not there";
            }
        }
        else {
           QFileInfo qfi(allarguments.at(i));
           if (qfi.isDir()) allatsfiles.append(qfi);
        }

    }



    if (allatsfiles.size()) tsplot->open_file_list(allatsfiles);


    tsplot->show();


    return a.exec();
}
