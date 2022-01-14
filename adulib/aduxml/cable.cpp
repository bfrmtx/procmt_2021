#include "cable.h"


cable::cable(const int &num) :  adbboard(num) {
    // qDebug() << "init cable" << nslot;
    this->data.insert("length", (double) 20.0);
}

cable &cable::operator = (const cable &rhs)
{
    if (&rhs == this) return *this;
    adbboard::operator = (rhs);
    this->Type = rhs.Type;
    this->Serial = rhs.Serial;
    this->RevMain = rhs.RevMain;
    this->RevSub = rhs.RevMain;
    this->GMS = rhs.GMS;
    this->Name = rhs.Name;
    return *this;
}

bool cable::operator == (const cable &rhs)
{
    return ( (GMS == rhs.GMS) && (RevMain == rhs.RevMain) && (RevSub == rhs.RevSub)
             && (Type == rhs.Type && adbboard::operator ==(rhs)));
}

QVariant cable::get(const QString &what) const
{
    // special cases FIRST, then base class
    return aduslot::get(what);
}


bool cable::set(const QString &what, const QVariant &value)
{
    // special cases FIRST, then base class
    return adbboard::set(what,value);
}


