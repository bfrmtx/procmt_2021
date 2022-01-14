#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->qfi_db.setFile(procmt_homepath("info.sql3"));
    this->qfi_master.setFile(procmt_homepath("master_calibration.sql3"));
    this->cal = std::make_shared<calibration>(qfi_db, -1, -1, true, "db_sensor_query_cal2sql", this);
    this->cal->open_master_cal(this->qfi_master);


    this->dialog = new QFileDialog(this);
    this->selcoil = new select_coil(this);
    this->callib = new xmlcallib(this->qfi_db, this->qfi_master, this);
    this->xy_plot = new xy_regression_plot(this);
    this->mode = -1;


    // that is done during init and giving QParent
    //QObject::connect(this->cal.get(), &calibration::tx_cal_message, this, &MainWindow::rx_cal_message);
    //QObject::connect(this->dialog, QFileDialog::accepted, this, &MainWindow::scan_db);
    QObject::connect(this->dialog, SIGNAL(accepted()), this, SLOT(scan_db()));
    QObject::connect(this->selcoil, &select_coil::coil_selcted, this, &MainWindow::dlg_coil_selected);
    QObject::connect(this, &MainWindow::plot_cal, this->callib, &xmlcallib::slot_set_cal);
    this->ui->plot_pushButton->setDisabled(true);
    this->ui->plot_append_pushButton->setDisabled(true);

    qDebug() << "starting ..";

}

MainWindow::~MainWindow()
{

    delete this->dialog;
    delete this->selcoil;
    delete this->callib;
    delete this->xy_plot;
    delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {

    event->acceptProposedAction();

}

void MainWindow::scan_db()
{

    if (this->mode != -2) qfi.setFile(this->dialog->selectedFiles().at(0));
    if (this->mode == -2) this->mode = 0; // swtich from drag & drop to open mode

    if (mode == 1) {
        if (qfi.completeSuffix().size() == 0) qfi.setFile(this->dialog->selectedFiles().at(0) + ".sql3");
    }
    QString message = this->qfi.absoluteFilePath() + " opened";
    this->rx_cal_message(-1, -1, message);
    this->cal->open_create_db(qfi.fileName(), qfi.absoluteDir());

    bool created = false;
    // 1 = create
    if (this->mode) {
        if (!this->cal->prepare_cal_db()) {
            qDebug() << "main can not create cal_db cal table";
        }

        if (!this->cal->prepare_info_table()) {
            qDebug() << "main can not create info table";
        }
        created = true;
        this->mode = 0;
    }

    this->cal->sensors_summary();

    // if freshly created we have no coils
    if (!created) this->alldb = this->cal->all_sensors_in_db();

    this->selcoil->set_coils(this->cal->get_selectable_sensors(), this->alldb);




    this->dirname = QFileDialog::getExistingDirectory(this, tr("Open Output Directory for txt files"),
                                                      qfi.absolutePath(),
                                                      QFileDialog::ShowDirsOnly
                                                      | QFileDialog::DontResolveSymlinks);


    if (qfi.baseName().size()) {
        this->setWindowTitle("Cal2SQL -> " +  qfi.completeBaseName());
    }

}

void MainWindow::dlg_coil_selected(const QString &coil, const int &serno)
{


    this->coil = coil;
    this->serno = serno;

    QString tmpcoil = this->coil;
    tmpcoil.remove("-");


    if (!this->cal->select_coil(coil, serno, true)) return;
    this->cal->get_master_cal();

    QFileInfo qfix;


    qfix.setFile(this->dirname + "/" + tmpcoil + QString::number(serno).rightJustified(4, '0') + ".txt");

    this->cal->write_std_txt_file(qfix);


    this->ui->plot_pushButton->setDisabled(false);
    this->ui->plot_pushButton->setText("plot: " + this->coil + " " + QString::number(serno).rightJustified(4, '0'));
    this->ui->plot_append_pushButton->setDisabled(false);
    this->ui->plot_append_pushButton->setText("plot (append): " + this->coil + " " + QString::number(serno).rightJustified(4, '0'));


}

void MainWindow::rx_cal_message(const int &channel, const int &slot, const QString &message)
{
    this->ui->qlw->addItem(message);
    this->qts_buffer.append(message);
    this->ui->qlw->update();
    this->ui->qlw->scrollToBottom();
    this->ui->qlw->update();

}
void MainWindow::dropEvent(QDropEvent *event) {


    // 0 is open

    int i;
    QFileInfo qfix;


    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) {
        return;
    }

    else {

        for (i = 0; i < urls.size(); ++i) {
            qfix.setFile(urls.at(i).toLocalFile());

            if ( (urls.size() == 1 ) && (qfix.absoluteFilePath().endsWith("sql3") || qfix.absoluteFilePath().endsWith("db3") ||  qfix.absoluteFilePath().endsWith("db") ||  qfix.absoluteFilePath().endsWith("sql")) ) {

                this->qfi.setFile(qfix.absoluteFilePath());
                this->mode = -2;
                this->scan_db();
                return;
            }


            if (this->cal->read_std_txt_file(-1, qfix)) {

                if (!this->cal->insert_into_cal_db()) {
                    qDebug() << "main can not insert into cal_db cal";
                }
            }
        }

        this->cal->sensors_summary();
        this->alldb = this->cal->all_sensors_in_db();

        this->selcoil->set_coils(this->cal->get_selectable_sensors(), this->alldb);


    }
}

