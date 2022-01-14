#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

// qt-includes
#include <QDialog>

namespace Ui {
class ProgressDialog;
}

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    void set_min_max(int min, int max);
    void set_progress(int value);
    void set_progress_text(QString const & text);
    void inc_progress();
    explicit ProgressDialog(QWidget *parent = nullptr, QString const & caption = "", QString const & initial_text = "");
    ~ProgressDialog();

private:
    Ui::ProgressDialog *ui;
};

#endif // PROGRESSDIALOG_H
