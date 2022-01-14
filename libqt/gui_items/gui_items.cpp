#include "gui_items.h"


frequencies_combo::frequencies_combo(const QString &dbname, QComboBox *box)
{
    if (box == nullptr) return;
    if (box == 0x0) return;
    this->box = box;
    this->box->setEditable(false);
    this->dbname = dbname;


    this->db = std::make_unique<QSqlDatabase>(QSqlDatabase::addDatabase("QSQLITE", "frequencies_combo" ));
    this->db->setConnectOptions("QSQLITE_OPEN_READONLY");


    this->db->setDatabaseName(dbname);
    // frequencies_drop_down
    if ( this->db->open()) {
        QSqlQuery query(*db.get());
        if( query.exec("select frequencies_drop_down.display,  frequencies_drop_down.hz from frequencies_drop_down;") ) {
            this->box->blockSignals(true);
            while (query.next()) {
                this->names.append(query.value(0).toString());
                this->values.append(query.value(1));
            }

            if (this->names.size() == this->values.size()) {
                for (int i = 0; i < names.size(); ++i) {
                    this->box->addItem(this->names.at(i), this->values.at(i));
                }

            }

            this->freq = this->values.at(this->box->currentIndex()).toDouble();

        }
        this->box->blockSignals(false);
        if ((this->names.size() == this->values.size()) && this->names.size()) this->box->setCurrentText(this->names.at(0));
        else qDebug() << "db err: query failed" ;
        this->db->close();
        *this->db = QSqlDatabase();
        QSqlDatabase::removeDatabase(this->db->connectionName());
    }

    else qDebug() << "can not open" <<  this->db->databaseName();
    this->db.reset();
}

double frequencies_combo::current_index_freq(const int &current_index) const
{
    return this->values.at(current_index).toDouble();
}

QString frequencies_combo::current_index_name(const int &current_index) const
{
     return this->names.at(current_index);
}

double frequencies_combo::set_frequency(const double &f)
{
    if (this->values.empty()) return -1;
    int i = 0;
    int ci = 0;
    bool gotit = false;
    for (auto val : values) {
        double value(val.toDouble());
        if (f == value) {
            //this->box->setCurrentIndex(i);
            this->box->setCurrentText(this->names.at(i));
            ci = i;
            gotit = true;
        }
        ++i;
    }
    if (!gotit) return -1.0;
    this->freq = this->values.at(ci).toDouble();
    return freq;
}

double frequencies_combo::get_frequency() const
{
    return freq;
}


windowlength_combo::windowlength_combo(const QString &dbname, QComboBox *box)
{
    if (box == nullptr) return;
    if (box == 0x0) return;
    this->box = box;
    this->box->setEditable(false);
    this->dbname = dbname;


    this->db = std::make_unique<QSqlDatabase>(QSqlDatabase::addDatabase("QSQLITE", "windowlength_combo" ));
    this->db->setConnectOptions("QSQLITE_OPEN_READONLY");


    this->db->setDatabaseName(dbname);
    QVariant check;
    // frequencies_drop_down
    if ( this->db->open()) {
        QSqlQuery query(*db.get());
        if( query.exec("select frequencies_drop_down.hz from frequencies_drop_down;") ) {
            while (query.next()) {
                check = query.value(0);
                double fx = check.toDouble();
                if (fx >= this->lower_limit) {
                    int wl = (int)fx;
                    this->values.append(wl);
                    this->names.append(QString::number(wl));
                }
            }

            if (this->names.size() == this->values.size()) {
                for (int i = 0; i < names.size(); ++i) {
                    this->box->addItem(this->names.at(i), this->values.at(i));
                }

            }
        }
        else qDebug() << "db err: query failed" ;
        this->db->close();
        *this->db = QSqlDatabase();
        QSqlDatabase::removeDatabase(this->db->connectionName());
    }

    else qDebug() << "can not open" <<  this->db->databaseName();
    this->db.reset();
}

int windowlength_combo::current_index_wl(const int &current_index) const
{
    return this->values.at(current_index).toInt();
}

int windowlength_combo::set_window_length(const int &wl)
{
    if (this->values.empty()) return -1;
    int i = 0;
    int ci = 0;
    for (auto val : values) {
        int value(val.toInt());
        if (wl == value) {
            //this->box->setCurrentIndex(i);
            this->box->setCurrentText(this->names.at(i));
            ci = i;
        }
        ++i;
    }
    return this->values.at(ci).toInt();
}

