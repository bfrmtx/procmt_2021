#include "manualscalingdialog.h"
#include "ui_manualscalingdialog.h"

ManualScalingDialog::ManualScalingDialog(QWidget *parent) : QDialog(parent),
    ui(new Ui::ManualScalingDialog) {
    ui->setupUi(this);    
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
}

ManualScalingDialog::~ManualScalingDialog() {
    delete ui;
}
