#include "adu_gui.h"
#include "ui_adu_gui.h"

adu_gui::adu_gui(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::adu_gui)
{
    ui->setupUi(this);

    QFileInfo info_db_file(app_homepath("info.sql3"));

    if (!info_db_file.exists()) {
        qDebug() << Q_FUNC_INFO << "info.sql3 not found in app dir";
        return;
    }

    this->db = std::make_unique<QSqlDatabase>(QSqlDatabase::addDatabase("QSQLITE", "adu"));
    this->db->setConnectOptions("QSQLITE_OPEN_READONLY");
    this->db->setDatabaseName(info_db_file.absoluteFilePath());
    if (!this->db->isOpen()) {
        if (!this->db->open()) {
            qDebug() << Q_FUNC_INFO << "info.sql3 can not be opened";
            return;
        }
    }

    // init query for getting pre-defined data and values for the GUI
    this->query = std::make_unique<QSqlQuery>(*this->db.get());

    this->get_sensor_types();

    this->rec = std::make_unique<recording>(512, 512, this);
    this->ui->hzl_start->addWidget(this->rec->starts);
    this->ui->hzl_stop->addWidget(this->rec->stops);
    this->ui->hzl_duration->addWidget(this->rec->days);
    this->ui->hzl_duration->addWidget(new QLabel("d", this));
    this->ui->hzl_duration->addWidget(this->rec->hours);
    this->ui->hzl_duration->addWidget(new QLabel("h", this));
    this->ui->hzl_duration->addWidget(this->rec->minutes);
    this->ui->hzl_duration->addWidget(new QLabel("m", this));
    this->ui->hzl_duration->addWidget(this->rec->seconds);
    this->ui->hzl_duration->addWidget(new QLabel("s", this));

    this->ui->hzl_grid_time->addWidget(this->rec->qrb_64);
    this->ui->hzl_grid_time->addWidget(this->rec->qrb_60);
    this->ui->hzl_grid_time->addWidget(this->rec->qrb_1);

    this->ui->hzl_extras->addWidget(this->rec->until_midnight);


    this->rec->qrb_64->click();



}

adu_gui::~adu_gui()
{
    delete ui;
}

void adu_gui::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void adu_gui::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;

    bool ok = false;
    std::unique_ptr<QFileInfo> qfi = std::make_unique<QFileInfo>(urls.at(0).toLocalFile());
    if (!qfi->exists()) return;
    if (!qfi->suffix().compare("xml", Qt::CaseInsensitive)) {
        ok = true;
        if (this->adu != nullptr) this->adu.reset();
        this->adu = std::make_unique<adu_json>(this);
        if(!this->adu->from_XML(qfi.get())) {
            this->clear_gui();
            ok = false;
        }
    }
    else if (!qfi->suffix().compare("json", Qt::CaseInsensitive)) {
        ok = true;
        if (this->adu != nullptr) this->adu.reset();
        this->adu = std::make_unique<adu_json>(this);
        if(!this->adu->from_JSON(qfi.get())) {
            this->clear_gui();
            ok = false;
        }
    }
    if (!ok) {
        if (this->adu != nullptr) this->adu.reset();
    }

    this->assemble_from_existing(false);
    return;
}


void adu_gui::on_pb_create_bare_bone_clicked()
{

    uint nchan = uint(this->ui->sp_create_channels->value());

    if (!nchan) {
        qDebug() << Q_FUNC_INFO << "can not initialze with 0 channels";
        return;
    }

    this->create_bare_bone(nchan);


}
void adu_gui::create_bare_bone(uint chans)
{
    if (!chans) {
        qDebug() << Q_FUNC_INFO << "can not initialze with 0 channels";
        return;
    }
    uint nchan = chans;

    if (this->adu != nullptr) this->adu.reset();

    this->clear_gui();


    // make a new ADU with channels
    this->adu = std::make_unique<adu_json>(this);
    this->adu->init_channels(nchan);


    this->assemble_gui(true);


}

