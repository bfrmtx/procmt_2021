#include "prc_com.h"

prc_com::prc_com(const bool threaded) : QMap<QString, QVariant>()
{
    this->threaded = threaded;
    this->myname = "prc_com";
}

prc_com::prc_com(const QMap<QString, QVariant> &rhs, const bool threaded) : QMap<QString, QVariant>(){

    QMap<QString,QVariant>::operator =(rhs);
    this->threaded = threaded;

}


prc_com::prc_com(const prc_com &rhs) : QMap<QString, QVariant>()
{

    this->operator =(rhs);

}

prc_com &prc_com::operator = (const prc_com &rhs)
{
    if (&rhs == this) return *this;
    this->threaded = rhs.threaded;
    this->myname = rhs.myname;
    QMap<QString,QVariant>::operator =(rhs);

    return *this;

}

QString prc_com::myname_is() const
{
    return this->myname;
}

//prc_com::~prc_com()
//{
//}


void prc_com::set_class_name(const QString &name, const bool threaded)
{
    this->myname = name;
    this->threaded = threaded;
}

void prc_com::set_data(const QMap<QString, QVariant> &in_map, const bool strip_classname, const bool check_options)
{
    if (this->threaded) std::lock_guard<std::recursive_mutex> lock(this->mutex);

    QString new_key;
    QVariant new_val;
    QMap<QString, QVariant>::const_iterator citer = in_map.constBegin();         /*!< iterator for string data */
    while (citer != in_map.constEnd()) {
        new_key = citer.key();
        if (strip_classname && new_key.contains(this->myname) && this->myname.size()) {
            new_key.remove((this->myname + "_"));
            this->set_key_value(new_key, citer.value(), check_options);
        }
        else if (strip_classname && new_key.contains("global_") && this->myname.size()) {
            new_key.remove(("global_"));
            this->set_key_value(new_key, citer.value(), check_options);
        }
        else if (!strip_classname) this->set_key_value(citer.key(), citer.value(), check_options);
        ++citer;
    }

}

void prc_com::insert_data(const QMap<QString, QVariant> &in_map, const bool strip_classname)
{
    if (this->threaded) std::lock_guard<std::recursive_mutex> lock(this->mutex);

    QString new_key;
    QVariant new_val;
    QMap<QString, QVariant>::const_iterator citer = in_map.constBegin();         /*!< iterator for string data */
    while (citer != in_map.constEnd()) {
        new_key = citer.key();
        if (strip_classname && new_key.contains(this->myname) && this->myname.size()) {
            new_key.remove((this->myname + "_"));
            QMap<QString,QVariant>::insert(new_key, citer.value());
        }
        else if (strip_classname && new_key.contains("global_") && this->myname.size()) {
            new_key.remove(("global_"));
            QMap<QString,QVariant>::insert(new_key, citer.value());
        }
        else if (!strip_classname) QMap<QString,QVariant>::insert(citer.key(), citer.value());
        ++citer;
    }

}

void prc_com::append_data_from_other(const QMap<QString, QVariant> &in_map, const QString prepend_classname_)
{
    if (this->threaded) std::lock_guard<std::recursive_mutex> lock(this->mutex);

    QMap<QString, QVariant>::const_iterator citer = in_map.constBegin();         /*!< iterator for string data */
    while (citer != in_map.constEnd()) {
        QString newkey;
        if (prepend_classname_.size()) {
            newkey = prepend_classname_ + citer.key();
            QMap<QString,QVariant>::insert(newkey, citer.value());
        }
        else {
            QMap<QString,QVariant>::insert(citer.key(), citer.value());

        }
        ++citer;
    }

}

