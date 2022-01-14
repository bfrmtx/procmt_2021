#include "edi_file.h"


edi_file::edi_file(QObject *parent): QObject(parent), QFileInfo(), prc_com(false)
{
    // >HEAD
    this->edi_HEAD.insert("DATAID","GEO");
    this->edi_HEAD.insert("ACQBY","Metronix");
    this->edi_HEAD.insert("FILEBY","Metronix");
    this->edi_HEAD.insert("ACQDATE","12/31/99");
    this->edi_HEAD.insert("ENDDATE","12/31/99");
    this->edi_HEAD.insert("FILEDATE","auto");
    this->edi_HEAD.insert("REFLOC","auto");
    this->edi_HEAD.insert("STATE","LX");
    this->edi_HEAD.insert("COUNTRY","Germany");
    this->edi_HEAD.insert("PROSPECT","auto");
    this->edi_HEAD.insert("STDVERS=", "SEG 1.0");
    this->edi_HEAD.insert("PROGVERS", "Version  Date: 2020-08-24");
    this->edi_HEAD.insert("PROGDATE","Version  Date: 2020-08-24");
    this->edi_HEAD.insert("MAXSECT", "12");
    this->edi_HEAD.insert("EMPTY", "1E+32");
    this->edi_HEAD.insert("LAT", "00:00:00.00");
    this->edi_HEAD.insert("LONG", "00:00:00.00");
    this->edi_HEAD.insert("ELEV", 0.0);


    // >INFO
    this->edi_INFO.insert("MAXINFO", "1000");

    this->edi_DEFINEMEAS.insert("MAXCHAN", "9");
    this->edi_DEFINEMEAS.insert("MAXRUN", "999");
    this->edi_DEFINEMEAS.insert("MAXMEAS", "1000");
    this->edi_DEFINEMEAS.insert("REFTYPE", "CART");
    this->edi_DEFINEMEAS.insert("REFLOC", "auto");
    this->edi_DEFINEMEAS.insert("REFLAT", "00:00:00.00");
    this->edi_DEFINEMEAS.insert("REFLONG", "00:00:00.00");
    this->edi_DEFINEMEAS.insert("REFELEV", 0.0);


    this->znames_real << ">ZXXR //" << ">ZXYR //" <<  ">ZYXR //" <<  ">ZYYR //" << ">TXR.EXP //" << ">TYR.EXP //";
    this->znames_imag << ">ZXXI //" << ">ZXYI //" <<  ">ZYXI //" <<  ">ZYYI //" << ">TXI.EXP //" << ">TYI.EXP //";
    this->znames_var  << ">ZXX.VAR //" << ">ZXY.VAR //" << ">ZYX.VAR //" << ">ZYY.VAR //" << ">TXVAR.EXP //" << ">TYVAR.EXP //";

    //this->tnames_real << ">TXR.EXP //" << ">TYR.EXP //";
    //this->tnames_imag << ">TXI.EXP //" << ">TYI.EXP //";
    //this->tnames_var  << ">TXVAR.EXP //" << ">TYVAR.EXP //";
    // std::cerr << "edi header" << std::endl;

}

edi_file::edi_file(const edi_file &edi)  :  QObject(edi.parent()), prc_com(false)
{
    this->operator =(edi);
    //qDebug() << "header copy";
}

edi_file &edi_file::operator = (const edi_file &rhs)
{
    if(&rhs == this) return *this;

    this->edi_DEFINEMEAS = rhs.edi_DEFINEMEAS;
    this->edi_HEAD = rhs.edi_HEAD;
    this->edi_INFO = rhs.edi_INFO;
    this->znames_real = rhs.znames_real;
    this->znames_imag = rhs.znames_imag;
    this->znames_var = rhs.znames_var;
    this->set_QFileInfo(rhs);
    this->msg = rhs.msg;
    this->z = rhs.z;
    this->in_ats_prc_coms.resize(rhs.in_ats_prc_coms.size());
    for (size_t i = 0; i < this->in_ats_prc_coms.size(); ++i) {
        this->in_ats_prc_coms[i] = rhs.in_ats_prc_coms.at(i);
    }
    return *this;
}

bool edi_file::test_full_tensor()
{
    return this->z.test_full_tensor();
}

bool edi_file::test_tipper() const
{
    return this->z.test_tipper();

}

edi_file::~edi_file()
{
    if (this->file.isOpen()) this->file.close();
    this->msg.reset();

    qDebug() << "edi_file " << this->baseName() << "destroyed";
}

void edi_file::set_QFileInfo(const QFileInfo &qfi)
{
    this->setFile(qfi.absoluteFilePath());
}

void edi_file::set_msg_logger(std::shared_ptr<msg_logger> msg)
{
    this->msg = msg;
    if (this->msg != nullptr) {
        QObject::connect(this, &edi_file::signal_general_msg,  this->msg.get(), &msg_logger::slot_general_msg);
    }
}

bool edi_file::toASCII_file(const QFileInfo &qfi) const
{
    return this->z.toASCII_file(qfi);
}

