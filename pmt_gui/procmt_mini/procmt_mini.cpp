#include "procmt_mini.h"
#include "ui_procmt_mini.h"

static const char * prop_frequency = "hz";
static const char * prop_count = "counter";

static const char * prop_frequency_sel = "hz";
static const char * prop_count_sel = "counter";

static const char * prop_what_to_plot = "ExEx";
static const char * prop_count_what_to_plot = "counter";

static const char * prop_center_tsplot = "xml";
static const char * prop_count_center_tsplot = "counter";

static const char * prop_rr_tsplot = "xml";
static const char * prop_count_rr_tsplot = "counter";

static const char * prop_emap_tsplot = "xml";
static const char * prop_count_emap_tsplot = "counter";


procmt_mini::procmt_mini(std::shared_ptr<prc_com> cmdline, std::shared_ptr<msg_logger> msg, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::procmt_mini)
{


    this->cmdline = cmdline;
    this->fill_prc_com_with_base_values();
    this->msg = msg;

    this->dbname = procmt_homepath("info.sql3");

    ui->setupUi(this);
    this->setLocale(QLocale::c());

    check_freq = new check_boxes_scoll_area(this);
    this->ui->vl_selfreqs->addWidget(check_freq);

    connect(check_freq, &check_boxes_scoll_area::clicked_freqs, this, &procmt_mini::re_check_allmeasdocs);

    this->ui->dirs_base_lineEdit->set_what(2);
    this->ui->dirs_base_lineEdit->set_check_subdir("/ts");
    on_dirs_base_lineEdit_valid_dir(false);
    this->ui->dirs_center_lineEdit->set_what(0);
    this->ui->dirs_emap_lineEdit->set_what(0);
    this->ui->dirs_rr_lineEdit->set_what(0);



    this->message_queue_size = "queue size: ";
    this->messages_running = ", status: ";
    this->ui->statusBar->show();
    this->ui->statusBar->showMessage(this->message_queue_size + QString::number(this->processing_queue.size()) + this->messages_running);

    this->qprgb = new QProgressBar(this);
    this->ui->statusBar->addPermanentWidget(this->qprgb);

    this->smooth_medium_sharp = new checkbox_list(this);
    this->insert_combos();

    this->ui->verticalLayout_settings_freq->addStretch();

    this->num_threads = std::thread::hardware_concurrency();
    if (this->num_threads < 4) num_threads = 4;


    // this->ui->vl_cut_upper->addStretch();
    // this->ui->vl_cut_lower->addStretch();




    this->ui->tableWidget_msg->setColumnCount(4);
    //this->ui->tableWidget_msg->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    this->ui->tableWidget_msg->resizeColumnToContents(0);
    this->ui->tableWidget_msg->resizeColumnToContents(1);
    this->ui->tableWidget_msg->resizeColumnToContents(2);
    this->ui->tableWidget_msg->resizeColumnToContents(3);
    this->ui->tableWidget_msg->setAlternatingRowColors(true);
    this->ui->tableWidget_msg->horizontalHeader()->setStretchLastSection(true);

    this->ui->tabWidget->setCurrentIndex(0);

    this->my_mttype = mttype::mt;
    this->mt_types = new checkbox_list(this);


    this->mtsite = std::make_shared<mt_site>(this->num_threads, mttype::mt, this);

    this->cmdline->append_data_from_other(this->mtsite->get_data(), "mt_site_");
    this->cmdline->insert_int("force_frequency_list", 1);

    this->cmdline->insert_szt("mt_site_huber_iterations", 20);


    this->cmdline->set_key_size_t("mt_site_min_stacks", 5);


    this->mtsite.reset();

    // transfer the GUI values to the cmdline without emitting

    for (auto &kv : this->scc) {
        cmdline->set_key_value(kv->get_key(), kv->get_value());
    }
    for (auto &kv : this->cbp) {
        cmdline->set_key_value(kv->get_key(), kv->get_value());
    }


}

procmt_mini::~procmt_mini()
{
    if (this->dirdialog != Q_NULLPTR) {
        delete this->dirdialog;
        this->dirdialog = Q_NULLPTR;
    }
    if (this->procediplotter != Q_NULLPTR) {
        this->procediplotter->close();
    }

    // Qt6
    if (this->spc_plotter != nullptr) {
        this->spc_plotter->close();
        this->spc_plotter.reset();

    }

    this->plots_re.clear();
    this->plots_im.clear();

    delete ui;


    this->msg.reset();
    this->cmdline.reset();
    this->mtsite.reset();



}

void procmt_mini::keyPressEvent(QKeyEvent *keyevent)
{
    if (keyevent->key() == Qt::Key_F1) {
        this->on_actionHelp_triggered();
    }
}

