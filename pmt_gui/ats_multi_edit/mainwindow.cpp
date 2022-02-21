#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(const QString &dbname, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->dbname = dbname;

    ui->setupUi(this);
    this->setLocale(QLocale::c());
    this->ui->spinBox_shift_x_gps->setRange(-INT32_MAX, INT32_MAX);
    this->ui->spinBox_shift_y_gps->setRange(-INT32_MAX, INT32_MAX);

    this->setWindowTitle("ats multi edit");

    QFileInfo qfi;
    this->system_table = std::make_unique<prc_com_table>(this);
    prc_com prc;

    this->ui->pushButton_accept->setEnabled(false);
    this->ui->buttonBox->setEnabled(false);

    this->excludes << "sample_freq" << "num_samples" << "start_date" << "stop_date" << "stop_date_time"
                   << "start_time" << "dipole_length" << "dipole_angle"
                   << "stop_date" << "stop_time" << "run_number"
                   << "UTMZone" << "HF_filters" << "LF_filters";

}

MainWindow::~MainWindow()
{
    this->clean_up(false);
    delete ui;

}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}
void MainWindow::dropEvent(QDropEvent *event)
{

    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;

    this->open_urls(urls);
}

void MainWindow::open_urls(const QList<QUrl> urls) {

    this->clean_up(false);
    int dirty = 0;

    if (urls.size()) {
        for (auto &ur : urls) {
            if (ur.isValid()) {
                this->qfis.append(QFileInfo(ur.toLocalFile()));
            }
            else {
                dirty = 1;
            }
        }
    }
    if (dirty) this->clean_up();
    if (dirty) return;

    for (auto &file : this->qfis) {
        if (!file.fileName().endsWith("ats", Qt::CaseInsensitive)) {
            dirty = 1;
        }
    }

    if (dirty) this->clean_up();
    if (dirty) return;

    for (auto &qfi : qfis) {
        this->atshs.emplace_back(std::make_unique<atsheader>(qfi, 0, this));
    }
    dirty = 0;
    for (auto &atsh : atshs) {
        size_t check = atsh->scan_header_close();
        if (check == SIZE_MAX) {
            dirty = 1;
        }

    }
    if (dirty) this->clean_up();
    for (auto &atsh : atshs) {
        atsh->set_change_measdir(false);
    }

    this->setWindowTitle(this->qfis.at(0).baseName());

    this->can_exec = true;
    this->ui->pushButton_accept->setEnabled(true);
    this->set_table();

    this->ui->statusBar->showMessage("atsfiles loaded:" + QString::number(this->atshs.size()));




}

void MainWindow::clean_up(const bool setWT)
{
    if (setWT) this->setWindowTitle("FAILED");
    this->qfis.clear();
    for(auto &atsh : atshs) {
        atsh.reset();
    }
    this->atshs.clear();
    this->tmp_header.clear();
    this->master_data.clear();
    this->diff_data.clear();
    prc_com_table empt;
    if (this->system_table != nullptr) this->system_table.reset();
    this->ui->tableView_system->setModel(&empt);
    this->ui->statusBar->showMessage("");
    this->ui->proceed_label->setText("");

    this->ui->checkBox_fliplsb->setChecked(false);

    this->ui->doubleSpinBox_mul_LSB->setValue(0.0);

    this->ui->spinBox_shift_x_gps->setValue(0);

    this->ui->spinBox_shift_y_gps->setValue(0);

    this->can_exec = false;

}

void MainWindow::set_table()
{
    if (!this->atshs.size()) return;
    this->atshs[0]->scan_header_close();

    if (this->system_table != nullptr) this->system_table.reset();
    this->system_table = std::make_unique<prc_com_table>(this);

    this->tmp_header = this->atshs[0]->get_data();
    this->master_data.insert_data(this->atshs[0]->get_data(), false);
    this->system_table->set_excludes(this->excludes);

    this->system_table->set_map(this->tmp_header);

    this->ui->tableView_system->setModel(this->system_table.get());
    this->ui->tableView_system->setAlternatingRowColors(true);
    this->ui->tableView_system->resizeColumnToContents(0);
    this->ui->tableView_system->resizeColumnToContents(1);
    this->ui->tableView_system->resizeRowsToContents();
    this->ui->tableView_system->setSelectionMode(QAbstractItemView::SingleSelection);
    this->ui->tableView_system->horizontalHeader()->setStretchLastSection(true);



}

