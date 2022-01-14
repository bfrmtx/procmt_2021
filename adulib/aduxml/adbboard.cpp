#include "adbboard.h"

adbboard::adbboard(const int &num) : aduslot(num) {
    // qDebug() << "init board" << nslot;

    // insert all public vars here; name same / similar as in XML

    this->data.insert("sample_freq", (double) 0.0);

    // global config section w / wo private data
    this->data.insert("gain_stage1", (double) 4.0);
    this->data.insert("gain_stage2", (double) 1.0);
    this->data.insert("ext_gain", (double) 1.0);
    this->data.insert("filter_type", QString(""));
    this->data.insert("dac_val", (double) 0.0);

    // only for testing / system check, not for std jobs
    this->data.insert("calon", (int) 0);
    this->data.insert("atton", (int) 0);
    this->data.insert("calref", (int) 0);
    this->data.insert("calint", (int) 0);
    this->data.insert("short_circuit", (int) 0);
    this->data.insert("calfreq", (double) 1.0);

    // control HF High Pass 500Hz or 1Hz and 4Hz low Pass for LF
    // don't use empty here - it must be one of the alreay defined types
    this->data.insert("filter_type_main", (QString) "");
    this->data.insert("autogain", (int) 1);



    // never usedthis->data.insert("decimation", (int) 0);       // never used
    // never usedthis->data.insert("flush_fill", (int) 0);       // never used
    // never usedthis->data.insert("ovl_fill", (int) 0);         // never used
    // never usedthis->data.insert("start_stop_fill", (int) 0);  // never used




    // ADB board sets the chopper pin
    this->prvdata.insert("hchopper",(int) 0);
    this->prvdata.insert("echopper", (int) 0);
    this->prvdata.insert("dac_on", (int) 0);

    // ADU08 0 M mode, 1 Z mode
    // ADU07 small / big connectors
    this->prvdata.insert("input", (int) 0);
    this->prvdata.insert("input_divider", (int) 0); // has to be set

    this->prvdata.insert("buffer", (int) 0);  // set by system
    this->prvdata.insert("start_stop_mode", (int) 0); // set by system




    // don make the mistake in insert data instead of prvdata - that mixes up the tableview!

}

adbboard &adbboard::operator = (const adbboard &rhs)
{
    if (&rhs == this) return *this;

    aduslot::operator = (rhs);
    this->Type = rhs.Type;
    this->Serial = rhs.Serial;
    this->RevMain = rhs.RevMain;
    this->RevSub = rhs.RevMain;
    this->GMS = rhs.GMS;
    this->Name = rhs.Name;

    this->SampleFreqs = rhs.SampleFreqs;
    this->GainsStage1 = rhs.GainsStage1;
    this->GainsStage2 = rhs.GainsStage1;
    this->DAC = rhs.DAC;
    this->filters = rhs.filters;
    this->filters_hf = rhs.filters_hf;
    this->filters_lf = rhs.filters_lf;
    this->input_circuitry = rhs.input_circuitry;

    // values from auto gain settings
    this->auto_filter_type_main = rhs.auto_filter_type_main;
    this->auto_input = rhs.auto_input;
    this->auto_input_circuitry = rhs.auto_input_circuitry;
    this->auto_gain_stage1 = rhs.auto_gain_stage1;
    this->auto_gain_stage2 = rhs.auto_gain_stage2;
    this->auto_filter_type = rhs.auto_filter_type;
    this->auto_dac_val = rhs.auto_dac_val;
    this->auto_dac_on = rhs.auto_dac_on;







    return *this;
}

bool adbboard::operator == (const adbboard &rhs)
{
    return ( (GMS == rhs.GMS) && (RevMain == rhs.RevMain) && (RevSub == rhs.RevSub)
             && (Type == rhs.Type) && aduslot::operator ==(rhs));
}


QVariant adbboard::get(const QString &what) const
{
    // special cases FIRST, then base class
    return aduslot::get(what);
}

