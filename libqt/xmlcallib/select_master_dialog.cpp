#include "select_master_dialog.h"
#include "ui_select_master_dialog.h"

select_master_dialog::select_master_dialog(const QString& dbname, const QChar E_or_H, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::select_master_dialog)
{
    ui->setupUi(this);
    this->dbname = dbname;
    this->allsensors = std::make_unique<sensors_combo>(this->dbname, E_or_H, this->ui->comboBox_sensortype);

}

select_master_dialog::~select_master_dialog()
{
    delete ui;
}

QString select_master_dialog::current_master_selection() const
{
    return this->allsensors->get_name();
}

void select_master_dialog::on_comboBox_sensortype_currentIndexChanged(const QString &arg1)
{
    qDebug() << "--selected " << arg1;
    emit this->master_sensor_selected(arg1);
}
