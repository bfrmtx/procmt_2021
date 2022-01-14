#ifndef ADBBOARD_H
#define ADBBOARD_H

#include <cmath>
#include "aduslot.h"

/*!
   \brief The board class
 */


class adbboard : public aduslot
{

public:
    adbboard(const int &num);


    /*!
       \brief operator = copies the slot number only and Type/Serial/RevMain,RevSub, GMS; the data/prvdata will be copied in the channel only!
       \param rhs
       \return
     */
    adbboard& operator = (const adbboard &rhs);



    /*!
       \brief operator == compares the item BUT NOT the status or values
       \param rhs
       \return
     */
    bool operator == (const adbboard &rhs);

    virtual QVariant get(const QString &what) const;

    /*!
     * \brief set ref \ref adulib::set()
     * \param what
     * \param value
     * \return
     */
    virtual bool set(const QString &what, const QVariant &value);


protected:
    // 10 LF, 11 HF, 13 BB (no switch of channels)
    int Type = 0;                                   //!< database ID
    int Serial = 0;                                 //!< database ID
    int RevMain = 0;                                //!< database ID
    int RevSub = 0;                                 //!< database ID
    int GMS = 0;                                    //!< database ID
    QString Name;                                   //!< database ID - readable name

    // repeat this data in operator = in case
    std::vector<double> SampleFreqs;                //!< all available sample frequencies for this board
    // take for 07e, mask for 08
    std::vector<double> GainsStage1;                //!< gain stage 1
    std::vector<double> GainsStage2;                //!< gain stage 2
    std::vector<double> DAC;                        //!< DAC offset; will be set to steps of 100; shall be off mostly, <DAC>-900, 900</DAC> from HwDatabase
    QStringList filters;                            //!< available filters such as ADU07_LF_RF_4 or ADU07_HF_HP_1HZ
    QStringList filters_hf;                         //!< filters for this part of the ADB
    QStringList filters_lf;                         //!< filters for this part of the ADB
    QString input_circuitry;                        //!< HF or LF for ADU-08e that is similar to change the board from HF to LF

    QStringList auto_filter_type_main;
    QList<int>  auto_input;
    QStringList auto_input_circuitry;
    QList<double> auto_gain_stage1;
    QList<double> auto_gain_stage2;
    QStringList auto_filter_type;
    QList<double> auto_dac_val;
    QList<int>    auto_dac_on;

private :
    /*!
     * \brief traverse find the autogain settings and set the gains coreespondingly
     * \param input
     * \param input_circuitry
     * \param filter_type_main
     */
    void traverse(const int &tinput, const QString &tinput_circuitry, const QString &tfilter_type_main);

};

#endif // ADBBOARD_H