bool adbboard::set(const QString &what, const QVariant &value)
{
    // special cases FIRST, then base class

    if (!QString::compare(what, "filter_type_main")) {

        if (this->filters.contains(value.toString())) {
            this->data.insert("filter_type_main", value.toString());
        }
        this->no_set_error = 0;
        this->set("sample_freq", this->data.value("sample_freq"));
    }

    // Other base classes can set the frequency again ...
    else if (!QString::compare(what, "sample_freq")) {

        bool ok;

        //qDebug() << data.value("sample_freq").typeName() << data.value("sample_freq").type();
        double freq = value.toDouble(&ok);
        if (!ok) {
            freq = 0;
            this->no_set_error = 2;
        }
        if (std::find(SampleFreqs.begin(), SampleFreqs.end(), freq) != SampleFreqs.end()) {

            //  right - if you don't care sample frequency will be a string

            this->data.insert("sample_freq", get_native(value, data.value("sample_freq")));
            this->prvdata.insert("active", true);

            // debug only - will be removed
            this->data.insert("active", true);

            // ADU-08e
            if (this->data.value("sample_freq").toDouble() > 4097.) {
                this->input_circuitry = "HF";
                if (this->data.value("autogain").toInt() == 1) {
                    QString tmp_main_filter = "";
                    if (this->filters_hf.contains(this->data.value("filter_type_main").toString())) {
                        tmp_main_filter = this->data.value("filter_type_main").toString();
                    }
                    this->traverse(this->data.value("input").toInt(), this->input_circuitry, tmp_main_filter);
                }
            }
            else {
                this->input_circuitry = "LF";
                if (this->data.value("autogain").toInt() == 1) {
                    QString tmp_main_filter = "";
                    if (this->filters_lf.contains(this->data.value("filter_type_main").toString())) {
                        tmp_main_filter = this->data.value("filter_type_main").toString();
                    }

                    this->traverse(this->data.value("input").toInt(), this->input_circuitry, tmp_main_filter);
                }
            }

        }
        else {
            //    qDebug() << "adb board set sampling freq NOT ok";
            this->data.insert("sample_freq", (double) 0.0);
            this->prvdata.insert("active", false);
            this->input_circuitry = "";

            //!< @todo remove!
            // debug only - will be removed
            this->data.insert("active", false);
        }
        // now set the chopper pin

        // 512 Hz shall be chopper on; 1024 and 2048
        // would deliver better data @256 Hz (8 times less noise)
        // therefore the user shall record 4096/2048/1024 chopper off
        // and do not overlap 4096 / 4096 / 4x to much with 512 Hz data
        if (this->data.value("sample_freq").toDouble() < 1024) {
            //if (this->is_active()) qDebug() << "chopper on";
            if (this->is_active())  this->prvdata.insert("hchopper", 1);

        }
        else {
            //if (this->is_active()) qDebug() << "chopper off";
            if (this->is_active())  this->prvdata.insert("hchopper", 0);
        }

        // it should be always false - use a fallback in case
        // this is done later with min freq
        this->no_set_error = 1;
        return true;
    }

    // dac_val may receive arbitrary values; switch back into the limits
    else if (!QString::compare(what, "dac_val")) {

        bool ok;
        double ddac = value.toDouble(&ok);
        if (!ok) {
            this->data.insert("dac_val", (double) 0.0);
            this->prvdata.insert("dac_on", (int) 0);
        }

        auto result = std::minmax_element (adbboard::DAC.begin(), adbboard::DAC.end() );

        // fallback is always 0
        if (ddac  < *result.first || ddac > *result.second) {
            this->data.insert("dac_val", (double) 0.0);
            this->prvdata.insert("dac_on", (int) 0);
        }
        else {
            auto newdac = std::upper_bound(adbboard::DAC.begin(), adbboard::DAC.end(), ddac);

            // Unlike upper_bound, the value pointed by the iterator returned by this function may also be equivalent to val, and not only greater.
            // if dac is positive take the next SMALLER
            if ( (*newdac) > 0 ) --newdac;
            // avoid jitter
            if ( ( std::fabs(*newdac) < 5 ) ) {
                this->data.insert("dac_val", (double) 0.0);
                this->prvdata.insert("dac_on", (int) 0);
            }
            // here we finally set in case
            else {
                this->data.insert("dac_val", (double) *newdac);
                this->prvdata.insert("dac_on", (int) 1);
            }
        }

        // we should have found a setting
        this->no_set_error = 1;
        return true;
    }

    else if (!QString::compare(what, "gain_stage1")) {

        bool ok;
        double newgain = value.toDouble(&ok);
        if (!ok) this->no_set_error = 0;
        else if (std::find(GainsStage1.begin(), GainsStage1.end(), newgain) != GainsStage1.end()) {
            this->data.insert("gain_stage1", get_native(value, data.value("gain_stage1")));
            this->no_set_error = 1;
        }
        else this->no_set_error = 0;
        // old value is not changed
        // AND: we can not forsee the correct gain - so no fallback here

        return false;
    }
    else if (!QString::compare(what, "gain_stage2")) {

        bool ok;
        double newgain = value.toDouble(&ok);
        if (!ok) this->no_set_error = 0;
        else if (std::find(GainsStage2.begin(), GainsStage2.end(), newgain) != GainsStage2.end()) {
            this->data.insert("gain_stage2", get_native(value, data.value("gain_stage2")));
            this->no_set_error = 1;
            return true;
        }
        else this->no_set_error = 0;

        return false;
    }

    else if (!QString::compare(what, "filter_type")) {

        QString newfilter = value.toString();
        if (filters.contains(newfilter)) {
            this->data.insert("filter_type", newfilter);
            this->no_set_error = 1;
            return true;
        }
        // fallback for the LF board
        else if (this->Type == 10) {
            // that is mostly used and ok
            this->data.insert("filter_type", QString("ADU07_LF_RF_4"));
            this->no_set_error = 2;
            return false;
        }
        else if (this->Type == 13) {
            // that is mostly used and ok
            if (this->data.value("channel_type").toString().startsWith(QChar('E'), Qt::CaseInsensitive))
            this->data.insert("filter_type", QString("LF_RF_2"));
            if (this->data.value("channel_type").toString().startsWith(QChar('H'), Qt::CaseInsensitive))
            this->data.insert("filter_type", QString("LF_RF_1"));  // for the coil - is default

            this->no_set_error = 2;
            return false;
        }

        if ((newfilter.size() < 2) && (this->Type == 11)) {
            this->data.insert("filter_type", QString(""));
            this->no_set_error = 1;
            return true;
        }
        // fallback for HF default on
        else if (this->Type == 11) {
            this->data.insert("filter_type", QString("ADU07_HF_HP_1HZ"));
            this->no_set_error = 2;
            return false;
        }



        return false;
    }

    else if (!QString::compare(what, "input_divider")) {

        if (value.toInt() == 0 || value.toInt() == 1) {
            this->prvdata.insert("input_divider", value);
            return true;
        }
        this->no_set_error = 0;
        return false;
    }

    else if (!QString::compare(what, "input")) {

        if (value.toInt() == 0 || value.toInt() == 1) {
            this->data.insert("input", value);
            return true;
        }
        this->no_set_error = 0;
        return false;
    }



    // pipe through if I don't have
    else return aduslot::set(what, value);

    return false;

}

