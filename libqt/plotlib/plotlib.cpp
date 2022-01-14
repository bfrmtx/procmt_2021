#include "plotlib.h"


plotlib::plotlib(const QString& object_name, QWidget *parent) : QCustomPlot(parent)
{
    this->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
    if (object_name.size()) this->setObjectName(object_name);
//#ifdef QCUSTOMPLOT_USE_OPENGL
//    this->setOpenGl(true, 16);
//    if(this->openGl()) qDebug() << "plotlib openGL ON";
//#endif

}

plotlib::~plotlib()
{

    this->signalMapper_fon_ranges.reset();
    this->signalMapper_foff_ranges.reset();
    this->signalMapper_f_ranges.reset();

    this->delete_pushbuttons(rho_ranges);
    this->delete_pushbuttons(phi_ranges);
    this->delete_pushbuttons(foff_ranges);
    this->delete_pushbuttons(f_ranges);

    this->fon_ranges_area.reset();
    this->foff_ranges_area.reset();
    this->f_ranges_area.reset();


    this->rho_ranges_box.reset();
    this->phi_ranges_box.reset();


    qDebug() << "plotlib destroyed ";

    //this->QCustomPlot::~QCustomPlot();

}

void plotlib::set_ScaleType_log_xy(const QString leftaxislabel)
{
    this->addGraph(this->xAxis, this->yAxis);
    this->yAxis->setNumberFormat("eb");
    this->xAxis->setNumberFormat("eb");
    this->xAxis->setNumberPrecision(0);
    this->yAxis->setNumberPrecision(0);
    this->yAxis->grid()->setSubGridVisible(true);
    this->xAxis->grid()->setSubGridVisible(true);
    this->yAxis->setScaleType(QCPAxis::stLogarithmic);
    this->xAxis->setScaleType(QCPAxis::stLogarithmic);

    if (this->logTicker == nullptr)
        this->logTicker = QSharedPointer<QCPAxisTickerLog>(new QCPAxisTickerLog);
    this->yAxis->setTicker(logTicker);
    this->xAxis->setTicker(logTicker);

    if (leftaxislabel.size()) this->yAxis->setLabel(leftaxislabel);
}

void plotlib::set_ScaleType_log_x_lin_y(const QString leftaxislabel)
{
    this->addGraph(this->xAxis, this->yAxis);
    this->yAxis->setNumberFormat("f");
    this->xAxis->setNumberFormat("eb");
    this->xAxis->setNumberPrecision(0);
    this->yAxis->setNumberPrecision(1);
    this->yAxis->grid()->setSubGridVisible(true);
    this->xAxis->grid()->setSubGridVisible(true);
    this->yAxis->setScaleType(QCPAxis::stLinear);
    this->xAxis->setScaleType(QCPAxis::stLogarithmic);

    if (this->logTicker == nullptr)
        this->logTicker = QSharedPointer<QCPAxisTickerLog>(new QCPAxisTickerLog);
    this->xAxis->setTicker(logTicker);

    if (leftaxislabel.size()) this->yAxis->setLabel(leftaxislabel);

}

void plotlib::set_ScaleType_xy(const QString leftaxislabel)
{
    this->addGraph(this->xAxis, this->yAxis);
    this->yAxis->setNumberFormat("f");
    this->xAxis->setNumberFormat("f");
    this->xAxis->setNumberPrecision(1);
    this->yAxis->setNumberPrecision(1);
    this->yAxis->grid()->setSubGridVisible(true);
    this->xAxis->grid()->setSubGridVisible(true);
    this->yAxis->setScaleType(QCPAxis::stLinear);
    this->xAxis->setScaleType(QCPAxis::stLinear);

    if (leftaxislabel.size()) this->yAxis->setLabel(leftaxislabel);
}

void plotlib::add_right_logx_liny(const QString rightaxislabel)
{
    this->addGraph(this->xAxis, this->yAxis2);
    this->yAxis2->setNumberFormat("f");
    this->yAxis2->setNumberPrecision(0);
    this->yAxis2->setScaleType(QCPAxis::stLinear);
    if (rightaxislabel.size()) this->yAxis2->setLabel(rightaxislabel);

    this->yAxis2->setVisible(true);


}

