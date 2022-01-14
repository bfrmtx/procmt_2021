#include "num_keypad.h"
#include "ui_num_keypad.h"



num_keypad::num_keypad(QString title, num_keypad_InputMode ipt, QWidget *parent ) :
    QDialog(parent), ui(new Ui::num_keypadgui)


{
    this->ui->setupUi(this);
    ui->titleLabel->setText(title);
    this->InputMode = ipt;


    //    mHaveMaxValue( false ),
    //    mHaveMinValue( false ),
    //    mClearOnFirstKey( false )

    QSettings settings;
    restoreGeometry( settings.value("Keypad/geometry").toByteArray() );

    mapButton(this->ui->backButton, 'B');
    mapButton(this->ui->clearButton, 'C');
    mapButton(this->ui->negButton, 'N' );
    mapButton(this->ui->dotButton, '.' );
    mapButton(this->ui->plusButton, 'I');
    mapButton(this->ui->minusButton, 'D');
    mapButton(this->ui->_0Button, '0');
    mapButton(this->ui->_1Button, '1');
    mapButton(this->ui->_2Button, '2');
    mapButton(this->ui->_3Button, '3');
    mapButton(this->ui->_4Button, '4');
    mapButton(this->ui->_5Button, '5');
    mapButton(this->ui->_6Button, '6');
    mapButton(this->ui->_7Button, '7');
    mapButton(this->ui->_8Button, '8');
    mapButton(this->ui->_9Button, '9');

    this->clear(true);

}

void num_keypad::clear(const bool clear_max_min)
{
    qstrInput.clear();
    if (this->InputMode == num_keypad_InputMode::IntegerMode) {
        this->keypad_value = int(0);
        this->keypad_value_tmp = int(0);
        if (clear_max_min) this->max = int(0);
        if (clear_max_min) this->min = int(0);
    }

    else if (this->InputMode == num_keypad_InputMode::UnsignedIntegerMode) {
        this->keypad_value = uint(0);
        this->keypad_value_tmp = uint(0);
        if (clear_max_min) this->max = uint(0);
        if (clear_max_min) this->min = uint(0);
    }

    else if (this->InputMode == num_keypad_InputMode::UnsignedDoubleMode) {
        this->keypad_value = double(0.);
        this->keypad_value_tmp = double(0.);
        if (clear_max_min) this->max = double(0.);
        if (clear_max_min) this->min = double(0.);
    }


    else if (this->InputMode == num_keypad_InputMode::DoubleMode) {
        this->keypad_value = double(0.);
        this->keypad_value_tmp = double(0.);
        this->max = double(0.);
        this->min = double(0.);
    }

    this->convok = false;
    this->wait_for_next_char = false;
    this->ui->valueLine->setText(qstrInput);

}

bool num_keypad::contains_valid_char(const QChar &lastchar, const QString &input_line) const
{
    bool can_be_valid = false;
    if (this->InputMode == num_keypad_InputMode::IntegerMode) {
        if (this->valids_int.contains(lastchar)) {
            can_be_valid = true;
        }
    }
    else if (this->InputMode == num_keypad_InputMode::UnsignedIntegerMode) {
        if (this->valids_uint.contains(lastchar)) {
            can_be_valid = true;
        }
    }
    else if (this->InputMode == num_keypad_InputMode::UnsignedDoubleMode) {
        if (this->valids_udbl.contains(lastchar)) {
            can_be_valid = true;
        }
    }
    else if (this->InputMode == num_keypad_InputMode::DoubleMode) {
        if (this->valids_dbl.contains(lastchar)) {
            can_be_valid = true;
        }
    }

    // . is a valid char but can't be used
    if (input_line.endsWith("..")) {
        can_be_valid = false;
    }

    // we already have a . somewhere else
    if (input_line.indexOf(this->dot) != input_line.lastIndexOf(this->dot) ) {
        can_be_valid = false;
    }
    if (input_line.indexOf(this->minus) != input_line.lastIndexOf(this->minus) ) {
        can_be_valid = false;
    }

    return can_be_valid;
}