void edi_file::set_edi_data_z(const mt_data_res<std::complex<double> > &zin, const std::vector<prc_com> &prcs)
{

    int loops = 0;
    this->z = zin;
    this->z.check_delete_err_dbl_max();
    if (!this->in_ats_prc_coms.size()) {
        this->in_ats_prc_coms = prcs;

        //        >EMEAS ID=1000.0001 CHTYPE=EX X=-3.000000e+01 Y=0.000000e+00 Z=0.000000e+00 X2=3.000000e+01 Y2=0.000000e+00 Z2=0.000000e+00
        //        >EMEAS ID=1001.0001 CHTYPE=EY X=0.000000e+00 Y=-3.000000e+01 Z=0.000000e+00 X2=0.000000e+00 Y2=3.000000e+01 Z2=0.000000e+00
        //        >HMEAS ID=1002.0001 CHTYPE=HX X=0.000000e+00 Y=0.000000e+00 Z=0.000000e+00 X2=0.000000e+00 Y2=0.000000e+00 Z2=0.000000e+00
        //        >HMEAS ID=1003.0001 CHTYPE=HY X=0.000000e+00 Y=0.000000e+00 Z=0.000000e+00 X2=0.000000e+00 Y2=0.000000e+00 Z2=0.000000e+00
        //        >HMEAS ID=1004.0001 CHTYPE=HZ X=0.000000e+00 Y=0.000000e+00 Z=0.000000e+00 X2=0.000000e+00 Y2=0.000000e+00 Z2=0.000000e+00
        for (auto &prc : this->in_ats_prc_coms) {

            QString line;
            if (prc.svalue("channel_type").startsWith(QChar('E'), Qt::CaseInsensitive)) {

                if (prc.ivalue("channel_number") < 10) {
                    line.append(">EMEAS ID=100");
                }
                else {
                    line.append(">EMEAS ID=10");
                }

                // take the positon from either EMAP or local where we should have E
                if (!loops) {

                    geocoordinates geocor;
                    eQDateTime edtstart, edtstop;

                    // elevation of the metronix adu should be in cm
                    geocor.set_lat_lon_msec(prc.ivalue("latitude"), prc.ivalue("longitude"), (prc.dvalue("elevation") / 100.));
                    QString sla, slo;
                    geocor.get_lat_lon_str_EDI( sla, slo);
                    this->edi_HEAD.insert("LAT", sla);
                    this->edi_HEAD.insert("LONG", slo);
                    this->edi_HEAD.insert("ELEV", geocor.get_elevation_str(false));

                    edtstart.set_date_time(prc.qint64value("start_date_time"));
                    edtstop.set_date_time(prc.qint64value("stop_date_time"));


                    this->edi_HEAD.insert("ACQDATE", edtstart.toString("MM/dd/yy") + " " + edtstart.toString("hh:mm:ss"));
                    this->edi_HEAD.insert("ENDDATE", edtstop.toString("MM/dd/yy") + " " + edtstop.toString("hh:mm:ss"));


                    this->edi_DEFINEMEAS.insert("REFLAT", sla);
                    this->edi_DEFINEMEAS.insert("REFLONG", slo);
                    this->edi_DEFINEMEAS.insert("REFELEV",  geocor.get_elevation_str(false));

                    ++loops;

                }

            }
            if (prc.svalue("channel_type").startsWith(QChar('H'), Qt::CaseInsensitive)) {
                if (prc.ivalue("channel_number") < 10) {
                    line.append(">HMEAS ID=100");
                }
                else {
                    line.append(">HMEAS ID=10");
                }
            }
            line.append(prc.svalue("channel_number") + ".0001 CHTYPE=");

            line.append(prc.svalue("channel_type").toUpper() + " ");
            line.append("X=" + QString::number(prc.dvalue("pos_x1"), 'e', 6) + " ");
            if (prc.dvalue("pos_x1") > 0) line.append(" ");
            line.append("Y=" + QString::number(prc.dvalue("pos_y1"), 'e', 6) + " ");
            if (prc.dvalue("pos_y1") > 0) line.append(" ");
            line.append("Z=" + QString::number(prc.dvalue("pos_z1"), 'e', 6) + " ");
            if (prc.dvalue("pos_z1") > 0) line.append(" ");
            line.append("X2=" + QString::number(prc.dvalue("pos_x2"), 'e', 6) + " ");
            if (prc.dvalue("pos_x2") > 0) line.append(" ");
            line.append("Y2=" + QString::number(prc.dvalue("pos_y2"), 'e', 6) + " ");
            if (prc.dvalue("pos_y2") > 0) line.append(" ");
            line.append("Z2=" + QString::number(prc.dvalue("pos_z2"), 'e', 6) + " ");

            this->e_h_measids.append(line);

            if (prc.ivalue("channel_number") < 10) {
                this->e_h_meas.insert(prc.svalue("channel_type").toUpper(), QString( "100" + prc.svalue("channel_number") + ".0001"));
            }
            else {
                this->e_h_meas.insert(prc.svalue("channel_type").toUpper(), QString("10" + prc.svalue("channel_number") + ".0001"));

            }

        }
    }


}

