#include "atsedit.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle("macintosh");



    if ( (a.arguments().contains("-cmd")) || (a.arguments().contains("-help")) || (a.arguments().contains("--help"))) {
        std::cout << "command line" << std::endl;

        // need a template for the header qmap
        QFileInfo qfitemp;
        std::unique_ptr<atsheader> tpl = std::make_unique<atsheader>(qfitemp, 80);
        std::unique_ptr<prc_com> cmdline = std::make_unique<prc_com>(tpl->get_data(),false);

        QStringList allfiles;
        QStringList allatsfiles;
        QStringList allmeasdocs;

        if ((a.arguments().contains("-help")) || (a.arguments().contains("--help")) ) {
            std::cout << "first column shows the option availabe " << std::endl;
            std::cout << "like  -ADB_board_type HF    will set the ADB board to HF for all files" << std::endl;
            cmdline->show_my_map(false, true);
        }

        cmdline->scan_cmdline(a.arguments(), allfiles, allatsfiles, allmeasdocs,true, true, true);

#ifdef QT_DEBUG
        cmdline->show_my_map();
#endif

        for (auto &str : allatsfiles) {
            QFileInfo qfi(str);

            if (qfi.exists() && qfi.fileName().endsWith("ats", Qt::CaseInsensitive)) {

                std::unique_ptr<measdocxml> measdoc = nullptr;

                std::unique_ptr<atsheader> atsh = std::make_unique<atsheader>(qfi, 80);
                atsh->scan_header_close(true);
                atsh->set_change_measdir(false);
                atsh->write_header(true);
                atsh->close(false);

                QFileInfo qfi_measdoc(atsh->absolutePath() + "/" + atsh->svalue("xml_header"));
                if (qfi_measdoc.exists()) {
                    measdoc  = std::make_unique<measdocxml>(&qfi_measdoc, nullptr, nullptr);
                }

                if (qfi_measdoc.exists() && (measdoc != nullptr)) {
                    measdoc->slot_update_atswriter(atsh->get_atswriter_section(), atsh->ivalue("channel_number"), atsh->absolutePath(), atsh->get_measdoc_name());
                    measdoc->set_date_time(atsh->get_start_datetime(), atsh->get_stop_datetime());
                    measdoc->set_simple_section("HwStatus", "GPS", atsh->get_gps_section());
                    measdoc->save(&qfi_measdoc);
                }
            }
        }

        exit(0);
    }

    QString sqlbd = procmt_homepath("info.sql3");
    QString dbname = sqlbd;
    atsedit w(dbname);
    QFileInfo fname;
    QList<QUrl> urls;
    for (auto &name : a.arguments()) {
        if (name.endsWith(".ats", Qt::CaseInsensitive)) {

            fname.setFile(name);
            break;
        }
    }
    if (fname.exists()) w.open_urls(urls, fname.absoluteFilePath());
    w.show();

    return a.exec();
}
