#include "qwebdav.h"

const char * FORMAT_RFC1123         = "ddd', 'dd MMM yyyy hh:mm:ss' GMT'";
const char * XML_MULTISTATUS        = "D:multistatus";
const char * XML_RESPONSE           = "D:response";
const char * XML_PROPSTAT           = "D:propstat";
const char * XML_PROP               = "D:prop";
const char * XML_PROP_HREF          = "D:href";
const char * XML_PROP_CONTENTTYPE   = "D:getcontenttype";
const char * XML_PROP_CONTENTLENGTH = "D:getcontentlength";
const char * XML_PROP_ETAG          = "D:getetag";
const char * XML_PROP_LAST_MODIFIED = "D:getlastmodified";

using namespace webdav;

ContentSubtype parseSubType(QString const & str) {
    if(str == "unix-directory") return ContentSubtype::UnixDirectory;
    if(str == "pdf") return ContentSubtype::PDF;
    if(str == "xml") return ContentSubtype::XML;
    if(str == "doc") return ContentSubtype::DOC;
    if(str == "json") return ContentSubtype::JSON;
    if(str == "octet-stream") return ContentSubtype::OctetStream;
    return ContentSubtype::Unknown;
}

ContentType parseType(QString const & str) {
    if(str == "application") return ContentType::Application;
    if(str == "image") return ContentType::Image;
    if(str == "audio") return ContentType::Audio;
    if(str == "text") return ContentType::Text;
    if(str == "multipart") return ContentType::Multipart;
    if(str == "httpd") return ContentType::Httpd;
    return ContentType::Unknown;
}


WebDavResponse WebDavResponse::parseResponse(const QString &response) {
    return WebDavResponse(response);
}

const QVector<Response> &WebDavResponse::getResponses() const {
    return m_responses;
}

WebDavResponse::WebDavResponse(const QString &response) {
    // Qt6
    /*
    QDomDocument doc;
    doc.setContent(response);

    QDomNode n_mstatus = doc.firstChildElement(XML_MULTISTATUS);
    if(n_mstatus.nodeName() == XML_MULTISTATUS) {
        QDomElement n_response = n_mstatus.firstChildElement(XML_RESPONSE);
        while(!n_response.isNull()) {
            this->m_responses.push_back(Response());
            auto & new_response = this->m_responses.back();

            QDomElement n_propstat = n_response.firstChildElement(XML_PROPSTAT);
            if(!n_propstat.isNull()) {
                QDomElement n_property = n_propstat.firstChildElement(XML_PROP);
                if(!n_property.isNull()) {
                    QDomNode n_prop = n_property.firstChild();
                    while(!n_prop.isNull()) {
                        QDomElement n_elem = n_prop.toElement();
                        auto name = n_elem.nodeName();
                        if(name == XML_PROP_CONTENTLENGTH) {
                            if(n_elem.text().length() > 0) {
                                new_response.content_length = n_elem.text().toULongLong();
                            }
                        } else if(name == XML_PROP_CONTENTTYPE) {
                            auto split = n_elem.text().split('/');
                            if(split.size() == 2) {
                                new_response.type = parseType(split[0].toLower());
                                new_response.subtype = parseSubType(split[1].toLower());
                            }
                        } else if(name == XML_PROP_ETAG) {
                            new_response.eTag = n_elem.text();
                        } else if(name == XML_PROP_LAST_MODIFIED) {
                            auto date = QLocale(QLocale::Language::English).toDateTime(n_elem.text(), FORMAT_RFC1123);
                            if(date.isValid()) {
                                new_response.last_modified.reset(new QDateTime(date));
                            }
                        }
                        n_prop = n_prop.nextSibling();
                    }
                }
            }

            QDomElement n_href = n_response.firstChildElement(XML_PROP_HREF);
            if(!n_href.isNull()) {
                new_response.ref = n_href.text();
            }

            n_response = n_response.nextSiblingElement(XML_RESPONSE);
        }
    }

    */
}


WebDavResponse::operator QString() const {
    QString ret;
    ret += "[WebDavResponse]" + QString('\n');
    for(auto & prop : this->m_responses) {
        ret += QString(prop);
    }
    return ret;
}

Response::operator QString() const {
    QString ret;
    ret += "[Property]" + QString("\n");
    ret += "Ref: " + this->ref + QString("\n");
    ret += "content length: " + QString::number(this->content_length) + QString("\n");
    ret += "creation date : " + (this->creation.isNull() ? "null" : this->creation.get()->toString()) + QString("\n");
    ret += "last modified : " + (this->last_modified.isNull() ? "null" : this->last_modified.get()->toString()) + QString("\n");
    ret += "e tag         : " + this->eTag + QString("\n");
    ret += "type          : " + QString::number(static_cast<int>(this->type)) + QString("\n");
    ret += "subtype       : " + QString::number(static_cast<int>(this->subtype)) + QString("\n");
    return ret;
}
