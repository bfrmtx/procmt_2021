#include "dir_dialog.h"
#include "ui_dir_dialog.h"

dir_dialog::dir_dialog(QWidget *parent, const QString &absolute_path, int target) :
    QDialog(parent),
    ui(new Ui::dir_dialog)
{
    ui->setupUi(this);
    this->target = target;
    this->qfsi = new QFileSystemModel(this);
    dir.setPath(absolute_path);
    dir.setCurrent(dir.absolutePath());
    this->setWindowTitle("open folder");

    this->ui->buttonBox->button(QDialogButtonBox::Ok)->setText("selectFolder");

    this->qfsi->setRootPath(dir.currentPath());
    this->qfsi->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);


    this->ui->dir_treeView->setModel(this->qfsi);
    this->ui->dir_treeView->setRootIndex(qfsi->setRootPath(dir.currentPath()));
    this->ui->dir_treeView->setColumnHidden(1, true);
    this->ui->dir_treeView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    this->ui->dir_treeView->resizeColumnToContents(0);



}

dir_dialog::~dir_dialog()
{
    delete ui;
    delete this->qfsi;

}



void dir_dialog::on_dir_treeView_clicked(const QModelIndex &index)
{
  QString mPath = this->qfsi->fileInfo(index).absoluteFilePath();
  emit this->selected_dir(mPath, this->target);
}