wl_combo::wl_combo(const QString &dbname, const QString &table_name, const QString &double_int_string, QWidget *parent, QComboBox *box, const QString prc_com_key, const QString label_text, const bool init_database)
    :single_column_combo(dbname, table_name, double_int_string, parent, box, prc_com_key, label_text, false)
{

    this->init_data_base(dbname, table_name, double_int_string);

}

void wl_combo::init_data_base(const QString &dbname, const QString &table_name, const QString &double_int_string)
{
    this->db = std::make_unique<QSqlDatabase>(QSqlDatabase::addDatabase("QSQLITE", "wl_combo" ));
    this->db->setConnectOptions("QSQLITE_OPEN_READONLY");


    this->db->setDatabaseName(dbname);
    QVariant check;
    // frequencies_drop_down
    if ( this->db->open()) {
        QSqlQuery query(*db.get());
        if( query.exec("select frequencies_drop_down.hz from frequencies_drop_down;") ) {
            while (query.next()) {
                check = query.value(0);
                double fx = check.toDouble();
                if (fx >= this->lower_limit) {
                    int wl = (int)fx;
                    this->values.append(wl);
                    this->names.append(QString::number(wl));
                }
            }

            if (this->names.size() == this->values.size()) {
                for (int i = 0; i < names.size(); ++i) {
                    this->box->addItem(this->names.at(i), this->values.at(i));
                }

            }
        }
        else qDebug() << "db err: query failed" ;
        this->db->close();
        *this->db = QSqlDatabase();
        QSqlDatabase::removeDatabase(this->db->connectionName());
    }

    else qDebug() << "can not open" <<  this->db->databaseName();
    this->db.reset();
    this->value = this->current_index_qvariant(this->box->currentIndex());
    emit this->signal_current_variant_changed(this->key, value);
}

