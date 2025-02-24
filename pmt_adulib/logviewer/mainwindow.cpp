#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  this->create_plot();

  this->ui->vl_plot->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

  this->ui->dial_from->setValue(this->dial_from_pos);
  this->ui->dial_to->setValue(this->dial_to_pos);

  ui->vl_selbtns->addStretch();
  this->ui->label_temperature->setStyleSheet("QLabel { background-color : red; color : black; }");
  this->ui->label_gps->setStyleSheet("QLabel { background-color : royalBlue; color : black; }");
  this->ui->label_Batt_1->setStyleSheet("QLabel { background-color : #00FF00; color : black; }");
  this->ui->label_Batt_2->setStyleSheet("QLabel { background-color : green; color : black; }");
  this->ui->label_volt->setStyleSheet("QLabel { background-color : olive; color : black; }");
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
  event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event) {

  QList<QUrl> urls = event->mimeData()->urls();
  if (urls.isEmpty())
    return;

  QFileInfo dbfile(urls.at(0).toLocalFile());
  if (!dbfile.exists())
    return;

  if (this->db != nullptr) {
    if (this->db->isOpen())
      this->db->close();
    this->db.reset();
  }

  this->tokens << "num_sats" << "batt1_curr" << "batt2_curr" << "batt_volt" << "temp";

  this->dbname = dbfile.baseName();

  this->db = std::make_unique<QSqlDatabase>(QSqlDatabase::addDatabase("QSQLITE", "MCP_DB"));
  this->db->setConnectOptions("QSQLITE_OPEN_READONLY");
  this->db->setDatabaseName(dbfile.absoluteFilePath());

  // get max min date

  if (this->db->open()) {
    QSqlQuery query(*this->db.get());

    //        if( query.exec("select id,  timestamp, priority, component, mainindex,  subindex, message  from error_log;") ) {
    //            while (query.next()) {
    ////                qDebug() << query.record().count() << query.value(1).toString();
    //            }
    //        }
    //        query.clear();

    if (query.exec("select id,  timestamp, priority, component, mainindex,  subindex, message  from log;")) {
      this->reserves(10000);
      while (query.next()) {
        this->read_message(query);
      }
    }
  }

  if (!this->all_datetimes.size())
    return;
  this->min_max_time();

  // make it fixed baby - no comfort
  this->add_gps();
  this->add_temperature();
  this->add_batt_volt();
  this->add_batt1_curr();
  this->add_batt2_curr();

  customPlot->replot();
}

void MainWindow::add_gps() {
  if (this->customPlot == nullptr)
    return;
  if (!this->num_sats_sats.size())
    return;
  if (this->num_sats_sats.size() != this->num_sats_datetimes.size())
    return;
  this->customPlot->addGraph();
  QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
  dateTicker->setDateTimeFormat("hh:mm \nMMM dd \nyyyy");
  customPlot->xAxis->setTicker(dateTicker);
  this->customPlot->graph(this->nplot)->setData(this->num_sats_datetimes, this->num_sats_sats);
  this->customPlot->graph(this->nplot)->setPen((QPen(Qt::blue, 2)));

  this->customPlot->graph(this->nplot)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCrossCircle, 12));
  this->nplot++;
}

void MainWindow::add_temperature() {
  if (this->customPlot == nullptr)
    return;
  if (!this->temperatures.size())
    return;
  if (this->temperatures.size() != this->temperature_datetimes.size())
    return;
  this->customPlot->addGraph(customPlot->xAxis, customPlot->yAxis2);

  this->customPlot->graph(this->nplot)->setData(this->temperature_datetimes, this->temperatures);
  this->customPlot->graph(this->nplot)->setPen((QPen(Qt::red, 2)));
  this->customPlot->graph(this->nplot)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 12));

  this->nplot++;
}

void MainWindow::add_batt_volt() {
  if (this->customPlot == nullptr)
    return;
  if (!this->batt_volts.size())
    return;
  if (this->batt_volts.size() != this->batt_volt_datetimes.size())
    return;
  this->customPlot->addGraph();
  this->customPlot->graph(this->nplot)->setData(this->batt_volt_datetimes, this->batt_volts);
  this->customPlot->graph(this->nplot)->setPen((QPen(Qt::darkYellow, 2)));
  this->customPlot->graph(this->nplot)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPlus, 12));

  this->nplot++;
}

void MainWindow::add_batt1_curr() {
  if (this->customPlot == nullptr)
    return;
  if (!this->batt1_currs.size())
    return;
  if (this->batt1_currs.size() != this->batt1_curr_datetimes.size())
    return;
  this->customPlot->addGraph();
  this->customPlot->graph(this->nplot)->setData(this->batt1_curr_datetimes, this->batt1_currs);
  this->customPlot->graph(this->nplot)->setPen((QPen(Qt::green, 2)));
  this->customPlot->graph(this->nplot)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssTriangle, 12));

  this->nplot++;
}