void adu_gui::on_pb_save_JSON_clicked()
{
    QFileDialog dialog(this);
    dialog.setModal(false);
    dialog.show();
    QString fileName;
    fileName = dialog.getSaveFileName();
    if (fileName.size()) {

        QFileInfo qfi(fileName);
        this->adu->to_JSON_file(qfi);

    }
}

void adu_gui::on_pb_save_XML_clicked()
{
    QFileDialog dialog(this);
    dialog.setModal(false);
    dialog.show();
    QString fileName;
    fileName = dialog.getSaveFileName();
    if (fileName.size()) {

        QFileInfo qfi(fileName);
        this->adu->to_XML_file(qfi);

    }
}


void adu_gui::assemble_from_existing(const bool is_barebone)
{
    if (this->adu == nullptr) {
        qDebug() << Q_FUNC_INFO << "ADU not initialized";
        return;
    }
    if (!this->adu->size()) {
        qDebug() << Q_FUNC_INFO << " no channels available";
        return;
    }
    this->clear_gui();
    this->assemble_gui(is_barebone);
    this->get_doubles_from_adu();
    this->get_strings_edit_from_adu();
}


void adu_gui::on_pb_load_JSON_clicked()
{
    QFileDialog dialog(this, "open JSON");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setFilter(QDir::Files);
    QStringList exts;
    exts << "*.js" << "*.JS" << "*.json" << "*.JSON";
    dialog.setNameFilters(exts);
    dialog.setModal(false);
    dialog.show();

    QStringList fileNames;
    if (dialog.exec()) fileNames = dialog.selectedFiles();
    if (fileNames.size()) {
        if (this->adu != nullptr) this->adu.reset();
        this->adu = std::make_unique<adu_json>(this);
        std::unique_ptr<QFileInfo> qfi = std::make_unique<QFileInfo>(fileNames.at(0));
        if(!this->adu->from_JSON(qfi.get())) {
            this->adu.reset();
            qDebug()  << Q_FUNC_INFO << "failed to load / create from file!";
            QMessageBox msgBox;
            msgBox.setText("open JSON file failed!");
            msgBox.exec();
            this->clear_gui();
        }

        else {
            this->assemble_from_existing(false);

        }
    }
}



void adu_gui::on_pb_load_XML_clicked()
{
    QFileDialog dialog(this, "open XML");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setFilter(QDir::Files);
    QStringList exts;
    exts << "*.xml" << "*.XML";
    dialog.setNameFilters(exts);
    dialog.setModal(false);
    dialog.show();

    QStringList fileNames;
    if (dialog.exec()) fileNames = dialog.selectedFiles();
    if (fileNames.size()) {
        if (this->adu != nullptr) this->adu.reset();
        this->adu = std::make_unique<adu_json>(this);
        std::unique_ptr<QFileInfo> qfi = std::make_unique<QFileInfo>(fileNames.at(0));
        if(!this->adu->from_XML(qfi.get())) {
            this->adu.reset();
            qDebug()  << Q_FUNC_INFO << "failed to load / create from file!";
            QMessageBox msgBox;
            msgBox.setText("open XML file failed!");
            msgBox.exec();
            this->clear_gui();
        }

        else {
            this->assemble_from_existing(false);
        }
    }
}

void adu_gui::update_adu_values(const uint &varmap, const uint &channel, const QString &key, QVariant &value)
{
    if (channel != adu_defs::no_channel) {

        if (channel >= this->adu->size()) return;
        if (varmap == adu_defs::channel_map) {
            this->adu->set_channel_value(channel, key, value);
            qDebug()  << Q_FUNC_INFO << "updating "  << key << "channel" << channel << "with:" << value;
        }
        if (varmap == adu_defs::sensor_map) {
            this->adu->set_sensor_value(channel, key, value);
            qDebug()  << Q_FUNC_INFO << "updating "  << key << "channel" << channel << "with:" << value;
        }
    }
    else if (channel == adu_defs::no_channel) {
        if (varmap == adu_defs::system_map) {
            this->adu->set_system_value(key, value);
            qDebug()  << Q_FUNC_INFO << "updating "  << key << "with:" << value;
        }

    }
}