void procmt_mini::insert_combos()
{
    //this->all_combos = 0;
    this->all_ckecks = 0;


    // parzen
    // create a new combo box with database, connect to this, null -> means create the combobox and connect to this
    // link to a key called parzen_radius (e.g. to prc_com) name the label "Parzen Radius"
    // a signal will be emitted signal_current_variant_changed and
    // connected to this->slot_change_prc_com_variant

    this->scc.append(new wl_combo (this->dbname, "frequencies_drop_down", "double", this, Q_NULLPTR,
                                   "wl", "window length"));
    this->scc.last()->add_to_gui(this->ui->verticalLayout_settings_freq, "horizontal", this->mapscc);
    this->scc[this->mapscc.value("wl")]->set_qvariant_to_current_index(QVariant(1024));




    // parzen
    this->scc.append(new single_column_combo (this->dbname, "parzen_radius", "double", this, Q_NULLPTR,
                                              "parzen_radius", "Parzen radius"));
    this->scc.last()->add_to_gui(this->ui->verticalLayout_settings_freq, "horizontal", this->mapscc);
    this->scc[this->mapscc.value("parzen_radius")]->set_qvariant_to_current_index(QVariant(0.15));


    // min lines
    this->scc.append(new single_column_combo (this->dbname, "min_lines", "int", this, Q_NULLPTR,
                                              "min_lines", "Spectral Lines"));
    this->scc.last()->add_to_gui(this->ui->verticalLayout_settings_freq, "horizontal", this->mapscc);
    this->scc[this->mapscc.value("min_lines")]->set_qvariant_to_current_index(QVariant(0));


    //  overlapping
    this->scc.append(new single_column_combo (this->dbname, "overlapping_factor", "double", this, Q_NULLPTR,
                                              "overlapping_factor", "overlapping"));
    this->scc.last()->add_to_gui(this->ui->verticalLayout_settings_freq, "horizontal", this->mapscc);
    this->scc[this->mapscc.value("overlapping_factor")]->set_qvariant_to_current_index(QVariant(0.4));

    // auto band width
    this->scc.append(new dual_column_combo (this->dbname, "auto_bandwidth_scales", "double", this, Q_NULLPTR,
                                            "auto_bandwidth", "auto bandwidth"));
    this->scc.last()->add_to_gui(this->ui->verticalLayout_settings_freq, "horizontal", this->mapscc);
    this->scc[this->mapscc.value("auto_bandwidth")]->set_qvariant_to_current_index(QVariant(1.0));

    // auto parzen
    this->scc.append(new dual_column_combo (this->dbname, "auto_parzen_scales", "double", this, Q_NULLPTR,
                                            "auto_parzen_radius", "auto Parzen"));
    this->scc.last()->add_to_gui(this->ui->verticalLayout_settings_freq, "horizontal", this->mapscc);
    this->scc[this->mapscc.value("auto_parzen_radius")]->set_qvariant_to_current_index(QVariant(1.0));


    QHBoxLayout *hzl = new QHBoxLayout(this);
    QLabel *labelt = new QLabel(this);
    labelt->setText("FTable");

    this->ftable_combo = new QComboBox(this);
    QStringList ftable_items;
    ftable_items << "default_mt_frequencies" << "user1_mt_frequencies" << "user2_mt_frequencies" << "user3_mt_frequencies"
                 << "user1_csem_frequencies" << "user2_csem_frequencies" << "user3_csem_frequencies" ;
    ftable_combo->addItems(ftable_items);
    ftable_combo->setCurrentText(this->cmdline->svalue("target_frequencies_table"));


    hzl->addWidget(ftable_combo);
    hzl->addWidget(labelt);

    this->ui->verticalLayout_settings_freq->addLayout(hzl);



    // cut upper frequency
    this->scc.append(new single_column_combo (this->dbname, "fcut", "double", this, Q_NULLPTR,
                                              "fcut_upper", "cut upper f"));
    this->scc.last()->add_to_gui(this->ui->vl_base_settings, "horizontal", this->mapscc);
    this->scc[this->mapscc.value("fcut_upper")]->set_qvariant_to_current_index(QVariant(0.04));

    // cut lower frequency
    this->scc.append(new single_column_combo (this->dbname, "fcut", "double", this, Q_NULLPTR,
                                              "fcut_lower", "cut lower f"));
    this->scc.last()->add_to_gui(this->ui->vl_base_settings, "horizontal", this->mapscc);
    this->scc[this->mapscc.value("fcut_lower")]->set_qvariant_to_current_index(QVariant(0.01));


    // upper threshold
    this->scc.append(new single_column_combo (this->dbname, "coherency_threshold", "double", this, Q_NULLPTR,
                                              "mt_site_upper_rejection_coh", "upper threshold"));
    this->scc.last()->add_to_gui(this->ui->vl_ct_upper_lower, "horizontal", this->mapscc);
    this->scc[this->mapscc.value("mt_site_upper_rejection_coh")]->set_qvariant_to_current_index(QVariant(1.0));

    // lower threshold
    this->scc.append(new single_column_combo (this->dbname, "coherency_threshold", "double", this, Q_NULLPTR,
                                              "mt_site_lower_rejection_coh", "lower threshold"));
    this->scc.last()->add_to_gui(this->ui->vl_ct_upper_lower, "horizontal", this->mapscc);
    this->scc[this->mapscc.value("mt_site_lower_rejection_coh")]->set_qvariant_to_current_index(QVariant(0.8));


    // vl_mt_median_rho
    this->scc.append(new single_column_combo (this->dbname, "limit_median", "double", this, Q_NULLPTR,
                                              "mt_site_upper_median_rho", "upper median rho:"));
    this->scc.last()->add_to_gui(this->ui->vl_mt_median_rho, "horizontal", this->mapscc);
    this->scc[this->mapscc.value("mt_site_upper_median_rho")]->set_qvariant_to_current_index(QVariant(1.5));


    this->scc.append(new single_column_combo (this->dbname, "limit_median", "double", this, Q_NULLPTR,
                                              "mt_site_lower_median_rho", "lower median rho"));
    this->scc.last()->add_to_gui(this->ui->vl_mt_median_rho, "horizontal", this->mapscc);
    this->scc[this->mapscc.value("mt_site_lower_median_rho")]->set_qvariant_to_current_index(QVariant(1.5));

    this->scc.append(new single_column_combo (this->dbname, "limit_median", "double", this, Q_NULLPTR,
                                              "mt_site_upper_median_phi", "upper median phi"));
    this->scc.last()->add_to_gui(this->ui->vl_median_phi, "horizontal", this->mapscc);
    this->scc[this->mapscc.value("mt_site_upper_median_phi")]->set_qvariant_to_current_index(QVariant(1.3));

    this->scc.append(new single_column_combo (this->dbname, "limit_median", "double", this, Q_NULLPTR,
                                              "mt_site_lower_median_phi", "lower median phi"));
    this->scc.last()->add_to_gui(this->ui->vl_median_phi, "horizontal", this->mapscc);
    this->scc[this->mapscc.value("mt_site_lower_median_phi")]->set_qvariant_to_current_index(QVariant(1.3));


    this->scc.append(new single_column_combo (this->dbname, "xstddev", "double", this, Q_NULLPTR,
                                              "mt_site_xstddev", "x std dev"));
    this->scc.last()->add_to_gui(this->ui->vl_mt_ct_btns, "horizontal", this->mapscc);
    this->scc[this->mapscc.value("mt_site_xstddev")]->set_qvariant_to_current_index(QVariant(1.7));


    this->scc.append(new single_column_combo (this->dbname, "xstddev", "double", this, Q_NULLPTR,
                                              "mt_site_xstddev_min", "x std dev min"));
    this->scc.last()->add_to_gui(this->ui->vl_mt_ct_btns, "horizontal", this->mapscc);
    this->scc[this->mapscc.value("mt_site_xstddev_min")]->set_qvariant_to_current_index(QVariant(1.0));




    // check boxes
    this->cbp.append(new prc_com_check_box(this, Q_NULLPTR, "mt_site_activate_quadrant_check", "quadrant check"));
    this->cbp.last()->add_to_gui(this->ui->vl_mt_basic_btns_two, this->mapscbp, true);


    QCheckBox *smooth = new QCheckBox("smooth", this);
    QCheckBox *medium = new QCheckBox("medium", this);
    QCheckBox *sharp =  new QCheckBox("sharp", this);
    QCheckBox *off =    new QCheckBox("off", this);

    this->skip_marked_parts =    new QCheckBox("skip marked parts", this);
    this->skip_marked_parts->setObjectName("skip_marked_parts");
    connect(skip_marked_parts, &QCheckBox::clicked, this, &procmt_mini::on_skip_marked_parts_clicked);


    smooth->setObjectName("smooth");
    medium->setObjectName("medium");
    sharp->setObjectName("sharp");
    off->setObjectName("off");



    medium->setChecked(true);

    this->smooth_medium_sharp->append(smooth);
    this->smooth_medium_sharp->append(medium);
    this->smooth_medium_sharp->append(sharp);
    this->smooth_medium_sharp->append(off);

    this->ui->vl_base_settings->addWidget(new QLabel("ProcMT preference:"));
    this->ui->vl_base_settings->addWidget(smooth);
    this->ui->vl_base_settings->addWidget(medium);
    this->ui->vl_base_settings->addWidget(sharp);
    this->ui->vl_base_settings->addWidget(off);

    this->ui->vl_base_settings->addWidget(skip_marked_parts);

    QObject::connect(this->smooth_medium_sharp, &checkbox_list::btn_name_checked, this, &procmt_mini::slot_set_bw_prz);

    this->cbp.append(new prc_com_check_box(this, Q_NULLPTR, "mt_site_activate_median_rho_phi", "median rho/phi active"));
    this->cbp.last()->add_to_gui(this->ui->vl_mt_basic_btns, this->mapscbp, false);


    this->cbp.append(new prc_com_check_box(this, Q_NULLPTR, "mt_site_activate_ct", "ct active"));
    this->cbp.last()->add_to_gui(this->ui->vl_mt_basic_btns, this->mapscbp, true);



    this->cbp.append(new prc_com_check_box(this, Q_NULLPTR, "mt_site_activate_rho_limits", "rho limits active"));
    this->cbp.last()->add_to_gui(this->ui->vl_mt_basic_btns, this->mapscbp, false);


    this->cbp.append(new prc_com_check_box(this, Q_NULLPTR, "mt_site_activate_phi_limits", "phi limits active"));
    this->cbp.last()->add_to_gui(this->ui->vl_mt_basic_btns, this->mapscbp, false);

    this->cbp.append(new prc_com_check_box(this, Q_NULLPTR, "mt_site_xstddev_auto_active", "stddev auto"));
    this->cbp.last()->add_to_gui(this->ui->vl_mt_ct_btns, this->mapscbp, false);

    this->cbp.append(new prc_com_check_box(this, Q_NULLPTR, "mt_site_xstddev_active", "stddev limit active"));
    this->cbp.last()->add_to_gui(this->ui->vl_mt_ct_btns, this->mapscbp, true);

    this->ui->vl_base_settings->addStretch();


}

void procmt_mini::evaluate_mt_type()
{
    this->my_mttype = mttype::nomt;

    QString str_mt = "no MT";
    if (this->allmeasdocs_center.size()) {

        this->my_mttype = mttype::mt;
        str_mt = "MT";
        slot_add_tsplotter_center_button();
    }
    if (this->allmeasdocs_center.size() && this->allmeasdocs_emap.size()) {
        this->my_mttype =  mttype::emap;
        str_mt = "EMAP";
        slot_add_tsplotter_emap_button();


    }
    if (this->allmeasdocs_center.size() && this->allmeasdocs_rr.size()) {

        this->my_mttype = mttype::rr;
        str_mt = "RR";
        slot_add_tsplotter_rr_button();
    }
    if (this->allmeasdocs_center.size() && this->allmeasdocs_rr.size() && this->allmeasdocs_emap.size()) {
        this->my_mttype = mttype::emap_rr;
        str_mt = "EMAP RR";

    }

    this->ui->mtmode_label->setText(str_mt);

}


//void procmt_mini::get_mt_data(std::shared_ptr<mt_site> mtsite, std::vector<std::shared_ptr<edi_file> > &edifiles)
//{
//    if (mtsite == nullptr) return;
//    this->mtsite = mtsite;
//    this->edifiles = edifiles;


//    this->cmdline->append_data_from_other(this->mtsite->get_data(), "mt_site_");

//    // transfer the GUI values to the cmdline without emitting

//    for (auto &kv : this->scc) {
//        cmdline->set_key_value(kv->get_key(), kv->get_value());
//    }
//    for (auto &kv : this->cbp) {
//        cmdline->set_key_value(kv->get_key(), kv->get_value());
//    }



//    connect(this->mtsite.get(), &mt_site::signal_general_msg, this->msg.get(), &msg_logger::slot_general_msg);
//    connect(this->mtsite.get(), &mt_site::signal_general_msg_nums, this->msg.get(), &msg_logger::slot_general_msg_nums);
//}



