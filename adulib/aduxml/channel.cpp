#include "channel.h"

channel::channel(const int &num, QObject *parent) : QObject(parent), sensor(num) {
    qDebug() << "init channel, aduslot: " << this->prvdata.value("slot").toInt();

    this->all_classes << "adbboard" << "cable" << "sensor";

    this->e_channel_types << "Ex" << "Ey" << "Ez"
                          << "Ex2" << "Ex3"
                          << "Ey2" << "Ey3";

    this->h_channel_types << "Hx" << "Hy" << "Hz"
                          << "Hx2" << "Hy2" << "Hz2"
                          << "Hx3" << "Hy3" << "Hz3";

    this->general_channel_types << "Temp";

    this->all_channel_types << this->e_channel_types << this->h_channel_types;
    this->data.insert("channel_type", QString("empty"));
    this->data.insert("ats_data_file", QString("empty"));
    this->data.insert("ts_lsb", (double) 0.);
    this->data.insert("num_samples", (quint64) 0);
    this->data.insert("xmlcal_channel", "empty");





}

channel::channel(const channel &rhs) : QObject(rhs.parent()),  sensor(rhs.prvdata.value("slot").toInt())
{
    *this = rhs;

}

channel &channel::operator = (const channel &rhs)
{
    if (&rhs == this) return *this;

    sensor::operator = (rhs);
    this->all_classes = rhs.all_classes;
    this->e_channel_types = rhs.e_channel_types;
    this->h_channel_types = rhs.h_channel_types;
    this->all_channel_types = rhs.all_channel_types;
    // selftest_result is already in slot

    // reset error state - copy error state is not desired
    this->no_set_error = 1;

    return *this;
}

bool channel::operator == (const channel &rhs)
{
    return (sensor::operator ==(rhs));
}

QVariant channel::get(const QString &what) const
{
    // special cases FIRST, then base class

    if (what.contains("is_electric")) {
        if (this->e_channel_types.contains(this->data.value("channel_type").toString())) {
            return true;
        }
        else return false;
    }
    if (what.contains("is_magnetic")) {
        if (this->h_channel_types.contains(this->data.value("channel_type").toString())) {
            return true;
        }
        else return false;
    }
    if (what.contains("is_valid_channel_type")) {
        if (this->all_channel_types.contains(this->data.value("channel_type").toString())) {
            return true;
        }
        else return false;
    }
    if (what.contains("xmlcal_channel")) {
        return this->data.value("xmlcal_channel");

    }

    if (this->selftest_result.contains(what)) {
        return this->selftest_result.value(what);
    }

    return this->aduslot::get(what);
}

bool channel::set(const QString &what, const QVariant &value)
{
    // special cases FIRST, then parent class and so on
    if (!what.compare("channel_type")) {
        if (all_channel_types.contains(value.toString())) {
            this->data.insert("channel_type", value.toString());
        }
        else this->no_set_error = 2;
        return true;
    }

    if (!what.compare("xmlcal_channel")) {
        this->data.insert("xmlcal_channel", value.toString());
        return true;
    }

    return sensor::set(what, value);

}

QVariant channel::get_data_from_index(const int &idx) const
{
    int i = 0;
    QMapIterator<QString, QVariant> dat(this->data);
    while (dat.hasNext()) {
        dat.next();
        // qDebug() << dat.key() << ": " << dat.value() << i;
        if (i == idx) return dat.value();
        ++i;
    }
    return QVariant();
}

QString channel::get_name_from_index(const int &idx) const
{
    int i = 0;
    QMapIterator<QString, QVariant> dat(this->data);
    while (dat.hasNext()) {
        dat.next();
        // qDebug() << dat.key() << ": " << dat.value() << i;
        if (i == idx) return dat.key();
        ++i;
    }
    return QString();
}

void channel::get_name_and_data_from_index(const int &idx, QVariant &name, QVariant &value) const
{
    int i = 0;
    QMapIterator<QString, QVariant> dat(this->data);
    while (dat.hasNext()) {
        dat.next();
        // qDebug() << dat.key() << ": " << dat.value() << i;
        if (i == idx) {
            value = dat.value();
            name = QVariant(dat.key());
        }
        ++i;
    }
}

QMap<QString, QVariant> channel::get_data() const
{
    return this->data;
}