single_column_combo::single_column_combo(const QString &dbname, const QString &table_name, const
                                         QString &double_int_string,
                                         QWidget *parent, QComboBox *box,
                                         const QString prc_com_key, const QString label_text, const bool init_database) : QObject(parent)
{

    this->parent = parent;
    this->key = prc_com_key;

    if (box == Q_NULLPTR) {
        this->box = new QComboBox(this->parent);
        this->is_local_combo = true;
    }
    else this->box = box;

    if ((this->parent != Q_NULLPTR) && this->key.size()) {

        QObject::connect(this, SIGNAL(signal_current_variant_changed(QString, QVariant)), parent , SLOT(slot_change_prc_com_variant(QString, QVariant)));
        QObject::connect(this->box, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
    }

    if (label_text.size()) {

        this->label = new QLabel(label_text, this->parent);
    }
    this->box->setEditable(false);
    this->dbname = dbname;



    if (init_database) {

        this->init_data_base(dbname, table_name, double_int_string);
    }

}

void single_column_combo::init_data_base(const QString &dbname, const QString &table_name, const QString &double_int_string)
{
    this->db = std::make_unique<QSqlDatabase>(QSqlDatabase::addDatabase("QSQLITE", "single_column_combo" ));
    this->db->setConnectOptions("QSQLITE_OPEN_READONLY");


    this->db->setDatabaseName(dbname);
    // frequencies_drop_down
    if ( this->db->open()) {
        QSqlQuery query(*db.get());
        if( query.exec("select *from " + table_name + ";" )) {
            while (query.next()) {
                this->names.append(query.value(0).toString());
                if (double_int_string == "double") this->values.append(query.value(0).toDouble());
                else if (double_int_string == "int") this->values.append(query.value(0).toInt());
                else if (double_int_string == "string") this->values.append(query.value(0).toString());
                else this->values.append(query.value(0));
            }

            if (this->names.size() == this->values.size()) {
                for (int i = 0; i < names.size(); ++i) {
                    this->box->addItem(this->names.at(i), this->values.at(i));
                }

            }
        }
        else qDebug() << "db err: query failed" ;
        this->db->close();
        *this->db = QSqlDatabase();
        QSqlDatabase::removeDatabase(this->db->connectionName());
    }

    else qDebug() << "can not open" <<  this->db->databaseName();
    this->db.reset();
    this->value = this->current_index_qvariant(this->box->currentIndex());
    emit this->signal_current_variant_changed(this->key, value);
}



void single_column_combo::add_to_gui(QBoxLayout *layout, const QString horizontal_vertical, QMap<QString, int> &map_scc)
{
    if (this->parent){
        if (horizontal_vertical == "horizontal") {
            this->hzl = new QHBoxLayout();
            layout->addLayout(this->hzl);
            if (this->box)  this->hzl->addWidget(this->box);
            if (this->label) this->hzl->addWidget(this->label);

        }
        if (horizontal_vertical == "verical") {
            this->vtl = new QVBoxLayout();
            layout->addLayout(this->vtl);
            if (this->box) this->vtl->addWidget(this->box);
            if (this->label) this->vtl->addWidget(this->label);
        }
        if (map_scc.isEmpty()) {
            map_scc.insert(this->key, 0);
        }
        else {
            QList<int> vals = map_scc.values();
            std::sort(vals.begin(), vals.end());
            map_scc.insert(this->key, vals.last() + 1);
        }


    }
}

single_column_combo::~single_column_combo()
{
    //   if(is_local_combo) delete this->box;
    //   if (this->label != Q_NULLPTR) delete this->label;
}

QVariant single_column_combo::current_index_qvariant(const int &current_index) const
{
    return this->values.at(current_index);
}

QVariant single_column_combo::set_qvariant_to_current_index(const QVariant &value)
{
    if (this->values.empty()) return -1;
    int i = 0;
    int ci = 0;
    for (auto val : values) {
        if (val == value) {
            this->value = value;
            this->box->setCurrentText(this->names.at(i));
            emit this->signal_current_variant_changed(key, this->value);
            ci = i;
            break;
        }
        ++i;
    }
    return this->values.at(ci);
}

void single_column_combo::set_data(const QVariant &value)
{
    if (this->values.empty()) return;
    this->blockSignals(true);
    int i = 0;
    for (auto val : values) {
        if (val == value) {
            this->value = value;
            this->box->setCurrentText(this->names.at(i));
            break;
        }
        ++i;
    }
    this->blockSignals(false);
}

void single_column_combo::currentIndexChanged(int index)
{
    this->value = this->values.at(index);
    if (this->key.size()) {
        emit this->signal_current_variant_changed(key, this->value);
    }
}


sensors_combo::sensors_combo(const QString &dbname, const QChar E_or_H, QComboBox *box)
{
    if (box == nullptr) return;
    if (box == 0x0) return;

    this->box = box;
    this->box->setEditable(false);
    this->dbname = dbname;

    this->E_or_H = E_or_H;

    this->name = "empty";
    this->atsheadername = "empty";


    this->db = std::make_unique<QSqlDatabase>(QSqlDatabase::addDatabase("QSQLITE", "sensors_combo"));
    this->db->setConnectOptions("QSQLITE_OPEN_READONLY");


    this->db->setDatabaseName(dbname);
    // frequencies_drop_down
    if ( this->db->open()) {
        QSqlQuery query(*db.get());
        QString querystr;
        if (this->E_or_H == QChar('H')) {
            querystr = "select sensortypes.Name, sensortypes.atsheadername from sensortypes where  sensortypes.Unit like \"H\";";
        }
        if (this->E_or_H == QChar('E')) {

            querystr = "select sensortypes.Name, sensortypes.atsheadername from sensortypes where  sensortypes.Unit like \"E\";";
        }
        else {
            querystr = "select sensortypes.Name, sensortypes.atsheadername from sensortypes;";
        }

        if( query.exec(querystr) ) {
            while (query.next()) {
                this->names.append(query.value(0).toString());
                this->atsheadernames.append(query.value(1).toString());
            }

            if (this->names.size() == this->atsheadernames.size()) {
                for (int i = 0; i < names.size(); ++i) {
                    this->box->addItem(this->names.at(i));
                }

                this->name = this->names.at(0);
                this->atsheadername = this->atsheadernames.at(0);

            }
        }
        else qDebug() << "db err: query failed" ;
        *this->db = QSqlDatabase();
        QSqlDatabase::removeDatabase("sensors_combo");
    }

    else qDebug() << "can not open" <<  this->db->databaseName();
    this->db.reset();

}

QString sensors_combo::set_atsheader_sensor_name(const QString &inatsheadername)
{
    if (!this->names.size()) return QString("empty");
    int i = 0;
    for (auto &str : this->atsheadernames) {
        if (!str.compare(inatsheadername, Qt::CaseInsensitive)) {
            this->name = this->names.at(i);
            this->box->setCurrentText(this->name);
            this->atsheadername = this->atsheadernames.at(i);
            return this->names.at(i);
        }
        ++i;
    }

    // in case it was "wrong" MFS-06e with minus , correct
    i = 0;
    for (auto &str : this->names) {
        if (!str.compare(inatsheadername, Qt::CaseInsensitive)) {
            this->name = this->names.at(i);
            this->box->setCurrentText(this->name);
            this->atsheadername = this->atsheadernames.at(i);
            return this->names.at(i);
        }
        ++i;
    }

    return QString("empty");
}

QString sensors_combo::set_name(const QString &inname)
{
    if (!this->names.size()) return QString("empty");
    int i = 0;
    for (auto &str : this->names) {
        if (!str.compare(inname, Qt::CaseInsensitive)) {
            this->name = this->names.at(i);
            this->box->setCurrentText(this->name);
            this->atsheadername = this->atsheadernames.at(i);
            return this->atsheadernames.at(i);
        }
        ++i;
    }
    // in case MFS06e was used, correct it to MFS-06e
    i = 0;
    for (auto &str : this->atsheadernames) {
        if (!str.compare(inname, Qt::CaseInsensitive)) {
            this->name = this->names.at(i);
            this->box->setCurrentText(this->name);
            this->atsheadername = this->atsheadernames.at(i);
            return this->atsheadernames.at(i);
        }
        ++i;
    }
    return QString("empty");

}

QString sensors_combo::get_name() const
{
    return this->name;
}

QString sensors_combo::get_atsheader_sensor_name() const
{
    return this->atsheadername;
}

QString sensors_combo::get_current_index_atsheader_sensor_name(const int &current_index) const
{
    return this->atsheadernames.at(current_index);
}

message_window::message_window()
{
    this->setAlternatingRowColors(true);
}

message_window::~message_window()
{

    this->clear();
}

void message_window::add_message(const QString &message)
{
    this->addItem(message);
    this->update();
    this->scrollToBottom();
}

void message_window::add_header(const QString &header_string)
{
    QListWidgetItem *item = new QListWidgetItem(this);

    QFont bold;
    QColor bck;
    bck.setNamedColor("#c6deff");

    bold.setBold(true);
    bold.setItalic(true);
    item->setFont(bold);
    item->setText(header_string);
    item->setBackground(bck);

    this->update();
    this->scrollToBottom();


}

void message_window::add_QMap_dbl(const QMap<QString, double> &qmp)
{
    QMap<QString, double>::const_iterator qmi = qmp.cbegin();

    for (; qmi != qmp.cend(); ++qmi) {
        this->addItem(qmi.key() + " " + QString::number(qmi.value()));
    }

    this->update();
    this->scrollToBottom();

}

void message_window::add_statmap(const statmap& smp)
{
    for ( auto it = smp.cbegin(); it != smp.cend(); ++it ) {
        this->addItem(g_stat::gauss_names.at(it->first) + " " + QString::number(it->second));
    }
    this->update();
    this->scrollToBottom();
}

//void message_window::add_QMap_dbl_shared(const QList<std::shared_ptr<QMap<QString, double> > > &qmps)
//{
//    for (auto &li : qmps) {

//    }

//    this->update();
//    this->scrollToBottom();

//}

checkbox_list::checkbox_list(QObject *parent) : QObject(parent)
{
}

void checkbox_list::append(QCheckBox *box)
{
    this->vboxes.emplace_back(box);
    this->vbox_names.emplace_back(box->text());
    QObject::connect(box, SIGNAL(clicked()), this, SLOT(slot_re_check()));
}

void checkbox_list::slot_re_check()
{

    qDebug() << "received ------>" << QObject::sender();
    size_t i = 0;
    size_t index = SIZE_MAX;
    QString btn_name;
    for (auto &box : this->vboxes) {
        if (box == QObject::sender()) {
            index = i;
            btn_name = box->objectName();
        }
        ++i;
    }

    i = 0;
    for (auto &box : this->vboxes) {
        if (box->isChecked() && index != i) {
            box->blockSignals(true);
            box->setChecked(false);
            box->blockSignals(false);
        }
        ++i;
    }

    emit this->btn_name_checked(btn_name);
}


radiobutton_list::radiobutton_list(QObject *parent) : QObject(parent)
{
}

void radiobutton_list::append(QRadioButton *radio)
{
    this->vradios.emplace_back(radio);
    this->vradio_names.emplace_back(radio->text());
    QObject::connect(radio, SIGNAL(clicked()), this, SLOT(slot_re_check()));
}

void radiobutton_list::slot_re_check()
{

    qDebug() << "received ------>" << QObject::sender();
    size_t i = 0;
    size_t index = SIZE_MAX;
    QString btn_name;
    for (auto &box : this->vradios) {
        if (box == QObject::sender()) {
            index = i;
            btn_name = box->objectName();
        }
        ++i;
    }
    if (index < vradio_names.size()) emit this->btn_name_checked(vradio_names.at(index));
}




hz_s_spinbox::hz_s_spinbox(bool is_lowpass, QWidget *parent) : QWidget(parent)
{
    this->is_lowpass = is_lowpass;
    this->fsp = new QSpinBox(this);
    this->fsp->setDisabled(true);
    this->Hz_s_btn = new QPushButton(this);
    this->Hz_s_btn->setText("-");

    QObject::connect(this->fsp, SIGNAL(valueChanged(int)), this, SLOT(on_fsp_valueChanged(int)));
    QObject::connect(this->Hz_s_btn, SIGNAL(clicked(bool)), this, SLOT(on_Hz_s_btn_clicked(bool)));
}

hz_s_spinbox::~hz_s_spinbox()
{
    if (this->fsp != Q_NULLPTR) delete this->fsp;
    if (this->Hz_s_btn != Q_NULLPTR) delete this->Hz_s_btn;
}


void hz_s_spinbox::set_initValue(const double f)
{
    if (f < 0.0) return;
    if (f >= 1) {
        int fi = (int)std::floor(f);
        if ((this->fmax >= fi) && (this->fmin <= fi)) {
            this->fsp->setDisabled(false);
            this->f = f;
            this->fsp->setValue(fi);
            this->toggle_btn(true);
        }
    }
    else {
        int pi = (int) std::floor(1.0 / f);
        if ((this->pmax >= pi) && (this->pmin <= pi)) {
            this->fsp->setDisabled(false);
            this->f = f;
            this->fsp->setValue(pi);
            this->toggle_btn(false);


        }

    }
}


void hz_s_spinbox::set_f_min_max(const double fmin, const double fmax)
{
    if ((fmax < 0.0) || (fmin < 0.0)) return;
    this->fmax = (int)std::floor(fmax);
    this->pmin = (int)std::floor(1.0 / fmax);
    this->fmin = (int)std::floor(fmin);
    this->pmax = (int)std::floor(1.0 / fmin);


    // pure f
    if ((this->fmax > 0) && this->fmin > 0) {
        this->toggle_btn(true);
        this->Hz_s_btn->setDisabled(true);
        this->fsp->setRange(this->fmin, this->fmax);
    }
    // pure p
    else if ((this->pmin > 0) && (this->pmax > 0)) {
        this->toggle_btn(false);
        this->Hz_s_btn->setDisabled(true);
        this->fsp->setRange(this->pmin, this->pmax);
    }
    // f and p, start with f
    else {
        this->pmin = 1;
        this->fmin = 1;
        if (is_lowpass) {
            this->toggle_btn(true);
            this->Hz_s_btn->setDisabled(false);
            this->fsp->setRange(this->fmin, this->fmax);
        }
        else {
            this->toggle_btn(false);
            this->Hz_s_btn->setDisabled(false);
            this->fsp->setRange(this->pmin, this->pmax);

        }
    }


}


void hz_s_spinbox::on_Hz_s_btn_clicked(bool b)
{
    if (this->is_hz) {
        this->toggle_btn(false);
        this->fsp->setRange(this->pmin, this->pmax);
        if (is_lowpass) {
            this->fsp->setValue(this->pmin * 2);
        }
        else {
            this->fsp->setValue(this->pmax / 2);
        }
    }

    else {
        this->toggle_btn(true);
        this->fsp->setRange(this->fmin, this->fmax);
        if (is_lowpass) {
            this->fsp->setValue(this->fmax / 2);
        }
        else {
            this->fsp->setValue(this->fmin * 2);
        }

    }
}

void hz_s_spinbox::on_fsp_valueChanged(int d)
{
    if (is_hz) f = (double)d;
    else f = 1.0 / (double)d;
}

void hz_s_spinbox::toggle_btn(const bool is_hz)
{
    this->is_hz = is_hz;
    if (this->is_hz) {
        this->Hz_s_btn->setDown(false);
        if (this->is_lowpass) this->Hz_s_btn->setText("[Hz] LP");
        else this->Hz_s_btn->setText("[Hz] HP");
    }
    else {
        this->Hz_s_btn->setDown(false);
        if (this->is_lowpass) this->Hz_s_btn->setText("[s] LP");
        else this->Hz_s_btn->setText("[s] HP");
    }
}


prc_com_check_box::prc_com_check_box(QWidget *parent, QCheckBox *box, const QString prc_com_key, const QString label_text)
{
    this->parent = parent;
    this->key = prc_com_key;

    if (box == Q_NULLPTR) {
        this->box = new QCheckBox(label_text, this->parent);
        this->is_local_checkbox = true;
    }
    else this->box = box;
    this->value = (int)0;

    if ((this->parent != Q_NULLPTR) && this->key.size()) {

        QObject::connect(this, SIGNAL(signal_current_variant_changed(QString, QVariant)), parent , SLOT(slot_change_prc_com_variant(QString, QVariant)));
        QObject::connect(this->box, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
    }

    this->box->setCheckable(true);
    this->box->setChecked(false);
    emit this->signal_current_variant_changed(this->key, this->value.toInt());

}

void prc_com_check_box::add_to_gui(QBoxLayout *layout, QMap<QString, int> &map_scc, const bool set_checked)
{
    if ( (this->parent != Q_NULLPTR) && (this->box != Q_NULLPTR) ){

        layout->addWidget(this->box);
    }
    if (map_scc.isEmpty()) {
        map_scc.insert(this->key, 0);
    }
    else {
        QList<int> vals = map_scc.values();
        std::sort(vals.begin(), vals.end());
        // new and last index of a vector conatining this button
        map_scc.insert(this->key, vals.last() + 1);
    }

    if (set_checked) this->box->setChecked(true);
    else this->box->setChecked(false);

}


void prc_com_check_box::set_data(const QVariant &value)
{
    this->blockSignals(true);
    if (value.toInt() == 1) {
        this->box->setChecked(true);
        this->value = int(1);
    }
    else {
        this->box->setChecked(false);
        this->value = int(0);
    }
    this->blockSignals(false);
}


void prc_com_check_box::stateChanged(int state)
{
    if (state == 0) this->value = (int)0;
    else this->value = (int)1;

    emit this->signal_current_variant_changed(this->key, this->value);
}




dual_column_combo::dual_column_combo(const QString &dbname, const QString &table_name, const QString &double_int_string, QWidget *parent, QComboBox *box,
                                     const QString prc_com_key, const QString label_text, const bool init_database)
    : single_column_combo(dbname, table_name, double_int_string, parent, box, prc_com_key, label_text, false)
{

    this->init_data_base(dbname, table_name, double_int_string);

}

void dual_column_combo::init_data_base(const QString &dbname, const QString &table_name, const QString &double_int_string)
{

    this->db = std::make_unique<QSqlDatabase>(QSqlDatabase::addDatabase("QSQLITE", "dual_column_combo" ));
    this->db->setConnectOptions("QSQLITE_OPEN_READONLY");


    this->db->setDatabaseName(dbname);
    // frequencies_drop_down
    if ( this->db->open()) {
        QSqlQuery query(*db.get());
        if( query.exec("select *from " + table_name + ";" )) {
            while (query.next()) {
                this->names.append(query.value(0).toString());
                if (double_int_string == "double") this->values.append(query.value(1).toDouble());
                else if (double_int_string == "int") this->values.append(query.value(1).toInt());
                else if (double_int_string == "string") this->values.append(query.value(1).toString());
                else this->values.append(query.value(0));
            }

            if (this->names.size() == this->values.size()) {
                for (int i = 0; i < names.size(); ++i) {
                    this->box->addItem(this->names.at(i), this->values.at(i));
                }

            }
        }
        else qDebug() << "db err: query failed" ;
        this->db->close();
        *this->db = QSqlDatabase();
        QSqlDatabase::removeDatabase(this->db->connectionName());
    }

    else qDebug() << "can not open" <<  this->db->databaseName();
    this->db.reset();
    this->value = this->current_index_qvariant(this->box->currentIndex());
    emit this->signal_current_variant_changed(this->key, value);

}

//////////////////////////////////////////////


check_boxes_scoll_area::check_boxes_scoll_area(QWidget *parent) : QScrollArea(parent) {

    vl = new QVBoxLayout(this);
    this->setLayout(vl);
}

check_boxes_scoll_area::~check_boxes_scoll_area() {
    this->clear();
}

void check_boxes_scoll_area::check_all(const bool checked)
{
    for (auto &btn : this->check_boxes) {
        btn->setChecked(checked);
    }

    std::vector<double> fs;

    for (auto &btn : this->check_boxes) {
        if (btn->isChecked()) fs.push_back( btn->property(prop_frequency).toDouble() );
    }

    emit this->clicked_freqs(fs);
}

void check_boxes_scoll_area::add_f_checkbox(const double &frequency)
{
    if (frequency < (1.0/ 100000000.)) return;
    QCheckBox * new_check_button = new QCheckBox(this);
    new_check_button->setProperty(prop_frequency, QVariant(frequency));
    new_check_button->setProperty(prop_count, QVariant(uint(check_boxes.size())));
    this->check_boxes.push_back(new_check_button);
    new_check_button->setChecked(true);
    new_check_button->setText(doublefreq2string_unit(frequency));

    connect(new_check_button, &QCheckBox::clicked, this, &check_boxes_scoll_area::clicked_box);

    this->layout()->addWidget(new_check_button);
}

void check_boxes_scoll_area::clear()
{
    //    for (auto widget: this->findChildren<QWidget*>()) {
    //      delete widget;
    //    }
    for (auto &w : this->check_boxes) delete w;
    this->check_boxes.clear();

}

void check_boxes_scoll_area::clicked_box()
{
    std::vector<double> fs;

    for (auto &btn : this->check_boxes) {
        if (btn->isChecked()) fs.push_back( btn->property(prop_frequency).toDouble() );
    }

    emit this->clicked_freqs(fs);
}

////////////////////////////////////////////////////////


push_buttons_scoll_area::push_buttons_scoll_area(QWidget *parent) : QScrollArea(parent)
{
    vl = new QVBoxLayout(this);
    this->setLayout(vl);
}

push_buttons_scoll_area::~push_buttons_scoll_area()
{
    this->clear();
}

void push_buttons_scoll_area::add_f_push_button(const double &frequency)
{
    if (frequency < (1.0/ 100000000.)) return;
    QPushButton * new_push_button = new QPushButton(this);
    new_push_button->setProperty(prop_frequency, QVariant(frequency));
    new_push_button->setProperty(prop_count, QVariant(uint(push_buttons.size())));
    this->push_buttons.push_back(new_push_button);
    new_push_button->setText(doublefreq2string_unit(frequency));

    connect(new_push_button, &QPushButton::clicked, this, &push_buttons_scoll_area::clicked_button);

    this->layout()->addWidget(new_push_button);
}

void push_buttons_scoll_area::clear()
{
    for (auto &w : this->push_buttons) delete w;
    this->push_buttons.clear();
}

void push_buttons_scoll_area::clicked_button()
{
    auto * freqButton = reinterpret_cast<QPushButton*>(sender());
    double freq = freqButton->property(prop_frequency).toDouble();

}

pmt_open_help::pmt_open_help(QObject *parent) : QObject(parent)
{

}

pmt_open_help::~pmt_open_help()
{

}

void pmt_open_help::help(const QString which_page_main, const QString which_page_sub, int local_1_both_2_online_3, const QString lang)
{
    QUrl url;
    if (local_1_both_2_online_3 < 3) {
        QFileInfo docdir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/metronix/metronix_manuals/" + lang + "/" + which_page_main + "/" + which_page_sub);
        if (docdir.exists()) {
            url.setPath(docdir.absoluteFilePath());
        }
    }
    else if (local_1_both_2_online_3 > 1) {
        url.setUrl("https://www.geo-metronix.de/metronix_manuals/" + lang + "/" + which_page_main + "/" + which_page_sub);
    }

    QDesktopServices::openUrl(url);
}
