/***************************************************************************
                          my_vallarray_double.cpp  -  description
                             -------------------
    begin                : Sun Nov 12 2000
    copyright            : (C) 2000 by B. Friedrichs
    email                : geo@metronix.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "my_valarray_double.h"
//using namespace my_valarray_double

int my_valarray_double::aspline(const valarray<double>& x, const valarray<double>& y,
                                valarray<double>& b, valarray<double>& c, valarray<double>& d) {


    size_t n = x.size();
    size_t i;
    // check for duplicates!!!
    for (i = 0; i < n - 1; i++)
        if (x[i] == x[i+1]) cerr << "aspline -> duplicate entry on x-axis( " << x[i] << ", " << x[i+1] << "); result unstable; continued ****************" << endl;

    b.resize(x.size());
    c.resize(x.size());
    d.resize(x.size());


    double num, den;
    double m_m1, m_m2, m_p1, m_p2;
    double x_m1, x_m2, x_p1, x_p2;
    double y_m1, y_m2, y_p1, y_p2;

#define dx(i) (x[i+1]-x[i])
#define dy(i) (y[i+1]-y[i])
#define  m(i) (dy(i)/dx(i))

    if (n > 0)		     /* we have data to process */
    {

        /*
       * calculate the coefficients of the spline
       * (the Akima interpolation itself)
       */

        /* b) interpolate the missing points: */

        x_m1 = x[0] + x[1] - x[2];
        y_m1 = (x[0]-x_m1) * (m(1) - 2 * m(0)) + y[0];

        m_m1 = (y[0]-y_m1)/(x[0]-x_m1);

        x_m2 = 2 * x[0] - x[2];
        y_m2 = (x_m1-x_m2) * (m(0) - 2 * m_m1) + y_m1;

        m_m2 = (y_m1-y_m2)/(x_m1-x_m2);

        x_p1 = x[n-1] + x[n-2] - x[n-3];
        y_p1 = (2 * m(n-2) - m(n-3)) * (x_p1 - x[n-1]) + y[n-1];

        m_p1 = (y_p1-y[n-1])/(x_p1-x[n-1]);

        x_p2 = 2 * x[n-1] - x[n-3];
        y_p2 = (2 * m_p1 - m(n-2)) * (x_p2 - x_p1) + y_p1;

        m_p2 = (y_p2-y_p1)/(x_p2-x_p1);

        /* i = 0 */
        num=fabs(m(1) - m(0)) * m_m1 + fabs(m_m1 - m_m2) * m(0);
        den=fabs(m(1) - m(0)) + fabs(m_m1 - m_m2);

        if (den != 0.0) b[0]=num / den;
        else            b[0]=0.0;

        /* i = 1 */
        num=fabs(m(2) - m(1)) * m(0) + fabs(m(0) - m_m1) * m(1);
        den=fabs(m(2) - m(1)) + fabs(m(0) - m_m1);

        if (den != 0.0) b[1]=num / den;
        else            b[1]=0.0;

        for (i=2; i < n-2; i++)
        {

            num=fabs(m(i+1) - m(i)) * m(i-1) + fabs(m(i-1) - m(i-2)) * m(i);
            den=fabs(m(i+1) - m(i)) + fabs(m(i-1) - m(i-2));

            if (den != 0.0) b[i]=num / den;
            else            b[i]=0.0;
        }

        /* i = n - 2 */
        num=fabs(m_p1 - m(n-2)) * m(n-3) + fabs(m(n-3) - m(n-4)) * m(n-2);
        den=fabs(m_p1 - m(n-2)) + fabs(m(n-3) - m(n-4));

        if (den != 0.0) b[n-2] = num / den;
        else	          b[n-2] = 0.0;

        /* i = n - 1 */
        num=fabs(m_p2 - m_p1) * m(n-2) + fabs(m(n-2) - m(n-3)) * m_p1;
        den=fabs(m_p2 - m_p1) + fabs(m(n-2) - m(n-3));

        if (den != 0.0) b[n-1]=num / den;
        else	      b[n-1]=0.0;

        for (i=0; i < n-1; i++)
        {
            double dxv = dx(i);
            c[i]=(3 * m(i) - 2 * b[i] - b[i+1]) / dxv;
            d[i]=(b[i] + b[i+1] - 2 * m(i)) / (dxv * dxv);
        }
    }
