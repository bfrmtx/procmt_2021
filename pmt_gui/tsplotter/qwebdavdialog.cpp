#include "qwebdavdialog.h"

#include <atsfile.h>
#include <atsheader.h>
#include <qthelper.h>

int constexpr ATS_HEADER_SIZE = 1024;
int constexpr ATS_HEADER_VERSION = 80;

std::array<const char *, 2> const FILTER{{"json", "ats"}};

QString trimURI(QString uri) {
  uri = uri.trimmed();
  if (uri.isEmpty())
    return QString();
  uri = QUrl(uri).toString(QUrl::NormalizePathSegments);
  if (uri.back() == '/')
    uri = uri.mid(0, uri.size() - 1);
  if (uri.front() == '/')
    uri = uri.mid(1);
  return uri;
}

QString fileNameOfURI(QString uri) {
  auto ref = uri.back() == '/' ? uri.mid(0, uri.size() - 1) : uri;
  return ref.split("/").last();
}

QString fileExtension(QString file_name) {
  QFileInfo fi(file_name);
  return fi.completeSuffix();
}

bool extIsAllowed(QString const &ext) {
  return std::find(FILTER.begin(), FILTER.end(), ext.toStdString()) != FILTER.end();
}

QWebDavDialog::QWebDavDialog(QString const &host_address, QWidget *parent) :
    QDialog(parent), m_edit_timer(new QTimer(parent)) {
  setupUi(this);
  setWindowFlag(Qt::WindowContextHelpButtonHint, false);
  progress_download->setVisible(false);
  this->setEnabled(false);

  m_network_manager = new ATSNetworkManager(host_address, this);
  connect(m_network_manager, &ATSNetworkManager::connectionVerified, this, [this, host_address]() {
    this->setEnabled(true);
    QMessageBox::information(this, "Information", "connection verrified");

    connect(m_edit_timer, &QTimer::timeout, this, &QWebDavDialog::timer_timeout);

    m_root = host_address;
    if (!m_root.startsWith("https://", Qt::CaseInsensitive)) {
      m_root.prepend("https://");
    }

    auto m_request = m_root + "/webdav_mtdata/";

    m_links.push_back(m_request);
    edit_url->setText(m_request);

    requestFolder(m_request);
  });
  connect(m_network_manager, &ATSNetworkManager::connectionVerificationFailed, this, [this]() {
    QMessageBox::critical(this, "Error", "connection could not be verified");
    close();
    return;
  });
}

void QWebDavDialog::show() {
  enableButtons(false);
  setWindowTitle("Select ATS file(s)...");
  QDialog::show();
}

void QWebDavDialog::requestFolder(QString const &uri, QTreeWidgetItem *current) {
  m_network_manager->getContent(uri, [uri, current, this](webdav::WebDavResponse const &response) {
    QTreeWidgetItem *parent = current;

    if (current == nullptr) {
      tree_view->clear();
      tree_view->header()->hide();
      auto top = new QTreeWidgetItem();
      top->setText(0, uri);
      top->setIcon(0, QIcon(":/icons/web.png"));
      tree_view->addTopLevelItem(top);
      parent = top;
    }

    for (auto &v : response.getResponses()) {
      // Qt6
      // auto full_uri = QString("%1/%2").arg(m_root, (v.ref.at(0) == "/" ) ? v.ref.mid(1) : v.ref);
      QString full_uri;
      full_uri = QUrl(full_uri).toString(QUrl::NormalizePathSegments);

      if (full_uri == uri) {
        continue;
      }

      auto ref = fileNameOfURI(trimURI(v.ref));
      auto ext = fileExtension(ref);

      if (extIsAllowed(ext) || v.subtype == webdav::ContentSubtype::UnixDirectory) {
        auto child = new QTreeWidgetItem();

        if (v.subtype == webdav::ContentSubtype::UnixDirectory) {
          child->setSelected(false);
        }

        child->setText(0, ref);
        child->setData(0, Qt::UserRole, QVariant::fromValue(new QWebDavDialogItemData(full_uri, v)));

        if (v.subtype == webdav::ContentSubtype::UnixDirectory) {
          child->setIcon(0, QIcon(":/icons/folder.png"));
        } else {
          child->setIcon(0, QIcon(":/icons/generic.png"));
        }
        parent->addChild(child);
      }
    }
    parent->setExpanded(true);
  });
}

void QWebDavDialog::requestHeader(QString const &uri) {
  m_network_manager->execSecuredGetRequest(uri, 0, ATS_HEADER_SIZE - 1, [this, uri](QByteArray const &answer) {
    auto *ptr = reinterpret_cast<ATSHeader_80 const *>(answer.constData());

    if (ptr->uiHeaderLength == ATS_HEADER_SIZE && ptr->siHeaderVers == ATS_HEADER_VERSION) {
      uint64_t samples = ptr->uiSamples == UINT_MAX ? ptr->uiSamples64bit : ptr->uiSamples;
      label_ats_lat->setText(QString::number(ptr->iLatitude));
      label_ats_lon->setText(QString::number(ptr->iLongitude));
      label_ats_samples->setText(QString::number(samples));
      label_ats_freq->setText(QString("%1 Hz").arg(QString::number(ptr->rSampleFreq)));
      label_ats_time->setText(QDateTime::fromSecsSinceEpoch(ptr->uiStartDateTime).toString(Qt::DateFormat::ISODate));
    }
  });
}

