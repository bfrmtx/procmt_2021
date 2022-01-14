#include "mt_gensign.h"
#include "ui_mt_gensign.h"

mt_gensign::mt_gensign(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mt_gensign)
{
    ui->setupUi(this);

    // ADU works in UTC always
    this->ui->start_datetime->setTimeSpec(Qt::UTC);
    this->ui->stop_datetime->setTimeSpec(Qt::UTC);
    this->ui->start_datetime->setDateTime(QDateTime::fromString("2000-01-01 12:00:00", "yyyy-MM-dd HH:mm:ss"));
    this->ui->stop_datetime->setDateTime(QDateTime::fromString("2000-01-01 12:01:00", "yyyy-MM-dd HH:mm:ss"));

    //this->ui->slice_length->setValue(this->wl);;
    this->ui->slices->setValue(this->slices);



    this->basedir= procmt_homepath("5chanEHnoise") + "/";
    this->HWConfig =      this->basedir +      "HwConfig.xml";
    this->HwDatabase =    this->basedir +    "HwDatabase.xml";
    this->HwStatus =      this->basedir +      "HwStatus.xml";
    this->ChannelConfig = this->basedir + "ChannelConfig.xml";
    this->outputjobsdir = "/tmp/aduxml/";
    this->qfiHWConfig = new QFileInfo(HWConfig);
    this->qfiHwDatabase = new QFileInfo(HwDatabase);
    this->qfiChannelConfig = new QFileInfo(ChannelConfig);
    this->qfiHwStatus = new QFileInfo(HwStatus);

    this->cht << "Ex" << "Ey" << "Hx" << "Hy" << "Hz";
    //this->outdir = (QDir::homePath() + "/gensign/");

    QString outdir(QDir::homePath() + "/gensign/ts/Site_1/empty");
    setsm = outdir;
    if (!setsm.exists()) {
        qDebug() << setsm.path();
        setsm.mkpath(setsm.path());
    }

    if (!setsm.exists()) {
        exit(1);
    }


    this->deldir_string = outdir;
    this->ui->label_output_dir->setText(setsm.absolutePath());

    this->eqdt_start.set_sample_freq(1024.);
    this->eqdt_start.set_num_samples(this->wl * this->slices);


    this->qfi_db.setFile(procmt_homepath("info.sql3"));
    this->selectfreq = std::make_unique<frequencies_combo>  ( this->qfi_db.absoluteFilePath(), this->ui->select_sample_feq);

    this->fsine_f.resize(4, 0.0);
    this->fsine_a.resize(4, 0.0);
    this->fsine_p.resize(4, 0.0);

    this->fsine_a[0] = 1;
    this->fsine_a[1] = 1;
    this->fsine_a[2] = 1;
    this->fsine_a[3] = 1;

    this->ui->fsine_f1->setValue(this->fsine_f[0]);
    this->ui->fsine_f2->setValue(this->fsine_f[1]);
    this->ui->fsine_f3->setValue(this->fsine_f[2]);
    this->ui->fsine_f3->setValue(this->fsine_f[3]);

    this->ui->fsine_a1->setValue(this->fsine_a[0]);
    this->ui->fsine_a2->setValue(this->fsine_a[1]);
    this->ui->fsine_a3->setValue(this->fsine_a[2]);
    this->ui->fsine_a3->setValue(this->fsine_a[3]);


    this->ui->fsine_p1->setValue(this->fsine_p[0]);
    this->ui->fsine_p2->setValue(this->fsine_p[1]);
    this->ui->fsine_p3->setValue(this->fsine_p[2]);
    this->ui->fsine_p3->setValue(this->fsine_p[3]);





    //    this->mapper = new QSignalMapper(this);

    //    this->ui->hl_sine_f1->addWidget(new QLabel("frequency", this));
    //    this->qdspins.insert("sine_f1", new QDoubleSpinBox(this));
    //    this->qdspins.last()->setObjectName("sine_f1");

    //    this->ui->hl_sine_f1->addWidget(this->qdspins.last());

    //    connect(this->qdspins.last(), SIGNAL(valueChanged(double)), this->mapper, SLOT(map()));

    //    this->mapper->setMapping(this->qdspins.last(), this->qdspins.last()->objectName());

    //    connect(this->mapper, SIGNAL(mapped(QString)), this, SLOT(sine_f_changed(QString)));

    this->eqdt_start = this->ui->start_datetime->dateTime();
    //this->eqdt_start.set_sample_freq(this->selectfreq->current_index_freq( this->ui->select_sample_feq->currentIndex() ));

    size_t i = 0;
    for (size_t j = 0; j < this->channels; ++j) {
        this->ui->vl1_chan->addWidget(new QLabel("channel " + QString::number(i), this));
        this->ch_combos.append(new QComboBox(this));
        if ( i < 2) {
            this->scboxes.emplace_back(std::make_unique<sensors_combo>( this->qfi_db.absoluteFilePath(), 'E',  this->ch_combos[i]));
        }
        else {
            this->scboxes.emplace_back(std::make_unique<sensors_combo>( this->qfi_db.absoluteFilePath(), 'H',this->ch_combos[i]));
        }
        this->ui->vl1_chan->addWidget(ch_combos[i]);
        ++i;
    }

    for (auto &box : this->ch_combos) {
        connect(box, SIGNAL(currentIndexChanged(int)), this, SLOT(sensor_changed(int)));
    }

    //    this->sgnck.append(this->ui->sine_waves_checkbox);
    //    connect(this->sgnck.last(), SIGNAL(clicked()), this->sgnck_mapper, SLOT(map()));
    //    this->sgnck_mapper->setMapping(this->sgnck.last(), this->sgnck.last()->text());
    //    connect(this->sgnck_mapper, SIGNAL(mapped(QString)), this, SLOT(sign_checked(QString)));

    this->check_list_waves = new checkbox_list(this);
    this->check_list_waves->append(this->ui->generate_sinewaves_tab_0);
    this->check_list_waves->append(this->ui->straight_100_mV);
    this->check_list_waves->append(this->ui->sawtooth_50_to_50_mV);
    this->check_list_waves->append(this->ui->one_rectangular_50_to_50_mV);
    //this->check_list_waves->append(this->ui->two_rectangular_50_to_50_mV);
    this->check_list_waves->append(this->ui->mt19937noise);
    this->check_list_waves->append(this->ui->std_mt_noise_coils);



    for (i = 0; i < this->channels; ++i) {
        this->generators.push_back(std::make_shared<generator>(this->slices, this->wl, this));
    }
    i = 0;
    for (auto &gener : this->generators) {
        QObject::connect(this->check_list_waves, &checkbox_list::btn_name_checked, gener.get(), &generator::slot_set_function);
        gener->channel_type = i++;
    }

    this->clear();

    // parzen
    //this->scc.append(new single_column_combo (this->dbname, "parzen_radius", "double", this, Q_NULLPTR,
    //                                          "parzen_radius", "Parzen radius"));

    //    single_column_combo(const QString& dbname, const QString &table_name, const QString &double_int_string,
    //                        QWidget *parent = Q_NULLPTR,  QComboBox *box = Q_NULLPTR,
    //                        const QString prc_com_key = "", const QString label_text = "", const bool init_database = true);

    this->selectwindow = std::make_unique<single_column_combo>(this->qfi_db.absoluteFilePath(),"slices", "int", this, this->ui->select_slice_length );
    this->selectwindow->set_qvariant_to_current_index(1024);
    //    this->selectwindow = std::make_unique<windowlength_combo>( this->qfi_db.absoluteFilePath(), this->ui->select_slice_length);
    //    this->selectwindow->set_window_length(this->wl);
    this->ui->select_sample_feq->setCurrentText("512 Hz");

    this->is_init = true;






}

