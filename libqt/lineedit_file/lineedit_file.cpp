#include "lineedit_file.h"


lineedit_file::lineedit_file(QWidget *parent, const int what) : QLineEdit(parent)
{
    this->acceptDrops();
    this->QLineEdit::clear();
    this->what = what;
    QObject::connect(this, &QLineEdit::textChanged, this, &lineedit_file::lineedit_changed);
}



lineedit_file::lineedit_file(const QFileInfo qfi, const int file_0_dir_1, const int what, QWidget *parent)  : lineedit_file(parent, what) {
    if (file_0_dir_1 == 0) this->set_file(qfi, what);
    if (file_0_dir_1 == 1) this->set_dir(QDir(qfi.absoluteFilePath()), what);
}


lineedit_file::lineedit_file(const QDir dir, const int what, QWidget *parent)   : lineedit_file(parent, what) {
    this->set_dir(dir, what);
}

void lineedit_file::set_dir(const QString str, const int what) {
    this->what = what;
    QDir dir(str);
    this->set_dir(dir, what);

}

// main work
void lineedit_file::set_dir(const QDir dir, const int what) {
    this->what = what;
    if (dir.exists()) {
        this->QLineEdit::clear();
        this->setFile(dir.absolutePath());
        this->set_text();

    }
}

void lineedit_file::set_file(const QString str, const int what) {
    this->what = what;
    QFileInfo qfi(str);
    this->set_file(qfi, what);

}

// main work
void lineedit_file::set_file(const QFileInfo qfi, const int what) {
    this->what = what;
    if (qfi.exists() && !qfi.isDir()) {
        this->QLineEdit::clear();
        this->setFile(qfi.absoluteFilePath());
        this->set_text();
    }
}


void lineedit_file::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();

}

void lineedit_file::dropEvent(QDropEvent *event)
{
    QFileInfo qfix;

    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) {
        return;
    }

    qfix.setFile(urls.at(0).toLocalFile());
    if (!qfix.exists()) return;
    if (this->qfi_topdir.absolutePath().size()) {
        if (!qfix.absolutePath().contains(this->qfi_topdir.absoluteFilePath())) return;
    }
    this->QLineEdit::clear();
    this->setFile(qfix.absoluteFilePath());

    this->set_text();


}

void lineedit_file::set_what(const int base0_filename1_absolute2)
{
    this->what =    base0_filename1_absolute2;
}

void lineedit_file::clear()
{
    this->clear_me = true;
    this->QLineEdit::clear();
    this->setText("");
    this->setFile("");
    this->clear_me = false;

}

void lineedit_file::set_check_subdir(const QString slash_subdir)
{
    this->slash_subdir = slash_subdir;
}

void lineedit_file::set_toplevel_dir(const QString &toplevel_dir)
{
    QFileInfo qfi(toplevel_dir);
    this->set_toplevel_dir(qfi);
}

void lineedit_file::set_toplevel_dir(const QFileInfo &qfi_toplevel_dir)
{
    this->qfi_topdir.setFile(qfi_toplevel_dir.absoluteFilePath());
    if (!this->qfi_topdir.isDir() || !this->qfi_topdir.exists()) {
        this->qfi_topdir.setFile("");
        emit valid_topdir(false);
    }
    else {
        emit valid_topdir(true);
    }
}


void lineedit_file::lineedit_changed(const QString &str)
{
    if (this->qfi_topdir.absoluteFilePath().size()) {

        this->setFile(this->qfi_topdir.absoluteFilePath() + "/" + str);
    }
    else this->setFile(str);
    this->set_text();
}

void lineedit_file::set_text()
{
    bool ok = false;
    QObject::disconnect(this, &QLineEdit::textChanged, this, &lineedit_file::lineedit_changed);

    if ((this->what == 0) && this->exists())  {
        this->QLineEdit::setText(this->baseName());
        ok = true;

    }
    else if ((this->what == 1) && this->exists()) {
        this->QLineEdit::setText(this->fileName());
        ok = true;

    }
    else if ((this->what == 2) && this->exists()) {
        this->QLineEdit::setText(this->absoluteFilePath());
        ok = true;

    }

    QObject::connect(this, &QLineEdit::textChanged, this, &lineedit_file::lineedit_changed);

    if (this->slash_subdir.size() && ok && this->isDir()) {
        QFileInfo test_dir(this->absoluteFilePath() + this->slash_subdir);
        if (!test_dir.exists() || !test_dir.isDir()) ok = false;
    }

    // we do not want to emit
    if (this->clear_me) {
        this->clear_me = false;
        return;
    }

    if (ok && this->isDir()) emit valid_dir(ok);
    else emit valid_dir(false);
    if (ok && this->isFile()) emit valid_file(ok);
    else emit valid_file(false);

}
