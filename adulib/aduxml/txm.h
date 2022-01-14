#ifndef TXM_H
#define TXM_H

#include <vector>
#include <QObject>
#include <QString>
#include <QMap>
#include "geocoordinates.h"
#include "tinyxmlwriter.h"
#include "measdocxml.h"
#include "memory"
#include "qtx_templates.h"


/*!
 * \brief The TXM class for setting the TXM
 */
class TXM  : public QObject
{
  Q_OBJECT

public:
    TXM(const QString name_node = "TXM22Hardware", QObject *parent = Q_NULLPTR);

    bool set(const QString &what, const QVariant &value);

    QVariant get(const QString &what) const;

    /*!
     * \brief is_active checks if we have set values
     * \return false if nothing was set OR! BaseFrequency < 0.0000001 - that means I can not process data
     */
    bool is_active();

    QString getXmlStr() const;

    bool contains(const QString &what);

public slots:

    void slot_txm_cycle_qmap_created(const QMap<QString, QVariant> &data_map);
    void slot_txm_sequence_qmap_created(const QMap<QString, QVariant> &data_map);
    void slot_txm_waveform_qmap_created(const QMap<QString, QVariant> &data_map);
    void slot_txm_epos_qmap_created(const QMap<QString, QVariant> &data_map);


protected:

    QString name_node;
    QMap<QString, QVariant> txm_waveform;  //!< all data ... Waveform not as linear as inside ADU, otherwise this must be three classes
    QMap<QString, QVariant> txm_sequence;  //!< all data ... Sequence not as linear as inside ADU, otherwise this must be three classes
    QMap<QString, QVariant> txm_cycle;  //!< all data ... Cycle  not as linear as inside ADU, otherwise this must be three classes
    QMap<QString, QVariant> txm_epos;


    std::unique_ptr<measdocxml> measdoc = nullptr;

    bool is_set = false;


};

#endif // TXM_H
