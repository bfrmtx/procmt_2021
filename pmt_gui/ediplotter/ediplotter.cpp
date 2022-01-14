#include "ediplotter.h"
#include "ui_ediplotter.h"

#include <QFileDialog>

QString const filedialog_edi_filter = "EDI Files (*.edi)";
QString const filedialog_open_edi_title = "Open EDI File...";
QString const suff_tipper = "_tipper";
QString const suff_strike = "_strike";
QString const suff_anisotropy = "_anys";
QString const suff_coherance = "_coh";


void replot_qcp(QCustomPlot * widget) {
    widget->xAxis->rescale(true);
    widget->yAxis->rescale(true);
    widget->replot();
}

void replot_qcp(std::initializer_list<QCustomPlot *> list) {
    for(auto & p : list) replot_qcp(p);
}

ediplotter::ediplotter(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ediplotter)
{
    ui->setupUi(this);
    this->setLocale(QLocale::c());

    ui->frame_optional->setVisible(false);
    this->setAcceptDrops(true);

    // take colors from old ediplotter - avoid questions
    this->ui->button_rho_1->setStyleSheet("* { background-color: rgb(255,125,100) }");
    this->ui->button_rho_2->setStyleSheet("* { background-color: rgb(255,125,100) }");

    this->ui->button_rho_3->setStyleSheet("* { background-color: rgb(112,136,26) }");
    this->ui->button_rho_4->setStyleSheet("* { background-color: rgb(112,136,26) }");
    this->ui->button_rho_5->setStyleSheet("* { background-color: rgb(112,136,26) }");


    this->ui->button_rho_6->setStyleSheet("* { background-color: rgb(194,187,87) }");
    this->ui->button_rho_7->setStyleSheet("* { background-color: rgb(194,187,87) }");
    this->ui->button_rho_8->setStyleSheet("* { background-color: rgb(194,187,87) }");

    this->ui->button_rho_9->setStyleSheet("* { background-color: rgb(126,175,194) }");
    this->ui->button_rho_10->setStyleSheet("* { background-color: rgb(126,175,194) }");
    this->ui->button_rho_11->setStyleSheet("* { background-color: rgb(126,175,194) }");

    this->ui->button_rho_12->setStyleSheet("* { background-color: rgb(25,96,210) }");

    this->ui->dial_active_plot->setNotchTarget(3.7);
    this->ui->dial_active_plot->setNotchesVisible(true);
    this->ui->dial_active_plot->setRange(0,0);



    set_phi_range(0, 90);
    set_rho_range(0.1, 100);

    connect(this, &ediplotter::all_edi_files_removed, ui->widget_plot_options, &PlotOptions::slot_on_all_edi_plots_removed);
    connect(this, &ediplotter::new_edi_plot_added, ui->widget_plot_options, &PlotOptions::slot_on_new_edi_plot);
    connect(ui->widget_plot_options, &PlotOptions::remove_plot, this, &ediplotter::remove_edi_file);
    connect(ui->widget_plot_options, &PlotOptions::plot_visiblity_change, this, &ediplotter::change_edi_visibility);

    this->ui->button_actions_toggle_map_phase->setCheckable(true);
    this->ui->button_actions_toggle_map_phase->setChecked(true);
    this->phase_is_mapped = true;

    QCommonStyle style;
    ui->button_sidemenu->setIcon(style.standardIcon(QStyle::SP_ArrowRight));
    connect(ui->button_sidemenu, &QAbstractButton::clicked, this, [this] {
        QCommonStyle style;
        this->m_sidemenu_visible = !this->m_sidemenu_visible;
        this->ui->frame_optional->setVisible(m_sidemenu_visible);
        this->ui->button_sidemenu->setIcon(
            style.standardIcon(m_sidemenu_visible ? QStyle::SP_ArrowLeft : QStyle::SP_ArrowRight));
    });

    connect(this->ui->tab_xyyx, &DualPlot::signal_set_x_axis_label, this, &ediplotter::slot_set_x_axis_label);
    connect(this->ui->tab_xyyx, &DualPlot::signal_set_y_axis_label, this, &ediplotter::slot_set_y_axis_label);

    // new set graph number for cross hair cursor

    connect(this->ui->dial_active_plot, &QDial::valueChanged, this->ui->tab_xyyx, &DualPlot::slot_line_for_crosshair);
    connect(this->ui->dial_active_plot, &QDial::valueChanged, this->ui->tab_xxyy, &DualPlot::slot_line_for_crosshair);

}