mt_gensign::~mt_gensign()
{

    QDir dir(this->deldir_string);

    if (dir.exists() && dir.isEmpty()) {
        dir.rmdir(this->deldir_string);
    }

    delete this->qfiHWConfig;
    delete this->qfiHwDatabase;
    delete this->qfiChannelConfig;
    delete this->qfiHwStatus;
    delete ui;
}


void mt_gensign::seed_two_noise_src()
{

    std::random_device device_1;
    std::mt19937 generator_1(device_1());
    std::normal_distribution<double> uniform_1(0, 1);

    std::random_device device_2;
    std::mt19937 generator_2(device_2());
    std::normal_distribution<double> uniform_2(0, 1);

    this->noise_1.resize(this->wl);
    this->noise_2.resize(this->wl);

    size_t i, j, n;

    for (i = 0; i < slices; ++i) {
        for (j = 0; j < this->wl; ++j) {
            noise_1[j] = uniform_1(generator_1);
            noise_2[j] = uniform_2(generator_2);

        }
        // copy before deposit
        for (n = 0; n < this->channels; ++n) {
            this->noise_copy_1[n] = this->noise_1;
            this->noise_copy_2[n] = this->noise_2;
        }

        for (n = 0; n < this->channels; ++n) {
            if (i < this->slices - 1) {
                unique_noise_buffer_1[n]->deposit(this->noise_copy_1[n], threadbuffer_status::running);
                unique_noise_buffer_2[n]->deposit(this->noise_copy_2[n], threadbuffer_status::running);
            }
            else {

                unique_noise_buffer_1[n]->deposit(this->noise_copy_1[n], threadbuffer_status::finished);
                unique_noise_buffer_2[n]->deposit(this->noise_copy_2[n], threadbuffer_status::finished);
            }
        }

    }
}