/*void procmt_mini::on_dirs_base_lineEdit_textChanged(const QString &arg1)
{

    qDebug() << this->ui->dirs_base_lineEdit->text();
    //    this->check_url(this->ui->dirs_base_lineEdit->text(), true);
    //    QDir testdir(this->ui->dirs_base_lineEdit->text());
    QDir testdir(this->check_url(this->ui->dirs_base_lineEdit->text(), true).absoluteFilePath());
    if(this->ui->dirs_base_lineEdit->text().endsWith("//")) this->ui->dirs_base_lineEdit->setText(testdir.absolutePath());
    if(this->ui->dirs_base_lineEdit->text().endsWith("//")) this->ui->dirs_base_lineEdit->setText(testdir.absolutePath());
    QFileInfo testdir_ts(testdir.absolutePath() + "/ts");
    // QFileInfo testdir_ts_sharp(this->ui->dirs_base_lineEdit->text() + "/ts"); // protect against folder///subfolder

    if ( !(testdir.exists() && testdir_ts.exists())) {
        this->ui->dirs_center_lineEdit->setDisabled(true);
        this->ui->dirs_emap_lineEdit->setDisabled(true);
        this->ui->dirs_rr_lineEdit->setDisabled(true);
        this->qfi_base.setFile("");
        return;
    }
    if (this->ui->dirs_base_lineEdit->text() != testdir.absolutePath()) {
        this->ui->dirs_base_lineEdit->setText(testdir.absolutePath());
    }
    if (this->qfi_base.absoluteFilePath() != this->ui->dirs_base_lineEdit->text()) {
        this->qfi_base.setFile(testdir.absolutePath());
        this->cmdline->set_key_value("survey_basedir", this->qfi_base.absoluteFilePath());
        this->basedir.setPath(testdir.absolutePath());
        this->ui->dirs_center_lineEdit->setDisabled(false);
        this->ui->dirs_emap_lineEdit->setDisabled(false);
        this->ui->dirs_rr_lineEdit->setDisabled(false);
        // we changed
        this->ui->dirs_center_lineEdit->clear();
        this->ui->dirs_emap_lineEdit->clear();
        this->ui->dirs_rr_lineEdit->clear();

    }

}
*/
QFileInfo procmt_mini::check_url(const QString &instring, bool set_title)
{
    QFileInfo qfi;
    QStringList lst = instring.split("file://", Qt::SkipEmptyParts, Qt::CaseInsensitive);
    qfi.setFile(lst.last());
    if (qfi.exists() && qfi.isDir()) {
        if (set_title) {
            this->basedir.setPath(qfi.absoluteFilePath());
            basedir.setCurrent(this->basedir.absolutePath());
            this->setWindowTitle(qfi.baseName());
            this->ui->dirs_center_lineEdit->setDisabled(false);
            this->ui->dirs_emap_lineEdit->setDisabled(false);
            this->ui->dirs_rr_lineEdit->setDisabled(false);
        }
        qDebug() << "exists!" ;
    }
    else {
        QUrl url(instring);
        if (url.isValid()) {
            QUrl url(instring);
            if (url.isLocalFile()) {
                qfi.setFile(url.toLocalFile());
                if (qfi.exists() && qfi.isDir()) {
                    if (set_title) {
                        this->basedir.setPath(qfi.absoluteFilePath());
                        basedir.setCurrent(this->basedir.absolutePath());
                        this->setWindowTitle(qfi.baseName());
                        this->ui->dirs_center_lineEdit->setDisabled(false);
                        this->ui->dirs_emap_lineEdit->setDisabled(false);
                        this->ui->dirs_rr_lineEdit->setDisabled(false);
                    }
                    qDebug() << "exists!" ;
                }
            }
        }
    }

    return qfi;
}

/*
void procmt_mini::on_dirs_center_lineEdit_editingFinished()
{
    if (this->qfi_center.baseName() != this->ui->dirs_center_lineEdit->text()) {
        this->qfi_center = this->check_url(this->ui->dirs_center_lineEdit->text(), true);
        this->ui->dirs_center_lineEdit->setText(qfi_center.baseName());
        this->cmdline->set_key_value("site_dir", this->qfi_center.baseName());
        if (cmdline->svalue("survey_basedir") != "empty") {
            allmeasdocs_center = xmls_in_site(cmdline->svalue("survey_basedir"), cmdline->svalue("site_dir"));
            this->ui->radio_mt->click();
        }
    }

}
*/
/*
void procmt_mini::on_dirs_center_lineEdit_textChanged(const QString &arg1)
{
    if (this->qfi_center.baseName() != this->ui->dirs_center_lineEdit->text()) {
        this->qfi_center = this->check_url(this->ui->dirs_center_lineEdit->text(), true);
        this->ui->dirs_center_lineEdit->setText(qfi_center.baseName());
        this->cmdline->set_key_value("site_dir", this->qfi_center.baseName());
        if (cmdline->svalue("survey_basedir") != "empty") {
            allmeasdocs_center = xmls_in_site(cmdline->svalue("survey_basedir"), cmdline->svalue("site_dir"));
            this->ui->radio_mt->click();
        }
    }

}

void procmt_mini::on_dirs_emap_lineEdit_textChanged(const QString &arg1)
{
    if (this->qfi_emap.baseName() != this->ui->dirs_emap_lineEdit->text()) {
        this->qfi_emap = this->check_url(this->ui->dirs_emap_lineEdit->text(), true);
        this->ui->dirs_emap_lineEdit->setText(qfi_emap.baseName());
        this->dir_selected(qfi_emap.absoluteFilePath(), 1);


    }
}

void procmt_mini::on_dirs_rr_lineEdit_textChanged(const QString &arg1)
{
    if (this->qfi_rr.baseName() != this->ui->dirs_rr_lineEdit->text()) {
        this->qfi_rr = this->check_url(this->ui->dirs_rr_lineEdit->text(), true);
        this->ui->dirs_rr_lineEdit->setText(qfi_rr.baseName());
        this->dir_selected(qfi_rr.absoluteFilePath(), 3);
    }
}
*/
void procmt_mini::on_dirs_base_lineEdit_valid_dir(const bool &ok)
{
    if (ok) {
        if (this->qfi_base.absoluteFilePath() != this->ui->dirs_base_lineEdit->text()) {
            this->qfi_base.setFile(this->ui->dirs_base_lineEdit->absoluteFilePath());
            this->cmdline->set_key_value("survey_basedir", this->qfi_base.absoluteFilePath());
            this->basedir.setPath(this->ui->dirs_base_lineEdit->absoluteFilePath());
            this->ui->dirs_center_lineEdit->setDisabled(false);
            this->ui->dirs_emap_lineEdit->setDisabled(false);
            this->ui->dirs_rr_lineEdit->setDisabled(false);
            // we changed
            this->ui->dirs_center_lineEdit->clear();
            this->ui->dirs_emap_lineEdit->clear();
            this->ui->dirs_rr_lineEdit->clear();
            this->setWindowTitle(qfi_base.baseName());

            this->ui->dirs_emap_lineEdit->set_toplevel_dir(this->qfi_base.absoluteFilePath() + "/ts");
            this->ui->dirs_center_lineEdit->set_toplevel_dir(this->qfi_base.absoluteFilePath() + "/ts");
            this->ui->dirs_rr_lineEdit->set_toplevel_dir(this->qfi_base.absoluteFilePath() + "/ts");

        }

    }
    if (!ok) {
        this->ui->dirs_center_lineEdit->setDisabled(true);
        this->ui->dirs_emap_lineEdit->setDisabled(true);
        this->ui->dirs_rr_lineEdit->setDisabled(true);
        this->setWindowTitle("NO SURVEY");

    }
}

void procmt_mini::on_dirs_emap_lineEdit_valid_dir(const bool &ok)
{
    if (ok) {
        this->setWindowTitle(this->ui->dirs_base_lineEdit->baseName() + ": " + this->ui->dirs_emap_lineEdit->baseName());
        this->qfi_emap.setFile(this->ui->dirs_emap_lineEdit->absoluteFilePath());
        this->dir_selected(qfi_emap.absoluteFilePath(), 1);
    }
    else if (this->ui->dirs_center_lineEdit->text().size()) {
        this->setWindowTitle(this->ui->dirs_base_lineEdit->baseName() + ": " + this->ui->dirs_center_lineEdit->baseName());

    }
    this->evaluate_mt_type();
}

void procmt_mini::on_dirs_center_lineEdit_valid_dir(const bool &ok)
{
    if (ok && !this->ui->dirs_emap_lineEdit->text().size()) {
        this->setWindowTitle(this->ui->dirs_base_lineEdit->baseName() + ": " + this->ui->dirs_center_lineEdit->baseName());
        this->qfi_center.setFile(this->ui->dirs_center_lineEdit->absoluteFilePath());
        this->cmdline->set_key_value("site_dir", this->qfi_center.baseName());
        if (cmdline->svalue("survey_basedir") != "empty") {
            allmeasdocs_center = xmls_in_site(cmdline->svalue("survey_basedir"), cmdline->svalue("site_dir"));

            this->dir_selected(qfi_center.absoluteFilePath(), 2);
        }

    }
    else {
        this->setWindowTitle(this->ui->dirs_base_lineEdit->baseName());
    }

    this->evaluate_mt_type();
}

void procmt_mini::on_dirs_rr_lineEdit_valid_dir(const bool &ok)
{
    if (ok) {
        this->qfi_rr.setFile(this->ui->dirs_rr_lineEdit->absoluteFilePath());
        this->dir_selected(qfi_rr.absoluteFilePath(), 3);
    }
    else {
        this->allmeasdocs_rr.clear();
    }
    this->evaluate_mt_type();

}

