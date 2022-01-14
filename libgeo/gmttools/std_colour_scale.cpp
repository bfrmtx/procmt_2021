#include "std_colour_scale.h"

std_colour_scale::std_colour_scale(const QString which, const bool extends_colour, const bool nodata_white, const bool revert)
{

    if (which == "krl") {
        this->rgb << "255/25/0" << "255/76/0" << "255/127/0" << "255/178/0" << "255/229/0" << "229/255/0" << "178/255/0" << "127/255/0" << "76/255/0" << "0/255/153" << "0/255/255" << "0/153/255" << "0/51/255" << "84/0/229";
    }
    else {
        this->rgb << "255/229/252" << "254/204/244" << "255/178/232" << "255/153/214" << "255/127/191" << "255/101/163" << "254/76/130" << "255/50/91" << "255/25/48" << "255/0/0" << "255/25/0" << "255/51/0" << "255/76/0" << "255/102/0" << "255/127/0" << "255/153/0" << "255/178/0" << "255/204/0" << "255/229/0" << "255/255/0" << "229/255/0" << "204/255/0" << "178/255/0" << "153/255/0" << "127/255/0" << "102/255/0" << "76/255/0" << "51/255/0" << "25/255/0" << "0/255/0" << "0/255/51" << "0/255/102" << "0/255/153" << "0/255/204" << "0/255/255" << "0/204/255" << "0/153/255" << "0/102/255" << "0/51/255" << "0/0/255" << "84/0/229" << "149/0/204" << "183/0/165" << "158/0/84" << "137/0/22" << "112/20/0" << "91/50/0" << "66/60/0" << "32/45/0" << "7/20/0";
    }

    if (revert) {
        std::reverse(this->rgb.begin(), this->rgb.end());
    }

    if (extends_colour) {

        this->B = this->rgb.first();
        this->F = this->rgb.last();
        this->extends_colour = true;

    }
    else {
        this->extends_colour = false;
    }
    if (nodata_white) {
        this->N = "255/255/255";
    }
    else {
        this->nodata_white = false;
    }


}


int std_colour_scale::create_cpt_scale(QTextStream *qtx, const double min, const double max, const bool contour, const bool log, bool round)
{

    double base = 0, lnf = 0;
    int steps = this->rgb.size();
    int i;
    double val, val2 ;

    double rdmin, rdmax;
    double roundtomin;
    double roundtomax;

    if (log && min <= 0) {
        qDebug() << "\nlog equidistant values are used; please min > 0\n";
        return false;
    }
    if ( log) {
        lnf  = log10(max / min) / steps;
        base = log10(min);
    }
    else lnf = (max - min) / steps;

    qtx->setFieldWidth(14);
    qtx->AlignLeft;

    for (i = 0; i < steps; ++ i) {
        if (!log) {

            if (round) {
                roundtomin = autoMultiple<double>( min + lnf * i);
                rdmin = roundFloorMultiple<double>( min + lnf * i, roundtomin);
                roundtomax = autoMultiple<double>( min + lnf * (i + 1));
                rdmax = roundFloorMultiple<double>( min + lnf * (i + 1), roundtomax);

                if (!contour) {
                    std::cout << rdmin  << " " << this->rgb.at(i).toStdString() << " "  <<
                                 rdmax << " " << this->rgb.at(i).toStdString() << std::endl;
                    *qtx <<  rdmin << this->rgb.at(i) << rdmax << this->rgb.at(i) << Qt::endl;
                }
                else {
                    std::cout << rdmin  << " A" << std::endl;
                    *qtx <<  rdmin << " A" << Qt::endl;
                }
            }

            else {
                if (!contour) {

                    std::cout <<   min + lnf * i << " " << this->rgb.at(i).toStdString() << " "  << min + lnf * (i + 1) << " " << this->rgb.at(i).toStdString() << std::endl;
                    *qtx <<  min + lnf * i << this->rgb.at(i) << min + lnf * (i + 1) << this->rgb.at(i) << Qt::endl;
                }
                else {
                    std::cout <<   min + lnf * i << " A" << std::endl;
                    *qtx <<  min + lnf * i << " A" << Qt::endl;
                }
            }
        }
        else {
            val = pow(10, base + lnf * i);
            val2 = pow(10, base + lnf * (i+1) );



            if (round) {

                roundtomin = autoMultiple<double>( val);
                rdmin = roundFloorMultiple<double>( val, roundtomin);
                roundtomax = autoMultiple<double>( val2);
                rdmax = roundFloorMultiple<double>( val2, roundtomax);

                if (!contour) {
                    std::cout <<   rdmin << " " << this->rgb.at(i).toStdString() << " "  << rdmax << " " << this->rgb.at(i).toStdString() << std::endl;
                    *qtx <<  rdmin << this->rgb.at(i) << rdmax << this->rgb.at(i) << Qt::endl;
                }
                else {
                    std::cout <<   rdmin << " A" <<  std::endl;
                    *qtx <<  rdmin << " A" << Qt::endl;
                }
            }
            else {
                if (!contour) {
                    std::cout <<   val << " " << this->rgb.at(i).toStdString() << " "  << val2 << " " << this->rgb.at(i).toStdString() << std::endl;
                    *qtx <<  val << this->rgb.at(i) << val2 << this->rgb.at(i) << Qt::endl;
                }
                else {
                    std::cout <<   val << " A" <<  std::endl;
                    *qtx <<  val << " A" << Qt::endl;
                }
            }

        }
    }

    if (this->extends_colour) {
        *qtx << "B " << this->B << Qt::endl;
        *qtx << "F " << this->F << Qt::endl;
        std::cout << "B " << this->B.toStdString() << std::endl;
        std::cout << "F " << this->F.toStdString() << std::endl;
    }
    if (this->nodata_white) {
        *qtx << "N " << this->N << Qt::endl;
        std::cout << "N " << this->N.toStdString() << std::endl;

    }

    qtx->flush();
    return steps;
}