bool edi_file::write_head()
{
    if (!this->is_open) return this->is_open;

    qts << ">HEAD" << Qt::endl;
    for (auto it = this->edi_HEAD.cbegin(); it != this->edi_HEAD.cend(); ++it) {
        qts << "  " << it.key() << "=" << it.value().toString() << Qt::endl;
    }

    qts << Qt::endl;
    return this->is_open;
}

bool edi_file::write_info()
{
    if (!this->is_open) return this->is_open;

    qts << ">INFO" << Qt::endl;
    for (auto it = this->edi_INFO.cbegin(); it != this->edi_INFO.cend(); ++it) {
        qts << "  " << it.key() << "=" << it.value().toString() << Qt::endl;
    }

    qts << Qt::endl;
    return this->is_open;
}

bool edi_file::write_definemeas()
{

    if (!this->is_open) return this->is_open;

    qts << ">=DEFINEMEAS" << Qt::endl;

    for (auto it = this->edi_DEFINEMEAS.cbegin(); it != this->edi_DEFINEMEAS.cend(); ++it) {
        qts << "  " << it.key() << "=" << it.value().toString() << Qt::endl;
    }

    qts << Qt::endl;
    return this->is_open;
}

bool edi_file::write_e_h_meas()
{
    if (!this->is_open) return this->is_open;

    if (this->e_h_measids.size()) {
        for (auto &str : this->e_h_measids) {
            qts << str << Qt::endl;
        }
    }

    qts << Qt::endl;
    return this->is_open;
}

bool edi_file::write_mtsect()
{
    if (!this->is_open) return this->is_open;
    //std::vector<double> freqs;

    //auto fs = this->z.cbegin();  // begin returns a pointer

    //    for (auto &af : z) {    // that is the f1, f2, f3 part - so af.first is the freq, af.second is the z element
    //        freqs.push_back(af.f);
    //    }

    int i = 0;
    qts << ">=MTSECT" << Qt::endl;
    qts << "  SECTID=1" << Qt::endl;
    qts << "  NFREQ=" << z.freqs.size() << Qt::endl;

    QMap<QString, QString>::const_iterator iter = this->e_h_meas.constBegin();
    while (iter != this->e_h_meas.constEnd()) {
        qts  << "  " << iter.key() << "=" << iter.value() << Qt::endl;
        ++iter;
    }

    qts << Qt::endl;


    qts << ">FREQ //" << z.freqs.size() << Qt::endl;
    for (auto &d : z.freqs) {
        qts << " " << d;
        if (i < 4) qts << " ";
        else {
            qts << Qt::endl;
            i = -1;
        }
        ++i;
    }
    qts << Qt::endl;
    // std::cerr << "write mtss f done" << std::endl;


    size_t elm;
    //xx, xy ..yy
    // std::cerr <<  " dsize " << this->z.d.size() << std::endl;

    for (elm = 0; elm < this->z.d.size(); ++elm) {

        qts << Qt::endl << this->znames_real.at(elm) << z.freqs.size() << Qt::endl;
        i = 0;

        for (auto &allf : this->z.d[elm]) {

            if (real(allf) > 0.0) qts << " ";
            qts << real(allf) << " ";
            if (i > 3) {
                qts << Qt::endl;
                i = -1;
            }

            ++i;
        }
        qts << Qt::endl;

        // std::cerr << "write mtss re done" << std::endl;


        qts << Qt::endl << this->znames_imag.at(elm) << z.freqs.size() << Qt::endl;
        i = 0;
        for (auto &allf : this->z.d[elm]) {

            if (imag(allf) > 0.0) qts << " ";
            qts << imag(allf) << " ";
            if (i > 3) {
                qts << Qt::endl;
                i = -1;
            }
            ++i;
        }
        qts << Qt::endl;
        // std::cerr << "write mtss im done" << std::endl;


        qts << Qt::endl << this->znames_var.at(elm) << z.freqs.size() << Qt::endl;
        i = 0;

        // std::cerr <<  " err size " << this->z.err.size() << std::endl;

        for (auto &allf : this->z.err[elm]) {

            qts << " " << allf << " ";
            if (i > 3) {
                qts << Qt::endl;
                i = -1;
            }
            ++i;
        }
        qts << Qt::endl;


        // std::cerr << "write mtss var done" << std::endl;
    }

    if (this->z.coh[xy].size() && this->z.coh[yx].size()) {

        qts << Qt::endl << ">COH  MEAS1=" << this->e_h_meas.value("EX") << " MEAS2=" << this->e_h_meas.value("HY") << "  ROT=NORTH  //" << z.freqs.size() << Qt::endl;
        i = 0;
        for (auto &allf : this->z.coh[xy]) {

            qts << " " << allf << " ";
            if (i > 3) {
                qts << Qt::endl;
                i = -1;
            }
            ++i;
        }
        qts << Qt::endl;

        qts << Qt::endl << ">COH  MEAS1=" << this->e_h_meas.value("EY") << " MEAS2=" << this->e_h_meas.value("HX") << "  ROT=NORTH  //" << z.freqs.size() << Qt::endl;
        i = 0;
        for (auto &allf : this->z.coh[yx]) {

            qts << " " << allf << " ";
            if (i > 3) {
                qts << Qt::endl;
                i = -1;
            }
            ++i;
        }
        qts << Qt::endl;



    }

    // std::cerr << "hell on earth" << std::endl;
    qts << Qt::endl;

    return this->is_open;
}