void MainWindow::on_actionOpenDataBase_triggered()
{
    qDebug() << "open SQLite Database";
    this->mode = 0;
    dialog->setViewMode(QFileDialog::Detail);
    dialog->setFileMode(QFileDialog::AnyFile);
    dialog->setModal(true);
    dialog->setNameFilter(tr("Databases (*.*sql* *.*db*)"));
    dialog->show();


}

void MainWindow::on_actionGetCalibration_triggered()
{

    this->selcoil->show();
}

void MainWindow::on_actionDumpAll_triggered()
{
    // from all tables

    // get tables SELECT name FROM sqlite_master WHERE type='table' AND name != 'sensortypes'
    // get serials SELECT DISTINCT serial FROM `ser mfs-06e`  and so forth
    // ... start to dump


    qDebug() << "start multi query";


    QMultiMap<QString, int>::const_iterator i;

    QFileInfo qfix;

    for (i = this->alldb.constBegin(); i != this->alldb.constEnd(); ++i) {

        QString cn = this->cal->set_atsheader_sensor(i.key());
        cn = cn.remove("-");
        qfix.setFile(this->dirname + "/" + cn + QString::number(i.value()).rightJustified(4, '0') + ".txt");
        if ( this->cal->select_coil(i.key(), i.value(), true) ) {
            this->cal->write_std_txt_file(qfix);
        }

    }





}



void MainWindow::on_actionOpenCreate_triggered()
{


    qDebug() << "create SQLite Database";
    this->mode = 1;
    dialog->setViewMode(QFileDialog::Detail);
    dialog->setFileMode(QFileDialog::AnyFile);
    dialog->setModal(true);
    dialog->setNameFilter(tr("Databases (*.*sql* *.*db*)"));
    dialog->show();

}

void MainWindow::on_actionQuit_triggered()
{
    this->close();
}


void MainWindow::on_plot_pushButton_clicked()
{
    emit this->plot_cal(this->cal, true);
    this->callib->show();
}

void MainWindow::on_getcal_pushButton_clicked()
{
    this->on_actionGetCalibration_triggered();
}

void MainWindow::on_plot_append_pushButton_clicked()
{
    emit this->plot_cal(this->cal, false);
    this->callib->show();
}

