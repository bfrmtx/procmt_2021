#include "boost_student.h"
#include <boost/math/distributions/students_t.hpp>
#include <boost/random.hpp>

boost_student::boost_student() {
}

boost_student::~boost_student() {
  qDebug() << "boost_student destroyed";
}

statmap boost_student::student_t(const statmap &regression_data, const double upper_quantile) {
  this->regression_result.clear();
  this->regression_result = this->mk_regression_result(regression_data, this->is_ok);
  if (!this->is_ok) {
    std::cerr << "failed to create student_t_bst_lingreg sample sizes x,y differ, or size < 4 !!!!" << std::endl;
  }
  this->d_delta_slope = 0.0;
  this->d_student_t_inv = 0.0;
  this->d_upper_confidence = 0.0;
  this->d_lower_confidence = 0.0;
  this->d_upper_quantile = upper_quantile;

  std::unique_ptr<boost::math::students_t> dist = std::make_unique<boost::math::students_t>((((int)this->regression_result[g_stat::d_n_x]) - 2));
  this->d_student_t_inv = boost::math::quantile(*dist, this->d_upper_quantile);
  this->d_delta_slope = this->d_student_t_inv * sqrt(this->regression_result[g_stat::abscissa_xy]) /
                        (this->regression_result[g_stat::stddev_x] * sqrt(((this->regression_result[g_stat::d_n_x] - 1.0) * (this->regression_result[g_stat::d_n_x] - 2.0))));

  this->d_upper_confidence = this->regression_result[g_stat::slope_xy] + this->d_delta_slope;
  this->d_lower_confidence = this->regression_result[g_stat::slope_xy] - this->d_delta_slope;

  this->regression_result[g_stat::delta_slope_xy] = this->d_delta_slope;
  this->regression_result[g_stat::student_t_upper_quantile] = this->d_upper_quantile;
  this->regression_result[g_stat::student_t_inv] = this->d_student_t_inv;
  this->regression_result[g_stat::student_t_upper_confidence_xy] = this->d_upper_confidence;
  this->regression_result[g_stat::student_t_lower_confidence_xy] = this->d_lower_confidence;

  return this->regression_result;
}

// change this if you change in math_vector
statmap boost_student::mk_regression_result(const statmap &regression_data, bool &is_ok) {

  statmap datamap;

  if ((regression_data.at(g_stat::d_n_x) == regression_data.at(g_stat::d_n_y)) && (regression_data.at(g_stat::d_n_x) >= 4.00)) {

    for (auto it = regression_data.cbegin(); it != regression_data.cend(); ++it) {
      datamap[it->first] = it->second;
    }

    // copy convinience
    datamap[g_stat::delta_slope_xy] = DBL_MAX;
    datamap[g_stat::student_t_upper_quantile] = DBL_MAX;
    datamap[g_stat::student_t_inv] = DBL_MAX;
    datamap[g_stat::student_t_upper_confidence_xy] = DBL_MAX;
    datamap[g_stat::student_t_lower_confidence_xy] = DBL_MAX;

    is_ok = true;
  } else {
    is_ok = false;
  }

  return datamap;
}