bool edi_file::read_head()
{
    this->edi_HEAD.clear();
    if (this->seek_key(">HEAD")) {
        QStringList strlst;
        QStringList data;
        do {
            this->line = this->qts.readLine();
            this->line = this->line.simplified();
            strlst = this->line.split("=", Qt::SkipEmptyParts);
            for (auto &str : strlst) {
                str = str.simplified();
            }
            if ((strlst.size() % 2) == 0 ) {
                data.append(strlst);
            }

        } while (!(this->line.startsWith(">=") || this->line.startsWith(">END")) );


        for (int i = 0; i < data.size() - 1; ++i) {
            this->edi_HEAD.insert(data.at(i), data.at(i + 1));
            ++i;
        }

        emit this->signal_general_msg("edifile", "read HEAD ", "success");

        return true;
    }

    emit this->signal_general_msg("edifile", "read HEAD ", "ERROR");

    return false;
}

bool edi_file::read_info()
{
    this->edi_INFO.clear();
    if (this->seek_key(">INFO")) {

        QStringList strlst;
        QStringList data;
        do {
            this->line = this->qts.readLine();
            this->line = this->line.simplified();
            strlst = this->line.split("=", Qt::SkipEmptyParts);
            for (auto &str : strlst) {
                str = str.simplified();
            }
            if ((strlst.size() % 2) == 0 ) {
                data.append(strlst);
            }

        } while (!(this->line.startsWith(">=") || this->line.startsWith(">END")) );


        for (int i = 0; i < data.size() - 1; ++i) {
            this->edi_INFO.insert(data.at(i), data.at(i + 1));
            ++i;
        }

        emit this->signal_general_msg("edifile", "read INFO ", "success");

        return true;
    }
    emit this->signal_general_msg("edifile", "read INFO ", "ERROR");

    return false;
}

bool edi_file::read_definemeas()
{
    if (this->seek_key(">=DEFINEMEAS")) {
        QStringList strlst;
        QStringList data;
        do {
            this->line = this->qts.readLine();
            this->line = this->line.simplified();
            strlst = this->line.split("=", Qt::SkipEmptyParts);
            for (auto &str : strlst) {
                str = str.simplified();
            }
            if ((strlst.size() % 2) == 0 ) {
                data.append(strlst);
            }

        } while (!(this->line.startsWith(">=") || this->line.startsWith(">END")) );


        for (int i = 0; i < data.size() - 1; ++i) {
            this->edi_DEFINEMEAS.insert(data.at(i), data.at(i + 1));
            ++i;
        }

        emit this->signal_general_msg("edifile", "read DEFINEMEAS ", "success");

        return true;
    }

    emit this->signal_general_msg("edifile", "read DEFINEMEAS ", "ERROR");

    return false;
}

bool edi_file::read_e_h_meas()
{
    if (this->seek_key(">=DEFINEMEAS")) {
        QStringList strlst;
        QStringList data;
        std::vector<prc_com> in_ats_prc_coms_tmp;
        do {
            this->line = this->qts.readLine();
            this->line = this->line.simplified();
            if (this->line.startsWith(">EMEAS") ||this->line.startsWith(">HMEAS") ) {
                this->line = this->line.replace(">EMEAS", "");
                this->line = this->line.replace(">HMEAS", "");
                this->line = this->line.simplified();
                strlst = this->line.split(" ", Qt::SkipEmptyParts);
                prc_com prc;
                for (auto &str : strlst) {
                    str = str.simplified();
                }

                for (auto &str : strlst) {
                    QStringList strmp = str.split("=");
                    if (!(strmp.size() % 2)) data.append(strmp);
                }

                prc.insert("channel_type", "empty");
                for (int i = 0; i < data.size() - 1; ++i) {

                    if (!QString::compare(data.at(i), "X", Qt::CaseInsensitive)) prc.insert_double("pos_x1", data.at(i+1).toDouble());
                    else if (!QString::compare(data.at(i), "Y", Qt::CaseInsensitive)) prc.insert_double("pos_y1", data.at(i+1).toDouble());
                    else if (!QString::compare(data.at(i), "Z", Qt::CaseInsensitive)) prc.insert_double("pos_z1", data.at(i+1).toDouble());

                    else if (!QString::compare(data.at(i), "X2", Qt::CaseInsensitive)) prc.insert_double("pos_x2", data.at(i+1).toDouble());
                    else if (!QString::compare(data.at(i), "Y2", Qt::CaseInsensitive)) prc.insert_double("pos_y2", data.at(i+1).toDouble());
                    else if (!QString::compare(data.at(i), "Z2", Qt::CaseInsensitive)) prc.insert_double("pos_z2", data.at(i+1).toDouble());

                    else if (!QString::compare(data.at(i), "CHTYPE", Qt::CaseInsensitive)) {
                        for (auto &str : pmt::channel_types) {
                            if (!QString::compare(str, data.at(i+1), Qt::CaseInsensitive)) prc.insert("channel_type", str);
                        }
                        if (prc.svalue("channel_type") == "empty") prc.insert("channel_type", data.at(i+1));
                    }

                    else if (!QString::compare(data.at(i), "ID", Qt::CaseInsensitive)) {
                        prc.insert("string_id", data.at(i+1));
                    }


                    ++i;
                }
                in_ats_prc_coms_tmp.push_back(prc);
                strlst.clear();
                data.clear();


            }

        } while (!(this->line.startsWith(">=") || this->line.startsWith(">END")) );


        // keep the standard sort
        for (auto str : pmt::channel_types) {
            for (auto prc : in_ats_prc_coms_tmp) {
                if (prc.svalue("channel_type") == str) this->in_ats_prc_coms.push_back(prc);
            }
        }
        // don't know what to do
        for (auto str : pmt::channel_types) {
            for (auto prc : in_ats_prc_coms_tmp) {
                if (!pmt::channel_types.contains(str, Qt::CaseInsensitive)) this->in_ats_prc_coms.push_back(prc);
            }
        }

        emit this->signal_general_msg("edifile", "read EMEAS, HMEAS, found channels ", QString::number(in_ats_prc_coms.size()));

        return true;
    }

    emit this->signal_general_msg("edifile", "read EMEAS, HMEAS", "ERROR");
    return false;
}