QMap<QString, QVariant>::iterator prc_com::create_from(const QString &key, const QVariant &value, const QString &type)
{
    if (!type.compare("string", Qt::CaseInsensitive) || !type.compare("QString", Qt::CaseInsensitive)) {
        return this->insert(key, value.toString());
    }
    if (!type.compare("double", Qt::CaseInsensitive) || !type.compare("float", Qt::CaseInsensitive) ) {

        bool ok (false);
        double val (value.toDouble(&ok));
        if (ok) return this->insert(key, val);
        return this->insert(key, DBL_MAX);
    }


    if (!type.compare("int", Qt::CaseInsensitive) || !type.compare("int32_t", Qt::CaseInsensitive) || !type.compare("qint32", Qt::CaseInsensitive)) {

        bool ok (false);
        qint32 val (value.toInt(&ok));
        if (ok) return this->insert(key, val);
        return this->insert(key, INT32_MAX);
    }

    if (!type.compare("int16", Qt::CaseInsensitive) || !type.compare("int16_t", Qt::CaseInsensitive) || !type.compare("qint16", Qt::CaseInsensitive)) {

        bool ok (false);
        qint16 val (value.toInt(&ok));
        if (ok) return this->insert(key, val);
        return this->insert(key, INT16_MAX);
    }

    if (!type.compare("int8", Qt::CaseInsensitive) || !type.compare("int8_t", Qt::CaseInsensitive) || !type.compare("qint8", Qt::CaseInsensitive)) {

        bool ok (false);
        qint8 val (value.toInt(&ok));
        if (ok) return this->insert(key, val);
        return this->insert(key, INT8_MAX);
    }

    if (!type.compare("uint", Qt::CaseInsensitive) || !type.compare("uint32_t", Qt::CaseInsensitive) || !type.compare("quint32", Qt::CaseInsensitive)) {

        bool ok (false);
        quint32 val (value.toInt(&ok));
        if (ok) return this->insert(key, val);
        return this->insert(key, UINT32_MAX);
    }

    if (!type.compare("uint16", Qt::CaseInsensitive) || !type.compare("uint16_t", Qt::CaseInsensitive) || !type.compare("quint16", Qt::CaseInsensitive)) {

        bool ok (false);
        qint16 val (value.toUInt(&ok));
        if (ok) return this->insert(key, val);
        return this->insert(key, UINT16_MAX);
    }

    if (!type.compare("uint8", Qt::CaseInsensitive) || !type.compare("uint8_t", Qt::CaseInsensitive) || !type.compare("quint8", Qt::CaseInsensitive)) {

        bool ok (false);
        qint8 val (value.toUInt(&ok));
        if (ok) return this->insert(key, val);
        return this->insert(key, UINT8_MAX);
    }


    if (!type.compare("int64", Qt::CaseInsensitive) || !type.compare("int64_t", Qt::CaseInsensitive) || !type.compare("qint64", Qt::CaseInsensitive)) {

        bool ok (false);
        qint64 val (value.toLongLong(&ok));
        if (ok) return this->insert(key, val);
        return this->insert(key, LLONG_MAX);
    }

    if (!type.compare("uint64", Qt::CaseInsensitive) || !type.compare("uint64_t", Qt::CaseInsensitive) || !type.compare("quint64", Qt::CaseInsensitive)) {

        bool ok (false);
        quint64 val (value.toULongLong(&ok));
        if (ok) return this->insert(key, val);
        return this->insert(key, ULLONG_MAX);
    }

    return this->insert(key, value);


}




QMap<QString, QVariant> prc_com::get_data() const
{
    return *this;
}

bool prc_com::set_key_value(const QString &key, const QVariant &qvalue, const bool check_options)
{
    if (this->threaded) std::lock_guard<std::recursive_mutex> lock(this->mutex);
    if (this->contains(key)) {
        if (check_options) {
            /// @todo check min max, list contains
        }
        QMap<QString,QVariant>::insert(key, get_native(qvalue, this->value(key)));
        return true;
    }

    return false;
}

bool prc_com::set_key_size_t(const QString &key, const size_t &qvalue, const bool check_options)
{
    if (this->threaded) std::lock_guard<std::recursive_mutex> lock(this->mutex);

    return this->set_key_value(key, quint64(qvalue), check_options);
}

bool prc_com::set_key_size_t(const QString &key, const QVariant &qvalue, const bool check_options)
{
    if (this->threaded) std::lock_guard<std::recursive_mutex> lock(this->mutex);
    if (this->contains(key)) {
        if (check_options) {
            /// @todo check min max, list contains
        }
        quint64 xvalue = qvalue.toULongLong();
        QMap<QString,QVariant>::insert(key, get_native(xvalue, value(key)));
        return true;
    }

    return false;
}




