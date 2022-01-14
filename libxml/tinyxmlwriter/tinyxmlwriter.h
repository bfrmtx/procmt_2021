/***************************************************************************
**                                                                        **
**  ProcMT and modules                                                    **
**  Copyright (C) 2017-2021 metronix GmbH                                 **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
** Parts of the software are linked against the Open Source Version of Qt **
** Qt source code copy is available under https://www.qt.io               **
** Qt source code modifications are non-proprietary,no modifications made **
** https://www.qt.io/licensing/open-source-lgpl-obligations               **
****************************************************************************
**           Author: metronix geophysics                                  **
**  Website/Contact: www.metronix.de                                      **
**                                                                        **
**                                                                        **
****************************************************************************/

#ifndef tinyxmlwriter_H
#define tinyxmlwriter_H

#include <QString>
#include <QTextStream>
#include <QVector>
#include <QMap>
#include <QVariant>
#include <iostream>
#include <QVariant>
#include <QDateTime>

//!< @todo round 1.999 to 2 as option

/*!
 * \brief The tinyxmlwriter class is simple a xml writer; collects data into a QString

Usage for small XML files written at once like calibration files

@class tinyxmlwriter wites XML data line by line <br>
xml.push ( "caldata", caldata_qualifier ); <br>
xml.sci_element ( "c0", "unit=\"V\"", 0. );  <br>
xml.sci_element ( "c1", "unit=\"Hz\"", f );  <br>
xml.sci_element ( "c2", "unit=\"V/(nT*Hz)\"", ampl );<br>
xml.sci_element ( "c3", "unit=\"deg\"", phase_grad );<br>
.... <br>
xml.pop(); <br> <br>
writes a section like: <br>

   </caldata> <br>
    <caldata chopper="off" gain_1="1" gain_2="1" gain_3="1" impedance="(0,0)"> <br>
      <c0 unit="V">0.00000000e+00</c0> <br>
      <c1 unit="Hz">6.30950000e+01</c1> <br>
      <c2 unit="V/(nT*Hz)">1.35300000e-02</c2> <br>
      <c3 unit="deg">3.34840000e+00</c3> <br>
    </caldata> <br>


 Wrting a file:
    QTextStream xml_out;

        xml_out.setDevice(&file);
        xml_out << xmlwriter->getXmlStr();
        file.close();
 */

// #define Qt::endl '\n' << std::flush

class tinyxmlwriter
{

public:
    tinyxmlwriter(const bool write_header = false);

    void header();

    tinyxmlwriter(const tinyxmlwriter &rhs);

    tinyxmlwriter& operator = (const tinyxmlwriter &rhs);

    ~tinyxmlwriter();

    QString getXmlStr() const;

    /*!
     * \brief indent whenever push() is called the indent increases tinyxmlwriter::stack , pop decreases the indentation
     */
    void indent();
    void push( const QString& element );
    void push ( const QString& element, const QString& attributes );
    /*!
     * \brief push for typical: "channel id=3"
     * \param element like channel
     * \param str_attribute like id
     * \param int_value like 3
     * \return
     */
    void push(const QString& element, const QString& str_attribute, const int &int_value);

    template<class T>  void element( const QString& element, T val );
    void element( const QString& element, const QString& val ) ;

    template<class T>  void element( const QString& element, const QString& str_attribute, const int &int_value, T val );
    void element( const QString& element, const QString& str_attribute, const int &int_value, const QString& val ) ;


    template<class T>  void  element( const QString& element, const QString& attributes, T val );
    void element( const QString& element,const QString& attributes,  const QString& val );

    template<class T>  void  sci_element( const QString& element, T val, const int &precision );
    /*!
     * \brief sci_element puts elements in scientific notation
     * xmlwriter->sci_element ( "c2", "unit=\"V/(nT)\"", ampl_off.at(i) / 1000.  );
     */
    template<class T>  void  sci_element( const QString& element, const QString& attributes, T val, const int &precision );


    /*!
     * \brief qv_int_element writes a QVariant as integer
     * \param element
     * \param val Qvariant
     */
    void qv_int_element(const QString& element, const QVariant& val);

    /*!
     * \brief qv_int_element writes a QVariant as integer with attributes
     * \param element
     * \param attributes
     * \param val
     */
    void qv_int_element(const QString& element, const QString& attributes, const QVariant& val);

    /*!
     * \brief qv_int_element writes a QVariant as unsigned integer
     * \param element
     * \param val Qvariant
     */
    void qv_uint_element(const QString& element, const QVariant& val);

    /*!
     * \brief qv_int_element writes a QVariant as unsigned integer with attributes
     * \param element
     * \param attributes
     * \param val
     */
    void qv_uint_element(const QString& element, const QString& attributes, const QVariant& val);


