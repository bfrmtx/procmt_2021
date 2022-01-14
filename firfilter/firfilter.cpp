#include "firfilter.h"


firfilter::firfilter(QObject *parent) :  QObject(parent), prc_com(false)
{

    this->clear();

}

firfilter::firfilter(const firfilter &fil, QObject *parent) :  QObject(parent), prc_com(false)
{
    this->clear();
    this->operator =(fil);
}

firfilter::~firfilter()
{

}

void firfilter::copy_data(firfilter *fil)
{
    this->clear();
    prc_com::operator = (*fil);
}


firfilter &firfilter::operator = (const firfilter &fil)
{
    if(&fil == this) return *this;
    this->clear();
    prc_com::operator = (fil);
    return *this;

}

void firfilter::clear()
{
    this->coeff.clear();
    this->QMap<QString,QVariant>::clear();
    insert("filter_name", QString("empty"));
    insert("filter_type", QString("empty"));
    insert("filter_long_description", QString("empty"));
    insert("filter_category", QString("empty"));
    insert("filter_move", (quint64)0);
    insert("filter_length", (quint64)0);
    insert("filer_ouputs", (quint64)0);
    insert("sample_freq", (double)0.);
    insert("num_samples", (quint64)0);

    insert("upper_freq", (double)0.0);
    insert("lower_freq", (double)0.0);


    strs_opts << "mtx32" << "mtx8" << "mtx4" <<  "mtx2" << "mtx25" << "mtx10" << "mtx15"<< "notch" << "highpass" << "lowpass" << "bandpass";
    if (this->coeff.size()) this->coeff.clear();


}

void firfilter::clear_coefficients()
{
    this->coeff.clear();
}

double firfilter::fir_fold(const std::vector<double> &tsdata_in)
{
    double sum = 0;
    std::vector<double>::const_iterator coef_first = this->coeff.begin();
    std::vector<double>::const_iterator coef_last = this->coeff.end();
    std::vector<double>::const_iterator first = tsdata_in.begin();


    while (coef_first != coef_last) {
        sum  += ((*first) * (*coef_first));
        ++coef_first;
        ++first;
    }
    return sum;
}

bool firfilter::set_firfilter(const prc_com &filter_part)
{
    this->set_data(filter_part.get_data(), false);
    //    qDebug() << map_strs_opts.value("filter_name");
    //    qDebug() << svalue("filter_name");
    filter_part.show_my_map();
    if (!strs_opts.contains( svalue("filter_name")) ) {
        qDebug() << "firfilter::set_filter" << "filter does not exist";
        this->clear();
        return false;
    }
    return true;
}