ediplotter::~ediplotter() {
    delete ui;
}

void ediplotter::set_msg_logger(std::shared_ptr<msg_logger> msg) {
    this->msg = msg;
}

void ediplotter::dragEnterEvent(QDragEnterEvent *event) {
    bool accept = true;
    if(event->mimeData()->hasUrls()) {
        for(auto url : event->mimeData()->urls()) {
            auto localFileName = url.toLocalFile();
            if(QFile::exists(localFileName)) {
                auto suffix = QFileInfo(localFileName).suffix();
                if(suffix != "edi") {
                    accept = false;
                }
            } else {
                accept = false;
            }
        }
    } else {
        accept = false;
    }
    if(accept)
        event->acceptProposedAction();
}

void ediplotter::dropEvent(QDropEvent *event) {
    for(auto url : event->mimeData()->urls()) {
        auto localFileName = url.toLocalFile();
        open_edi_file(localFileName);
    }
}

void ediplotter::slot_set_x_axis_label(const QString &label_x_axis)
{
    this->ui->label_x_axis->setText(label_x_axis);
}

void ediplotter::slot_set_y_axis_label(const QString &label_x_axis)
{
    this->ui->label_y_axis->setText(label_x_axis);
}


void ediplotter::on_action_quit_triggered() {
    this->close();
}

void ediplotter::on_action_open_triggered() {
    QString filename = QFileDialog::getOpenFileName(nullptr, filedialog_open_edi_title, QString(), filedialog_edi_filter);
    if(!filename.isNull() && !filename.isEmpty()) {
        open_edi_file(filename);
    }
}

void ediplotter::open_edi_file(const QString & filename) {
    QFileInfo fileinfo(filename);
    auto new_edi_file = new edi_file(this);
    new_edi_file->set_QFileInfo(fileinfo);
    auto edi_name = fileinfo.fileName();

    if(m_edi_files.contains(edi_name)) {
        qDebug() << "TODO: IMPLEMENT what if edi file is already contained in map?";
    } else {
        switch(m_open_mode) {
        case EdiPlotterOpenMode::ADD: {

            break;
        }
        case EdiPlotterOpenMode::REPLACE: {
            remove_all_edi_files();
            m_edi_files.insert(fileinfo.fileName(), new_edi_file);
            break;
        }
        case EdiPlotterOpenMode::OVERLAY: {
            m_edi_files.insert(fileinfo.fileName(), new_edi_file);
            break;
        }
        }

        new_edi_file->read();
        if (new_edi_file->z.test_tensor()) {
            new_edi_file->z.remove_fake_errors();
            new_edi_file->z.backup();               // keep copy in case after rotation etc we go back to the originals
            add_edi_data(edi_name, new_edi_file->z);
            this->ui->dial_active_plot->setRange(0, (this->m_edi_files.size() * 2) - 1);
            emit new_edi_plot_added(edi_name);
        }

    }
}

void ediplotter::set_rho_range(double min, double max) {
    ui->tab_xxyy->set_rho_range(min, max);
    ui->tab_xyyx->set_rho_range(min, max);
}

void ediplotter::set_phi_range(double min, double max) {
    ui->tab_xxyy->set_phi_range(min, max);
    ui->tab_xyyx->set_phi_range(min, max);
}