void mt_gensign::create_unique_seed(bool createme)
{
    size_t i;
    if (createme) {
        this->noise_copy_1.resize(this->channels);
        this->noise_copy_2.resize(this->channels);


        for (i = 0; i < this->channels; ++i) {
            this->unique_noise_buffer_1.emplace_back(std::make_shared<threadbuffer<double>>(20, this->wl, atsfileout_unscaled_timeseries, i));
            this->unique_noise_buffer_2.emplace_back(std::make_shared<threadbuffer<double>>(20, this->wl, atsfileout_unscaled_timeseries, i));
            this->noise_copy_1[i].resize(this->wl);
            this->noise_copy_2[i].resize(this->wl);


        }
        for (i = 0; i < this->channels; ++i) {
            this->generators[i]->unique_noise_buffer_1 = this->unique_noise_buffer_1[i];
            this->generators[i]->unique_noise_buffer_2 = this->unique_noise_buffer_2[i];

        }
    }
    else {

        for (auto &gen : this->generators) {
            if (gen->unique_noise_buffer_1 != nullptr)  gen->unique_noise_buffer_1.reset();
            if (gen->unique_noise_buffer_2 != nullptr)  gen->unique_noise_buffer_2.reset();
        }

        for (i = 0; i < this->channels; ++i) {
            this->unique_noise_buffer_1[i].reset();
            this->unique_noise_buffer_1[i].reset();
            this->noise_copy_1[i].clear();
            this->noise_copy_2[i].clear();

        }
        this->unique_noise_buffer_1.clear();
        this->unique_noise_buffer_2.clear();
        this->noise_copy_1.clear();
        this->noise_copy_2.clear();
        this->uni_seed_threads.clear();

    }

}

void mt_gensign::create_xml()
{

    if (!this->atsfiles.size()) {
        qDebug() << "no ats files exist, can not create XML";
        return;
    }

    for ( auto & ats : this->atsfiles) {
        ats->set_sample_freq(this->f_sample, true, true);
        ats->init_calibration(0, false);
    }

    //    this->joblist->init_from_files(qfiHWConfig, qrlHWConfig, qbaHWConfig, qfiHwDatabase, qrlHwDatabase, qbaHwDatabase);
    //    this->joblist->job(0).fetch_atswriter_part(qfiChannelConfig, qrlChannelConfig, qbaChannelConfig);

    this->adujob = std::make_unique<adulib>(1,this);
    this->adujob->fetch_hwcfg(qfiHWConfig, qrlHWConfig, qbaHWConfig);
    this->adujob->fetch_hwdb(qfiHwDatabase, qrlHwDatabase, qbaHwDatabase);
    this->adujob->build_channels();
    this->adujob->build_hardware();



    this->adujob->set_start_time(this->ui->start_datetime->dateTime());
    this->adujob->on_set_duration(qint64((this->slices * this->wl)/this->f_sample),false);
    this->adujob->set("pos_x1", -500.0, 0, false);
    this->adujob->set("pos_x2", 500.0, 0, false);
    this->adujob->set("pos_y1", -500.0, 1, false);
    this->adujob->set("pos_y2", 500.0, 1, false);

    // add fake sample freqs
    this->adujob->set("sample_freq", this->f_sample, 1, false);

    this->adujob->set("system_serial_number", -1, false);
    int i = 0;
    for ( auto & ats : this->atsfiles) {


        // adu sets the chopper
        this->adujob->set("sample_freq", ats->value("sample_freq"), i, false);
        //qDebug() << this->adujob->get("hchopper",i).toInt();

        this->adujob->set("ts_lsb", ats->value("lsb"), i, false);
        this->adujob->set("sensor_type", ats->value("sensor_type"), i, false);
        this->adujob->set("num_samples", ats->value("num_samples"), i, false);
        this->adujob->set("sensor_sernum", ats->value("sensor_sernum"), i, false);
        this->adujob->set("ats_data_file", ats->get_qfile_info().fileName(), i, false);



        this->adujob->set("ADC_sernum", ats->value("ADC_sernum"), i, false);
        this->adujob->set("DC_offset", ats->value("DC_offset"), i, false);
        this->adujob->set("gain_stage1", ats->value("gain_stage1"), i, false);
        this->adujob->set("gain_stage2", ats->value("gain_stage2"), i, false);

        this->adujob->set("latitude", ats->value("latitude"), i, false);
        this->adujob->set("longitude", ats->value("longitude"), i, false);
        this->adujob->set("elevation", ats->value("elevation"), i, false);

        this->adujob->set("xmlcal_sensor", ats->get_xmlcal(), i, false);


        ++i;
    }


    //    this->adujob->on_set_filename( this->atsfiles.at(0)->get_qfile_info().absolutePath() +  "/001_" +
    //                                   this->atsfiles.at(0)->get_start_datetime().toString("yyyy-MM-dd_hh-mm-ss") + "_" +
    //                                   this->atsfiles.at(0)->get_stop_datetime().toString("yyyy-MM-dd_hh-mm-ss") + "_" +
    //                                   this->adujob->get_sample_file_string() + ".xml");


    this->adujob->on_set_filename( this->atsfiles.at(0)->absolutePath() + "/" + this->atsfiles.at(0)->get_measdoc_name());
    this->adujob->on_write_job(true);
    this->ui->start_datetime->setDateTime(this->eqdt_start.addMSecs(1000));




}