void procmt_mini::slot_add_tsplotter_center_button()
{

    if (!this->allmeasdocs_center.size()) return;
    while (auto item = this->ui->scroll_tsplotter_center->layout()->takeAt(0)) {
        delete item->widget();
    }

    QFileInfo qfi_title(this->allmeasdocs_center.at(0));
    QMap <QString, QVariant> title = atsfilename_site_survey(qfi_title.absoluteFilePath());

    if (title.contains("site")) this->ui->label_scroll_tsplotter_center->setText("center: " + title.value("site").toString());

    tsplotter_center_buttons.clear();
    for (auto str : this->allmeasdocs_center) {
        QFileInfo qfi(str);

        QPushButton * new_menu_button = new QPushButton(this->ui->scroll_tsplotter_center);
        new_menu_button->setProperty(prop_center_tsplot, QVariant(str));
        new_menu_button->setProperty(prop_count_center_tsplot, QVariant(uint(tsplotter_center_buttons.size())));
        this->tsplotter_center_buttons.push_back(new_menu_button);
        QMap <QString, QVariant> button_name = atsfilename_site_survey(qfi.absoluteFilePath());
        new_menu_button->setText(button_name.value("measdir").toString());
        connect(new_menu_button, &QPushButton::clicked, this, &procmt_mini::slot_add_tsplotter_center_button_clicked);
        this->ui->scroll_tsplotter_center->layout()->addWidget(new_menu_button);


    }
}

void procmt_mini::slot_add_tsplotter_center_button_clicked()
{

    auto * plotButton = reinterpret_cast<QPushButton*>(sender());
    QString xml = plotButton->property(prop_center_tsplot).toString();

    qDebug() << "try to open" << xml;
    this->slot_tsplotter(xml);

}

void procmt_mini::slot_add_tsplotter_rr_button()
{

    if (!this->allmeasdocs_rr.size()) return;
    while (auto item = this->ui->scroll_tsplotter_rr->layout()->takeAt(0)) {
        delete item->widget();
    }

    QFileInfo qfi_title(this->allmeasdocs_rr.at(0));
    QMap <QString, QVariant> title = atsfilename_site_survey(qfi_title.absoluteFilePath());

    if (title.contains("site")) this->ui->label_scroll_tsplotter_center->setText("remote: " + title.value("site").toString());

    tsplotter_rr_buttons.clear();
    for (auto str : this->allmeasdocs_rr) {
        QFileInfo qfi(str);

        QPushButton * new_menu_button = new QPushButton(this->ui->scroll_tsplotter_rr);
        new_menu_button->setProperty(prop_rr_tsplot, QVariant(str));
        new_menu_button->setProperty(prop_count_rr_tsplot, QVariant(uint(tsplotter_rr_buttons.size())));
        this->tsplotter_rr_buttons.push_back(new_menu_button);
        QMap <QString, QVariant> button_name = atsfilename_site_survey(qfi.absoluteFilePath());
        new_menu_button->setText(button_name.value("measdir").toString());
        connect(new_menu_button, &QPushButton::clicked, this, &procmt_mini::slot_add_tsplotter_rr_button_clicked);
        this->ui->scroll_tsplotter_rr->layout()->addWidget(new_menu_button);


    }
}

void procmt_mini::slot_add_tsplotter_rr_button_clicked()
{
    auto * plotButton = reinterpret_cast<QPushButton*>(sender());
    QString xml = plotButton->property(prop_rr_tsplot).toString();

    qDebug() << "try to open" << xml;
    this->slot_tsplotter(xml);
}

void procmt_mini::slot_add_tsplotter_emap_button()
{
    if (!this->allmeasdocs_emap.size()) return;
    while (auto item = this->ui->scroll_tsplotter_emap->layout()->takeAt(0)) {
        delete item->widget();
    }

    QFileInfo qfi_title(this->allmeasdocs_emap.at(0));
    QMap <QString, QVariant> title = atsfilename_site_survey(qfi_title.absoluteFilePath());

    if (title.contains("site")) this->ui->label_scroll_tsplotter_emap->setText("center: " + title.value("site").toString());

    tsplotter_emap_buttons.clear();
    for (auto str : this->allmeasdocs_emap) {
        QFileInfo qfi(str);

        QPushButton * new_menu_button = new QPushButton(this->ui->scroll_tsplotter_emap);
        new_menu_button->setProperty(prop_emap_tsplot, QVariant(str));
        new_menu_button->setProperty(prop_count_emap_tsplot, QVariant(uint(tsplotter_emap_buttons.size())));
        this->tsplotter_emap_buttons.push_back(new_menu_button);
        QMap <QString, QVariant> button_name = atsfilename_site_survey(qfi.absoluteFilePath());
        new_menu_button->setText(button_name.value("measdir").toString());
        connect(new_menu_button, &QPushButton::clicked, this, &procmt_mini::slot_add_tsplotter_emap_button_clicked);
        this->ui->scroll_tsplotter_emap->layout()->addWidget(new_menu_button);


    }
}

void procmt_mini::slot_add_tsplotter_emap_button_clicked()
{
    auto * plotButton = reinterpret_cast<QPushButton*>(sender());
    QString xml = plotButton->property(prop_emap_tsplot).toString();

    qDebug() << "try to open" << xml;
    this->slot_tsplotter(xml);
}




void procmt_mini::slot_add_f_button(const double &frequency)
{
    QPushButton * new_menu_button = new QPushButton(this->ui->scrollFreqs);
    new_menu_button->setProperty(prop_frequency, QVariant(frequency));
    new_menu_button->setProperty(prop_count, QVariant(uint(frequency_buttons.size())));
    this->frequency_buttons.push_back(new_menu_button);
    new_menu_button->setCheckable(true);
    new_menu_button->setText(doublefreq2string_unit(frequency));

    connect(new_menu_button, &QPushButton::clicked, this, &procmt_mini::slot_frequency_button_clicked);

    //            ui->usermenu_left->layout()->removeItem(ui->spacer_left);
    this->ui->scrollFreqs->layout()->addWidget(new_menu_button);
    //            this->ui->scrollFreqs->layout()->addItem(ui->spacer_left);
    //            ui->usermenu_left_top->setMaximumWidth(ui->usermenu_left->width() + 64);
}



void procmt_mini::slot_frequency_button_clicked()
{
    if (this->mtsite == nullptr) return;

    auto * freqButton = reinterpret_cast<QPushButton*>(sender());
    double freq = freqButton->property(prop_frequency).toDouble();

    if (this->mtsite->ac_spectra.at(0).size()) {   // have auto and cross spectra

        this->plots_im.clear();
        this->plots_re.clear();

        size_t idx = f_index_acsp(this->mtsite->ac_spectra, freq);


        size_t i = 0;
        std::vector<double> xaxis(this->mtsite->ac_spectra.at(idx).ac[pmt::hxhx].size());
        for (auto &x : xaxis) x = i++;
        if (!xaxis.size()) return;

        i = 0;
        for (auto &str : pmt::ac_spectra_names) {
            if (this->mtsite->ac_spectra.at(idx).ac[i].size()) {
                this->plots_re.insert(str, this->ui->scrollplots);
                this->plots_im.insert(str); // only one button, I will call re and im together
                this->plots_re.at(str)->set_data_complex(0, xaxis, this->mtsite->ac_spectra.at(idx).ac[i], 1, QPen(Qt::blue));
                this->plots_im.at(str)->set_data_complex(0, xaxis, this->mtsite->ac_spectra.at(idx).ac[i], 2, QPen(Qt::red));
                this->plots_re.at(str)->set_tile(str + "re, " + doublefreq2string_unit(freq));
                this->plots_im.at(str)->set_tile(str + "im, " + doublefreq2string_unit(freq));

                connect(&this->plots_re, &multi_static_plots::plotting, &this->plots_im, &multi_static_plots::slot_plot);



            }
            ++i;
        }
    }
}




void procmt_mini::on_dirs_base_select_pushButton_clicked()
{
    QString where = "/survey";
    QDir sv("/survey");
    if (!sv.exists()) where = QDir::homePath();
    auto dialog = new QFileDialog(this, "Select base dir", where);

    dialog->setViewMode(QFileDialog::Detail);
    dialog->setFileMode(QFileDialog::Directory);
    dialog->setOption(QFileDialog::ShowDirsOnly, true);


    dialog->setModal(false);
    dialog->show();
    QString fileName;
    if (dialog->exec()) {
        fileName = dialog->selectedFiles().at(0);
    }
    if (fileName.size()) {
        QDir bd(fileName);
        this->ui->dirs_base_lineEdit->setText(bd.absolutePath());
        qDebug() << this->ui->dirs_base_lineEdit->text() << " set!";
    }
}


void procmt_mini::on_dirs_emap_select_pushButton_clicked()
{
    if (this->dirdialog != Q_NULLPTR) {
        delete this->dirdialog;
        this->dirdialog = Q_NULLPTR;
    }
    QDir init(this->basedir.absolutePath() + "/ts");
    this->dirdialog = new dir_dialog(this, init.absolutePath(), 1);
    connect(this->dirdialog, &dir_dialog::selected_dir, this, &procmt_mini::dir_selected);
    this->dirdialog->exec();

}


void procmt_mini::on_dirs_center_select_pushButton_clicked()
{

    if (this->dirdialog != Q_NULLPTR) {
        delete this->dirdialog;
        this->dirdialog = Q_NULLPTR;
    }
    QDir init(this->basedir.absolutePath() + "/ts");
    this->dirdialog = new dir_dialog(this, init.absolutePath(), 2);
    connect(this->dirdialog, &dir_dialog::selected_dir, this, &procmt_mini::dir_selected);
    this->dirdialog->exec();
}

