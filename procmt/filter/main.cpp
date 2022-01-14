#include "filter.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    filter w;

    std::shared_ptr<prc_com> cmdline = std::make_shared<prc_com>(false);

    if (a.arguments().size() > 2) {
        cmdline->insert("filter", "0x");
        cmdline->insert("notch", "0");
        cmdline->insert("ascii", "empty");
        cmdline->insert("gui", "true");

        QStringList allfiles;
        QStringList allatsfiles;
        QStringList allmeasdocs;


        cmdline->scan_cmdline(a.arguments(), allfiles, allatsfiles, allmeasdocs,true, true, true);

        if (allmeasdocs.size()) {
            QList<QFileInfo> qxml;
            for (auto &str : allmeasdocs) {
                qxml.append(QFileInfo(str));
            }
            w.set_xmlfiles(qxml);

            if (cmdline->contains("filter") && (cmdline->svalue("filter") != "0x") ) {
                w.set_decimation_filter(cmdline->svalue("filter"));
            }

            else if (cmdline->contains("ascii") && (cmdline->svalue("ascii") != "empty")) {
                w.set_ascii_output(cmdline->svalue("ascii"));
            }

            else if (cmdline->contains("notch") && (cmdline->svalue("notch") != "0")) {
                w.set_notch_filter(cmdline->svalue("notch"));
            }

            if (cmdline->svalue("gui") == "false") {
                w.on_runbuton_clicked();
                w.close();
                exit(0);
            }
            else w.show();
        }
    }

    else w.show();

    return a.exec();
}
