#ifndef CHANNEL_H
#define CHANNEL_H

#include <QUrl>
#include <memory>
#include <QObject>

#include "sensor.h"

/*!
   \brief The channel class - sets the values for all
 */
class channel : public QObject, public sensor
{

    Q_OBJECT
public:
    channel(const int &num, QObject *parent = 0);

    channel(const channel& rhs);


    /*!
       \brief operator = copies the slot number and Type/Serial/RevMain,RevSub, GMS; AND ALL DATA
       \param rhs
       \return
     */
    channel& operator = (const channel &rhs);

    /*!
       \brief operator == makes a hardware comparision only
       \param rhs
       \return
     */
    bool operator == (const channel& rhs);


    virtual QVariant get(const QString &what) const;

    /*!
     * \brief set ref \ref adulib::set()
     * \param what
     * \param value
     * \return
     */
    virtual bool set(const QString &what, const QVariant &value);

    QVariant get_data_from_index(const int &idx) const;
    QString get_name_from_index(const int &idx) const;
    void get_name_and_data_from_index(const int &idx, QVariant &name, QVariant &value) const;


    QMap<QString, QVariant> get_data() const;
    void set_data(const QMap<QString, QVariant> &setdata);


    void set_Type(const QString &classname, const int &value);
    void set_Serial(const QString &classname, const int &value);
    void set_RevMain(const QString &classname, const int &value);
    void set_RevSub(const QString &classname, const int &value);
    void set_GMS(const QString &classname, const int &value);

    /*!
     * \brief set_Name set the name of sensor_type like EFP-06 or board and so on
     * \param classname use sensor (== classname) to the the sensor type and adbboard to set the name of the ADB board
     * \param value
     */
    void set_Name(const QString classname, const QString value);



    int get_Type(const QString &classname) const;
    int get_Serial(const QString &classname) const;
    int get_RevMain(const QString &classname) const;
    int get_RevSub(const QString &classname) const;
    int get_GMS(const QString &classname) const;
    QString get_Name(const QString &classname) const;

    int copy_item_xmlvals(const QString classname, const channel &chan);


    bool has_board(const int &type, const int &revmain, const int &revsub, const int &gms, QString &classname);

    void append_sample_freq(const double &freq);
    void append_gain_stage1(const double &gain);
    void append_gain_stage2(const double &gain);
    void append_filter(const QString &filter);
    void append_filter_lf(const QString &filter);
    void append_filter_hf(const QString &filter);
    void append_dac(const double &dac);


    /*!
       \brief replace_UNKN_E with no sensor detection the fallback for E is unkown - change to EFP her
     */
    void replace_UNKN_E();

    /*!
       \brief set_dac_steps - it is not usefull to set DAC to 20; set it to 100, 200 and so on
     */
    void set_dac_steps(const double &stepsizemV = 100.);



    std::vector<double> get_avail_board_frequencies(const bool &ignore_off = false) const;

    QStringList get_avail_channel_types() const;

    QMap<QString, QVariant> fetch_selftest_part(const QMap<QString, QVariant> &xmlpart);



    QMap<QString, QVariant> get_selftest_template() const;

    QMap<QString, QVariant> get_atswriter_template() const;

public slots:

    void slot_fetch_atswriter(const QMap<QString, QVariant> &xmlpart, const int &id_slot);
    void slot_adbboard_autogains(const QMap<QString, QVariant> &xmlpart, const int &id_slot);
    void slot_act_gains(const QMap<QString, QVariant> &xmlpart, const int &id_slot);

private:

    QStringList all_classes;
    QStringList e_channel_types;
    QStringList h_channel_types;
    QStringList general_channel_types;
    QStringList all_channel_types;







};




#endif // CHANNEL_H
