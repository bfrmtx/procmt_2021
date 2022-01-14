#include "adugui.h"
#include "ui_adugui_main.h"


adugui::adugui(const QFileInfo &sqlfile, QWidget *parent) : sqlfile(sqlfile), QMainWindow(parent),  ui(new Ui::mw_adugui)

{
    ui->setupUi(this);

    this->grid_time = grid_time_rr;

    this->joblist =  std::make_shared<adujoblist>(this->grid_time, this);
    if (this->sqlfile.exists()) {
        this->sdb = std::make_shared<db_sensor_query>(sqlfile.absoluteFilePath(), -1, -1, true, "adugui");

    }
    else {
        qFatal("adugui::adugui cant ot open data base (default: info.sql3)");
    }
    // fill buttons with values

    connect(this->joblist.get(), &adujoblist::job_zero_created, this, &adugui::on_job_zero_created);

    /*
     *                     POSITIONING
     *
     */


    // on gui te tab will move in that order
    this->posbtns.append(new pos_button(this->ui->hzl_exNS, "Ex North [+]", "pos_x2", 0, false, this));
    this->posbtns.append(new pos_button(this->ui->hzl_exNS, "Ex South [-]", "pos_x1", 0, true, this));
    this->posbtns.append(new pos_button(this->ui->hzl_exEW, "Ex East  [+]", "pos_y2", 0, false, this));
    this->posbtns.append(new pos_button(this->ui->hzl_exEW, "Ex West  [-]", "pos_y1", 0, true, this));

    this->posbtns.append(new pos_button(this->ui->hzl_eyNS, "Ey North [+]", "pos_x2", 1, false, this));
    this->posbtns.append(new pos_button(this->ui->hzl_eyNS, "Ey South [-]", "pos_x1", 1, true, this));
    this->posbtns.append(new pos_button(this->ui->hzl_eyEW, "Ey East  [+]", "pos_y2", 1, false, this));
    this->posbtns.append(new pos_button(this->ui->hzl_eyEW, "Ey West  [-]", "pos_y1", 1, true, this));


    this->posbtns.append(new pos_button(this->ui->hzl_ex_down_up, "Ex [N,E] elev.", "pos_z2", 0, true, this));
    this->posbtns.append(new pos_button(this->ui->hzl_ex_down_up, "Ex [S,W] elev.", "pos_z1", 0, true, this));


    this->posbtns.append(new pos_button(this->ui->hzl_ey_down_up, "Ey [E,S] elev.", "pos_z2", 1, true, this));
    this->posbtns.append(new pos_button(this->ui->hzl_ey_down_up, "Ey [W,N] elev.", "pos_z1", 1, true, this));

    this->posbtns.append(new pos_button(this->ui->hzl_hxNS, "Hx North [+]", "pos_x2", 2, false, this));
    this->posbtns.append(new pos_button(this->ui->hzl_hxNS, "Hx South [-]", "pos_x1", 2, true, this));
    this->posbtns.append(new pos_button(this->ui->hzl_hxEW, "Hx East  [+]", "pos_y2", 2, false, this));
    this->posbtns.append(new pos_button(this->ui->hzl_hxEW, "Hx West  [-]", "pos_y1", 2, true, this));

    this->posbtns.append(new pos_button(this->ui->hzl_hyNS, "Hy North [+]", "pos_x2", 3, false, this));
    this->posbtns.append(new pos_button(this->ui->hzl_hyNS, "Hy South [-]", "pos_x1", 3, true, this));
    this->posbtns.append(new pos_button(this->ui->hzl_hyEW, "Hy East  [+]", "pos_y2", 3, false, this));
    this->posbtns.append(new pos_button(this->ui->hzl_hyEW, "Hy West  [-]", "pos_y1", 3, true, this));


    this->posbtns.append(new pos_button(this->ui->hzl_hzNS, "Hz North [+]", "pos_x2", 4, false, this));
    this->posbtns.append(new pos_button(this->ui->hzl_hzNS, "Hz South [-]", "pos_x1", 4, true, this));
    this->posbtns.append(new pos_button(this->ui->hzl_hzEW, "Hz East  [+]", "pos_y2", 4, false, this));
    this->posbtns.append(new pos_button(this->ui->hzl_hzEW, "Hz West  [-]", "pos_y1", 4, true, this));



    this->posbtns.append(new pos_button(this->ui->hzl_hx_down_up, "Hx [N,E] elev.", "pos_z2", 2, true, this));
    this->posbtns.append(new pos_button(this->ui->hzl_hx_down_up, "Hx [S,W] elev.", "pos_z1", 2, true, this));

    this->posbtns.append(new pos_button(this->ui->hzl_hy_down_up, "Hy [N,E] elev.", "pos_z2", 3, true, this));
    this->posbtns.append(new pos_button(this->ui->hzl_hy_down_up, "Hy [S,W] elev.", "pos_z1", 3, true, this));

    this->posbtns.append(new pos_button(this->ui->hzl_hz_down_up, "Hz up elev.", "pos_z2", 3, true, this));
    this->posbtns.append(new pos_button(this->ui->hzl_hz_down_up, "Hz down elev.", "pos_z1", 3, true, this));

    /*
     *                     SENSOR TYPES
     *                    the QOBJECT names will do all tricks later
     *
     */

    this->chcfgbtns.append(new triple_button(this->ui->hzl_E_sensortypes, "Ex", "EFP-XX", "0", 0, this->sdb, this));
    this->ui->hzl_E_sensortypes->addSpacing(25);
    this->chcfgbtns.append(new triple_button(this->ui->hzl_E_sensortypes, "Ey", "EFP-XX", "0", 1, this->sdb, this));
    this->chcfgbtns.append(new triple_button(this->ui->hzl_H_sensortypes, "Hx", "MFS-XX", "0", 2, this->sdb, this));
    this->ui->hzl_H_sensortypes->addSpacing(25);
    this->chcfgbtns.append(new triple_button(this->ui->hzl_H_sensortypes, "Hy", "MFS-XX", "0", 3, this->sdb, this));
    this->ui->hzl_H_sensortypes->addSpacing(25);
    this->chcfgbtns.append(new triple_button(this->ui->hzl_H_sensortypes, "Hz", "MFS-XX", "0", 4, this->sdb, this));


    this->on_set_exmode(this->exmode);

    QObject::connect(this->joblist.get(), &adujoblist::selectable_frequencies_changed, this, &adugui::on_selectable_frequencies_changed);



    this->single_jobtable =  std::unique_ptr<adutable>(new adutable(0));
    this->ssingle_jobtable =  std::unique_ptr<adutable>(new adutable(0));


    /*
 *    we may want connect update and buttons by slot
 *
 */



}

