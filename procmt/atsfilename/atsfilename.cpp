#include "atsfilename.h"


atsfilename::atsfilename(const QFileInfo &qfi, QObject *parent) : QObject(parent)
{
    this->clear_fname();
    this->t_name = qfi.baseName();
    this->suffix = qfi.suffix();
    this->atsfile = qfi;
    this->parse_fname(this->atsfile.exists());
}

atsfilename::atsfilename(QObject *parent) : QObject(parent)
{
    this->clear_fname();
}

void atsfilename::dir_split(const QFileInfo &qfidir, QFileInfo &basedir, QFileInfo &sitedir, QFileInfo &measdir)
{
    basedir.setFile("");
    sitedir.setFile("");
    measdir.setFile("");
    if ((qfidir.absolutePath().size() > 1) && qfidir.exists() && qfidir.isDir()) {

        QDir basedir(qfidir.absoluteDir());
        QString teststr = basedir.absolutePath();
        while (teststr.endsWith("/ts", Qt::CaseInsensitive) && teststr.size()) {
            if(basedir.cdUp()) {
                teststr = basedir.absolutePath();
            }
            else return;
        }
    }
    qDebug() << "basdir" << basedir.absolutePath();

}

void atsfilename::clear_fname()
{
    this->fill = '0';
    this->insert("fname_aduser",  int(0));
    this->insert("fname_version", QString("01"));
    this->insert("fname_channel", int (0));
    this->insert("fname_06_channel", QString("X"));
    this->insert("fname_run", int(0));

    // Ex Ey Hx Hy Hz
    this->insert("fname_type",  QString("  "));
    // A B X Y Z
    this->insert("fname_06type",  QString(" "));
    // BL BH BM
    this->insert("fname_board",  " ");
    this->insert("fname_sample",  int(0));
    this->insert("fname_sample_unit",  " ");
    this->insert("fname_band06", QString("X"));

    this->insert("fname_date_string", QString("        "));

    // "ns" natural source, "ca" controlled source AM, "cp" controlled source PM
    this->insert("fname_source", "   ");
    // e.g a number like 21007
    this->insert("observatory", "      ");

    // 07, CEA, 06
    this->insert("fname_format", "empty");


    this->is_init = false;
}

bool atsfilename::parse_fname(const bool read_atsheader) {
    if (!is_init) {
        if (!t_name.length()) return false;
        QStringList tokens;
        QString stmp;

        // 008_V01_C00_R000_TEx_BL_32H.ATS
        // 21007_20130322_nn_1024H_Ex.ats
        tokens = t_name.split("_");

        if (read_atsheader) {
            if (this->atsh == nullptr) {
                this->atsh =  std::make_unique<atsheader>(this->atsfile);
            }
            this->atsh->scan_header_close();
        }

        // get a CEA file WITH existing header

        if (this->siHeaderVers >= 1080) {
            /*            this->set_key_value("fname_format", "CEA");
             tokens = t_name.split("_");
             stmp = tokens[0];
             this->map["observatory"] = stmp.mid(0);


            this->map["aduser"] =  QString("%1").arg(this->atsh->value("Serial"),3, 10, fill);
            this->map["version"] = "01";
            this->map["channel"] =  QString("%1").arg(this->atsh->get_key_ivalue("channel"),2, 10, fill);
            this->map["run"] = 0;
            this->map["type"] = this->atsh->get_key_svalue("channel_type");
            this->map["board"] = this->atsh->get_key_svalue("AdbBoardType");
            if ((map["board"].size() == 2) && (map["board"].endsWith("F", Qt::CaseInsensitive))) {
                map["board"].remove(map["board"].size()-1,1);
            }
            this->map["source"] = tokens.at(2);
            this->set_samplefreq(this->atsh->get_key_dvalue("sample_freq"));
            this->sync_imap();


            qDebug() << this->atsh->get_datetime_ui();
            this->set_datetime_uint(this->atsh->get_datetime_ui());

*/

        }


        else if (this->t_name.indexOf("_C") != -1) {
            this->set_key_value("fname_format", "07");
            //  008_V01_C00_R000_TEx_BL_32H.ATS

            stmp = tokens[0];
            this->set_key_value("fname_aduser", stmp.mid(0));

            stmp = tokens[1];
            this->set_key_value("fname_version", stmp.mid(1));

            stmp = tokens[2];
            this->set_key_value("fname_channel", stmp.mid(1));

            stmp = tokens[3];
            this->set_key_value("fname_run", stmp.mid(1));

            stmp = tokens[4];
            this->set_key_value("fname_type", stmp.mid(1));

            stmp = tokens[5];
            this->set_key_value("fname_board", stmp.mid(1));



            // first grep the H Hertz or S Seconds
            this->set_key_value("fname_sample_unit", tokens[6].right(1));

            this->set_key_value("fname_sample", tokens[6].left(tokens[6].indexOf(svalue("fname_sample_unit"))));

            if (svalue("fname_sample_unit").toUpper() == "H") {
                this->sample_freq_h = this->value("fname_sample").toInt();
                this->sample_freq_s = 0;
            } else {
                this->sample_freq_s = this->value("fname_sample").toInt();
                this->sample_freq_h = 0;
            }
            this->channels_06_from_07();
            this->bandstr_06_from_samplefreq();
            this->channel_type_07_to_06();

            this->is_init = true;


        }

        else {
            //067A02AD.ats
            this->set_key_value("fname_format", "06");
            this->set_key_value("fname_aduser", t_name.mid(0,3));
            this->set_key_value("fname_06_channel", t_name.mid(3,1));
            this->set_key_value("fname_run", t_name.mid(4,2));
            this->set_key_value("fname_06type", t_name.mid(6,1));
            this->set_key_value("fname_band_06", t_name.mid(7,1));

            this->channels_07_from_06();
            this->channel_type_06_to_07();

            if (this->atsh != nullptr) {
                this->set_sampleFreq( this->atsh->get_sample_freq(), false);
            }

            this->is_init = true;



        }

        qDebug() << "file:" << this->t_name;
        this->show_my_map();



    }
    return is_init;
}

