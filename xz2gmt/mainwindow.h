#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QTextStream>
#include <QIODevice>
#include <QDropEvent>
#include <QDragEnterEvent>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <QStatusBar>
#include <algorithm>
#include <QMainWindow>
#include <QProcess>
#include <QVector>
#include <cmath>
#include <limits>
#include <cfloat>
#include <algorithm>
#include <functional>


namespace xz2gmt {
template <class T, class Iterator> bool variance2D(Iterator firstx, const Iterator lastx, Iterator firsty, const Iterator lasty,
                                                T &varx, T& vary, T& stdx, T& stdy, T& meanx, T& meany, T& Qx, T& Qy, T& Qxy, size_t& n, T& covxy,
                                                T& corr_coeff, T& reg_coeff, T& stdxy, T& stdyx,
                                                T& abscissa, T& std_abscissa, T& std_reg_coeff)  {



    if ( !(lastx - firstx) || !(lasty - firsty)) {

        varx = T(0);
        vary = T(0);
        stdx = T(0);
        stdy = T(0);
        meanx = T(0);
        meany = T(0);
        Qx = T(0);
        Qy = T(0);
        Qxy = T(0);
        n = 0;
        covxy = T(0);
        corr_coeff = T(0);
        reg_coeff = T(0);
        stdxy = T(0);
        stdyx = T(0);
        abscissa = T(0);
        std_abscissa = T(0);
        std_reg_coeff = T(0);
        return false;


    }

    T add = T (0);
    T sumx = std::accumulate(firstx, lastx, add);
    T sumy = std::accumulate(firsty, lasty, add);

    n = (size_t)(lastx - firstx);

    T powsumx = T(0);
    T powsumy = T(0);
    T sum_of_squaresx = T (0);
    T sum_of_squaresy= T (0);

    T sum_of_product = T (0);

    T mysize  = T(lastx - firstx);
    T one = T(1);
    T two = T (2);

    abscissa = T(0);
    varx = T (0);
    stdx = T (0);
    vary = T (0);
    stdy = T (0);
    covxy = T (0);
    stdxy = T(0);
    stdyx = T (0);
    std_abscissa = T(0);
    std_reg_coeff = T(0);


    corr_coeff = T(0);
    reg_coeff  = T(0);

    Qxy = T(0);
    Qx = T(0);
    Qy = T(0);



    // copy and modify
    while (firstx != lastx) {

        sum_of_product  += *firstx * *firsty;
        sum_of_squaresx += pow(*firstx++, 2.);
        sum_of_squaresy += pow(*firsty++, 2.);

    }

    powsumx = pow(sumx,2.);
    powsumx /= mysize;

    powsumy = pow(sumy,2.);
    powsumy /= mysize;

    meanx = sumx;
    meanx /= mysize;

    meany = sumy;
    meany /= mysize;

    Qx = sum_of_squaresx - ((powsumx));
    Qy = sum_of_squaresy - ((powsumy));
    Qxy = sum_of_product - ((sumx * sumy)/mysize);




    // precision of double is not perfect
    if (fabs(Qx) < 1E-10) {
        Qx = 0;


    }
    else {
        varx = ( (one/(mysize - one)) * Qx) ;
        stdx = sqrt(varx);
    }

    // precision of double is not perfect
    if (fabs(Qy) < 1E-10) {
        Qy = 0;

    }
    else {
        vary = ( (one/(mysize - one)) * Qy) ;
        stdy = sqrt(vary);

    }

    if (fabs(Qxy) < 1E-10) {
        Qxy = 0;

    }
    else {
        covxy = Qxy / (mysize - one);

    }

    if (stdy && stdy) {

        corr_coeff = covxy / (stdx * stdy);
    }

    if (stdy && stdy && Qxy) {

        reg_coeff = Qxy / Qx;
    }

    if (Qy && Qy && Qxy) {

        stdyx = sqrt(  (Qy -( pow(Qxy, two) / Qx ))/ (mysize - two));
        stdxy = sqrt(  (Qx -( pow(Qxy, two) / Qy ))/ (mysize - two));
        std_abscissa = stdyx * sqrt ( one/mysize + (pow(meanx,two)/Qx));
        std_reg_coeff = stdxy / sqrt(Qx);
        abscissa = meany - reg_coeff * meanx;

    }
    return true;


}


}

using namespace std;

namespace Ui {
class mainwindow;
}

class mainwindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit mainwindow(QWidget *parent = Q_NULLPTR);
    ~mainwindow();

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

    void make_script();