void adu_gui::update_adu_double_values(const double &value_dbl)
{
    object_name_to_struct<double> data(sender()->objectName(), value_dbl);
    this->update_adu_values(data.varmap, data.channel, data.key, data.value);
}

void adu_gui::update_adu_string_values( const QString& value_str)
{
    object_name_to_struct<QString> data(sender()->objectName(), value_str);
    this->update_adu_values(data.varmap, data.channel, data.key, data.value);
}

void adu_gui::update_adu_string_values_from_idx( const int& index)
{
    // Qt6
    QString value_str;
    if (!this->cbboxs.size()) return;
    for (const auto &chb : this->cbboxs) {
        qDebug() << chb->objectName();
        if (chb->objectName() == sender()->objectName()) {
            value_str = chb->itemText(index);
        }
    }
    object_name_to_struct<QString> data(sender()->objectName(), value_str);
    this->update_adu_values(data.varmap, data.channel, data.key, data.value);
}

void adu_gui::update_adu_int_values(const int &value_int)
{
    object_name_to_struct<int> data(sender()->objectName(), value_int);
    this->update_adu_values(data.varmap, data.channel, data.key, data.value);
}

void adu_gui::channel_type_changed(const bool &checked)
{
    if (!checked) return;
    qDebug() << sender()->objectName() << checked;
    object_name_to_struct<int> data(sender()->objectName(), checked);

    for (auto &cb : this->cbboxs) {
        object_name_to_struct<int> xx(cb->objectName(), 0);
        if (data.channel == xx.channel) {
            if (xx.key == "sensor_type") {
                qDebug() << "change sensor type";

                if (data.key == "E") {
                    cb->clear();
                    cb->addItems(this->E_sensors);
                }
                if (data.key == "H") {
                    cb->clear();
                    cb->addItems(this->H_sensors);
                }
            }
            if (xx.key == "channel_type") {
                qDebug() << "change channel type";

                if (data.key == "E") {
                    cb->clear();
                    cb->addItems(this->E_channel_types);
                }
                if (data.key == "H") {
                    cb->clear();
                    cb->addItems(this->H_channel_types);
                }
            }
        }
    }
}

void adu_gui::clear_gui()
{



    // reset all local data
    for (auto &item : this->dbl_spbxs) delete item;
    this->dbl_spbxs.clear();

    for (auto &item : this->dbl_spbxs_labels) delete item;
    this->dbl_spbxs_labels.clear();

    // reset all local data
    for (auto &item : this->spbxs) delete item;
    this->spbxs.clear();

    for (auto &item : this->spbxs_labels) delete item;
    this->spbxs_labels.clear();

    // reset all local data
    for (auto &item : this->cbboxs) delete item;
    this->cbboxs.clear();

    for (auto &item : this->cbboxs_labels) delete item;
    this->cbboxs_labels.clear();

    // reset all local data
    for (auto &item : this->lineetds) delete item;
    this->lineetds.clear();

    for (auto &item : this->cbboxs_labels) delete item;
    this->cbboxs_labels.clear();

    for (auto &item : this->rb_click_E) delete item;
    this->rb_click_E.clear();

    for (auto &item : this->rb_click_H) delete item;
    this->rb_click_H.clear();


    // clear the positioning tab   ------------------------
    auto childs = this->ui->tab_pos->children();
    for (auto &child : childs) delete child;

    childs = this->ui->tab_sensor->children();
    for (auto &child : childs) delete child;

}

