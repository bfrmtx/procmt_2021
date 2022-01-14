#include "prc_com_table.h"


prc_com_table::prc_com_table(QObject *parent) :  QAbstractTableModel(parent)

{
    this->header.insert(0, "item");
    this->header.insert(1, "data");
}

prc_com_table::~prc_com_table()
{
    this->clear_variant_lists();
    this->clear();
}

int prc_com_table::columnCount(const QModelIndex &/*parent*/) const
{

    return this->header.size();

}

int prc_com_table::rowCount(const QModelIndex &parent) const
{
    return this->map.size();
}

QVariant prc_com_table::data(const QModelIndex &index, int role) const

{
    if (!index.isValid()) return QVariant();
    if ((role == Qt::DisplayRole)  || (role == Qt::EditRole)) {
        QString key;
        QVariant value;
        this->get_row_key_value(index, key, value);
        if (index.column() == 0) return key;
        if (index.column() == 1) {
            QString str(qvariant_double_to_sci_str(value));
            if (str.size()) {
                return str;
            }
            else return value.toString();
        }
    }
    return QVariant();
}

QVariant prc_com_table::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation == Qt::Horizontal) {
        return this->header.value(section);
    }
    return QVariant();
}

Qt::ItemFlags prc_com_table::flags(const QModelIndex &index) const
{

    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}


bool prc_com_table::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::EditRole)
        return false;

    if (index.column() == 1) {
        return this->set_row_key_value(index, value);
    }
    return false;
}

bool prc_com_table::get_row_key_value(const QModelIndex &index, QString &key, QVariant &value) const
{

    if (index.row() < this->map.size()) {
        int j = 0;
        QMap<QString, QVariant>::const_iterator i = this->map.constBegin();
        while ( i != this->map.constEnd() ) {
            if (j == index.row()) {
                key = i.key();
                value = i.value();
                return true;
            }
            ++i;
            ++j;
        }
    }

    return false;

}

bool prc_com_table::set_row_key_value(const QModelIndex &index, const QVariant &value)
{
    if (index.row() < this->map.size()) {
        int j = 0;
        QMap<QString, QVariant>::iterator i = this->map.begin();
        while ( i != this->map.end()  ) {
            if (j == index.row()) {
                i.value() = value;
                return true;
            }
            ++i;
            ++j;
        }
    }
    return false;
}

int prc_com_table::set_map(const QMap<QString, QVariant> &inmap)
{
    if (!inmap.size()) return 0;

    // avoid copy on write for threads
    QMap<QString, QVariant> tmpmap;

    QMap<QString, QVariant>::const_iterator i = inmap.constBegin();
    while ( i != inmap.constEnd() ) {
        tmpmap.insert(i.key(), i.value());
        ++i;
    }
    if (this->includes.size()) {
        QMap<QString, QVariant>::iterator j = tmpmap.begin();
        while ( j != tmpmap.end() ) {
            if (!this->includes.contains(j.key())) {
                j = tmpmap.erase(j);
            }
            else ++j;
        }
    }

    if (this->excludes.size()) {
        QMap<QString, QVariant>::iterator k = tmpmap.begin();
        while ( k != tmpmap.end() ) {
            qDebug() << k.key();
            if (this->excludes.contains(k.key())) {
                k = tmpmap.erase(k);
            }

            else ++k;
        }
    }

    QMap<QString, QVariant>::iterator l = tmpmap.begin();
    while ( l != tmpmap.end() ) {
        this->map.insert(l.key(), l.value());
        ++l;
    }

    return this->map.size();
}



QMap<QString, QVariant> prc_com_table::get_map() const
{
    return this->map;
}

int prc_com_table::set_includes(const QStringList &includes)
{
    // avoid copy on write for threads
    for (auto &str : includes) {
        this->includes.append(str);
    }
    return this->includes.size();
}

QStringList prc_com_table::get_includes() const
{
    return this->includes;
}