void channel::set_data(const QMap<QString, QVariant> &setdata)
{
    QMapIterator<QString,  QVariant> dat(setdata);
    while (dat.hasNext()) {
        dat.next();
        this->set(dat.key(), dat.value());
    }
}



void channel::set_Type(const QString &classname, const int &value)
{

    if (!QString::compare(classname, "adbboard")) {
        adbboard::Type = value;
    }
    else if (!QString::compare(classname, "cable")) {
        cable::Type = value;
    }
    else if (!QString::compare(classname, "sensor")) {
        sensor::Type = value;
    }
}

void channel::set_Serial(const QString &classname, const int &value)
{
    if (!QString::compare(classname, "adbboard")) {
        adbboard::Serial = value;
    }
    else if (!QString::compare(classname, "cable")) {
        cable::Serial = value;
    }
    else if (!QString::compare(classname, "sensor")) {
        sensor::Serial = value;
        // input is switched in sensor class
        this->set("sensor_sernum", value);
    }

}

void channel::set_RevMain(const QString &classname, const int &value)
{
    if (!QString::compare(classname, "adbboard")) {
        adbboard::RevMain = value;
    }
    else if (!QString::compare(classname, "cable")) {
        cable::RevMain = value;
    }
    else if (!QString::compare(classname, "sensor")) {
        sensor::RevMain = value;
    }
}

void channel::set_RevSub(const QString &classname, const int &value)
{
    if (!QString::compare(classname, "adbboard")) {
        adbboard::RevSub = value;
    }
    else if (!QString::compare(classname, "cable")) {
        cable::RevSub = value;
    }
    else if (!QString::compare(classname, "sensor")) {
        sensor::RevSub = value;
    }
}

void channel::set_GMS(const QString &classname, const int &value)
{

    if (!QString::compare(classname, "adbboard")) {
        adbboard::GMS = value;
    }
    else if (!QString::compare(classname, "cable")) {
        cable::GMS = value;
    }
    else if (!QString::compare(classname, "sensor")) {
        sensor::GMS = value;
    }

    if (value == 8) {
        this->selftest_result.insert("AdbError", (int) 0);
        this->selftest_result.insert("Atten_LF_LP_4Hz", double (0.0));
        this->selftest_result.insert("Atten_HF_HP_500Hz", double (0.0));
        this->selftest_result.insert("DCOffset", double (0.0));
        this->selftest_result.insert("ExtOffsetComp", double (0.0));
        this->selftest_result.insert("GainAmpl", double (0.0));
        this->selftest_result.insert("GainCheckHF16", double (0.0));
        this->selftest_result.insert("GainCheckHF4", double (0.0));
        this->selftest_result.insert("GainCheckHF8", double (0.0));
        this->selftest_result.insert("GainCheckLF16", double (0.0));
        this->selftest_result.insert("GainCheckLF4", double (0.0));
        this->selftest_result.insert("GainCheckLF8", double (0.0));
        this->selftest_result.insert("GainCorr", double (0.0));
        this->selftest_result.insert("HF_LSB", double (0.0));
        this->selftest_result.insert("InputDivCheckOff", double (0.0));
        this->selftest_result.insert("InputDivCheckOn", double (0.0));
        this->selftest_result.insert("IntOffsetCorr_1_HF", double (0.0));
        this->selftest_result.insert("IntOffsetCorr_1_LF", double (0.0));
        this->selftest_result.insert("IntOffsetCorr_2_HF", double (0.0));
        this->selftest_result.insert("IntOffsetCorr_2_LF", double (0.0));
        this->selftest_result.insert("LF_LSB", double (0.0));
        this->selftest_result.insert("MaxAmplitude", double (0.0));
        this->selftest_result.insert("Message", QString(""));//double (0.0));
        this->selftest_result.insert("NoiseCheckHF", double (0.0));
        this->selftest_result.insert("NoiseCheckLF", double (0.0));
        this->selftest_result.insert("RFCheckStrong", double (0.0));
        this->selftest_result.insert("RFCheckWeak", double (0.0));
        this->selftest_result.insert("Resistivity", double (0.0));
        this->selftest_result.insert("Severity", QString(""));
    }
    else if (value == 7) {
        this->selftest_result.insert("AdbError", (int) 0);
        this->selftest_result.insert("Atten_HF_HP_1Hz", (double) 0);
        this->selftest_result.insert("DCOffset", (double) 0);
        this->selftest_result.insert("Atten_LF_LP_4Hz", (double) 0.);
        this->selftest_result.insert("Atten_MF_HP_500Hz", (double) 0.);
        this->selftest_result.insert("ExtDCOffsetComp", (double) 0.);
        this->selftest_result.insert("ExtOffsetComp", (double) 0.);
        this->selftest_result.insert("GainAmpl", (double) 0.);
        this->selftest_result.insert("GainCorr", (double) 0.);
        this->selftest_result.insert("GainTest_16_4_HF", (double) 0.);
        this->selftest_result.insert("GainTest_16_4_LF", (double) 0.);
        this->selftest_result.insert("GainTest_1_1", (double) 0.);
        this->selftest_result.insert("GainTest_1_64", (double) 0.);
        this->selftest_result.insert("GainTest_4_16_HF", (double) 0.);
        this->selftest_result.insert("GainTest_4_16_LF", (double) 0.);
        this->selftest_result.insert("GainTest_64_1", (double) 0.);
        this->selftest_result.insert("GainTest_8_8", (double) 0.);
        this->selftest_result.insert("HF_LSB", (double) 0.);
        this->selftest_result.insert("IntOffsetCorr_1_HF", (double) 0.);
        this->selftest_result.insert("IntOffsetCorr_1_LF", (double) 0.);
        this->selftest_result.insert("IntOffsetCorr_2_HF", (double) 0.);
        this->selftest_result.insert("IntOffsetCorr_2_LF", (double) 0.);
        this->selftest_result.insert("LF_LSB", (double) 0.);
        this->selftest_result.insert("MaxAmplitude", (double) 0.);
        this->selftest_result.insert("Resistivity", (double) 0.);
        this->selftest_result.insert("InitError", (int) 0);
    }
}

