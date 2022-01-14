#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    int i = 0;
    int old_size;

    this->setWindowTitle("generate ADB calfile name");

    this->sys = new QButtonGroup(this->ui->hzl_system);

    this->sys_btns.append(new QRadioButton("ADU-08e",this));
    this->sys_btns.append(new QRadioButton("ADU-07e",this));
    this->sys_btns.append(new QRadioButton("ADU-09e",this));

    for (auto &btn : this->sys_btns) {
        this->sys->addButton(btn, i++);
        btn->setCheckable(true);
        this->ui->hzl_system->addWidget(btn);
    }
    this->ui->hzl_system->addStretch();
    old_size = this->sys_btns.size();
    connect(sys, &QButtonGroup::idClicked, this, &MainWindow::slot_sys_changed);

    // ---------------------------------------------------------------------------------

    this->adb = new QButtonGroup(this->ui->hzl_board);
    this->sys_btns.append(new QRadioButton("ADB08e-HF",this));
    this->sys_btns.append(new QRadioButton("ADB08e-LF",this));
    this->sys_btns.append(new QRadioButton("ADB07e-HF",this));
    this->sys_btns.append(new QRadioButton("ADB07e-LF",this));

    connect(adb, &QButtonGroup::idClicked, this, &MainWindow::slot_sys_changed);


    int j = 0;
    for (auto &btn : this->sys_btns) {
        if (j++ >= old_size ) {
            this->adb->addButton(btn, i++);
            btn->setCheckable(true);
            this->ui->hzl_board->addWidget(btn);
        }
    }
    this->ui->hzl_board->addStretch();
    old_size = this->sys_btns.size();


    // ---------------------------------------------------------------------------------

    this->div = new QButtonGroup(this->ui->hzl_div);
    this->sys_btns.append(new QRadioButton("DIV-1", this));
    this->sys_btns.append(new QRadioButton("DIV-8", this));
    connect(div, &QButtonGroup::idClicked, this, &MainWindow::slot_sys_changed);


    j = 0;
    for (auto &btn : this->sys_btns) {
        if (j++ >= old_size ) {
            this->div->addButton(btn, i++);
            btn->setCheckable(true);
            this->ui->hzl_div->addWidget(btn);
        }
    }
    this->ui->hzl_div->addStretch();
    old_size = this->sys_btns.size();

    // ---------------------------------------------------------------------------------

    this->gain_1 = new QButtonGroup(this->ui->hzl_gain_1);
    this->sys_btns.append(new QRadioButton("Gain1-1", this));
    this->sys_btns.append(new QRadioButton("Gain1-4", this));
    this->sys_btns.append(new QRadioButton("Gain1-8", this));
    this->sys_btns.append(new QRadioButton("Gain1-16", this));
    connect(gain_1, &QButtonGroup::idClicked, this, &MainWindow::slot_sys_changed);


    j = 0;
    for (auto &btn : this->sys_btns) {
        if (j++ >= old_size ) {
            this->gain_1->addButton(btn, i++);
            btn->setCheckable(true);
            this->ui->hzl_gain_1->addWidget(btn);
        }
    }
    this->ui->hzl_gain_1->addStretch();
    old_size = this->sys_btns.size();





    // ---------------------------------------------------------------------------------

    this->dac = new QButtonGroup(this->ui->hzl_dac);
    this->sys_btns.append(new QRadioButton("DAC-on", this));
    this->sys_btns.append(new QRadioButton("DAC-off", this));
    connect(dac, &QButtonGroup::idClicked, this, &MainWindow::slot_sys_changed);


    j = 0;
    for (auto &btn : this->sys_btns) {
        if (j++ >= old_size ) {
            this->dac->addButton(btn, i++);
            btn->setCheckable(true);
            this->ui->hzl_dac->addWidget(btn);
        }
    }
    this->ui->hzl_dac->addStretch();
    old_size = this->sys_btns.size();

    // ---------------------------------------------------------------------------------

    this->gain_2 = new QButtonGroup(this->ui->hzl_gain_2);
    this->sys_btns.append(new QRadioButton("Gain2-1", this));
    this->sys_btns.append(new QRadioButton("Gain2-4", this));
    this->sys_btns.append(new QRadioButton("Gain2-8", this));
    this->sys_btns.append(new QRadioButton("Gain2-16", this));
    this->sys_btns.append(new QRadioButton("Gain2-32", this));
    this->sys_btns.append(new QRadioButton("Gain2-64", this));
    connect(gain_2, &QButtonGroup::idClicked, this, &MainWindow::slot_sys_changed);


    j = 0;
    for (auto &btn : this->sys_btns) {
        if (j++ >= old_size ) {
            this->gain_2->addButton(btn, i++);
            btn->setCheckable(true);
            this->ui->hzl_gain_2->addWidget(btn);
        }
    }
    this->ui->hzl_gain_2->addStretch();
    old_size = this->sys_btns.size();


    // ---------------------------------------------------------------------------------

    this->rf = new QButtonGroup(this->ui->hzl_rf);
    this->sys_btns.append(new QRadioButton("LF-RF-1", this));
    this->sys_btns.append(new QRadioButton("LF-RF-2", this));
    this->sys_btns.append(new QRadioButton("LF-RF-3", this));
    this->sys_btns.append(new QRadioButton("LF-RF-4", this));
    this->sys_btns.append(new QRadioButton("LF-RF-off", this));
    connect(rf, &QButtonGroup::idClicked, this, &MainWindow::slot_sys_changed);


    j = 0;
    for (auto &btn : this->sys_btns) {
        if (j++ >= old_size ) {
            this->rf->addButton(btn, i++);
            btn->setCheckable(true);
            this->ui->hzl_rf->addWidget(btn);
        }
    }
    this->ui->hzl_rf->addStretch();
    old_size = this->sys_btns.size();

    // ---------------------------------------------------------------------------------

    this->filter_lf = new QButtonGroup(this->ui->hzl_filter_lf);
    this->sys_btns.append(new QRadioButton("LF-LP-4Hz", this));
    this->sys_btns.append(new QRadioButton("LF-LP-off", this));

    connect(filter_lf, &QButtonGroup::idClicked, this, &MainWindow::slot_sys_changed);


    j = 0;
    for (auto &btn : this->sys_btns) {
        if (j++ >= old_size ) {
            this->filter_lf->addButton(btn, i++);
            btn->setCheckable(true);
            this->ui->hzl_filter_lf->addWidget(btn);
        }
    }
    this->ui->hzl_filter_lf->addStretch();
    old_size = this->sys_btns.size();


    this->filter_hf = new QButtonGroup(this->ui->hzl_filter_hf);
    this->sys_btns.append(new QRadioButton("HF-HP-500Hz", this));
    this->sys_btns.append(new QRadioButton("HF-HP-1Hz", this));
    this->sys_btns.append(new QRadioButton("HF-HP-off", this));
    connect(filter_hf, &QButtonGroup::idClicked, this, &MainWindow::slot_sys_changed);


    j = 0;
    for (auto &btn : this->sys_btns) {
        if (j++ >= old_size ) {
            this->filter_hf->addButton(btn, i++);
            btn->setCheckable(true);
            this->ui->hzl_filter_hf->addWidget(btn);
        }
    }
    this->ui->hzl_filter_hf->addStretch();
    old_size = this->sys_btns.size();


    this->set_default();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slot_sys_changed(int id)
{
    int i = 0;
    this->ui->lineEdit->clear();
    QString ltext("TRF_");
    qDebug() << "sys btn" << id;
    for (auto &btn : this->sys_btns) {
        if (btn->isChecked()) {
            qDebug() << "checked" <<  btn->text();
            ltext.append(btn->text());
            ltext.append("_");
        }
        ++i;
    }
    ltext.append("SensorResistance-"); ltext.append(QString::number(this->ui->spinBox_sens_res->value()));
    ltext.append(".txt");
    this->ui->lineEdit->setText(ltext);
}

void MainWindow::set_default()
{
    int i = 0;
    for (auto const &btn : this->sys_btns) {
        all_btns.insert(btn->text(), i++);
    }

    this->sys_btns[all_btns.value("HF-HP-off")]->click();
    this->sys_btns[all_btns.value("LF-LP-off")]->click();
    this->sys_btns[all_btns.value("Gain2-1")]->click();
    this->sys_btns[all_btns.value("DAC-off")]->click();
    this->sys_btns[all_btns.value("ADU-08e")]->click();
}



void MainWindow::on_spinBox_sens_res_valueChanged(int arg1)
{
    this->slot_sys_changed(100);
}

void MainWindow::on_checkBox_coil_clicked(bool checked)
{
    if (checked) {
        this->ui->spinBox_sens_res->setValue(200000);
        this->ui->spinBox_sens_res->setDisabled(true);
    }
    else {
        this->ui->spinBox_sens_res->setEnabled(true);
    }
    this->slot_sys_changed(100);
}
