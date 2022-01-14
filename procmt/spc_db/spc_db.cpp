#include "spc_db.h"

spc_db::spc_db(const QFileInfo qfi_base, const QString subdir_str, QObject *parent) : QObject(parent)

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
                qFatal("spc_db::spc_db NO DIRECTORY TO WRITE INTO");
            }
        }
        else {
            this->dir.setPath(c.absoluteFilePath());
        }

    }
    if (!qfi_base.exists()) {
        qFatal("spc_db::spc_db NO DIRECTORY TO WRITE INTO");
    }

}

spc_db::spc_db(QObject *parent): QObject(parent)
{

}

QFileInfo spc_db::create_new_db(const QString &dbname_name_only, const std::vector<acsp> &ac_spectra, std::vector<prc_com> &prcs)
{
    this->clear();
    QFileInfo tmpfi(dir.absolutePath() + "/" + dbname_name_only + ".sql3");

    if (tmpfi.exists()) {

        for (int i = 1; i < 1000; ++i) {
            tmpfi.setFile(dir.absolutePath() + "/" + dbname_name_only + "_" + QString::number(i) + ".sql3");
            if (!tmpfi.exists()) break;
        }
        if (tmpfi.exists()) {
            qDebug("spc_db::create_new_db more than 1000 databases or other error");
            return this->dbname;
        }
    }



    this->spcdb = QSqlDatabase::addDatabase("QSQLITE");
    this->spcdb.setDatabaseName(tmpfi.absoluteFilePath());
    if (this->spcdb.open()) {


        int j = 0;
        for (auto &acv : ac_spectra.at(0).ac) {
            if (acv.size()) {
                this->columns.append(pmt::ac_spectra_names.at(j) + "_re");
                this->columns.append(pmt::ac_spectra_names.at(j) + "_im");

            }
            ++j;
        }
        j = 0;
        for (auto &sspv : ac_spectra.at(0).ssp) {
            if (sspv.size()) {
                this->columns.append(pmt::channel_types.at(j) + "_re");
                this->columns.append(pmt::channel_types.at(j) + "_im");

            }
            ++j;
        }



        for (const auto &acsp : ac_spectra) {
            QSqlQuery query;
            QString dblstr = doublefreq2string_unit(acsp.f).simplified().remove(' ');

            QString qstr = "CREATE TABLE `" + dblstr + "`( ";
            int i = 0;
            int icols = this->columns.size();

            qstr += " `idx` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, ";


            for (auto &colstr : this->columns) {
                qstr += " `" + colstr + "` REAL";
                if (i < icols - 1) qstr += " ,";
                ++i;
            }

            qstr += ")";

            query.exec(qstr);
            // qDebug() << qstr;
            qstr.clear();
            query.clear();


        }



        for (const auto &acsp : ac_spectra) {
            this->spcdb.transaction();
            QSqlQuery query;
            QString qstr;
            qstr.reserve(6000);
            QString dblstr = doublefreq2string_unit(acsp.f).simplified().remove(' ');
            qstr = "INSERT into `" + dblstr + "` (";
            int i = 0;
            for (const auto &str : this->columns) {

                qstr.append("`" + str + "`");
                if (i < this->columns.size()-1) qstr.append(", ");
                ++i;
            }
            qstr.append(") VALUES (");

            i = 0;
            for (const auto &str : this->columns) {
                qstr.append("?");
                if (i < this->columns.size()-1) qstr.append(", ");
                ++i;
            }
            qstr.append(")");
            query.prepare(qstr);


            j = 0;

            size_t k = 0, size = acsp.size();

            for (k = 0; k < size; ++k) {
                i = 0;
                for (auto &acv : acsp.ac) {
                    if (acv.size()) {
                        query.addBindValue(std::real(acv[k]));
                        query.addBindValue(std::imag(acv[k]));
                        i += 2;

                    }
                }
                for (auto &sspv : acsp.ssp) {
                    if (sspv.size()) {
                        query.addBindValue(std::real(sspv[k]));
                        query.addBindValue(std::imag(sspv[k]));
                        i += 2;
                    }
                }

                // qDebug() << this->columns.size() << i;


                // qDebug() << qstr;
                query.exec();

                // qDebug() << query.executedQuery();
            }
            this->spcdb.commit();


        }

        // fill the channel tables now

        for (auto &prc : prcs) {
            QSqlQuery query;
            QString qstr = "CREATE TABLE `chan_" + prc.value("channel_number").toString() + "`( ";
            qstr += " `idx` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, `key` TEXT, `value` TEXT)";

            if (query.exec(qstr)) {

                auto xiter = prc.cbegin();
                while (xiter != prc.cend()) {
                    qstr.clear();
                    qstr  = "INSERT INTO `chan_" + prc.value("channel_number").toString() + "` (`key`, `value`) VALUES ('";
                    qstr += xiter.key() + "', '" + xiter.value().toString() + "')";
                    query.exec(qstr);
                    qDebug() << qstr;
                    ++xiter;
                }

            }

        }



    }
    else return QFileInfo();


    return this->dbname;
}