void MainWindow::on_actionStats_triggered()
{

    if (alldb.size() < 3) {
        // in order to make statisics we need mor than 4 coils

        QString message;
        message = "database too small for statistics, size must be greater than 3, ";
        message += "size is " + QString::number(this->alldb.size());
        this->rx_cal_message(-1, -1,  message);
        return;
    }

    qDebug() << "stats for" << this->coil;
    QMultiMap<QString, int>::const_iterator i;

    QList<calstat> allfs;
    bool contains = false;


    for (i = this->alldb.constBegin(); i != this->alldb.constEnd(); ++i) {

        QString cn = this->cal->set_atsheader_sensor(i.key());
        if (!this->coil.compare(i.key(), Qt::CaseInsensitive)) {
            qDebug() << i.key() << i.value();
            this->cal->sensortype = i.key();
            this->cal->sernum = i.value();
            if ( this->cal->select_coil(i.key(), i.value(), true)) {

                qDebug() << "found coil in db" << i.key() << i.value();
                contains = false;
                for (size_t j = 0; j < this->cal->f_on.size(); ++j) {
                    for (auto &all : allfs) {
                        if (all.chopper == cal::chopper_on) {
                            if (all.add_cal(this->cal->f_on.at(j), this->cal->sernum , this->cal->ampl_on.at(j), this->cal->phase_grad_on.at(j))) {
                                contains = true;
                                // qDebug() << "adding to " << this->cal->f_on.at(j) << this->cal->phase_grad_on.at(j);
                            }
                        }
                    }
                    if (!contains) {
                        allfs.append(calstat(this->cal->f_on.at(j), cal::chopper_on, 3));
                        allfs.last().add_cal(this->cal->f_on.at(j), this->cal->sernum, this->cal->ampl_on.at(j), this->cal->phase_grad_on.at(j));
                        // qDebug() << "creating    " << this->cal->f_on.at(j);
                    }
                }
                contains = false;
                for (size_t j = 0; j < this->cal->f_off.size(); ++j) {
                    for (auto &all : allfs) {
                        if (all.chopper == cal::chopper_off) {
                            if (all.add_cal(this->cal->f_off.at(j), this->cal->sernum , this->cal->ampl_off.at(j), this->cal->phase_grad_off.at(j))) {
                                contains = true;
                                // qDebug() << "adding to " << this->cal->f_off.at(j) << this->cal->phase_grad_off.at(j);
                            }
                        }
                    }
                    if (!contains) {
                        allfs.append(calstat(this->cal->f_off.at(j), cal::chopper_off, 3));
                        allfs.last().add_cal(this->cal->f_off.at(j), this->cal->sernum, this->cal->ampl_off.at(j), this->cal->phase_grad_off.at(j));
                        // qDebug() << "creating    " << this->cal->f_off.at(j);
                    }
                }



                qDebug() << "--------------";
            }
        }

    }


    this->csdpl.reset();
    this->csdpl = std::make_unique<cal_stat_display_lib>(this->qfi_db, this->qfi_master, this);

    // create a new calibration we do need the db but have to supply
    //calibration sumcal(qfidb, -1);

    // copy types and set to number 1
    this->csdpl->sumcal->sensortype = this->cal->sensortype;
    this->csdpl->sumcal->sernum = 9999;
    this->csdpl->sumcal->caltime = QDateTime::currentDateTime();
    //        this->csdpl->sumcal->get_master_cal();

    // SATISTICS for ON and OFF , copy for ON and OFF
    for (auto &item : allfs) {
        item.median_variance_ampl();
        item.median_variance_phase();
        if ((item.chopper == cal::chopper_on) && (item.variance_ampls_data.size() == g_stat::xstat_size)) {
            this->csdpl->sumcal->f_on.push_back(item.f);
            this->csdpl->sumcal->ampl_on.push_back(item.variance_ampls_data.at(g_stat::median_x) );
            this->csdpl->sumcal->phase_grad_on.push_back(item.variance_phzs_data.at(g_stat::median_x));
            this->csdpl->sumcal->ampl_on_stddev.push_back(item.variance_ampls_data.at(g_stat::stddev_x) );
            this->csdpl->sumcal->phase_grad_on_stddev.push_back(item.variance_phzs_data.at(g_stat::stddev_x));
        }
        if (item.chopper == cal::chopper_off && (item.variance_ampls_data.size() == g_stat::xstat_size)) {
            this->csdpl->sumcal->f_off.push_back(item.f);
            this->csdpl->sumcal->ampl_off.push_back(item.variance_ampls_data.at(g_stat::median_x) );
            this->csdpl->sumcal->phase_grad_off.push_back(item.variance_phzs_data.at(g_stat::median_x));
            this->csdpl->sumcal->ampl_off_stddev.push_back(item.variance_ampls_data.at(g_stat::stddev_x) );
            this->csdpl->sumcal->phase_grad_off_stddev.push_back(item.variance_phzs_data.at(g_stat::stddev_x));

        }

    }






    QFileInfo qfixc;
    QString tmpcoil = this->csdpl->sumcal->sensortype;
    tmpcoil.remove("-");
    qfixc.setFile(this->dirname + "/" + tmpcoil + QString::number(this->csdpl->sumcal->sernum).rightJustified(4, '0') + ".txt");
    this->csdpl->sumcal->write_std_txt_file(qfixc);

    qfixc.setFile(this->dirname + "/" + "calresult_" + this->cal->sensortype + ".csv");
    this->csdpl->sumcal->write_csv_table(qfixc);

    // sort by serials
    math_vector mathv;
    for (auto &item : allfs) {
        mathv.sort_vectors(item.serials, item.ampls, item.phzs);
    }


    this->csdpl->set_calstat(allfs);
    allfs.clear();
    this->csdpl->setWindowTitle(tmpcoil);
    this->csdpl->show();



    qDebug() << "stat finished collect";
    //        qDebug() << allfs_on.at(3).f ;
    //        this->xy_plot->set_data(allfs_on.at(3).serials_to_double(), allfs_on.at(3).phzs);
    //        this->xy_plot->plot_tp->setWindowTitle("Phase on at " + QString::number(allfs_on.at(3).f));




}

void MainWindow::on_qlw_itemDoubleClicked(QListWidgetItem *item)
{
    QString str_item(item->text());
    str_item = str_item.split(" ").at(0);
    if (this->cal->get_selectable_sensors().contains(str_item)) {
        this->selcoil->preselect(str_item);
        this->selcoil->on_Sensor_comboBox_activated(str_item);
        this->selcoil->on_Sensor_comboBox_currentTextChanged(str_item);

    }
    this->selcoil->show();
}

void MainWindow::on_actionWrite_LogFile_triggered()
{
    this->qfi_log.setFile(this->dirname + "/" + this->cal->sensortype + "cal2sql.log");

    QFile file(this->qfi_log.absoluteFilePath());
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream log_out;
        log_out.setDevice(&file);
        for (auto &s : this->qts_buffer) {
            log_out << s << Qt::endl;
        }
    }
    file.close();
    this->qts_buffer.clear();
}