void plotlib::clear_all_data()
{
    int i;
    for (i = 0; i < this->graphCount(); ++i) {
        this->graph(i)->data()->clear();
    }
}

void plotlib::clear_downto(const int i)
{
    this->clear_all_data();
    int j;
    for (j = this->graphCount() -1; j >= i; --j) {
        this->removeGraph(j);
    }
}

void plotlib::add_hz_line(const double &y, const double &x_start, const double &x_stop, const QPen &pen)
{
    auto ng = this->addGraph();
    ng->setPen(pen);
    ng->setLineStyle((QCPGraph::LineStyle)1);
    ng->addData(x_start, y);
    ng->addData(x_stop, y);
}


QSharedPointer<QGroupBox> plotlib::create_rho_ranges()
{
    this->rho_ranges_box = QSharedPointer<QGroupBox>(new QGroupBox("rho-ranges", this));
    QVBoxLayout *vbox = new QVBoxLayout(this->rho_ranges_box.data());

    this->signalMapper_rho_ranges = QSharedPointer<QSignalMapper>(new QSignalMapper());

    this->rho_ranges.append(new QPushButton("0.1 - 10", this->rho_ranges_box.data()));
    this->rho_ranges.last()->setStyleSheet("* { background-color: rgb(255,125,100) }");

    this->rho_ranges.append(new QPushButton("0.1 - 100", this->rho_ranges_box.data()));
    this->rho_ranges.last()->setStyleSheet("* { background-color: rgb(255,125,100) }");

    this->rho_ranges.append(new QPushButton("1 - 10", this->rho_ranges_box.data()));
    this->rho_ranges.last()->setStyleSheet("* { background-color: rgb(112,136,26) }");

    this->rho_ranges.append(new QPushButton("1 - 100", this->rho_ranges_box.data()));
    this->rho_ranges.last()->setStyleSheet("* { background-color: rgb(112,136,26) }");

    this->rho_ranges.append(new QPushButton("1 - 1000", this->rho_ranges_box.data()));
    this->rho_ranges.last()->setStyleSheet("* { background-color: rgb(112,136,26) }");

    this->rho_ranges.append(new QPushButton("1 - 1E4", this->rho_ranges_box.data()));
    this->rho_ranges.last()->setStyleSheet("* { background-color: rgb(112,136,26) }");

    this->rho_ranges.append(new QPushButton("10 - 100", this->rho_ranges_box.data()));
    this->rho_ranges.last()->setStyleSheet("* { background-color: rgb(194,187,87) }");

    this->rho_ranges.append(new QPushButton("10 - 1000", this->rho_ranges_box.data()));
    this->rho_ranges.last()->setStyleSheet("* { background-color: rgb(194,187,87) }");

    this->rho_ranges.append(new QPushButton("10 - 1E4", this->rho_ranges_box.data()));
    this->rho_ranges.last()->setStyleSheet("* { background-color: rgb(194,187,87) }");

    this->rho_ranges.append(new QPushButton("100 - 1000", this->rho_ranges_box.data()));
    this->rho_ranges.last()->setStyleSheet("* { background-color: rgb(126,175,194) }");

    this->rho_ranges.append(new QPushButton("100 - 1E4", this->rho_ranges_box.data()));
    this->rho_ranges.last()->setStyleSheet("* { background-color: rgb(126,175,194) }");

    this->rho_ranges.append(new QPushButton("100 - 1E5", this->rho_ranges_box.data()));
    this->rho_ranges.last()->setStyleSheet("* { background-color: rgb(126,175,194) }");

    this->rho_ranges.append(new QPushButton("1000 - 1E6", this->rho_ranges_box.data()));
    this->rho_ranges.last()->setStyleSheet("* { background-color: rgb(25,96,210) }");



    for (auto &btn : this->rho_ranges) {
        vbox->addWidget(btn);
        connect(btn, SIGNAL(clicked()), this->signalMapper_rho_ranges.data(), SLOT(map()));
        this->signalMapper_rho_ranges->setMapping(btn, btn->text());
    }

    connect(this->signalMapper_rho_ranges.data(), SIGNAL(mapped(QString)),
            this, SIGNAL(signalMapper_rho_ranges_clicked(QString)));

    connect(this, &plotlib::signalMapper_rho_ranges_clicked, this, &plotlib::rho_range_clicked);

    vbox->addStretch(1);
    this->rho_ranges_box ->setLayout(vbox);

    return this->rho_ranges_box;


}