void adugui::on_job_zero_created()
{

    if (joblist->size()) {

    }
}


adugui::~adugui()
{

    delete ui;
    this->joblist.reset();


    this->single_jobtable.reset();


    this->ssingle_jobtable.reset();

    if (this->jobtimetable != nullptr) {
        this->jobtimetable->close();
        delete this->jobtimetable;
    }

    this->posbtns.clear();



    qDebug() << "adugui destroyed";

}


void adugui::edit_job(const int &index)
{


    //single_jobtable.setParent(this);

    single_jobtable->setModel(&joblist->operator[](index));
    single_jobtable->resize(1400, 480);
    single_jobtable->show();

}

void adugui::edit_job2(const int &index)
{

    //ssingle_jobtable.setParent(this);
    ssingle_jobtable->setModel(&joblist->operator[](index));
    ssingle_jobtable->resize(1400, 480);
    ssingle_jobtable->show();

}

void adugui::on_update_from_xml()
{

    // positioning
    if (this->joblist->size()) {
        for (auto &btn : this->posbtns) {
            btn->on_update_from_xml();
        }
    }

    //  sensors

    if (this->joblist->size()) {
        for (auto &btn : this->chcfgbtns) {
            btn->on_update_from_xml();
        }
    }


    // create the database and get the types



}