int atsfilename::inc_run(const int increment)
{
    int run = this->ivalue("fname_run") ;
    if ((run + increment) < 0) return INT_MAX;
    if (increment) {
        this->set_key_value("fname_run", this->ivalue("fname_run") + increment);
    }
    else {
        this->set_key_value("fname_run", this->ivalue("fname_run") + 1);
    }

    this->t_name = this->get_filename();
    this->parse_fname(false);

    return this->ivalue("fname_run");
}

void atsfilename::set_suffix(const QString &suffix)
{
    this->suffix = suffix.simplified();
    this->suffix.remove(QChar('.'));
}

QString atsfilename::get_filename(const QString type)
{
    if (is_init) {
        QString name;

        if (!QString::compare("07", type)) {
            name = this->svalue_rformat("fname_aduser", 3, '0') + "_V" + this->svalue_rformat("fname_version", 2, '0') + "_C" + this->svalue_rformat("fname_channel", 2, '0')
                    + "_R" + this->svalue_rformat("fname_run", 3, '0') + "_T" + this->svalue("fname_type") +
                    "_B" + this->svalue("fname_board") + "_" + this->svalue("fname_sample") + this->svalue("fname_sample_unit") + "." + this->suffix;

            //if (directory && this->dir.size()) return (this->dir + "/" + this->name07);
            //else return this->name07;
            return name;
        }

        if (!QString::compare("06", type)) {
            name = this->svalue_rformat("fname_aduser", 3, '0') + this->svalue("fname_06_channel") + this->svalue_rformat("fname_run", 2, '0')
                    + this->svalue("fname_06type") + this->svalue("fname_band06") + "." + suffix;
        }
        return name;
    }
    return "not initialized";
}

void atsfilename::set_sampleFreq(const double &dsample, const bool check)
{
    if (dsample >= 1.0) {
        this->sample_freq_h = (int)dsample;
        this->sample_freq_s = 0;

        this->set_key_value("fname_sample", this->sample_freq_h);
        this->set_key_value("fname_sample_unit", "H");
    }
    else {
        this->sample_freq_h = 0;
        this->sample_freq_s = (int)(1./dsample);

        this->set_key_value("fname_sample", this->sample_freq_s);
        this->set_key_value("fname_sample_unit", "S");

    }

    if (check) {
        this->t_name = this->get_filename();
        this->parse_fname(false);

    }
}

void atsfilename::slot_set_atsfilename(const QString &filename)
{
    this->clear_fname();
    this->atsfile.setFile(filename);
    this->t_name = this->atsfile.baseName();
    this->suffix = this->atsfile.suffix();
    this->parse_fname(false);
}

void atsfilename::slot_set_filename_for_measdoc_from_ats(const QString &filename, const QString &start_datetime, const QString &stop_datetime, const QString &absolute_measdir)
{
    if ((this->start_datetime == start_datetime) && (this->stop_datetime == stop_datetime) && (this->atsfile.fileName() == filename ) && (this->absolute_measdir == absolute_measdir ) ) return;
    this->clear_fname();
    this->atsfile.setFile(QDir(absolute_measdir), filename);
    this->t_name = this->atsfile.baseName();
    this->suffix = this->atsfile.suffix();

    if (this->start_datetime != start_datetime) {

        // new dir needed
    }

    this->start_datetime = start_datetime;
    this->stop_datetime = stop_datetime;
    this->parse_fname(false);

    //this->measdoc_name = "meas_" + this->sta;




}