#undef dx
#undef dy
#undef  m
    return 1;
}


//******************************************************************************************
/***************************************************************************
 * copy from fit.c from xmgrace
 ***************************************************************************/


double my_valarray_double::seval(const double u, const valarray<double>& x, const valarray<double>& y,
                                 const valarray<double>& b, const valarray<double>& c, const valarray<double>& d)
{
    /*
c
c  this subroutine evaluates the cubic spline function
c
c    seval = y(i) + b(i)*(u-x(i)) + c(i)*(u-x(i))**2 + d(i)*(u-x(i))**3
c
c    where  x(i) .lt. u .lt. x(i+1), using horner's rule
c
c  if  u .lt. x(1) then  i = 1	is used.
c  if  u .ge. x(n) then  i = n	is used.
c
c  input..
c
c    n = the number of data points
c    u = the abscissa at which the spline is to be evaluated
c    x,y = the arrays of data abscissas and ordinates
c    b,c,d = arrays of spline coefficients computed by spline
c
c  if  u  is not in the same interval as the previous call, then a
c  binary search is performed to determine the proper interval.
c
*/
    unsigned int n = x.size();
    unsigned int i, j, k;
    double dx;

    /*
c
c  binary search
c
*/

    if (u < x[0]) i = 0;
    else if (u >= x[n - 1])	i = n - 1;
    else {
        i = 0;
        j = n;

        do {
            k = (i + j) / 2;
            if (u < x[k])
                j = k;
            if (u >= x[k])
                i = k;
        } while (j > i + 1);
    }
    /*
l20: ;
   k = (i + j) / 2; //cerr << k << " ";
   if (u < x[k])
     j = k;
   if (u >= x[k])
     i = k;
   if (j > i + 1)
     goto l20;

    }
*/
    /*
c
c  evaluate spline
c
*/
    dx = u - x[i];

    return (y[i] + dx * (b[i] + dx * (c[i] + dx * d[i])));
}

double my_valarray_double::seval_const_borders(double u, const valarray<double>& x, const valarray<double>& y,
                                               const valarray<double>& b, const valarray<double>& c, const valarray<double>& d)
{

    // see above; except that if u > x or u < x  -> y[n] or y[0] will be returned!!!
    unsigned int n = x.size();
    unsigned int i, j, k;
    double dx;

    if (u < x[0]) return y[0];
    else if (u >= x[n - 1]) return y[n-1];
    else {
        i = 0;
        j = n;
        do {
            k = (i + j) / 2;
            if (u < x[k])
                j = k;
            if (u >= x[k])
                i = k;
        } while (j > i + 1);
    }
    /*
l20: ;
   k = (i + j) / 2;
   if (u < x[k])  j = k;
   if (u >= x[k]) i = k;
   if (j > i + 1) goto l20;
    }
*/
    dx = u - x[i];

    return (y[i] + dx * (b[i] + dx * (c[i] + dx * d[i])));
}