int prc_com_table::set_excludes(const QStringList &excludes)
{
    // avoid copy on write for threads
    for (auto &str : excludes) {
        this->excludes.append(str);
    }
    return this->excludes.size();
}

QStringList prc_com_table::get_excludes() const
{
    return this->excludes;
}

void prc_com_table::clear()
{
    this->map.clear();
    this->includes.clear();
    this->excludes.clear();
}

int prc_com_table::read_cvs_table(const QFileInfo &qfi, const int channel, const int slot)
{

    QString message;
    if (!qfi.exists()) {
        message = "prc_com_table::read_cvs_table -> file not exists: " + qfi.absoluteFilePath();
        qDebug() << message;
            emit this->prc_com_table_message(channel, slot, message);

        return 0;
    }
    QTextStream qts;
    QFile csvfile;
    QString line;
    QStringList line_items;
    this->csv_data.clear();
    int cols;
    csvfile.setFileName(qfi.absoluteFilePath());

    qts.setDevice(&csvfile);
    if (csvfile.open(QIODevice::ReadOnly)) {
        line = qts.readLine().trimmed();
        line = line.simplified();
        line = line.toLower();

        line_items = line.split(",");
        cols = line_items.size();
        for (int i = 0; i < line_items.size(); ++i) {
            this->csv_header_cols.insert(line_items.at(i).simplified(), i);
            this->csv_data.append(QSharedPointer<QList<QVariant>>(new QList<QVariant>()));
        }

        do {
            line = qts.readLine().trimmed();
            line = line.simplified();
            line_items = line.split(",");
            if(line_items.size() == cols) {
                for (int i = 0; i < line_items.size(); ++i) {
                    this->csv_data[i]->append(QVariant(line_items.at(i).simplified()));
                }
            }

        } while ( !qts.atEnd() );
    }
    else {
        message = "prc_com_table::read_cvs_table -> error parsing CSV table";
        qDebug() << message;
            emit this->prc_com_table_message(channel, slot, message);

        return 0;
    }

    message = "prc_com_table::read_cvs_table -> read " + QString::number(csv_data.at(0)->size()) + " rows and "  + QString::number(csv_data.size()) + " columns";
    qDebug() << message;
        emit this->prc_com_table_message(channel, slot, message);

    return csv_data.at(0)->size();

}

int prc_com_table::sort_csv_table(const QString col_name, const bool use_double, const int channel, const int slot)
{

    QString message;
    if (this->csv_data.size() == 0) {
        message = "prc_com_table::sort_csv_table -> no csv_data available";
        qDebug() << message;
            emit this->prc_com_table_message(channel, slot, message);

        return 0;
    }

    if (!this->csv_data.at(0)->size()) {
        message = "prc_com_table::sort_csv_table -> at least csv_data no data in cloumn 0";
        qDebug() << message;
            emit this->prc_com_table_message(channel, slot, message);

        return 0;
    }
    if (!this->csv_header_cols.contains(col_name)) {
        message = "prc_com_table::sort_csv_table -> column name not available: " + col_name;
        qDebug() << message;
            emit this->prc_com_table_message(channel, slot, message);

        return 0;
    }

    int col = this->csv_header_cols.value(col_name);

    std::vector<std::pair<QVariant, int>> sortcol;



    // get data from columnn to sort
    QList<QVariant> tmp_sortcol(this->get_csv_column(col_name));

    // swap
    QList<QSharedPointer<QList<QVariant>>> csv_data_sort;
    std::swap(this->csv_data, csv_data_sort);
    // prepare empty
    for (int i = 0; i < csv_data_sort.size(); ++i) {
        this->csv_data.append(QSharedPointer<QList<QVariant>>(new QList<QVariant>()));
    }

    bool is_ok;
    int cnt = 0;

    for (auto &val : tmp_sortcol) {
        if (use_double) {
            double d;
            d = val.toDouble(&is_ok);
            if (!is_ok) {
                message = "prc_com_table::sort_csv_table -> can not convert to double, line: " + QString::number(cnt) + " col: " + QString::number(col);
                qDebug() << message;
                    emit this->prc_com_table_message(channel, slot, message);

                return 0;
            }
            sortcol.push_back(std::make_pair(d, cnt));
        }
        else {
            sortcol.push_back(std::make_pair(val, cnt));
        }
        ++cnt;
    }

    tmp_sortcol.clear();

    // low first
    // below does not work with Qt6
    // std::sort(sortcol.begin(), sortcol.end(), iter::cmp_pair_first_lt<std::pair<QVariant, int>>());
    std::sort(sortcol.begin(), sortcol.end(), [](auto &left, auto &right) { return left.first.toDouble() < right.first.toDouble();});
    for (size_t j = 0; j < sortcol.size(); ++j) {
        for (int k = 0; k < csv_data_sort.size(); ++k) {
            this->csv_data[k]->append(csv_data_sort.at(k)->at(sortcol.at(j).second));
        }
    }

    this->clear_and_resize_llv(csv_data_sort, false);

    return csv_data.at(0)->size();
}