QSharedPointer<QGroupBox> plotlib::create_phi_ranges()
{

    this->phi_ranges_box = QSharedPointer<QGroupBox>(new QGroupBox("phi-ranges", this));
    QVBoxLayout *vbox = new QVBoxLayout(this->phi_ranges_box.data());

    this->signalMapper_phi_ranges = QSharedPointer<QSignalMapper>(new QSignalMapper());


    this->phi_ranges.append(new QPushButton("0 - 90", this->phi_ranges_box.data()));

    this->phi_ranges.append(new QPushButton("0 - 60", this->phi_ranges_box.data()));

    this->phi_ranges.append(new QPushButton("10 - 80", this->phi_ranges_box.data()));

    this->phi_ranges.append(new QPushButton("20 - 90", this->phi_ranges_box.data()));

    this->phi_ranges.append(new QPushButton("20 - 70", this->phi_ranges_box.data()));

    this->phi_ranges.append(new QPushButton("30 - 60", this->phi_ranges_box.data()));

    this->phi_ranges.append(new QPushButton("-180 - 180", this->phi_ranges_box.data()));



    for (auto &btn : this->phi_ranges) {
        vbox->addWidget(btn);
        connect(btn, SIGNAL(clicked()), this->signalMapper_phi_ranges.data(), SLOT(map()));
        this->signalMapper_phi_ranges->setMapping(btn, btn->text());
    }

    connect(this->signalMapper_phi_ranges.data(), SIGNAL(mapped(QString)),
            this, SIGNAL(signalMapper_phi_ranges_clicked(QString)));

    connect(this, &plotlib::signalMapper_phi_ranges_clicked, this, &plotlib::phi_range_clicked);

    vbox->addStretch(1);
    this->phi_ranges_box ->setLayout(vbox);

    return this->phi_ranges_box;


}

QSharedPointer<QScrollArea> plotlib::create_fon_ranges(std::vector<double> &vf, const int iwidth)
{
    this->fon_ranges_area = QSharedPointer<QScrollArea>(new QScrollArea(this));
    this->fon_ranges_area->setWidgetResizable(true);
    this->fon_ranges_area->setMaximumWidth(iwidth);
    QWidget *inner = new QWidget(this->fon_ranges_area.data());
    this->fon_ranges_area->setWidget(inner);
    QVBoxLayout *vbox = new QVBoxLayout(inner);
    //vbox->addStretch(1);
    inner->setLayout(vbox);

    this->signalMapper_fon_ranges = QSharedPointer<QSignalMapper>(new QSignalMapper());

    this->f_on = vf;

    for (auto &d : vf) {
        this->fon_ranges.append(new QPushButton(QString::number(d,'E', 6), this->fon_ranges_area.data()));
    }

    int i = 0;
    for (auto &btn : this->fon_ranges) {
        vbox->addWidget(btn);
        connect(btn, SIGNAL(clicked()), this->signalMapper_fon_ranges.data(), SLOT(map()));
        this->signalMapper_fon_ranges->setMapping(btn, i++);
    }

    connect(this->signalMapper_fon_ranges.data(), SIGNAL(mapped(int)),
            this, SIGNAL(signalMapper_fon_ranges_clicked(int)));

    connect(this, &plotlib::signalMapper_fon_ranges_clicked, this, &plotlib::f_on_range_clicked);

    return this->fon_ranges_area;

}