QMap<QString, QVariant>::iterator prc_com::insert(const QString &key, const QVariant &value)
{
    if (this->threaded) std::lock_guard<std::recursive_mutex> lock(this->mutex);
    return QMap<QString,QVariant>::insert(key, QVariant(value));
}

QMap<QString, QVariant>::iterator prc_com::insert_szt(const QString &key, const size_t &value)
{
    if (this->threaded) std::lock_guard<std::recursive_mutex> lock(this->mutex);
    return QMap<QString,QVariant>::insert(key, QVariant(quint64(value)));
}

QMap<QString, QVariant>::iterator prc_com::insert_szt(const QString &key, const QVariant& value)
{
    if (this->threaded) std::lock_guard<std::recursive_mutex> lock(this->mutex);
    return QMap<QString,QVariant>::insert(key, QVariant(value.toULongLong()));
}

QMap<QString, QVariant>::iterator prc_com::insert_double(const QString &key, const QVariant &value)
{
    if (this->threaded) std::lock_guard<std::recursive_mutex> lock(this->mutex);
    return QMap<QString,QVariant>::insert(key, value.toDouble());
}

QMap<QString, QVariant>::iterator  prc_com::insert_int(const QString &key, const QVariant &value)
{
    if (this->threaded) std::lock_guard<std::recursive_mutex> lock(this->mutex);
    return QMap<QString,QVariant>::insert(key, value.toInt());
}

QMap<QString, QVariant>::iterator  prc_com::insert_uint64(const QString &key, const QVariant &value)
{
    if (this->threaded) std::lock_guard<std::recursive_mutex> lock(this->mutex);
    return QMap<QString,QVariant>::insert(key, value.toULongLong());
}

QMap<QString, QVariant>::iterator  prc_com::insert_int64(const QString &key, const QVariant &value)
{
    if (this->threaded) std::lock_guard<std::recursive_mutex> lock(this->mutex);
    return QMap<QString,QVariant>::insert(key, value.toLongLong());
}



int prc_com::show_my_map(bool show_values, bool prepend_leading_minus) const
{

    if (this->threaded) std::lock_guard<std::recursive_mutex> lock(this->mutex);
    int i = 0;


    QMap<QString, QVariant>::const_iterator citer = this->constBegin();         /*!< iterator for data */
    while (citer != this->constEnd()) {
        if (show_values) {
            if (prepend_leading_minus) {
                qInfo() << QString("-" + citer.key()) << " -> " << citer.value().toString();
            }
            else qInfo() << citer.key() << " -> " << citer.value().toString();
        }
        if (!show_values) {
            if (prepend_leading_minus) {
                qInfo() << QString("-" + citer.key());
            }
            else qInfo() << citer.key();
        }
        ++citer;
        ++i;
    }
    return i;
}



QStringList prc_com::myHashes_toQstringList(bool prepend_leading_minus) const
{
    if (this->threaded) std::lock_guard<std::recursive_mutex> lock(this->mutex);

    QString mykey, myhashes_str;
    QStringList allhashes;

    QMap<QString, QVariant>::const_iterator citer = this->constBegin();         /*!< iterator for data */
    while (citer != this->cend()) {
        if (prepend_leading_minus) mykey = "-" + citer.key();
        else mykey = citer.key();
        myhashes_str.append(mykey);
        myhashes_str.append(" -> ");
        myhashes_str.append(citer->toString());
        allhashes.append(myhashes_str);
        myhashes_str.clear();
        ++citer;
    }

    return allhashes;
}

void prc_com::prc_com_to_classmembers()
{
    // virtual
}

void prc_com::classmembers_to_prc_com()
{
    // virtual
}

QVariant prc_com::value(const QString key) const
{
    return QMap<QString, QVariant>::value(key, "empty");
}

int prc_com::ivalue(const QString key) const
{
    return QMap<QString, QVariant>::value(key, INT_MAX).toInt();
}

