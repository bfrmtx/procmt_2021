#include "addressdialog.h"

NetworkAddressDialog::NetworkAddressDialog(QWidget *parent) : QDialog(parent) {
    setupUi(this);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
}

void NetworkAddressDialog::on_button_okay_clicked() {
    emit addressSelected(edit_address->text());
    close();
}

void NetworkAddressDialog::on_button_cancel_clicked() {
    close();
}
