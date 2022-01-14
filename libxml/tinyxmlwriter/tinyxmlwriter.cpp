#include "tinyxmlwriter.h"



/*!
constructor
initialize object and set the this->xml init QString
*/
tinyxmlwriter::tinyxmlwriter(const bool write_header)
{
    this->xml.setString(&buf);
    this->indentation = "  ";
    this->xml.setEncoding(QStringConverter::Utf8);
    if (write_header) {
        this->xml << "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>" << Qt::endl;
    }
}



void tinyxmlwriter::header()
{
    this->xml << "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>" << Qt::endl;
}

tinyxmlwriter::tinyxmlwriter(const tinyxmlwriter &rhs)
{
    *this = rhs;
}

tinyxmlwriter &tinyxmlwriter::operator =(const tinyxmlwriter &rhs)
{
    this->clear(false);
    this->xml << rhs.buf;
    return *this;
}

/*!
destructor
*/
tinyxmlwriter::~tinyxmlwriter() {}

/*!
control the this->indents for each level; deping to the amount of this->stacks the amount of this->indents is written
*/
void tinyxmlwriter::indent()
{
    for ( int i = 0; i < this->stack.size(); i++ )
    {
        this->xml << this->indentation;
    }
}

/*!
return the complete this->xml data as QString; uses the QString::readAll() function
*/
QString tinyxmlwriter::getXmlStr() const
{
    return  this->buf;
}



/*!
open a new level and remember the level name
the closing function pop() will automatically
write the /element entry
*/
void tinyxmlwriter::push ( const QString& element )
{
    this->indent();
    this->xml << "<" << element << ">" << Qt::endl;
    this->stack.push_back ( element );
}
void tinyxmlwriter::push( const QString& element, const QString& attributes )
{
    this->indent();
    this->xml << "<" << element << " " << attributes << ">" << Qt::endl;
    this->stack.push_back ( element );
}
void tinyxmlwriter::push (const QString& element, const QString& str_attribute, const int &int_value) {
    this->indent();

    this->xml << "<" << element << " " << str_attribute << "=\"" << QString::number(int_value) << "\">" << Qt::endl;
    this->stack.push_back ( element );
}

/**
 * instead of the template function here a QString function;
  for some reason the template needs alwas a string_val
  and not simply a string_val; this leeds to consequent errors which are
  herewith eleminated
 * @param element tag name
 * @param val input QString
 * @return
 */
void tinyxmlwriter::element( const QString& element, const QString& val )
{
    this->indent();
    this->xml << "<" << element  << ">" << val << "</" << element << ">" << Qt::endl;
}


void tinyxmlwriter::element( const QString& element, const QString& str_attribute, const int &int_value, const QString& val )
{
    this->indent();
    this->xml << "<" << element  << " " << str_attribute << "=\"" << QString::number(int_value) << "\">" << val << "</" << element << ">" << Qt::endl;
}



/**
 * instead of the template function here a QString function;
  for some reason the template needs alwas a string_val
  and not simply a string_val; this leeds to consequent errors which are
  herewith eleminated
 * @param element
 * @param attributes
 * @param val
 * @return
 */
void tinyxmlwriter::element( const QString& element, const QString& attributes, const QString& val )
{
    this->indent();
    this->xml << "<" << element << " " << attributes << ">" << val << "</" << element << ">" << Qt::endl;
}

void tinyxmlwriter::qv_int_element(const QString &element, const QVariant &val)
{
    qint64 i = val.toLongLong();
    this->indent();
    xml << "<" << element  << ">" << i << "</" << element << ">" << Qt::endl;

}

void tinyxmlwriter::qv_int_element(const QString &element, const QString &attributes, const QVariant &val)
{
    qint64 i = val.toLongLong();
    this->indent();
    this->xml << "<" << element << " " << attributes << ">" << i << "</" << element << ">" << Qt::endl;

}

void tinyxmlwriter::qv_uint_element(const QString &element, const QVariant &val)
{
    quint64 i = val.toULongLong();
    this->indent();
    xml << "<" << element  << ">" << i << "</" << element << ">" << Qt::endl;
}

void tinyxmlwriter::qv_uint_element(const QString &element, const QString &attributes, const QVariant &val)
{
    quint64 i = val.toULongLong();
    this->indent();
    this->xml << "<" << element << " " << attributes << ">" << i << "</" << element << ">" << Qt::endl;
}

void tinyxmlwriter::qv_double_element(const QString &element, const QVariant &val)
{
    double d = val.toDouble();
    this->indent();
    xml << "<" << element  << ">" << d << "</" << element << ">" << Qt::endl;
}

void tinyxmlwriter::qv_double_element(const QString &element, const QString &attributes, const QVariant &val)
{
    double d = val.toDouble();
    this->indent();
    this->xml << "<" << element << " " << attributes << ">" << d << "</" << element << ">" << Qt::endl;
}