void procmt_mini::on_dirs_rr_select_pushButton_clicked()
{
    if (this->dirdialog != Q_NULLPTR) {
        delete this->dirdialog;
        this->dirdialog = Q_NULLPTR;
    }
    QDir init(this->basedir.absolutePath() + "/ts");
    this->dirdialog = new dir_dialog(this, init.absolutePath(), 3);
    connect(this->dirdialog, &dir_dialog::selected_dir, this, &procmt_mini::dir_selected);
    this->dirdialog->exec();



}


void procmt_mini::dir_selected(const QString &absolute_path, const int &target)
{
    qDebug() << "dir" << absolute_path;
    if (target == 1) {
        this->qfi_emap.setFile(absolute_path);
        this->ui->dirs_emap_lineEdit->setText(qfi_emap.baseName());
        this->cmdline->set_key_value("site_dir_emap", this->qfi_emap.baseName());
        if (cmdline->svalue("survey_basedir") != "empty") {
            allmeasdocs_emap = xmls_in_site(cmdline->svalue("survey_basedir"), cmdline->svalue("site_dir_emap"));

        }

    }
    if (target == 2) {
        this->qfi_center.setFile(absolute_path);
        this->ui->dirs_center_lineEdit->setText(qfi_center.baseName());
        this->cmdline->set_key_value("site_dir", this->qfi_center.baseName());
        if (cmdline->svalue("survey_basedir") != "empty") {
            allmeasdocs_center = xmls_in_site(cmdline->svalue("survey_basedir"), cmdline->svalue("site_dir"));
        }
    }
    if (target == 3) {
        this->qfi_rr.setFile(absolute_path);
        this->ui->dirs_rr_lineEdit->setText(qfi_rr.baseName());
        this->cmdline->set_key_value("site_dir_rr", this->qfi_rr.baseName());
        if (cmdline->svalue("survey_basedir") != "empty") {
            allmeasdocs_rr = xmls_in_site(cmdline->svalue("survey_basedir"), cmdline->svalue("site_dir_rr"));
        }
    }


    if (this->dirdialog != Q_NULLPTR) {
        this->dirdialog->close();
    }
    if (allmeasdocs_center.size()) {

        // clean buttons
        // check with remote
        // set buttons
        // backup allmeasdoc
        this->allmeasdocs_center_orig = allmeasdocs_center;
        this->map_sel_freqs.clear();
        this->check_freq->clear();


        // change allmeasdocs

        for (auto &doc : allmeasdocs_center) {
            QPair<double, QString> addb(xml_measdoc_to_f_s(doc));
            if (addb.first != 0.0000000) this->map_sel_freqs.insert(addb.first, addb.second);


        }


        QMap<double, QString>::const_iterator m = this->map_sel_freqs.constBegin();
        while (m != this->map_sel_freqs.constEnd()) {
            this->check_freq->add_f_checkbox(m.key());
            ++m;
        }

        this->evaluate_mt_type();


    }

}

void procmt_mini::slot_recalculate()
{
    this->ui->statusBar->showMessage("calucating tensor");
    this->edifiles.emplace_back(std::make_shared<edi_file>(this));



    QObject::connect(this->edifiles.back().get(), &edi_file::signal_actual_edifile_name, this, &procmt_mini::slot_actual_edifile_name, Qt::DirectConnection);


    QString site_name;
    QFileInfo qfi;
    QString rr_indicator("");
    if (this->ui->dirs_rr_lineEdit->text().size()) {
        rr_indicator = "_RR_" + this->ui->dirs_rr_lineEdit->text();
    }

    if (this->ui->dirs_emap_lineEdit->text().size()) {
        site_name = this->qfi_base.absoluteFilePath() + "/edi/" + this->ui->dirs_emap_lineEdit->text() + rr_indicator + ".edi";
    }
    else {
        site_name = this->qfi_base.absoluteFilePath() + "/edi/" + this->ui->dirs_center_lineEdit->text() + rr_indicator + ".edi";
    }
    qfi.setFile(site_name);
    this->edifiles.back()->set_QFileInfo(qfi);

    //QObject::connect(this->mtsite.get(), &mt_site::signal_mt_site_EDI_finished, this->edifiles.back().get(), &edi_file::set_edi_data_z );

    this->mtsite->insert_prc_com(this->cmdline);
    mtsite->run();
    this->edifiles.back()->set_edi_data_z(this->mtsite->zet, this->mtsite->in_ats_prc_coms);

    // catch the result by slot

    if (this->edifiles.back()->write()) {

        this->slot_plot();
    }

}

void procmt_mini::slot_mt_site_finished()
{

    int i = 0;
    QStringList table_data;
    i = this->ui->tableWidget_msg->rowCount();
    this->ui->tableWidget_msg->setRowCount(this->msg->size() + i);
    while (!this->msg->isEmpty()) {
        table_data = this->msg->get_items();
        qDebug() << table_data;
        this->ui->tableWidget_msg->setItem(i, 0, new QTableWidgetItem(table_data.at(0)));
        this->ui->tableWidget_msg->setItem(i, 1, new QTableWidgetItem(table_data.at(1)));
        this->ui->tableWidget_msg->setItem(i, 2, new QTableWidgetItem(table_data.at(2)));
        this->ui->tableWidget_msg->setItem(i, 3, new QTableWidgetItem(table_data.at(3)));
        ++i;

    }
    //msg->show_and_empty();
    this->ui->tableWidget_msg->resizeColumnToContents(0);
    this->ui->tableWidget_msg->resizeColumnToContents(1);
    this->ui->tableWidget_msg->resizeColumnToContents(2);
    this->ui->tableWidget_msg->resizeColumnToContents(3);
    this->update();


}


void procmt_mini::slot_mtdata_finished(const QString &message)
{

    // catch the thread
    this->pmt_thread.join();
    qDebug() << "measdoc closed ";
    this->ui->statusBar->showMessage(message);

    if (!this->processing_queue.empty()) {
        this->processing_queue.front()->disconnect();
        this->processing_queue.front().reset();
        this->processing_queue.pop();
    }

    this->slot_recalculate();

    //this->pmt_lib.reset();





    if (!this->processing_queue.empty()) {
        this->pmt_thread = std::thread(&procmt_lib::run, this->processing_queue.front());

    }
    this->ui->statusBar->showMessage(this->message_queue_size + QString::number(this->processing_queue.size()) + this->messages_running);

    // this->ui->statusBar->showMessage("waiting for files");

}

void procmt_mini::slot_file_progess(const QString &message)
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);
    this->ui->statusBar->showMessage(this->message_queue_size + QString::number(this->processing_queue.size()) + this->messages_running + message);
}

void procmt_mini::slot_change_prc_com_variant(const QString &key, const QVariant &value)
{

    // save the last valid
    if ((key == "parzen_radius") && value.toDouble() != 0.0) {
        this->old_parzen_radius = value.toDouble();
    }
    // disable parzen if I have lines
    if ((key == "min_lines") && (value.toInt() == 0)) {
        this->scc[this->mapscc.value("parzen_radius")]->set_qvariant_to_current_index(QVariant(this->old_parzen_radius));
        this->cmdline->set_key_value("parzen_radius", this->old_parzen_radius);

    }
    else if ((key == "min_lines") && (value.toInt() != 0)) {
        this->scc[this->mapscc.value("parzen_radius")]->set_qvariant_to_current_index(QVariant(0));
        this->cmdline->set_key_value("parzen_radius", 0);
        this->scc[this->mapscc.value("fcut_upper")]->set_qvariant_to_current_index(QVariant(0.3));
        this->scc[this->mapscc.value("fcut_lower")]->set_qvariant_to_current_index(QVariant(0.1));


    }

    if ((key == "xstddev") && (value.toString() == "auto")) {

    }

    this->cmdline->set_key_value(key, value);




}

void procmt_mini::slot_set_survey_base_dir(const QString &survey_base_dir)
{
    QFileInfo qfix(survey_base_dir);

    if (qfix.exists() && qfix.isDir()) {
        this->ui->dirs_base_lineEdit->setText("");
        this->ui->dirs_base_lineEdit->set_dir(survey_base_dir, 2); // full path
        on_dirs_base_lineEdit_valid_dir(true);



    }

}