bool num_keypad::convert_to_number(const QString &input_line)
{


    if (input_line.startsWith(QChar('.'))) {
        this->keypad_value_string = QChar('0') + input_line;
    }
    else if (input_line.startsWith(QChar('-')) && (input_line.size() == 1)) {
        this->wait_for_next_char = true;
        this->convok = false;
        return false;
    }
    else if (input_line.startsWith(QString("-0")) && (input_line.size() == 2)) {
        this->wait_for_next_char = true;
        this->convok = false;
        this->keypad_value = -0.;
        this->keypad_value_tmp = -0.;
        return false;
    }
    else if (input_line.startsWith(QString("-0.")) && (input_line.size() == 3)) {
        this->wait_for_next_char = true;
        this->keypad_value = -0.;
        this->keypad_value_tmp = -0.;
        this->convok = false;
        return false;
    }
    else this->keypad_value_string = input_line;

    if (this->InputMode == num_keypad_InputMode::IntegerMode) {
        this->keypad_value = this->keypad_value_string.toInt(&this->convok);


      if (this->convok) {
        if (this->min.toInt() < this->max.toInt()) {
            if ((this->keypad_value.toInt() >= this->min.toInt()) && (this->keypad_value.toInt() <= this->max.toInt())) {
                this->keypad_value_tmp = this->keypad_value.toInt();
            }
        }
        else {
            this->keypad_value_tmp = this->keypad_value.toInt();
        }
      }

    }
    else if (this->InputMode == num_keypad_InputMode::UnsignedIntegerMode) {
        this->keypad_value = this->keypad_value_string.toUInt(&this->convok);
      if (this->convok) {
        if (this->min.toUInt() < this->max.toUInt()) {
            if ((this->keypad_value.toUInt() >= this->min.toUInt()) && (this->keypad_value.toUInt() <= this->max.toUInt())) {
                this->keypad_value_tmp = this->keypad_value.toUInt();
            }
        }
        else {
            this->keypad_value_tmp = this->keypad_value.toUInt();
        }
      }

    }
    else if (this->InputMode == num_keypad_InputMode::UnsignedDoubleMode) {
        this->keypad_value = this->keypad_value_string.toDouble(&this->convok);
      if (this->convok) {
        if (this->min.toDouble() < this->max.toDouble()) {
            if ((this->keypad_value.toDouble() >= this->min.toDouble()) && (this->keypad_value.toDouble() <= this->max.toDouble())) {
                this->keypad_value_tmp = this->keypad_value.toDouble();
            }
        }
        else {
            this->keypad_value_tmp = this->keypad_value.toDouble();
        }
      }

    }
    else if (this->InputMode == num_keypad_InputMode::DoubleMode) {
        this->keypad_value = this->keypad_value_string.toDouble(&this->convok);

      if (this->convok) {
        if (this->min.toDouble() < this->max.toDouble()) {
            if ((this->keypad_value.toDouble() >= this->min.toDouble()) && (this->keypad_value.toDouble() <= this->max.toDouble())) {
                this->keypad_value_tmp = this->keypad_value.toDouble();
            }
        }
        else {
            this->keypad_value_tmp = this->keypad_value.toDouble();
        }
      }

    }

//     if (this->convok) {
//         if (this->min < this->max) {
//             if ((this->keypad_value >= this->min) && (this->keypad_value <= this->max)) {
//                 this->keypad_value_tmp = this->keypad_value;
//             }
//         }
//         else {
//             this->keypad_value_tmp = this->keypad_value;
//         }
//     }

    return this->convok;
}

QString num_keypad::QVariant_to_double_with_dot(const QString &input_line, const bool &can_be_valid)
{
    QString tmp = input_line;
    if (!can_be_valid) tmp.remove(tmp.size()-1,1);
    if (!tmp.size()) {
        this->clear(false);
        return tmp;
    }
    QChar lastchar = tmp.at(tmp.size()-1);
    QString endswith_dot_zero = ".0";


    // try for 0.0000X as along we are zero
    if ((this->keypad_value == 0) || (this->keypad_value == -0)) {
        this->qstrInput = tmp;
    }
    // QVariant does not make a dot for =.0
    else if (lastchar == this->dot) {
        this->qstrInput = (this->keypad_value_tmp.toString() + this->dot);
        if (tmp.size() > 1 && tmp.endsWith(endswith_dot_zero)) {
            this->qstrInput = (this->keypad_value_tmp.toString() ) + endswith_dot_zero;
        }
    }
    // we should have a valid number and use it
    else this->qstrInput = (this->keypad_value_tmp.toString());

    // keypad_value_tmp and keypad_value are the same now
    // keypad_value_string should represent the dobe/int value but with do n case
    this->keypad_value_string = this->qstrInput;
    this->ui->valueLine->setText(this->qstrInput);

    return this->qstrInput;
}


