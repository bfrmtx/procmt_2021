#include "mk_master_cal.h"
#include "ui_mk_master_cal.h"

mk_master_cal::mk_master_cal(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mk_master_cal)
{
    ui->setupUi(this);
    this->setAcceptDrops(true);
    this->is_ready = false;
    this->qfi_db =  fall_back_default_str("info.sql3");

    this->scbox = std::make_unique<sensors_combo>( this->qfi_db.absoluteFilePath(), 'H', this->ui->sensor_box);

    this->ui->pushButton_make_master->setDisabled(true);

    QObject::connect(&this->pctab, &prc_com_table::prc_com_table_message, this, &mk_master_cal::rx_cal_message);
}

mk_master_cal::~mk_master_cal()
{
    delete ui;
    this->inmaster.reset();
    this->extf.reset();
    this->outmaster.reset();
    this->scbox.reset();

}

void mk_master_cal::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();

}

void mk_master_cal::dropEvent(QDropEvent *event)
{
    QFileInfo qfix;

    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) {
        return;
    }

    qfix.setFile(urls.at(0).toLocalFile());
    if (!qfix.exists()) return;

    this->qfi_out.setFile(qfix.absoluteDir(), "outnewmaster.txt");

    this->ui->qlw->clear();

    int lc = 0;
    lc = this->pctab.read_cvs_table(qfix);
    if (!lc) return;
    lc = this->pctab.sort_csv_table_with_chopper("f", "chopper");
    if (!lc) return;

    this->inmaster.reset();
    this->outmaster.reset();

    // at least we have got something
    /*
     *
     * the "this" in the constructor tries to connect to tx_cal_message to rx_cal_message counter part
     *
     */
    this->inmaster = std::make_unique<calibration>(this->qfi_db, -1, -1, true, "db_sensor_query", this);
    //QObject::connect(this->inmaster.get(), &db_sensor_query::cal_message, this, &mk_master_cal::rx_calmessage);

    // get the data which can be chopper on and chopper of ONLY

    this->inmaster->f_on = this->pctab.get_dvector_column("f", "csv_data_on");
    this->inmaster->ampl_on = this->pctab.get_dvector_column("a", "csv_data_on");
    this->inmaster->phase_grad_on = this->pctab.get_dvector_column("p", "csv_data_on");
    this->inmaster->ampl_on_stddev = this->pctab.get_dvector_column("stddev a", "csv_data_on");
    this->inmaster->phase_grad_on_stddev = this->pctab.get_dvector_column("stddev p", "csv_data_on");

    this->inmaster->f_off = this->pctab.get_dvector_column("f", "csv_data_off");
    this->inmaster->ampl_off = this->pctab.get_dvector_column("a", "csv_data_off");
    this->inmaster->phase_grad_off = this->pctab.get_dvector_column("p", "csv_data_off");
    this->inmaster->ampl_off_stddev = this->pctab.get_dvector_column("stddev a", "csv_data_off");
    this->inmaster->phase_grad_off_stddev = this->pctab.get_dvector_column("stddev p", "csv_data_off");

    if (this->inmaster->f_on.size() < 3 || this->inmaster->f_off.size() < 3) {
        message = "not enough values in master cal table";
        qDebug() << "not enough values in master cal table";

        this->rx_cal_message(-1, -1, message);
        return;
    }

    this->inmaster->sort_lower_to_upper_check_duplicates(cal::calibration, cal::chopper_on);
    this->inmaster->sort_lower_to_upper_check_duplicates(cal::calibration, cal::chopper_off);


    this->ui->inmaster->setText(qfix.fileName());


    if ((this->inmaster != nullptr)) {

        // connect to tx_cal_message to rx_cal_message counter part
        this->outmaster = std::make_shared<calibration>(this->qfi_db, -1, -1, true, "db_sensor_query_two", this);
        this->outmaster->sernum = 111111;
        this->is_ready = true;
        this->ui->pushButton_make_master->setEnabled(true);
        on_sensor_box_currentTextChanged(this->ui->sensor_box->currentText());
    }

}

void mk_master_cal::on_sensor_box_currentTextChanged(const QString &arg1)
{
    if (this->is_ready) {

        this->coilname = arg1;
        qDebug() << this->coilname << "selected";
        this->outmaster->sensortype = this->coilname;
        this->inmaster->sensortype = this->coilname;


    }
}