void atsfilename::channels_06_from_07()
{
    if (!this->is_init) {
        if (this->ivalue("fname_channel") == 0)
            this->set_key_value("fname_06_channel","A");
        if (this->ivalue("fname_channel") == 1)
            this->set_key_value("fname_06_channel","B");
        if (this->ivalue("fname_channel") == 2)
            this->set_key_value("fname_06_channel","C");
        if (this->ivalue("fname_channel") == 3)
            this->set_key_value("fname_06_channel","D");
        if (this->ivalue("fname_channel") == 4)
            this->set_key_value("fname_06_channel","E");

        if (this->ivalue("fname_channel") == 5)
            this->set_key_value("fname_06_channel","A");
        if (this->ivalue("fname_channel") == 6)
            this->set_key_value("fname_06_channel","B");
        if (this->ivalue("fname_channel") == 7)
            this->set_key_value("fname_06_channel","C");
        if (this->ivalue("fname_channel") == 8)
            this->set_key_value("fname_06_channel","D");
        if (this->ivalue("fname_channel") == 9)
            this->set_key_value("fname_06_channel","E");
    }
}

void atsfilename::channels_07_from_06()
{
    if (!this->is_init) {
        if (this->svalue("fname_06_channel") == "A")
            this->set_key_value("fname_channel", 0);
        if (this->svalue("fname_06_channel") == "B")
            this->set_key_value("fname_channel", 1);
        if (this->svalue("fname_06_channel") == "C")
            this->set_key_value("fname_channel", 2);
        if (this->svalue("fname_06_channel") == "D")
            this->set_key_value("fname_channel", 3);
        if (this->svalue("fname_06_channel") == "E")
            this->set_key_value("fname_channel", 4);
        if (this->svalue("fname_06_channel") == "F")
            this->set_key_value("fname_channel", 5);
        if (this->svalue("fname_06_channel") == "G")
            this->set_key_value("fname_channel", 6);
        if (this->svalue("fname_06_channel") == "H")
            this->set_key_value("fname_channel", 7);
    }

}

void atsfilename::bandstr_06_from_samplefreq()
{
    if (!this->is_init) {
        if (sample_freq_h > 4096)
            this->set_key_value("fname_band06", "A");
        else if (sample_freq_h >= 512 && sample_freq_h <= 4096)
            this->set_key_value("fname_band06", "B");
        //    else if (sample_freq_h >= 256. && sample_freq_h < 1024.)
        //      this->set_key_value("fname_band06", "F");
        else if (sample_freq_h >= 64 && sample_freq_h < 512)
            this->set_key_value("fname_band06", "C");
        else if (sample_freq_h > 1 && sample_freq_h < 64)
            this->set_key_value("fname_band06", "D");

        else if (sample_freq_s >= 1 && sample_freq_s <= 16)
            this->set_key_value("fname_band06", "E");
        else if (sample_freq_s > 16)
            this->set_key_value("fname_band06", "G");
    }
}

void atsfilename::channel_type_07_to_06()
{
    if (!this->is_init) {
        if (this->svalue("fname_type").toUpper() == "EX")
            this->set_key_value("fname_06type", "A");
        if (this->svalue("fname_type").toUpper() == "EY")
            this->set_key_value("fname_06type", "B");
        if (this->svalue("fname_type").toUpper() == "HX")
            this->set_key_value("fname_06type", "X");
        if (this->svalue("fname_type").toUpper() == "HY")
            this->set_key_value("fname_06type", "Y");
        if (this->svalue("fname_type").toUpper() == "HZ")
            this->set_key_value("fname_06type", "Z");
    }
}

void atsfilename::channel_type_06_to_07()
{
    if (!this->is_init) {
        if (this->svalue("fname_06type").toUpper() == "A")
            this->svalue("fname_type") = "Ex";
        if (this->svalue("fname_06type").toUpper() == "B")
            this->svalue("fname_type") = "Ey";
        if (this->svalue("fname_06type").toUpper() == "X")
            this->svalue("fname_type") = "Hx";
        if (this->svalue("fname_06type").toUpper() == "Y")
            this->svalue("fname_type") = "Hy";
        if (this->svalue("fname_06type").toUpper() == "Z")
            this->svalue("fname_type") = "Hz";
    }
}