void QWebDavDialog::downloadFiles(QString const &target, QStringList const &URIs) {
  m_pending_requests = URIs.size();
  m_requested_bytes = 0;

  for (auto const &uri : URIs) {
    m_network_manager->execSecuredHeadRequest(uri, [this, target, URIs](QList<QNetworkReply::RawHeaderPair> const &header) {
      --m_pending_requests;

      for (auto const &h : header) {
        if (h.first == "Content-Length") {
          m_requested_bytes += h.second.toULongLong();
        }
      }

      if (m_pending_requests == 0) {
        progress_download->setMaximum(static_cast<int>(m_requested_bytes / 100));
        progress_download->setValue(0);
        progress_download->setVisible(true);
        label_progress->setText(QString("0/%1 KB").arg(m_requested_bytes / 1024));
        label_progress->setVisible(true);

        auto *progresses = new std::vector<int>();
        auto *finished = new std::vector<bool>();
        size_t f = 0;

        enableButtons(false);
        for (auto const &uri : URIs) {
          progresses->push_back(0);
          finished->push_back(false);

          auto rep = m_network_manager->execSecuredGetRequestObj(uri, [=, this](QNetworkReply *reply) {
            QString file_name = fileNameOfURI(trimURI(uri));
            QString target_file = QString("%1/%2").arg(target, file_name);
            QFile output(target_file);
            if (output.open(QFile::WriteOnly)) {
              output.write(reply->readAll());
              output.close();
            }
            (*finished)[f] = true;
            if (std::find(finished->begin(), finished->end(), false) == finished->end()) {
              delete finished;
              delete progresses;
              enableButtons(true);
              progress_download->setVisible(false);
              label_progress->setVisible(false);
            }
          });
          connect(rep, &QNetworkReply::downloadProgress, this, [&, progresses, f](qint64 br, qint64) {
            (*progresses)[f] = br;
            auto val = std::accumulate(progresses->begin(), progresses->end(), 0);
            progress_download->setValue(val / 100);
            label_progress->setText(QString("%1/%2 KB").arg(val / 1024).arg(m_requested_bytes / 1024));
          });
          ++f;
        }
      }
    });
  }
}

void QWebDavDialog::close_and_select(const QStringList &URIs) {
  emit files_selected(URIs, rd_load_all->isChecked());
  this->close();
}

void QWebDavDialog::enableButtons(bool value) {
  button_select->setEnabled(value);
  button_download->setEnabled(value);
}

QStringList const QWebDavDialog::currentURIs() {
  auto const &items = tree_view->selectedItems();
  QStringList files;
  for (auto &item : items) {
    auto const &data = qvariant_cast<QWebDavDialogItemData *>(item->data(0, Qt::UserRole));
    if (data != nullptr) {
      files.push_back(data->uri);
    }
  }
  return files;
}

void QWebDavDialog::on_tree_view_itemClicked(QTreeWidgetItem *item, int) {
  enableButtons(false);
  auto const &data = qvariant_cast<QWebDavDialogItemData *>(item->data(0, Qt::UserRole));
  if (data != nullptr) {
    if (data->data.subtype == webdav::ContentSubtype::UnixDirectory) {
      if (item->childCount() == 0) {
        requestFolder(data->uri, item);
      }
      item->setSelected(false);
    } else {
      if (tree_view->selectedItems().size() == 1) {
        auto ext = fileExtension(data->uri);
        if (ext == "ats") {
          requestHeader(data->uri);
          enableButtons(true);
          m_current_uri = data->uri;
        }
      }
    }
  }
  if (!tree_view->selectedItems().isEmpty()) {
    enableButtons(true);
  }
}

void QWebDavDialog::on_button_goto_clicked() {
  requestFolder(edit_url->text());
}

void QWebDavDialog::on_button_cancel_clicked() {
  this->close();
}

void QWebDavDialog::on_button_select_clicked() {
  close_and_select(currentURIs());
}

void QWebDavDialog::on_edit_url_textEdited(QString const &) {
  m_edit_time = 4;
  if (!m_edit_timer->isActive()) {
    m_edit_timer->start(250);
  }
  tree_view->setEnabled(false);
}

void QWebDavDialog::timer_timeout() {
  if (m_edit_time > 0) {
    --m_edit_time;
    if (m_edit_time == 0) {
      m_edit_timer->stop();
      timer_triggered();
    }
  }
}

void QWebDavDialog::timer_triggered() {
  requestFolder(edit_url->text());
  tree_view->setEnabled(true);
  if (edit_url->text().toLower().startsWith("https://")) {
    auto i = edit_url->text().indexOf('/', 9);
    m_root = edit_url->text().mid(0, i);
  } else {
    auto i = edit_url->text().indexOf('/');
    m_root = edit_url->text().mid(0, i);
  }
}

QWebDavDialogItemData::QWebDavDialogItemData(QString uri, webdav::Response data) :
    uri(uri), data(data) {}

void QWebDavDialog::on_button_download_clicked() {
  auto files = currentURIs();

  auto target = QFileDialog::getExistingDirectory(nullptr, "Select target location...",
                                                  QStandardPaths::writableLocation(QStandardPaths::StandardLocation::HomeLocation));
  if (!target.isEmpty()) {
    QDir target_dir(target);
    if (target_dir.exists() && qthelper::create_survey_subs(target_dir)) {
      QString full_target = QString("%1/%2/%3/meas_YYYY-MM-DD_hh-mm-ss").arg(target, "ts", "Site_1");
      target_dir.mkpath(full_target);
      downloadFiles(full_target, files);
    }
  }
}
