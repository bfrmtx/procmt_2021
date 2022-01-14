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

#ifndef FREEKEYPADWINDOW_H
#define FREEKEYPADWINDOW_H

#include <QDialog>
#include <QMap>
#include <QPushButton>
#include <QString>
#include <QSettings>
#include <QVariant>
#include <QKeyEvent>
#include <QDebug>
#include <limits>
#include <QSettings>


enum num_keypad_InputMode
{
    UnsignedIntegerMode = 1,
    IntegerMode   = 2,
    UnsignedDoubleMode   =3,
    DoubleMode = 4
};

namespace Ui {
class num_keypadgui;
}


/*!
 * \brief The num_keypad class a gui keypad; hence that this keypad works with numbers and not with strings
 */
class num_keypad : public QDialog
{
    Q_OBJECT
public:



    explicit num_keypad( QString title, num_keypad_InputMode ipt = num_keypad_InputMode::DoubleMode, QWidget *parent = Q_NULLPTR);

    /** \brief Handler for keyboard input.
     * Allows to use keyboard instead of buttons
     * \param event Qt Keyboard event
     */
    void keyPressEvent(QKeyEvent *event);

    /** \brief Access input value as int.
     * \return Converted value
     */

    /** \brief Access input value.
     * \return Current input string
     */
    QString    toText() const;

    /*!
     * \brief toUint
     * \return last succsessful conversion to uint
     */
    uint       toUint() const;
    double     toDouble() const;
    int        toInt() const;
    QVariant   value() const;

    //inline void setTitle( const QString& newTitle ) { ui.titleLabel->setText( newTitle ); }

    void setInitialValue(const QVariant &value );
    void setMaxMinValue( const QVariant &max, const QVariant &min );
    void setClearOnFirstKey();

    virtual ~num_keypad();

private slots:
    /** \brief Handler for button clicks.
     * The calling button is decode in the handler
     */
    void buttonClicked();

    void on_valueLine_textChanged(const QString &arg1);

    void on_num_keypadgui_accepted();

protected:

    virtual void closeEvent( QCloseEvent *);
    virtual void showEvent ( QShowEvent * );

    /** \brief Enables the given set of buttons.
     * \param buttons String with chars to enable
     */
    void enableButtons(const QString buttons);

    /** \brief Process all input  (Buttons and keys).
     * \param key Key triggered
     */
    void keyPress(const QChar key);

    /** \brief Internal helper: map a button to a certain key.
     */
    void mapButton(QPushButton* button, const QChar c);


    QMap<QPushButton*,QChar>    qmButtons;
    QString             qstrInput;
    QString             qstrInput_tmp;

    QString             qstrLegalKeys;
    bool                mSignedValuesAllowed;


    bool convok = false;
    bool wait_for_next_char = false;

    bool clear_onfirstkey = false;

    QVariant keypad_value;                 //!< this is the master
    QVariant keypad_value_tmp;
    QVariant keypad_value_string;
    QVariant min;
    QVariant max;

    num_keypad_InputMode InputMode;


private:
    Ui::num_keypadgui *ui;

    QString valids_dbl =  QString("0123456789.-");
    QString valids_udbl = QString("0123456789.");
    QString valids_int =  QString("0123456789-");
    QString valids_uint = QString("0123456789");
    QChar minus = QChar('-');
    QChar dot = QChar('.');
    QChar comma = QChar(',');

    void clear(const bool clear_max_min = false);

    /*!
     * \brief contains_valid_char test if we want this character at all
     * \param lastchar
     * \return false in case we dont want it for the number string
     */
    bool contains_valid_char(const QChar &lastchar, const QString &input_line) const;

    /*!
     * \brief convert_to_number convertes to  keypad_value; checks also min max
     * \return true or false; sets also keypad_value_tmp = keypad_value if true or set wait_next_char
     */
    bool convert_to_number(const QString &input_line);


    /*!
     * \brief QVariant_to_double_with_dot : QVariant to String can not create 1. or 2. it make 1 or only; set the text in keypad
     * \param input_line input line with a dot on case
     * \param can_be_valid means that the previous check was negative and the last character has to be removed
     * \return
     */
    QString QVariant_to_double_with_dot(const QString &input_line, const bool &can_be_valid);


    /*!
     * \brief number_string_check checks if a string can be converted in to (u)int and (u)double
     * \param input_str
     * \return input_str or last valid str
     */
    bool number_string_check(const QString &input_str);



};

#endif // FREEKEYPADWINDOW_H
