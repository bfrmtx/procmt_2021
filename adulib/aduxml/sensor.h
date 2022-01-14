#ifndef SENSOR_H
#define SENSOR_H

#include "cable.h"

/*!
   \brief The sensor class
 */
class sensor : public cable
{

public:
    sensor(const int &num);

    /*!
       \brief operator = copies the slot number only and Type/Serial/RevMain,RevSub, GMS; the data/prvdata will be copied in the channel only!
       \param rhs
       \return
     */
    sensor& operator = (const sensor &rhs);

    /*!
       \brief operator == compares the item BUT NOT the status or values
       \param rhs
       \return
     */
    bool operator == (const sensor& rhs);

    virtual QVariant get(const QString &what) const;

    /*!
     * \brief set ref \ref adulib::set()
     * \param what
     * \param value
     * \return
     */
    virtual bool set(const QString &what, const QVariant &value);


    void init_aliases(const QStringList &sensors, const QStringList &sensor_aliases, const QList<int> &sensors_input_divider);


protected:
    int Type = 0;                       //!< database ID
    int Serial = 0;                     //!< database ID
    int RevMain = 0;                    //!< database ID
    int RevSub = 0;                     //!< database ID
    int GMS = 0;                        //!< database ID
    QString Name;                       //!< database ID - readable name


    QMap<QString, QString> map_names;   //!< maps North to pos_x2, South to pos_x1, West to pos_y1, East to pos_y2

    QStringList sensor_aliases;         //!< alias names from XML and atsheader (limited space!)
    QStringList sensors;                //!< List from database
    QList<int> sensors_input_divider;



};

#endif // SENSOR_H