private slots:


    void on_x_EW_min_doubleSpinBox_valueChanged(double arg1);

    void on_x_EW_max_doubleSpinBox_valueChanged(double arg1);

    void on_yz_NS_min_doubleSpinBox_valueChanged(double arg1);

    void on_yz_NS_max_doubleSpinBox_valueChanged(double arg1);

    void on_IncX_doubleSpinBox_valueChanged(double arg1);

    void on_IncY_doubleSpinBox_valueChanged(double arg1);

    void on_IncXsub_doubleSpinBox_valueChanged(double arg1);

    void on_IncYsub_doubleSpinBox_valueChanged(double arg1);

    void on_ann_x_maj_doubleSpinBox_valueChanged(double arg1);

    void on_ann_x_min_doubleSpinBox_valueChanged(double arg1);

    void on_ann_y_maj_doubleSpinBox_valueChanged(double arg1);

    void on_ann_y_min_doubleSpinBox_valueChanged(double arg1);

    void on_delta_ns_doubleSpinBox_valueChanged(double arg1);

    void on_delta_ew_doubleSpinBox_valueChanged(double arg1);

    void on_make_shell_pb_clicked();

    void on_exec_shell_pb_clicked();


    void on_Title_lineEdit_textChanged(const QString &arg1);

    void on_EW_x_label_lineEdit_textChanged(const QString &arg1);

    void on_NS_yz_label_lineEdit_textChanged(const QString &arg1);

    void on_ScaleTitle_lineEdit_textChanged(const QString &arg1);

    void on_ScaleUnit_lineEdit_textChanged(const QString &arg1);

    void on_x_EW_scale_doubleSpinBox_valueChanged(double arg1);

    void on_yz_NS_scale_doubleSpinBox_valueChanged(double arg1);

    void on_mul_x_inputby_doubleSpinBox_valueChanged(double arg1);

    void on_mul_yz_inputby_doubleSpinBox_valueChanged(double arg1);

    void on_mul_vals_inputby_doubleSpinBox_valueChanged(double arg1);

    void on_Scale_annot_doubleSpinBox_valueChanged(double arg1);

    void on_rescale_pushButton_clicked();

    void on_one_by_one_checkBox_clicked(bool checked);

    void on_xcol_spinBox_valueChanged(int arg1);

    void on_ycol_spinBox_valueChanged(int arg1);

    void on_valcol_spinBox_valueChanged(int arg1);

    void on_profile_ycol_spinBox_valueChanged(int arg1);

    void on_profile_rot_doubleSpinBox_valueChanged(double arg1);

    void on_mul_profile_ycol_inputby_doubleSpinBox_valueChanged(double arg1);

    void guess_profile();

    void update();

    void polyline_bottom_top();

    void on_y_offset_doubleSpinBox_valueChanged(double arg1);

    void on_advals_doubleSpinBox_valueChanged(double arg1);

    void on_zero_x_ax_startcheckBox_clicked();

private:
    Ui::mainwindow *ui;

    QFileInfo qfi_in;
    QFile     f_in;

    QFileInfo qfi_out;
    QFile     f_out;



    QTextStream instr;
    QTextStream qtx;

    QString line;
    QStringList line_items;


    int info_lines = 0;

    int xcol = 0;
    int yzcol = 1;
    int valcol = 2;

    QVector<double> x;
    QVector<double> yz;
    QVector<double> vals;
    QVector<double> vals_orig;
    QVector<double> prfy;
    QVector<double> prfx;

    // poly line for bottom and top

    // @TODO : must sort x
    QVector<double> bx;
    QVector<double> bx_val;
    double bt_tx_start_val;
    double bt_tx_end_val;


    QVector<double> tx;
    QVector<double> tx_val;
    double tx_bt_start_val;
    double tx_bt_end_val;



    double delta_ew = 0;
    double delta_ns = 0;

    double ann_y_min;
    double ann_y_maj;
    double ann_x_min;
    double ann_x_maj;

    double IncYsub;
    double IncXsub;
    double IncX;
    double IncY;


    double yz_NS_max;
    double yz_NS_min;
    double x_EW_max;
    double x_EW_min;

    double x_EW_max_len = 20.0;
    double yz_NS_max_len = 15.0;

    double x_EW_act_len = 20.0;
    double yz_NS_act_len = 15.0;

    double x_EW_scale = 1;
    double yz_NS_scale = 1;


    QString ScaleUnit;
    QString ScaleTitle;

    QString EW_x_label;
    QString NS_yz_label;

    QString Title;

    QProcess *exec_shellProcess;

    QString exec_shell_str;

    double mul_x_inputby = 1.;
    double mul_yz_inputby = 1.;
    double mul_vals = 1.;
    double add_vals = 0;

    double Scale_annot = 10;

    int profile_ycol = -1;
    double profile_rot_deg = 0;
    double profile_rot_rad = 0;
    double mul_profile_ycol_inputby = 1;

    double slope;
    double y_offset;




};

#endif // MAINWINDOW_H