void mk_master_cal::rx_cal_message(const int &channel, const int &slot, const QString &message)
{
    this->ui->qlw->addItem(message);
    this->ui->qlw->update();
    this->ui->qlw->scrollToBottom();
    this->ui->qlw->update();
}

void mk_master_cal::on_pushButton_make_master_clicked()
{

    // generate the frequency lists for chopper on/off

    if (this->coilname == "MFS-06e") {
        this->outmaster->chopper = cal::chopper_on;
        this->outer_fon = this->outmaster->gen_cal_freqs(1E-6, this->inmaster->f_on.at(0), 11);
        this->inner_fon = this->outmaster->gen_cal_freqs(1E-6, +4.3288E+04, 11);

        this->outmaster->chopper = cal::chopper_off;
        this->outer_foff = this->outmaster->gen_cal_freqs(0.01, this->inmaster->f_off.at(0), 11);
        this->inner_foff = this->outmaster->gen_cal_freqs(0.01, +4.3288E+04, 11);

        // reset to default
        this->outmaster->chopper = cal::chopper_on;

    }
    if (this->coilname == "MFS-07e") {
        this->outmaster->chopper = cal::chopper_on;
        this->outer_fon = this->outmaster->gen_cal_freqs(1E-6, this->inmaster->f_on.at(0), 11);
        this->inner_fon = this->outmaster->gen_cal_freqs(1E-6, 50000, 11);


        this->outmaster->chopper = cal::chopper_off;
        this->outer_foff = this->outmaster->gen_cal_freqs(0.01, this->inmaster->f_off.at(0), 11);
        this->inner_foff = this->outmaster->gen_cal_freqs(0.01, 50000, 11);

        // reset to default
        this->outmaster->chopper = cal::chopper_on;
    }

    if (this->coilname == "MFS-09u") {
        this->outmaster->chopper = cal::chopper_on;
        this->outer_fon = this->outmaster->gen_cal_freqs(1E-6, this->inmaster->f_on.at(0), 11);
        this->inner_fon = this->outmaster->gen_cal_freqs(1E-6, 4096, 11);


        this->outmaster->chopper = cal::chopper_off;
        this->outer_foff = this->outmaster->gen_cal_freqs(0.01, this->inmaster->f_off.at(0), 11);
        this->inner_foff = this->outmaster->gen_cal_freqs(0.01, 4096, 11);

        // reset to default
        this->outmaster->chopper = cal::chopper_on;
    }

    this->outmaster->caltime = QDateTime::currentDateTime();

    // if deleta would be zero we would NOT get a std dev
    double dad = 0.03;
    double dpg = 0.5;

    QFileInfo qfi_masterdb(this->qfi_db.absolutePath() + "/" + "master_calibration.sql3");

    if ( (this->coilname == "MFS-06e") || (this->coilname == "MFS-07e") || (this->coilname == "MFS-09u")) {

        // here the theoretical will be calculated for ON
        this->inmaster->chopper = cal::chopper_on;
        this->inmaster->gen_theo_cal(this->outer_fon, dad, dpg);

        // merge the theoretical with the data

        //remove the overlapping f
        this->inmaster->f_on_theo.pop_back();
        this->inmaster->f_on_theo.insert(this->inmaster->f_on_theo.end(), this->inmaster->f_on.begin(), this->inmaster->f_on.end());

        // average firs last to avoid a (not likely) jump
        this->inmaster->ampl_on[0] = (this->inmaster->ampl_on[0] + this->inmaster->ampl_on_theo.back()) / 2.0;
        this->inmaster->phase_grad_on[0] = (this->inmaster->phase_grad_on[0] + this->inmaster->phase_grad_on_theo.back()) / 2.0;

        this->inmaster->ampl_on_theo.pop_back();
        this->inmaster->ampl_on_theo.insert(this->inmaster->ampl_on_theo.end(),   this->inmaster->ampl_on.begin(), this->inmaster->ampl_on.end());
        this->inmaster->phase_grad_on_theo.pop_back();
        this->inmaster->phase_grad_on_theo.insert(this->inmaster->phase_grad_on_theo.end(),   this->inmaster->phase_grad_on.begin(), this->inmaster->phase_grad_on.end());
        this->inmaster->ampl_on_theo_stddev.pop_back();
        this->inmaster->ampl_on_theo_stddev.insert(this->inmaster->ampl_on_theo_stddev.end(), this->inmaster->ampl_on_stddev.begin(), this->inmaster->ampl_on_stddev.end());
        this->inmaster->phase_grad_on_theo_stddev.pop_back();
        this->inmaster->phase_grad_on_theo_stddev.insert(this->inmaster->phase_grad_on_theo_stddev.end(), this->inmaster->phase_grad_on_stddev.begin(), this->inmaster->phase_grad_on_stddev.end());

        // copy the data into new cal
        this->outmaster->f_on = this->inmaster->f_on_theo;
        this->outmaster->ampl_on = this->inmaster->ampl_on_theo;
        this->outmaster->phase_grad_on = this->inmaster->phase_grad_on_theo;
        this->outmaster->ampl_on_stddev = this->inmaster->ampl_on_theo_stddev;
        this->outmaster->phase_grad_on_stddev = this->inmaster->phase_grad_on_theo_stddev;

        // here the theoretical will be calculated for OFF
        this->inmaster->chopper = cal::chopper_off;
        this->inmaster->gen_theo_cal(this->outer_foff, dad, dpg);

        // merge the theoretical with the data

        //remove the overlapping f
        this->inmaster->f_off_theo.pop_back();
        this->inmaster->f_off_theo.insert(this->inmaster->f_off_theo.end(), this->inmaster->f_off.begin(), this->inmaster->f_off.end());

        // average firs last to avoid a (not likely) jump
        this->inmaster->ampl_off[0] = (this->inmaster->ampl_off[0] + this->inmaster->ampl_off_theo.back()) / 2.0;
        this->inmaster->phase_grad_off[0] = (this->inmaster->phase_grad_off[0] + this->inmaster->phase_grad_off_theo.back()) / 2.0;

        this->inmaster->ampl_off_theo.pop_back();
        this->inmaster->ampl_off_theo.insert(this->inmaster->ampl_off_theo.end(),   this->inmaster->ampl_off.begin(), this->inmaster->ampl_off.end());
        this->inmaster->phase_grad_off_theo.pop_back();
        this->inmaster->phase_grad_off_theo.insert(this->inmaster->phase_grad_off_theo.end(),   this->inmaster->phase_grad_off.begin(), this->inmaster->phase_grad_off.end());
        this->inmaster->ampl_off_theo_stddev.pop_back();
        this->inmaster->ampl_off_theo_stddev.insert(this->inmaster->ampl_off_theo_stddev.end(), this->inmaster->ampl_off_stddev.begin(), this->inmaster->ampl_off_stddev.end());
        this->inmaster->phase_grad_off_theo_stddev.pop_back();
        this->inmaster->phase_grad_off_theo_stddev.insert(this->inmaster->phase_grad_off_theo_stddev.end(), this->inmaster->phase_grad_off_stddev.begin(), this->inmaster->phase_grad_off_stddev.end());

        // copy the data into new cal
        this->outmaster->f_off = this->inmaster->f_off_theo;
        this->outmaster->ampl_off = this->inmaster->ampl_off_theo;
        this->outmaster->phase_grad_off = this->inmaster->phase_grad_off_theo;
        this->outmaster->ampl_off_stddev = this->inmaster->ampl_off_theo_stddev;
        this->outmaster->phase_grad_off_stddev = this->inmaster->phase_grad_off_theo_stddev;






        this->qfi_out.setFile(this->qfi_out.absoluteDir(), this->coilname + "_newmaster.txt");
        this->outmaster->write_std_txt_file(this->qfi_out);
        this->qfi_out.setFile(this->qfi_out.absoluteDir(), this->coilname + "_newmaster.csv");

        outmaster->write_csv_table(this->qfi_out);

        this->outmaster->open_master_cal(qfi_masterdb, "ccnd");
        this->outmaster->get_master_cal();


        this->inmaster->chopper = cal::chopper_on;
        this->inmaster->gen_theo_cal(this->inner_fon, dad, dpg);

        this->inmaster->chopper = cal::chopper_off;
        this->inmaster->gen_theo_cal(this->inner_fon, dad, dpg);

    }



    xmlcal = std::make_unique<xmlcallib>(this->qfi_db, qfi_masterdb, this);
    xmlcal->slot_set_cal(this->outmaster,true);

    xmlcal->show();



}