int prc_com_table::sort_csv_table_with_chopper(const QString col_name, const QString chopper_name, const int channel, const int slot)
{
    this->split_chopper(col_name, chopper_name);

    std::swap(this->csv_data, this->csv_data_off);
    // sort on data_off as "cvs data"
    this->sort_csv_table(col_name, true);
    std::swap(this->csv_data, this->csv_data_off);


    std::swap(this->csv_data, this->csv_data_on);
    // sort on data_on
    this->sort_csv_table(col_name, true);
    // data si back in on now and csv restored
    std::swap(this->csv_data, this->csv_data_on);

    // make a empty
    this->clear_and_resize_llv(this->csv_data, true);

    // copy first on data and then off data, hardcopy true does only append to col, not making new cols
    this->hardcopy(this->csv_data_on, this->csv_data, true);
    this->hardcopy(this->csv_data_off, this->csv_data, true);


    if (!this->csv_data.size()) return 0;
    return this->csv_data.at(0)->size();
}

int prc_com_table::remove_duplicates_from_sorted(const QString col_name, const QString qlistvariant_name, const bool use_double, const int channel, const int slot)
{


    QString message;
    if (this->csv_data.size() == 0) {
        message = "prc_com_table::remove_duplicates_from_sorted -> no csv_data available";
        qDebug() << message;
            emit this->prc_com_table_message(channel, slot, message);

        return 0;
    }

    if (!this->csv_data.at(0)->size()) {
        message = "prc_com_table::remove_duplicates_from_sorted -> at least csv_data no data in cloumn 0";
        qDebug() << message;
            emit this->prc_com_table_message(channel, slot, message);

        return 0;
    }
    if (!this->csv_header_cols.contains(col_name)) {
        message = "prc_com_table::remove_duplicates_from_sorted -> column name not available: " + col_name;
        qDebug() << message;
            emit this->prc_com_table_message(channel, slot, message);

        return 0;
    }

    int col = this->csv_header_cols.value(col_name);
    bool is_ok;
    int cnt = 0;
    QList<QVariant> tmp_colitems(this->get_csv_column(col_name, qlistvariant_name, channel));


    // we should have a QVariant string type still!

    QList<QSharedPointer<QList<QVariant>>> csv_data_dup;
    std::swap(this->csv_data, csv_data_dup);

    std::vector<std::pair<QVariant, int>> dupcol;

    for (auto &val : tmp_colitems) {
        if (use_double) {
            double d;
            d = val.toDouble(&is_ok);
            if (!is_ok) {
                message = "prc_com_table::remove_duplicates_from_sorted -> can not convert to double, line: " + QString::number(cnt) + " col: " + QString::number(col);
                qDebug() << message;
                    emit this->prc_com_table_message(channel, slot, message);

                return 0;
            }
            dupcol.push_back(std::make_pair(d, cnt));
        }
        else {
            dupcol.push_back(std::make_pair(val, cnt));
        }
        ++cnt;
    }

    tmp_colitems.clear();

    // as guessed, the data must have been sorted in advance

    size_t j = 1;
    for (size_t i = 1; i < dupcol.size(); ++i) {
        if (dupcol.at(i - j).first != dupcol.at(i).first ) {
            // i to copy
            for (int k = 0; k < csv_data_dup.size(); ++k) {
                this->csv_data[k]->append(csv_data_dup.at(k)->at(dupcol.at(i).second));
            }
            j = 1;
        }
        else {
            message = "prc_com_table::remove_duplicates_from_sorted -> ************* DUPLICATES please check " + dupcol.at(i).first.toString();

                emit this->prc_com_table_message(channel, slot, message);

            ++j;
        }
    }


    return 1;



}