void adugui::on_buttonBox_accepted()
{
    this->joblist->on_save();
}

void adugui::on_buttonBox_rejected()
{
    if (this->single_jobtable != nullptr) {
        this->single_jobtable->close();
    }
    this->close();
}

void adugui::on_selectable_frequencies_changed(const std::vector<double> &selectable_frequencies)
{
    //    if (this->joblist->size()) {

    //        for (size_t i = 0; i < selectable_frequencies.size(); ++i) {
    //            QVariant val(selectable_frequencies.at(i));
    //            this->ui->select_frequency_comboBox->addItem(val.toString(), val);
    //        }
    //    }
}

void adugui::on_JobTablepushButton_clicked()
{
    if (this->jobtimetable == nullptr) this->jobtimetable = new QTableView(0);
    this->jobtimetable->setModel(this->joblist.get());

    QObject::connect(jobtimetable, &QTableView::clicked, this->joblist.get(), &adujoblist::on_clicked);



    // this->jobtimetable->resizeColumnsToContents();
    // this->jobtimetable->resizeRowsToContents();
    this->jobtimetable->setAlternatingRowColors(true);


    this->jobtimetable->resize(this->jobtimetable->width(), this->jobtimetable->height());

    //      this->jobtimetable->resizeColumnToContents(0);
    //      this->jobtimetable->resizeColumnToContents(1);
    //      this->jobtimetable->resizeColumnToContents(2);
    //      this->jobtimetable->resizeColumnToContents(3);

    //      this->jobtimetable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    this->jobtimetable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    this->jobtimetable->show();




}

void adugui::on_mw_adugui_destroyed()
{
    this->~adugui();
}

void adugui::on_lineEdit_site_name_editingFinished()
{
    this->joblist->on_set("site_name", this->ui->lineEdit_site_name->text(), -1);
}

void adugui::on_lineEdit_site_name_rr_editingFinished()
{
    this->joblist->on_set("site_name_rr", this->ui->lineEdit_site_name_rr->text(), -1);
}

void adugui::on_lineEdit_site_name_emap_editingFinished()
{
    this->joblist->on_set("site_name_emap", this->ui->lineEdit_site_name_emap->text(), -1);
}