void mt_gensign::on_start_datetime_dateTimeChanged(const QDateTime &dateTime)
{
    this->eqdt_start = dateTime;
    for ( auto &file : atsfiles) {
        file->set_start_date_time(dateTime);
        qDebug() << file->get_new_filename(false, "08");
    }

    this->ui->stop_datetime->setDateTime(this->eqdt_start.sample_time(this->slices * this->wl));

}



void mt_gensign::on_select_sample_feq_currentIndexChanged(int index)
{
    if (this->selectfreq != nullptr) {
        this->f_sample = this->selectfreq->current_index_freq( this->ui->select_sample_feq->currentIndex() );
        for ( auto &file : atsfiles) {
            file->set_sample_freq(this->f_sample, true, false);
            file->set_key_value("num_samples", (quint64) (this->wl * this->slices) );

            if (this->f_sample > 512) {
                file->set_LSBMV_BB_HF_default();
                file->set_key_value("chopper_status", 0);
            }
            else {
                file->set_LSBMV_BB_LF_default();
                file->set_key_value("chopper_status", 1);
            }
            file->prc_com_to_classmembers();

            qDebug() << file->get_new_filename(false, "08");

        }

        this->eqdt_start.set_sample_freq(this->f_sample);
        this->ui->stop_datetime->setDateTime(this->eqdt_start.sample_time(this->wl * this->slices));
    }
}



void mt_gensign::sensor_changed(const int)
{
    size_t i = 0;
    for (auto &file : this->atsfiles) {
        int myindex = this->ch_combos[i]->currentIndex();
        qDebug() << "new sensor" << this->scboxes.at(i)->get_current_index_atsheader_sensor_name(myindex);
        file->set_key_value("sensor_type", this->scboxes.at(i)->get_current_index_atsheader_sensor_name(myindex));
        file->prc_com_to_classmembers();

        ++i;
    }
}

void mt_gensign::on_slices_valueChanged(int arg1)
{
    this->slices = arg1;
    this->eqdt_start.set_num_samples(this->wl * this->slices);
    this->ui->stop_datetime->setDateTime(this->eqdt_start.sample_time(this->wl * this->slices));
    for (auto &gener : this->generators) {
        gener->slot_set_slices(arg1);
    }

    for (auto &file : this->atsfiles) {
        file->set_key_value("num_samples", quint64(this->wl * this->slices) );
        file->prc_com_to_classmembers();

    }

}



