#include "db_sensor_query.h"



//db_sensor_query::db_sensor_query(QSqlDatabase &dbinuse, QString &message)
//{
//    this->init_db_sensor_query(dbinuse, message);
//}


db_sensor_query::db_sensor_query(const QFileInfo &dbname_info, const int channel, const int slot, const bool opendb, const QString &connection_name,
                                 QObject *parent, std::shared_ptr<msg_logger> msg) : QObject(parent)
{

    QString message;
    this->channel = channel;
    this->slot = slot;

    if (parent != Q_NULLPTR) {
        QObject::connect(this, SIGNAL(tx_cal_message(int, int, QString)), parent, SLOT(rx_cal_message(int, int, QString)));
    }
    if (msg != nullptr) {
        QObject::connect(this, SIGNAL(tx_cal_message(int, int, QString)), msg.get(), SLOT(rx_cal_message(int, int, QString)));

    }


    if (!dbname_info.exists()) {
        message = "db_sensor_query::db_sensor_query -> can not find database " + dbname_info.absoluteFilePath();
        qDebug() << message;
        emit this->tx_cal_message(this->channel, this->slot, message);
        return;
    }

    this->dbname_info = dbname_info;


    if (connection_name.size()) {
        this->connection_name = connection_name;
    }
    else {
        QString temp_str;

        if (this->channel >= 0) {
            temp_str = "calinfo_query_" + QString::number(this->channel);
        }
        else temp_str = "calinfo_query_void";
        this->connection_name = temp_str;
    }



    if (opendb) {
        this->open_db(this->connection_name);
    }

}



db_sensor_query::~db_sensor_query()
{
    if (this->info_db.isOpen()) this->info_db.close();
    // destroy the reference
    this->info_db = QSqlDatabase();
    QSqlDatabase::removeDatabase(this->connection_name);
    qDebug() << "db_sensor_query destroyed";


}

void db_sensor_query::open_db(const QString &connection_name)
{

    QString message;
    if (connection_name.size()) {
        this->connection_name = connection_name;
    }
    else {
        QString temp_str;

        if (this->channel >= 0) {
            temp_str = "calinfo_query_" + QString::number(this->channel);
        }
        else temp_str = "calinfo_query_void";
        this->connection_name = temp_str;
    }

    this->db_type = "QSQLITE";
    this->info_db = QSqlDatabase::addDatabase(this->db_type, this->connection_name);
    this->info_db.setConnectOptions("QSQLITE_OPEN_READONLY");


    this->info_db.setDatabaseName(dbname_info.absoluteFilePath());
    if ( this->info_db.open()) {
        this->init_db_sensor_query(this->info_db, message);
    }
    else {
        message = " failed to open database ";
        qDebug() << message;
        emit this->tx_cal_message(this->channel, this->slot, message);

    }
}

QStringList db_sensor_query::get_H_sensors() const
{

    return this->names_h;
}

QStringList db_sensor_query::get_E_sensors() const
{

    return this->names_e;
}

QStringList db_sensor_query::get_H_sensors_atsnames() const
{
    return this->atsheadernames_h;
}

QStringList db_sensor_query::get_E_sensors_atsnames() const
{
    return this->atsheadernames_e;
}

QString db_sensor_query::get_connection_name() const
{
    return this->connection_name;
}

QFileInfo db_sensor_query::get_dbname_info() const
{
    return this->dbname_info;
}

int db_sensor_query::get_channel_no() const
{
    return this->channel;
}

int db_sensor_query::get_slot_no() const
{
    return this->slot;
}

QString db_sensor_query::set_atsheader_sensor(const QString &inatsheadername)
{
    if (!this->names_h.size()) return QString("empty");
    int i = 0;
    for (auto &str : this->atsheadernames_h) {
        if (!str.compare(inatsheadername, Qt::CaseInsensitive)) {
            this->name_h = this->names_h.at(i);
            this->atsheadername_h = this->atsheadernames_h.at(i);
            return this->name_h;
        }
        ++i;
    }

    // in case it was "wrong" MFS-06e with minus , correct
    i = 0;
    for (auto &str : this->names_h) {
        if (!str.compare(inatsheadername, Qt::CaseInsensitive)) {
            this->name_h = this->names_h.at(i);
            this->atsheadername_h = this->atsheadernames_h.at(i);
            return this->name_h;
        }
        ++i;
    }

    if (!this->names_e.size()) return QString("empty");
    i = 0;
    for (auto &str : this->atsheadernames_e) {
        if (!str.compare(inatsheadername, Qt::CaseInsensitive)) {
            this->name_e = this->names_e.at(i);
            this->atsheadername_e = this->atsheadernames_e.at(i);
            return this->name_e;
        }
        ++i;
    }

    // in case it was "wrong" MFS-06e with minus , correct
    i = 0;
    for (auto &str : this->names_e) {
        if (!str.compare(inatsheadername, Qt::CaseInsensitive)) {
            this->name_e = this->names_e.at(i);
            this->atsheadername_e = this->atsheadernames_e.at(i);
            return this->name_e;
        }
        ++i;
    }


    return QString("empty");
}