void adugui::on_set_exmode(bool set_true_false)
{

    if (set_true_false == true) {

        for (auto &btn : this->posbtns) {
            if (btn->xml_name == "pos_y1" && btn->channel_no == 0) btn->btn->setEnabled(true);
            if (btn->xml_name == "pos_y2" && btn->channel_no == 0) btn->btn->setEnabled(true);
            if (btn->xml_name == "pos_z1" && btn->channel_no == 0) btn->btn->setEnabled(true);
            if (btn->xml_name == "pos_z2" && btn->channel_no == 0) btn->btn->setEnabled(true);

            if (btn->xml_name == "pos_x1" && btn->channel_no == 1) btn->btn->setEnabled(true);
            if (btn->xml_name == "pos_x2" && btn->channel_no == 1) btn->btn->setEnabled(true);
            if (btn->xml_name == "pos_z1" && btn->channel_no == 1) btn->btn->setEnabled(true);
            if (btn->xml_name == "pos_z2" && btn->channel_no == 1) btn->btn->setEnabled(true);

            if (btn->xml_name == "pos_x1" && btn->channel_no == 2) btn->btn->setEnabled(true);
            if (btn->xml_name == "pos_x2" && btn->channel_no == 2) btn->btn->setEnabled(true);
            if (btn->xml_name == "pos_y1" && btn->channel_no == 2) btn->btn->setEnabled(true);
            if (btn->xml_name == "pos_y2" && btn->channel_no == 2) btn->btn->setEnabled(true);
            if (btn->xml_name == "pos_z1" && btn->channel_no == 2) btn->btn->setEnabled(true);
            if (btn->xml_name == "pos_z2" && btn->channel_no == 2) btn->btn->setEnabled(true);

            if (btn->xml_name == "pos_x1" && btn->channel_no == 3) btn->btn->setEnabled(true);
            if (btn->xml_name == "pos_x2" && btn->channel_no == 3) btn->btn->setEnabled(true);
            if (btn->xml_name == "pos_y1" && btn->channel_no == 3) btn->btn->setEnabled(true);
            if (btn->xml_name == "pos_y2" && btn->channel_no == 3) btn->btn->setEnabled(true);
            if (btn->xml_name == "pos_z1" && btn->channel_no == 3) btn->btn->setEnabled(true);
            if (btn->xml_name == "pos_z2" && btn->channel_no == 3) btn->btn->setEnabled(true);

            if (btn->xml_name == "pos_x1" && btn->channel_no == 4) btn->btn->setEnabled(true);
            if (btn->xml_name == "pos_x2" && btn->channel_no == 4) btn->btn->setEnabled(true);
            if (btn->xml_name == "pos_y1" && btn->channel_no == 4) btn->btn->setEnabled(true);
            if (btn->xml_name == "pos_y2" && btn->channel_no == 4) btn->btn->setEnabled(true);
            if (btn->xml_name == "pos_z1" && btn->channel_no == 4) btn->btn->setEnabled(true);
            if (btn->xml_name == "pos_z2" && btn->channel_no == 4) btn->btn->setEnabled(true);
        }
    }
    else {
        for (auto &btn : this->posbtns) {
            if (btn->xml_name == "pos_y1" && btn->channel_no == 0) btn->btn->setEnabled(false);
            if (btn->xml_name == "pos_y2" && btn->channel_no == 0) btn->btn->setEnabled(false);
            if (btn->xml_name == "pos_z1" && btn->channel_no == 0) btn->btn->setEnabled(false);
            if (btn->xml_name == "pos_z2" && btn->channel_no == 0) btn->btn->setEnabled(false);

            if (btn->xml_name == "pos_x1" && btn->channel_no == 1) btn->btn->setEnabled(false);
            if (btn->xml_name == "pos_x2" && btn->channel_no == 1) btn->btn->setEnabled(false);
            if (btn->xml_name == "pos_z1" && btn->channel_no == 1) btn->btn->setEnabled(false);
            if (btn->xml_name == "pos_z2" && btn->channel_no == 1) btn->btn->setEnabled(false);

            if (btn->xml_name == "pos_x1" && btn->channel_no == 2) btn->btn->setEnabled(false);
            if (btn->xml_name == "pos_x2" && btn->channel_no == 2) btn->btn->setEnabled(false);
            if (btn->xml_name == "pos_y1" && btn->channel_no == 2) btn->btn->setEnabled(false);
            if (btn->xml_name == "pos_y2" && btn->channel_no == 2) btn->btn->setEnabled(false);
            if (btn->xml_name == "pos_z1" && btn->channel_no == 2) btn->btn->setEnabled(false);
            if (btn->xml_name == "pos_z2" && btn->channel_no == 2) btn->btn->setEnabled(false);

            if (btn->xml_name == "pos_x1" && btn->channel_no == 3) btn->btn->setEnabled(false);
            if (btn->xml_name == "pos_x2" && btn->channel_no == 3) btn->btn->setEnabled(false);
            if (btn->xml_name == "pos_y1" && btn->channel_no == 3) btn->btn->setEnabled(false);
            if (btn->xml_name == "pos_y2" && btn->channel_no == 3) btn->btn->setEnabled(false);
            if (btn->xml_name == "pos_z1" && btn->channel_no == 3) btn->btn->setEnabled(false);
            if (btn->xml_name == "pos_z2" && btn->channel_no == 3) btn->btn->setEnabled(false);

            if (btn->xml_name == "pos_x1" && btn->channel_no == 4) btn->btn->setEnabled(false);
            if (btn->xml_name == "pos_x2" && btn->channel_no == 4) btn->btn->setEnabled(false);
            if (btn->xml_name == "pos_y1" && btn->channel_no == 4) btn->btn->setEnabled(false);
            if (btn->xml_name == "pos_y2" && btn->channel_no == 4) btn->btn->setEnabled(false);
            if (btn->xml_name == "pos_z1" && btn->channel_no == 4) btn->btn->setEnabled(false);
            if (btn->xml_name == "pos_z2" && btn->channel_no == 4) btn->btn->setEnabled(false);

        }
    }
}