void ediplotter::add_edi_data(QString const & name, mt_data_res<std::complex<double>> const & data) {
    ui->tab_xxyy->add_plots(name, data, tns::xx, tns::yy);
    ui->tab_xyyx->add_plots(name, data, tns::xy, tns::yx);
    add_coh_plot(name, data);
    add_anisotropy_plot(name, data);
    add_tipper_plot(name, data);
    add_strike_plot(name, data);
}

// TODO: simplify this 4 methods (prevent boilerplate code)
void ediplotter::add_coh_plot(QString const & name, const mt_data_res<std::complex<double>> & data) {
    auto new_graph = ui->tab_coh->addGraph();
    for(size_t i = 0; i < data.freqs.size(); ++i) {
        auto const & f = data.freqs[i];
        auto const & y = data.coherency(tns::xxxy, i);
        new_graph->addData(f, y);
        new_graph->setPen(QPen(Qt::blue));
        new_graph->setScatterStyle(QCPScatterStyle::ssSquare);
    }
    replot_qcp(ui->tab_coh);
    m_graphs[name].push_back(new_graph);
}

void ediplotter::add_anisotropy_plot(QString const & name, const mt_data_res<std::complex<double>> & data) {
    auto new_graph = ui->tab_anisotropy->addGraph();
    for(size_t index = 0; index < data.freqs.size(); ++index) {
        auto const & f = data.freqs[index];
        auto const & y = data.anisotropy(index);
        new_graph->addData(f, y);
        new_graph->setPen(QPen(Qt::blue));
        new_graph->setScatterStyle(QCPScatterStyle::ssSquare);
    }
    replot_qcp(ui->tab_anisotropy);
    m_graphs[name].push_back(new_graph);
}

void ediplotter::add_tipper_plot(QString const & name, const mt_data_res<std::complex<double>> & data) {
    auto minx = std::numeric_limits<double>::max();
    auto maxx = std::numeric_limits<double>::lowest();

    for(size_t index = 0; index < data.freqs.size(); ++index) {
        auto const & f = std::log(data.freqs[index]);

        minx = std::min(f, minx);
        maxx = std::max(f, maxx);

        {
            auto d1 = data.d[tns::tx][index];
            QCPItemLine * arrow = new QCPItemLine(ui->tab_tipper);
//            QCPItemEllipse * ellipse = new QCPItemEllipse(ui->tab_tipper);
//            {
//                auto rect = new QCPItemRect(ui->tab_tipper);
//                rect->set
//            }
            arrow->setPen(QPen(Qt::red));
            arrow->start->setCoords(f, 0);
            auto p = std::arg(d1);
            arrow->end->setCoords(f + std::cos(p), std::sin(p));
            arrow->setHead(QCPLineEnding::esSpikeArrow);
            arrow->setVisible(true);
        }
        {
            auto d2 = data.d[tns::ty][index];
            QCPItemLine * arrow = new QCPItemLine(ui->tab_tipper);
            arrow->setPen(QPen(Qt::blue));
            arrow->start->setCoords(f, 0);
            auto p = std::arg(d2);
            arrow->end->setCoords(f + std::cos(p), std::sin(p));
            arrow->setHead(QCPLineEnding::esSpikeArrow);
            arrow->setVisible(true);
        }
    }

    ui->tab_tipper->xAxis->setRange(minx, maxx);
    ui->tab_tipper->yAxis->setRange(-5, 5);
   // arrow->end->setCoords(4, 1.6); // point to (4, 1.6) in x-y-plot coordinates
}

void ediplotter::add_strike_plot(QString const & name, const mt_data_res<std::complex<double>> & data) {
    auto new_graph = ui->tab_strike->addGraph();
    for(size_t index = 0; index < data.freqs.size(); ++index) {
        auto const & f = data.freqs[index];
        auto const & y = data.strike(index);
        new_graph->addData(f, y);
        new_graph->setPen(QPen(Qt::blue));
        new_graph->setScatterStyle(QCPScatterStyle::ssSquare);
    }
    replot_qcp(ui->tab_strike);
    m_graphs[name].push_back(new_graph);

}