int prc_com_table::split_chopper(const QString col_name, const QString chopper_name, const int channel, const int slot)
{
    QString message;
    if (this->csv_data.size() == 0) {
        message = "prc_com_table::split_chopper -> no csv_data available";
        qDebug() << message;
            emit this->prc_com_table_message(channel, slot, message);

        return 0;
    }

    if (!this->csv_data.at(0)->size()) {
        message = "prc_com_table::split_chopper -> at least csv_data no data in cloumn 0";
        qDebug() << message;
            emit this->prc_com_table_message(channel, slot, message);

        return 0;
    }
    if (!this->csv_header_cols.contains(col_name)) {
        message = "prc_com_table::split_chopper -> column name not available: " + col_name;
        qDebug() << message;
            emit this->prc_com_table_message(channel, slot, message);

        return 0;
    }
    int col = this->csv_header_cols.value(col_name);

    if (!this->csv_header_cols.contains(chopper_name)) {

        message = "prc_com_table::split_chopper -> column for CHOPPER not available: " + chopper_name;
        qDebug() << message;
            emit this->prc_com_table_message(channel, slot, message);

        return 0;
    }
    int choppercol = this->csv_header_cols.value(chopper_name);

    for (int i = 0; i < this->csv_data.size(); ++i) {
        this->csv_data_on.append(QSharedPointer<QList<QVariant>>(new QList<QVariant>()));
        this->csv_data_off.append(QSharedPointer<QList<QVariant>>(new QList<QVariant>()));
    }

    // hardcopy and fetch the column to sort; col vector is i
    QList<QVariant>::ConstIterator cb;
    int k;
    for (int i = 0; i < this->csv_data.size(); ++i) {
        // for list i (column i)
        k = 0;
        for (cb = this->csv_data.at(i)->cbegin(); cb != this->csv_data.at(i)->cend(); ++cb) {
            //qDebug() << *cb;
            if (this->csv_data.at(choppercol)->at(k).toDouble() < 1) {
                this->csv_data_off[i]->append(*cb);
            }
            else {
                this->csv_data_on[i]->append(*cb);
            }
            ++k;
        }
    }

    qDebug() << "splitting done";

    if (this->csv_data_on.size()) {
        if (this->csv_data_on.at(0)->size()) {
            message = "prc_com_table::split_chopper -> CHOPPER ON entries:  " + QString::number(this->csv_data_on.at(0)->size());
            qDebug() << message;
                emit this->prc_com_table_message(channel, slot, message);

        }
    }

    if (this->csv_data_off.size()) {
        if (this->csv_data_off.at(0)->size()) {
            message = "prc_com_table::split_chopper -> CHOPPER OFF entries: " + QString::number(this->csv_data_off.at(0)->size());
            qDebug() << message;
                emit this->prc_com_table_message(channel, slot, message);

        }
    }


    if (this->csv_data_off.size() && this->csv_data_on.size()) {
        return this->csv_data_on.at(0)->size() + this->csv_data_off.at(0)->size();
    }
    if (this->csv_data_off.size() && !this->csv_data_on.size()) {
        this->csv_data_off.at(0)->size();
    }
    if (!this->csv_data_off.size() && this->csv_data_on.size()) {
        return this->csv_data_on.at(0)->size();
    }
    return 0;


}