void channel::set_Name(const QString classname, const QString value)
{

    if (!QString::compare(classname, "adbboard")) {
        adbboard::Name = value;
    }
    else if (!QString::compare(classname, "cable")) {
        cable::Name = value;
    }
    else if (!QString::compare(classname, "sensor")) {
        sensor::Name = value;
        // input is switched in sensor class
        this->set("sensor_type", value);
    }
}



int channel::get_Type(const QString &classname) const
{

    if (!QString::compare(classname, "adbboard")) {
        return adbboard::Type;
    }
    else if (!QString::compare(classname, "cable")) {
        return cable::Type;
    }
    else if (!QString::compare(classname, "sensor")) {
        return sensor::Type;
    }
    else return 0;
}

int channel::get_Serial(const QString &classname) const
{

    if (!QString::compare(classname, "adbboard")) {
        return adbboard::Serial;
    }
    else if (!QString::compare(classname, "cable")) {
        return cable::Serial;
    }
    else if (!QString::compare(classname, "sensor")) {
        return sensor::Serial;
    }
    else return 0;

}

int channel::get_RevMain(const QString &classname) const
{
    if (!QString::compare(classname, "adbboard")) {
        return adbboard::RevMain;
    }
    else if (!QString::compare(classname, "cable")) {
        return cable::RevMain;
    }
    else if (!QString::compare(classname, "sensor")) {
        return sensor::RevMain;
    }
    else return 0;
}

int channel::get_RevSub(const QString &classname) const
{
    if (!QString::compare(classname, "adbboard")) {
        return adbboard::RevSub;
    }
    else if (!QString::compare(classname, "cable")) {
        return cable::RevSub;
    }
    else if (!QString::compare(classname, "sensor")) {
        return sensor::RevSub;
    }
    else return 0;
}

int channel::get_GMS(const QString &classname) const
{
    if (!QString::compare(classname, "adbboard")) {
        return adbboard::GMS;
    }
    else if (!QString::compare(classname, "cable")) {
        return cable::GMS;
    }
    else if (!QString::compare(classname, "sensor")) {
        return sensor::GMS;
    }
    else return 0;
}