QSharedPointer<QScrollArea> plotlib::create_foff_ranges(std::vector<double> &vf, const int iwidth)
{
    this->foff_ranges_area = QSharedPointer<QScrollArea>(new QScrollArea(this));
    this->foff_ranges_area->setWidgetResizable(true);
    this->foff_ranges_area->setMaximumWidth(iwidth);
    QWidget *inner = new QWidget(this->foff_ranges_area.data());
    this->foff_ranges_area->setWidget(inner);
    QVBoxLayout *vbox = new QVBoxLayout(inner);
    //vbox->addStretch(1);
    inner->setLayout(vbox);

    this->signalMapper_foff_ranges = QSharedPointer<QSignalMapper>(new QSignalMapper());

    this->f_off = vf;

    for (auto &d : vf) {
        this->foff_ranges.append(new QPushButton(QString::number(d,'E', 6), this->foff_ranges_area.data()));
    }

    int i = 0;
    for (auto &btn : this->foff_ranges) {
        vbox->addWidget(btn);
        connect(btn, SIGNAL(clicked()), this->signalMapper_foff_ranges.data(), SLOT(map()));
        this->signalMapper_foff_ranges->setMapping(btn, i++);
    }

    connect(this->signalMapper_foff_ranges.data(), SIGNAL(mapped(int)),
            this, SIGNAL(signalMapper_foff_ranges_clicked(int)));

    connect(this, &plotlib::signalMapper_foff_ranges_clicked, this, &plotlib::f_off_range_clicked);

    return this->foff_ranges_area;

}

QSharedPointer<QScrollArea> plotlib::create_f_ranges(std::vector<double> &vf, const int iwidth)
{
    this->f_ranges_area = QSharedPointer<QScrollArea>(new QScrollArea(this));
    this->f_ranges_area->setWidgetResizable(true);
    this->f_ranges_area->setMaximumWidth(iwidth);
    QWidget *inner = new QWidget(this->f_ranges_area.data());
    this->f_ranges_area->setWidget(inner);
    QVBoxLayout *vbox = new QVBoxLayout(inner);
    //vbox->addStretch(1);
    inner->setLayout(vbox);

    this->signalMapper_f_ranges = QSharedPointer<QSignalMapper>(new QSignalMapper());
    this->f = vf;

    for (auto &d : vf) {
        this->f_ranges.append(new QPushButton(QString::number(d,'E', 6), this->f_ranges_area.data()));
    }

    int i = 0;
    for (auto &btn : this->f_ranges) {
        vbox->addWidget(btn);
        connect(btn, SIGNAL(clicked()), this->signalMapper_f_ranges.data(), SLOT(map()));
        this->signalMapper_f_ranges->setMapping(btn, i++);
    }

    connect(this->signalMapper_f_ranges.data(), SIGNAL(mapped(int)),
            this, SIGNAL(signalMapper_f_ranges_clicked(int)));

    connect(this, &plotlib::signalMapper_f_ranges_clicked, this, &plotlib::f_range_clicked);

    return this->f_ranges_area;

}

void plotlib::xmax_xmin(const std::vector<double> &x)
{
    this->xmin = this->xmax = 0;
    if (!x.size()) return;

    auto result = std::minmax_element (x.begin(),x.end());
    this->xmin = *result.first;
    this->xmax = *result.second;
}

void plotlib::ymax_ymin(const std::vector<double> &y)
{
    this->ymin = this->ymax = 0;
    if (!y.size()) return;

    auto result = std::minmax_element (y.begin(),y.end());
    this->ymin = *result.first;
    this->ymax = *result.second;
}

void plotlib::std_regression_plot(const QString what, const std::vector<double> &x, const std::vector<double> &y, const std::vector<bool> &selected, const bool alreadySorted)
{
    auto ng = this->addGraph();

    if (!selected.size()) {
        ng->setData(x,y, alreadySorted);
    }
    else {
        ng->setData_if(x, y, selected, alreadySorted);
    }
    // plot blue data points
    if (!what.compare("data")) {
        ng->setPen(QPen(Qt::blue));
        ng->setLineStyle((QCPGraph::LineStyle)0);
        ng->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 15));
    }
    if (!what.compare("center")) {

    }
    if (!what.compare("slope")) {
        ng->setPen(QPen(Qt::red));
        ng->setLineStyle((QCPGraph::LineStyle)1);
    }
    if (!what.compare("lower_upper_slope")) {
        ng->setPen(QPen(Qt::green));
        ng->setLineStyle((QCPGraph::LineStyle)1);
    }

    if (!what.compare("lower_upper_confidence")) {
        ng->setPen(QPen(Qt::blue));
        ng->setLineStyle((QCPGraph::LineStyle)1);
    }

}