void mt_gensign::on_run_pushButton_clicked()
{

    //    this->setsm = this->outdir;
    //    if (!this->setsm.exists()) {
    //        qDebug() << setsm.path();
    //        this->setsm.mkpath(this->setsm.path());
    //        this->setsm = this->outdir + "/" + "/ts/Site_1/empty";
    //        this->setsm.mkpath(this->setsm.path());
    //    }
    //     if (!this->setsm.exists()) {
    //         return;
    //     }

    for (auto &gener : this->generators) {
        gener->slot_set_sines(this->f_sample, this->fsine_f, this->fsine_a, this->fsine_p);
    }

    bool has_two_source_noise = false;

    if (this->ui->std_mt_noise_coils->isChecked()) {

        has_two_source_noise = true;
        this->create_unique_seed(true);

        this->uni_seed_threads.emplace_back( std::thread (&mt_gensign::seed_two_noise_src, this));
        qDebug() << "starting in uni noise thread";

    }

    int i = 0;
    for ( auto file : this->atsfiles) {
        this->buffers.emplace_back(file->get_create_buffer(20, atsfileout_unscaled_timeseries));

    }

    i = 0;
    for (auto &v : this->generators) {
        v->set_threadbuffer(this->buffers[i]);
        ++i;
    }


    i = 0;
    for (auto &v : this->generators) {
        this->generatorthreads.emplace_back( std::thread (&generator::seed, v));
        qDebug() << "starting in noise thread" << i++;
    }

    i = 0;
    for (auto &v : this->atsfiles) {
        this->writerthreads.emplace_back( std::thread (&atsfile::write, v));
        qDebug() << "starting in channel writer thread" << i++;
    }


    if (has_two_source_noise) {
        for (auto &v : this->uni_seed_threads) {
            qDebug() << "joining uni seeders: " << i++;
            v.join();
        }
    }


    i = 0;
    for (auto &v : this->generatorthreads) {
        qDebug() << "joining generators: " << i++;
        v.join();
    }

    i = 0;
    for (auto &v : this->writerthreads) {
        qDebug() << "joining writerthreads: " << i++;
        v.join();
    }

    if (has_two_source_noise) {
        this->create_unique_seed(false);
        has_two_source_noise = false;

    }

    this->generatorthreads.clear();
    this->writerthreads.clear();

    this->create_xml();

    qDebug() << "finished all threads";

    // if (this->setsm.isEmpty()) this->setsm.rmdir(this->setsm.absolutePath());


    this->clear();


}

void mt_gensign::clear()
{

    int i;


    for (auto &gener : this->generators) {
        if (gener != nullptr) gener->buffer.reset();
        if (gener != nullptr) gener->unique_noise_buffer_1.reset();
        if (gener != nullptr) gener->unique_noise_buffer_2.reset();

    }
    //this->generators.clear();


    for (auto &v : this->buffers) {
        if (v != nullptr)  v.reset();
    }
    this->buffers.clear();


    for (auto &v : this->atsfiles) {
        if (v != nullptr)  v.reset();
    }
    this->atsfiles.clear();

    for (size_t ki = 0; ki < this->channels; ++ki) {
        QFileInfo qfi("test.ats");
        this->atsfiles.emplace_back(std::make_shared<atsfile>(qfi, 1, 80, 1, this->f_sample, 1, ki, cht.at(ki), "L"));
    }
    i = 0;
    for ( auto &file : this->atsfiles) {
        file->set_slot(i++);
        file->set_LSBMV_BB_LF_default();
        file->set_basedir(setsm);
        file->set_key_value("num_samples", quint64(this->wl * this->slices) );
        file->set_key_value("system_serial_number", 1);
        file->set_key_value("header_version", 80);
        file->set_key_value("header_length", 1024);
        file->set_key_value("sensor_sernum", i+1);
        file->set_key_value("ADC_sernum", i+1);

        if (i < 2) {
            file->set_key_value("probe_resistivity", 200 + i + 1);
        }
        else {
            file->set_key_value("probe_resistivity", 4.80664e+06);
        }

        file->set_key_value("DC_offset", 0.0);
        file->set_key_value("gain_stage1", 1);
        file->set_key_value("gain_stage2", 1);




        file->set_key_value("comments", "mt_gensign");
        file->set_key_value("site_name", "laboratory");

        file->set_key_value("latitude", 187230024);
        file->set_key_value("longitude", 38538792);
        file->set_key_value("elevation", 22790);

        file->prc_com_to_classmembers();
        file->set_window_length(1024, 0, 0, false, quint64(this->wl * this->slices), 0, atsfileout_unscaled_timeseries);

        file->prc_com_to_classmembers();




    }

    // setup with actual values
    this->atsfiles[0]->set_e_pos(-500.0, 500.0, 0., 0., 0., 0.);
    this->atsfiles[1]->set_e_pos(0., 0., -500., 500.0, 0., 0.);
    this->sensor_changed(0);
    this->on_select_sample_feq_currentIndexChanged(0);
    this->ui->start_datetime->dateTimeChanged( this->ui->start_datetime->dateTime());


    qDebug() << "mt_gensign.cleared";



}

generator::generator(const size_t &slices, const size_t &slice,  mt_gensign *master) : slices(slices)
{
    this->v.resize(slice);
    this->master = master;
    this->coil_mixer.reserve(4);
    this->fgs_mixer.reserve(4);
    this->coil_mixer[0] = 1.0;
    this->coil_mixer[1] = 2.0;
    this->coil_mixer[2] = 11.31;
    this->coil_mixer[3] = 22.61;

    this->fgs_mixer[0] = 1.0;
    this->fgs_mixer[1] = 2.0;
    this->fgs_mixer[2] = 1.0/707.1;
    this->fgs_mixer[3] = 2.0/707.1;

}