void adu_gui::assemble_gui(const bool is_barebone)
{


    // assemble the positioning tab   ------------------------
    // create positioning
    this->gen_sensor_pos();

    // create sensor types
    this->gen_sensor_types();

    if (!is_barebone) {
        for (auto &combo : this->cbboxs) combo->blockSignals(true);
    }

    if (adu->size() <= 2) {
        for (auto &combo : this->cbboxs) {
            QString name(combo->objectName());
            object_name_to_struct<int> data(combo->objectName(), 1);
            auto i = data.channel;
            if (name.contains("sensor_type")) {
                combo->addItems(this->E_sensors);
                this->rb_click_E[i]->setChecked(true);
            }
            if (name.contains("channel_type")) {
                combo->addItems(this->E_channel_types);
            }

        }
    }
    else if (this->adu->size() == 3) {
        for (auto &combo : this->cbboxs) {
            QString name(combo->objectName());
            object_name_to_struct<int> data(combo->objectName(), 1);
            auto i = data.channel;
            if (name.contains("sensor_type")) {
                combo->addItems(this->H_sensors);
                this->rb_click_H[i]->setChecked(true);

            }
            if (name.contains("channel_type")) {
                combo->addItems(this->H_channel_types);
            }
        }
    }
    else {

        for (auto &combo : this->cbboxs) {
            QString name(combo->objectName());
            object_name_to_struct<int> data(combo->objectName(), 1);
            auto i = data.channel;
            if (name.contains("sensor_type") && (i < 2)) {
                combo->addItems(this->E_sensors);
                this->rb_click_E[i]->setChecked(true);
            }
            if (name.contains("channel_type") && (i < 2)) {
                combo->addItems(this->E_channel_types);
            }

            if (name.contains("sensor_type") && (i >= 2)) {
                combo->addItems(this->H_sensors);
                this->rb_click_H[i]->setChecked(true);

            }
            if (name.contains("channel_type") && (i >= 2)) {
                combo->addItems(this->H_channel_types);
            }

            ++i;
        }
    }
    if (!is_barebone) {
        for (auto &combo : this->cbboxs) combo->blockSignals(false);
    }



    // set some defauls
    this->set_default_ranges(adu_defs::channel_map, "pos", double(-100000), double(100000));
    this->set_default_ranges(adu_defs::channel_map, "ts_lsb", DBL_MIN, DBL_MAX);

    this->gen_str_results();
}

void adu_gui::get_doubles_from_adu()
{

    //  QVariant get_channel_value(const uint &channel, const QString &QVariantMap_name, const QString &key) const;
    for (auto &v : this->dbl_spbxs) {
        v->blockSignals(true);
        QString uuid(v->objectName());

        uint channel;
        QString QVariantMap_name;
        QString key;

        QStringList split(uuid.split("++"));
        bool ok = false;

        if (split.size() == 3) {
            channel = split.last().toUInt(&ok);
            if (ok) {
                if (channel >= this->adu->size()) return;
                QVariantMap_name = split.at(0);
                key = split.at(1);
                QVariant value = this->adu->get_channel_value(channel, key);
                int mydouble = value.toDouble(&ok);
                // qDebug()  << Q_FUNC_INFO << "getting from ADU "  << QVariantMap_name  << key << "channel" << channel << "with:" << value.toDouble(&ok);
                if (ok) v->setValue(mydouble);
                else {
                    qDebug()  << Q_FUNC_INFO << "getting from ADU CAN NOT SET DOUBLE value";
                }
            }
        }
        v->blockSignals(false);
    }
}

void adu_gui::get_strings_edit_from_adu()
{
    for (auto &v : this->lineetds) {
        v->blockSignals(true);
        QString uuid(v->objectName());

        uint channel;
        QString QVariantMap_name;
        QString key;

        QStringList split(uuid.split("++"));
        bool ok = false;

        if (split.size() == 3) {
            channel = split.last().toUInt(&ok);
            if (ok) {
                if (channel >= this->adu->size()) return;
                QVariantMap_name = split.at(0);
                key = split.at(1);
                QString value = this->adu->get_channel_value(channel, key).toString();
                // qDebug()  << Q_FUNC_INFO << "getting from ADU "  << QVariantMap_name  << key << "channel" << channel << "with:" << value.toDouble(&ok);
                v->setText(value);

            }
        }
        v->blockSignals(false);
    }

    for (auto &v : this->cbboxs) {
        v->blockSignals(true);
        QString uuid(v->objectName());

        uint channel;
        QString QVariantMap_name;
        QString key;

        QStringList split(uuid.split("++"));
        bool ok = false;

        if (split.size() == 3) {
            channel = split.last().toUInt(&ok);
            if (ok) {
                if (channel >= this->adu->size()) return;
                QVariantMap_name = split.at(0);
                key = split.at(1);
                QString value = this->adu->get_channel_value(channel, key).toString();
                qDebug()  << Q_FUNC_INFO << "getting from ADU "  << QVariantMap_name  << key << "channel" << channel << "with:" << value;
                auto idx = v->findText(value);
                if (idx != -1) v->setCurrentIndex(idx);

            }
        }
        v->blockSignals(false);
    }


}

