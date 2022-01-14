#include "manual_scales.h"
#include "ui_manual_scales.h"


manual_scales::manual_scales(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::manual_scales)
{
    ui->setupUi(this);
    this->ui->phz_max->setRange(-360, 360);
    this->ui->phz_min->setRange(-360, 360);
    this->ui->a_max->setRange(DBL_MIN, DBL_MAX);
    this->ui->a_min->setRange(DBL_MIN, DBL_MAX);
    this->ui->f_min->setRange(DBL_MIN, DBL_MAX);
    this->ui->f_max->setRange(DBL_MIN, DBL_MAX);
}

manual_scales::~manual_scales()
{
    delete ui;
}

void manual_scales::set_data(QCPRange &ampl, QCPRange &phz, QCPRange &freq)
{
    this->ampl = ampl;
    this->phz = phz;
    this->freq = freq;
    this->ui->a_min->setValue(this->ampl.lower);
    this->ui->a_max->setValue(this->ampl.upper);

    this->ui->phz_min->setValue(this->phz.lower);
    this->ui->phz_max->setValue(this->phz.upper);

    this->ui->f_min->setValue(this->freq.lower);
    this->ui->f_max->setValue(this->freq.upper);

}

void manual_scales::on_a_min_valueChanged(double arg1)
{
    this->ampl.lower = arg1;
    emit ayrange_changed(this->ampl);
}

void manual_scales::on_a_max_valueChanged(double arg1)
{
    this->ampl.upper = arg1;
    emit ayrange_changed(this->ampl);


}

void manual_scales::on_phz_min_valueChanged(double arg1)
{
    this->phz.lower = arg1;
    emit pyrange_changed(this->phz);
}

void manual_scales::on_phz_max_valueChanged(double arg1)
{
    this->phz.upper = arg1;
    emit pyrange_changed(this->phz);
}

void manual_scales::on_f_min_valueChanged(double arg1)
{
    this->freq.lower = arg1;
    emit xrange_changed(this->freq);
}

void manual_scales::on_f_max_valueChanged(double arg1)
{
    this->freq.upper = arg1;
    emit xrange_changed(this->freq);
}
