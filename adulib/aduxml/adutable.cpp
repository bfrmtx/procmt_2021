#include "adutable.h"

adutable::adutable(QWidget *parent)  : QTableView(parent)
{

}
adutable::~adutable()
{
}

void adutable::closeEvent(QCloseEvent *)
{
    emit this->save_table();
    qDebug() << "closing table and save model in case";

}

void adutable::setModel(QAbstractItemModel *model)
{
    QTableView::setModel(model);
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->resizeColumnsToContents();
    this->resizeRowsToContents();
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setAlternatingRowColors(true);
    this->horizontalHeader()->setStretchLastSection(true);

}

void adutable::save()
{
    emit this->save_table();
}