void ediplotter::remove_edi_file(const QString & name) {
    if(m_edi_files.contains(name)) {
        m_edi_files[name]->deleteLater();
        m_edi_files.remove(name);
        this->ui->dial_active_plot->setRange(0, (this->m_edi_files.size() * 2) - 1);

    }
    ui->tab_xxyy->remove_plots(name);
    ui->tab_xyyx->remove_plots(name);
    for(auto & graph : m_graphs[name]) {
        graph->parentPlot()->removeGraph(graph);
    }
    m_graphs.remove(name);
    replot_qcp({ui->tab_anisotropy, ui->tab_coh, ui->tab_strike});
}

void ediplotter::remove_all_edi_files() {
    for(auto file : m_edi_files) {
        file->deleteLater();
    }
    m_edi_files.clear();
    ui->tab_xxyy->remove_all_plots();
    ui->tab_xyyx->remove_all_plots();
    for(auto & vec : m_graphs) {
        for(auto & graph : vec) {
            graph->parentPlot()->removeGraph(graph);
        }
    }
    m_graphs.clear();
    replot_qcp({ui->tab_anisotropy, ui->tab_coh, ui->tab_strike});
    this->ui->dial_active_plot->setRange(0, 0);    
    emit all_edi_files_removed();
}

void ediplotter::change_edi_visibility(const QString & name, bool value) {
    ui->tab_xxyy->set_plots_visible(name, value);
    ui->tab_xyyx->set_plots_visible(name, value);
    for(auto & graph : m_graphs[name]) {
        graph->setVisible(value);
    }
    replot_qcp({ui->tab_anisotropy, ui->tab_coh, ui->tab_strike});
}

void ediplotter::phi_button_clicked() {
    auto senderBtn = qobject_cast<QPushButton*>(sender());
    if(senderBtn != nullptr) {
        auto min = senderBtn->property("min").toDouble();
        auto max = senderBtn->property("max").toDouble();
        set_phi_range(min, max);
    }
}

void ediplotter::rho_button_clicked() {
    auto senderBtn = qobject_cast<QPushButton*>(sender());
    if(senderBtn != nullptr) {
        auto min = senderBtn->property("min").toDouble();
        auto max = senderBtn->property("max").toDouble();
        set_rho_range(min, max);
    }
}

void ediplotter::on_button_group_open_mode_buttonClicked(QAbstractButton * button) {
    if(button == ui->button_actions_openmode_add) {
        m_open_mode = EdiPlotterOpenMode::ADD;
    } else if(button == ui->button_actions_openmode_overlay) {
        m_open_mode = EdiPlotterOpenMode::OVERLAY;
    } else if(button == ui->button_actions_openmode_replace) {
        m_open_mode = EdiPlotterOpenMode::REPLACE;
    }
}

void ediplotter::on_button_phi_fit_clicked() {
    ui->tab_xxyy->fit_phi_ranges();
    ui->tab_xyyx->fit_phi_ranges();
}

void ediplotter::on_button_rho_fit_clicked() {
    ui->tab_xxyy->fit_rho_ranges();
    ui->tab_xyyx->fit_rho_ranges();
}


void ediplotter::on_button_actions_toggle_map_phase_clicked(bool checked)
{

    if (this->ui->button_actions_toggle_map_phase->isChecked()) this->phase_is_mapped = true;
    else this->phase_is_mapped = false;

    emit this->map_phase(this->phase_is_mapped);


}

void ediplotter::on_dial_active_plot_valueChanged(int value)
{
    this->ui->label_line_plot->setText("line: " + QString::number(value));
}

void ediplotter::on_action_export_ascii_triggered()
{
    // QMap<QString, edi_file *> m_edi_files;
    QMap<QString, edi_file*>::iterator edi = m_edi_files.begin();
     while (edi != m_edi_files.constEnd()) {

        QFileInfo qfi(edi.value()->absolutePath() + "/" + edi.value()->completeBaseName() + ".dat");
        edi.value()->toASCII_file(qfi);
        ++edi;

     }

}