QString db_sensor_query::set_sensor(const QString &inname)
{

    QString message;

    if (!this->names_h.size()) {
        message = "db_sensor_query::set_sensor -> no names loaded" ;
        qDebug() << message;
        emit this->tx_cal_message(this->channel, this->slot, message);
        return QString("empty");
    }
    int i = 0;
    for (auto &str : this->names_h) {
        if (!str.compare(inname, Qt::CaseInsensitive)) {
            this->name_h = this->names_h.at(i);
            this->atsheadername_h = this->atsheadernames_h.at(i);
            message = "db_sensor_query::set_sensor -> " + this->atsheadername_h;
            qDebug() << message;
            emit this->tx_cal_message(this->channel, this->slot, message);
            return this->atsheadername_h;
        }
        ++i;
    }
    // in case MFS06e was used, correct it to MFS-06e
    i = 0;
    for (auto &str : this->atsheadernames_h) {
        if (!str.compare(inname, Qt::CaseInsensitive)) {
            this->name_h = this->names_h.at(i);
            this->atsheadername_h = this->atsheadernames_h.at(i);
            message = "db_sensor_query::set_sensor -> " + this->atsheadername_h;
            qDebug() << message;
            emit this->tx_cal_message(this->channel, this->slot, message);
            return this->atsheadername_h;
        }
        ++i;
    }

    if (!this->names_e.size()) return QString("empty");
    i = 0;
    for (auto &str : this->names_e) {
        if (!str.compare(inname, Qt::CaseInsensitive)) {
            this->name_e = this->names_e.at(i);
            this->atsheadername_e = this->atsheadernames_e.at(i);
            message = "db_sensor_query::set_sensor -> " + this->atsheadername_e;
            qDebug() << message;
            emit this->tx_cal_message(this->channel, this->slot, message);
            return this->atsheadername_e;
        }
        ++i;
    }
    // in case MFS06e was used, correct it to MFS-06e
    i = 0;
    for (auto &str : this->atsheadernames_e) {
        if (!str.compare(inname, Qt::CaseInsensitive)) {
            this->name_e = this->names_e.at(i);
            this->atsheadername_e = this->atsheadernames_e.at(i);
            message = "db_sensor_query::set_sensor -> " + this->atsheadername_e;
            qDebug() << message;
            emit this->tx_cal_message(this->channel, this->slot, message);
            return this->atsheadername_e;
        }
        ++i;
    }
    message = "db_sensor_query::set_sensor -> empty";
    qDebug() << message;
    emit this->tx_cal_message(this->channel, this->slot, message);
    return QString("empty");
}



void db_sensor_query::init_db_sensor_query(QSqlDatabase &dbinuse, QString &message)
{
    if (!dbinuse.isOpen()) {
        message = "db_sensor_query::init_db_sensor_query -> database not open!" ;
        qDebug() << message;
        emit this->tx_cal_message(this->channel, this->slot, message);

        return;
    }
    QSqlQuery query(dbinuse);
    QString querystr;
    querystr = "select sensortypes.Name, sensortypes.atsheadername from sensortypes where  sensortypes.Unit like \"H\";";
    if( query.exec(querystr) ) {
        while (query.next()) {
            this->names_h.append(query.value(0).toString());
            this->atsheadernames_h.append(query.value(1).toString());
        }

        if (this->names_h.size() == this->atsheadernames_h.size()) {
            this->name_h = this->names_h.at(0);
            this->atsheadername_h = this->atsheadernames_h.at(0);

        }
    }
    else {
        message = "db_sensor_query::init_db_sensor_query -> query H sensors  failed" ;
        emit this->tx_cal_message(this->channel, this->slot, message);
        qDebug() << message;
    }
    query.clear();

    querystr = "select sensortypes.Name, sensortypes.atsheadername from sensortypes where  sensortypes.Unit like \"E\";";
    if( query.exec(querystr) ) {
        while (query.next()) {
            this->names_e.append(query.value(0).toString());
            this->atsheadernames_e.append(query.value(1).toString());
        }

        if (this->names_e.size() == this->atsheadernames_e.size()) {
            this->name_e = this->names_e.at(0);
            this->atsheadername_e = this->atsheadernames_e.at(0);

        }
    }
    else {
        message = "db_sensor_query::init_db_sensor_query -> query E sensors  failed" ;
        qDebug() << message;
        emit this->tx_cal_message(this->channel, this->slot, message);

    }

    QSqlQuery querya(dbinuse);
    QString query_str;

    query_str = "SELECT `alias_or_headername`, `true_name` FROM `sensor_aliases`" ;
    if (querya.exec(query_str)) {
        while (querya.next()) {
            this->sensor_aliases.append(querya.value(0).toString());
            this->sensor_aliases_true_names.append(querya.value(1).toString());

        }
    }
    else {
        message = "db_sensor_query::init_db_sensor_query failed to get sensor aliases";
        qDebug() << message;
        emit this->tx_cal_message(this->channel, this->slot, message);

    }

    if (sensor_aliases.size() < 3) {
        message = "db_sensor_query::init_db_sensor_query failed to get sensor aliases - name too short";
        qDebug() << message;
        emit this->tx_cal_message(this->channel, this->slot, message);

    }
    else {
        message = "cal database open";
        emit this->tx_cal_message(this->channel, this->slot, message);
    }
}
