#ifndef CABLE_H
#define CABLE_H

#include "adbboard.h"

/*!
   \brief The cable class this class is a future extension for active cables
 */
class cable : public adbboard
{

public:
    cable(const int &num);

    /*!
       \brief operator = copies the slot number only and Type/Serial/RevMain,RevSub, GMS; the data/prvdata will be copied in the channel only!
       \param rhs
       \return
     */
    cable& operator = (const cable &rhs);

    /*!
       \brief operator == compares the item BUT NOT the status or values
       \param rhs
       \return
     */
    bool operator == (const cable& rhs);

    virtual QVariant get(const QString &what) const;

    /*!
     * \brief set ref \ref adulib::set()
     * \param what
     * \param value
     * \return
     */
    virtual bool set(const QString &what, const QVariant &value);



protected:
    double length = 0;                      //!< database ID
    int Type = 0;                           //!< database ID
    int Serial = 0;                         //!< database ID
    int RevMain = 0;                        //!< database ID
    int RevSub = 0;                         //!< database ID
    int GMS = 0;                            //!< database ID
    QString Name;                           //!< database ID - readable name
};




#endif // CABLE_H
