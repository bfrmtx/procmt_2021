#include "progressdialog.h"
#include "ui_progressdialog.h"

void ProgressDialog::set_min_max(int min, int max) {
    ui->progress_bar->setRange(min, max);
}

void ProgressDialog::set_progress(int value) {
    ui->progress_bar->setValue(value);
}

void ProgressDialog::set_progress_text(QString const & text) {
    ui->progress_label->setText(text);
}

void ProgressDialog::inc_progress() {
    ui->progress_bar->setValue(ui->progress_bar->value() + 1);
}

ProgressDialog::ProgressDialog(QWidget *parent, const QString & caption, const QString & initial_text) :
    QDialog(parent),
    ui(new Ui::ProgressDialog)
{
    ui->setupUi(this);
    setWindowTitle(caption);
    ui->progress_label->setText(initial_text);
}

ProgressDialog::~ProgressDialog()
{
    delete ui;
}
