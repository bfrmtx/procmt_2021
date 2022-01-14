#ifndef ADDRESSDIALOG_H
#define ADDRESSDIALOG_H

#include "ui_addressdialog.h"

class NetworkAddressDialog : public QDialog, private Ui::NetworkAddressDialog {
    Q_OBJECT

public:
    explicit NetworkAddressDialog(QWidget *parent = nullptr);

private slots:
    void on_button_okay_clicked();
    void on_button_cancel_clicked();

signals:
    void addressSelected(QString const &);
};

#endif // ADDRESSDIALOG_H
