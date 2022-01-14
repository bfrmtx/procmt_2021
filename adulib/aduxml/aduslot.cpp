#include "aduslot.h"


aduslot::aduslot(const int &num)  {

    this->data.insert("sample_freq", (double) 0);                //!< set the status to false
    this->prvdata.insert("slot",  num);                          //!< handled inside
    this->prvdata.insert("active",  (bool) false);               //!< use this board?, handled inside
    this->data.insert("read_hwcfg_db_jobfile", adudefs::adulib_source::unknown);      //!< 0 unset,  1 if from HWConfig, 2 from ADU Database, 3 job file , 4 measdoc from disk (finished job)
}


aduslot &aduslot::operator = (const aduslot &rhs)

{
    if (&rhs == this) return *this;
    //data.insert("slot", rhs.data.value("slot"));
    this->data = rhs.data;
    this->prvdata = rhs.prvdata;
    this->selftest_result = rhs.selftest_result;
    return *this;
}

bool aduslot::operator == (const aduslot &rhs)
{
    return ( (this->prvdata.value("slot") == rhs.prvdata.value("slot")) );
}


QVariant aduslot::get(const QString &what) const
{
    if (data.contains(what)) {
        return this->data.value(what);
    }

    if (prvdata.contains(what)) {
        return this->prvdata.value(what);
    }

    return QVariant();
}

QVariant aduslot::get_selftest(const QString &what) const
{
    if (selftest_result.contains(what)) {
        return this->selftest_result.value(what);
    }
    return QVariant();
}

bool aduslot::set(const QString &what, const QVariant &value)
{
    if (!QString::compare(what, "active")) {
        this->prvdata.insert("active", value.toBool());
    }
    if (data.contains(what)) {
        this->data.insert(what, get_native(value, data.value(what)));
        this->no_set_error = 1;
        return true;
    }
    else {
        this->no_set_error = 0;
    }
    return false;
}

QVariant aduslot::prvget(const QString &what) const
{
    if (this->prvdata.contains(what)) {
        return this->prvdata.value(what);
    }
    return QVariant("aduslot::prvget -> no match for: " + what);
}

QVariant aduslot::prvset(const QString &what, const QVariant &value)
{
    if (prvdata.contains(what)) {
        this->prvdata.insert(what, value);
        return this->prvdata.value(what);
    }
    return QVariant("aduslot::prvset -> no match for: " + what);
}


bool aduslot::is_active() const
{
    return this->prvdata.value("active").toBool();
}

int aduslot::get_slot() const
{
    return this->prvdata.value("slot").toInt();
}

bool aduslot::append_xml_data(tinyxml2::XMLDocument &xmldoc, tinyxml2::XMLElement *jbchannel, tinyxml2::XMLElement *jbdata, const QString &what)
{
    const char *str;

    if (this->prvdata.contains(what)) {
        jbdata = xmldoc.NewElement(what.toStdString().c_str());
        str = prvdata.value(what).toString().toStdString().c_str();

    }
    else if (this->data.contains(what)) {
        jbdata = xmldoc.NewElement(what.toStdString().c_str());
        str = (data.value(what).toString().toStdString().c_str());
    }
    else return false;
    if (!jbdata) return false;
    jbdata->SetText(str);
    jbchannel->InsertEndChild(jbdata);

    return true;

}

int aduslot::get_data_size() const
{
    return this->data.size();
}

int aduslot::has_no_set_error()
{
    int tmperr = this->no_set_error;
    this->no_set_error = 1;
    return tmperr;
}



// ***********************************************************************************



anyboard::anyboard()
{
    qDebug() << "init";
}

anyboard::anyboard(const anyboard &rhs)
{
    *this = rhs;
}

anyboard &anyboard::operator = (const anyboard &rhs)
{

    if (&rhs == this) return *this;
    this->Type = rhs.Type;
    this->Serial = rhs.Serial;
    this->RevMain = rhs.RevMain;
    this->RevSub = rhs.RevMain;
    this->GMS = rhs.GMS;
    this->Name = rhs.Name;

    return *this;

}