void MainWindow::add_batt2_curr() {
  if (this->customPlot == nullptr)
    return;
  if (!this->batt2_currs.size())
    return;
  if (this->batt2_currs.size() != this->batt2_curr_datetimes.size())
    return;
  this->customPlot->addGraph();
  this->customPlot->graph(this->nplot)->setData(this->batt2_curr_datetimes, this->batt2_currs);
  this->customPlot->graph(this->nplot)->setPen((QPen(Qt::darkGreen, 2)));
  this->customPlot->graph(this->nplot)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssTriangleInverted, 12));

  this->nplot++;
}

void MainWindow::estimate_scale() {
  qint64 days = this->ui->dateTimeEdit_from->dateTime().daysTo(this->ui->dateTimeEdit_to->dateTime());
}

void MainWindow::create_plot() {
  if (this->customPlot != nullptr) {
    this->customPlot.reset();
  }
  this->customPlot = std::make_unique<QCustomPlot>(this);
  this->ui->vl_plot->addWidget(customPlot.get());
  customPlot->setLocale(QLocale(QLocale::English, QLocale::World));
  customPlot->xAxis->setLabel("Date");
  customPlot->yAxis->setLabel("values [n, A, V]");
  customPlot->yAxis2->setLabel("temperature [°C]");
  customPlot->yAxis->setRange(0, 20);
  customPlot->yAxis2->setRange(0, 100);
  customPlot->yAxis2->setVisible(true);
  customPlot->show();
}

void MainWindow::reserves(const size_t size) {
  this->num_sats_sats.reserve(size);
  this->num_sats_datetimes.reserve(size);

  this->batt1_currs.reserve(size);
  this->batt1_curr_datetimes.reserve(size);

  this->batt2_currs.reserve(size);
  this->batt2_curr_datetimes.reserve(size);

  this->batt_volts.reserve(size);
  this->batt_volt_datetimes.reserve(size);

  this->batt_states.reserve(size);
  this->batt_state_datetimes.reserve(size);

  this->temperatures.reserve(size);
  this->temperature_datetimes.reserve(size);

  this->all_datetimes.reserve(size);
}

void MainWindow::read_message(const QSqlQuery &query) {
  if (query.record().count() != 7)
    return;
  QDateTime timestamp;
  timestamp.setTimeZone(QTimeZone::utc());
  timestamp = QDateTime::fromString(query.value(1).toString(), "yyyy-MM-dd hh:mm:ss");

  this->xmlreader.addData(query.value(6).toString()); // xml field
  this->xmlreader.readNextStartElement();
  while (this->xmlreader.readNextStartElement()) {
    bool found = false;
    for (auto const &str : this->tokens) {
      //           qDebug() << this->xmlreader.name();
      if (!xmlreader.name().compare(str)) {
        found = true;
        QString s = xmlreader.readElementText();
        if (timestamp > QDateTime::fromString("2010-01-01", "yyyy-MM-dd")) {
          if (str == "num_sats") {
            this->num_sats_sats.emplace_back(s.toDouble());
            this->num_sats_datetimes.push_back(double(timestamp.toSecsSinceEpoch()));
          }
          if (str == "batt1_curr") {
            this->batt1_currs.emplace_back(s.toDouble() / 100.);
            this->batt1_curr_datetimes.push_back(double(timestamp.toSecsSinceEpoch()));
          }
          if (str == "batt2_curr") {
            this->batt2_currs.emplace_back(s.toDouble() / 100.);
            this->batt2_curr_datetimes.push_back(double(timestamp.toSecsSinceEpoch()));
          }
          if (str == "batt_volt") {
            this->batt_volts.emplace_back(s.toDouble() / 10.);
            this->batt_volt_datetimes.push_back(double(timestamp.toSecsSinceEpoch()));
          }
          if (str == "batt_state") {
            this->batt_states.emplace_back(s.toDouble());
            this->batt_state_datetimes.push_back(double(timestamp.toSecsSinceEpoch()));
          }
          if (str == "temp") {
            this->temperatures.emplace_back(s.toDouble());
            this->temperature_datetimes.push_back(double(timestamp.toSecsSinceEpoch()));
          }
          this->all_datetimes.push_back(timestamp.toSecsSinceEpoch());
        }
      }
    }
    if (!found)
      xmlreader.skipCurrentElement();
  }
  xmlreader.clear();
}

