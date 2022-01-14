#include "qstring_utilities.h"



QString round_digits(const int ndig_9_0, const QString &in)
{
    if (!in.contains(".")) return in;
    QStringList int_frac = in.split(QChar('.'));

    if (int_frac.size() != 2) return in;

    qint64 intpart = int_frac.at(0).toLongLong();


    QString frac_starts_with_9 = QString(ndig_9_0, QChar('9'));

    if (int_frac.at(1).startsWith(frac_starts_with_9) ){

        if (intpart > 0) intpart++;
        else intpart--;
        return QString::number(intpart) + QString(".0");

    }

    QString frac_starts_with_0 = QString(ndig_9_0, QChar('0'));

    if (int_frac.at(1).startsWith(frac_starts_with_0) ) {

        return QString::number(intpart) + QString(".0");
    }

    return in;
}

QString round_digits(const int ndig_9_0, const double &in) {

    QString decstr = QString::number(in, 'f', 8);
    return round_digits(ndig_9_0, decstr);
}

QStringList xmls_in_site(const QString &survey_base_dir, const QString &site_dir)
{
    QStringList measdocs;
    QDir survey_site_dir(survey_base_dir + "/ts/" + site_dir);
    survey_site_dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDot | QDir::NoDotDot);
    QStringList xmls;
    xmls << "*.xml" << "*.XML";
    survey_site_dir.setNameFilters(xmls);
    QDirIterator it(survey_site_dir, QDirIterator::Subdirectories);
    while(it.hasNext()) {
        measdocs.append(it.next());
    }

    measdocs.sort();
    return measdocs;
}

QList<QList<QFileInfo>> ats_in_site(const QString &survey_base_dir, const QString &site_dir)
{
    QList<QFileInfo> atsfiles;
    QList<QList<QFileInfo>> all_atsfiles;
    QDir survey_site_dir(survey_base_dir + "/ts/" + site_dir);
    survey_site_dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDot | QDir::NoDotDot);
    QStringList atss;
    atss << "*.ats" << "*.ats";
    survey_site_dir.setNameFilters(atss);
    QDirIterator it(survey_site_dir, QDirIterator::Subdirectories);
    while(it.hasNext()) {
        QFileInfo qfi(it.next());
        if (atsfiles.size()) {
            if (it.fileInfo().absolutePath().compare(atsfiles.back().absolutePath())) {
                all_atsfiles.append(atsfiles);
                atsfiles.clear();
            }
        }
        atsfiles.append(qfi);
    }

    if (atsfiles.size()) all_atsfiles.append(atsfiles);  // catch the last
    return all_atsfiles;
}

QStringList sites_in_survey_basedir(const QString &survey_base_dir)
{
    QStringList site_dirs;
    QDir survey_site_dir(survey_base_dir + "/ts");
    survey_site_dir.setFilter(QDir::Dirs | QDir::NoDot | QDir::NoDotDot);
    site_dirs = survey_site_dir.entryList();
    site_dirs.sort(Qt::CaseInsensitive);
    return site_dirs;
}

int xml_measdoc_overlap_time(QStringList &local, QStringList &remote, QList<qint64> &overlap_secs)
{
    QStringList l, r;
    QDateTime start_l, stop_l, start_r, stop_r;
    QString f_l, f_r;

    for (auto &lstr : local) {
        xml_measdoc_to_start_stop(lstr, start_l, stop_l, f_l);
        for (auto &rstr : remote) {
            xml_measdoc_to_start_stop(rstr, start_r, stop_r, f_r);

            // same freq ?
            if (f_l == f_r) {
                // check for overlap
                qint64 overlap = 0;
                qint64 overlap_start = 0;
                qint64 overlap_stop = 0;

                auto startsat = start_l.toSecsSinceEpoch();
                auto stopsat = stop_l.toSecsSinceEpoch();
                auto rrstartsat = start_r.toSecsSinceEpoch();
                auto rrstopsat = stop_r.toSecsSinceEpoch();

                // remote site is early but stops AFTER local site started, take local start time
                if (rrstartsat <= startsat && rrstopsat > startsat) {
                    overlap_start = startsat;
                }
                // remote site is late but start time is before local has stopped
                else if (rrstartsat > startsat && rrstartsat < stopsat) {
                    overlap_start = rrstartsat;
                }

                // remote sites stops later than local but was started BEFORE local
                if (rrstopsat >= stopsat && rrstartsat < stopsat) {
                    overlap_stop = stopsat;
                }
                // remote site stops earlier but stops AFTER local was started
                else if(rrstopsat < stopsat && rrstopsat > startsat) {
                    overlap_stop = rrstopsat;
                }

                overlap = overlap_stop - overlap_start;
                if (overlap > 1) {
                    l.append(lstr);
                    r.append(rstr);
                    overlap_secs.append(overlap);
                }

            }
        }
    }

    local = l;
    remote = r;
    return local.size();
}