pos_button::pos_button(QBoxLayout *layout, QString label_text, QString xml_name, int channel_no, bool needs_flip, adugui *gui) :
    QObject(gui), needs_flip(needs_flip), channel_no(channel_no), xml_name(xml_name), label_text(label_text), layout(layout)
{
    this->gui = gui;
    this->btn = new QPushButton(gui);
    this->lbl = new QLabel(gui);
    this->lbl->setText(this->label_text);
    this->layout->addWidget(this->lbl);
    this->value = 0;

    this->layout->addWidget(btn);


    connect(btn, &QPushButton::clicked, this, &pos_button::on_clicked );
}

void pos_button::on_update_from_xml()
{
    if (gui->joblist->size()) {
        if (gui->joblist->job(0).is_active(this->channel_no)) {
            this->value = gui->joblist->job(0).get(this->xml_name, this->channel_no);
            if (this->needs_flip && !gui->exmode) {
                this->value = fabs(value.toDouble());
            }
            else this->value = value.toDouble();
            this->btn->setText(this->value.toString());
        }
        else {
            this->btn->setText("off");
            this->btn->setEnabled(false);
        }
    }

}


void pos_button::on_clicked()
{
    if (!gui->exmode) {
        this->keyp = std::make_unique<num_keypad>("doubles", num_keypad_InputMode::UnsignedDoubleMode, gui);
    }
    else {
        this->keyp = std::make_unique<num_keypad>("doubles", num_keypad_InputMode::DoubleMode, gui);
    }


    if (this->needs_flip && !gui->exmode) {

        this->keyp->setInitialValue(fabs(this->value.toDouble()));
    }
    else  this->keyp->setInitialValue(this->value);

    if (this->keyp->exec() == 1) {
        this->value = this->keyp->value();
        // in non expert mode we flip some vars (shown always positive BUT must be negative, like S, W)
        if (this->needs_flip && !gui->exmode) {
            this->value = fabs(value.toDouble());
            this->btn->setText(this->value.toString());
            this->value = fabs(value.toDouble()) * -1.0;
        }
        // in this mode the keypad is in signed mode
        else {
            this->btn->setText(this->value.toString());
        }
        if (gui->joblist->size()) {
            gui->joblist->on_set(this->xml_name, this->value, this->channel_no, -1);
            qDebug() << "check dipole length for" << this->channel_no << ": " << gui->joblist->job(0).get_dipole_length(this->channel_no);
        }
    }

}

triple_button::triple_button(QBoxLayout *layout, const QString channel_type, const QString sensor_type, const QString sensor_sernum, const int channel_no, std::shared_ptr<db_sensor_query> sdb, adugui *gui) :
    QObject(gui), sdb(sdb), channel_no(channel_no), sensor_sernum(sensor_sernum), sensor_type(sensor_type), channel_type(channel_type), layout(layout)
{
    this->gui = gui;
    this->setObjectName("triple_button_" + QString::number(this->channel_no));


    this->channel_type_btn = new QPushButton(this->channel_type, this->gui);
    this->sensor_type_btn = new QPushButton(this->sensor_type, this->gui);
    this->sensor_sernum_btn = new QPushButton(this->sensor_sernum, this->gui);

    //    this->channel_type_btn->setObjectName("channel_type");
    //    this->sensor_type_btn->setObjectName("sensor_type");
    //    this->sensor_sernum_btn->setObjectName("sensor_sernum");


    //db.setConnectOptions(QLatin1String("QSQLITE_OPEN_READONLY"));

    connect(channel_type_btn,  &QPushButton::clicked, this, &triple_button::on_change_channel_type );
    connect(sensor_type_btn,   &QPushButton::clicked, this, &triple_button::on_change_sensor_type );
    connect(sensor_sernum_btn, &QPushButton::clicked, this, &triple_button::on_change_sensor_sernum );

    this->layout->addWidget(this->channel_type_btn);
    this->layout->addWidget(this->sensor_type_btn);
    this->layout->addWidget(this->sensor_sernum_btn);




}
int triple_button::get_channel_no() const
{
    return this->channel_no;
}