void adu_gui::get_ints_from_adu()
{
    for (auto &v : this->spbxs) {
        v->blockSignals(true);
        QString uuid(v->objectName());

        uint channel;
        QString QVariantMap_name;
        QString key;

        QStringList split(uuid.split("++"));
        bool ok = false;

        if (split.size() == 3) {
            channel = split.last().toUInt(&ok);
            if (ok) {
                if (channel >= this->adu->size()) return;
                QVariantMap_name = split.at(0);
                key = split.at(1);
                QVariant value = this->adu->get_channel_value(channel, key);
                int myint = value.toInt(&ok);
                // qDebug()  << Q_FUNC_INFO << "getting from ADU "  << QVariantMap_name  << key << "channel" << channel << "with:" << value.toDouble(&ok);
                if (ok) v->setValue(myint);
                else {
                    qDebug()  << Q_FUNC_INFO << "getting from ADU CAN NOT SET INTEGER value";
                }
            }
        }
        v->blockSignals(false);
    }
}


QVariant adu_gui::get_label(const adu_defs::adu_defs varmap, const QString &key)
{
    QString querystr;
    if (varmap == adu_defs::channel_map) querystr = adu_row.select_substr_channel() + " WHERE key = '" + key + "' LIMIT 1";
    if (varmap == adu_defs::system_map) querystr = adu_row.select_substr_system() + " WHERE key = '" + key + "' LIMIT 1";



    if (!query->exec(querystr)) {
        qDebug() << Q_FUNC_INFO << "FAILED to exec query " << querystr;
        return QVariant("invalid");
    }

    adu_row.set_query_result(this->query);

    return this->adu_row.label;


}

void adu_gui::get_sensor_types()
{
    QString querystr;
    querystr = "select sensortypes.Name, sensortypes.atsheadername from sensortypes where  sensortypes.Unit like \"H\";";
    if( query->exec(querystr) ) {
        while (query->next()) {
            this->H_sensors.append(query->value(0).toString());
            this->H_sensors_atsheader.append(query->value(1).toString());
        }
    }

    querystr = "select sensortypes.Name, sensortypes.atsheadername from sensortypes where  sensortypes.Unit like \"E\";";
    if( query->exec(querystr) ) {
        while (query->next()) {
            this->E_sensors.append(query->value(0).toString());
            this->E_sensors_atsheader.append(query->value(1).toString());
        }
    }

    this->E_channel_types << "Ex" << "Ey" << "Ez";
    this->H_channel_types << "Hx" << "Hy" << "Hz";

}

void adu_gui::set_default_ranges(const adu_defs::adu_defs varmap, const QString &token_2, double min, double max)
{
    for (auto &v : this->dbl_spbxs) {
        QString tags = v->objectName();
        QString token_1(QString::number(varmap) + "++");
        if ( (tags.contains(token_1)) && (tags.contains(token_2)) ) {
            v->setRange(min, max);
        }
    }
}