/*
 a literal translation of the spline routine in
 Forsyth, Malcolm, and Moler
 copy from fit.c from xmgrace
*/
int my_valarray_double::spline(const valarray<double>& x, const valarray<double>& y,
                               valarray<double>& b, valarray<double>& c, valarray<double>& d)
{
    /*
c
c  the coefficients b(i), c(i), and d(i), i=1,2,...,n are computed
c  for a cubic interpolating spline
c
c    s(x) = y(i) + b(i)*(x-x(i)) + c(i)*(x-x(i))**2 + d(i)*(x-x(i))**3
c
c    for  x(i) .le. x .le. x(i+1)
c
c  input..
c
c    n = the number of data points or knots (n.ge.2)
c    x = the abscissas of the knots in strictly increasing order
c    y = the ordinates of the knots
c
c  output..
c
c    b, c, d  = arrays of spline coefficients as defined above.
c
c  using  p  to denote differentiation,
c
c    y(i) = s(x(i))
c    b(i) = sp(x(i))
c    c(i) = spp(x(i))/2
c    d(i) = sppp(x(i))/6  (derivative from the right)
c
c  the accompanying function subprogram  seval	can be used
c  to evaluate the spline.
c
c
*/
    int n = x.size();
    b.resize(x.size());
    c.resize(x.size());
    d.resize(x.size());
    int nm1, ib, i;
    double t;

    /*
Gack!
*/
    /************** do not know what happends here, not working with valarrays
    x--;
    y--;
    b--;
    c--;
    d--;
*/

    /*
Fortran 66
*/
    nm1 = n - 1;
    if (n < 2)
        return 1;
    if (n < 3)
        goto l50;
    /*
c
c  set up tridiagonal system
c
c  b = diagonal, d = offdiagonal, c = right hand side.
c
*/
    d[1] = x[2] - x[1];
    c[2] = (y[2] - y[1]) / d[1];
    for (i = 2; i <= nm1; i++) {
        d[i] = x[i + 1] - x[i];
        b[i] = 2.0 * (d[i - 1] + d[i]);
        c[i + 1] = (y[i + 1] - y[i]) / d[i];
        c[i] = c[i + 1] - c[i];
    }
    /*
c
c  end conditions.  third derivatives at  x(1)	and  x(n)
c  obtained from divided differences
c
*/
    b[1] = -d[1];
    b[n] = -d[n - 1];
    c[1] = 0.0;
    c[n] = 0.0;
    if (n == 3)
        goto l15;
    c[1] = c[3] / (x[4] - x[2]) - c[2] / (x[3] - x[1]);
    c[n] = c[n - 1] / (x[n] - x[n - 2]) - c[n - 2] / (x[n - 1] - x[n - 3]);
    c[1] = c[1] * d[1] * d[1] / (x[4] - x[1]);
    c[n] = -c[n] * d[n - 1] * d[n - 1] / (x[n] - x[n - 3]);
    /*
c
c  forward elimination
c
*/
l15:;
    for (i = 2; i <= n; i++) {
        t = d[i - 1] / b[i - 1];
        b[i] = b[i] - t * d[i - 1];
        c[i] = c[i] - t * c[i - 1];
    }
    /*
c
c  back substitution
c
*/
    c[n] = c[n] / b[n];
    for (ib = 1; ib <= nm1; ib++) {
        i = n - ib;
        c[i] = (c[i] - d[i] * c[i + 1]) / b[i];
    }
    /*
c
c  c(i) is now the sigma(i) of the text
c
c  compute polynomial coefficients
c
*/
    b[n] = (y[n] - y[nm1]) / d[nm1] + d[nm1] * (c[nm1] + 2.0 * c[n]);
    for (i = 1; i <= nm1; i++) {
        b[i] = (y[i + 1] - y[i]) / d[i] - d[i] * (c[i + 1] + 2.0 * c[i]);
        d[i] = (c[i + 1] - c[i]) / d[i];
        c[i] = 3.0 * c[i];
    }
    c[n] = 3.0 * c[n];
    d[n] = d[n - 1];
    return 1;

l50:;
    b[1] = (y[2] - y[1]) / (x[2] - x[1]);
    c[1] = 0.0;
    d[1] = 0.0;
    b[2] = b[1];
    c[2] = 0.0;
    d[2] = 0.0;
    return 1;
}

// copy from fit.c from xmgrace

