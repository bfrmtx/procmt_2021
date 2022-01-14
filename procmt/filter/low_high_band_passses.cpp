#include "low_high_band_passses.h"
#include "ui_low_high_band_passses.h"

low_high_band_passses::low_high_band_passses(std::shared_ptr< QMap<QString, QVariant> > extrafilters_status, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::low_high_band_passses)
{
    if (extrafilters_status == nullptr) {
        delete ui;
        return;
    }
    ui->setupUi(this);

    this->filter_name = extrafilters_status->value("filter_name").toString();
    this->setWindowTitle(filter_name);

    if (this->filter_name.contains("highpass") || this->filter_name.contains("bandpass")) {
        this->spnbhighpass = new hz_s_spinbox(false, this);
        this->ui->hzl_hp->addWidget(this->spnbhighpass->fsp);
        this->ui->hzl_hp->addWidget(this->spnbhighpass->Hz_s_btn);

    }

    if (this->filter_name.contains("lowpass") || this->filter_name.contains("bandpass")) {
        this->spnblowpass = new hz_s_spinbox(true, this);
        this->ui->hzl_lp->addWidget(this->spnblowpass->fsp);
        this->ui->hzl_lp->addWidget(this->spnblowpass->Hz_s_btn);


    }

    this->extrafilters_status = extrafilters_status;

}

low_high_band_passses::~low_high_band_passses()
{
    this->extrafilters_status.reset();
    if (this->spnblowpass != Q_NULLPTR) delete this->spnblowpass;
    if (this->spnbhighpass != Q_NULLPTR) delete this->spnbhighpass;
    delete ui;
}

void low_high_band_passses::set_f_min_max_lp(const double fmin, const double fmax)
{
    if (this->spnblowpass != Q_NULLPTR) this->spnblowpass->set_f_min_max(fmin, fmax);

}

void low_high_band_passses::set_f_min_max_hp(const double fmin, const double fmax)
{

    if (this->spnbhighpass != Q_NULLPTR) this->spnbhighpass->set_f_min_max(fmin, fmax);

}

void low_high_band_passses::set_initValue_lp(const double f)
{
    if (this->spnblowpass != Q_NULLPTR) this->spnblowpass->set_initValue(f);

}

void low_high_band_passses::set_initValue_hp(const double f)
{
    if (this->spnbhighpass != Q_NULLPTR) this->spnbhighpass->set_initValue(f);
}

void low_high_band_passses::on_buttonBox_accepted()
{
    // upper_freq lower_freq
    if (!this->filter_name.compare("lowpass") && (this->spnblowpass != Q_NULLPTR)) {
        this->extrafilters_status->insert("upper_freq", this->spnblowpass->f);
        this->extrafilters_status->insert("lower_freq", (double)0.0);
    }

    if (!this->filter_name.compare("highpass") && (this->spnbhighpass != Q_NULLPTR)) {
        this->extrafilters_status->insert("upper_freq", (double)1.0);
        this->extrafilters_status->insert("lower_freq", this->spnbhighpass->f);

    }
    if ((!this->filter_name.compare("bandpass")) && (this->spnblowpass != Q_NULLPTR) && (this->spnbhighpass != Q_NULLPTR)) {
        this->extrafilters_status->insert("upper_freq", this->spnblowpass->f);
        this->extrafilters_status->insert("lower_freq", this->spnbhighpass->f);
    }
}


void low_high_band_passses::on_buttonBox_rejected()
{
    this->extrafilters_status->clear();
    this->extrafilters_status.reset();
}