void adu_gui::gen_sensor_pos()
{
    if (this->adu == nullptr) {
        qDebug() << Q_FUNC_INFO << "ADU not initialized";
        return;
    }
    if (!this->adu->size()) {
        qDebug() << Q_FUNC_INFO << " no channels available";
        return;
    }

    QHBoxLayout *poslayout = new QHBoxLayout(this->ui->tab_pos);


    std::vector<QVBoxLayout*> vls;
    std::vector<QHBoxLayout*> hls;


    for (size_t i = 0; i < this->adu->size(); ++i) {

        vls.emplace_back(new QVBoxLayout()); // channel 0
        // header
        vls.back()->addWidget(new QLabel("channel " + QString::number(i)));

        hls.emplace_back(new QHBoxLayout()); // x2
        vls.back()->addLayout(hls.back());

        this->create_double_spinbox(adu_defs::channel_map, i, "pos_x2", this->ui->tab_pos);
        auto frame = (new QFrame());
        auto layout = new QVBoxLayout();
        layout->addWidget(this->dbl_spbxs_labels.back());
        layout->addWidget(dbl_spbxs.back());
        frame->setLayout(layout);
        frame->setFrameStyle(QFrame::Box);
        hls.back()->addWidget(frame);


        hls.emplace_back(new QHBoxLayout()); // y1 & y2
        vls.back()->addLayout(hls.back());
        this->create_double_spinbox(adu_defs::channel_map, i, "pos_y1", this->ui->tab_pos);
        auto frame2 = (new QFrame());
        auto layout2 = new QVBoxLayout();
        layout2->addWidget(this->dbl_spbxs_labels.back());
        layout2->addWidget(this->dbl_spbxs.back());
        frame2->setLayout(layout2);
        frame2->setFrameStyle(QFrame::Box);
        hls.back()->addWidget(frame2);



        this->create_double_spinbox(adu_defs::channel_map, i, "pos_y2", this->ui->tab_pos);
        auto frame3 = (new QFrame());
        auto layout3 = new QVBoxLayout();
        layout3->addWidget(this->dbl_spbxs_labels.back());
        layout3->addWidget(dbl_spbxs.back());
        frame3->setLayout(layout3);
        frame3->setFrameStyle(QFrame::Box);
        hls.back()->addWidget(frame3);


        hls.emplace_back(new QHBoxLayout()); // x1
        vls.back()->addLayout(hls.back());
        this->create_double_spinbox(adu_defs::channel_map, i, "pos_x1", this->ui->tab_pos);
        auto frame4 = (new QFrame());
        auto layout4 = new QVBoxLayout();
        layout4->addWidget(this->dbl_spbxs_labels.back());
        layout4->addWidget(dbl_spbxs.back());
        frame4->setLayout(layout4);
        frame4->setFrameStyle(QFrame::Box);
        hls.back()->addWidget(frame4);


        hls.emplace_back(new QHBoxLayout()); // z1 & z2
        vls.back()->addLayout(hls.back());


        this->create_double_spinbox(adu_defs::channel_map, i, "pos_z1", this->ui->tab_pos);
        auto frame5 = (new QFrame());
        auto layout5 = new QVBoxLayout();
        layout5->addWidget(this->dbl_spbxs_labels.back());
        layout5->addWidget(dbl_spbxs.back());
        frame5->setLayout(layout5);
        frame5->setFrameStyle(QFrame::Box);
        hls.back()->addWidget(frame5);


        this->create_double_spinbox(adu_defs::channel_map, i, "pos_z2", this->ui->tab_pos);
        auto frame6 = (new QFrame());
        auto layout6 = new QVBoxLayout();
        layout6->addWidget(this->dbl_spbxs_labels.back());
        layout6->addWidget(dbl_spbxs.back());
        frame6->setLayout(layout6);
        frame6->setFrameStyle(QFrame::Box);
        hls.back()->addWidget(frame6);


        poslayout->addLayout(vls.back());


    }

    this->ui->tab_pos->setLayout(poslayout);
}