size_t spc_db::fetch_all(const QFileInfo qfi, std::vector<acsp> &ac_spectra, std::vector<prc_com> &prcs)
{
    if (!qfi.exists()) return 0;
    this->clear();

    this->info_filedb.setFile(procmt_homepath("info.sql3"));
    if (!this->info_filedb.exists()) return 0;

    prc_com header_map;
    this->info_db = QSqlDatabase::addDatabase("QSQLITE", "get_header");
    this->info_db.setConnectOptions("QSQLITE_OPEN_READONLY");
    this->info_db.setDatabaseName(this->info_filedb.absoluteFilePath());
    if (this->info_db.open()) {
        QSqlQuery query(this->info_db);
        QString query_str("SELECT `key`, `value`, `type` from 'atsheader'");
        if( query.exec(query_str) ) {
            while (query.next()) {
                QString key = query.value(0).toString();
                QVariant value = query.value(1);
                QString type = query.value(2).toString();
                header_map.create_from(key, value, type);

            }
        }
        else qDebug() << "spc_db::fetch_all db err: could not get headers" ;

        query_str = ("SELECT `key`, `value`, `type` from 'atsfile'");
        if( query.exec(query_str) ) {
            while (query.next()) {
                QString key = query.value(0).toString();
                QVariant value = query.value(1);
                QString type = query.value(2).toString();
                header_map.create_from(key, value, type);

            }
        }
        else qDebug() << "spc_db::fetch_all db err: could not get headers" ;


    }




    size_t countall = 0;
    this->spcdb = QSqlDatabase::addDatabase("QSQLITE");
    this->spcdb.setDatabaseName(qfi.absoluteFilePath());
    if (this->spcdb.open()) {
        qDebug() << "open";
        if (this->table_names()) {
            ac_spectra.reserve(size_t(this->tables.size()));

            int table_no = 0;
            for (auto &str : this->tables) {

                size_t rows = this->row_count(str);
                if (rows == 0) return 0;
                qDebug() << "rows" << rows;
                ac_spectra.emplace_back(acsp());
                ac_spectra.back().f = this->freqs.at(table_no++);
                QStringList column_names;
                int i = this->ac_ssp_columns(str, column_names);
                if (i == 0) return 0;
                QString qstr;
                qstr.reserve(512);
                qstr = "SELECT ";
                i = 0;
                for (auto &cstr : column_names) {
                    qstr.append("`" + cstr +"`");
                    if (i < column_names.size()-1) qstr.append(", ");
                    ++i;
                }
                qstr.append(" FROM 'main'.'" + str + "'");
                qDebug() << qstr;
                QSqlQuery query;

                if (query.exec(qstr)) {
                    size_t cols = query.record().count();

                    qDebug() << "succ" << rows << cols << column_names.size();

                    std::vector<std::pair<size_t, int>> where_to;
                    where_to.reserve(column_names.size());
                    size_t check;
                    int ac_or_ssp;
                    for (auto &cstr : column_names) {
                        check = pmt::db_cols_to_auto_cross_ssp_index<size_t>(cstr, ac_or_ssp);
                        if (check != SIZE_MAX) {
                            where_to.push_back(std::make_pair(check, ac_or_ssp));
                        }

                    }

                    // try to avoid re-allocation
                    for (auto &pairs : where_to) {
                        if (pairs.second == 1) {
                            ac_spectra.back().ac[pairs.first].reserve(rows);
                        }
                        if (pairs.second == 2) {
                            ac_spectra.back().ssp[pairs.first].reserve(rows);
                        }
                    }



                    // `ExEx_re`, `ExEx_im`, `EyEy_re`, `EyEy_im`, to acpos
                    // procmt alldiefiden
                    // resever atze
                    countall++;
                    while (query.next()) {
                        for (size_t c = 0; c < cols; ++c) {
                            double res = query.value(c++).toDouble();
                            double ims = query.value(c).toDouble();
                            if (where_to.at(c).second == 1) {
                                ac_spectra.back().ac[where_to.at(c).first].push_back(std::complex<double>(res, ims));
                            }
                            if (where_to.at(c).second == 2) {
                                ac_spectra.back().ssp[where_to.at(c).first].push_back(std::complex<double>(res, ims));
                            }


                        }
                    }
                }
                else return 0;


            }

        }
    }

    prcs.clear();
    if (this->channel_tables.size()) {

        for (const auto &str :this->channel_tables) {

            QString qstr;
            qstr = "SELECT `key`, `value` FROM 'main'.'" + str + "'";
            QSqlQuery query;
            if (query.exec(qstr)) {
                QMap<QString, QVariant> map;
                while (query.next()) {
                    QString skey = query.value(0).toString();
                    QVariant qvalue = query.value(1);
                    if (header_map.size()) {
                        map.insert(skey, get_native(qvalue, header_map.value(skey)));

                    }
                    else {
                        map.insert(skey, qvalue);
                    }
                }
                prcs.emplace_back(map);
            }
            else return 0;
        }
    }
    else return 0;


    return countall;
}

