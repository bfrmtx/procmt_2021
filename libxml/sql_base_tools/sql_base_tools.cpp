#include "sql_base_tools.h"


sql_base_tools::sql_base_tools()
{
}

int sql_base_tools::copy_complete_table(QSqlDatabase &indb, const QString &in_table_name, QSqlDatabase &outdb,
                                        const QString &out_table_name, QString &message)
{
    if (!indb.isOpen()) return 0;
    if (!outdb.isOpen()) return 0;
    QList<QSqlRecord> allrecords;


    // copy the creation of the table which I want to copy
    QString query_create_str = "SELECT sql FROM sqlite_master WHERE tbl_name = '" + in_table_name + "' AND type = 'table'";
    QSqlQuery query_create(indb);
    if (!query_create.exec(query_create_str)) {
        message = "copy_complete_table::copy_complete_table -> can not creation string from " + in_table_name;
        return 0;
    }
    // create the copy inside out_table

    query_create.first();
    if (!query_create.isValid()) {
        message = "copy_complete_table::copy_complete_table -> can not create copy table " + in_table_name + " invalid query";
        return 0;
    }

    QString query_create_result = query_create.value(0).toString().simplified();

    query_create_result.replace(query_create_result.indexOf(in_table_name), in_table_name.size(), out_table_name);

    QSqlQuery query_createnow(outdb);
    if(!query_createnow.exec(query_create_result)) {
        message = "copy_complete_table::copy_complete_table -> can not create copy table " + in_table_name;
        return 0;
    }
    // table structure copy is created

    // copy all data
    QString query_sa_str = "SELECT * from `" + in_table_name +"`";
    QSqlQuery query_sa(indb);
    if (!query_sa.exec(query_sa_str)) {
        message = "copy_complete_table::copy_complete_table -> can not copy table contents of " + in_table_name;
        return 0;
    }

    while (query_sa.next()) {
        allrecords.append(query_sa.record());
    }
    // all data is copied now and now put into the other database

    QSqlQuery query_insert(outdb);
    QString query_insert_str;
    query_insert_str.reserve(8192);
    int i, j;
    j = 0;
    for (auto &allrecs : allrecords) {
        query_insert_str.append("INSERT INTO `" + out_table_name + "` (");
        for (i = 0; i < allrecs.count() - 1; ++i) {
            query_insert_str.append("`" + allrecs.fieldName(i) + "`, ");
        }
        query_insert_str.append("`" + allrecs.fieldName(i) + "`)  VALUES ( ");
        for (i = 0; i < allrecs.count() - 1; ++i) {
            query_insert_str.append("'" + allrecs.value(i).toString() + "', ");
        }
        query_insert_str.append("'" + allrecs.value(i).toString() + "')");
        //qDebug() <<query_insert_str;

        if(!query_insert.exec(query_insert_str)) {

            message = "copy_complete_table::copy_complete_table -> error inserting in " + out_table_name + " at line: " + QString::number(j);
        }
        ++j;
        query_insert_str.clear();
        query_insert_str.reserve(8192);

    }

    //    for (auto &qr : allrecords) {
    //        for (int i = 0; i < qr.count(); ++i)  qDebug() << qr.fieldName(i) << qr.value(i);
    //    }


    message.clear();


    return 0;
}