bool MainWindow::items_checked()
{
    bool is_checked = false;

    if (this->ui->checkBox_fliplsb->isChecked()) is_checked = true;

    if (this->ui->doubleSpinBox_mul_LSB->value() != 0.0) is_checked = true;

    if (this->ui->spinBox_shift_x_gps->value() != 0) is_checked = true;

    if (this->ui->spinBox_shift_y_gps->value() != 0) is_checked = true;


    return is_checked;

}

void MainWindow::on_buttonBox_accepted()
{
    if (!this->can_exec) return;
    if (!this->can_proceed) return;

    int i = 0;
    QMap<QString, QVariant> changed_items;
    for (auto &atsh : this->atshs) {
        atsh->scan_header_close(true);

        if (this->ui->checkBox_fliplsb->isChecked()){
            double d = atsh->dvalue("lsb");
            d *= -1.0;
            diff_data.insert_double("lsb", d);

        }
        if (this->ui->doubleSpinBox_mul_LSB->value() != 0.0) {
            double d = atsh->dvalue("lsb");
            d *= this->ui->doubleSpinBox_mul_LSB->value();
            diff_data.insert_double("lsb", d);

        }
        if (this->ui->spinBox_shift_x_gps->value() != 0) {
            int iv = atsh->ivalue("latitude");
            iv += this->ui->spinBox_shift_x_gps->value();
            diff_data.insert_int("latitude", iv);

        }
        if (this->ui->spinBox_shift_y_gps->value() != 0) {
            int iv = atsh->ivalue("longitude");
            iv += this->ui->spinBox_shift_y_gps->value();
            diff_data.insert_int("longitude", iv);
        }
        if (diff_data.size()) atsh->set_data(this->diff_data.get_data(), false, false);

        atsh->write_header(true);
        atsh->close(false);

        changed_items.insert(this->qfis.at(i).baseName(), "changed");

        QString pth = this->qfis.at(i).absolutePath();
        QFileInfo qfi_measdoc;
        std::unique_ptr<measdocxml> measdoc;
        qfi_measdoc.setFile(pth + "/" + atsh->svalue("xml_header"));
        if (qfi_measdoc.exists()) {
            measdoc = std::make_unique<measdocxml>(&qfi_measdoc, nullptr, nullptr, this);
        }
        if (measdoc != nullptr) {
            measdoc->slot_update_atswriter(atsh->get_atswriter_section(), atsh->ivalue("channel_number"), atsh->absolutePath(), atsh->get_measdoc_name());
            measdoc->set_date_time(atsh->get_start_datetime(), atsh->get_stop_datetime());
            measdoc->set_simple_section("HwStatus", "GPS", atsh->get_gps_section());
            measdoc->set_lat_lon_elev(atsh->ivalue("latitude"), atsh->ivalue("longitude"), atsh->ivalue("elevation"));

            measdoc->save(&qfi_measdoc);
            measdoc.reset();
        }
        if (measdoc != nullptr) measdoc.reset();
        this->ui->statusBar->showMessage("changed " + qfis.at(i).baseName());
        ++i;

    }

    if (this->system_table != nullptr) this->system_table.reset();
    this->system_table = std::make_unique<prc_com_table>(this);
    this->system_table->set_map(changed_items);
    this->ui->tableView_system->setModel(this->system_table.get());

    this->can_exec = false;
    this->can_proceed = false;
    this->ui->buttonBox->setEnabled(false);
    auto msgbx = new QMessageBox(this);
    msgbx->setWindowTitle("ats multi edit");
    msgbx->setText("all files processed            ");
    msgbx->exec();


}

void MainWindow::on_buttonBox_rejected()
{
    this->clean_up(false);
    this->setWindowTitle("ats multi edit");

}

void MainWindow::on_pushButton_accept_clicked()
{
    if (!this->can_exec) return;
    this->can_proceed = false;
    this->changed_data.insert_data(this->system_table->get_map(), false);
    this->diff_data = create_prc_com_from_diff(master_data, changed_data);
    if (diff_data.size()) {
        this->tmp_header.clear();
        this->tmp_header = this->diff_data.get_data();
        if (this->system_table != nullptr) this->system_table.reset();
        this->system_table = std::make_unique<prc_com_table>(this);
        this->system_table->set_excludes(this->excludes);
        this->system_table->set_map(this->tmp_header);
        this->ui->tableView_system->setModel(this->system_table.get());
        this->can_proceed = true;

    }
    else {
        this->system_table->clear();
        if (this->system_table != nullptr) this->system_table.reset();
    }
    if(items_checked()) this->can_proceed = true;
    if (this->can_proceed) {
        this->ui->buttonBox->setEnabled(true);
    }



}
