#include "add_range_labels.h"
#include "ui_add_range_labels.h"

add_range_labels::add_range_labels(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::add_range_labels)
{
    ui->setupUi(this);
    this->ui->spin_lower->setMinimum(-1 * DBL_MAX);
    this->ui->spin_lower->setMaximum(DBL_MAX);

    this->ui->spin_upper->setMinimum(-1 * DBL_MAX);
    this->ui->spin_upper->setMaximum(DBL_MAX);

}

add_range_labels::~add_range_labels()
{
    delete ui;
    this->textTicker.reset();
}

void add_range_labels::set_data(QCPAxis *axis, const QVector<double> dnums_labels, const QVector<QString> strnums_labels)
{
    this->dnums_labels = dnums_labels;
    this->strnums_labels = strnums_labels;
    this->textTicker = QSharedPointer<QCPAxisTickerText>(new QCPAxisTickerText);

    this->dnums_labels.append(1); this->dnums_labels.append(2);
    this->strnums_labels.append("1"); this->strnums_labels.append("2");

    this->axis = axis;

}

void add_range_labels::on_spin_lower_valueChanged(double arg1)
{
    int sz = this->dnums_labels.size() - 2;
    this->dnums_labels[sz] = arg1;
    this->strnums_labels[sz] = QString::number(arg1);
    textTicker->clear();
    textTicker->addTicks(this->dnums_labels, this->strnums_labels);
    axis->setTicker(textTicker);
    emit signal_range_label_updated();
}

void add_range_labels::on_spin_upper_valueChanged(double arg1)
{
    int sz = this->dnums_labels.size() - 1;
    this->dnums_labels[sz] = arg1;
    this->strnums_labels[sz] = QString::number(arg1);
    textTicker->clear();
    textTicker->addTicks(this->dnums_labels, this->strnums_labels);
    axis->setTicker(textTicker);
    emit signal_range_label_updated();

}

void add_range_labels::on_buttonBox_rejected()
{
    int sz = this->dnums_labels.size() - 2;
    this->dnums_labels.resize(sz);
    this->strnums_labels.resize(sz);
    textTicker->clear();
    textTicker->addTicks(this->dnums_labels, this->strnums_labels);
    axis->setTicker(textTicker);
    emit signal_range_label_updated();
}