void MainWindow::min_max_time() {
  if (!this->all_datetimes.size())
    return;
  this->mintime.setTimeZone(QTimeZone::utc());
  this->maxtime.setTimeZone(QTimeZone::utc());
  auto result = std::minmax_element(this->all_datetimes.begin(), this->all_datetimes.end());

  this->mintime = QDateTime::fromSecsSinceEpoch(*result.first);
  this->maxtime = QDateTime::fromSecsSinceEpoch(*result.second);
  this->old_from = QDateTime::fromSecsSinceEpoch(*result.first);
  this->old_to = QDateTime::fromSecsSinceEpoch(*result.second);

  this->ui->dateTimeEdit_from->setDateTime(mintime);
  this->ui->dateTimeEdit_to->setDateTime(maxtime);
  this->ui->dateTimeEdit_from->setDateTimeRange(this->mintime, this->maxtime);
  this->ui->dateTimeEdit_to->setDateTimeRange(this->mintime, this->maxtime);

  customPlot->xAxis->setRange(double(*result.first), double(*result.second));

  //    this->ui->dateTimeEdit_from->setDateTimeRange(this->mintime, this->maxtime);
  //    this->ui->dateTimeEdit_to->setDateTimeRange(this->mintime, this->maxtime);
}

MainWindow::~MainWindow() {
  if (this->db->isOpen()) {

    this->db->close();
    QSqlDatabase::removeDatabase(this->db->connectionName());
  }
  this->db.reset();
  delete ui;
}

void MainWindow::on_dateTimeEdit_from_dateTimeChanged(const QDateTime &dateTime) {
  if (dateTime >= this->old_to) {
    this->blockSignals(true);
    this->ui->dateTimeEdit_from->setDateTime(this->old_from);
    this->blockSignals(false);
    return;
  }

  int old_year = this->old_from.date().year();
  int new_year = dateTime.date().year();

  int old_month = this->old_from.date().month();
  int new_month = dateTime.date().month();

  bool changed = false;

  QDateTime new_dt(dateTime);

  if (!this->last30) {
    if (new_year > old_year) {
      new_dt.setDate(QDate(new_year, 1, 1));
      this->blockSignals(true);
      this->ui->dateTimeEdit_from->setDateTime(new_dt);
      changed = true;
      this->blockSignals(false);
    }

    if (!changed && (new_month > old_month)) {
      new_dt.setDate(QDate(new_year, new_month, 1));
      this->blockSignals(true);
      this->ui->dateTimeEdit_from->setDateTime(new_dt);
      changed = true;
      this->blockSignals(false);
    }
  }
  this->last30 = false;
  this->old_from = new_dt;

  customPlot->xAxis->setRange(double(new_dt.toSecsSinceEpoch()),
                              double(this->ui->dateTimeEdit_to->dateTime().toSecsSinceEpoch()));

  this->customPlot->replot();
}

void MainWindow::on_dateTimeEdit_to_dateTimeChanged(const QDateTime &dateTime) {
  if (dateTime <= this->old_from) {
    this->blockSignals(true);
    this->ui->dateTimeEdit_to->setDateTime(this->old_to);
    this->blockSignals(false);
    return;
  }
  this->old_to = dateTime;
  customPlot->xAxis->setRange(double(this->ui->dateTimeEdit_from->dateTime().toSecsSinceEpoch()), double(dateTime.toSecsSinceEpoch()));
  this->customPlot->replot();
}

void MainWindow::on_pushButton_3_clicked() {
  QDateTime l30(this->maxtime);
  l30 = l30.addDays(-30);
  this->last30 = true;
  this->ui->dateTimeEdit_from->setDateTime(l30);
}

void MainWindow::on_dial_from_sliderMoved(int position) {
  qDebug() << position;
  if ((position > this->dial_from_pos) || ((this->dial_from_pos == this->ui->dial_from->maximum()) && position == this->ui->dial_from->minimum())) {
    this->dial_from_pos = position;
    this->ui->dateTimeEdit_from->setDateTime(this->old_from.addDays(1));
  } else {
    this->dial_from_pos = position;
    this->ui->dateTimeEdit_from->setDateTime(this->old_from.addDays(-1));
  }
}

void MainWindow::on_dial_to_sliderMoved(int position) {
  qDebug() << position;
  if ((position > this->dial_to_pos) || ((this->dial_to_pos == this->ui->dial_to->maximum()) && position == this->ui->dial_to->minimum())) {
    this->dial_to_pos = position;
    this->ui->dateTimeEdit_to->setDateTime(this->old_to.addDays(1));
  } else {
    this->dial_from_pos = position;
    this->ui->dateTimeEdit_to->setDateTime(this->old_to.addDays(-1));
  }
}