bool edi_file::read_mtsect()
{
    std::vector<double> tmpv;

    std::vector<std::complex<double>> tmp_cplx;

    // FREQ
    this->seek_key(">=MTSECT");
    if (this->seek_key(">FREQ", true)) {
        if(this->read_number_block(tmpv)) {
            this->z.freqs = tmpv;
        }
        emit this->signal_general_msg("edifile", "read FREQ mtsect Z, found", "freqs " + QString::number(this->z.freqs.size()));

    }

    // ZXX
    tmp_cplx.clear();
    this->seek_key(">=MTSECT");
    if (this->seek_key(">ZXXR", true)) {
        if(this->read_number_block(tmpv)) {
            tmp_cplx.resize(tmpv.size());
            size_t i = 0;
            for (auto &d : tmpv) {
                tmp_cplx[i++].real(d);
            }
            emit this->signal_general_msg("edifile", "read ZXXR mtsect Z, reading", QString::number(tmpv.size()));

        }
    }

    this->seek_key(">=MTSECT");
    if (this->seek_key(">ZXXI", true)) {
        if(this->read_number_block(tmpv)) {
            if (tmpv.size() != tmp_cplx.size()) {
                emit this->signal_general_msg("edifile", "read ZXXI mtsect Z, error reading", QString::number(tmpv.size()));
            }
            else {
                size_t i = 0;
                for (auto &d : tmpv) {
                    tmp_cplx[i++].imag(d);
                }
                this->z.d[xx] = tmp_cplx;
                emit this->signal_general_msg("edifile", "read ZXXI mtsect Z, reading", QString::number(tmpv.size()));
            }
            tmp_cplx.clear();
        }
    }

    this->seek_key(">=MTSECT");
    if (this->seek_key(">ZXX.VAR", true)) {
        if(this->read_number_block(tmpv)) {
            this->z.err[xx] = tmpv;
        }
        emit this->signal_general_msg("edifile", "read ZXX.VAR mtsect Z, reading ", QString::number(z.d[xx].size()));

    }

    // ZXY
    tmp_cplx.clear();
    this->seek_key(">=MTSECT");
    if (this->seek_key(">ZXYR", true)) {
        if(this->read_number_block(tmpv)) {
            tmp_cplx.resize(tmpv.size());
            size_t i = 0;
            for (auto &d : tmpv) {
                tmp_cplx[i++].real(d);
            }
            emit this->signal_general_msg("edifile", "read ZXYR mtsect Z, reading", QString::number(tmpv.size()));

        }
    }

    this->seek_key(">=MTSECT");
    if (this->seek_key(">ZXYI", true)) {
        if(this->read_number_block(tmpv)) {
            if (tmpv.size() != tmp_cplx.size()) {
                emit this->signal_general_msg("edifile", "read ZXYI mtsect Z, error reading", QString::number(tmpv.size()));
            }
            else {
                size_t i = 0;
                for (auto &d : tmpv) {
                    tmp_cplx[i++].imag(d);
                }
                this->z.d[xy] = tmp_cplx;
                emit this->signal_general_msg("edifile", "read ZXYI mtsect Z, reading", QString::number(tmpv.size()));
            }
            tmp_cplx.clear();
        }
    }

    this->seek_key(">=MTSECT");
    if (this->seek_key(">ZXY.VAR", true)) {
        if(this->read_number_block(tmpv)) {
            this->z.err[xy] = tmpv;
        }
        emit this->signal_general_msg("edifile", "read ZXY.VAR mtsect Z, reading ", QString::number(z.d[xx].size()));

    }

    // ZYX
    tmp_cplx.clear();
    this->seek_key(">=MTSECT");
    if (this->seek_key(">ZYXR", true)) {
        if(this->read_number_block(tmpv)) {
            tmp_cplx.resize(tmpv.size());
            size_t i = 0;
            for (auto &d : tmpv) {
                tmp_cplx[i++].real(d);
            }
            emit this->signal_general_msg("edifile", "read ZYXR mtsect Z, reading", QString::number(tmpv.size()));

        }
    }

    this->seek_key(">=MTSECT");
    if (this->seek_key(">ZYXI", true)) {
        if(this->read_number_block(tmpv)) {
            if (tmpv.size() != tmp_cplx.size()) {
                emit this->signal_general_msg("edifile", "read ZYXI mtsect Z, error reading", QString::number(tmpv.size()));
            }
            else {
                size_t i = 0;
                for (auto &d : tmpv) {
                    tmp_cplx[i++].imag(d);
                }
                this->z.d[yx] = tmp_cplx;
                emit this->signal_general_msg("edifile", "read ZYXI mtsect Z, reading", QString::number(tmpv.size()));
            }
            tmp_cplx.clear();
        }
    }

    this->seek_key(">=MTSECT");
    if (this->seek_key(">ZYX.VAR", true)) {
        if(this->read_number_block(tmpv)) {
            this->z.err[yx] = tmpv;
        }
        emit this->signal_general_msg("edifile", "read ZYX.VAR mtsect Z, reading ", QString::number(z.d[xx].size()));

    }

    // ZYY
    tmp_cplx.clear();
    this->seek_key(">=MTSECT");
    if (this->seek_key(">ZYYR", true)) {
        if(this->read_number_block(tmpv)) {
            tmp_cplx.resize(tmpv.size());
            size_t i = 0;
            for (auto &d : tmpv) {
                tmp_cplx[i++].real(d);
            }
            emit this->signal_general_msg("edifile", "read ZYYR mtsect Z, reading", QString::number(tmpv.size()));

        }
    }

    this->seek_key(">=MTSECT");
    if (this->seek_key(">ZYYI", true)) {
        if(this->read_number_block(tmpv)) {
            if (tmpv.size() != tmp_cplx.size()) {
                emit this->signal_general_msg("edifile", "read ZYYI mtsect Z, error reading", QString::number(tmpv.size()));
            }
            else {
                size_t i = 0;
                for (auto &d : tmpv) {
                    tmp_cplx[i++].imag(d);
                }
                this->z.d[yy] = tmp_cplx;
                emit this->signal_general_msg("edifile", "read ZYYI mtsect Z, reading", QString::number(tmpv.size()));
            }
            tmp_cplx.clear();
        }
    }

    this->seek_key(">=MTSECT");
    if (this->seek_key(">ZYY.VAR", true)) {
        if(this->read_number_block(tmpv)) {
            this->z.err[yy] = tmpv;
        }
        emit this->signal_general_msg("edifile", "read ZYY.VAR mtsect Z, reading ", QString::number(z.d[xx].size()));

    }

    this->seek_key(">=MTSECT");
    if (this->seek_key(">TXR.EXP", true)) {
        if(this->read_number_block(tmpv)) {
            tmp_cplx.resize(tmpv.size());
            size_t i = 0;
            for (auto &d : tmpv) {
                tmp_cplx[i++].real(d);
            }
            emit this->signal_general_msg("edifile", "read TXR.EXP mtsect Z, reading", QString::number(tmpv.size()));

        }
    }

    this->seek_key(">=MTSECT");
    if (this->seek_key(">TXI.EXP", true)) {
        if(this->read_number_block(tmpv)) {
            if (tmpv.size() != tmp_cplx.size()) {
                emit this->signal_general_msg("edifile", "read TXI.EXP mtsect Z, error reading", QString::number(tmpv.size()));
            }
            else {
                size_t i = 0;
                for (auto &d : tmpv) {
                    tmp_cplx[i++].imag(d);
                }
                this->z.d[tx] = tmp_cplx;
                emit this->signal_general_msg("edifile", "read TXI.EXP mtsect Z, reading", QString::number(tmpv.size()));
            }
            tmp_cplx.clear();
        }
    }

    this->seek_key(">=MTSECT");
    if (this->seek_key(">TXVAR.EXP", true)) {
        if(this->read_number_block(tmpv)) {
            this->z.err[tx] = tmpv;
        }
        emit this->signal_general_msg("edifile", "read TXVAR.EXP mtsect Z, reading ", QString::number(z.d[tx].size()));

    }

    tmp_cplx.clear();
    this->seek_key(">=MTSECT");
    if (this->seek_key(">TYR.EXP", true)) {
        if(this->read_number_block(tmpv)) {
            tmp_cplx.resize(tmpv.size());
            size_t i = 0;
            for (auto &d : tmpv) {
                tmp_cplx[i++].real(d);
            }
            emit this->signal_general_msg("edifile", "read TYR.EXP mtsect Z, reading", QString::number(tmpv.size()));

        }
    }

    this->seek_key(">=MTSECT");
    if (this->seek_key(">TYI.EXP", true)) {
        if(this->read_number_block(tmpv)) {
            if (tmpv.size() != tmp_cplx.size()) {
                emit this->signal_general_msg("edifile", "read TYI.EXP mtsect Z, error reading", QString::number(tmpv.size()));
            }
            else {
                size_t i = 0;
                for (auto &d : tmpv) {
                    tmp_cplx[i++].imag(d);
                }
                this->z.d[ty] = tmp_cplx;
                emit this->signal_general_msg("edifile", "read TYI.EXP mtsect Z, reading", QString::number(tmpv.size()));
            }
            tmp_cplx.clear();
        }
    }

    this->seek_key(">=MTSECT");
    if (this->seek_key(">TYVAR.EXP", true)) {
        if(this->read_number_block(tmpv)) {
            this->z.err[ty] = tmpv;
        }
        emit this->signal_general_msg("edifile", "read TYVAR.EXP mtsect Z, reading ", QString::number(z.d[ty].size()));

    }




    // COH
    this->seek_key(">=MTSECT");
    qint64 pos = 0;
    bool has_xy = false;
    bool has_yx = false;
    while (this->seek_key(">COH", true, true)) {
        //qDebug() << this->line;
        pos = this->read_a_coh_section();
        if ((this->z.freqs.size() == this->z.coh[xy].size()) && has_xy == false) {
            has_xy = true;
            emit this->signal_general_msg("edifile", "read COH mtsect Z, reading xy ", QString::number(z.coh[xy].size()));
        }
        if ((this->z.freqs.size() == this->z.coh[yx].size()) && has_yx == false) {
            has_yx = true;
            emit this->signal_general_msg("edifile", "read COH mtsect Z, reading yx ", QString::number(z.coh[yx].size()));

        }
        this->qts.seek(pos); // first COH line was read, so we are in front of the number  block, seek back
    }



    bool can_cont = false;
    if (this->z.test_tensor()) {
        can_cont = true;
        if (this->z.test_full_tensor()) {
            emit this->signal_general_msg("edifile", "Z Data ", "OK");

        }
        else {
            emit this->signal_general_msg("edifile", "Z Data ", "not full");
        }
        this->edi_INFO.insert("MAXINFO", 1000);
        int i = 0;
        for (auto &prc : this->in_ats_prc_coms) {
            prc.insert_int("channel_number",i++);
        }

        if (this->z.test_tipper()) {
            emit this->signal_general_msg("edifile", "Tipper Data ", "OK");
        }

    }
    else {
        emit this->signal_general_msg("edifile", "Z Data ", "NOK, TERMINATE");
        this->z.clear(false);
        return false;

    }

    z.remove_fake_coherencies();
    z.remove_fake_errors();





    return can_cont;
}