double prc_com::dvalue(const QString key) const
{
    return QMap<QString, QVariant>::value(key, DBL_MAX).toDouble();
}

QString prc_com::svalue(const QString key) const
{
    return QMap<QString, QVariant>::value(key, "empty").toString();
}

uint prc_com::uivalue(const QString key) const
{
    return QMap<QString, QVariant>::value(key, UINT_MAX).toUInt();
}

size_t prc_com::sztvalue(const QString key) const
{
    quint64 szt = QMap<QString, QVariant>::value(key, ULLONG_MAX).toULongLong();
    return size_t (szt);
}

qint64 prc_com::qint64value(const QString key) const
{
    qint64 szt = QMap<QString, QVariant>::value(key, LLONG_MAX).toLongLong();
    return  szt;
}

quint64 prc_com::quint64value(const QString key) const
{
    quint64 szt = QMap<QString, QVariant>::value(key, ULLONG_MAX).toULongLong();
    return  szt;
}



QString prc_com::svalue_rformat(const QString key, const uint width, QChar fill) const
{
    if (this->threaded) std::lock_guard<std::recursive_mutex> lock(this->mutex);
    QString result = QMap<QString, QVariant>::value(key, "empty").toString();
    return result.rightJustified(int(width), fill);


}


void prc_com::scan_cmdline(const QStringList &argues, QStringList &files, QStringList &atsfiles, QStringList &measdocs, const bool remove_leading_minus, const bool check_and_replace, const bool auto_remove)
{
    QString key, arg;
    int i, j;
    QMap<QString, QVariant> tmp_map;

    if (check_and_replace) {
        tmp_map = *this;
        clear();
    }

    for (i = 0; i < argues.size(); ++i) {
        arg = argues.at(i);
        if (remove_leading_minus) if (arg.startsWith("-")) arg.remove(0,1);                           // remove the leading minus
        if (i < argues.size()) {
            if (check_and_replace) {
                // does not contain keyword files
                if (tmp_map.contains(arg) && (arg.compare("files") != 0)) {

                   // qDebug() << tmp_map[arg] << "is " << tmp_map[arg].typeName() << arg << argues.at(i+1);
                    if ((tmp_map[arg].metaType().id() == QMetaType::Double)) QMap<QString,QVariant>::insert(arg,argues.at(i+1).toDouble());
                    else if ((tmp_map[arg].metaType().id() == QMetaType::Int)) QMap<QString,QVariant>::insert(arg,argues.at(i+1).toInt());
                    else if ((tmp_map[arg].metaType().id() == QMetaType::UInt)) QMap<QString,QVariant>::insert(arg,argues.at(i+1).toUInt());
                    else if ((tmp_map[arg].metaType().id() == QMetaType::ULongLong)) QMap<QString,QVariant>::insert(arg,argues.at(i+1).toULongLong());
                    else if ((tmp_map[arg].metaType().id() == QMetaType::LongLong)) QMap<QString,QVariant>::insert(arg,argues.at(i+1).toLongLong());
                    else if ((tmp_map[arg].metaType().id() == QMetaType::QString) && !argues.at(i+1).compare("true")) QMap<QString,QVariant>::insert(arg,true);
                    else if ((tmp_map[arg].metaType().id() == QMetaType::QString) && !argues.at(i+1).compare("false")) QMap<QString,QVariant>::insert(arg,false);
                    else QMap<QString,QVariant>::insert(arg,argues.at(i+1));

                }
                else if (arg.compare("files") == 0) {
                    ++i;
                    for (j = i; j < argues.size(); ++j) {
                        files.append(argues.at(j));
                        ++i;
                    }
                }

            }
            // no type and existance check
            else {
                if (arg.compare("files")) {
                    QMap<QString,QVariant>::insert(arg,argues.at(i+1));
                }
                else if (!arg.compare("files")) {
                    ++i;
                    for (j = i; j < argues.size(); ++j) {
                        files.append(argues.at(j));
                        ++i;
                    }
                }
            }
        }

    }


    for ( i = 0; i < argues.size(); ++i) {

        // std xml
        if ( argues.at(i).contains("_R", Qt::CaseInsensitive)  && argues.at(i).endsWith(".xml", Qt::CaseInsensitive)) {
            measdocs.append(argues.at(i));
        }
        // CEA xml
        if ( argues.at(i).contains("_ns_", Qt::CaseInsensitive) &&  argues.at(i).endsWith(".xml", Qt::CaseInsensitive)) {
            measdocs.append(argues.at(i));
        }
        // CEA xml
        if ( argues.at(i).contains("_ca_", Qt::CaseInsensitive) &&  argues.at(i).endsWith(".xml", Qt::CaseInsensitive)) {
            measdocs.append(argues.at(i));
        }
        // new ats files
        if ( argues.at(i).contains("_V", Qt::CaseInsensitive) && argues.at(i).contains("_R", Qt::CaseInsensitive) && argues.at(i).contains("_T", Qt::CaseInsensitive) && argues.at(i).contains(".ats", Qt::CaseInsensitive)) {
            atsfiles.append(argues.at(i));
            //--qDebug() << "got file " << atsfiles[0];
        }
        // CEA ats files
        if ( argues.at(i).contains("_ns_", Qt::CaseInsensitive) && argues.at(i).contains(".ats", Qt::CaseInsensitive)) {
            atsfiles.append(argues.at(i));
            //--qDebug() << "got file " << atsfiles[0];
        }
        // CEA ats files
        if ( argues.at(i).contains("_ca_", Qt::CaseInsensitive) && argues.at(i).contains(".ats", Qt::CaseInsensitive)) {
            atsfiles.append(argues.at(i));
            //--qDebug() << "got file " << atsfiles[0];
        }


    }

    if (auto_remove) {

        QMap<QString, QVariant>::iterator b = begin();

        while (b != end()) {

            //qDebug() << b.key() << b.value();
            if ((b->metaType().id() == QMetaType::Double)) {
                // qDebug() << "type is" << b->type() << b->typeName();
                // qDebug() << b.key() << b.value();
                if ( b.value().toDouble() == DBL_MAX) {
                    b = erase(b);
                }
                else ++b;
            }
            else if ((b->metaType().id() == QMetaType::QString)) {
                // qDebug() << "type is" << b->type() << b->typeName();
                // qDebug() << b.key() << b.value();
                if ( b.value() == "empty") {
                    b = erase(b);
                }
                else ++b;
            }

            else ++b;
        }

    }

    tmp_map.clear();
}