void adu_gui::gen_sensor_types()
{
    if (this->adu == nullptr) {
        qDebug() << Q_FUNC_INFO << "ADU not initialized";
        return;
    }
    if (!this->adu->size()) {
        qDebug() << Q_FUNC_INFO << " no channels available";
        return;
    }

    QHBoxLayout *poslayout = new QHBoxLayout(this->ui->tab_sensor);

    std::vector<QVBoxLayout*> vls;
    std::vector<QHBoxLayout*> hls;


    for (size_t i = 0; i < this->adu->size(); ++i) {

        vls.emplace_back(new QVBoxLayout()); // channel 0
        // header
        vls.back()->addWidget(new QLabel("channel " + QString::number(i)));



        // creates also the radio buttons for change
        this->create_sensor_combo(adu_defs::channel_map, i, "sensor_type", this->ui->tab_sensor);

        hls.emplace_back(new QHBoxLayout()); // channel 0
        auto qbg = new QButtonGroup(this->ui->tab_sensor);
        qbg->addButton(this->rb_click_E.back());
        qbg->addButton(this->rb_click_H.back());
        qbg->setExclusive(true);
        hls.back()->addWidget(this->rb_click_E.back());
        hls.back()->addWidget(this->rb_click_H.back());

        vls.back()->addLayout(hls.back());



        this->create_line_edit_channel(adu_defs::channel_map, i, "sensor_sernum", this->ui->tab_sensor);

        auto frame = (new QFrame());
        auto layout = new QVBoxLayout();

        layout->addWidget(new QLabel("Sensor"));

        layout->addWidget(this->cbboxs.back());
        layout->addWidget(new QLabel("Serial Number"));
        layout->addWidget(this->lineetds.back());
        this->create_sensor_combo(adu_defs::channel_map, i, "channel_type", this->ui->tab_sensor);
        layout->addWidget(this->cbboxs.back());
        frame->setLayout(layout);
        frame->setFrameStyle(QFrame::Box);
        vls.back()->addWidget(frame);
        vls.back()->addItem((new QSpacerItem(0,5000,QSizePolicy::Maximum, QSizePolicy::Maximum)));

        poslayout->addLayout(vls.back());

    }
}

void adu_gui::gen_str_results()
{
    if (this->adu == nullptr) {
        qDebug() << Q_FUNC_INFO << "ADU not initialized";
        return;
    }

    QList<QStringList> txt_values;
    QStringList out_keys;
    QStringList out_tooltips;
    QStringList header;
    header << "key";
    if (!this->adu->get_selftest_channels(txt_values, out_keys, out_tooltips)) return;
    if (!out_keys.size()) return;

    if (txt_values.size() != this->adu->size()) return;
    for (int i = 0; i < this->adu->size(); ++i) {
        if (txt_values.at(i).size() != out_keys.size()) return;
        header << ("channel " + QString::number(i));
    }
    header << "hint / default / ranges";

    QHBoxLayout *layout = new QHBoxLayout(this->ui->tab_selftest);

    QTableWidget *table_SRT = new QTableWidget(out_keys.size(), this->adu->size() + 2, this->ui->tab_selftest);
    table_SRT->setHorizontalHeaderLabels(header);

    for (int i = 0; i < out_keys.size(); ++i) {
        table_SRT->setItem(i, 0, new QTableWidgetItem(out_keys.at(i)));
        for (int j = 0; j < this->adu->size(); ++j) table_SRT->setItem(i, j+1, new QTableWidgetItem(txt_values.at(j).at(i)));
        table_SRT->setItem(i, this->adu->size() + 1, new QTableWidgetItem(out_tooltips.at(i)));

    }

    table_SRT->setAlternatingRowColors(true);
    for (int i = 0; i < table_SRT->rowCount(); ++i) table_SRT->resizeColumnToContents(i);
    table_SRT->resizeRowsToContents();
    layout->addWidget(table_SRT);

}


void adu_gui::create_double_spinbox(const adu_defs::adu_defs varmap, const uint &channel, const QString &key, QWidget *parent)
{
    if (this->adu == nullptr) {
        qDebug() << Q_FUNC_INFO << "ADU not initialized";
        return;
    }
    if ( (channel >= this->adu->size()) && (varmap == adu_defs::channel_map)) {
        qDebug() << Q_FUNC_INFO << channel << "is greater than ADU channels:" << this->adu->size();
        return;
    }

    QString uuid = "";
    QString label;

    if (varmap == adu_defs::channel_map) {
        uuid = QString::number(varmap) + "++"  + key + "++" + QString::number(channel);
        label = this->get_label(varmap, key).toString();
    }


    this->dbl_spbxs.emplace_back(new QDoubleSpinBox(parent));
    this->dbl_spbxs.back()->setObjectName(uuid);
    //connect(this->dbl_spbxs.back(), QOverload<const QString &>::of(&QDoubleSpinBox::valueChanged), this, &adu_gui::update_adu);
    connect(this->dbl_spbxs.back(), QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &adu_gui::update_adu_double_values);
    this->dbl_spbxs_labels.emplace_back(new QLabel(label, parent));





}

