#include "recording.h"

recording::recording(const qint64 sample_freq, const qint64 max_sample_freq, QWidget *parent) :
    QWidget(parent), sample_freq(sample_freq), max_sample_freq(max_sample_freq)
{


    this->starts  = new QDateTimeEdit(parent);
    this->starts->setObjectName("starts");
    this->starts->setTimeSpec(Qt::UTC);
    this->starts->setDisplayFormat("yyyy-MM-dd   hh:mm:ss");
    this->starts->setDate(QDateTime::currentDateTimeUtc().date());
    this->starts->setTime(QDateTime::currentDateTimeUtc().time());
    this->starts->setMinimumDateTime(starts->dateTime());
    this->starts->setTimeSpec(Qt::UTC);

    connect(this->starts, &QDateTimeEdit::dateTimeChanged, this, &recording::starts_changed);

    this->stops  = new QDateTimeEdit(parent);
    this->stops->setObjectName("stops");
    this->stops->setTimeSpec(Qt::UTC);
    this->stops->setDisplayFormat("yyyy-MM-dd   hh:mm:ss");
    this->stops->setDateTime(this->starts->dateTime());
    this->stops->setMinimumDateTime(stops->dateTime());
    this->stops->setTimeSpec(Qt::UTC);
    connect(this->stops, &QDateTimeEdit::dateTimeChanged, this, &recording::stops_changed);


    this->duration = 0;


    this->days    = new QSpinBox(parent);
    this->hours   = new QSpinBox(parent);
    this->minutes = new QSpinBox(parent);
    this->seconds = new QSpinBox(parent);

    this->days->setObjectName("days");
    this->hours->setObjectName("hours");
    this->minutes->setObjectName("minutes");
    this->seconds->setObjectName("seconds");



    this->days->setMinimum(0);
    this->hours->setMinimum(0);
    this->minutes->setMinimum(0);
    this->seconds->setMinimum(0);

    this->days->setMaximum(365000);
    this->hours->setMaximum(23);
    this->minutes->setMaximum(59);
    this->seconds->setMaximum(59);

    this->days->setValue(0);
    this->hours->setValue(0);
    this->minutes->setValue(0);
    this->seconds->setValue(0);

    connect(this->days, QOverload<int>::of(&QSpinBox::valueChanged), this, &recording::spins_valueChanged);
    connect(this->hours, QOverload<int>::of(&QSpinBox::valueChanged), this, &recording::spins_valueChanged);
    connect(this->minutes, QOverload<int>::of(&QSpinBox::valueChanged), this, &recording::spins_valueChanged);
    connect(this->seconds, QOverload<int>::of(&QSpinBox::valueChanged), this, &recording::spins_valueChanged);

    this->qrb_1 = new QRadioButton("free", parent);
    this->qrb_1->setObjectName("free_grid");
    this->qrb_60 = new QRadioButton("full minute", parent);
    this->qrb_60->setObjectName("60s_grid");
    this->qrb_64 = new QRadioButton("64s RR grid", parent);
    this->qrb_64->setObjectName("64s_grid");

    connect(this->qrb_1, &QRadioButton::clicked, this, &recording::grid_changed);
    connect(this->qrb_60, &QRadioButton::clicked, this, &recording::grid_changed);
    connect(this->qrb_64, &QRadioButton::clicked, this, &recording::grid_changed);

    this->until_midnight = new QPushButton("EOD", parent);
    connect(this->until_midnight, &QPushButton::clicked, this, &recording::duration_to_midnight);





}


qint64 recording::duration_from_spins() const
{
    return 86400 * std::abs(this->days->value()) + 3600 * std::abs(this->hours->value()) + 60 * std::abs(this->minutes->value()) + std::abs(this->seconds->value());

}

void recording::duration_to_spins()
{
    if (!this->duration) {
        this->days->setValue(0);
        this->hours->setValue(0);
        this->minutes->setValue(0);
        this->seconds->setValue(0);
        return;
    }
    qint64 s = duration % 60;
    qint64 m = (duration / 60) % 60;
    qint64 h = (duration / 3600) % 24;
    qint64 d = duration / 86400;

    this->blockSignals(true);
    this->days->setValue(d);
    this->hours->setValue(h);
    this->minutes->setValue(m);
    this->seconds->setValue(s);
    this->blockSignals(false);



}

void recording::spins_valueChanged()
{
    this->blockSignals(true);
    this->stops->setDateTime(this->starts->dateTime().addSecs(this->duration_from_spins()));
    this->blockSignals(false);

}

void recording::grid_changed()
{
    qint64 grid_time = 1;
    if (this->qrb_60->isChecked()) {
        grid_time = 60;
    }

    if (this->qrb_64->isChecked()) {
        grid_time = 64;

    }

    if (grid_time != 1) {
        QDateTime newstart(this->starts->dateTime());
        newstart.setSecsSinceEpoch((this->starts->dateTime().toSecsSinceEpoch() / grid_time) * grid_time);  // will be equal or less; integer division rounds always down

        if (newstart < this->starts->dateTime()) {
            newstart = newstart.addSecs(grid_time);
        }
        this->blockSignals(true);
        this->starts->setDateTime(newstart);
    }
    this->stops->setDateTime(this->starts->dateTime().addSecs(duration));
    this->blockSignals(false);
    this->duration_to_spins();


}

void recording::starts_changed(QDateTime dt)
{
    this->blockSignals(true);
    this->stops->setDateTime(dt.addSecs(duration));
    this->blockSignals(false);


}

void recording::stops_changed(QDateTime dt)
{
    this->duration = (this->starts->dateTime().secsTo(dt));
    this->duration_to_spins();

}

void recording::duration_to_midnight()
{
    QTime t(this->starts->time());
    QTime n(23, 50, 0, 0);
    QTime s(23, 58, 0, 0);


    if (t > n) return;

    qDebug() << s;

    this->blockSignals(true);
    this->stops->setDateTime(this->starts->dateTime());
    this->stops->setTime(s);
    this->stops->setTime(s);

    qDebug() << "set s" << s << this->stops->time();

    this->duration = (this->starts->dateTime().secsTo(stops->dateTime()));
    this->duration_to_spins();
    this->blockSignals(false);


}

