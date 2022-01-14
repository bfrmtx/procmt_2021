#include "select_items_from_db_sensors.h"
#include "ui_select_items_from_db_sensors.h"

select_items_from_db_sensors::select_items_from_db_sensors(const QStringList &selections, const int &cols, QWidget *parent) :
    QDialog(parent), cols(cols), selections(selections),
    ui(new Ui::select_items_from_db_sensors)
{
    ui->setupUi(this);

    if (!this->selections.size()) return;
    if (this->cols < 1) return;

    for (int i = 0; i < this->selections.size(); ++i) {
        this->nbutton = i;

        if ( !(i % cols)) {

            this->nlayout++;
            this->hzls.append(new QHBoxLayout());
            this->hzls.last()->setObjectName("hzl_" + QString::number(i));
            this->ui->vtl_sens->addLayout(this->hzls.last());

        }
        this->btns.append(new sens_button(this->nbutton, this->nlayout-1, this->hzls.last(), this->selections.at(i), this));


    }

}

select_items_from_db_sensors::~select_items_from_db_sensors()
{
    this->btns.clear();
    this->hzls.clear();
    delete ui;
}

QString select_items_from_db_sensors::get_selection() const
{
    return this->selection;
}

void select_items_from_db_sensors::clear()
{
    this->selection = "empty";
}

void select_items_from_db_sensors::set_selection(const QString &selection)
{
    this->selection = selection;
    this->accept();
}

void select_items_from_db_sensors::activate_button(const QString &button_name)
{
    if (this->btns.size()) {
        for (auto &btn : this->btns) {
            if (btn->label_text == button_name) {
                btn->btn->setFocus();
                qDebug() << "focus" << button_name;
            }
        }
    }
}




sens_button::sens_button(const uint &nbutton, const uint &nlayout, QBoxLayout *layout, const QString &label_text, select_items_from_db_sensors *gui) :
    QObject(gui), label_text(label_text), layout(layout), nlayout(nlayout), nbutton(nbutton)
{
    this->btn = new QPushButton(gui);
    this->btn->setText(this->label_text);
    this->layout->addWidget(this->btn);


    connect(btn, &QPushButton::clicked, this, &sens_button::on_clicked);
    connect(this, &sens_button::selection_done, gui, &select_items_from_db_sensors::set_selection);


}

sens_button::~sens_button()
{

    delete this->btn;
}

void sens_button::on_clicked()
{

    emit this->selection_done(this->label_text);


}