qint64 edi_file::read_a_coh_section()
{
    // >COH  MEAS1=1000.0001 MEAS2=1003.0001  ROT=NORTH  //42  that is mtx for example Ex vs Hy
    qint64 sp = this->qts.pos();

    QStringList strlst;
    QStringList data;
    std::vector<double> tmpv;
    this->line = this->line.replace(">COH", "");
    this->line = this->line.simplified();
    strlst = this->line.split(" ", Qt::SkipEmptyParts);
    for (auto &str : strlst) {
        str = str.simplified();
    }

    for (auto &str : strlst) {
        QStringList strmp = str.split("=");
        if (!(strmp.size() % 2)) data.append(strmp);
    }

    if (data.contains("MEAS1", Qt::CaseInsensitive) && data.contains("MEAS2", Qt::CaseInsensitive)) {
        QString one;
        QString two;

        for (int i = 0; i < data.size() -1; ++i) {
            if (!QString::compare(data.at(i), "meas1", Qt::CaseInsensitive)) {
                one = data.at(i+1);
            }
            else if (!QString::compare(data.at(i), "meas2", Qt::CaseInsensitive)) {
                two = data.at(i+1);
            }
            ++i;
        }

        size_t vone = 0;
        size_t vtwo = 0;

        for (size_t j = 0; j < this->in_ats_prc_coms.size(); ++j) {
            if (in_ats_prc_coms.at(j).svalue("string_id") == one) {
                vone = j;
            }
            else if (in_ats_prc_coms.at(j).svalue("string_id") == two) {
                vtwo = j;
            }
        }
        if ((vone != vtwo) ) {

            if ( ((in_ats_prc_coms.at(vone).svalue("channel_type") == "Ex") && (in_ats_prc_coms.at(vtwo).svalue("channel_type") == "Hy"))
                 ||  ((in_ats_prc_coms.at(vone).svalue("channel_type") == "Hy") && (in_ats_prc_coms.at(vtwo).svalue("channel_type") == "Ex")) )
            {
                if(this->read_number_block(tmpv)) {
                    this->z.coh[xy] = tmpv;
                }
            }

            if ( ((in_ats_prc_coms.at(vone).svalue("channel_type") == "Ey") && (in_ats_prc_coms.at(vtwo).svalue("channel_type") == "Hx"))
                 ||  ((in_ats_prc_coms.at(vone).svalue("channel_type") == "Hx") && (in_ats_prc_coms.at(vtwo).svalue("channel_type") == "Ey")) )
            {
                if(this->read_number_block(tmpv)) {
                    this->z.coh[yx] = tmpv;
                }
            }
        }



    }

    return sp;

}