int my_valarray_double::runavg(const valarray<double>& x, const valarray<double>& y, valarray<double>& ax,
                               valarray<double>& ay, unsigned int ilen)
{
    size_t i;
    double sumy = 0.0;
    double sumx = 0.0;

    for (i = 0; i < ilen; i++) {
        sumx = sumx + x[i];
        sumy = sumy + y[i];
    }
    ax[0] = sumx / ilen;
    ay[0] = sumy / ilen;
    for (i = 1; i < (x.size() - ilen + 1); i++) {
        sumx = x[i + ilen - 1] - x[i - 1] + sumx;
        ax[i] = sumx / ilen;
        sumy = y[i + ilen - 1] - y[i - 1] + sumy;
        ay[i] = sumy / ilen;
    }
    return 1;
}


/***************************************************************************
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *   applies filter with 32x decimation                                    *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/


size_t my_valarray_double::filx(const valarray<double>& in, valarray<double>& out, const unsigned int reduction,
                                const valarray<double>& cadblFilterFac, const double& scale ) {

    if (cadblFilterFac.size() && reduction){
        cerr << "filx -> using external coefficients (" << cadblFilterFac.size()
             << "), " << reduction << "x reduction" << endl;
    }
    else {
        cerr << "filx -> nothing to do" << endl;
        return in.size();
    }
    if (in.size() < cadblFilterFac.size()) {
        cerr << "filx -> filter has " << cadblFilterFac.size() << " coefficients, your time series only "
             << in.size() << " points; return 0" << endl;
        return 0;
    }

    size_t i = 0;
    valarray<double> vtmp(cadblFilterFac.size());
    // resize output if neccessary
    //	if (out.size() != ( (in.size() - cadblFilterFac.size())/reduction ))
    out.resize( (in.size() - cadblFilterFac.size())/reduction +1);


    // fold the input signal with the filter function
    while (i  < out.size() ) {
        vtmp = in[slice( i * reduction, cadblFilterFac.size(),1)] * cadblFilterFac;
        out[i++] = vtmp.sum();
    }
    if (scale != 1.0) out *= scale;
    // cerr << "filter ready " << in.size() << "  " << out.size() << "  " << cadblFilterFac.size()
    // 			<< "  " << --i << endl << flush;
    return out.size();
}

size_t my_valarray_double::resample(const valarray<double>& in, valarray<double>& out, const double old_D_f, const double new_D_f){

    if (!new_D_f || new_D_f == DBL_MAX) {
        cerr <<  "my_valarray_double -> resample new sampling frequency probably not initialised: " << new_D_f << endl;
        exit(0);
    }

    size_t old_f = size_t(old_D_f);
    size_t new_f = size_t(new_D_f);


    if (old_f < 2 || new_f < 2) {
        cerr << "my_valarray_double -> resample only implemented for sampling frequencies above 2 Hz, exit" << endl;
        exit(0);
    }



    size_t i, j, k;

    out.resize((in.size()/old_f) * new_f);    // new data vector

    cerr <<  "my_valarray_double -> resample:: input " << in.size() << " @" << old_f << " ouput " << out.size() << " @" << new_f << endl;

    valarray<double> in_segm(old_f);          // generate a input segment of one second

    valarray<double> in_ts(old_f);            // time axis of input vector for one second
    valarray<double> out_ts(new_f);           // time axis of output vector for one second

    valarray<double> b, c, d;                 // needed by interpolation routine

    for (i = 0; i < old_f; i++) in_ts[i] = double (i)  / old_f;   // set up the sampling intervals
    for (i = 0; i < new_f; i++) out_ts[i] = double (i) / new_f;

    for (j = 0; j < in.size() / old_f; j++) {
        in_segm = in[slice(j * old_f, old_f, 1)];       // sclice it
        my_valarray_double::aspline(in_ts, in_segm, b, c, d);
        for (i = 0; i < new_f; i++) out[i + j*new_f] = my_valarray_double::seval(out_ts[i], in_ts, in_segm, b, c, d);

    }


    return out.size();

}