void triple_button::on_change_channel_type()
{
    if (this->locked) return;
}

void triple_button::on_change_sensor_type()
{
    if (this->locked) return;
    if (this->sdb == nullptr) {
        qDebug() << "database not initialized!";
        return;
    }
    else qDebug() << "using connection" << this->sdb->get_connection_name();
    QStringList sensor_types;
    if (this->channel_type.startsWith("E")) {
        sensor_types = this->sdb->get_E_sensors();
    }
    else if (this->channel_type.startsWith("H")) {
        sensor_types = this->sdb->get_H_sensors();
    }
    if (!sensor_types.size()) return;

    // create the dialog
    this->sdb_select = std::make_unique<select_items_from_db_sensors>(sensor_types, 2, this->gui);
    if (this->sdb_select->exec()) {
        QString sensor_typestr = sdb_select->get_selection();
        this->sensor_type_btn->setText(sensor_typestr);
        this->gui->joblist->on_set("sensor_type", sensor_typestr, this->channel_no);

    }

}

void triple_button::on_change_sensor_sernum()
{
    if (this->locked) return;
    QVariant servalue;
    QVariant value;
    this->keyp = std::make_unique<num_keypad>("sernum", num_keypad_InputMode::UnsignedIntegerMode, this->gui);
    servalue =  gui->joblist->job(0).get("sensor_sernum", this->channel_no);
    this->keyp->setInitialValue(servalue.toUInt());

    if (this->keyp->exec() == 1) {
        value = this->keyp->value();
        this->sensor_sernum_btn->setText(value.toString());
        if (gui->joblist->size()) {
            gui->joblist->on_set("sensor_sernum", value, this->channel_no, -1);
        }
    }
}

void triple_button::on_update_from_xml()
{
    // switch off and see what comes
    this->channel_type_btn->setEnabled(false);
    this->sensor_type_btn->setEnabled(false);
    this->sensor_sernum_btn->setEnabled(false);
    if (this->channel_type.startsWith("E")) {
        this->channel_type_btn->setText("E");
        this->sensor_type_btn->setText("EFP-XX");
        this->sensor_sernum_btn->setText("0");
    }
    else if (this->channel_type.startsWith("H")) {
        this->channel_type_btn->setText("H");
        this->sensor_type_btn->setText("MFS-XX");
        this->sensor_sernum_btn->setText("0");
    }

    // find settings for the channel
    for (int i = 0; i < (int)this->gui->joblist->job(0).size(); ++i) {
        if ((this->channel_no == i) &&  (this->gui->joblist->job(0).is_active(i)) ) {

            this->channel_type_btn->setText(this->gui->joblist->on_get("channel_type", i, 0).toString());
            this->channel_type_btn->setEnabled(true);
            this->sensor_type_btn->setText(this->gui->joblist->on_get("sensor_type", i, 0).toString());
            this->sensor_type_btn->setEnabled(true);
            this->sensor_sernum_btn->setText(this->gui->joblist->on_get("sensor_sernum", i, 0).toString());
            this->sensor_sernum_btn->setEnabled(true);
        }
    }



    this->locked = false;
}