size_t spc_db::table_names()
{
    QSqlQuery query;
    QStringList tmp_tables;
    if (query.exec("SELECT tbl_name FROM `main`.sqlite_master  WHERE type='table' AND tbl_name != 'sqlite_sequence'")) {
        while (query.next()) {
            tmp_tables.append(query.value(0).toString());
        }
    }

    if (tmp_tables.size()) {
        for (auto &str : tmp_tables) {
            if (str.endsWith("Hz", Qt::CaseSensitive) ) {
                QString fstr(str.left(str.length() - 2));
                bool ok(false);
                double f = fstr.toDouble(&ok);
                if (ok) {
                    this->freqs.append(f);
                    this->tables.append(str);
                }
            }

            if (str.endsWith("s", Qt::CaseSensitive)) {
                QString fstr(str.left(str.length() - 1));
                bool ok(false);
                double f = fstr.toDouble(&ok);
                if (ok) {
                    this->freqs.append(1.0/f);
                    this->tables.append(str);

                }
            }

            if (str.startsWith("chan_", Qt::CaseSensitive)) {
                this->channel_tables.append(str);
            }


        }
    }

    if (this->tables.size() == this->freqs.size()) return size_t(this->tables.size());
    else return 0;
}

size_t spc_db::row_count(const QString table_name)
{
    // SELECT COUNT(*) FROM (SELECT `_rowid_`,* FROM 'main'.'512.0000s')
    QString str("SELECT COUNT(*) FROM (SELECT `_rowid_`,* FROM 'main'.'");
    str.append(table_name); str.append("')");
    QSqlQuery query(str);
    if (query.exec()) {
        int i;
        bool ok(false);
        while (query.next()) {
            i = query.value(0).toULongLong(&ok);
        }

        if (ok) return i;
    }

    return 0;

}

size_t spc_db::ac_ssp_columns(const QString table_name, QStringList &column_names)
{

    QString str("PRAGMA table_info(`");
    str.append(table_name); str.append("`)");
    QSqlQuery query(str);
    column_names.clear();
    if (query.exec()) {
        int fieldNo = query.record().indexOf("name");
        while (query.next()) {
            QString str(query.value(fieldNo).toString());
            if (str != "idx") column_names.append(str);
        }
    }

    // we have always! _re and _im
    if ((column_names.size() % 2) != 0) {
        column_names.clear();
        return 0;
    }

    return size_t(column_names.size());

}

void spc_db::clear()
{
    if (this->spcdb.isOpen()) this->spcdb.close();
    this->dbname.setFile("");
    this->columns.clear();
}

spc_db::~spc_db()
{
    if (this->spcdb.isOpen()) this->spcdb.close();
}
