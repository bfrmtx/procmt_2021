#include "select_coil.h"
#include "ui_select_coil.h"

select_coil::select_coil(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::select_coil)
{
    ui->setupUi(this);

}

select_coil::~select_coil()
{
    delete ui;
}

void select_coil::set_coils(const QStringList &coils, const QMultiMap<QString, int> &alldb)
{
    this->ui->Sensor_comboBox->clear();
    this->ui->Sensor_comboBox->addItems(coils);
    qDebug() << "selected coil" << this->ui->Sensor_comboBox->currentText();
    this->coil = this->ui->Sensor_comboBox->currentText();
    this->alldb = alldb;
    this->extract_serials();
}

void select_coil::on_Sensor_comboBox_activated(const QString &arg1)
{
    qDebug() << "selected coil" << arg1;
    this->coil = arg1;
    this->extract_serials();
}

void select_coil::preselect(const QString &arg1)
{
    this->ui->Sensor_comboBox->setCurrentText(arg1);
}

void select_coil::on_Serial_spinBox_valueChanged(int arg1)
{
    this->serno = arg1;
}

void select_coil::on_select_coil_accepted()
{
    emit this->coil_selcted(this->coil, this->serno);
}


void select_coil::on_Sensor_comboBox_currentTextChanged(const QString &arg1)
{
    this->coil = arg1;
    this->extract_serials();
}

int select_coil::extract_serials()
{

    this->ui->listWidget->clear();
    QMultiMap<QString, int>::const_iterator i;

    std::vector<int> isers;
    isers.reserve(1000);

    for (i = this->alldb.constBegin(); i != this->alldb.constEnd(); ++i) {

        if (!this->coil.compare(i.key(), Qt::CaseInsensitive)) {
            qDebug() << i.value();
            isers.push_back(i.value());
        }
    }

    std::sort(isers.begin(), isers.end());

    for (auto &is : isers) {
        this->ui->listWidget->addItem(QString::number(is));

    }
    this->ui->listWidget->update();
    if (isers.size()) this->ui->Serial_spinBox->setValue(isers.at(0));

    return isers.size();
}

void select_coil::on_listWidget_itemClicked(QListWidgetItem *item)
{
    this->serno = item->text().toInt();
    this->ui->Serial_spinBox->setValue(this->serno);
}

void select_coil::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    this->serno = item->text().toInt();
    this->ui->Serial_spinBox->setValue(this->serno);
    emit this->accept();

}