void procmt_mini::on_run_pushButton_clicked()
{
    if (!this->qfi_base.filePath().size()) return;

    this->evaluate_mt_type();
    if (!this->allmeasdocs_center.size()) return;
    QList<qint64> ovrs;
    if(this->my_mttype == mttype::rr) {
        xml_measdoc_overlap_time(this->allmeasdocs_center, this->allmeasdocs_rr, ovrs);
    }
    else if (this->my_mttype ==  mttype::emap) {
        xml_measdoc_overlap_time(this->allmeasdocs_center, this->allmeasdocs_emap, ovrs);

    }
    else if (this->my_mttype == mttype::emap_rr) {
        xml_measdoc_overlap_time(this->allmeasdocs_center, this->allmeasdocs_rr, ovrs);

        xml_measdoc_overlap_time(this->allmeasdocs_center, this->allmeasdocs_emap, ovrs);


    }

    if (!this->allmeasdocs_center.size()) return;

    //this->ui->scrollFreqs->layout() has to get new buttons
    for (auto widget: ui->scrollFreqs->findChildren<QWidget*>())
        delete widget;
    this->frequency_buttons.clear();

    // check the overlapping according to the frequency



    if (this->mtsite != nullptr) this->mtsite.reset();

    // Qt6
    if (this->spc_plotter != nullptr) {
        this->spc_plotter->close();
        this->spc_plotter.reset();
    }



    if (this->ui->cb_auto_residuals->isChecked()) this->cmdline->set_key_value("remove_auto_residuals", 1);
    else this->cmdline->set_key_value("remove_auto_residuals", 0);
    if (this->ui->cb_rr_residuals->isChecked()) this->cmdline->set_key_value("remove_rr_residuals", 1);
    else this->cmdline->set_key_value("remove_rr_residuals", 0);

    this->cmdline->insert("target_frequencies_table", this->ftable_combo->currentText());


    this->mtsite = std::make_shared<mt_site>(this->num_threads, this->my_mttype, this);
    this->mtsite->insert_prc_com(this->cmdline);


    QObject::connect(this->mtsite.get(), &mt_site::signal_general_msg,      this->msg.get(), &msg_logger::slot_general_msg, Qt::QueuedConnection);
    QObject::connect(this->mtsite.get(), &mt_site::signal_general_msg_nums, this->msg.get(), &msg_logger::slot_general_msg_nums, Qt::QueuedConnection);
    QObject::connect(this->mtsite.get(), &mt_site::signal_general_msg_4strs, this->msg.get(), &msg_logger::slot_general_msg_4strs, Qt::QueuedConnection);
    QObject::connect(this->mtsite.get(), &mt_site::signal_mt_site_finished, this, &procmt_mini::slot_mt_site_finished, Qt::QueuedConnection);
    QObject::connect(this->mtsite.get(), &mt_site::signal_frequency_added,  this, &procmt_mini::slot_add_f_button, Qt::QueuedConnection);

    QObject::connect(this->mtsite.get(), &mt_site::signal_get_max_counts_and_last, this, &procmt_mini::slot_set_max_counts_and_last, Qt::DirectConnection );
    QObject::connect(this->mtsite.get(), &mt_site::signal_update_progress_bar, this, &procmt_mini::slot_update_progress_bar, Qt::DirectConnection);

    this->processing_queue.emplace(std::make_shared<procmt_lib>(cmdline, msg, mtsite, this));
    QObject::connect(processing_queue.back().get(), &procmt_lib::signal_mtdata_finished, this, &procmt_mini::slot_mtdata_finished, Qt::QueuedConnection );
    QObject::connect(processing_queue.back().get(), &procmt_lib::signal_file_progess, this, &procmt_mini::slot_file_progess, Qt::DirectConnection );

    QObject::connect(processing_queue.back().get(), &procmt_lib::signal_get_max_counts_and_last, this, &procmt_mini::slot_set_max_counts_and_last, Qt::QueuedConnection );
    QObject::connect(processing_queue.back().get(), &procmt_lib::signal_update_progress_bar, this, &procmt_mini::slot_update_progress_bar, Qt::QueuedConnection );


    this->ui->tabWidget->setCurrentIndex(2);

    processing_queue.back()->set_data(this->allmeasdocs_center, this->allmeasdocs_emap, this->allmeasdocs_rr,
                                      this->my_mttype, this->my_proc_type);

    if (this->processing_queue.size() == 1) {
        this->pmt_thread = std::thread(&procmt_lib::run, this->processing_queue.front());
    }
    this->ui->statusBar->showMessage(this->message_queue_size + QString::number(this->processing_queue.size()) + this->messages_running);

}

void procmt_mini::fill_prc_com_with_base_values()
{
    this->cmdline->insert_szt("wl", 1024);                 // want size_t for window length of fft
    this->cmdline->insert_szt("nbuffers", 240);              // chache size of the ats threadbuffer capacity; min should be three - max depends on your system memory; if greater than possbile stack: useless
    this->cmdline->insert_szt("overlapping", 0);           // overlapping in points - if you have SET ZERO IN THIS GUI
    this->cmdline->insert("overlapping_factor", double(0.0));  // overlapping in scale - that may easier

    this->cmdline->insert_szt("min_lines", 0);            // spectral lines to be joined as same frequency


    this->cmdline->insert("ascii", "ts");                  // ? wants ascii spectra
    //this->cmdline->insert("ats", "ts");                  // ? wants ats timeseries
    this->cmdline->insert("task", "convert");              // ?
    this->cmdline->insert("filter_name", "mtx32");         // filter with mtx32, mtx8 ... mtx4: mtx4 give best overlapping; in theory: record with 16x sample freqs and filter in between with 4
    this->cmdline->insert_double("upper_freq", double(0.0));
    this->cmdline->insert_double("lower_freq", double(0.0));

    this->cmdline->insert_double("fcut_upper", double(0.1));      // 10% loss of upper FFT - that is ok
    this->cmdline->insert_double("fcut_lower", double(0.03));     // 3% loss of lower part off FFT
    this->cmdline->insert_double("parzen_radius", double(0.1));   // parzen radius for mtspectra

    this->cmdline->insert_double("auto_bandwidth", double(1.0));  // override the window length depending on the sampling frequency
    this->cmdline->insert_double("auto_parzen_radius", double(1.0)); // override the parzen radius depending on the sampling frequency

    this->cmdline->insert_int("skip_marked_parts", 0);

    this->cmdline->insert("mtspectra", "std");
    this->cmdline->insert("task", "acspectra");
    this->cmdline->insert("subtask", "nothing");

    this->cmdline->insert("calibration", "auto");
    this->cmdline->insert("ats_scale", "auto");            // filter does not want to scale

    this->cmdline->insert("survey_basedir", "");
    this->cmdline->insert("site_dir", "");
    this->cmdline->insert("site_dir_emap", "");
    this->cmdline->insert("site_dir_rr", "");

    this->cmdline->insert_int("mttype", 0);                    // ref procmt_alldefines

    this->cmdline->insert_int("remove_auto_residuals", 1);
    this->cmdline->insert_int("remove_rr_residuals", 1);
    if (cmdline->contains("target_frequencies_table")) {
        if (cmdline->svalue("target_frequencies_table").size()) {
            this->cmdline->insert("target_frequencies_table", cmdline->svalue("target_frequencies_table"));
        }
    }



    ///////////////////////////////////////////////



}



//void procmt_mini::check_dirlines()
//{
//    if (this->ui->dirs_emap_lineEdit->text().size() && this->ui->dirs_center_lineEdit->text().size() && this->ui->dirs_rr_lineEdit->text().size() ) this->ui->radio_emap_rr->clicked(true);
//    if (!this->ui->dirs_emap_lineEdit->text().size() && this->ui->dirs_center_lineEdit->text().size() && this->ui->dirs_rr_lineEdit->text().size() ) this->ui->radio_rr->clicked(true);
//    if (this->ui->dirs_emap_lineEdit->text().size() && this->ui->dirs_center_lineEdit->text().size() && !this->ui->dirs_rr_lineEdit->text().size() ) this->ui->radio_emap->clicked(true);
//    if (!this->ui->dirs_emap_lineEdit->text().size() && this->ui->dirs_center_lineEdit->text().size() && !this->ui->dirs_rr_lineEdit->text().size() ) this->ui->radio_mt->clicked(true);

//}

void procmt_mini::slot_set_max_counts_and_last(const size_t &max_counts, const size_t &last_reading)
{

    // std::lock_guard<std::recursive_mutex> lock(this->mutex);

    qDebug() << "Progress bar" << max_counts << "," << last_reading;
    this->qprgb->setRange(0, int(max_counts));

}

void procmt_mini::slot_update_progress_bar(const int counts)
{
    //std::lock_guard<std::recursive_mutex> lock(this->mutex);

    this->qprgb->setValue(counts);
}


void procmt_mini::on_pushButton_to_settings_clicked()
{
    int i = this->ui->tabWidget->currentIndex();
    if (i < 2)    this->ui->tabWidget->setCurrentIndex(++i);
    else this->ui->tabWidget->setCurrentIndex(2);
}

void procmt_mini::slot_plot()
{

    if (this->procediplotter != Q_NULLPTR) {
        this->procediplotter->close();

    }
    else procediplotter = new QProcess(Q_NULLPTR);
    //QStringList site_names;
    if (!this->site_names.contains(this->edifile_name)) site_names.append(this->edifile_name);

    QString procpath;
    //procpath = "/home/bfr/cpp/sw/mtx_devel/proc_mt/opensuse_tumbleweed_gcc/bin/ediplotter";
#ifdef Q_OS_LINUX
    QFileInfo qfied(fall_back_default_str("ediplotter"));
    procpath =  qfied.absoluteFilePath();
#endif
#ifdef Q_OS_MACOS
    QFileInfo qfied(fall_back_default_str("ediplotter.app"));
    procpath =  qfied.absoluteFilePath();
#endif
#ifdef Q_OS_WIN
    QFileInfo qfied(fall_back_default_str("ediplotter.exe"));
    procpath =  qfied.absoluteFilePath();
#endif
#ifdef yasc
    procpath = "D:/projects/Qt/metronix/ProcMT_2019_03_07/ProcMT/ediplotter.exe";
#endif


    QFileInfo qfi(procpath);

    if (qfi.exists()) {
        procediplotter->start(procpath, site_names);
    }
}

