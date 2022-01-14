#ifndef GPS_H
#define GPS_H

#include <QObject>
#include <QString>
#include <QMap>
#include <memory>
#include "geocoordinates.h"
#include "tinyxmlwriter.h"
#include "measdocxml.h"
#include "qtx_templates.h"

class GPS  : public QObject
{
  Q_OBJECT

public:
    GPS(QObject *parent = Q_NULLPTR);
    ~GPS();

    bool set(const QString &what, const QVariant &value);

    QVariant get(const QString &what) const;

    bool contains(const QString &what);

    QString getXmlStr() const;

    bool is_active() const;



public slots:

    void slot_gps_status_qmap_created(const QMap<QString, QVariant> &map);

protected:
    QMap<QString, QVariant> data;
    std::unique_ptr<measdocxml> measdoc = nullptr;

    bool is_set = false;


};

#endif // GPS_H
