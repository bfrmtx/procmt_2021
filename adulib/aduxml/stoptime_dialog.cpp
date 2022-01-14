#include "stoptime_dialog.h"
#include "ui_stoptime_dialog.h"

stoptime_dialog::stoptime_dialog(const int &index, const QDateTime &start_time, const qint64 &duration, const double &sample_freq, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::stoptime_dialog)
{
    ui->setupUi(this);
    this->dd = 0;
    this->hh = 0;
    this->mm = 0;
    this->ss = 0;
    this->set_starts(index, start_time, duration, sample_freq);

}


stoptime_dialog::~stoptime_dialog()
{
    delete ui;
}

void stoptime_dialog::set_starts(const int &index, const QDateTime &start_time, const qint64 &duration, const double &sample_freq)
{

    this->start_time = start_time;
    this->stop_time = start_time;
    this->index = index;
    this->sample_freq = sample_freq;
    this->set_duration(duration);
    this->oldduration = duration;

    this->ui->stoptime_dateTimeEdit->setDateTime(this->stop_time.addSecs(duration));

}

void stoptime_dialog::set_duration(const qint64 &duration)
{
    this->dd = duration / 60 / 60 / 24;
    this->hh = (duration / 60 / 60) % 24;
    this->mm = (duration / 60) % 60;
    this->ss = duration % 60;

    this->no_dial = false;
    this->ui->second_spin->setValue(this->ss);
    this->ui->minute_spin->setValue(this->mm);
    this->ui->hour_spin->setValue(this->hh);
    this->ui->day_spin->setValue(this->dd);
    this->no_dial = true;



}

void stoptime_dialog::on_day_spin_valueChanged(int value)
{
    if (this->dd != value) {
        this->dd = value;
        this->is_on = false;
        if (this->no_dial) this->on_stoptime_dateTimeEdit_dateTimeChanged(this->start_time);
    }


}

void stoptime_dialog::on_hour_spin_valueChanged(int value)
{
    if (this->hh != value) {
        this->hh = value;
        this->is_on = false;
        if (this->no_dial) this->on_stoptime_dateTimeEdit_dateTimeChanged(this->start_time);
    }
}

void stoptime_dialog::on_minute_spin_valueChanged(int value)
{
    if (this->mm != value) {
        this->mm  = value;
        this->is_on = false;
        if (this->no_dial) this->on_stoptime_dateTimeEdit_dateTimeChanged(this->start_time);
    }

}

void stoptime_dialog::on_second_spin_valueChanged(int value)
{
    if (this->ss != value)  {
        this->ss = value;
        this->is_on = false;
        if (this->no_dial) this->on_stoptime_dateTimeEdit_dateTimeChanged(this->start_time);
    }

}




void stoptime_dialog::on_stoptime_dateTimeEdit_dateTimeChanged(const QDateTime &dateTime)
{

    // the dial buttons
    if (! this->is_on) {
        qDebug() << "called" << stop_time.toString("yyyy-MM-dd HH:mm:ss") << start_time.toString("yyyy-MM-dd HH:mm:ss");



        this->stop_time = this->start_time.addDays(this->dd);
        this->stop_time = this->stop_time.addSecs(this->hh * 3600);
        this->stop_time = this->stop_time.addSecs(this->mm * 60);
        this->stop_time = this->stop_time.addSecs(this->ss );
        qDebug() << "called" << dd << hh << mm << ss;

        // emit this->stop_time_changed(this->jobno, this->starts.secsTo(this->stops) );
        QString lab = QString::number(this->dd);
        lab += "d  " + QString::number(this->hh);
        lab += "h " + QString::number(this->mm);
        lab += "m " + QString::number(this->ss) + "s";
        this->ui->duration_label->setText(lab);
        this->ui->stoptime_dateTimeEdit->setDateTime(this->stop_time);
        //stop_time = start_time;


    }
    // direct edit in the stop time field
    if (this->start_time < dateTime) {
        this->stop_time = dateTime;

        qDebug() << "updating buttons";
        this->is_on = true;
        this->set_duration(this->start_time.secsTo(this->stop_time));
        this->is_on = false;


        qDebug() << "called event";




        emit this->stop_time_changed(this->index, this->start_time, this->start_time.secsTo(this->stop_time), this->sample_freq );
    }


}



void stoptime_dialog::on_buttonBox_rejected()
{
    emit this->stop_time_changed(this->index, this->start_time, this->oldduration, this->sample_freq );
}