void procmt_mini::slot_tsplotter(const QString &xmlfile)
{
    QProcess* tsplotter_proc = new QProcess(Q_NULLPTR);

    QStringList xmlfiles;
    xmlfiles << xmlfile;
    QString procpath;
#ifdef Q_OS_LINUX
    QFileInfo qfied(fall_back_default_str("tsplotter"));
    procpath =  qfied.absoluteFilePath();
#endif
#ifdef Q_OS_MACOS
    QFileInfo qfied(fall_back_default_str("tsplotter.app"));
    procpath =  qfied.absoluteFilePath();
#endif
#ifdef Q_OS_WIN
    QFileInfo qfied(fall_back_default_str("tsplotter.exe"));
    procpath =  qfied.absoluteFilePath();
#endif
#ifdef yasc
    procpath = "D:/projects/Qt/metronix/ProcMT_2019_03_07/ProcMT/tsplotter.exe";
#endif


    QFileInfo qfi(procpath);

    if (qfi.exists()) {
        tsplotter_proc->start(procpath, xmlfiles);

    }
}



void procmt_mini::on_pushButton_recaluculate_clicked()
{
    if (this->mtsite != nullptr) {
        this->slot_recalculate();
    }
}

void procmt_mini::slot_actual_edifile_name(const QString edifile_name)
{
    this->edifile_name = edifile_name;
}

void procmt_mini::slot_set_bw_prz(const QString &str_smooth_medium_sharp)
{
    if (str_smooth_medium_sharp == "smooth") {
        this->scc[this->mapscc.value("auto_bandwidth")]->set_qvariant_to_current_index(QVariant(2.0));
        this->scc[this->mapscc.value("auto_parzen_radius")]->set_qvariant_to_current_index(QVariant(2.0));

    }
    if (str_smooth_medium_sharp == "medium") {
        this->scc[this->mapscc.value("auto_bandwidth")]->set_qvariant_to_current_index(QVariant(1.0));
        this->scc[this->mapscc.value("auto_parzen_radius")]->set_qvariant_to_current_index(QVariant(1.0));
    }
    if (str_smooth_medium_sharp == "sharp") {
        this->scc[this->mapscc.value("auto_bandwidth")]->set_qvariant_to_current_index(QVariant(0.125));
        this->scc[this->mapscc.value("auto_parzen_radius")]->set_qvariant_to_current_index(QVariant(0.25));
    }

    if (str_smooth_medium_sharp == "off") {
        this->scc[this->mapscc.value("auto_bandwidth")]->set_qvariant_to_current_index(QVariant(0.0));
        this->scc[this->mapscc.value("auto_parzen_radius")]->set_qvariant_to_current_index(QVariant(0.0));

    }
}

void procmt_mini::on_pushButton_save_spectra_clicked()
{
    if (this->mtsite != nullptr) {

        //        QFileInfo edf(this->edifile_name);
        //        QString dumpname = this->qfi_base.absoluteFilePath() + "/dump/" + edf.baseName() + ".binspectra";
        //        this->last_spectral_dump_qfi.setFile(dumpname);

        auto msgBox = new QMessageBox(this);
        msgBox->setWindowTitle("Saving to SQLite");
        msgBox->setText("Takes minutes depending on CPU and disk");
        msgBox->setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
        int msgBoxCode = msgBox->exec();
        if (msgBoxCode == QMessageBox::Save ) {
            auto wtBox = new QMessageBox(this);
            wtBox->setWindowTitle("Saving to SQLite ....");
            wtBox->setText("wait until window closes!");
            wtBox->setMinimumWidth(144);
            wtBox->setStandardButtons(QMessageBox::StandardButtons());
            wtBox->show();
            wtBox->raise();
            std::unique_ptr<spc_db> spcd = std::make_unique<spc_db>(this->qfi_base);
            spcd->create_new_db(this->qfi_center.baseName(), this->mtsite->ac_spectra, mtsite->in_ats_prc_coms);
            wtBox->close();
            delete(wtBox);
        }

    }
}

void procmt_mini::on_pushButton_load_spectra_clicked()
{
    if (this->basedir.absolutePath().size() == 0) return;
    if (this->mtsite != nullptr) this->mtsite.reset();
    auto dialog = new QFileDialog(this);
    dialog->setViewMode(QFileDialog::Detail);
    dialog->setFileMode(QFileDialog::AnyFile);
    dialog->setModal(false);
    if (cmdline->svalue("survey_basedir") != "empty") dialog->setDirectory(cmdline->svalue("survey_basedir"));

    dialog->setNameFilter(tr("binary spectra (*.sql3*)"));
    dialog->show();
    QStringList fileNames;
    if (dialog->exec()) fileNames = dialog->selectedFiles();

    if (fileNames.size()) {
        QFileInfo qfi(fileNames.at(0));
        std::unique_ptr<spc_db> spcd = std::make_unique<spc_db>(this);

        if (this->mtsite != nullptr) this->mtsite.reset();
        this->mtsite = std::make_shared<mt_site>(this->num_threads, this->my_mttype, this);
        connect(this->mtsite.get(), &mt_site::signal_general_msg, this->msg.get(), &msg_logger::slot_general_msg);
        connect(this->mtsite.get(), &mt_site::signal_general_msg_nums, this->msg.get(), &msg_logger::slot_general_msg_nums);
        std::vector<prc_com> prcs;
        if (spcd->fetch_all(qfi, this->mtsite->ac_spectra, prcs)) {
            this->mtsite->in_ats_prc_coms = prcs;
            this->mtsite->resize_z();
            this->edifile_name = qfi.baseName() + ".edi";
            this->ui->dirs_center_lineEdit->setText(qfi.baseName());
            this->mtsite->insert_prc_com(this->cmdline);
            if (this->ui->cb_auto_residuals->isChecked()) this->cmdline->set_key_value("remove_auto_residuals", 1);
            else this->cmdline->set_key_value("remove_auto_residuals", 0);
            if (this->ui->cb_rr_residuals->isChecked()) this->cmdline->set_key_value("remove_rr_residuals", 1);
            else this->cmdline->set_key_value("remove_rr_residuals", 0);
            this->mtsite->clean_up_residuals();
            this->ui->pushButton_recaluculate->clicked(true);
        }
        else {
            if (this->mtsite != nullptr) this->mtsite.reset();
        }


        //        if(this->mtsite->read_bin_spectra(qfi)) {
        //            this->last_spectral_dump_qfi.setFile(qfi.filePath());
        //        }
    }
}


void procmt_mini::on_pushButton_spectral_plotter_clicked()
{

    bool was_open;
    // Qt6
    if (this->spc_plotter == nullptr) {
        this->spc_plotter = std::make_unique<spectra_plotter>(this);
        connect(this->mtsite.get(), &mt_site::signal_mt_site_finished, this->spc_plotter.get(), &spectra_plotter::slot_replot);
        connect(this->mtsite.get(), &mt_site::signal_mt_site_updated, this->spc_plotter.get(), &spectra_plotter::slot_replot);

        was_open = false;

    }
    else {
        was_open = true;
    }
    if (!was_open) this->spc_plotter->set_mt_site(this->mtsite);
    this->spc_plotter->show();



////////////// forget
    //    if (this->last_spectral_dump_qfi.exists()) {
    //        QProcess *proc = new QProcess(Q_NULLPTR);


    //        QString procpath;
    //        procpath = procmt_homepath() + "/spectra_plotter";
    //        QStringList fileNames;
    //        fileNames.append( this->last_spectral_dump_qfi.filePath());
    //#ifdef yasc
    //        procpath = procmt_homepath() + "/spectra_plotter.exe";
    //#endif

    //        // if (this->spc_plot != nullptr) this->spc_plot = make_unique<spectra_plotter>(this);
    //        // this->spc_plot->set_mt_site(this->mtsite);

    //        proc->start(procpath, fileNames);
    //    }
}

void procmt_mini::on_pushButton_reset_tensor_clicked()
{
    if (this->mtsite != nullptr) {
        this->mtsite->slot_reset_booleans();
    }
}


void procmt_mini::on_pushButton_clear_log_clicked()
{
    //this->ui->tableWidget_msg->clearContents();
    this->ui->tableWidget_msg->setRowCount(0);
}

void procmt_mini::on_pushButton_edi_plotter_clicked()
{
    this->slot_plot();
}

void procmt_mini::on_pushButton_clear_edi_clicked()
{
    this->site_names.clear();
    if (this->procediplotter != Q_NULLPTR) {
        this->procediplotter->close();
    }

}

void procmt_mini::on_pushButton_export_ascii_clicked()
{
    if (this->mtsite == nullptr) return;
    QString site_name;
    if (this->ui->dirs_emap_lineEdit->text().size()) {
        site_name = this->qfi_base.baseName();
    }
    else {
        site_name = this->qfi_base.baseName();
    }
    QString md(this->cmdline->svalue("survey_basedir") + "/dump/" + site_name);

    QDir dir(md);
    if (!dir.exists()) dir.mkdir(md);

    QFileInfo qfi(md);
    this->mtsite->write_ascii_single_spectra(qfi);

}