bool edi_file::read_spectrasect()
{
    return false;
}

bool edi_file::seek_key(const QString skey, const bool b_continue, const bool silent)
{
    if (!this->is_open) return false;
    if (!b_continue) this->qts.seek(0);
    do  {
        this->line = this->qts.readLine();
        this->line = this->line.simplified();
        if (this->line.startsWith(skey)) return true;
    } while (!this->qts.atEnd());

    if (this->qts.status() == QTextStream::ReadPastEnd) {
        if(silent) {
            qDebug() << "section" << skey << "not found!";
            emit this->signal_general_msg("edifile", "read seek_key sction not found ", skey);
        }
    }
    qts.resetStatus();
    return false;

}

size_t edi_file::read_number_block(std::vector<double> &v)
{
    v.clear();
    QStringList strnums;
    do {
        this->line = this->qts.readLine();
        this->line = this->line.simplified();

        if (this->line.startsWith(">")) {
            return v.size();
        }
        else {
            strnums = this->line.split(" ");
            if (strnums.size()) {
                for (auto &str : strnums) {
                    bool ok = false;
                    double num = str.toDouble(&ok);
                    if (ok) v.push_back(num);
                }
            }
        }
    } while (strnums.size());
    this->qts.resetStatus();
    return v.size();
}


bool edi_file::write()
{
    if (!this->absoluteFilePath().size()) {

        qDebug() << "edi_file::write no file name set!";
        return false;
    }

    if (!this->z.freqs.size()) return false;

    // std::cerr << "write all" << Qt::endl;

    if (this->exists()) {
        QFileInfo qfi;
        int max = 1000;
        QString test = this->absoluteFilePath();
        test.chop(4);
        for (int i = 1; i < max; ++i) {
            QString isthere = test + "_" + QString::number(i) + ".edi";
            qfi.setFile(isthere);
            if (!qfi.exists()) break;
        }
        this->setFile(qfi.absoluteFilePath());


    }
    // std::cerr << "write all" << Qt::endl;

    emit this->signal_actual_edifile_name(this->absoluteFilePath());
    // std::cerr << "write fna" << Qt::endl;

    this->open_write();
    // std::cerr << "write open done" << std::endl;

    this->write_head();
    // std::cerr << "write header done" << std::endl;

    this->write_info();
    // std::cerr << "write info done" << std::endl;

    this->write_definemeas();
    // std::cerr << "write definemeas done" << std::endl;

    this->write_e_h_meas();
    // std::cerr << "write emeas done" << std::endl;

    this->write_mtsect();
    // std::cerr << "write mtsect donene" << std::endl;


    qts << ">END" << Qt::endl;
    this->file.close();
    this->qts.reset();
    this->is_open = false;
    return true;

}