num_keypad::~num_keypad() {
    QSettings settings;
    settings.setValue("Keypad/geometry", saveGeometry());
}

void num_keypad::enableButtons(const QString buttons)
{
    qstrLegalKeys = buttons;
    foreach(QPushButton* button, qmButtons.keys())
    {
        button->setEnabled(buttons.contains(qmButtons[button]));
    }
}

void num_keypad::mapButton(QPushButton *button, const QChar c)
{
    qmButtons[button] = c;
    connect(button, &QPushButton::clicked, this, &num_keypad::buttonClicked);
}



void num_keypad::buttonClicked()
{
    QChar key = qmButtons[static_cast<QPushButton*>(sender())];
    keyPress(key);
}

void num_keypad::closeEvent(QCloseEvent * event )
{
    QSettings settings;
    settings.setValue("Keypad/geometry", saveGeometry());
    QDialog::closeEvent(event);
}

void num_keypad::showEvent ( QShowEvent * event )
{
    QSettings settings;
    restoreGeometry( settings.value("Keypad/geometry").toByteArray());
    QDialog::showEvent( event );
}

void num_keypad::keyPressEvent(QKeyEvent *event)
{
    QString text(event->text());
    QChar key;

    if (event->key() == Qt::Key_Delete)
        key = 'C';
    else if (event->key() == Qt::Key_Backspace)
        key = 'B';
    else if (event->key() == Qt::Key_Return ||
             event->key() == Qt::Key_Enter)
        accept();
    else if (text.length() == 1)
        key = text[0];
    else {
        QDialog::keyPressEvent(event);
        return;
    }
    if (key==',')key = '.';

    if (qstrLegalKeys.contains(key)) {
        event->accept();
        keyPress(key);
    }
    else
        QDialog::keyPressEvent(event);
}

QString num_keypad::toText() const
{
    return this->keypad_value.toString();
}

uint num_keypad::toUint() const
{
    return this->keypad_value.toUInt();
}

double num_keypad::toDouble() const
{
    return this->keypad_value.toDouble();
}

int num_keypad::toInt() const
{
    return this->keypad_value.toInt();
}

QVariant num_keypad::value() const
{
    return this->keypad_value;
}

void num_keypad::setInitialValue(const QVariant& value )
{
    this->ui->valueLine->setText(value.toString());
    //this->ui->valueLine->setText(this->qstrInput_tmp);

}

void num_keypad::setMaxMinValue(const QVariant &max, const QVariant &min)
{
    if (this->InputMode == num_keypad_InputMode::IntegerMode) {
        this->min = min.toInt();
        this->max = max.toInt();
    }
    else if (this->InputMode == num_keypad_InputMode::UnsignedIntegerMode) {

        this->min = min.toUInt();
        this->max = max.toUInt();
    }
    else {
        this->min = min.toDouble();
        this->max = max.toDouble();
    }
}


void num_keypad::setClearOnFirstKey()
{
    this->clear_onfirstkey = true;

}





bool num_keypad::number_string_check(const QString &input_str)
{
    if (!input_str.size()) return false;

    // we simply have to check where in case the minus sign is and cointains a dot

    QChar lastchar = input_str.at(input_str.size()-1);
    QString tmp_str = input_str;
    bool can_be_valid = false;


    // stage one : is valid input character ?
    can_be_valid = this->contains_valid_char(lastchar, input_str);

    if (!can_be_valid) this->QVariant_to_double_with_dot(input_str, can_be_valid);
    else {
        // stage two - try a conversion to a number
        if (this->convert_to_number(input_str)) {
            this->QVariant_to_double_with_dot(input_str, can_be_valid);
        }
        else {
            if (this->wait_for_next_char ) {
                this->qstrInput = input_str;
                this->wait_for_next_char = false;
            }
        }
    }
    return convok;
}