void adbboard::traverse(const int &tinput, const QString &tinput_circuitry, const QString &tfilter_type_main)
{
    int i;

    if (this->auto_filter_type_main.size() == 0) return;
    if ( (this->auto_filter_type_main.size() != this->auto_input.size())
         || (this->auto_filter_type_main.size() != this->auto_input_circuitry.size())
         || (this->auto_filter_type_main.size() != this->auto_gain_stage1.size())
         || (this->auto_filter_type_main.size() != this->auto_gain_stage2.size())
         || (this->auto_filter_type_main.size() != this->auto_filter_type.size())
         || (this->auto_filter_type_main.size() != this->auto_dac_val.size())
         || (this->auto_filter_type_main.size() != this->auto_dac_on.size()) ) {
        return;
    }
    for (i = 0; i < auto_input.size(); ++i) {
        if (tinput == this->auto_input.at(i) &&
                tinput_circuitry == this->auto_input_circuitry.at(i) &&
                tfilter_type_main == this->auto_filter_type_main.at(i)) {

            this->data.insert("gain_stage1", get_native(this->auto_gain_stage1.at(i), data.value("gain_stage1")) );
            this->data.insert("gain_stage2", get_native(this->auto_gain_stage2.at(i), data.value("gain_stage2")) );
            this->data.insert("dac_val", get_native(this->auto_dac_val.at(i), data.value("dac_val")) );
            this->data.insert("dac_on", get_native(this->auto_dac_on.at(i), data.value("dac_on")) );
            if ((this->auto_filter_type_main.at(i).size() < 2 ) || (this->auto_filter_type_main.at(i) == "empty")) {
                this->data.insert("filter_type", this->auto_filter_type.at(i));
            }
            else {
                this->data.insert("filter_type", this->auto_filter_type_main.at(i));
            }
        }

    }
}