std::vector<double>  firfilter::generate_coefficients(const QString &filter_name, bool &success, const bool create_coeff_vector,
                                                      const double sample_freq, const quint64 num_samples)
{
    size_t i;
    this->set_key_value("num_samples", num_samples);

    if (0 == filter_name.compare("mtx32")) {

        insert("filter_name", filter_name);
        insert("filter_type", QString("decimation"));
        insert("filter_long_description", QString("metronix 32x decimation FIR with Remez"));
        insert("filter_category", QString("FIR"));
        insert("filter_move", (quint64) 32);

        insert("filter_length", (quint64) 471);
        insert("filer_ouputs", (quint64)1);

        if ((this->quint64value("filter_length") > this->quint64value("num_samples") ) && this->quint64value("num_samples") ) {
            success = false;
            if (create_coeff_vector) return this->coeff;

        }
        else success = true;

        if (create_coeff_vector) {
            i = 0;
            coeff.reserve(471);
            coeff.resize(471);
            coeff[i++] =  7.636836537E-6;
            coeff[i++] = -4.584081140E-7;
            coeff[i++] = -6.420621773E-7;
            coeff[i++] = -9.585163552E-7;
            coeff[i++] = -1.406648604E-6;
            coeff[i++] = -1.998638145E-6;
            coeff[i++] = -2.738873598E-6;
            coeff[i++] = -3.644118238E-6;
            coeff[i++] = -4.722818807E-6;
            coeff[i++] = -5.995041318E-6;
            coeff[i++] = -7.472232203E-6;
            coeff[i++] = -9.176794514E-6;
            coeff[i++] = -1.112234516E-5;
            coeff[i++] = -1.333254225E-5;
            coeff[i++] = -1.582208887E-5;
            coeff[i++] = -1.861485657E-5;
            coeff[i++] = -2.173198454E-5;
            coeff[i++] = -2.516514786E-5;
            coeff[i++] = -2.898004296E-5;
            coeff[i++] = -3.317231449E-5;
            coeff[i++] = -3.775996159E-5;
            coeff[i++] = -4.276666661E-5;
            coeff[i++] = -4.820699838E-5;
            coeff[i++] = -5.410173248E-5;
            coeff[i++] = -6.046320172E-5;
            coeff[i++] = -6.730941714E-5;
            coeff[i++] = -7.465003370E-5;
            coeff[i++] = -8.249954407E-5;
            coeff[i++] = -9.086434157E-5;
            coeff[i++] = -9.975482472E-5;
            coeff[i++] = -1.091733763E-4;
            coeff[i++] = -1.191253534E-4;
            coeff[i++] = -1.296074562E-4;
            coeff[i++] = -1.406239993E-4;
            coeff[i++] = -1.521536162E-4;
            coeff[i++] = -1.641994695E-4;
            coeff[i++] = -1.767433788E-4;
            coeff[i++] = -1.897693608E-4;
            coeff[i++] = -2.032547074E-4;
            coeff[i++] = -2.171759590E-4;
            coeff[i++] = -2.315025723E-4;
            coeff[i++] = -2.462023283E-4;
            coeff[i++] = -2.612362165E-4;
            coeff[i++] = -2.765628091E-4;
            coeff[i++] = -2.921342447E-4;
            coeff[i++] = -3.078995543E-4;
            coeff[i++] = -3.238019452E-4;
            coeff[i++] = -3.397810303E-4;
            coeff[i++] = -3.557713869E-4;
            coeff[i++] = -3.717022806E-4;
            coeff[i++] = -3.875041326E-4;
            coeff[i++] = -4.030925790E-4;
            coeff[i++] = -4.183881624E-4;
            coeff[i++] = -4.333045055E-4;
            coeff[i++] = -4.477517782E-4;
            coeff[i++] = -4.616362436E-4;
            coeff[i++] = -4.748620065E-4;
            coeff[i++] = -4.873296221E-4;
            coeff[i++] = -4.989382902E-4;
            coeff[i++] = -5.095842466E-4;
            coeff[i++] = -5.191635197E-4;
            coeff[i++] = -5.275698387E-4;
            coeff[i++] = -5.346977357E-4;
            coeff[i++] = -5.404403178E-4;
            coeff[i++] = -5.446927481E-4;
            coeff[i++] = -5.473501339E-4;
            coeff[i++] = -5.483090804E-4;
            coeff[i++] = -5.474720373E-4;
            coeff[i++] = -5.447404020E-4;
            coeff[i++] = -5.400204594E-4;
            coeff[i++] = -5.332247422E-4;
            coeff[i++] = -5.242688413E-4;
            coeff[i++] = -5.130760420E-4;
            coeff[i++] = -4.995743979E-4;
            coeff[i++] = -4.837010952E-4;
            coeff[i++] = -4.653995222E-4;
            coeff[i++] = -4.446235738E-4;
            coeff[i++] = -4.213348542E-4;
            coeff[i++] = -3.955068689E-4;
            coeff[i++] = -3.671220976E-4;
            coeff[i++] = -3.361761343E-4;
            coeff[i++] = -3.026747054E-4;
            coeff[i++] = -2.666383911E-4;
            coeff[i++] = -2.280967805E-4;
            coeff[i++] = -1.870984044E-4;
            coeff[i++] = -1.437014503E-4;
            coeff[i++] = -9.798027317E-5;
            coeff[i++] = -5.002295223E-5;
            coeff[i++] = 6.650805861E-8;
            coeff[i++] = 5.216999048E-5;
            coeff[i++] = 1.061534694E-4;
            coeff[i++] = 1.618682996E-4;
            coeff[i++] = 2.191501347E-4;
            coeff[i++] = 2.778199969E-4;
            coeff[i++] = 3.376837961E-4;
            coeff[i++] = 3.985329065E-4;
            coeff[i++] = 4.601444760E-4;
            coeff[i++] = 5.222817499E-4;
            coeff[i++] = 5.846951505E-4;
            coeff[i++] = 6.471211557E-4;
            coeff[i++] = 7.092871295E-4;
            coeff[i++] = 7.709064997E-4;
            coeff[i++] = 8.316839987E-4;
            coeff[i++] = 8.913148587E-4;
            coeff[i++] = 9.494867270E-4;
            coeff[i++] = 1.005879618E-3;
            coeff[i++] = 1.060168711E-3;
            coeff[i++] = 1.112024067E-3;
            coeff[i++] = 1.161113718E-3;
            coeff[i++] = 1.207103285E-3;
            coeff[i++] = 1.249659522E-3;
            coeff[i++] = 1.288450003E-3;
            coeff[i++] = 1.323146621E-3;
            coeff[i++] = 1.353425440E-3;
            coeff[i++] = 1.378970268E-3;
            coeff[i++] = 1.399473034E-3;
            coeff[i++] = 1.414635734E-3;
            coeff[i++] = 1.424173640E-3;
            coeff[i++] = 1.427815957E-3;
            coeff[i++] = 1.425306899E-3;
            coeff[i++] = 1.416409765E-3;
            coeff[i++] = 1.400906398E-3;
            coeff[i++] = 1.378601223E-3;
            coeff[i++] = 1.349320687E-3;
            coeff[i++] = 1.312917368E-3;
            coeff[i++] = 1.269269289E-3;
            coeff[i++] = 1.218283797E-3;
            coeff[i++] = 1.159896754E-3;
            coeff[i++] = 1.094076440E-3;
            coeff[i++] = 1.020822503E-3;
            coeff[i++] = 9.401694729E-4;
            coeff[i++] = 8.521856064E-4;
            coeff[i++] = 7.569767290E-4;
            coeff[i++] = 6.546832637E-4;
            coeff[i++] = 5.454852368E-4;
            coeff[i++] = 4.295998542E-4;
            coeff[i++] = 3.072833495E-4;
            coeff[i++] = 1.788304330E-4;
            coeff[i++] = 4.457541058E-5;
            coeff[i++] = -9.510877341E-5;
            coeff[i++] = -2.398100744E-4;
            coeff[i++] = -3.890782669E-4;
            coeff[i++] = -5.424250214E-4;
            coeff[i++] = -6.993249817E-4;
            coeff[i++] = -8.592167665E-4;
            coeff[i++] = -1.021503690E-3;
            coeff[i++] = -1.185555638E-3;
            coeff[i++] = -1.350709832E-3;
            coeff[i++] = -1.516273716E-3;
            coeff[i++] = -1.681524623E-3;
            coeff[i++] = -1.845714934E-3;
            coeff[i++] = -2.008071371E-3;
            coeff[i++] = -2.167799162E-3;
            coeff[i++] = -2.324083450E-3;
            coeff[i++] = -2.476093196E-3;
            coeff[i++] = -2.622982610E-3;
            coeff[i++] = -2.763895500E-3;
            coeff[i++] = -2.897967000E-3;
            coeff[i++] = -3.024328216E-3;
            coeff[i++] = -3.142107998E-3;
            coeff[i++] = -3.250437738E-3;
            coeff[i++] = -3.348453605E-3;
            coeff[i++] = -3.435301325E-3;
            coeff[i++] = -3.510138398E-3;
            coeff[i++] = -3.572139058E-3;
            coeff[i++] = -3.620496926E-3;
            coeff[i++] = -3.654428959E-3;
            coeff[i++] = -3.673178894E-3;
            coeff[i++] = -3.676021889E-3;
            coeff[i++] = -3.662266157E-3;
            coeff[i++] = -3.631258445E-3;
            coeff[i++] = -3.582385690E-3;
            coeff[i++] = -3.515080065E-3;
            coeff[i++] = -3.428820433E-3;
            coeff[i++] = -3.323137079E-3;
            coeff[i++] = -3.197613170E-3;
            coeff[i++] = -3.051889060E-3;
            coeff[i++] = -2.885663125E-3;
            coeff[i++] = -2.698696085E-3;
            coeff[i++] = -2.490811485E-3;
            coeff[i++] = -2.261899363E-3;
            coeff[i++] = -2.011916230E-3;
            coeff[i++] = -1.740888906E-3;
            coeff[i++] = -1.448913226E-3;
            coeff[i++] = -1.136157251E-3;
            coeff[i++] = -8.028608895E-4;
            coeff[i++] = -4.493370356E-4;
            coeff[i++] = -7.597124000E-5;
            coeff[i++] = 3.167778112E-4;
            coeff[i++] = 7.283788660E-4;
            coeff[i++] = 1.158228692E-3;
            coeff[i++] = 1.605652920E-3;
            coeff[i++] = 2.069907434E-3;
            coeff[i++] = 2.550179259E-3;
            coeff[i++] = 3.045589330E-3;
            coeff[i++] = 3.555193420E-3;
            coeff[i++] = 4.077985536E-3;
            coeff[i++] = 4.612899392E-3;
            coeff[i++] = 5.158812763E-3;
            coeff[i++] = 5.714548608E-3;
            coeff[i++] = 6.278880471E-3;
            coeff[i++] = 6.850534598E-3;
            coeff[i++] = 7.428194815E-3;
            coeff[i++] = 8.010505412E-3;
            coeff[i++] = 8.596076712E-3;
            coeff[i++] = 9.183488074E-3;
            coeff[i++] = 9.771293872E-3;
            coeff[i++] = 1.035802663E-2;
            coeff[i++] = 1.094220357E-2;
            coeff[i++] = 1.152233001E-2;
            coeff[i++] = 1.209690556E-2;
            coeff[i++] = 1.266442813E-2;
            coeff[i++] = 1.322340035E-2;
            coeff[i++] = 1.377233339E-2;
            coeff[i++] = 1.430975326E-2;
            coeff[i++] = 1.483420529E-2;
            coeff[i++] = 1.534425967E-2;
            coeff[i++] = 1.583851574E-2;
            coeff[i++] = 1.631560838E-2;
            coeff[i++] = 1.677421125E-2;
            coeff[i++] = 1.721304312E-2;
            coeff[i++] = 1.763087094E-2;
            coeff[i++] = 1.802651596E-2;
            coeff[i++] = 1.839885639E-2;
            coeff[i++] = 1.874683295E-2;
            coeff[i++] = 1.906945131E-2;
            coeff[i++] = 1.936578751E-2;
            coeff[i++] = 1.963498933E-2;
            coeff[i++] = 1.987628129E-2;
            coeff[i++] = 2.008896602E-2;
            coeff[i++] = 2.027242813E-2;
            coeff[i++] = 2.042613519E-2;
            coeff[i++] = 2.054964111E-2;
            coeff[i++] = 2.064258657E-2;
            coeff[i++] = 2.070470105E-2;
            coeff[i++] = 2.073580358E-2;
            coeff[i++] = 2.073580358E-2;
            coeff[i++] = 2.070470105E-2;
            coeff[i++] = 2.064258657E-2;
            coeff[i++] = 2.054964111E-2;
            coeff[i++] = 2.042613519E-2;
            coeff[i++] = 2.027242813E-2;
            coeff[i++] = 2.008896602E-2;
            coeff[i++] = 1.987628129E-2;
            coeff[i++] = 1.963498933E-2;
            coeff[i++] = 1.936578751E-2;
            coeff[i++] = 1.906945131E-2;
            coeff[i++] = 1.874683295E-2;
            coeff[i++] = 1.839885639E-2;
            coeff[i++] = 1.802651596E-2;
            coeff[i++] = 1.763087094E-2;
            coeff[i++] = 1.721304312E-2;
            coeff[i++] = 1.677421125E-2;
            coeff[i++] = 1.631560838E-2;
            coeff[i++] = 1.583851574E-2;
            coeff[i++] = 1.534425967E-2;
            coeff[i++] = 1.483420529E-2;
            coeff[i++] = 1.430975326E-2;
            coeff[i++] = 1.377233339E-2;
            coeff[i++] = 1.322340035E-2;
            coeff[i++] = 1.266442813E-2;
            coeff[i++] = 1.209690556E-2;
            coeff[i++] = 1.152233001E-2;
            coeff[i++] = 1.094220357E-2;
            coeff[i++] = 1.035802663E-2;
            coeff[i++] = 9.771293872E-3;
            coeff[i++] = 9.183488074E-3;
            coeff[i++] = 8.596076712E-3;
            coeff[i++] = 8.010505412E-3;
            coeff[i++] = 7.428194815E-3;
            coeff[i++] = 6.850534598E-3;
            coeff[i++] = 6.278880471E-3;
            coeff[i++] = 5.714548608E-3;
            coeff[i++] = 5.158812763E-3;
            coeff[i++] = 4.612899392E-3;
            coeff[i++] = 4.077985536E-3;
            coeff[i++] = 3.555193420E-3;
            coeff[i++] = 3.045589330E-3;
            coeff[i++] = 2.550179259E-3;
            coeff[i++] = 2.069907434E-3;
            coeff[i++] = 1.605652920E-3;
            coeff[i++] = 1.158228692E-3;
            coeff[i++] = 7.283788660E-4;
            coeff[i++] = 3.167778112E-4;
            coeff[i++] = -7.597124000E-5;
            coeff[i++] = -4.493370356E-4;
            coeff[i++] = -8.028608895E-4;
            coeff[i++] = -1.136157251E-3;
            coeff[i++] = -1.448913226E-3;
            coeff[i++] = -1.740888906E-3;
            coeff[i++] = -2.011916230E-3;
            coeff[i++] = -2.261899363E-3;
            coeff[i++] = -2.490811485E-3;
            coeff[i++] = -2.698696085E-3;
            coeff[i++] = -2.885663125E-3;
            coeff[i++] = -3.051889060E-3;
            coeff[i++] = -3.197613170E-3;
            coeff[i++] = -3.323137079E-3;
            coeff[i++] = -3.428820433E-3;
            coeff[i++] = -3.515080065E-3;
            coeff[i++] = -3.582385690E-3;
            coeff[i++] = -3.631258445E-3;
            coeff[i++] = -3.662266157E-3;
            coeff[i++] = -3.676021889E-3;
            coeff[i++] = -3.673178894E-3;
            coeff[i++] = -3.654428959E-3;
            coeff[i++] = -3.620496926E-3;
            coeff[i++] = -3.572139058E-3;
            coeff[i++] = -3.510138398E-3;
            coeff[i++] = -3.435301325E-3;
            coeff[i++] = -3.348453605E-3;
            coeff[i++] = -3.250437738E-3;
            coeff[i++] = -3.142107998E-3;
            coeff[i++] = -3.024328216E-3;
            coeff[i++] = -2.897967000E-3;
            coeff[i++] = -2.763895500E-3;
            coeff[i++] = -2.622982610E-3;
            coeff[i++] = -2.476093196E-3;
            coeff[i++] = -2.324083450E-3;
            coeff[i++] = -2.167799162E-3;
            coeff[i++] = -2.008071371E-3;
            coeff[i++] = -1.845714934E-3;
            coeff[i++] = -1.681524623E-3;
            coeff[i++] = -1.516273716E-3;
            coeff[i++] = -1.350709832E-3;
            coeff[i++] = -1.185555638E-3;
            coeff[i++] = -1.021503690E-3;
            coeff[i++] = -8.592167665E-4;
            coeff[i++] = -6.993249817E-4;
            coeff[i++] = -5.424250214E-4;
            coeff[i++] = -3.890782669E-4;
            coeff[i++] = -2.398100744E-4;
            coeff[i++] = -9.510877341E-5;
            coeff[i++] = 4.457541058E-5;
            coeff[i++] = 1.788304330E-4;
            coeff[i++] = 3.072833495E-4;
            coeff[i++] = 4.295998542E-4;
            coeff[i++] = 5.454852368E-4;
            coeff[i++] = 6.546832637E-4;
            coeff[i++] = 7.569767290E-4;
            coeff[i++] = 8.521856064E-4;
            coeff[i++] = 9.401694729E-4;
            coeff[i++] = 1.020822503E-3;
            coeff[i++] = 1.094076440E-3;
            coeff[i++] = 1.159896754E-3;
            coeff[i++] = 1.218283797E-3;
            coeff[i++] = 1.269269289E-3;
            coeff[i++] = 1.312917368E-3;
            coeff[i++] = 1.349320687E-3;
            coeff[i++] = 1.378601223E-3;
            coeff[i++] = 1.400906398E-3;
            coeff[i++] = 1.416409765E-3;
            coeff[i++] = 1.425306899E-3;
            coeff[i++] = 1.427815957E-3;
            coeff[i++] = 1.424173640E-3;
            coeff[i++] = 1.414635734E-3;
            coeff[i++] = 1.399473034E-3;
            coeff[i++] = 1.378970268E-3;
            coeff[i++] = 1.353425440E-3;
            coeff[i++] = 1.323146621E-3;
            coeff[i++] = 1.288450003E-3;
            coeff[i++] = 1.249659522E-3;
            coeff[i++] = 1.207103285E-3;
            coeff[i++] = 1.161113718E-3;
            coeff[i++] = 1.112024067E-3;
            coeff[i++] = 1.060168711E-3;
            coeff[i++] = 1.005879618E-3;
            coeff[i++] = 9.494867270E-4;
            coeff[i++] = 8.913148587E-4;
            coeff[i++] = 8.316839987E-4;
            coeff[i++] = 7.709064997E-4;
            coeff[i++] = 7.092871295E-4;
            coeff[i++] = 6.471211557E-4;
            coeff[i++] = 5.846951505E-4;
            coeff[i++] = 5.222817499E-4;
            coeff[i++] = 4.601444760E-4;
            coeff[i++] = 3.985329065E-4;
            coeff[i++] = 3.376837961E-4;
            coeff[i++] = 2.778199969E-4;
            coeff[i++] = 2.191501347E-4;
            coeff[i++] = 1.618682996E-4;
            coeff[i++] = 1.061534694E-4;
            coeff[i++] = 5.216999048E-5;
            coeff[i++] = 6.650805861E-8;
            coeff[i++] = -5.002295223E-5;
            coeff[i++] = -9.798027317E-5;
            coeff[i++] = -1.437014503E-4;
            coeff[i++] = -1.870984044E-4;
            coeff[i++] = -2.280967805E-4;
            coeff[i++] = -2.666383911E-4;
            coeff[i++] = -3.026747054E-4;
            coeff[i++] = -3.361761343E-4;
            coeff[i++] = -3.671220976E-4;
            coeff[i++] = -3.955068689E-4;
            coeff[i++] = -4.213348542E-4;
            coeff[i++] = -4.446235738E-4;
            coeff[i++] = -4.653995222E-4;
            coeff[i++] = -4.837010952E-4;
            coeff[i++] = -4.995743979E-4;
            coeff[i++] = -5.130760420E-4;
            coeff[i++] = -5.242688413E-4;
            coeff[i++] = -5.332247422E-4;
            coeff[i++] = -5.400204594E-4;
            coeff[i++] = -5.447404020E-4;
            coeff[i++] = -5.474720373E-4;
            coeff[i++] = -5.483090804E-4;
            coeff[i++] = -5.473501339E-4;
            coeff[i++] = -5.446927481E-4;
            coeff[i++] = -5.404403178E-4;
            coeff[i++] = -5.346977357E-4;
            coeff[i++] = -5.275698387E-4;
            coeff[i++] = -5.191635197E-4;
            coeff[i++] = -5.095842466E-4;
            coeff[i++] = -4.989382902E-4;
            coeff[i++] = -4.873296221E-4;
            coeff[i++] = -4.748620065E-4;
            coeff[i++] = -4.616362436E-4;
            coeff[i++] = -4.477517782E-4;
            coeff[i++] = -4.333045055E-4;
            coeff[i++] = -4.183881624E-4;
            coeff[i++] = -4.030925790E-4;
            coeff[i++] = -3.875041326E-4;
            coeff[i++] = -3.717022806E-4;
            coeff[i++] = -3.557713869E-4;
            coeff[i++] = -3.397810303E-4;
            coeff[i++] = -3.238019452E-4;
            coeff[i++] = -3.078995543E-4;
            coeff[i++] = -2.921342447E-4;
            coeff[i++] = -2.765628091E-4;
            coeff[i++] = -2.612362165E-4;
            coeff[i++] = -2.462023283E-4;
            coeff[i++] = -2.315025723E-4;
            coeff[i++] = -2.171759590E-4;
            coeff[i++] = -2.032547074E-4;
            coeff[i++] = -1.897693608E-4;
            coeff[i++] = -1.767433788E-4;
            coeff[i++] = -1.641994695E-4;
            coeff[i++] = -1.521536162E-4;
            coeff[i++] = -1.406239993E-4;
            coeff[i++] = -1.296074562E-4;
            coeff[i++] = -1.191253534E-4;
            coeff[i++] = -1.091733763E-4;
            coeff[i++] = -9.975482472E-5;
            coeff[i++] = -9.086434157E-5;
            coeff[i++] = -8.249954407E-5;
            coeff[i++] = -7.465003370E-5;
            coeff[i++] = -6.730941714E-5;
            coeff[i++] = -6.046320172E-5;
            coeff[i++] = -5.410173248E-5;
            coeff[i++] = -4.820699838E-5;
            coeff[i++] = -4.276666661E-5;
            coeff[i++] = -3.775996159E-5;
            coeff[i++] = -3.317231449E-5;
            coeff[i++] = -2.898004296E-5;
            coeff[i++] = -2.516514786E-5;
            coeff[i++] = -2.173198454E-5;
            coeff[i++] = -1.861485657E-5;
            coeff[i++] = -1.582208887E-5;
            coeff[i++] = -1.333254225E-5;
            coeff[i++] = -1.112234516E-5;
            coeff[i++] = -9.176794514E-6;
            coeff[i++] = -7.472232203E-6;
            coeff[i++] = -5.995041318E-6;
            coeff[i++] = -4.722818807E-6;
            coeff[i++] = -3.644118238E-6;
            coeff[i++] = -2.738873598E-6;
            coeff[i++] = -1.998638145E-6;
            coeff[i++] = -1.406648604E-6;
            coeff[i++] = -9.585163552E-7;
            coeff[i++] = -6.420621773E-7;
            coeff[i++] = -4.584081140E-7;
        }

    }


    if (0 == filter_name.compare("mtx4")) {

        insert("filter_name", filter_name);
        insert("filter_type", QString("decimation"));
        insert("filter_long_description", QString("metronix 4x decimation FIR with Remez"));
        insert("filter_category", QString("FIR"));
        insert("filter_move", (quint64) 4);

        insert("filter_length", (quint64) 71);
        insert("filer_ouputs", (quint64)1);

        if ((this->quint64value("filter_length") > this->quint64value("num_samples") ) && this->quint64value("num_samples") ) {
            success = false;
            if (create_coeff_vector) return this->coeff;

        }
        else success = true;


        if (create_coeff_vector) {
            coeff.reserve(71);
            coeff.resize(71);
            i = 0;
            coeff[i++] = 6.854913864E-6;
            coeff[i++] = 2.933716833E-5;
            coeff[i++] = 8.267682092E-5;
            coeff[i++] = 1.822009396E-4;
            coeff[i++] = 3.346982688E-4;
            coeff[i++] = 5.253549867E-4;
            coeff[i++] = 7.050447753E-4;
            coeff[i++] = 7.848495715E-4;
            coeff[i++] = 6.456473839E-4;
            coeff[i++] = 1.680573886E-4;
            coeff[i++] = -7.184734519E-4;
            coeff[i++] = -1.978042517E-3;
            coeff[i++] = -3.421991913E-3;
            coeff[i++] = -4.698704960E-3;
            coeff[i++] = -5.340810348E-3;
            coeff[i++] = -4.877338474E-3;
            coeff[i++] = -2.996021341E-3;
            coeff[i++] = 2.840936053E-4;
            coeff[i++] = 4.488289189E-3;
            coeff[i++] = 8.671796886E-3;
            coeff[i++] = 1.155862537E-2;
            coeff[i++] = 1.183726918E-2;
            coeff[i++] = 8.564797604E-3;
            coeff[i++] = 1.581379992E-3;
            coeff[i++] = -8.191622918E-3;
            coeff[i++] = -1.869064668E-2;
            coeff[i++] = -2.696017386E-2;
            coeff[i++] = -2.970906692E-2;
            coeff[i++] = -2.407566432E-2;
            coeff[i++] = -8.426506187E-3;
            coeff[i++] = 1.702308969E-2;
            coeff[i++] = 4.995732286E-2;
            coeff[i++] = 8.618845263E-2;
            coeff[i++] = 1.203573989E-1;
            coeff[i++] = 1.469705098E-1;
            coeff[i++] = 1.615376929E-1;
            coeff[i++] = 1.615376929E-1;
            coeff[i++] = 1.469705098E-1;
            coeff[i++] = 1.203573989E-1;
            coeff[i++] = 8.618845263E-2;
            coeff[i++] = 4.995732286E-2;
            coeff[i++] = 1.702308969E-2;
            coeff[i++] = -8.426506187E-3;
            coeff[i++] = -2.407566432E-2;
            coeff[i++] = -2.970906692E-2;
            coeff[i++] = -2.696017386E-2;
            coeff[i++] = -1.869064668E-2;
            coeff[i++] = -8.191622918E-3;
            coeff[i++] = 1.581379992E-3;
            coeff[i++] = 8.564797604E-3;
            coeff[i++] = 1.183726918E-2;
            coeff[i++] = 1.155862537E-2;
            coeff[i++] = 8.671796886E-3;
            coeff[i++] = 4.488289189E-3;
            coeff[i++] = 2.840936053E-4;
            coeff[i++] = -2.996021341E-3;
            coeff[i++] = -4.877338474E-3;
            coeff[i++] = -5.340810348E-3;
            coeff[i++] = -4.698704960E-3;
            coeff[i++] = -3.421991913E-3;
            coeff[i++] = -1.978042517E-3;
            coeff[i++] = -7.184734519E-4;
            coeff[i++] = 1.680573886E-4;
            coeff[i++] = 6.456473839E-4;
            coeff[i++] = 7.848495715E-4;
            coeff[i++] = 7.050447753E-4;
            coeff[i++] = 5.253549867E-4;
            coeff[i++] = 3.346982688E-4;
            coeff[i++] = 1.822009396E-4;
            coeff[i++] = 8.267682092E-5;
            coeff[i++] = 2.933716833E-5;
        }

    }

    if (0 == filter_name.compare("mtx25")) {

        insert("filter_name", filter_name);
        insert("filter_type", QString("decimation"));
        insert("filter_long_description", QString("metronix 25x decimation FIR with Remez"));
        insert("filter_category", QString("FIR"));
        insert("filter_move", (quint64) 25);

        insert("filter_length", (quint64) 501);
        insert("filer_ouputs", (quint64)1);

        if ((this->quint64value("filter_length") > this->quint64value("num_samples") ) && this->quint64value("num_samples") ) {
            success = false;
            if (create_coeff_vector) return this->coeff;

        }
        else success = true;


        if (create_coeff_vector) {
            coeff.reserve(501);
            coeff.resize(501);
            i = 0;
            coeff[i++] = 6.033421866E-6;
            coeff[i++] = 2.676325810E-6;
            coeff[i++] = 3.222534925E-6;
            coeff[i++] = 3.827166873E-6;
            coeff[i++] = 4.453934884E-6;
            coeff[i++] = 5.124141231E-6;
            coeff[i++] = 5.800775889E-6;
            coeff[i++] = 6.505914843E-6;
            coeff[i++] = 7.197343949E-6;
            coeff[i++] = 7.884980426E-6;
            coeff[i++] = 8.516606921E-6;
            coeff[i++] = 9.114891746E-6;
            coeff[i++] = 9.625235634E-6;
            coeff[i++] = 1.003648676E-5;
            coeff[i++] = 1.031824274E-5;
            coeff[i++] = 1.044943685E-5;
            coeff[i++] = 1.038617157E-5;
            coeff[i++] = 1.011188154E-5;
            coeff[i++] = 9.572239634E-6;
            coeff[i++] = 8.752664909E-6;
            coeff[i++] = 7.595561728E-6;
            coeff[i++] = 6.081250392E-6;
            coeff[i++] = 4.150454235E-6;
            coeff[i++] = 1.789225480E-6;
            coeff[i++] = -1.065808633E-6;
            coeff[i++] = -4.430573845E-6;
            coeff[i++] = -8.359587903E-6;
            coeff[i++] = -1.287124785E-5;
            coeff[i++] = -1.801261028E-5;
            coeff[i++] = -2.379862795E-5;
            coeff[i++] = -3.027002047E-5;
            coeff[i++] = -3.743172185E-5;
            coeff[i++] = -4.531537798E-5;
            coeff[i++] = -5.391767489E-5;
            coeff[i++] = -6.325699327E-5;
            coeff[i++] = -7.331555427E-5;
            coeff[i++] = -8.410042835E-5;
            coeff[i++] = -9.557776617E-5;
            coeff[i++] = -1.077378938E-4;
            coeff[i++] = -1.205313165E-4;
            coeff[i++] = -1.339280209E-4;
            coeff[i++] = -1.478624139E-4;
            coeff[i++] = -1.622839197E-4;
            coeff[i++] = -1.771065961E-4;
            coeff[i++] = -1.922603218E-4;
            coeff[i++] = -2.076404218E-4;
            coeff[i++] = -2.231557200E-4;
            coeff[i++] = -2.386825730E-4;
            coeff[i++] = -2.541121312E-4;
            coeff[i++] = -2.693021876E-4;
            coeff[i++] = -2.841284830E-4;
            coeff[i++] = -2.984328933E-4;
            coeff[i++] = -3.120773912E-4;
            coeff[i++] = -3.248937697E-4;
            coeff[i++] = -3.367334631E-4;
            coeff[i++] = -3.474213494E-4;
            coeff[i++] = -3.568048631E-4;
            coeff[i++] = -3.647079355E-4;
            coeff[i++] = -3.709788142E-4;
            coeff[i++] = -3.754470898E-4;
            coeff[i++] = -3.779688976E-4;
            coeff[i++] = -3.783859922E-4;
            coeff[i++] = -3.765705658E-4;
            coeff[i++] = -3.723827379E-4;
            coeff[i++] = -3.657183535E-4;
            coeff[i++] = -3.564650030E-4;
            coeff[i++] = -3.445484294E-4;
            coeff[i++] = -3.298909654E-4;
            coeff[i++] = -3.124562161E-4;
            coeff[i++] = -2.922080968E-4;
            coeff[i++] = -2.691542843E-4;
            coeff[i++] = -2.433066376E-4;
            coeff[i++] = -2.147221058E-4;
            coeff[i++] = -1.834659442E-4;
            coeff[i++] = -1.496490141E-4;
            coeff[i++] = -1.133921552E-4;
            coeff[i++] = -7.486367851E-5;
            coeff[i++] = -3.424196288E-5;
            coeff[i++] = 8.247412141E-6;
            coeff[i++] = 5.236884961E-5;
            coeff[i++] = 9.784101759E-5;
            coeff[i++] = 1.443740374E-4;
            coeff[i++] = 1.916347771E-4;
            coeff[i++] = 2.392845909E-4;
            coeff[i++] = 2.869453901E-4;
            coeff[i++] = 3.342366967E-4;
            coeff[i++] = 3.807448877E-4;
            coeff[i++] = 4.260584841E-4;
            coeff[i++] = 4.697388935E-4;
            coeff[i++] = 5.113564642E-4;
            coeff[i++] = 5.504613603E-4;
            coeff[i++] = 5.866202820E-4;
            coeff[i++] = 6.193877286E-4;
            coeff[i++] = 6.483427819E-4;
            coeff[i++] = 6.730610289E-4;
            coeff[i++] = 6.931517916E-4;
            coeff[i++] = 7.082294153E-4;
            coeff[i++] = 7.179508008E-4;
            coeff[i++] = 7.219877825E-4;
            coeff[i++] = 7.200626478E-4;
            coeff[i++] = 7.119220394E-4;
            coeff[i++] = 6.973709513E-4;
            coeff[i++] = 6.762470510E-4;
            coeff[i++] = 6.484537892E-4;
            coeff[i++] = 6.139343821E-4;
            coeff[i++] = 5.727036117E-4;
            coeff[i++] = 5.248221025E-4;
            coeff[i++] = 4.704266009E-4;
            coeff[i++] = 4.097031683E-4;
            coeff[i++] = 3.429168574E-4;
            coeff[i++] = 2.703839955E-4;
            coeff[i++] = 1.924998527E-4;
            coeff[i++] = 1.097109919E-4;
            coeff[i++] = 2.254020501E-5;
            coeff[i++] = -6.844100913E-5;
            coeff[i++] = -1.625896783E-4;
            coeff[i++] = -2.592198443E-4;
            coeff[i++] = -3.575815770E-4;
            coeff[i++] = -4.568898156E-4;
            coeff[i++] = -5.563055942E-4;
            coeff[i++] = -6.549657827E-4;
            coeff[i++] = -7.519653048E-4;
            coeff[i++] = -8.463883441E-4;
            coeff[i++] = -9.372913391E-4;
            coeff[i++] = -1.023735145E-3;
            coeff[i++] = -1.104769398E-3;
            coeff[i++] = -1.179464486E-3;
            coeff[i++] = -1.246897229E-3;
            coeff[i++] = -1.306183083E-3;
            coeff[i++] = -1.356462282E-3;
            coeff[i++] = -1.396931670E-3;
            coeff[i++] = -1.426831634E-3;
            coeff[i++] = -1.445477047E-3;
            coeff[i++] = -1.452244350E-3;
            coeff[i++] = -1.446601729E-3;
            coeff[i++] = -1.428095363E-3;
            coeff[i++] = -1.396378986E-3;
            coeff[i++] = -1.351199147E-3;
            coeff[i++] = -1.292422892E-3;
            coeff[i++] = -1.220022383E-3;
            coeff[i++] = -1.134100498E-3;
            coeff[i++] = -1.034874080E-3;
            coeff[i++] = -9.226971497E-4;
            coeff[i++] = -7.980428757E-4;
            coeff[i++] = -6.615237753E-4;
            coeff[i++] = -5.138725193E-4;
            coeff[i++] = -3.559587663E-4;
            coeff[i++] = -1.887682440E-4;
            coeff[i++] = -1.341699361E-5;
            coeff[i++] = 1.688719162E-4;
            coeff[i++] = 3.567569569E-4;
            coeff[i++] = 5.488033375E-4;
            coeff[i++] = 7.434744001E-4;
            coeff[i++] = 9.391588386E-4;
            coeff[i++] = 1.134164614E-3;
            coeff[i++] = 1.326748102E-3;
            coeff[i++] = 1.515110304E-3;
            coeff[i++] = 1.697427268E-3;
            coeff[i++] = 1.871848875E-3;
            coeff[i++] = 2.036529976E-3;
            coeff[i++] = 2.189630828E-3;
            coeff[i++] = 2.329349171E-3;
            coeff[i++] = 2.453921567E-3;
            coeff[i++] = 2.561655869E-3;
            coeff[i++] = 2.650932956E-3;
            coeff[i++] = 2.720239007E-3;
            coeff[i++] = 2.768167089E-3;
            coeff[i++] = 2.793448797E-3;
            coeff[i++] = 2.794954825E-3;
            coeff[i++] = 2.771725254E-3;
            coeff[i++] = 2.722969136E-3;
            coeff[i++] = 2.648092202E-3;
            coeff[i++] = 2.546695009E-3;
            coeff[i++] = 2.418597710E-3;
            coeff[i++] = 2.263835998E-3;
            coeff[i++] = 2.082682612E-3;
            coeff[i++] = 1.875640462E-3;
            coeff[i++] = 1.643460332E-3;
            coeff[i++] = 1.387131025E-3;
            coeff[i++] = 1.107892930E-3;
            coeff[i++] = 8.072245142E-4;
            coeff[i++] = 4.868517704E-4;
            coeff[i++] = 1.487310630E-4;
            coeff[i++] = -2.049460614E-4;
            coeff[i++] = -5.717734569E-4;
            coeff[i++] = -9.491303179E-4;
            coeff[i++] = -1.334205093E-3;
            coeff[i++] = -1.723999814E-3;
            coeff[i++] = -2.115357462E-3;
            coeff[i++] = -2.504970297E-3;
            coeff[i++] = -2.889410113E-3;
            coeff[i++] = -3.265140259E-3;
            coeff[i++] = -3.628548543E-3;
            coeff[i++] = -3.975962028E-3;
            coeff[i++] = -4.303682165E-3;
            coeff[i++] = -4.608001668E-3;
            coeff[i++] = -4.885241117E-3;
            coeff[i++] = -5.131767357E-3;
            coeff[i++] = -5.344030530E-3;
            coeff[i++] = -5.518583172E-3;
            coeff[i++] = -5.652117097E-3;
            coeff[i++] = -5.741482169E-3;
            coeff[i++] = -5.783721971E-3;
            coeff[i++] = -5.776091629E-3;
            coeff[i++] = -5.716091302E-3;
            coeff[i++] = -5.601482126E-3;
            coeff[i++] = -5.430316754E-3;
            coeff[i++] = -5.200952413E-3;
            coeff[i++] = -4.912077878E-3;
            coeff[i++] = -4.562722931E-3;
            coeff[i++] = -4.152280771E-3;
            coeff[i++] = -3.680513387E-3;
            coeff[i++] = -3.147568876E-3;
            coeff[i++] = -2.553982101E-3;
            coeff[i++] = -1.900686349E-3;
            coeff[i++] = -1.189009041E-3;
            coeff[i++] = -4.206772836E-4;
            coeff[i++] = 4.021914379E-4;
            coeff[i++] = 1.277090101E-3;
            coeff[i++] = 2.201136628E-3;
            coeff[i++] = 3.171080776E-3;
            coeff[i++] = 4.183323892E-3;
            coeff[i++] = 5.233931659E-3;
            coeff[i++] = 6.318658779E-3;
            coeff[i++] = 7.432967122E-3;
            coeff[i++] = 8.572054943E-3;
            coeff[i++] = 9.730879853E-3;
            coeff[i++] = 1.090419199E-2;
            coeff[i++] = 1.208656106E-2;
            coeff[i++] = 1.327241254E-2;
            coeff[i++] = 1.445605819E-2;
            coeff[i++] = 1.563173433E-2;
            coeff[i++] = 1.679363471E-2;
            coeff[i++] = 1.793595004E-2;
            coeff[i++] = 1.905290209E-2;
            coeff[i++] = 2.013878354E-2;
            coeff[i++] = 2.118799213E-2;
            coeff[i++] = 2.219506970E-2;
            coeff[i++] = 2.315473541E-2;
            coeff[i++] = 2.406192299E-2;
            coeff[i++] = 2.491181170E-2;
            coeff[i++] = 2.569986079E-2;
            coeff[i++] = 2.642183753E-2;
            coeff[i++] = 2.707384757E-2;
            coeff[i++] = 2.765235921E-2;
            coeff[i++] = 2.815422883E-2;
            coeff[i++] = 2.857672063E-2;
            coeff[i++] = 2.891752642E-2;
            coeff[i++] = 2.917478012E-2;
            coeff[i++] = 2.934707129E-2;
            coeff[i++] = 2.943345408E-2;
            coeff[i++] = 2.943345408E-2;
            coeff[i++] = 2.934707129E-2;
            coeff[i++] = 2.917478012E-2;
            coeff[i++] = 2.891752642E-2;
            coeff[i++] = 2.857672063E-2;
            coeff[i++] = 2.815422883E-2;
            coeff[i++] = 2.765235921E-2;
            coeff[i++] = 2.707384757E-2;
            coeff[i++] = 2.642183753E-2;
            coeff[i++] = 2.569986079E-2;
            coeff[i++] = 2.491181170E-2;
            coeff[i++] = 2.406192299E-2;
            coeff[i++] = 2.315473541E-2;
            coeff[i++] = 2.219506970E-2;
            coeff[i++] = 2.118799213E-2;
            coeff[i++] = 2.013878354E-2;
            coeff[i++] = 1.905290209E-2;
            coeff[i++] = 1.793595004E-2;
            coeff[i++] = 1.679363471E-2;
            coeff[i++] = 1.563173433E-2;
            coeff[i++] = 1.445605819E-2;
            coeff[i++] = 1.327241254E-2;
            coeff[i++] = 1.208656106E-2;
            coeff[i++] = 1.090419199E-2;
            coeff[i++] = 9.730879853E-3;
            coeff[i++] = 8.572054943E-3;
            coeff[i++] = 7.432967122E-3;
            coeff[i++] = 6.318658779E-3;
            coeff[i++] = 5.233931659E-3;
            coeff[i++] = 4.183323892E-3;
            coeff[i++] = 3.171080776E-3;
            coeff[i++] = 2.201136628E-3;
            coeff[i++] = 1.277090101E-3;
            coeff[i++] = 4.021914379E-4;
            coeff[i++] = -4.206772836E-4;
            coeff[i++] = -1.189009041E-3;
            coeff[i++] = -1.900686349E-3;
            coeff[i++] = -2.553982101E-3;
            coeff[i++] = -3.147568876E-3;
            coeff[i++] = -3.680513387E-3;
            coeff[i++] = -4.152280771E-3;
            coeff[i++] = -4.562722931E-3;
            coeff[i++] = -4.912077878E-3;
            coeff[i++] = -5.200952413E-3;
            coeff[i++] = -5.430316754E-3;
            coeff[i++] = -5.601482126E-3;
            coeff[i++] = -5.716091302E-3;
            coeff[i++] = -5.776091629E-3;
            coeff[i++] = -5.783721971E-3;
            coeff[i++] = -5.741482169E-3;
            coeff[i++] = -5.652117097E-3;
            coeff[i++] = -5.518583172E-3;
            coeff[i++] = -5.344030530E-3;
            coeff[i++] = -5.131767357E-3;
            coeff[i++] = -4.885241117E-3;
            coeff[i++] = -4.608001668E-3;
            coeff[i++] = -4.303682165E-3;
            coeff[i++] = -3.975962028E-3;
            coeff[i++] = -3.628548543E-3;
            coeff[i++] = -3.265140259E-3;
            coeff[i++] = -2.889410113E-3;
            coeff[i++] = -2.504970297E-3;
            coeff[i++] = -2.115357462E-3;
            coeff[i++] = -1.723999814E-3;
            coeff[i++] = -1.334205093E-3;
            coeff[i++] = -9.491303179E-4;
            coeff[i++] = -5.717734569E-4;
            coeff[i++] = -2.049460614E-4;
            coeff[i++] = 1.487310630E-4;
            coeff[i++] = 4.868517704E-4;
            coeff[i++] = 8.072245142E-4;
            coeff[i++] = 1.107892930E-3;
            coeff[i++] = 1.387131025E-3;
            coeff[i++] = 1.643460332E-3;
            coeff[i++] = 1.875640462E-3;
            coeff[i++] = 2.082682612E-3;
            coeff[i++] = 2.263835998E-3;
            coeff[i++] = 2.418597710E-3;
            coeff[i++] = 2.546695009E-3;
            coeff[i++] = 2.648092202E-3;
            coeff[i++] = 2.722969136E-3;
            coeff[i++] = 2.771725254E-3;
            coeff[i++] = 2.794954825E-3;
            coeff[i++] = 2.793448797E-3;
            coeff[i++] = 2.768167089E-3;
            coeff[i++] = 2.720239007E-3;
            coeff[i++] = 2.650932956E-3;
            coeff[i++] = 2.561655869E-3;
            coeff[i++] = 2.453921567E-3;
            coeff[i++] = 2.329349171E-3;
            coeff[i++] = 2.189630828E-3;
            coeff[i++] = 2.036529976E-3;
            coeff[i++] = 1.871848875E-3;
            coeff[i++] = 1.697427268E-3;
            coeff[i++] = 1.515110304E-3;
            coeff[i++] = 1.326748102E-3;
            coeff[i++] = 1.134164614E-3;
            coeff[i++] = 9.391588386E-4;
            coeff[i++] = 7.434744001E-4;
            coeff[i++] = 5.488033375E-4;
            coeff[i++] = 3.567569569E-4;
            coeff[i++] = 1.688719162E-4;
            coeff[i++] = -1.341699361E-5;
            coeff[i++] = -1.887682440E-4;
            coeff[i++] = -3.559587663E-4;
            coeff[i++] = -5.138725193E-4;
            coeff[i++] = -6.615237753E-4;
            coeff[i++] = -7.980428757E-4;
            coeff[i++] = -9.226971497E-4;
            coeff[i++] = -1.034874080E-3;
            coeff[i++] = -1.134100498E-3;
            coeff[i++] = -1.220022383E-3;
            coeff[i++] = -1.292422892E-3;
            coeff[i++] = -1.351199147E-3;
            coeff[i++] = -1.396378986E-3;
            coeff[i++] = -1.428095363E-3;
            coeff[i++] = -1.446601729E-3;
            coeff[i++] = -1.452244350E-3;
            coeff[i++] = -1.445477047E-3;
            coeff[i++] = -1.426831634E-3;
            coeff[i++] = -1.396931670E-3;
            coeff[i++] = -1.356462282E-3;
            coeff[i++] = -1.306183083E-3;
            coeff[i++] = -1.246897229E-3;
            coeff[i++] = -1.179464486E-3;
            coeff[i++] = -1.104769398E-3;
            coeff[i++] = -1.023735145E-3;
            coeff[i++] = -9.372913391E-4;
            coeff[i++] = -8.463883441E-4;
            coeff[i++] = -7.519653048E-4;
            coeff[i++] = -6.549657827E-4;
            coeff[i++] = -5.563055942E-4;
            coeff[i++] = -4.568898156E-4;
            coeff[i++] = -3.575815770E-4;
            coeff[i++] = -2.592198443E-4;
            coeff[i++] = -1.625896783E-4;
            coeff[i++] = -6.844100913E-5;
            coeff[i++] = 2.254020501E-5;
            coeff[i++] = 1.097109919E-4;
            coeff[i++] = 1.924998527E-4;
            coeff[i++] = 2.703839955E-4;
            coeff[i++] = 3.429168574E-4;
            coeff[i++] = 4.097031683E-4;
            coeff[i++] = 4.704266009E-4;
            coeff[i++] = 5.248221025E-4;
            coeff[i++] = 5.727036117E-4;
            coeff[i++] = 6.139343821E-4;
            coeff[i++] = 6.484537892E-4;
            coeff[i++] = 6.762470510E-4;
            coeff[i++] = 6.973709513E-4;
            coeff[i++] = 7.119220394E-4;
            coeff[i++] = 7.200626478E-4;
            coeff[i++] = 7.219877825E-4;
            coeff[i++] = 7.179508008E-4;
            coeff[i++] = 7.082294153E-4;
            coeff[i++] = 6.931517916E-4;
            coeff[i++] = 6.730610289E-4;
            coeff[i++] = 6.483427819E-4;
            coeff[i++] = 6.193877286E-4;
            coeff[i++] = 5.866202820E-4;
            coeff[i++] = 5.504613603E-4;
            coeff[i++] = 5.113564642E-4;
            coeff[i++] = 4.697388935E-4;
            coeff[i++] = 4.260584841E-4;
            coeff[i++] = 3.807448877E-4;
            coeff[i++] = 3.342366967E-4;
            coeff[i++] = 2.869453901E-4;
            coeff[i++] = 2.392845909E-4;
            coeff[i++] = 1.916347771E-4;
            coeff[i++] = 1.443740374E-4;
            coeff[i++] = 9.784101759E-5;
            coeff[i++] = 5.236884961E-5;
            coeff[i++] = 8.247412141E-6;
            coeff[i++] = -3.424196288E-5;
            coeff[i++] = -7.486367851E-5;
            coeff[i++] = -1.133921552E-4;
            coeff[i++] = -1.496490141E-4;
            coeff[i++] = -1.834659442E-4;
            coeff[i++] = -2.147221058E-4;
            coeff[i++] = -2.433066376E-4;
            coeff[i++] = -2.691542843E-4;
            coeff[i++] = -2.922080968E-4;
            coeff[i++] = -3.124562161E-4;
            coeff[i++] = -3.298909654E-4;
            coeff[i++] = -3.445484294E-4;
            coeff[i++] = -3.564650030E-4;
            coeff[i++] = -3.657183535E-4;
            coeff[i++] = -3.723827379E-4;
            coeff[i++] = -3.765705658E-4;
            coeff[i++] = -3.783859922E-4;
            coeff[i++] = -3.779688976E-4;
            coeff[i++] = -3.754470898E-4;
            coeff[i++] = -3.709788142E-4;
            coeff[i++] = -3.647079355E-4;
            coeff[i++] = -3.568048631E-4;
            coeff[i++] = -3.474213494E-4;
            coeff[i++] = -3.367334631E-4;
            coeff[i++] = -3.248937697E-4;
            coeff[i++] = -3.120773912E-4;
            coeff[i++] = -2.984328933E-4;
            coeff[i++] = -2.841284830E-4;
            coeff[i++] = -2.693021876E-4;
            coeff[i++] = -2.541121312E-4;
            coeff[i++] = -2.386825730E-4;
            coeff[i++] = -2.231557200E-4;
            coeff[i++] = -2.076404218E-4;
            coeff[i++] = -1.922603218E-4;
            coeff[i++] = -1.771065961E-4;
            coeff[i++] = -1.622839197E-4;
            coeff[i++] = -1.478624139E-4;
            coeff[i++] = -1.339280209E-4;
            coeff[i++] = -1.205313165E-4;
            coeff[i++] = -1.077378938E-4;
            coeff[i++] = -9.557776617E-5;
            coeff[i++] = -8.410042835E-5;
            coeff[i++] = -7.331555427E-5;
            coeff[i++] = -6.325699327E-5;
            coeff[i++] = -5.391767489E-5;
            coeff[i++] = -4.531537798E-5;
            coeff[i++] = -3.743172185E-5;
            coeff[i++] = -3.027002047E-5;
            coeff[i++] = -2.379862795E-5;
            coeff[i++] = -1.801261028E-5;
            coeff[i++] = -1.287124785E-5;
            coeff[i++] = -8.359587903E-6;
            coeff[i++] = -4.430573845E-6;
            coeff[i++] = -1.065808633E-6;
            coeff[i++] = 1.789225480E-6;
            coeff[i++] = 4.150454235E-6;
            coeff[i++] = 6.081250392E-6;
            coeff[i++] = 7.595561728E-6;
            coeff[i++] = 8.752664909E-6;
            coeff[i++] = 9.572239634E-6;
            coeff[i++] = 1.011188154E-5;
            coeff[i++] = 1.038617157E-5;
            coeff[i++] = 1.044943685E-5;
            coeff[i++] = 1.031824274E-5;
            coeff[i++] = 1.003648676E-5;
            coeff[i++] = 9.625235634E-6;
            coeff[i++] = 9.114891746E-6;
            coeff[i++] = 8.516606921E-6;
            coeff[i++] = 7.884980426E-6;
            coeff[i++] = 7.197343949E-6;
            coeff[i++] = 6.505914843E-6;
            coeff[i++] = 5.800775889E-6;
            coeff[i++] = 5.124141231E-6;
            coeff[i++] = 4.453934884E-6;
            coeff[i++] = 3.827166873E-6;
            coeff[i++] = 3.222534925E-6;
            coeff[i++] = 2.676325810E-6;

        }

    }

    if (0 == filter_name.compare("mtx10")) {
        insert("filter_name", filter_name);
        insert("filter_type", QString("decimation"));
        insert("filter_long_description", QString("metronix 10x decimation FIR with Remez"));
        insert("filter_category", QString("FIR"));
        insert("filter_move", (quint64) 10);

        insert("filter_length", (quint64) 393);
        insert("filer_ouputs", (quint64)1);

        if ((this->quint64value("filter_length") > this->quint64value("num_samples") ) && this->quint64value("num_samples") ) {
            success = false;
            if (create_coeff_vector) return this->coeff;

        }
        else success = true;


        if (create_coeff_vector) {
            coeff.reserve(393);
            coeff.resize(393);
            i = 0;
            coeff[i++] = -1.114057940E-5;
            coeff[i++] = -1.444129841E-5;
            coeff[i++] = -2.297470495E-5;
            coeff[i++] = -3.393866207E-5;
            coeff[i++] = -4.743958790E-5;
            coeff[i++] = -6.333670051E-5;
            coeff[i++] = -8.130847425E-5;
            coeff[i++] = -1.007896848E-4;
            coeff[i++] = -1.209699380E-4;
            coeff[i++] = -1.407922617E-4;
            coeff[i++] = -1.589924939E-4;
            coeff[i++] = -1.741413068E-4;
            coeff[i++] = -1.847406816E-4;
            coeff[i++] = -1.892711434E-4;
            coeff[i++] = -1.863491144E-4;
            coeff[i++] = -1.747866579E-4;
            coeff[i++] = -1.537787593E-4;
            coeff[i++] = -1.229349899E-4;
            coeff[i++] = -8.244527563E-5;
            coeff[i++] = -3.306880900E-5;
            coeff[i++] = 2.375455913E-5;
            coeff[i++] = 8.599032591E-5;
            coeff[i++] = 1.509978917E-4;
            coeff[i++] = 2.157073833E-4;
            coeff[i++] = 2.767040651E-4;
            coeff[i++] = 3.304802918E-4;
            coeff[i++] = 3.735954954E-4;
            coeff[i++] = 4.029757111E-4;
            coeff[i++] = 4.160951762E-4;
            coeff[i++] = 4.112585693E-4;
            coeff[i++] = 3.877293576E-4;
            coeff[i++] = 3.459279419E-4;
            coeff[i++] = 2.874398259E-4;
            coeff[i++] = 2.150705552E-4;
            coeff[i++] = 1.326879104E-4;
            coeff[i++] = 4.510774295E-5;
            coeff[i++] = -4.223086636E-5;
            coeff[i++] = -1.235303354E-4;
            coeff[i++] = -1.930788419E-4;
            coeff[i++] = -2.455932075E-4;
            coeff[i++] = -2.766836245E-4;
            coeff[i++] = -2.831648541E-4;
            coeff[i++] = -2.634261508E-4;
            coeff[i++] = -2.175960293E-4;
            coeff[i++] = -1.477080772E-4;
            coeff[i++] = -5.762383608E-5;
            coeff[i++] = 4.707121965E-5;
            coeff[i++] = 1.594178138E-4;
            coeff[i++] = 2.714466064E-4;
            coeff[i++] = 3.747534038E-4;
            coeff[i++] = 4.610338517E-4;
            coeff[i++] = 5.227584800E-4;
            coeff[i++] = 5.537233304E-4;
            coeff[i++] = 5.496501642E-4;
            coeff[i++] = 5.085715806E-4;
            coeff[i++] = 4.311777147E-4;
            coeff[i++] = 3.208772988E-4;
            coeff[i++] = 1.837649983E-4;
            coeff[i++] = 2.828066789E-5;
            coeff[i++] = -1.352269577E-4;
            coeff[i++] = -2.952082303E-4;
            coeff[i++] = -4.396575304E-4;
            coeff[i++] = -5.570349849E-4;
            coeff[i++] = -6.371149797E-4;
            coeff[i++] = -6.718807346E-4;
            coeff[i++] = -6.562144113E-4;
            coeff[i++] = -5.885011239E-4;
            coeff[i++] = -4.709129602E-4;
            coeff[i++] = -3.095126798E-4;
            coeff[i++] = -1.139790011E-4;
            coeff[i++] = 1.028699456E-4;
            coeff[i++] = 3.259071059E-4;
            coeff[i++] = 5.386642414E-4;
            coeff[i++] = 7.245427500E-4;
            coeff[i++] = 8.680205753E-4;
            coeff[i++] = 9.559284097E-4;
            coeff[i++] = 9.785401633E-4;
            coeff[i++] = 9.305414350E-4;
            coeff[i++] = 8.116405061E-4;
            coeff[i++] = 6.269052445E-4;
            coeff[i++] = 3.866380263E-4;
            coeff[i++] = 1.059165513E-4;
            coeff[i++] = -1.963241634E-4;
            coeff[i++] = -4.984852996E-4;
            coeff[i++] = -7.778337863E-4;
            coeff[i++] = -1.012134378E-3;
            coeff[i++] = -1.181397832E-3;
            coeff[i++] = -1.269486789E-3;
            coeff[i++] = -1.265578078E-3;
            coeff[i++] = -1.165234837E-3;
            coeff[i++] = -9.711100951E-4;
            coeff[i++] = -6.930866007E-4;
            coeff[i++] = -3.479314304E-4;
            coeff[i++] = 4.164954303E-5;
            coeff[i++] = 4.484040927E-4;
            coeff[i++] = 8.423988114E-4;
            coeff[i++] = 1.193132858E-3;
            coeff[i++] = 1.471853997E-3;
            coeff[i++] = 1.653810647E-3;
            coeff[i++] = 1.720361686E-3;
            coeff[i++] = 1.660680808E-3;
            coeff[i++] = 1.472996878E-3;
            coeff[i++] = 1.165161463E-3;
            coeff[i++] = 7.545550915E-4;
            coeff[i++] = 2.672193260E-4;
            coeff[i++] = -2.636727965E-4;
            coeff[i++] = -8.000074864E-4;
            coeff[i++] = -1.301366448E-3;
            coeff[i++] = -1.728008252E-3;
            coeff[i++] = -2.043907738E-3;
            coeff[i++] = -2.219698340E-3;
            coeff[i++] = -2.235216854E-3;
            coeff[i++] = -2.081505473E-3;
            coeff[i++] = -1.762026150E-3;
            coeff[i++] = -1.293019479E-3;
            coeff[i++] = -7.028785418E-4;
            coeff[i++] = -3.059574288E-5;
            coeff[i++] = 6.767003896E-4;
            coeff[i++] = 1.366920586E-3;
            coeff[i++] = 1.986747682E-3;
            coeff[i++] = 2.485618603E-3;
            coeff[i++] = 2.819714538E-3;
            coeff[i++] = 2.955608123E-3;
            coeff[i++] = 2.873320900E-3;
            coeff[i++] = 2.568484937E-3;
            coeff[i++] = 2.053445184E-3;
            coeff[i++] = 1.357126076E-3;
            coeff[i++] = 5.236532254E-4;
            coeff[i++] = -3.902632984E-4;
            coeff[i++] = -1.319009416E-3;
            coeff[i++] = -2.192617648E-3;
            coeff[i++] = -2.941853502E-3;
            coeff[i++] = -3.503509044E-3;
            coeff[i++] = -3.825485514E-3;
            coeff[i++] = -3.871297895E-3;
            coeff[i++] = -3.623608201E-3;
            coeff[i++] = -3.086498373E-3;
            coeff[i++] = -2.286227349E-3;
            coeff[i++] = -1.270363918E-3;
            coeff[i++] = -1.052598865E-4;
            coeff[i++] = 1.128007405E-3;
            coeff[i++] = 2.339013551E-3;
            coeff[i++] = 3.434329764E-3;
            coeff[i++] = 4.324420349E-3;
            coeff[i++] = 4.930590668E-3;
            coeff[i++] = 5.191484849E-3;
            coeff[i++] = 5.068612743E-3;
            coeff[i++] = 4.550458702E-3;
            coeff[i++] = 3.654784459E-3;
            coeff[i++] = 2.428874007E-3;
            coeff[i++] = 9.475889920E-4;
            coeff[i++] = -6.907258309E-4;
            coeff[i++] = -2.370302534E-3;
            coeff[i++] = -3.965624746E-3;
            coeff[i++] = -5.350243574E-3;
            coeff[i++] = -6.406143787E-3;
            coeff[i++] = -7.033010646E-3;
            coeff[i++] = -7.156713003E-3;
            coeff[i++] = -6.736350059E-3;
            coeff[i++] = -5.769271520E-3;
            coeff[i++] = -4.293604532E-3;
            coeff[i++] = -2.387968275E-3;
            coeff[i++] = -1.682464488E-4;
            coeff[i++] = 2.218515378E-3;
            coeff[i++] = 4.602812184E-3;
            coeff[i++] = 6.803512827E-3;
            coeff[i++] = 8.640184464E-3;
            coeff[i++] = 9.946133530E-3;
            coeff[i++] = 1.058130483E-2;
            coeff[i++] = 1.044414685E-2;
            coeff[i++] = 9.481584099E-3;
            coeff[i++] = 7.696322510E-3;
            coeff[i++] = 5.150862472E-3;
            coeff[i++] = 1.967783084E-3;
            coeff[i++] = -1.673906463E-3;
            coeff[i++] = -5.546304040E-3;
            coeff[i++] = -9.383735975E-3;
            coeff[i++] = -1.289691468E-2;
            coeff[i++] = -1.578936397E-2;
            coeff[i++] = -1.777517081E-2;
            coeff[i++] = -1.859700035E-2;
            coeff[i++] = -1.804326055E-2;
            coeff[i++] = -1.596331600E-2;
            coeff[i++] = -1.227974319E-2;
            coeff[i++] = -6.996769670E-3;
            coeff[i++] = -2.042587981E-4;
            coeff[i++] = 7.923144175E-3;
            coeff[i++] = 1.713179368E-2;
            coeff[i++] = 2.709954590E-2;
            coeff[i++] = 3.745047541E-2;
            coeff[i++] = 4.777303712E-2;
            coeff[i++] = 5.764067713E-2;
            coeff[i++] = 6.663373389E-2;
            coeff[i++] = 7.436137476E-2;
            coeff[i++] = 8.048229603E-2;
            coeff[i++] = 8.472297504E-2;
            coeff[i++] = 8.689239905E-2;
            coeff[i++] = 8.689239905E-2;
            coeff[i++] = 8.472297504E-2;
            coeff[i++] = 8.048229603E-2;
            coeff[i++] = 7.436137476E-2;
            coeff[i++] = 6.663373389E-2;
            coeff[i++] = 5.764067713E-2;
            coeff[i++] = 4.777303712E-2;
            coeff[i++] = 3.745047541E-2;
            coeff[i++] = 2.709954590E-2;
            coeff[i++] = 1.713179368E-2;
            coeff[i++] = 7.923144175E-3;
            coeff[i++] = -2.042587981E-4;
            coeff[i++] = -6.996769670E-3;
            coeff[i++] = -1.227974319E-2;
            coeff[i++] = -1.596331600E-2;
            coeff[i++] = -1.804326055E-2;
            coeff[i++] = -1.859700035E-2;
            coeff[i++] = -1.777517081E-2;
            coeff[i++] = -1.578936397E-2;
            coeff[i++] = -1.289691468E-2;
            coeff[i++] = -9.383735975E-3;
            coeff[i++] = -5.546304040E-3;
            coeff[i++] = -1.673906463E-3;
            coeff[i++] = 1.967783084E-3;
            coeff[i++] = 5.150862472E-3;
            coeff[i++] = 7.696322510E-3;
            coeff[i++] = 9.481584099E-3;
            coeff[i++] = 1.044414685E-2;
            coeff[i++] = 1.058130483E-2;
            coeff[i++] = 9.946133530E-3;
            coeff[i++] = 8.640184464E-3;
            coeff[i++] = 6.803512827E-3;
            coeff[i++] = 4.602812184E-3;
            coeff[i++] = 2.218515378E-3;
            coeff[i++] = -1.682464488E-4;
            coeff[i++] = -2.387968275E-3;
            coeff[i++] = -4.293604532E-3;
            coeff[i++] = -5.769271520E-3;
            coeff[i++] = -6.736350059E-3;
            coeff[i++] = -7.156713003E-3;
            coeff[i++] = -7.033010646E-3;
            coeff[i++] = -6.406143787E-3;
            coeff[i++] = -5.350243574E-3;
            coeff[i++] = -3.965624746E-3;
            coeff[i++] = -2.370302534E-3;
            coeff[i++] = -6.907258309E-4;
            coeff[i++] = 9.475889920E-4;
            coeff[i++] = 2.428874007E-3;
            coeff[i++] = 3.654784459E-3;
            coeff[i++] = 4.550458702E-3;
            coeff[i++] = 5.068612743E-3;
            coeff[i++] = 5.191484849E-3;
            coeff[i++] = 4.930590668E-3;
            coeff[i++] = 4.324420349E-3;
            coeff[i++] = 3.434329764E-3;
            coeff[i++] = 2.339013551E-3;
            coeff[i++] = 1.128007405E-3;
            coeff[i++] = -1.052598865E-4;
            coeff[i++] = -1.270363918E-3;
            coeff[i++] = -2.286227349E-3;
            coeff[i++] = -3.086498373E-3;
            coeff[i++] = -3.623608201E-3;
            coeff[i++] = -3.871297895E-3;
            coeff[i++] = -3.825485514E-3;
            coeff[i++] = -3.503509044E-3;
            coeff[i++] = -2.941853502E-3;
            coeff[i++] = -2.192617648E-3;
            coeff[i++] = -1.319009416E-3;
            coeff[i++] = -3.902632984E-4;
            coeff[i++] = 5.236532254E-4;
            coeff[i++] = 1.357126076E-3;
            coeff[i++] = 2.053445184E-3;
            coeff[i++] = 2.568484937E-3;
            coeff[i++] = 2.873320900E-3;
            coeff[i++] = 2.955608123E-3;
            coeff[i++] = 2.819714538E-3;
            coeff[i++] = 2.485618603E-3;
            coeff[i++] = 1.986747682E-3;
            coeff[i++] = 1.366920586E-3;
            coeff[i++] = 6.767003896E-4;
            coeff[i++] = -3.059574288E-5;
            coeff[i++] = -7.028785418E-4;
            coeff[i++] = -1.293019479E-3;
            coeff[i++] = -1.762026150E-3;
            coeff[i++] = -2.081505473E-3;
            coeff[i++] = -2.235216854E-3;
            coeff[i++] = -2.219698340E-3;
            coeff[i++] = -2.043907738E-3;
            coeff[i++] = -1.728008252E-3;
            coeff[i++] = -1.301366448E-3;
            coeff[i++] = -8.000074864E-4;
            coeff[i++] = -2.636727965E-4;
            coeff[i++] = 2.672193260E-4;
            coeff[i++] = 7.545550915E-4;
            coeff[i++] = 1.165161463E-3;
            coeff[i++] = 1.472996878E-3;
            coeff[i++] = 1.660680808E-3;
            coeff[i++] = 1.720361686E-3;
            coeff[i++] = 1.653810647E-3;
            coeff[i++] = 1.471853997E-3;
            coeff[i++] = 1.193132858E-3;
            coeff[i++] = 8.423988114E-4;
            coeff[i++] = 4.484040927E-4;
            coeff[i++] = 4.164954303E-5;
            coeff[i++] = -3.479314304E-4;
            coeff[i++] = -6.930866007E-4;
            coeff[i++] = -9.711100951E-4;
            coeff[i++] = -1.165234837E-3;
            coeff[i++] = -1.265578078E-3;
            coeff[i++] = -1.269486789E-3;
            coeff[i++] = -1.181397832E-3;
            coeff[i++] = -1.012134378E-3;
            coeff[i++] = -7.778337863E-4;
            coeff[i++] = -4.984852996E-4;
            coeff[i++] = -1.963241634E-4;
            coeff[i++] = 1.059165513E-4;
            coeff[i++] = 3.866380263E-4;
            coeff[i++] = 6.269052445E-4;
            coeff[i++] = 8.116405061E-4;
            coeff[i++] = 9.305414350E-4;
            coeff[i++] = 9.785401633E-4;
            coeff[i++] = 9.559284097E-4;
            coeff[i++] = 8.680205753E-4;
            coeff[i++] = 7.245427500E-4;
            coeff[i++] = 5.386642414E-4;
            coeff[i++] = 3.259071059E-4;
            coeff[i++] = 1.028699456E-4;
            coeff[i++] = -1.139790011E-4;
            coeff[i++] = -3.095126798E-4;
            coeff[i++] = -4.709129602E-4;
            coeff[i++] = -5.885011239E-4;
            coeff[i++] = -6.562144113E-4;
            coeff[i++] = -6.718807346E-4;
            coeff[i++] = -6.371149797E-4;
            coeff[i++] = -5.570349849E-4;
            coeff[i++] = -4.396575304E-4;
            coeff[i++] = -2.952082303E-4;
            coeff[i++] = -1.352269577E-4;
            coeff[i++] = 2.828066789E-5;
            coeff[i++] = 1.837649983E-4;
            coeff[i++] = 3.208772988E-4;
            coeff[i++] = 4.311777147E-4;
            coeff[i++] = 5.085715806E-4;
            coeff[i++] = 5.496501642E-4;
            coeff[i++] = 5.537233304E-4;
            coeff[i++] = 5.227584800E-4;
            coeff[i++] = 4.610338517E-4;
            coeff[i++] = 3.747534038E-4;
            coeff[i++] = 2.714466064E-4;
            coeff[i++] = 1.594178138E-4;
            coeff[i++] = 4.707121965E-5;
            coeff[i++] = -5.762383608E-5;
            coeff[i++] = -1.477080772E-4;
            coeff[i++] = -2.175960293E-4;
            coeff[i++] = -2.634261508E-4;
            coeff[i++] = -2.831648541E-4;
            coeff[i++] = -2.766836245E-4;
            coeff[i++] = -2.455932075E-4;
            coeff[i++] = -1.930788419E-4;
            coeff[i++] = -1.235303354E-4;
            coeff[i++] = -4.223086636E-5;
            coeff[i++] = 4.510774295E-5;
            coeff[i++] = 1.326879104E-4;
            coeff[i++] = 2.150705552E-4;
            coeff[i++] = 2.874398259E-4;
            coeff[i++] = 3.459279419E-4;
            coeff[i++] = 3.877293576E-4;
            coeff[i++] = 4.112585693E-4;
            coeff[i++] = 4.160951762E-4;
            coeff[i++] = 4.029757111E-4;
            coeff[i++] = 3.735954954E-4;
            coeff[i++] = 3.304802918E-4;
            coeff[i++] = 2.767040651E-4;
            coeff[i++] = 2.157073833E-4;
            coeff[i++] = 1.509978917E-4;
            coeff[i++] = 8.599032591E-5;
            coeff[i++] = 2.375455913E-5;
            coeff[i++] = -3.306880900E-5;
            coeff[i++] = -8.244527563E-5;
            coeff[i++] = -1.229349899E-4;
            coeff[i++] = -1.537787593E-4;
            coeff[i++] = -1.747866579E-4;
            coeff[i++] = -1.863491144E-4;
            coeff[i++] = -1.892711434E-4;
            coeff[i++] = -1.847406816E-4;
            coeff[i++] = -1.741413068E-4;
            coeff[i++] = -1.589924939E-4;
            coeff[i++] = -1.407922617E-4;
            coeff[i++] = -1.209699380E-4;
            coeff[i++] = -1.007896848E-4;
            coeff[i++] = -8.130847425E-5;
            coeff[i++] = -6.333670051E-5;
            coeff[i++] = -4.743958790E-5;
            coeff[i++] = -3.393866207E-5;
            coeff[i++] = -2.297470495E-5;
            coeff[i++] = -1.444129841E-5;
        }


    }

    if (0 == filter_name.compare("mtx2")) {
        insert("filter_name", filter_name);
        insert("filter_type", QString("decimation"));
        insert("filter_long_description", QString("metronix 2x decimation FIR with Remez"));
        insert("filter_category", QString("FIR"));
        insert("filter_move", (quint64) 2);

        insert("filter_length", (quint64) 35);
        insert("filer_ouputs", (quint64)1);
        if ((this->quint64value("filter_length") > this->quint64value("num_samples") ) && this->quint64value("num_samples") ) {
            success = false;
            if (create_coeff_vector) return this->coeff;

        }
        else success = true;


        if (create_coeff_vector) {
            coeff.reserve(35);
            coeff.resize(35);
            i = 0;
            coeff[i++] = -1.372521126E-4;
            coeff[i++] = -6.667804822E-4;
            coeff[i++] = -1.186883027E-3;
            coeff[i++] = -3.696243078E-4;
            coeff[i++] = 2.433044326E-3;
            coeff[i++] = 4.527364971E-3;
            coeff[i++] = 1.082752294E-3;
            coeff[i++] = -7.812171581E-3;
            coeff[i++] = -1.213328444E-2;
            coeff[i++] = -4.843352987E-4;
            coeff[i++] = 2.144382608E-2;
            coeff[i++] = 2.653128881E-2;
            coeff[i++] = -6.473890927E-3;
            coeff[i++] = -5.546122574E-2;
            coeff[i++] = -5.601629060E-2;
            coeff[i++] = 4.114742442E-2;
            coeff[i++] = 2.065781657E-1;
            coeff[i++] = 3.365394460E-1;
            coeff[i++] = 3.365394460E-1;
            coeff[i++] = 2.065781657E-1;
            coeff[i++] = 4.114742442E-2;
            coeff[i++] = -5.601629060E-2;
            coeff[i++] = -5.546122574E-2;
            coeff[i++] = -6.473890927E-3;
            coeff[i++] = 2.653128881E-2;
            coeff[i++] = 2.144382608E-2;
            coeff[i++] = -4.843352987E-4;
            coeff[i++] = -1.213328444E-2;
            coeff[i++] = -7.812171581E-3;
            coeff[i++] = 1.082752294E-3;
            coeff[i++] = 4.527364971E-3;
            coeff[i++] = 2.433044326E-3;
            coeff[i++] = -3.696243078E-4;
            coeff[i++] = -1.186883027E-3;
            coeff[i++] = -6.667804822E-4;

        }
    }
    if (0 == filter_name.compare("mtx15")) {
        insert("filter_name", filter_name);
        insert("filter_type", QString("decimation"));
        insert("filter_long_description", QString("metronix 15x decimation FIR with Remez"));
        insert("filter_category", QString("FIR"));
        insert("filter_move", (quint64) 15);

        insert("filter_length", (quint64) 193);
        insert("filer_ouputs", (quint64)1);
        if ((this->quint64value("filter_length") > this->quint64value("num_samples") ) && this->quint64value("num_samples") ) {
            success = false;
            if (create_coeff_vector) return this->coeff;

        }
        else success = true;


        if (create_coeff_vector) {
            coeff.reserve(193);
            coeff.resize(193);
            i = 0;
            coeff[i++] = -4.044646388326210E-05;
            coeff[i++] = -7.125357296762690E-05;
            coeff[i++] = -1.182903064715630E-04;
            coeff[i++] = -1.657729992539940E-04;
            coeff[i++] = -1.955377334965590E-04;
            coeff[i++] = -1.802091235321370E-04;
            coeff[i++] = -8.417172290272210E-05;
            coeff[i++] = 1.333334565741640E-04;
            coeff[i++] = 5.129075623129110E-04;
            coeff[i++] = 1.087727918046930E-03;
            coeff[i++] = 1.875193204599780E-03;
            coeff[i++] = 2.868711760546310E-03;
            coeff[i++] = 4.031186746077420E-03;
            coeff[i++] = 5.291926934972190E-03;
            coeff[i++] = 6.548246254406420E-03;
            coeff[i++] = 7.672793091555160E-03;
            coeff[i++] = 8.526434540132850E-03;
            coeff[i++] = 8.975823153750200E-03;
            coeff[i++] = 8.913496585899270E-03;
            coeff[i++] = 8.277693502109870E-03;
            coeff[i++] = 7.068489814377620E-03;
            coeff[i++] = 5.356970772465930E-03;
            coeff[i++] = 3.284841280731090E-03;
            coeff[i++] = 1.053101801720310E-03;
            coeff[i++] = -1.099885972970160E-03;
            coeff[i++] = -2.928740590104950E-03;
            coeff[i++] = -4.215886926156480E-03;
            coeff[i++] = -4.805640936145330E-03;
            coeff[i++] = -4.631905455394540E-03;
            coeff[i++] = -3.734197806456200E-03;
            coeff[i++] = -2.258231438950800E-03;
            coeff[i++] = -4.394865114783210E-04;
            coeff[i++] = 1.428954472036590E-03;
            coeff[i++] = 3.040216331443130E-03;
            coeff[i++] = 4.122593934438420E-03;
            coeff[i++] = 4.485705627354110E-03;
            coeff[i++] = 4.055701844895560E-03;
            coeff[i++] = 2.892660246535770E-03;
            coeff[i++] = 1.185923890154930E-03;
            coeff[i++] = -7.732648479493490E-04;
            coeff[i++] = -2.638494700700310E-03;
            coeff[i++] = -4.067209226948190E-03;
            coeff[i++] = -4.782252818150890E-03;
            coeff[i++] = -4.625008979640440E-03;
            coeff[i++] = -3.590008824981640E-03;
            coeff[i++] = -1.833573088156330E-03;
            coeff[i++] = 3.466769811991810E-04;
            coeff[i++] = 2.559468804386510E-03;
            coeff[i++] = 4.387574950239950E-03;
            coeff[i++] = 5.464261593874140E-03;
            coeff[i++] = 5.544834591166300E-03;
            coeff[i++] = 4.559640695342950E-03;
            coeff[i++] = 2.637460054145680E-03;
            coeff[i++] = 9.302479206313130E-05;
            coeff[i++] = -2.621285067667250E-03;
            coeff[i++] = -4.992796989421140E-03;
            coeff[i++] = -6.542270459946900E-03;
            coeff[i++] = -6.917390059145280E-03;
            coeff[i++] = -5.968364442408860E-03;
            coeff[i++] = -3.790136040710690E-03;
            coeff[i++] = -7.209459484446220E-04;
            coeff[i++] = 2.705479425549910E-03;
            coeff[i++] = 5.848296349033310E-03;
            coeff[i++] = 8.074594038494410E-03;
            coeff[i++] = 8.881291057799100E-03;
            coeff[i++] = 8.001329864149060E-03;
            coeff[i++] = 5.472995989090840E-03;
            coeff[i++] = 1.656529954957150E-03;
            coeff[i++] = -2.809043628297740E-03;
            coeff[i++] = -7.105333537665020E-03;
            coeff[i++] = -1.037594957051190E-02;
            coeff[i++] = -1.188669538449200E-02;
            coeff[i++] = -1.117626261202950E-02;
            coeff[i++] = -8.168701686096920E-03;
            coeff[i++] = -3.223761544739780E-03;
            coeff[i++] = 2.889323396409480E-03;
            coeff[i++] = 9.096268371124420E-03;
            coeff[i++] = 1.418807246952780E-02;
            coeff[i++] = 1.703234925367360E-02;
            coeff[i++] = 1.679153937732390E-02;
            coeff[i++] = 1.311029900731800E-02;
            coeff[i++] = 6.236819935129310E-03;
            coeff[i++] = -2.948109242306520E-03;
            coeff[i++] = -1.300660452337210E-02;
            coeff[i++] = -2.211914743349380E-02;
            coeff[i++] = -2.833840750155610E-02;
            coeff[i++] = -2.988554553005110E-02;
            coeff[i++] = -2.544397730521880E-02;
            coeff[i++] = -1.440355987391170E-02;
            coeff[i++] = 2.986032625697860E-03;
            coeff[i++] = 2.558172330403050E-02;
            coeff[i++] = 5.144391637650100E-02;
            coeff[i++] = 7.805539564819690E-02;
            coeff[i++] = 1.026330074082930E-01;
            coeff[i++] = 1.224893270217370E-01;
            coeff[i++] = 1.353914007504530E-01;
            coeff[i++] = 1.398638979850180E-01;
            coeff[i++] = 1.353914007504530E-01;
            coeff[i++] = 1.224893270217370E-01;
            coeff[i++] = 1.026330074082930E-01;
            coeff[i++] = 7.805539564819690E-02;
            coeff[i++] = 5.144391637650100E-02;
            coeff[i++] = 2.558172330403050E-02;
            coeff[i++] = 2.986032625697860E-03;
            coeff[i++] = -1.440355987391170E-02;
            coeff[i++] = -2.544397730521880E-02;
            coeff[i++] = -2.988554553005110E-02;
            coeff[i++] = -2.833840750155610E-02;
            coeff[i++] = -2.211914743349380E-02;
            coeff[i++] = -1.300660452337210E-02;
            coeff[i++] = -2.948109242306520E-03;
            coeff[i++] = 6.236819935129310E-03;
            coeff[i++] = 1.311029900731800E-02;
            coeff[i++] = 1.679153937732390E-02;
            coeff[i++] = 1.703234925367360E-02;
            coeff[i++] = 1.418807246952780E-02;
            coeff[i++] = 9.096268371124420E-03;
            coeff[i++] = 2.889323396409480E-03;
            coeff[i++] = -3.223761544739780E-03;
            coeff[i++] = -8.168701686096920E-03;
            coeff[i++] = -1.117626261202950E-02;
            coeff[i++] = -1.188669538449200E-02;
            coeff[i++] = -1.037594957051190E-02;
            coeff[i++] = -7.105333537665020E-03;
            coeff[i++] = -2.809043628297740E-03;
            coeff[i++] = 1.656529954957150E-03;
            coeff[i++] = 5.472995989090840E-03;
            coeff[i++] = 8.001329864149060E-03;
            coeff[i++] = 8.881291057799100E-03;
            coeff[i++] = 8.074594038494410E-03;
            coeff[i++] = 5.848296349033310E-03;
            coeff[i++] = 2.705479425549910E-03;
            coeff[i++] = -7.209459484446220E-04;
            coeff[i++] = -3.790136040710690E-03;
            coeff[i++] = -5.968364442408860E-03;
            coeff[i++] = -6.917390059145280E-03;
            coeff[i++] = -6.542270459946900E-03;
            coeff[i++] = -4.992796989421140E-03;
            coeff[i++] = -2.621285067667250E-03;
            coeff[i++] = 9.302479206313130E-05;
            coeff[i++] = 2.637460054145680E-03;
            coeff[i++] = 4.559640695342950E-03;
            coeff[i++] = 5.544834591166300E-03;
            coeff[i++] = 5.464261593874140E-03;
            coeff[i++] = 4.387574950239950E-03;
            coeff[i++] = 2.559468804386510E-03;
            coeff[i++] = 3.466769811991810E-04;
            coeff[i++] = -1.833573088156330E-03;
            coeff[i++] = -3.590008824981640E-03;
            coeff[i++] = -4.625008979640440E-03;
            coeff[i++] = -4.782252818150890E-03;
            coeff[i++] = -4.067209226948190E-03;
            coeff[i++] = -2.638494700700310E-03;
            coeff[i++] = -7.732648479493490E-04;
            coeff[i++] = 1.185923890154930E-03;
            coeff[i++] = 2.892660246535770E-03;
            coeff[i++] = 4.055701844895560E-03;
            coeff[i++] = 4.485705627354110E-03;
            coeff[i++] = 4.122593934438420E-03;
            coeff[i++] = 3.040216331443130E-03;
            coeff[i++] = 1.428954472036590E-03;
            coeff[i++] = -4.394865114783210E-04;
            coeff[i++] = -2.258231438950800E-03;
            coeff[i++] = -3.734197806456200E-03;
            coeff[i++] = -4.631905455394540E-03;
            coeff[i++] = -4.805640936145330E-03;
            coeff[i++] = -4.215886926156480E-03;
            coeff[i++] = -2.928740590104950E-03;
            coeff[i++] = -1.099885972970160E-03;
            coeff[i++] = 1.053101801720310E-03;
            coeff[i++] = 3.284841280731090E-03;
            coeff[i++] = 5.356970772465930E-03;
            coeff[i++] = 7.068489814377620E-03;
            coeff[i++] = 8.277693502109870E-03;
            coeff[i++] = 8.913496585899270E-03;
            coeff[i++] = 8.975823153750200E-03;
            coeff[i++] = 8.526434540132850E-03;
            coeff[i++] = 7.672793091555160E-03;
            coeff[i++] = 6.548246254406420E-03;
            coeff[i++] = 5.291926934972190E-03;
            coeff[i++] = 4.031186746077420E-03;
            coeff[i++] = 2.868711760546310E-03;
            coeff[i++] = 1.875193204599780E-03;
            coeff[i++] = 1.087727918046930E-03;
            coeff[i++] = 5.129075623129110E-04;
            coeff[i++] = 1.333334565741640E-04;
            coeff[i++] = -8.417172290272210E-05;
            coeff[i++] = -1.802091235321370E-04;
            coeff[i++] = -1.955377334965590E-04;
            coeff[i++] = -1.657729992539940E-04;
            coeff[i++] = -1.182903064715630E-04;
            coeff[i++] = -7.125357296762690E-05;
            coeff[i++] = -4.044646388326210E-05;

        }
    }


    if (0 == filter_name.compare("mtx8")) {
        insert("filter_name", filter_name);
        insert("filter_type", QString("decimation"));
        insert("filter_long_description", QString("metronix 4x decimation FIR with Remez"));
        insert("filter_category", QString("FIR"));
        insert("filter_move", (quint64) 8);
        coeff.reserve(159);
        coeff.resize(159);
        insert("filter_length", (quint64) 159);
        insert("filer_ouputs", (quint64)1);

        if ((this->quint64value("filter_length") > this->quint64value("num_samples") ) && this->quint64value("num_samples") ) {
            success = false;
            if (create_coeff_vector) return this->coeff;

        }
        else success = true;


        if (create_coeff_vector) {
            coeff.reserve(159);
            coeff.resize(159);

            i = 0;
            coeff[i++] = 1.274638071E-5;
            coeff[i++] = 1.871833207E-5;
            coeff[i++] = 3.051448966E-5;
            coeff[i++] = 4.508428102E-5;
            coeff[i++] = 6.158830100E-5;
            coeff[i++] = 7.858234792E-5;
            coeff[i++] = 9.395673097E-5;
            coeff[i++] = 1.050224364E-4;
            coeff[i++] = 1.087241754E-4;
            coeff[i++] = 1.018846125E-4;
            coeff[i++] = 8.161889406E-5;
            coeff[i++] = 4.577110201E-5;
            coeff[i++] = -6.565664053E-6;
            coeff[i++] = -7.462226237E-5;
            coeff[i++] = -1.555564299E-4;
            coeff[i++] = -2.442676894E-4;
            coeff[i++] = -3.333774120E-4;
            coeff[i++] = -4.135467518E-4;
            coeff[i++] = -4.740400810E-4;
            coeff[i++] = -5.036386902E-4;
            coeff[i++] = -4.917498406E-4;
            coeff[i++] = -4.297519242E-4;
            coeff[i++] = -3.123499630E-4;
            coeff[i++] = -1.389044065E-4;
            coeff[i++] = 8.554531293E-5;
            coeff[i++] = 3.496159862E-4;
            coeff[i++] = 6.355282534E-4;
            coeff[i++] = 9.196709247E-4;
            coeff[i++] = 1.173951543E-3;
            coeff[i++] = 1.367818261E-3;
            coeff[i++] = 1.470963341E-3;
            coeff[i++] = 1.456425398E-3;
            coeff[i++] = 1.303939654E-3;
            coeff[i++] = 1.003123800E-3;
            coeff[i++] = 5.562490955E-4;
            coeff[i++] = -1.986065669E-5;
            coeff[i++] = -6.930342056E-4;
            coeff[i++] = -1.416414222E-3;
            coeff[i++] = -2.130629748E-3;
            coeff[i++] = -2.767585135E-3;
            coeff[i++] = -3.255632794E-3;
            coeff[i++] = -3.525914544E-3;
            coeff[i++] = -3.519325575E-3;
            coeff[i++] = -3.193601619E-3;
            coeff[i++] = -2.529775410E-3;
            coeff[i++] = -1.537385494E-3;
            coeff[i++] = -2.576911868E-4;
            coeff[i++] = 1.235589439E-3;
            coeff[i++] = 2.838487276E-3;
            coeff[i++] = 4.422025666E-3;
            coeff[i++] = 5.840473903E-3;
            coeff[i++] = 6.942157624E-3;
            coeff[i++] = 7.582235943E-3;
            coeff[i++] = 7.636471028E-3;
            coeff[i++] = 7.014961223E-3;
            coeff[i++] = 5.674575302E-3;
            coeff[i++] = 3.628945214E-3;
            coeff[i++] = 9.548541383E-4;
            coeff[i++] = -2.205820901E-3;
            coeff[i++] = -5.649263137E-3;
            coeff[i++] = -9.118130628E-3;
            coeff[i++] = -1.231507218E-2;
            coeff[i++] = -1.492064668E-2;
            coeff[i++] = -1.661462901E-2;
            coeff[i++] = -1.709924220E-2;
            coeff[i++] = -1.612268602E-2;
            coeff[i++] = -1.350111910E-2;
            coeff[i++] = -9.137342056E-3;
            coeff[i++] = -3.034517438E-3;
            coeff[i++] = 4.696367013E-3;
            coeff[i++] = 1.383622388E-2;
            coeff[i++] = 2.406513571E-2;
            coeff[i++] = 3.497700849E-2;
            coeff[i++] = 4.610082290E-2;
            coeff[i++] = 5.692720913E-2;
            coeff[i++] = 6.693858403E-2;
            coeff[i++] = 7.564080472E-2;
            coeff[i++] = 8.259409732E-2;
            coeff[i++] = 8.744105514E-2;
            coeff[i++] = 8.992966248E-2;
            coeff[i++] = 8.992966248E-2;
            coeff[i++] = 8.744105514E-2;
            coeff[i++] = 8.259409732E-2;
            coeff[i++] = 7.564080472E-2;
            coeff[i++] = 6.693858403E-2;
            coeff[i++] = 5.692720913E-2;
            coeff[i++] = 4.610082290E-2;
            coeff[i++] = 3.497700849E-2;
            coeff[i++] = 2.406513571E-2;
            coeff[i++] = 1.383622388E-2;
            coeff[i++] = 4.696367013E-3;
            coeff[i++] = -3.034517438E-3;
            coeff[i++] = -9.137342056E-3;
            coeff[i++] = -1.350111910E-2;
            coeff[i++] = -1.612268602E-2;
            coeff[i++] = -1.709924220E-2;
            coeff[i++] = -1.661462901E-2;
            coeff[i++] = -1.492064668E-2;
            coeff[i++] = -1.231507218E-2;
            coeff[i++] = -9.118130628E-3;
            coeff[i++] = -5.649263137E-3;
            coeff[i++] = -2.205820901E-3;
            coeff[i++] = 9.548541383E-4;
            coeff[i++] = 3.628945214E-3;
            coeff[i++] = 5.674575302E-3;
            coeff[i++] = 7.014961223E-3;
            coeff[i++] = 7.636471028E-3;
            coeff[i++] = 7.582235943E-3;
            coeff[i++] = 6.942157624E-3;
            coeff[i++] = 5.840473903E-3;
            coeff[i++] = 4.422025666E-3;
            coeff[i++] = 2.838487276E-3;
            coeff[i++] = 1.235589439E-3;
            coeff[i++] = -2.576911868E-4;
            coeff[i++] = -1.537385494E-3;
            coeff[i++] = -2.529775410E-3;
            coeff[i++] = -3.193601619E-3;
            coeff[i++] = -3.519325575E-3;
            coeff[i++] = -3.525914544E-3;
            coeff[i++] = -3.255632794E-3;
            coeff[i++] = -2.767585135E-3;
            coeff[i++] = -2.130629748E-3;
            coeff[i++] = -1.416414222E-3;
            coeff[i++] = -6.930342056E-4;
            coeff[i++] = -1.986065669E-5;
            coeff[i++] = 5.562490955E-4;
            coeff[i++] = 1.003123800E-3;
            coeff[i++] = 1.303939654E-3;
            coeff[i++] = 1.456425398E-3;
            coeff[i++] = 1.470963341E-3;
            coeff[i++] = 1.367818261E-3;
            coeff[i++] = 1.173951543E-3;
            coeff[i++] = 9.196709247E-4;
            coeff[i++] = 6.355282534E-4;
            coeff[i++] = 3.496159862E-4;
            coeff[i++] = 8.554531293E-5;
            coeff[i++] = -1.389044065E-4;
            coeff[i++] = -3.123499630E-4;
            coeff[i++] = -4.297519242E-4;
            coeff[i++] = -4.917498406E-4;
            coeff[i++] = -5.036386902E-4;
            coeff[i++] = -4.740400810E-4;
            coeff[i++] = -4.135467518E-4;
            coeff[i++] = -3.333774120E-4;
            coeff[i++] = -2.442676894E-4;
            coeff[i++] = -1.555564299E-4;
            coeff[i++] = -7.462226237E-5;
            coeff[i++] = -6.565664053E-6;
            coeff[i++] = 4.577110201E-5;
            coeff[i++] = 8.161889406E-5;
            coeff[i++] = 1.018846125E-4;
            coeff[i++] = 1.087241754E-4;
            coeff[i++] = 1.050224364E-4;
            coeff[i++] = 9.395673097E-5;
            coeff[i++] = 7.858234792E-5;
            coeff[i++] = 6.158830100E-5;
            coeff[i++] = 4.508428102E-5;
            coeff[i++] = 3.051448966E-5;
            coeff[i++] = 1.871833207E-5;
        }
    }
    if (0 == filter_name.compare("notch")) {

        insert("filter_name", filter_name);
        insert("filter_type", QString("notch"));
        insert("filter_long_description", QString("FIR notch filter"));
        insert("filter_category", QString("FIR"));
        insert("filter_move", (quint64) 1);

        this->set_key_value("sample_freq", sample_freq);
        this->set_key_value("num_samples", num_samples);


        if (sample_freq > 0.99) {
            double tmp = (sample_freq * 2. + 1.);;
            if (tmp < 513) {
                insert("filter_length", (quint64) 513);
            }
            else {
                quint64 l = (quint64)tmp;
                // even needs to be odd
                if ((l % 2) == 0) ++l;
                insert("filter_length", l);
            }
        }

        if ((this->quint64value("filter_length") > this->quint64value("num_samples") ) && this->quint64value("num_samples") ) {
            success = false;
            if (create_coeff_vector) return this->coeff;

        }
        else success = true;

        insert("filer_ouputs", (quint64)1);

        // the notch swaps the values to indicate notch !!!!! !!!!!!!!!!!!!!!!!!!!!!!!!
        double CutLow = this->dvalue("upper_freq");
        double CutHigh = this->dvalue("lower_freq");

        if (create_coeff_vector) {
            this->coeff.resize(this->sztvalue("filter_length"));
            iter::fir_dsgn<double, std::vector<double>::iterator>(coeff.begin(), coeff.end(), this->dvalue("sample_freq"), CutLow, CutHigh);
        }

    }

    if (0 == filter_name.compare("highpass")) {

        insert("filter_name", filter_name);
        insert("filter_type", QString("highpass"));
        insert("filter_long_description", QString("FIR highpass filter"));
        insert("filter_category", QString("FIR"));
        insert("filter_move", (quint64) 1);


        this->set_key_value("sample_freq", sample_freq);
        this->set_key_value("num_samples", num_samples);

        insert("filer_ouputs", (quint64)1);


        if (sample_freq > 0.999) {
            quint64 l = (quint64)sample_freq * 4;
            if (l < 8193)  insert("filter_length", 8193 );
            else {
                if ((l % 2) == 0) ++l;
                insert("filter_length", l );
            }
        }
        else {
            insert("filter_length", 1025 );

        }


        if ((this->quint64value("filter_length") > this->quint64value("num_samples") ) && this->quint64value("num_samples") ) {
            success = false;
            if (create_coeff_vector) return this->coeff;

        }
        else success = true;

        if (create_coeff_vector) {
            this->coeff.resize(this->sztvalue("filter_length"));
            iter::fir_dsgn<double, std::vector<double>::iterator>(coeff.begin(), coeff.end(), this->dvalue("sample_freq"), this->dvalue("lower_freq"), 1.0);
        }

    }


    if (0 == filter_name.compare("bandpass")) {

        insert("filter_name", filter_name);
        insert("filter_type", QString("bandpass"));
        insert("filter_long_description", QString("FIR bandpass filter"));
        insert("filter_category", QString("FIR"));
        insert("filter_move", (quint64) 1);


        this->set_key_value("sample_freq", sample_freq);
        this->set_key_value("num_samples", num_samples);

        insert("filer_ouputs", (quint64)1);


        if (sample_freq > 0.999) {
            quint64 l = (quint64)sample_freq * 4;
            if (l < 8193)  insert("filter_length", 8193 );
            else {
                if ((l % 2) == 0) ++l;
                insert("filter_length", l );
            }
        }
        else {
            insert("filter_length", 1025 );

        }


        if ((this->quint64value("filter_length") > this->quint64value("num_samples") ) && this->quint64value("num_samples") ) {
            success = false;
            if (create_coeff_vector) return this->coeff;

        }
        else success = true;

        if (create_coeff_vector) {
            this->coeff.resize(this->sztvalue("filter_length"));
            iter::fir_dsgn<double, std::vector<double>::iterator>(coeff.begin(), coeff.end(), this->dvalue("sample_freq"), this->dvalue("lower_freq"), dvalue("upper_freq"));
        }

    }


    if (0 == filter_name.compare("lowpass")) {

        insert("filter_name", filter_name);
        insert("filter_type", QString("lowpass"));
        insert("filter_long_description", QString("FIR lowpass filter"));
        insert("filter_category", QString("FIR"));
        insert("filter_move", (quint64) 1);
        insert("lower_freq", (double) 0);

        this->set_key_value("sample_freq", sample_freq);
        this->set_key_value("num_samples", num_samples);

        // for above 128Hz this is always true
        double tmp = (sample_freq * 2. + 1.);;
        if (tmp < 513) {
            insert("filter_length", (quint64) 513);
        }
        // let the minimum of 257
        else {
            quint64 l = (quint64)tmp;
            // even needs to be odd
            if ((l % 2) == 0) ++l;
            insert("filter_length", l);
        }

        if ((this->quint64value("filter_length") > this->quint64value("num_samples") ) && this->quint64value("num_samples") ) {
            success = false;
            if (create_coeff_vector) return this->coeff;

        }
        else success = true;


        insert("filer_ouputs", (quint64)1);

        if (create_coeff_vector) {
            this->coeff.resize(this->sztvalue("filter_length"));
            iter::fir_dsgn<double, std::vector<double>::iterator>(coeff.begin(), coeff.end(), this->dvalue("sample_freq"), 0.0, this->dvalue("upper_freq"));
        }

    }



    return this->coeff;
}