void num_keypad::keyPress(const QChar key)
{

    this->qstrInput_tmp = this->qstrInput;


    if ((this->InputMode == num_keypad_InputMode::DoubleMode) && (this->valids_dbl.contains(key))) {
        this->qstrInput_tmp.append(key);
        this->ui->valueLine->setText(this->qstrInput_tmp);
    }

    if ( (this->InputMode == num_keypad_InputMode::IntegerMode) && (this->valids_int.contains(key))) {
        this->qstrInput_tmp.append(key);
        this->ui->valueLine->setText(this->qstrInput_tmp);
    }

    if ( (this->InputMode == num_keypad_InputMode::UnsignedIntegerMode) && (this->valids_uint.contains(key))) {
        this->qstrInput_tmp.append(key);
        this->ui->valueLine->setText(this->qstrInput_tmp);
    }

    if ( (this->InputMode == num_keypad_InputMode::UnsignedDoubleMode) && (this->valids_udbl.contains(key))) {
        this->qstrInput_tmp.append(key);
        this->ui->valueLine->setText(this->qstrInput_tmp);
    }


    else if (key == QChar('B')) {
        if (qstrInput.size()) {
            this->qstrInput.remove(qstrInput.size()-1, 1);
            this->ui->valueLine->setText(qstrInput);
        }
    }
    else if (key == QChar('C')) {
        this->clear(false);
        this->qstrInput.clear();
    }

    else if (key == QChar('N')) {
        if (this->InputMode == num_keypad_InputMode::IntegerMode) {
            this->keypad_value = -1 * this->keypad_value.toInt();
            this->ui->valueLine->setText(this->keypad_value.toString());
        }
        else if (this->InputMode == num_keypad_InputMode::DoubleMode) {
            this->keypad_value = -1. * this->keypad_value.toDouble();
            this->ui->valueLine->setText(this->keypad_value.toString());
        }
    }

    else if (key == QChar('I')) {
        if (this->InputMode == num_keypad_InputMode::IntegerMode) {
            this->keypad_value = this->keypad_value.toInt() + 1;
            this->ui->valueLine->setText(this->keypad_value.toString());
        }
        else if (this->InputMode == num_keypad_InputMode::DoubleMode) {
            this->keypad_value = this->keypad_value.toDouble() + 1.;
            this->ui->valueLine->setText(this->keypad_value.toString());
        }
        else if (this->InputMode == num_keypad_InputMode::UnsignedDoubleMode) {
            this->keypad_value = this->keypad_value.toDouble() + 1.;
            this->ui->valueLine->setText(this->keypad_value.toString());
        }
        else if (this->InputMode == num_keypad_InputMode::UnsignedIntegerMode) {
            this->keypad_value = this->keypad_value.toInt() + 1.;
            this->ui->valueLine->setText(this->keypad_value.toString());
        }
    }
    else if (key == QChar('D')) {
        if (this->InputMode == num_keypad_InputMode::IntegerMode) {
            this->keypad_value = this->keypad_value.toInt() - 1;
            this->ui->valueLine->setText(this->keypad_value.toString());
        }
        else if (this->InputMode == num_keypad_InputMode::DoubleMode) {
            this->keypad_value = this->keypad_value.toDouble() - 1.;
            this->ui->valueLine->setText(this->keypad_value.toString());
        }
        else if ((this->InputMode == num_keypad_InputMode::UnsignedDoubleMode) && (this->keypad_value.toDouble() > 1)) {
            this->keypad_value = this->keypad_value.toDouble() - 1.;
            this->ui->valueLine->setText(this->keypad_value.toString());
        }
        else if ((this->InputMode == num_keypad_InputMode::UnsignedIntegerMode) && (this->keypad_value.toUInt() > 1)) {
            this->keypad_value = this->keypad_value.toInt() - 1.;
            this->ui->valueLine->setText(this->keypad_value.toString());
        }
    }

    else {
        this->ui->valueLine->setText(qstrInput);
    }


}

void num_keypad::on_valueLine_textChanged(const QString &arg1)
{

    if (!arg1.size()) return;
    if (arg1 == this->qstrInput) return;
    QString tmp(arg1);
    tmp.replace(this->comma, this->dot);
    this->number_string_check(tmp);

    return;


}

void num_keypad::on_num_keypadgui_accepted()
{

}