void xml_measdoc_to_start_stop(const QString &measdoc, QDateTime &start, QDateTime &stop, QString &fstr)
{
    // 084_2009-08-20_13-22-00_2009-08-21_07-00-00_R001_128H.xml
    QFileInfo base(measdoc);
    QString mdoc(base.baseName());

    QStringList split = mdoc.split("_", Qt::SkipEmptyParts);
    if (split.size() != 7) return;
    QString start_date = split.at(1);
    QString start_time = split.at(2);
    QString stop_date = split.at(3);
    QString stop_time = split.at(4);

    fstr = split.at(6);
    //QDateTime QDateTime::fromString(const QString &string, const QString &format)
    start = QDateTime::fromString((start_date+"-"+start_time), "yyyy-MM-dd-HH-mm-ss");
    stop = QDateTime::fromString((stop_date+"-"+stop_time), "yyyy-MM-dd-HH-mm-ss");

}

QPair<double, QString> xml_measdoc_to_f_s(const QString &measdoc)
{
    QFileInfo base(measdoc);
    QString mdoc(base.baseName());
    QPair<double, QString> result;

    QStringList split = mdoc.split("_", Qt::SkipEmptyParts);
    if (split.size() != 7) QString("");
    QString sample_f_f = split.at(6);
    QString dbl = sample_f_f.left(sample_f_f.length() - 1);

    double f = dbl.toDouble();
    if (sample_f_f.endsWith("H", Qt::CaseInsensitive)) {

        sample_f_f.append("z");
        result.first = f;
        result.second = sample_f_f;
    }
    else if (sample_f_f.endsWith("s", Qt::CaseInsensitive)) {
        sample_f_f = sample_f_f.left(sample_f_f.length() - 1);
        sample_f_f.append("s");

        f = 1./f;
        result.first = f;
        result.second = sample_f_f;

    }

    return result;


}

QStringList measdocs_and_frequencies(const QStringList &allmeasdocs, const std::vector<double> &freqs)
{
    if (!allmeasdocs.size()) return QStringList();
    if (!freqs.size()) return QStringList();
    QStringList outdocs;
    int f_s;
    int a_f_s;

    for (auto &test : freqs) {
        if (test > 0.00) f_s = int(test);
        else f_s = int(1.0/test);

        for (auto &doc : allmeasdocs) {
            QPair<double, QString> fsp(xml_measdoc_to_f_s(doc));
            if (fsp.first > 0.00) a_f_s = int(test);
            else a_f_s = int(1.0/fsp.first);

            if (a_f_s == f_s) outdocs.append(doc);

        }


    }

    return outdocs;


}


QMap<QString, QVariant> atsfilename_values(const QString &filename)
{
    QMap<QString, QVariant> map;
    QFileInfo qfi(filename.trimmed());
    QStringList tokens = qfi.baseName().split("_");
    QString stmp;

    // 481_V01_C03_R000_THy_BL_2048H
    stmp = tokens[0];
    map.insert("aduser", stmp.mid(0).toInt());

    stmp = tokens[1];
    map.insert("version", stmp.mid(1).toInt());

    stmp = tokens[2];
    map.insert("channel", stmp.mid(1));

    stmp = tokens[3];
    map.insert("run", stmp.mid(1).toInt());

    stmp = tokens[4];
    map.insert("type", stmp.mid(1));

    stmp = tokens[5];
    map.insert("board", stmp.mid(1));

    stmp = tokens[6];
    map.insert("fname_sample_unit", tokens[6].right(1));

    stmp.chop(1);
    map.insert("fname_f_s", stmp.toDouble());

    if (!map.value("fname_sample_unit").toString().compare("H", Qt::CaseInsensitive)) {
        map.insert("sample_freq", map.value("fname_f_s").toDouble());
        map.insert("sample_period", 1.0/ map.value("fname_f_s").toDouble());
    }

    if (!map.value("fname_sample_unit").toString().compare("S", Qt::CaseInsensitive)) {
        map.insert("sample_freq", 1.0/ map.value("fname_f_s").toDouble());
        map.insert("sample_period", map.value("fname_f_s").toDouble());
    }

    return map;

}

QMap<QString, QVariant> atsfilename_site_survey(const QString &absloute_filename)
{

    QMap<QString, QVariant> map;
    if (!absloute_filename.size()) {
        return map;
    }
    QFileInfo qfi(absloute_filename.trimmed());
    if (absloute_filename.endsWith(".ats"), Qt::CaseInsensitive) {

        map = atsfilename_values(qfi.baseName());

    }

    QStringList tokens = qfi.absolutePath().split("/");

    size_t i = 0;
    for (auto str: tokens) {

        if (!str.compare("ts") && (i < tokens.size() - 2)) {
            map.insert("site", tokens.at(i + 1));
            map.insert("measdir", tokens.at(i + 2));
            if (i > 1) map.insert("survey", tokens.at(i - 1));
        }
        ++i;
    }

    if (map.contains("survey")) {
        QString path;
        for (auto str: tokens) {
            if (str.compare(map.value("survey").toString())) path.append(str + "/");
            else break;
        }
        path.chop(1);
        map.insert("survey_path", path);
    }

    return map;
}