void plotlib::std_regression_plot(const QString what, const double &x, const double &y)
{
    auto ng = this->addGraph();
    ng->addData(x, y);

    if (!what.compare("means")) {
        ng->setPen(QPen(Qt::black));
        ng->setLineStyle((QCPGraph::LineStyle)0);
        ng->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, 15));
    }

}

void plotlib::title(const QString text)
{
    this->plotLayout()->insertRow(0);
    this->plotLayout()->addElement(0, 0, new QCPTextElement(this, text, QFont("sans", 12, QFont::Light)));
}

void plotlib::subtitle(const QString text)
{
    this->plotLayout()->insertRow(1);
    this->plotLayout()->addElement(1, 0, new QCPTextElement(this, text, QFont("sans", 10, QFont::Light)));
}

void plotlib::set_linticks(const QString axisname, const double factors)
{
    if (this->fixedTicker == nullptr) {
        this->fixedTicker = QSharedPointer<QCPAxisTickerFixed>(new QCPAxisTickerFixed());
    }
    this->fixedTicker->setTickStep(factors);
    this->fixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples);

    if (!axisname.compare("yAxis2")) {
        this->yAxis2->setTicker(this->fixedTicker);
    }
    else if (!axisname.compare("yAxis")) {
        this->yAxis->setTicker(this->fixedTicker);
    }
    else if (!axisname.compare("xAxis")) {
        this->xAxis->setTicker(this->fixedTicker);
    }
    else if (!axisname.compare("xAxis2")) {
        this->xAxis2->setTicker(this->fixedTicker);
    }



}

void plotlib::set_chopper(const int chopper)
{
    this->chopper = chopper;
}

int plotlib::get_chopper() const
{
    return this->chopper;
}

void plotlib::rho_range_clicked(const QString &str)
{
    QStringList split = str.split(" ");

    qDebug() << "clicked" << str << split.at(0).toDouble() << split.at(2).toDouble();
}

void plotlib::phi_range_clicked(const QString &str)
{
    QStringList split = str.split(" ");

    qDebug() << "clicked" << str << split.at(0).toDouble() << split.at(2).toDouble();
}

void plotlib::f_on_range_clicked(const int &f)
{
    emit this->f_on_selected(this->f_on.at(f));

}

void plotlib::f_off_range_clicked(const int &f)
{
    emit this->f_off_selected(this->f_off.at(f));

}

void plotlib::f_range_clicked(const int &f)
{
    emit this->f_selected(this->f.at(f));

}

void plotlib::rescale_xaxis(const double &xmin, const double &xmax)
{
    this->xmin = this->xAxis->range().lower;
}

void plotlib::mousePressEvent(QMouseEvent *event)
{
    QCustomPlot::mousePressEvent(event);
    if(event->button() == Qt::RightButton) {
        double x = this->xAxis->pixelToCoord(event->pos().x());
        double y = this->yAxis->pixelToCoord(event->pos().y());
        emit this->signal_x_y_pos(x, y);
    }
}

//void plotlib::mouseReleaseEvent(QMouseEvent *event)
//{
//    if(event->button() == Qt::RightButton) {
//        emit this->signal_x_y_pos(DBL_MAX, DBL_MAX);
//    }
//}

void plotlib::delete_pushbuttons(QList<QPushButton *> &pbl)
{
    if (!pbl.size()) return;
    for ( QList<QPushButton *>::reverse_iterator iter = pbl.rbegin(); iter != pbl.rend(); ++iter  ) {

        if (*iter != Q_NULLPTR) delete *iter;
    }

    pbl.clear();
}

