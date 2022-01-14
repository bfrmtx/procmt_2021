#ifndef ADUTABLE_H
#define ADUTABLE_H

#include <QObject>
#include <QCloseEvent>
#include <QDebug>
#include <QTableView>
#include <QHeaderView>

class adutable : public QTableView
{
    Q_OBJECT
public:
    explicit adutable(QWidget * parent = 0);
    ~adutable();

    void closeEvent(QCloseEvent *);

    void setModel(QAbstractItemModel * model);

    /*!
     * \brief save emits adutable::save_table() so that the data object can call is save routine
     */
    void save();


signals:
    void save_table();
};


#endif // ADUTABLE_H
