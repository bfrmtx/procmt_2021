#include "sensor.h"



sensor::sensor(const int &num) : cable(num) {
    // qDebug() << "init sensor" << nslot;

    // insert public data
    this->data.insert("pos_x1", (double) 0.0);
    this->data.insert("pos_x2", (double) 0.0);
    this->data.insert("pos_y1", (double) 0.0);
    this->data.insert("pos_y2", (double) 0.0);
    this->data.insert("pos_z1", (double) 0.0);
    this->data.insert("pos_z2", (double) 0.0);

    this->data.insert("sensor_type", "ukn");
    this->data.insert("sensor_type_alias", "ukn");

    this->data.insert("sensor_sernum", (int) 0);

    this->data.insert("xmlcal_sensor", "empty");




    this->map_names.insert("North", "pos_x2");
    this->map_names.insert("South", "pos_x1");
    this->map_names.insert("East", "pos_y2");
    this->map_names.insert("West", "pos_y1");



}

sensor &sensor::operator = (const sensor &rhs)

{
    if (&rhs == this) return *this;
    cable::operator = (rhs);
    this->Type = rhs.Type;
    this->Serial = rhs.Serial;
    this->RevMain = rhs.RevMain;
    this->RevSub = rhs.RevMain;
    this->GMS = rhs.GMS;
    this->Name = rhs.Name;
    this->sensors_input_divider = rhs.sensors_input_divider;
    this->sensors = rhs.sensors;
    this->sensor_aliases = rhs.sensor_aliases;
    this->map_names = rhs.map_names;
    return *this;
}

bool sensor::operator == (const sensor &rhs)
{
    return ( (GMS == rhs.GMS) && (RevMain == rhs.RevMain) && (RevSub == rhs.RevSub)
             && (Type == rhs.Type && cable::operator ==(rhs)));
}

QVariant sensor::get(const QString &what) const
{
    // special cases FIRST, then base class

    if (this->map_names.contains(what)) {
        return aduslot::get(this->map_names.value(what));
    }

    if (what.contains("xmlcal_sensor")) {
        return this->data.value("xmlcal_sensor");
    }

    return aduslot::get(what);
}

bool sensor::set(const QString &what, const QVariant &value)
{
    // special cases FIRST, then base class


    // substring
    if (what.contains("pos_") || this->map_names.contains(what)) {

        // make sure that this is not optimized out
        bool ok;
        double dval = value.toDouble(&ok);
        // !value.canConvert<double> does not work
        if (!ok) {
            this->no_set_error = 0;
            dval = 0;
            return false;
        }
        else {
            if (this->map_names.contains(what)) aduslot::set(map_names.value(what), value);
            else aduslot::set(what, value);
            return true;
        }
    }

    if (what.contains("sensor_type")) {

        if (sensor_aliases.contains(value.toString()) || sensors.contains(value.toString())) {
            if ((this->sensors.size() == this->sensor_aliases.size()) && (this->sensors.size() == this->sensors_input_divider.size())) {
                int jj = 0;
                QString test = value.toString();
                for (auto &str : this->sensor_aliases) {
                    if (!QString::compare(str, test, Qt::CaseInsensitive)) {
                        this->Name = this->sensors.at(jj);
                        this->data.insert("sensor_type", this->Name);
                        this->data.insert("sensor_type_alias", value.toString());
                        adbboard::set("input_divider", this->sensors_input_divider.at(jj));
                    }
                    ++jj;
                }
                jj = 0;
                for (auto &str : this->sensors) {
                    if (!QString::compare(str, test, Qt::CaseInsensitive)) {
                        this->Name = this->sensors.at(jj);
                        this->data.insert("sensor_type", this->Name);
                        this->data.insert("sensor_type_alias", this->sensor_aliases.at(jj));
                        adbboard::set("input_divider", this->sensors_input_divider.at(jj));
                    }
                    ++jj;
                }

            }
            else {
                qDebug() << "sensor:: failure in alias table!!!";
                this->Name = "UNKN_E";
                this->data.insert("sensor_type", this->Name);
                this->data.insert("sensor_type_alias", this->Name);
            }
        }
        else {
            this->Name = value.toString();
            this->data.insert("sensor_type", this->Name);
            this->data.insert("sensor_type_alias", this->Name);
            int jj = 0;
            for (auto &str : this->sensors) {
                if (this->Name.contains(str)) {
                    adbboard::set("input_divider", this->sensors_input_divider.at(jj));
                }
                ++jj;
            }
        }
        return true;
    }
    if (what.contains("sensor_sernum")) {
        this->data.insert("sensor_sernum", value.toInt());
        return true;
    }

    if (what.contains("xmlcal_sensor")) {
        this->data.insert("xmlcal_sensor", value.toString());
        return true;
    }

    // nothing -> pipe through
    return cable::set(what, value);
}


void sensor::init_aliases(const QStringList &sensors, const QStringList &sensor_aliases, const QList<int> &sensors_input_divider)
{
    this->sensors.append(sensors);
    this->sensor_aliases.append(sensor_aliases);
    this->sensors_input_divider.append(sensors_input_divider);
}