int channel::copy_item_xmlvals(const QString classname, const channel &chan)
{

    if (!QString::compare(classname, "adbboard")) {
        this->set_Type("adbboard", chan.get_Type("adbboard"));
        this->set_Serial("adbboard", chan.get_Serial("adbboard"));
        this->set_RevMain("adbboard", chan.get_RevMain("adbboard"));
        this->set_RevSub("adbboard", chan.get_RevSub("adbboard"));
        this->set_GMS("adbboard", chan.get_GMS("adbboard"));
        this->set_Name("adbboard", chan.get_Name("adbboard"));
        return 1;
    }
    else if (!QString::compare(classname, "cable")) {
        this->set_Type("cable", chan.get_Type("cable"));
        this->set_Serial("cable", chan.get_Serial("cable"));
        this->set_RevMain("cable", chan.get_RevMain("cable"));
        this->set_RevSub("cable", chan.get_RevSub("cable"));
        this->set_GMS("cable", chan.get_GMS("cable"));
        this->set_Name("cable", chan.get_Name("cable"));
        return 1;
    }
    else if (!QString::compare(classname, "sensor")) {
        this->set_Type("sensor", chan.get_Type("sensor"));
        this->set_Serial("sensor", chan.get_Serial("sensor"));
        this->set_RevMain("sensor", chan.get_RevMain("sensor"));
        this->set_RevSub("sensor", chan.get_RevSub("sensor"));
        this->set_GMS("sensor", chan.get_GMS("sensor"));
        this->set_Name("sensor", chan.get_Name("sensor"));
        return 1;
    }
    else return 0;
}

QString channel::get_Name(const QString &classname) const
{

    if (!QString::compare(classname, "adbboard")) {
        return adbboard::Name;
    }
    else if (!QString::compare(classname, "cable")) {
        return cable::Name;
    }
    else if (!QString::compare(classname, "sensor")) {
        return sensor::Name;
    }
    else return QString("nn");
}

bool channel::has_board(const int &type, const int &revmain, const int &revsub, const int &gms, QString &classname)
{
    int mytype, myrevmain, myrevsub, mygms;
    for (auto &str : all_classes) {
        // qDebug() << "checking" << str;
        mytype = this->get_Type(str);
        myrevmain = this->get_RevMain(str);
        myrevsub = this->get_RevSub(str);
        mygms = this->get_GMS(str);

        if ((mygms == gms) && (mytype == type) && (myrevmain == revmain) && (myrevsub == revsub)) {
            qDebug() << "have board " << str;
            classname = str;
            return true;
        }


    }

    return false;
}

void channel::append_sample_freq(const double &freq)
{
    adbboard::SampleFreqs.push_back(freq);
}

void channel::append_filter(const QString &filter)
{
    adbboard::filters << filter.simplified();
}

void channel::append_filter_lf(const QString &filter)
{
    adbboard::filters_lf << filter.simplified();
}

void channel::append_filter_hf(const QString &filter)
{
    adbboard::filters_hf << filter.simplified();
}

void channel::append_dac(const double &dac)
{
    adbboard::DAC.push_back(dac);
}

void channel::append_gain_stage1(const double &gain)
{
    adbboard::GainsStage1.push_back(gain);
}

void channel::append_gain_stage2(const double &gain)
{
    adbboard::GainsStage2.push_back(gain);
}

void channel::replace_UNKN_E()
{
    int i = this->get_slot();
    if (i == 0 || i == 5 || i == 1 || i == 6) {
        if ( !QString::compare(this->get_Name("sensor"), "UNKN_E")) this->set_Name("sensor", "EFP-06");
    }
    else {
        if ( !QString::compare(this->get_Name("sensor"), "UNKN_E")) this->set_Name("sensor", "MFS-06");

    }
}

void channel::set_dac_steps(const double &stepsizemV)
{
    if (adbboard::DAC.size()) {
        double dmin;
        auto result = std::minmax_element (adbboard::DAC.begin(), adbboard::DAC.end() );

        std::vector<double> steps;
        for (dmin = *result.first; dmin <= *result.second; dmin += stepsizemV) {
            steps.push_back(dmin);

        }
        adbboard::DAC = steps;
    }

}

std::vector<double> channel::get_avail_board_frequencies(const bool &ignore_off) const
{
    if (ignore_off) return adbboard::SampleFreqs;
    if (this->is_active()) return adbboard::SampleFreqs;
    else return std::vector<double>();
}

QStringList channel::get_avail_channel_types() const
{
    return this->all_channel_types;
}

QMap<QString, QVariant> channel::fetch_selftest_part(const QMap<QString, QVariant> &xmlpart)
{

    copy_native(this->selftest_result, xmlpart);


    return this->selftest_result;
}

