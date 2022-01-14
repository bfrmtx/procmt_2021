#include "gps.h"

GPS::GPS(QObject *parent)   :  QObject(parent)
{

    this->measdoc = std::make_unique<measdocxml>(this);  // measdoc has all xml data

    connect(this->measdoc.get(), &measdocxml::signal_gps_status_qmap_created, this, &GPS::slot_gps_status_qmap_created);

    this->measdoc->create_gps_status(true); // get it by connections

}

GPS::~GPS()
{

}

bool GPS::set(const QString &what, const QVariant &value)
{
    if (data.contains(what)) {
        this->data.insert(what, get_native(value, data.value(what)));
        this->is_set = true;
        return true;
    }

    return false;
}

QVariant GPS::get(const QString &what) const
{

    return QVariant();
}

bool GPS::contains(const QString &what)
{
    return this->data.contains(what);
}

QString GPS::getXmlStr() const
{
    tinyxmlwriter tix;

    tix.clear(false);
    tix.push("GPS");


    auto diter = this->data.constBegin();
    while (diter != this->data.constEnd()) {
        tix.element(diter.key(), diter.value().toString());
        ++diter;
    }
    tix.pop("GPS");

    return tix.getXmlStr();
}

bool GPS::is_active() const
{
    return is_set;
}

void GPS::slot_gps_status_qmap_created(const QMap<QString, QVariant> &map)
{
    this->data = map;
}