/*
 *
 *  idea for exportin all doubles, ints and so on

if ( !var.isValid() || var.isNull() )
        //return -1;
        Q_ASSERT(0);

    switch ( var.type() )
    {
        case QVariant::Int:
                return QMap( var.toInt() );
            break;
        case QVariant::UInt:
                return QMap( var.toUInt() );
            break;
        case QVariant::Bool:
                return QMap( var.toUInt() );
            break;
        case QVariant::Double:
                return QMap( var.toUInt() );
            break;
        case QVariant::LongLong:
                return QMap( var.toLongLong() );
            break;
        case QVariant::ULongLong:
                return QMap( var.toULongLong() );
            break;
        case QVariant::String:
                return QMap( var.toString() );
            break;
        case QVariant::Char:
                return QMap( var.toChar() );
            break;
        case QVariant::StringList:
                return QMap( var.toString() );
            break;
        case QVariant::ByteArray:
                return QMap( var.toByteArray() );
            break;
        case QVariant::Date:
        case QVariant::Time:
        case QVariant::DateTime:
        case QVariant::Url:
        case QVariant::Locale:
        case QVariant::RegExp:
                return QMap( var.toString() );
            break;
        case QVariant::Map:
        case QVariant::List:
        case QVariant::BitArray:
        case QVariant::Size:
        case QVariant::SizeF:
        case QVariant::Rect:
        case QVariant::LineF:
        case QVariant::Line:
        case QVariant::RectF:
        case QVariant::Point:
        case QVariant::PointF:
            // not supported yet
            break;
        case QVariant::UserType:
        case QVariant::Invalid:
        default:
            return -1;
    }

 */

