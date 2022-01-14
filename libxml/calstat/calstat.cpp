#include "calstat.h"





calstat::calstat(const double &f, const cal::cal &chopper, const double &minmax_percentage)
{
    this->f = f;
    this->f_max = this->f + this->f * ( minmax_percentage / 100.);
    this->f_min = this->f - this->f * ( minmax_percentage / 100.);
    this->chopper = chopper;

}

calstat::calstat(const calstat &rhs)
{
    this->operator =(rhs);
}

calstat &calstat::operator =(const calstat &rhs)
{
    if(&rhs == this) return *this;

    this->f = rhs.f;
    this->f_max = rhs.f_max;
    this->f_min = rhs.f_min;
    this->chopper = rhs.chopper;
    this->ampls = rhs.ampls;
    this->phzs = rhs.phzs;
    this->serials = rhs.serials;

    this->variance_ampls_data = rhs.variance_ampls_data;
    this->variance_phzs_data = rhs.variance_phzs_data;


    return *this;
}

bool calstat::add_cal(const double &f, const int &serial, const double &ampl, const double &phz)
{
    if ( (this->f_min < f ) && (this->f_max > f) ) {
        this->ampls.push_back(ampl);
        this->phzs.push_back(phz);
        this->serials.push_back(serial);

        return true;

    }

    return false;
}

std::vector<double> calstat::serials_to_double() const
{
    std::vector<double> dserials;
    dserials.reserve(this->serials.size());
    for (auto &v : this->serials) dserials.push_back( (double)v );

    return dserials;
}

double calstat::median_variance_ampl()
{
    two_pass_variance tpv;
    double median;

    tpv.variance(this->ampls.cbegin(), this->ampls.cend(), 0.0);

    median = iter::median<double> (this->ampls.begin(), this->ampls.end());

    tpv.variance_data[g_stat::median_x] = median;

    this->variance_ampls_data = tpv.variance_data;

    return median;



}

double calstat::median_variance_phase()
{
    two_pass_variance tpv;
    double median;

    tpv.variance(this->phzs.cbegin(), this->phzs.cend(), 0.0);

    median = iter::median<double> (this->phzs.begin(), this->phzs.end());

    tpv.variance_data[g_stat::median_x] = median;

    this->variance_phzs_data = tpv.variance_data;

    return median;

}