generator::~generator()
{
    if (this->buffer != nullptr) this->buffer.reset();
    if (this->unique_noise_buffer_1 != nullptr) this->unique_noise_buffer_1.reset();
    if (this->unique_noise_buffer_2 != nullptr) this->unique_noise_buffer_2.reset();
    qDebug() << "generator" << this->channel_no << "deleted";
}

void generator::linear()
{
    size_t j = 0, i = 0;
    for (i = 0; i < (this->slices * this->v.size()); ++i) {

        this->v[j] = this->linear_mV ;
        if (j == (this->v.size() - 1)) {
            if (i < ((this->slices * this->v.size()) - 1)) buffer->deposit(v, threadbuffer_status::running);
            else {
                buffer->deposit(v, threadbuffer_status::finished);
                qDebug() << "seed finished";
                return;
            }
            j = 0;

        }
        else ++j;
    }
}

void generator::sine()
{
    if (!this->fsine_f.size()) return;

    size_t j = 0, i = 0;
    for (i = 0; i < (this->slices * this->v.size()); ++i) {

        this->v[j] = this->fsine_a[0] * std::sin(2.0 * M_PI * double(j) / this->f_sample * this->fsine_f[0]);

        if (this->fsine_f.size() > 1) {
            this->v[j] += this->fsine_a[1] * std::sin(2.0 * M_PI * double(j) / this->f_sample * this->fsine_f[1]);

        }
        if (this->fsine_f.size() > 2) {
            this->v[j] += this->fsine_a[2] * std::sin(2.0 * M_PI * double(j) / this->f_sample * this->fsine_f[2]);

        }

        if (this->fsine_f.size() > 3) {
            this->v[j] += this->fsine_a[3] * std::sin(2.0 * M_PI * double(j) / this->f_sample * this->fsine_f[3]);

        }
        if (j == (this->v.size() - 1)) {
            if (i < ((this->slices * this->v.size()) - 1)) buffer->deposit(v, threadbuffer_status::running);
            else {
                buffer->deposit(v, threadbuffer_status::finished);
                qDebug() << "seed finished";
                return;
            }
            j = 0;

        }
        else ++j;
    }
}

void generator::sawtooth()
{
    size_t j = 0, i = 0;
    double step = (this->upper - this->lower) / double(v.size());
    for (i = 0; i < (this->slices * this->v.size()); ++i) {

        this->v[j] = double(j) * step + lower ;
        if (j == (this->v.size() - 1)) {
            if (i < ((this->slices * this->v.size()) - 1)) buffer->deposit(v, threadbuffer_status::running);
            else {
                buffer->deposit(v, threadbuffer_status::finished);
                qDebug() << "seed finished";
                return;
            }
            j = 0;

        }
        else ++j;
    }
}

void generator::rect()
{
    size_t j = 0, i = 0;
    double sign = 1;
    for (i = 0; i < (this->slices * this->v.size()); ++i) {

        this->v[j] = this->lower * sign ;
        if (j == (this->v.size() - 1)) {
            if (i < ((this->slices * this->v.size()) - 1)) buffer->deposit(v, threadbuffer_status::running);
            else {
                buffer->deposit(v, threadbuffer_status::finished);
                qDebug() << "seed finished";
                return;
            }
            j = 0;
            sign *= -1.0;

        }
        else ++j;
    }
}

void generator::mt19937noise()
{

    std::random_device device;
    std::mt19937 generator(device());
    std::normal_distribution<double> uniform(0, 1);

    size_t j = 0, i = 0;
    for (i = 0; i < (this->slices * this->v.size()); ++i) {

        this->v[j] = uniform(generator);
        if (j == (this->v.size() - 1)) {
            if (i < ((this->slices * this->v.size()) - 1)) buffer->deposit(v, threadbuffer_status::running);
            else {
                buffer->deposit(v, threadbuffer_status::finished);
                qDebug() << "seed finished";
                return;
            }
            j = 0;

        }

        else ++j;

    }


}