void channel::slot_fetch_atswriter(const QMap<QString, QVariant> &xmlpart, const int &id_slot)
{
    // here we could overwrite the sensors .... but we don't want that!
    // we rely on the auto detection - otherwise the ADU can start mis configured
    if (id_slot != this->get_slot()) return;
    if (xmlpart.contains("pos_x1")) this->set("pos_x1", xmlpart.value("pos_x1"));
    if (xmlpart.contains("pos_x2")) this->set("pos_x2", xmlpart.value("pos_x2"));
    if (xmlpart.contains("pos_y1")) this->set("pos_y1", xmlpart.value("pos_y1"));
    if (xmlpart.contains("pos_y2")) this->set("pos_y2", xmlpart.value("pos_y2"));
    if (xmlpart.contains("pos_z1")) this->set("pos_z1", xmlpart.value("pos_z1"));
    if (xmlpart.contains("pos_z2")) this->set("pos_z2", xmlpart.value("pos_z2"));

    if (xmlpart.contains("ats_data_file")) this->set("ats_data_file", xmlpart.value("ats_data_file"));
    if (xmlpart.contains("ts_lsb")) this->set("ts_lsb", xmlpart.value("ts_lsb"));
    if (xmlpart.contains("num_samples")) this->set("num_samples", xmlpart.value("num_samples"));


    //return this->get_slot();
}

void channel::slot_adbboard_autogains(const QMap<QString, QVariant> &xmlpart, const int &id_slot)
{
    if (id_slot != this->get_slot()) return;

    adbboard::auto_filter_type_main.append(xmlpart.value("filter_type_main").toString().simplified());
    adbboard::auto_input.append(xmlpart.value("input").toInt());
    adbboard::auto_input_circuitry.append(xmlpart.value("input_circuitry").toString().simplified());
    adbboard::auto_gain_stage1.append(xmlpart.value("gain_stage1").toDouble());
    adbboard::auto_gain_stage2.append(xmlpart.value("gain_stage2").toDouble());
    adbboard::auto_filter_type.append(xmlpart.value("filter_type").toString().simplified());
    adbboard::auto_dac_val.append(xmlpart.value("dac_val").toDouble());
    adbboard::auto_dac_on.append(xmlpart.value("dac_on").toInt());


}

void channel::slot_act_gains(const QMap<QString, QVariant> &xmlpart, const int &id_slot)
{
    if (id_slot != this->get_slot()) return;

    // HF or LF input 0 or 1
    if ( (this->input_circuitry == xmlpart.value("input_circuitry").toString().simplified()) && (this->get("input").toInt() == xmlpart.value("input").toInt()) ) {


        // check the LF4 or HF 500 filter
        //  <Settings filter_type="LF_LP_4HZ" input="0" input_circuitry="LF"> filter_type main is LF_LP_4HZ
        // <Settings filter_type="" input="0" input_circuitry="LF"> no filter_type_main
        if (this->get("filter_type_main").toString().simplified() == xmlpart.value("filter_type_main").toString().simplified() ) {
            // if LF check the radio filter
            // it is ok if one channel has LF_RF_1 and the other one has LF_RF_2 , yes!, independent

            this->set("filter_type", xmlpart.value("filter_type").toString().simplified());
            this->set("gain_stage1", xmlpart.value("gain_stage1").toDouble());
            this->set("gain_stage2", xmlpart.value("gain_stage2").toDouble());
            this->set("dac_val", xmlpart.value("dac_val").toDouble());
            this->set("dac_on", xmlpart.value("dac_on").toInt());


        }
    }
}

QMap<QString, QVariant> channel::get_selftest_template() const
{
    return this->selftest_result;
}

QMap<QString, QVariant> channel::get_atswriter_template() const


{
    QMap<QString, QVariant> xdata;
    xdata.insert("pos_x1", (double) 0.0);
    xdata.insert("pos_x2", (double) 0.0);
    xdata.insert("pos_y1", (double) 0.0);
    xdata.insert("pos_y2", (double) 0.0);
    xdata.insert("pos_z1", (double) 0.0);
    xdata.insert("pos_z2", (double) 0.0);

    xdata.insert("ats_data_file", QString("empty"));
    xdata.insert("ts_lsb", (double) 0.0);
    xdata.insert("num_samples", (quint64) 0);
    return xdata;
}