void procmt_mini::on_pushButton_clear_rr_lineEdit_clicked()
{
    this->allmeasdocs_rr.clear();
    this->ui->dirs_rr_lineEdit->clear();
    this->evaluate_mt_type();

}

void procmt_mini::on_pushButton_clear_base_lineEdit_clicked()
{
    this->ui->dirs_base_lineEdit->clear();
    this->cmdline->set_key_value("survey_basedir", "");
    this->site_names.clear();
    on_pushButton_clear_emap_lineEdit_clicked();
    on_pushButton_clear_center_lineEdit_clicked();
    on_pushButton_clear_rr_lineEdit_clicked();
    this->my_mttype = mttype::nomt;
    if (this->procediplotter != Q_NULLPTR) {
        this->procediplotter->close();
    }
}

void procmt_mini::on_pushButton_clear_emap_lineEdit_clicked()
{
    this->allmeasdocs_emap.clear();
    this->ui->dirs_emap_lineEdit->clear();
    this->evaluate_mt_type();
}

void procmt_mini::on_pushButton_clear_center_lineEdit_clicked()
{
    this->allmeasdocs_center.clear();
    this->ui->dirs_center_lineEdit->clear();
    this->evaluate_mt_type();

}

void procmt_mini::on_skip_marked_parts_clicked()
{
    if (this->skip_marked_parts->isChecked()) {
        this->cmdline->insert_int("skip_marked_parts", 1);
    }
    else {
        this->cmdline->insert_int("skip_marked_parts", 0);
    }
}

void procmt_mini::on_pushButton_deselect_all_frequencies_clicked()
{
    this->check_freq->check_all(false);
}

void procmt_mini::on_pushButton_select_all_frequecies_clicked()
{
    this->check_freq->check_all(true);

}

void procmt_mini::re_check_allmeasdocs(const std::vector<double> &freqs)
{

    this->allmeasdocs_center.clear();
    if (!freqs.size()) return;
    for (auto & f: freqs) {
        QString ende(doublefreq2string_unit_measdoc(f));
        for (auto &doc : allmeasdocs_center_orig) {
            QFileInfo qfi(doc);
            if (qfi.baseName().endsWith(ende, Qt::CaseInsensitive)) allmeasdocs_center.append(doc);
        }
    }
#ifdef QT_DEBUG
    for (auto &doc : allmeasdocs_center) {
        QFileInfo qfi(doc);
        qDebug() << qfi.baseName();
    }

#endif
}

void procmt_mini::on_actionHelp_triggered()
{
    this->pmt_help.help("procmt", "procmt.html", 2);
}

void procmt_mini::on_actionCreate_Survey_triggered()
{
    QDir directory(QDir::home());
    QString path = QFileDialog::getExistingDirectory (this, tr("Directory"), directory.path());
    if ( path.isNull() == false )
    {
        directory.setPath(path);

    }
    if (directory.isEmpty()) {
        for (auto &str : pmt::survey_dirs) {
            directory.mkdir(str);

        }
        this->ui->dirs_base_lineEdit->setText(directory.absolutePath());

    }

    //    QDir dirprocessings(this->appdir + "/processings");
    //    if (dirprocessings.exists()) {
    //        this->copyFolder(this->appdir + "/processings", directory.absolutePath() + "/processings" );
    //    }

    //return path;
}

void procmt_mini::on_pushButton_close_all_plots_clicked()
{
    this->plots_re.close();
    this->plots_im.close();
    this->plots.close();
}

void procmt_mini::on_pushButton_dump_atze_clicked()
{
    if (this->mtsite == nullptr) return;

    this->mtsite->dump_ac_spc();
}


void procmt_mini::on_pushButton_save_processing_clicked()
{
    auto dialog = new QFileDialog(this);
    dialog->setViewMode(QFileDialog::Detail);
    dialog->setFileMode(QFileDialog::AnyFile);
    dialog->setWindowTitle("save processing");
    dialog->setDirectory(this->basedir.absolutePath() + "/processings");
    dialog->setModal(false);
    dialog->show();

    QStringList fileNames;
    if (dialog->exec()) fileNames = dialog->selectedFiles();
    if (fileNames.size()) {
        qDebug() << fileNames.at(0);
        QFileInfo qfi(fileNames.at(0));
        if (!qfi.absoluteFilePath().endsWith(".xml", Qt::CaseInsensitive)) qfi.setFile(fileNames.at(0) + ".xml");
        prc_com_xml px;
        px.create("procmt_processing", &qfi);
        px.add_main("proc", this->cmdline->get_data());
        px.close_and_write();

    }
}

void procmt_mini::on_pushButton_load_processing_clicked()
{
    auto dialog = new QFileDialog(this);
    dialog->setViewMode(QFileDialog::Detail);
    dialog->setFileMode(QFileDialog::ExistingFile);
    dialog->setWindowTitle("load processing");
    dialog->setDirectory(this->basedir.absolutePath() + "/processings");
    dialog->setNameFilter(tr("processing (*.xml*)"));

    dialog->setModal(false);
    dialog->show();

    QStringList fileNames;
    if (dialog->exec()) fileNames = dialog->selectedFiles();
    if (fileNames.size()) {
        qDebug() << fileNames.at(0);
        QFileInfo qfi(fileNames.at(0));
        prc_com_xml px;
        px.open(&qfi);
        QMap<QString, QVariant> map = px.read("procmt_processing", "proc");
        qDebug() << "map loaded";
        // insert data IF existing in cmdline
        this->cmdline->set_data(map,false);
        QStringList keys = this->cmdline->keys();
        for (const auto &str : keys) {
            for (auto &sc : this->scc) {
                if (sc->get_key() == str) sc->set_data(this->cmdline->value(str));
            }
            for (auto &sc : this->cbp) {
                if (sc->get_key() == str) sc->set_data(this->cmdline->value(str));
            }
        }

    }
}
std::unique_ptr<atsheader> procmt_mini::get_atsheader_first_xml_first_ats()
{
    QList<int> channel_nos;
    if (this->allmeasdocs_emap.size()) {
        QFileInfo files (allmeasdocs_emap.at(0));
        measdocxml measdoc(&files);
        QFileInfo opens(measdoc.get_atsfiles(channel_nos).at(0));
        return std::make_unique<atsheader>(opens);

    }
    else if(allmeasdocs_center.size()) {
        QFileInfo files (allmeasdocs_center.at(0));
        measdocxml measdoc(&files);
        QString firstf (measdoc.get_atsfiles(channel_nos).at(0));
        QFileInfo opens(files.absolutePath() + "/" + firstf);
        return std::make_unique<atsheader>(opens);
    }
return std::make_unique<atsheader>();
}

void procmt_mini::on_OSM_push_button_clicked()
{
    if (!this->allmeasdocs_center.size()) return;
    std::unique_ptr<atsheader> atsh = this->get_atsheader_first_xml_first_ats();
    if (atsh == nullptr) return;
    atsh->scan_header_close();
    std::unique_ptr<geocoordinates> coord = std::make_unique<geocoordinates>();
    long double lat_rad, lon_rad, elev;
    coord->set_lat_lon_msec(atsh->ivalue("latitude"), atsh->ivalue("longitude"), (long double)(atsh->dvalue("elevation")/100.));
    coord->coordinates(lat_rad,lon_rad, elev);
    // site_name
    lat_rad = lat_rad / M_PI * 180.;
    lon_rad = lon_rad / M_PI * 180.;
    QUrl url;
    //https://www.j-berkemeier.de/osm_marker.html?lat=52.518493,lon=13.376026,info=Reichstagsgeb%C3%A4ude;lat=52.516195,lon=13.377442,info=Brandenburger%20Tor;lat=52.519981,lon=13.36916,info=Bundeskanzleramt
    url.setUrl("https://www.openstreetmap.org/?mlat=" + QString::number(lat_rad, 'g') + "&mlon=" + QString::number(lon_rad, 'g'));
    //url.setUrl("https://geo-metronix.de/osm_marker.html?lat=" + QString::number(lat_rad, 'g') + ",lon=" + QString::number(lon_rad, 'g') + ",info=" + this->atsh->svalue("site_name"));

    QDesktopServices::openUrl(url);
}

void procmt_mini::on_GMaps_push_button_clicked()
{
    if (!this->allmeasdocs_center.size()) return;

    if (!this->allmeasdocs_center.size()) return;
    std::unique_ptr<atsheader> atsh = this->get_atsheader_first_xml_first_ats();
    if (atsh == nullptr) return;
    atsh->scan_header_close();
    std::unique_ptr<geocoordinates> coord = std::make_unique<geocoordinates>();
    long double lat_rad, lon_rad, elev;
    coord->set_lat_lon_msec(atsh->ivalue("latitude"), atsh->ivalue("longitude"), (long double)(atsh->dvalue("elevation")/100.));
    coord->coordinates(lat_rad,lon_rad, elev);
    // site_name
    lat_rad = lat_rad / M_PI * 180.;
    lon_rad = lon_rad / M_PI * 180.;

    QUrl url;
    // https://www.google.com/maps/search/?api=1&query=47.5951518,-122.3316393
    url.setUrl("https://www.google.com/maps/search/?api=1&query=" +  QString::number(lat_rad, 'g') + "," + QString::number(lon_rad, 'g'));
    QDesktopServices::openUrl(url);
}

