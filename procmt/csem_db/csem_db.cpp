#include "csem_db.h"

csem_db::csem_db(const QFileInfo qfi_base, const QString subdir_str,  QObject *parent) : QObject(parent)
{

    QDir ndir(qfi_base.absoluteFilePath());
    if (!qfi_base.exists()) {
        if(ndir.mkdir(qfi_base.absoluteFilePath())) {
            this->dir.setPath(qfi_base.absoluteFilePath());
        }

    }

    if (qfi_base.exists() && subdir_str.size()) {
        QFileInfo c(qfi_base.absoluteFilePath() + "/" + subdir_str);
        ndir.setPath(c.absoluteFilePath());
        if (!c.exists()) {
            if (!ndir.mkdir(c.absoluteFilePath())) {
                qFatal("csem_db::csem_db NO DIRECTORY TO WRITE INTO");
            }
        }
        else {
            this->dir.setPath(c.absoluteFilePath());
        }

    }
    if (!qfi_base.exists()) {
        qFatal("csem_db::csem_db NO DIRECTORY TO WRITE INTO");
    }


    this->columns = pmt::csem_dipcols;

    // append BUT 0, 1 are reserved for the dipoles
    tables << "dip_1" << "dip_2";


}

csem_db::~csem_db()
{
    if (this->csemdb.isOpen()) this->csemdb.close();

}

QFileInfo csem_db::create_new_db(const QString &dbname_name_only)
{

    this->clear();
    QFileInfo tmpfi(dir.absolutePath() + "/" + dbname_name_only + ".sql3");

    if (tmpfi.exists()) {

        for (int i = 1; i < 1000; ++i) {
            tmpfi.setFile(dir.absolutePath() + "/" + dbname_name_only + "_" + QString::number(i) + ".sql3");
            if (!tmpfi.exists()) break;
        }
        if (tmpfi.exists()) {
            qDebug("csem_db::create_new_db more than 1000 databases or other error");
            return this->dbname;
        }
    }


    this->csemdb = QSqlDatabase::addDatabase("QSQLITE");
    this->csemdb.setDatabaseName(tmpfi.absoluteFilePath());
    if (this->csemdb.open()) {
        QSqlQuery query;
        for (auto &str : this->tables) {
            QString qstr = "CREATE TABLE " + str + "( ";
            int i = 0;
            int icols = this->columns.size();
            for (auto &colstr : this->columns) {
                if (i == 0) qstr += " `" + colstr + "` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE";
                else qstr += " `" + colstr + "` TEXT";
                if (i < icols - 1) qstr += " ,";
                ++i;
            }

            qstr += ")";

            query.exec(qstr);
            qDebug() << qstr;
            qstr.clear();
        }

        this->dbname = tmpfi;
    }
    else return QFileInfo();
    return this->dbname;
}

QFileInfo csem_db::filename() const
{
    return this->dbname;
}

QFileInfo csem_db::open(const QString &dbname_name_only)
{
    this->clear();
    QFileInfo tmpfi(dir.absolutePath() + "/" + dbname_name_only + ".sql3");
    this->csemdb = QSqlDatabase::addDatabase("QSQLITE");
    this->csemdb.setDatabaseName(tmpfi.absoluteFilePath());
    if (this->csemdb.open()) {
        this->dbname = tmpfi;
    }

    return dbname;

}

void csem_db::close()
{
    this->csemdb.close();
    for (auto &dip : this->dip_1) dip.clear();
    for (auto &dip : this->dip_2) dip.clear();
}

void csem_db::clear()
{
    if (this->csemdb.isOpen()) this->csemdb.close();
    this->dbname.setFile("");

    for (auto &dip : this->dip_1) dip.clear();
    for (auto &dip : this->dip_2) dip.clear();
}

bool csem_db::update_true_or_insert_false(const QMap<QString, QVariant> &data, const int &dip1_1__dip2_2, int &rowid) const
{
    QSqlQuery query;
    QString qstr;
    rowid = -1;

    qstr.append("SELECT rowid, site_name, freq FROM ");
    if (dip1_1__dip2_2 == 1) {
        qstr.append("main.dip_1 ");
    }
    if (dip1_1__dip2_2 == 2) {
        qstr.append("main.dip_2 ");
    }
    qstr.append("WHERE site_name = '");
    qstr.append(data.value("site_name").toString());
    qstr.append("' AND freq = '");
    qstr.append(data.value("freq").toString());
    qstr.append("'");

    qDebug() << qstr;
    if (query.exec(qstr)) {
        while (query.next()) {
            rowid = query.value(0).toInt();
            if (rowid <= 0) {
                rowid = -1;
                return false;
            }
            else return true;
            //           qDebug() << "next" << query.value(0) << query.record().count();
        }

    }

    return false;
}