    /*!
     * \brief qv_double_element writes a QVariant as double
     * \param element
     * \param val
     */
    void qv_double_element(const QString& element, const QVariant& val);

    /*!
     * \brief qv_double_element writes a QVariant as double with attributes
     * \param element
     * \param attributes
     * \param val
     */
    void qv_double_element(const QString& element, const QString& attributes, const QVariant& val);

    /*!
     * \brief qv_double_element writes a QVariant as string
     * \param element
     * \param val
     */
    void qv_string_element(const QString& element, const QVariant& val);

    /*!
     * \brief qv_string_element writes a QVariant as string with attributes
     * \param element
     * \param attributes
     * \param val
     */
    void qv_string_element(const QString& element, const QString& attributes, const QVariant& val);

    /*!
     * \brief time_hh_mm_ss writes hh:mm:ss from a QDateTime
     * \param element
     * \param datetime QDateTime
     */
    void time_hh_mm_ss(const QString &element, const QDateTime &datetime);

    /*!
     * \brief date_yyyy_mm_dd write a yyyy-mm-dd from a QDateTime
     * \param element
     * \param datetime QDateTime
     */
    void date_yyyy_mm_dd(const QString &element, const QDateTime &datetime);

    /*!
     * \brief qv_sci_element
     * \param element
     * \param val
     * \param precision
     */
    void qv_sci_element(const QString& element, const QVariant& val, const int &precision );

    /*!
     * \brief qv_sci_element
     * \param element
     * \param attributes
     * \param val
     * \param precision
     */
    void qv_sci_element(const QString& element, const QString& attributes, const QVariant& val, const int &precision );




    /*!
     * \brief comment
     * \param comment text
     * \param linebreak_at say -1 if all in on line; say 50 if after 50 chars the next space in text will added with line break
     */
    void comment(const QString &comment, const int &linebreak_at = 60 );

    /*!
     * \brief comment_multi_line list of comments
     * \param comment_lines List of comments to be put line by line
     */
    void comment_multi_line(const QStringList &comment_lines);

    void emptyelement( const QString &element );
    void emptyelement( const QString &element, const QString& attributes );

    /*!
     * \brief pop closes a section
     * \param reminder leave empty or use the value used for "push"
     */
    void pop(const QString &reminder = "");
    void add_free_string( const QString& element );
    void clear(const bool write_header_again = true);

    void append(const QString &xmlsection);
    void insert(const QString &xmlsection);


private:
    QString buf;                //!< buffer for the textstream
    QTextStream xml;            //!< contains the complete XML including indents
    QString indentation;        //!< indent for better human reading; eg. two spaces
    QVector<QString> stack;     //!< stack push



};

/*!
write a element or data entry
dont forget to use val.c_str() in main in case val is a STRING
*/
template<class T>  void  tinyxmlwriter::element ( const QString& element, T val )
{
    indent();
    xml << "<" << element  << ">" << val << "</" << element << ">"  << Qt::endl;

}

template<class T>  void  tinyxmlwriter::element ( const QString& element, const QString& str_attribute, const int &int_value, T val )
{
    indent();

    xml << "<" << element  << " " << str_attribute << "=\"" << QString::number(int_value) << "\">" << val << "</" << element << ">"  << Qt::endl;

}

template<class T>  void  tinyxmlwriter::element ( const QString& element, const QString& attributes, T val )
{
    indent();
    xml << "<" << element << " " << attributes << ">" << val << "</" << element << ">"  << Qt::endl;

}

/*!
write a scientific number without loss of data as ascii like 1.07980000e-01
element shall be float or double; no ckeck here
*/
template<class T>  void  tinyxmlwriter::sci_element ( const QString& element, T val, const int &precision )
{
    indent();
    int prec = 6;
    if (precision > 0) prec = precision;
    xml.setRealNumberPrecision (prec);
    xml.setRealNumberNotation (QTextStream::ScientificNotation);
    xml << "<" << element << ">"
        << val << "</" << element << ">"  << Qt::endl;


}

//!< @todo do I have to unset the real number notation ?

template<class T>  void  tinyxmlwriter::sci_element ( const QString& element, const QString& attributes, T val, const int &precision )
{
    indent();
    int prec = 6;
    if (precision > 0) prec = precision;
    xml.setRealNumberPrecision (prec);
    xml.setRealNumberNotation (QTextStream::ScientificNotation);
    xml << "<" << element  << " " << attributes << ">"
        << val << "</" << element  << ">"  << Qt::endl;

}



#endif // tinyxmlwriter_H