void generator::mt_noise_coils()
{
    this->v2.resize(this->v.size());
    size_t i;

    do {
        // fetch the uniform noise for channels
        this->unique_noise_buffer_1->fetch(this->v, this->noise_1_status);
        this->unique_noise_buffer_2->fetch(this->v2, this->noise_2_status);


        // modify depending on channel
        if (this->channel_type == pmt::ex) {
            for (i = 0; i < this->v.size(); ++i) {
                this->v[i] = this->v[i] * this->coil_mixer[0] + this->v2[i] * this->coil_mixer[1];
            }


        }
        else if (this->channel_type == pmt::ey) {
            for (i = 0; i < this->v.size(); ++i) {
                this->v[i] = this->v[i] * this->coil_mixer[1] + this->v2[i] * this->coil_mixer[0];
            }

        }
        else if (this->channel_type == pmt::hx) {
            for (i = 0; i < this->v.size(); ++i) {
                this->v[i] = this->v[i] * this->coil_mixer[3] + this->v2[i] * this->coil_mixer[2];
            }

        }
        else if (this->channel_type == pmt::hy) {
            for (i = 0; i < this->v.size(); ++i) {
                this->v[i] = (this->v[i] * this->coil_mixer[2] + this->v2[i] * this->coil_mixer[3]) * -1.;
            }

        }
        else if (this->channel_type == pmt::hz) {
            for (i = 0; i < this->v.size(); ++i) {
                this->v[i] = (this->v[i] * this->coil_mixer[2] + this->v2[i] * this->coil_mixer[3]) * -1.;
            }

        }



        // forward to the writer
        buffer->deposit(this->v, this->noise_1_status);

        // while with two conditions should work because on thread can not skip the other
    } while ( (this->noise_1_status == threadbuffer_status::running) && (this->noise_2_status == threadbuffer_status::running) );

    // not neeeded anymore
    v2.resize(0);

}

void generator::seed()
{
    if (this->function_name == "straight_100_mV") {
        this->linear();
    }
    else if (this->function_name == "sawtooth_50_to_50_mV") {
        this->sawtooth();
    }

    else if (this->function_name == "one_rectangular_50_to_50_mV") {
        this->rect();
    }
    else if (this->function_name == "mt19937noise") {
        this->mt19937noise();
    }

    else if (this->function_name == "std_mt_noise_coils") {

        this->mt_noise_coils();
    }

    else if (this->function_name == "generate_sinewaves_tab_0") {
        if (!this->fsine_f.size()) return;
        this->sine();
    }


}





void generator::slot_set_function(const QString &function_name)
{
    this->function_name = function_name;
}

void generator::slot_set_slices(const int slices)
{
    this->slices = size_t(slices);
}

void generator::slot_set_slice_wl(const int slice)
{
    this->v.resize(size_t(slice));
}

void generator::slot_set_sines(const double &f_sample, const std::vector<double> &fsine_f, std::vector<double> &fsine_a, std::vector<double> &fsine_p)
{
    this->fsine_f.clear();
    this->fsine_a.clear();
    this->fsine_p.clear();
    auto a = fsine_a.cbegin();
    auto p = fsine_p.cbegin();
    for (const auto &f : fsine_f) {

        if (f > 0 && *a > 0) {
            this->fsine_f.push_back(f);
            this->fsine_a.push_back(*a++);
            this->fsine_p.push_back(*p++);
        }
    }

    this->f_sample = f_sample;
}

void generator::set_threadbuffer(std::shared_ptr<threadbuffer<double> > &buffer)
{
    if (this->buffer != nullptr) this->buffer.reset();
    this->buffer = buffer;
}



void mt_gensign::on_select_slice_length_currentIndexChanged(int index)
{
    if (this->selectwindow == nullptr) return;
    this->wl = size_t(this->selectwindow->current_index_qvariant(index).toULongLong());

    qDebug() << "slice / window length" << this->wl;
    for ( auto &file : atsfiles) {
        file->set_key_value("num_samples", quint64(this->wl * this->slices) );
        file->prc_com_to_classmembers();
        file->set_window_length(this->wl, 0, 0, false, this->wl * this->slices, 0, atsfileout_unscaled_timeseries);
        file->prc_com_to_classmembers();


    }
    for (auto &gener : this->generators) {
        gener->slot_set_slice_wl(int(wl));
    }
    this->eqdt_start.set_num_samples(this->wl * this->slices);
    this->ui->stop_datetime->setDateTime(this->eqdt_start.sample_time(this->wl * this->slices));
}

void mt_gensign::on_fsine_f1_valueChanged(double arg1)
{
    if ((this->ui->checkBox_use_seconds->checkState() == Qt::Checked) && (arg1 != 0.0)) this->fsine_f[0] = 1.0/arg1;
    if ((this->ui->checkBox_use_seconds->checkState() == Qt::Checked) && (arg1 == 0.0)) this->fsine_f[0] = 0.0;
    else this->fsine_f[0] = arg1;
}

void mt_gensign::on_fsine_a1_valueChanged(double arg1)
{
    this->fsine_a[0] = arg1;
}

