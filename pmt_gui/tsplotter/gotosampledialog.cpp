#include "gotosampledialog.h"
#include "ui_gotosampledialog.h"

#include "qthelper.h"

#include <QKeyEvent>

void GotoSampleDialog::set_min_max(qlonglong min, qlonglong max) {
    m_min_val = min;
    m_max_val = max;
    auto min_time = m_start_time;
    auto max_time = m_start_time.sample_time(static_cast<qulonglong>(m_max_val));
    ui->time->setMinimumDateTime(QDateTime(min_time.date(), min_time.time()));
    ui->time->setMaximumDateTime(QDateTime(max_time.date(), max_time.time()));
}

GotoSampleDialog::~GotoSampleDialog() {
    delete ui;
}

void LineEditNumber::keyPressEvent(QKeyEvent * event) {
    qlonglong current = text().toLongLong();
    auto * sdialog = qobject_cast<GotoSampleDialog *>(parent());
    if(sdialog != nullptr) {
        if(event->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier)) {
            if(event->key() == Qt::Key_Down)    sdialog->set_value(current - 32);
            else if(event->key() == Qt::Key_Up) sdialog->set_value(current + 32);
            else                                QLineEdit::keyPressEvent(event);
        } else {
            if(event->key() == Qt::Key_Down)    sdialog->set_value(current - 1);
            else if(event->key() == Qt::Key_Up) sdialog->set_value(current + 1);
            else                                QLineEdit::keyPressEvent(event);
        }
    }
}

void LineEditNumber::keyReleaseEvent(QKeyEvent * event) {
    QLineEdit::keyReleaseEvent(event);
}

void LineEditNumber::slot_on_text_changed(const QString & new_text) {
    QString new_text_alt;
    for(auto c : new_text) {
        if(c.isNumber()) {
            new_text_alt += c;
        }
    }

    auto * sdialog = qobject_cast<GotoSampleDialog *>(parent());
    if(sdialog != nullptr) {
        sdialog->set_value(new_text_alt.toLongLong());
    }
}


void GotoSampleDialog::set_value(qlonglong value) {
    value = std::max(m_min_val, std::min(m_max_val, value));
    auto time = m_start_time.sample_time(static_cast<qulonglong>(value));
    m_current = value;

    ui->edit_number->blockSignals(true);
    ui->edit_number->setText(QString::number(value));
    ui->edit_number->blockSignals(false);

    ui->time->blockSignals(true);
    ui->time->setDateTime(QDateTime(time.date(), time.time()));
    ui->time->blockSignals(false);

    emit value_changed(m_current);
}

void GotoSampleDialog::on_button_min_clicked() {
    set_value(m_min_val);
}

void GotoSampleDialog::on_button_max_clicked() {
    set_value(m_max_val);
}

LineEditNumber::LineEditNumber(QWidget * parent) : QLineEdit(parent) {
    connect(this, &QLineEdit::textChanged, this, &LineEditNumber::slot_on_text_changed);
}

GotoSampleDialog::GotoSampleDialog(QWidget *parent) : QDialog(parent),
    ui(new Ui::GotoSampleDialog)
{
    ui->setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
}

void GotoSampleDialog::on_time_dateTimeChanged(const QDateTime &dateTime) {
    auto edatetime = eQDateTime(dateTime);
    m_current = static_cast<qlonglong>(m_start_time.samples_to(edatetime));

    ui->edit_number->blockSignals(true);
    ui->edit_number->setText(QString::number(m_current));
    ui->edit_number->blockSignals(false);

    emit value_changed(m_current);
}