void tinyxmlwriter::qv_string_element(const QString &element, const QVariant &val)
{
    QString str  = val.toString();
    this->indent();
    xml << "<" << element  << ">" << str << "</" << element << ">" << Qt::endl;
}


void tinyxmlwriter::qv_string_element(const QString &element, const QString &attributes, const QVariant &val)
{
    QString str  = val.toString();
    this->indent();
    this->xml << "<" << element << " " << attributes << ">" << str << "</" << element << ">" << Qt::endl;
}

void tinyxmlwriter::time_hh_mm_ss(const QString &element, const QDateTime &datetime)
{
    QString stime = datetime.toString("HH:mm:ss");
    this->indent();
    xml << "<" << element  << ">" << stime << "</" << element << ">" << Qt::endl;
}

void tinyxmlwriter::date_yyyy_mm_dd(const QString &element, const QDateTime &datetime)
{
    QString sdate = datetime.toString("yyyy-MM-dd");
    this->indent();
    xml << "<" << element  << ">" << sdate << "</" << element << ">" << Qt::endl;
}

void tinyxmlwriter::qv_sci_element(const QString &element, const QVariant &val, const int &precision)
{
    double d = val.toDouble();
    this->indent();
    int prec = 6;
    if (precision > 0) prec = precision;
    xml.setRealNumberPrecision (prec);
    xml.setRealNumberNotation (QTextStream::ScientificNotation);
    xml << "<" << element << ">"
        << d << "</" << element << ">" << Qt::endl;
}

void tinyxmlwriter::qv_sci_element(const QString &element, const QString &attributes, const QVariant &val, const int &precision)
{
    double d = val.toDouble();
    this->indent();
    int prec = 6;
    if (precision > 0) prec = precision;
    xml.setRealNumberPrecision (prec);
    xml.setRealNumberNotation (QTextStream::ScientificNotation);
    xml << "<" << element  << " " << attributes << ">"
        << d << "</" << element  << ">" << Qt::endl;
}


void tinyxmlwriter::comment (const QString &comment , const int &linebreak_at)
{
    this->indent();

    if (linebreak_at == -1) {

         this->xml << "<!--" << comment <<  "-->"   << Qt::endl;
        return;
    }

    QStringList comments = comment.split(" ");
    int len = 0;
    this->xml << "<!--"  << Qt::endl;
    this->indent(); this->xml << "    ";

    for (auto &str : comments) {
        this->xml << str << " ";
        len += str.size();
        if (len > linebreak_at) {
            this->xml << Qt::endl;
            len = 0;
            this->indent(); this->xml << "    ";
        }

    }
    //this->xml << comment << Qt::endl;
    this->xml << Qt::endl;
    this->indent();

    this->xml << "-->"   << Qt::endl;
}

void tinyxmlwriter::comment_multi_line(const QStringList &comment_lines)
{
    this->indent();
    this->xml << "<!--"  << Qt::endl;
    for (auto &str : comment_lines) {
        this->indent(); this->xml << "    " << str << Qt::endl;
    }
    this->indent();
    this->xml << "-->"   << Qt::endl;
}

void tinyxmlwriter::emptyelement(const QString &element )
{
    this->indent();
    this->xml << "<" << element << "/>" << Qt::endl;
}

void tinyxmlwriter::emptyelement (const QString &element, const QString& attributes )
{
    this->indent();
    this->xml << "<" << element << " " << attributes << "/>" << Qt::endl;
}

/*!
refer to push;
closes one level
*/
void tinyxmlwriter::pop(const QString &reminder)
{
    if (stack.size()) {
        QString element = this->stack[stack.size()-1];
        this->stack.pop_back();
        this->indent();

        this->xml << "</" << element << ">" << Qt::endl;
        return;
    }
    else {
        std::cerr << "tinyxmlwriter::pop() -> this->stack is empty, " << reminder.toStdString() << std::endl;
    }
}

void tinyxmlwriter::add_free_string( const QString& element )
{
    this->indent();
    this->xml << "<" << element << ">" << Qt::endl;
}

void tinyxmlwriter::clear(const bool write_header_again)
{
    this->xml.flush();
    this->xml.reset();
    this->stack.clear();
    buf.clear();
    this->xml.setString(&buf);
    this->xml.setEncoding(QStringConverter::Utf8);
    if (write_header_again) {
        this->xml << "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>" << Qt::endl;
    }
}

void tinyxmlwriter::append(const QString &xmlsection)
{
    this->xml << xmlsection;
}

void tinyxmlwriter::insert(const QString &xmlsection)
{
    QStringList lines = xmlsection.split("\n");
    for (auto &line : lines) {
        if (line.size()) {
            this->indent();
            this->xml << line << "\n";
        }
    }
}
