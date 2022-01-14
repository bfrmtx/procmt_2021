#include "sql_vector.h"


SQL_vector::SQL_vector(const QFileInfo &dbname, const QString &connection_name, const QString &db_type)
{


    if (!dbname.exists()) {
        qDebug() << "Sql_vector::can not open database" << dbname.absoluteFilePath() << Qt::endl;
        this->is_open = false;
        return;
    }

    this->connection_name = connection_name;
    this->db_type = db_type;
    this->db = std::make_unique<QSqlDatabase>(QSqlDatabase::addDatabase(this->db_type, this->connection_name));
    this->db->setConnectOptions("QSQLITE_OPEN_READONLY");

    this->db->setDatabaseName(dbname.absoluteFilePath());
    this->is_open = true;
}

SQL_vector::~SQL_vector()
{
    if (this->is_open) {
        if (this->db->isOpen()) this->db->close();
        this->db.reset();
        QSqlDatabase::removeDatabase(QSqlDatabase::database().connectionName());
    }



}

size_t SQL_vector::get_vector(const QString &table, const QString &col, std::vector<double> &v)
{

    if (!this->is_open) {
        std::cerr << "SQL_vector::get_vector database not open" << std::endl;
        return 0;
    }
    v.clear();
    if ( this->db->open()) {
        QSqlQuery query(*db.get());
        //select `x` , `y`  from `krszg_table_4_3` where `x` AND `y` not NULL
        QString querystr = "select " + this->enquote(col) + " from " + this->enquote(table) + " where " +
                this->enquote(col) + " not NULL;";

        if( query.exec(querystr) ) {
            qDebug() << query.size();
            while (query.next()) {
                v.push_back(query.value(0).toDouble());
            }
        }
        else {
            qDebug() << "SQL_vector::get_vector query failed" ;
            qDebug() << querystr;
        }

    }
    else qDebug() << "SQL_vector::get_vector database " << this->db->databaseName() << " not open";


    return v.size();
}

size_t SQL_vector::get_vector(const QString &table, const QString &colx, const QString &coly,
                              std::vector<double> &x, std::vector<double> &y)
{
    if (!this->is_open) {
        std::cerr << "SQL_vector::get_vector database not open" << std::endl;
        return 0;
    }
    if ( this->db->open()) {
        x.clear();
        y.clear();
        QSqlQuery query(*db.get());
        QString querystr = "select " + this->enquote(colx) + " , " + this->enquote(coly) + " from " + this->enquote(table) + " where " +
                this->enquote(colx) + " IS NOT NULL " + " AND " + this->enquote(coly) + " IS NOT NULL;";

        if( query.exec(querystr) ) {
            while (query.next()) {
                x.push_back(query.value(0).toDouble());
                y.push_back(query.value(1).toDouble());

            }
        }
        else {
            qDebug() << "SQL_vector::get_vector query failed" ;
            qDebug() << querystr;
        }

    }
    else qDebug() << "SQL_vector::get_vector database " << this->db->databaseName() << " not open";


    return x.size();
}



size_t SQL_vector::get_vector(const QString &table, const QString &colx, const QString &coly, const QString &colz,
                              std::vector<double> &x, std::vector<double> &y, std::vector<double> &z)
{
    if (!this->is_open) {
        std::cerr << "SQL_vector::get_vector database not open" << std::endl;
        return 0;
    }
    if ( this->db->open()) {
        x.clear();
        y.clear();
        z.clear();
        QSqlQuery query(*db.get());
        QString querystr = "select " + this->enquote(colx) + " , " + this->enquote(coly) + " , " + this->enquote(colz)
                + " from " + this->enquote(table) + " where " +
                this->enquote(colx) + " IS NOT NULL " +
                " AND " + this->enquote(coly) + " IS NOT NULL " +
                " AND " + this->enquote(colz) + " IS NOT NULL;";

        if( query.exec(querystr) ) {
            qDebug() << query.size();
            while (query.next()) {
                x.push_back(query.value(0).toDouble());
                y.push_back(query.value(1).toDouble());
                z.push_back(query.value(2).toDouble());


            }
        }
        else {
            qDebug() << "SQL_vector::get_vector query failed" ;
            qDebug() << querystr;
        }

    }
    else qDebug() << "SQL_vector::get_vector database " << this->db->databaseName() << " not open";


    return x.size();


}

QString SQL_vector::enquote(const QString &str) const
{
    if (!str.size()) return "``";
    if (str.startsWith(QChar('`'))) return str;
    else return "`" + str + "`";
}
