#include "xy_regression_plot.h"


xy_regression_plot::xy_regression_plot(QWidget *parent) : QMainWindow(parent)
{

}

void xy_regression_plot::clear()
{
    this->x.clear();
    this->y.clear();
    this->tplrg.reset();
    this->rgplt_data.reset();
    this->tp_regressiondata.clear();
    this->tp_regressionresults.clear();
    this->stdlrg.reset();
    this->plot_x.clear();
    this->plot_y.clear();
    this->allselected.clear();
    this->msg.reset();
    this->note.clear();
    this->plot_tp.reset();
}

void xy_regression_plot::set_data(const std::vector<double> &x, const std::vector<double> &y)
{

    if (x.size() < 4) return;
    this->clear();

    math_vector mathv;

    this->x = x;
    this->y = y;

    // this struct calculates the linear regression
    this->tplrg = std::make_unique<two_pass_linreg>();

    // create the struct to calculate the regression data for plotting such as slopes
    this->rgplt_data = std::make_unique<regplot_data>();

    // do the regression and get the results
    this->tp_regressiondata = tplrg->linreg(x.cbegin(), x.cend(), y.cbegin(), y.cend(), 0.0);


    mathv.lin_round_min_max(this->tp_regressiondata[g_stat::min_x], this->tp_regressiondata[g_stat::max_x], 10., this->inner_min, this->inner_max);
    this->outer_minx = inner_min - 5;
    this->outer_maxx = inner_max + 5;
    mathv.lin_round_min_max(tp_regressiondata[g_stat::min_y], tp_regressiondata[g_stat::max_y], 10., outer_miny, outer_maxy);
    outer_miny -= 1.;
    outer_maxy += 1.;


    // create the Student's T distribution

    this->stdlrg = std::make_unique<boost_student>();

    this->tp_regressionresults = stdlrg->student_t(tp_regressiondata, 0.95);

    // add the results
    this->tplrg->test_against_other_slope(0.0, this->tp_regressionresults);

    msg = std::make_unique<message_window>();
    this->msg->add_header("Linear Regression");
    this->msg->add_statmap(tp_regressionresults);

    msg->resize(400, 800);
    msg->show();

    // add a plot
    plot_tp = std::make_unique<plotlib>();

    plot_tp->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);

    // create the standard slope inside the wanted plot range
    this->rgplt_data->mkplot_slope(x.cbegin(), x.cend(), plot_x, plot_y, this->tp_regressionresults, -1, inner_min, inner_max);

    plot_tp->std_regression_plot("data", x, y, allselected, true);
    plot_tp->std_regression_plot("slope", plot_x, plot_y, allselected, true);

    // lower slope starting from mean
    this->rgplt_data->mkplot_slope(x.cbegin(), x.cend(), plot_x, plot_y, this->tp_regressionresults, -1, inner_min, inner_max, this->tp_regressionresults[g_stat::mean_x], this->tp_regressionresults[g_stat::mean_y], 1);
    plot_tp->std_regression_plot("lower_upper_slope", plot_x, plot_y, allselected, true);

    // upper slope starting from mean
    this->rgplt_data->mkplot_slope(x.cbegin(), x.cend(), plot_x, plot_y, this->tp_regressionresults, -1, inner_min, inner_max, this->tp_regressionresults[g_stat::mean_x], this->tp_regressionresults[g_stat::mean_y], 2);
    plot_tp->std_regression_plot("lower_upper_slope", plot_x, plot_y, allselected, true);

    // add the mean of y, x
    plot_tp->std_regression_plot("means", this->tp_regressionresults[g_stat::mean_x], this->tp_regressionresults[g_stat::mean_y]);

    // add lower confidence intervall sqrt
    this->rgplt_data->mkplot_slope(x.cbegin(), x.cend(), plot_x, plot_y, this->tp_regressionresults, 1, 0.0, 0.0, 0.0, 0.0, 3, sqrt(this->tp_regressionresults[g_stat::stddev_y]));
    plot_tp->std_regression_plot("lower_upper_confidence", plot_x, plot_y, allselected, true);

    // add upper confidence intervall sqrt
    this->rgplt_data->mkplot_slope(x.cbegin(), x.cend(), plot_x, plot_y, this->tp_regressionresults, 1, 0.0, 0.0, 0.0, 0.0, 4, sqrt(this->tp_regressionresults[g_stat::stddev_y]));
    plot_tp->std_regression_plot("lower_upper_confidence", plot_x, plot_y, allselected, true);


    plot_tp->title("x y");
    note = "absc = " + QString::number(this->tp_regressionresults[g_stat::abscissa_xy]) + " slope = " + QString::number(this->tp_regressionresults[g_stat::slope_xy]);
    plot_tp->subtitle(note);
    plot_tp->xAxis->setRange(outer_minx, outer_maxx);
    plot_tp->yAxis->setRange(outer_miny, outer_maxy);
    //plot_tp->set_linticks("xAxis", 10);
    plot_tp->setWindowTitle("standard deviation");
    plot_tp->resize(800, 600);
    plot_tp->show();

}

