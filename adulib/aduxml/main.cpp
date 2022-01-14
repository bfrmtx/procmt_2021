#include <QApplication>
#include <QObject>
#include <QtGui>
#include <QTableView>
#include <QHeaderView>
#include "adulib.h"



using namespace tinyxml2;

    int main(int argc, char *argv[])
    {
       QApplication a(argc, argv);

       // change here

#ifdef bfrlnx
       QString basedir("/home/bfr/cpp/sw/procmt/adulib/Trunk/adulib/");
#endif

       QString outputfile(basedir + "job.xml");
       QString xmlconfig(basedir + "ADU07HwConfig.xml");
       QString xmlhwdb(basedir + "HwDatabase.xml");

        adulib adu, adun;
        adugui xgui;


        QObject::connect(&adu, &adulib::sensors_changed, &xgui, &adugui::slot_get_sensor_names);
        QObject::connect(&adu, &adulib::selectable_frequencies_changed, &xgui, &adugui::slot_get_selectable_frequencies);




        adu.fetch_cfg(xmlconfig);
        adu.fetch_hwdb(xmlhwdb);
        adu.build_channels();
        adu.build_hardware();

        qDebug() << " *******************************";

        qDebug() <<  adu.info("adbboard");

        qDebug() << " *******************************";


        qDebug() << " *******************************";

        qDebug() <<  adu.info("sensor");

        qDebug() << " *******************************";







        //xgui.on_button_sample_freq_value_accepted(1024);

        adu.set("sample_freq", QVariant(5000.), -1);
        adu.set("sample_freq", QVariant(128), -1);
        adu.slot_write_job(outputfile);

//        qDebug() << " copy adu set 8192 ******************";
//        adun = adu;


        QTableView *table = new QTableView(0);
        table->setModel(&adu);


        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->resizeColumnsToContents();
        table->resizeRowsToContents();
        table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        table->setAlternatingRowColors(true);
        table->horizontalHeader()->setStretchLastSection(true);
        table->resize(1400, 480);
        table->show();

        qDebug() << " finished";

        return a.exec();
    }
