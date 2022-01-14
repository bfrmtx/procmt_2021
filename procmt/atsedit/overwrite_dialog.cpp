#include "overwrite_dialog.h"
#include "ui_overwrite_dialog.h"

overwrite_dialog::overwrite_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::overwrite_dialog)
{
    ui->setupUi(this);
}

overwrite_dialog::~overwrite_dialog()
{
    delete ui;
}

