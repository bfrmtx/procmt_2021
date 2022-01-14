#include "plotoptiondialog.h"
#include "ui_plotoptiondialog.h"

#include <QCheckBox>
#include <QPushButton>

void PlotOptionDialog::remove_entry(const QString & name) {
    if(m_content.contains(name)) {
        auto element = m_content[name];
        ui->grid_layout->removeWidget(element.first);
        ui->grid_layout->removeWidget(element.second);
        element.first->deleteLater();
        element.second->deleteLater();
        m_content.remove(name);
        emit remove_plot(name);
    }
}

void PlotOptionDialog::slot_on_new_edi_plot(QString name) {
    auto new_checkbox = new QCheckBox(name);
         new_checkbox->setChecked(true);
    auto new_button = new QPushButton("remove");

    connect(new_button, &QPushButton::clicked, this, [=] {
        this->remove_entry(name);
    });

    connect(new_checkbox, &QCheckBox::toggled, this, [name, this] (bool state) {
        emit this->plot_visiblity_change(name, state);
    });

    m_content.insert(name, std::make_pair(new_checkbox, new_button));
    auto i = ui->grid_layout->rowCount();

    ui->grid_layout->removeItem(m_spacer);
    ui->grid_layout->addWidget(new_checkbox, i, 0);
    ui->grid_layout->addWidget(new_button, i, 1);
    ui->grid_layout->addItem(m_spacer, i + 1, 0);
}

void PlotOptionDialog::slot_on_all_edi_plots_removed() {
    QStringList list = m_content.keys();
    for(auto name : list) {
        remove_entry(name);
    }
}

PlotOptionDialog::PlotOptionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotOptionDialog)
{
    ui->setupUi(this);
    m_spacer = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);

}

PlotOptionDialog::~PlotOptionDialog() {
    delete ui;
}
