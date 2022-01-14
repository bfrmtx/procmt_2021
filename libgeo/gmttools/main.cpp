#include <QCoreApplication>
#include "prc_com.h"
#include "std_colour_scale.h"
#include <QFile>
#include <QTextStream>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QStringList allarguments = a.arguments();
    QStringList allmeasdocs;
    QStringList allatsfiles;
    QStringList files;

    prc_com cmdline;
    QFile data;
    QFile lines;

    // prepare possible cmdline parameter
    cmdline.insert("cpt", "");
    cmdline.insert_double("max", 90);
    cmdline.insert_double("min", 0);
    cmdline.insert("type", "lin");
    cmdline.insert_int("extends_colour", 1);
    cmdline.insert_int("revert", 0);
    cmdline.insert_int("nodata_white", 1);
    cmdline.insert_int("round", 1);
    cmdline.insert_int("createcpt", 1);
    cmdline.insert_int("createcontour", 1);
    cmdline.insert("basename", "new");


    if (allarguments.contains("--help") || allarguments.contains("-help") ) {
        qDebug() << "hashes ready to be set by command line";
        cmdline.show_my_map();
        a.quit();
        return 0;
    }


    cmdline.scan_cmdline(allarguments, files, allatsfiles,allmeasdocs);




    std_colour_scale scpt(cmdline.svalue("cpt"),cmdline.ivalue("extends_colour"),cmdline.ivalue("nodata_white"),cmdline.ivalue("revert"));

    if (cmdline.ivalue("createcpt") == 1) {
        data.setFileName(cmdline.svalue("basename") + ".cpt");
        if (data.open(QFile::WriteOnly | QFile::Truncate)) {
            QTextStream *out = new QTextStream(&data);
            if (cmdline.svalue("type") == "lin") {
                scpt.create_cpt_scale(out, cmdline.dvalue("min"), cmdline.dvalue("max"), false, false, cmdline.ivalue("round"));
            }
            else {
                scpt.create_cpt_scale(out, cmdline.dvalue("min"), cmdline.dvalue("max"), false,true, cmdline.ivalue("round"));
            }


            data.close();
            delete out;

        }
    }

    if (cmdline.ivalue("createcontour") == 1) {
        data.setFileName(cmdline.svalue("basename") + ".contourlines");
        if (data.open(QFile::WriteOnly | QFile::Truncate)) {
            QTextStream *out = new QTextStream(&data);
            if (cmdline.svalue("type") == "lin") {
                scpt.create_cpt_scale(out, cmdline.dvalue("min"), cmdline.dvalue("max"), true, false, cmdline.ivalue("round"));
            }
            else {
                scpt.create_cpt_scale(out, cmdline.dvalue("min"), cmdline.dvalue("max"), true,true, cmdline.ivalue("round"));
            }


            data.close();
            delete out;

        }
    }

    exit(0);
}