void adu_gui::create_sensor_combo(const adu_defs::adu_defs varmap, const uint &channel, const QString &key, QWidget *parent)
{
    if (this->adu == nullptr) {
        qDebug() << Q_FUNC_INFO << "ADU not initialized";
        return;
    }
    if ( (channel >= this->adu->size()) && (varmap == adu_defs::channel_map)) {
        qDebug() << Q_FUNC_INFO << channel << "is greater than ADU channels:" << this->adu->size();
        return;
    }

    QString uuid = "";
    QString label;

    // sensor type is stored in the channel
    if (varmap == adu_defs::channel_map) {
        uuid = QString::number(varmap) + "++"  + key + "++" + QString::number(channel);
        label = this->get_label(varmap, key).toString();
    }


    this->cbboxs.emplace_back(new QComboBox(parent));
    this->cbboxs.back()->setObjectName(uuid);
    // nope connect(this->dbl_spbxs.back(), QOverload<const QString &>::of(&QDoubleSpinBox::valueChanged), this, &adu_gui::update_adu);
    // nope connect(this->dbl_spbxs.back(), QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &adu_gui::update_adu_double_values);
    // 5.12 connect(this->cbboxs.back(), QOverload<const QString &>::of(&QComboBox::currentIndexChanged), this, &adu_gui::update_adu_string_values);
    connect(this->cbboxs.back(), &QComboBox::currentIndexChanged, this, &adu_gui::update_adu_string_values_from_idx);
    // Qt6
    //this->cbboxs_labels.emplace_back(new QLabel(label, parent));

    if (key == "sensor_type") {
        this->rb_click_E.emplace_back(new QRadioButton("E", parent));

        this->rb_click_H.emplace_back(new QRadioButton("H", parent));
        if (varmap == adu_defs::channel_map) {
            uuid = QString::number(varmap) + "++"  + "E" + "++" + QString::number(channel);
        }
        this->rb_click_E.back()->setObjectName(uuid);
        if (varmap == adu_defs::channel_map) {
            uuid = QString::number(varmap) + "++"  + "H" + "++" + QString::number(channel);
        }
        this->rb_click_H.back()->setObjectName(uuid);

        connect(this->rb_click_E.back(), &QAbstractButton::clicked, this, &adu_gui::channel_type_changed);
        connect(this->rb_click_H.back(), &QAbstractButton::clicked, this, &adu_gui::channel_type_changed);
    }
}

void adu_gui::create_line_edit_channel(const adu_defs::adu_defs varmap, const uint &channel, const QString &key, QWidget *parent)
{
    if (this->adu == nullptr) {
        qDebug() << Q_FUNC_INFO << "ADU not initialized";
        return;
    }
    if ( (channel >= this->adu->size()) && (varmap == adu_defs::channel_map)) {
        qDebug() << Q_FUNC_INFO << channel << "is greater than ADU channels:" << this->adu->size();
        return;
    }

    QString uuid = "";
    QString label;

    if (varmap == adu_defs::channel_map) {
        uuid = QString::number(varmap) + "++"  + key + "++" + QString::number(channel);
        label = this->get_label(varmap, key).toString();
    }


    this->lineetds.emplace_back(new QLineEdit(parent));
    this->lineetds.back()->setObjectName(uuid);
    //connect(this->dbl_spbxs.back(), QOverload<const QString &>::of(&QDoubleSpinBox::valueChanged), this, &adu_gui::update_adu);
    // connect(this->dbl_spbxs.back(), QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &adu_gui::update_adu_double_values);
    connect(this->lineetds.back(), &QLineEdit::textChanged, this, &adu_gui::update_adu_string_values);
}