void mt_gensign::on_fsine_p1_valueChanged(double arg1)
{
    this->fsine_p[0] = double(quint64(std::abs(arg1)));
}

void mt_gensign::on_fsine_f2_valueChanged(double arg1)
{
    if ((this->ui->checkBox_use_seconds->checkState() == Qt::Checked) && (arg1 != 0.0)) this->fsine_f[1] = 1.0/arg1;
    if ((this->ui->checkBox_use_seconds->checkState() == Qt::Checked) && (arg1 == 0.0)) this->fsine_f[1] = 0.0;
    else this->fsine_f[1] = arg1;
}

void mt_gensign::on_fsine_a2_valueChanged(double arg1)
{
    this->fsine_a[1] = arg1;
}

void mt_gensign::on_fsine_p2_valueChanged(double arg1)
{
    this->fsine_p[1] = double(quint64(std::abs(arg1)));
}

void mt_gensign::on_fsine_f3_valueChanged(double arg1)
{
    if ((this->ui->checkBox_use_seconds->checkState() == Qt::Checked) && (arg1 != 0.0)) this->fsine_f[2] = 1.0/arg1;
    if ((this->ui->checkBox_use_seconds->checkState() == Qt::Checked) && (arg1 == 0.0)) this->fsine_f[2] = 0.0;
    else this->fsine_f[2] = arg1;
}

void mt_gensign::on_fsine_a3_valueChanged(double arg1)
{
    this->fsine_a[2] = arg1;
}

void mt_gensign::on_fsine_p3_valueChanged(double arg1)
{
    this->fsine_p[2] = double(quint64(std::abs(arg1)));
}


void mt_gensign::on_fsine_f4_valueChanged(double arg1)
{
    if ((this->ui->checkBox_use_seconds->checkState() == Qt::Checked) && (arg1 != 0.0)) this->fsine_f[3] = 1.0/arg1;
    if ((this->ui->checkBox_use_seconds->checkState() == Qt::Checked) && (arg1 == 0.0)) this->fsine_f[3] = 0.0;
    else this->fsine_f[3] = arg1;
}

void mt_gensign::on_fsine_a4_valueChanged(double arg1)
{
    this->fsine_a[3] = arg1;
}

void mt_gensign::on_fsine_p4_valueChanged(double arg1)
{
    this->fsine_p[3] = double(quint64(std::abs(arg1)));
}

void mt_gensign::on_checkBox_use_seconds_stateChanged(int arg1)
{
    if (arg1 == 0) {
        this->ui->label_1_seconds->setText("frequency");
        this->ui->label_2_seconds->setText("frequency");
        this->ui->label_3_seconds->setText("frequency");
        this->ui->label_4_seconds->setText("frequency");

        if (this->fsine_f[0] != 0.0) this->fsine_f[0] = 1.0/fsine_f[0];
        if (this->fsine_f[1] != 0.0) this->fsine_f[1] = 1.0/fsine_f[1];
        if (this->fsine_f[2] != 0.0) this->fsine_f[2] = 1.0/fsine_f[2];
        if (this->fsine_f[3] != 0.0) this->fsine_f[3] = 1.0/fsine_f[3];

        this->blockSignals(true);
        this->ui->fsine_f1->setValue(this->fsine_f[0]);
        this->ui->fsine_f2->setValue(this->fsine_f[1]);
        this->ui->fsine_f3->setValue(this->fsine_f[2]);
        this->ui->fsine_f4->setValue(this->fsine_f[3]);
        this->blockSignals(false);


    }
    if (arg1 == 2) {
        this->ui->label_1_seconds->setText("seconds  ");
        this->ui->label_2_seconds->setText("seconds  ");
        this->ui->label_3_seconds->setText("seconds  ");
        this->ui->label_4_seconds->setText("seconds  ");

        if (this->fsine_f[0] != 0.0) this->fsine_f[0] = 1.0/fsine_f[0];
        if (this->fsine_f[1] != 0.0) this->fsine_f[1] = 1.0/fsine_f[1];
        if (this->fsine_f[2] != 0.0) this->fsine_f[2] = 1.0/fsine_f[2];
        if (this->fsine_f[3] != 0.0) this->fsine_f[3] = 1.0/fsine_f[3];

        this->blockSignals(true);
        this->ui->fsine_f1->setValue(this->fsine_f[0]);
        this->ui->fsine_f2->setValue(this->fsine_f[1]);
        this->ui->fsine_f3->setValue(this->fsine_f[2]);
        this->ui->fsine_f4->setValue(this->fsine_f[3]);
        this->blockSignals(false);


    }

}