bool edi_file::read()
{
    this->is_open = false;
    if (this->file.isOpen()) this->file.close();

    if (!this->absoluteFilePath().size()) {

        qDebug() << "edi_file::read no file name set!";
        return false;
    }
    if (!this->exists()) {

        qDebug() << "edi_file::not file exists!";
        return false;
    }


    this->file.setFileName(this->absoluteFilePath());

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream qts_tmp;
        this->qts_buf.clear();
        qts_tmp.setDevice(&this->file);
        this->qts_buf = qts_tmp.readAll();
        this->qts.setString(&this->qts_buf, QIODevice::ReadOnly | QIODevice::Text);

        this->file.close();
        this->is_open = true;

    }
    else {
        qDebug() << "can not open" << this->absoluteFilePath();
        this->file.close();
        this->qts.reset();
        this->qts_buf.clear();
        this->is_open = false;
        return false;
    }
    this->z.clear(true);

    if (!read_head() ){

        return false;
    }

    if (!read_info() ){

        return false;
    }

    if (!read_definemeas() ){

        return false;
    }

    if (!read_e_h_meas() ){

        return false;
    }

    if (!read_mtsect() ){

        return false;
    }

    //    if (!read_spectrasect() ){

    //        return false;
    //    }

    this->qts_buf.clear();
    this->qts.reset();
    this->line.clear();
    this->e_h_meas.clear();


    /*******************************************************************************/

    // to be done !
    //this->z.sort();


    /*******************************************************************************/

    this->z.backup();


    return true;
}

bool edi_file::open_write()
{

    if (this->file.isOpen()) this->file.close();
    this->is_open = false;

    this->file.setFileName(this->absoluteFilePath());
    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
        this->qts.setDevice(&this->file);
        this->qts.setRealNumberNotation(QTextStream::ScientificNotation);
        this->qts.setRealNumberPrecision(8);
        this->is_open = true;
        return true;
    }
    else {
        qDebug() << "can not open" << this->absoluteFilePath();
        this->file.close();
        this->qts.reset();
        return false;
    }

    return false;
}

edi_file merge_edi(const edi_file &lhs, const edi_file &rhs)
{

    edi_file edi;

    return edi;
}

std::shared_ptr<edi_file> merge_edi_sp(const std::shared_ptr<edi_file> &lhs, const std::shared_ptr<edi_file> &rhs)
{
        std::shared_ptr<edi_file> edi;

        return edi;
}
