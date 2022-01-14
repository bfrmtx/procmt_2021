#ifndef STOPTIME_DIALOG_H
#define STOPTIME_DIALOG_H

#include <QDialog>
#include <QDateTime>
#include <QDebug>

namespace Ui {
class stoptime_dialog;
}

class stoptime_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit stoptime_dialog(const int &index, const QDateTime &start_time, const qint64 &duration, const double& sample_freq, QWidget *parent = Q_NULLPTR);
    ~stoptime_dialog();

    void set_starts(const int &index, const QDateTime &start_time, const qint64 &duration, const double& sample_freq);



signals:

    void stop_time_changed(const int &index, const QDateTime &start_time, const qint64 &duration, const double &sample_freq);

private slots:
    void on_day_spin_valueChanged(int value);

    void on_hour_spin_valueChanged(int value);

    void on_minute_spin_valueChanged(int value);

    void on_second_spin_valueChanged(int value);

    void on_stoptime_dateTimeEdit_dateTimeChanged(const QDateTime &dateTime);


    void on_buttonBox_rejected();

private:

    void set_duration(const qint64 &duration);


    Ui::stoptime_dialog *ui;

    QDateTime stop_time, start_time;
    int index;
    double sample_freq;

    bool is_on = true;                          //!< prevent update during first set up and NO SIGNALS
    bool no_dial = false;                       //!< prevent update during first set up and NO SIGNALS

    qint64 dd, hh, mm, ss, oldduration;

};

#endif // STOPTIME_DIALOG_H