int csem_db::slot_insert_update(const QMap<QString, QVariant> &data, const int &dip1_1__dip2_2)
{
    if (!this->csemdb.isOpen()) return 0;

    if (!this->db_has_data_keys(data)) return 0;
    QString qstr;
    QString colnames, colvalues;
    qstr.reserve(12200);

    QSqlQuery query;
    int rowid = -1;
    QString tab(tables.at(0));
    if (dip1_1__dip2_2 == 2) tab = tables.at(1);


    // rowid is not part of the data

    if (this->update_true_or_insert_false(data, dip1_1__dip2_2, rowid)) {
        qstr = "UPDATE " + tab + " SET ";
            auto iter = data.constBegin();
            auto comma = data.constEnd();
            --comma;
            while (iter != data.constEnd()) {
                qstr.append("`");
                qstr.append(iter.key());
                qstr.append("` = '");
                qstr.append(iter.value().toString());
                if (iter != comma) qstr.append("', ");
                else qstr.append("' ");


                ++iter;
            }
            qstr.append(" WHERE ""rowid"" = ");
            qstr.append(QString::number(rowid));


    }
    else {
        this->QMAP_to_SQL(data, colnames, colvalues);

        qstr = "INSERT into " + tab + " (";
        qstr.append(colnames);
        qstr.append(") VALUES (");
        qstr.append(colvalues);
        qstr.append(")");

    }


    colnames.clear(); colvalues.clear();
    qDebug() << qstr;
    if (query.exec(qstr)) {
        return 1;
    }


    return 0;

}

int csem_db::write_data_file()
{
    if (!this->csemdb.isOpen()) return 0;

    QString qstr;
    QSqlQuery query;
    QFile file;
    QFileInfo dbdat(this->dbname.absolutePath() + "/" + this->dbname.completeBaseName() + "_dip_1" + ".dat");

    qstr.append("SELECT * from dip_1");
    if (query.exec(qstr)) {
        QFileInfo new_dbdat(next_filename(dbdat));
        file.setFileName(new_dbdat.absoluteFilePath());
        if (file.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
            QTextStream out(&file);
            out.setFieldWidth(24);
            int i = 0;
            for (auto &str : this->columns) {
               if (i++) out << str;
            }
            out << Qt::endl;
            while (query.next()) {
                for (int iq = 1; iq < query.record().count(); ++iq) {
                    out << query.value(iq).toString();
                }
                out << Qt::endl;
            }

        }
        file.close();
    }
    dbdat.setFile(this->dbname.absolutePath() + "/" + this->dbname.completeBaseName() + "_dip_2" + ".dat");
    qstr.clear();
    query.clear();
    qstr.append("SELECT * from dip_2");

    if (query.exec(qstr)) {
        QFileInfo new_dbdat(next_filename(dbdat));
        file.setFileName(new_dbdat.absoluteFilePath());
        if (file.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
            QTextStream out(&file);
            out.setFieldWidth(24);
            int i = 0;
            for (auto &str : this->columns) {
               if (i++) out << str;
            }
            out << Qt::endl;
            while (query.next()) {
                for (int iq = 1; iq < query.record().count(); ++iq) {
                    out << query.value(iq).toString();
                }
                out << Qt::endl;
            }

        }
        file.close();
    }



    return this->dip_1.size();
}

int csem_db::QMAP_to_SQL(const QMap<QString, QVariant> &data, QString &colnames, QString &colvalues)
{
    colnames.clear();
    colvalues.clear();
    colnames.reserve(6000);
    colvalues.reserve(6000);
    int i = 0;
    auto iter = data.constBegin();
    while (iter != data.constEnd()) {
        colnames.append("`");
        colvalues.append("'");
        colnames.append(iter.key());
        colvalues.append(iter.value().toString());
        colnames.append("`");
        colvalues.append("'");
        if (i < data.size() - 1) {
            colnames.append(", ");
            colvalues.append(", ");
        }
        ++i;
        ++iter;
    }

    return i;

}

bool csem_db::db_has_data_keys(const QMap<QString, QVariant> &data)
{
    if (!data.size()) return false;

    int count = 0;
    auto kystr(data.cbegin());
    for (kystr = data.cbegin(); kystr != data.cend(); ++kystr) {
        if (this->columns.contains(kystr.key())) ++count;
    }

    if (count == data.size()) return true;
    return false;
}