std::vector<double> prc_com_table::get_dvector_column(const QString colname, const QString qlistvariant_name, const int channel, const int slot) const
{
    std::vector<double> data;
    QList<QVariant> lvec = this->get_csv_column(colname, qlistvariant_name);
    if (!lvec.size()) return data;
    data.reserve(lvec.size());
    for (auto &dat : lvec) {
        data.push_back(dat.toDouble());
    }

    return data;
}

QList<QVariant> prc_com_table::get_csv_column(const QString colname, const QString qlistvariant_name, const int channel, const int slot) const
{
    QList<QVariant> coldata;
    QString message;
    int col;
    if (!this->csv_header_cols.contains(colname)) {
        message = "prc_com_table::get_csv_column -> header does not contain " + colname;
        qDebug() << message;
            emit this->prc_com_table_message(channel, slot, message);

        return coldata;
    }
    col = this->csv_header_cols.value(colname);
    if (this->csv_data.size() < col-1) {
        message = "prc_com_table::get_csv_column -> column size mismatch " + QString::number(this->csv_data.size()) + "size, access wanted: " + QString::number((col-1));
        qDebug() << message;
            emit this->prc_com_table_message(channel, slot, message);

        return coldata;
    }

    if (!qlistvariant_name.size() || qlistvariant_name == "csv_data" ) {
        for (int j = 0; j < this->csv_data.at(col)->size(); ++j) {
            coldata.append(this->csv_data.at(col)->at(j));
        }
    }

    if (qlistvariant_name == "csv_data_on") {
        for (int j = 0; j < this->csv_data_on.at(col)->size(); ++j) {
            coldata.append(this->csv_data_on.at(col)->at(j));
        }
    }

    if (qlistvariant_name == "csv_data_off") {
        for (int j = 0; j < this->csv_data_off.at(col)->size(); ++j) {
            coldata.append(this->csv_data_off.at(col)->at(j));
        }
    }

    return coldata;
}

void prc_com_table::clear_variant_lists()
{
    this->clear_and_resize_llv(this->csv_data, false);
    this->clear_and_resize_llv(this->csv_data_off, false);
    this->clear_and_resize_llv(this->csv_data_on, false);
}

void prc_com_table::clear_and_resize_llv(QList<QSharedPointer<QList<QVariant>>> &rebuildme, bool prepare_again)
{
    int sz = rebuildme.size();
    for (int i = 0; i < rebuildme.size(); ++i) {
        rebuildme[i]->clear();
        rebuildme[i].reset();
    }
    rebuildme.clear();

    if (prepare_again) {
        for (int i = 0; i < sz; ++i) {
            rebuildme.append(QSharedPointer<QList<QVariant>>(new QList<QVariant>()));
        }
    }

}

void prc_com_table::hardcopy(QList<QSharedPointer<QList<QVariant> > > &in, QList<QSharedPointer<QList<QVariant> > > &out, const bool append_to_cols_only)
{
    // in case we can append at the end of the cols; in this case size would not change
    if (!append_to_cols_only) {
        for (int i = 0; i < in.size(); ++i) {
            out.append(QSharedPointer<QList<QVariant>>(new QList<QVariant>()));
        }
    }

    // hardcopy and fetch the column to sort; col vector is i
    QList<QVariant>::ConstIterator cb;
    for (int i = 0; i < in.size(); ++i) {
        // for list i (column i)
        for (cb = in.at(i)->cbegin(); cb != in.at(i)->cend(); ++cb) {
            //qDebug() << *cb;
            out[i]->append(*cb);
        }
    }
}
