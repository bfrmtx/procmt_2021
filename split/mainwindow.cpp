#include "mainwindow.h"

#include <QButtonGroup>
#include <QDateTime>
#include <QDir>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QGroupBox>
#include <QHash>
#include <QLabel>
#include <QMessageBox>
#include <QMimeData>
#include <QPushButton>
#include <QRadioButton>
#include <QRegularExpression>
#include <QStatusBar>
#include <QTextEdit>
#include <QTextStream>
#include <QTimeZone>
#include <QUrl>
#include <QVBoxLayout>
#include <QtEndian>
#include <QtXml/QDomDocument>

#include <cstdint>
#include <cstring>
#include <limits>

namespace {
constexpr qint64 kAtsHeaderBytes = 1024;
constexpr qint64 kBytesPerSample = 4;

QString directoryFromMimeData(const QMimeData *mimeData) {
  if (!mimeData->hasUrls()) {
    return QString();
  }

  for (const QUrl &url : mimeData->urls()) {
    if (!url.isLocalFile()) {
      continue;
    }

    const QFileInfo fileInfo(url.toLocalFile());
    if (fileInfo.isFile() && fileInfo.suffix().compare(QStringLiteral("xml"), Qt::CaseInsensitive) == 0) {
      return fileInfo.absoluteFilePath();
    }
  }

  return QString();
}

bool loadXmlDocument(const QFileInfo &xmlFile, QDomDocument &document, QString *errorMessage) {
  QFile file(xmlFile.absoluteFilePath());
  if (!file.open(QIODevice::ReadOnly)) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("Could not open XML file: %1").arg(xmlFile.absoluteFilePath());
    }
    return false;
  }

  const QDomDocument::ParseResult parseResult = document.setContent(&file);
  if (!parseResult) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("Could not parse XML file %1 at %2:%3: %4")
                          .arg(xmlFile.absoluteFilePath())
                          .arg(parseResult.errorLine)
                          .arg(parseResult.errorColumn)
                          .arg(parseResult.errorMessage);
    }
    return false;
  }

  return true;
}

QDomElement childElement(QDomElement parent, const QString &name) {
  if (parent.isNull()) {
    return QDomElement();
  }

  return parent.firstChildElement(name);
}

void setElementText(QDomElement element, const QString &text) {
  QDomNode child = element.firstChild();
  if (child.isNull()) {
    element.appendChild(element.ownerDocument().createTextNode(text));
    return;
  }

  child.setNodeValue(text);
}

QDomElement recordingElement(QDomDocument &document) {
  return childElement(document.documentElement(), QStringLiteral("recording"));
}

QDomElement atsWriterConfiguration(QDomDocument &document) {
  QDomElement element = recordingElement(document);
  element = childElement(element, QStringLiteral("output"));
  element = childElement(element, QStringLiteral("ProcessingTree"));
  element = childElement(element, QStringLiteral("output"));
  if (element.isNull()) {
    return QDomElement();
  }

  const QDomNodeList atsWriterNodes = element.elementsByTagName(QStringLiteral("ATSWriter"));
  if (atsWriterNodes.isEmpty()) {
    return QDomElement();
  }

  return childElement(atsWriterNodes.at(0).toElement(), QStringLiteral("configuration"));
}

QHash<QString, QDomElement> channelsById(QDomElement configuration) {
  QHash<QString, QDomElement> channels;

  for (QDomElement channel = configuration.firstChildElement(QStringLiteral("channel")); !channel.isNull();
       channel = channel.nextSiblingElement(QStringLiteral("channel"))) {
    channels.insert(channel.attribute(QStringLiteral("id")), channel);
  }

  return channels;
}

bool saveXmlDocument(const QFileInfo &xmlFile, const QDomDocument &document, QString *errorMessage) {
  QFile file(xmlFile.absoluteFilePath());
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("Could not write target XML file: %1").arg(xmlFile.absoluteFilePath());
    }
    return false;
  }

  QTextStream stream(&file);
  document.save(stream, 4);
  return true;
}

QHash<QString, QString> atsFilesByChannel(const QFileInfo &xmlFile, QString *errorMessage) {
  QDomDocument document;
  if (!loadXmlDocument(xmlFile, document, errorMessage)) {
    return QHash<QString, QString>();
  }

  const QDomElement configuration = atsWriterConfiguration(document);
  if (configuration.isNull()) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("XML has no expected ATSWriter configuration: %1").arg(xmlFile.absoluteFilePath());
    }
    return QHash<QString, QString>();
  }

  QHash<QString, QString> atsFiles;
  const QHash<QString, QDomElement> channels = channelsById(configuration);
  for (auto it = channels.constBegin(); it != channels.constEnd(); ++it) {
    const QDomElement atsDataFile = childElement(it.value(), QStringLiteral("ats_data_file"));
    if (!atsDataFile.isNull() && !atsDataFile.text().isEmpty()) {
      atsFiles.insert(it.key(), atsDataFile.text());
    }
  }

  return atsFiles;
}

bool readAtsHeader(const QString &path, QByteArray &headerBytes, QString *errorMessage) {
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly)) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("Could not open ATS file: %1").arg(path);
    }
    return false;
  }

  headerBytes = file.read(kAtsHeaderBytes);
  if (headerBytes.size() != kAtsHeaderBytes) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("ATS header is incomplete: %1").arg(path);
    }
    return false;
  }

  return true;
}

bool writeAtsHeaderFields(QByteArray &headerBytes, quint32 startEpoch, quint32 samples32, quint64 samples64,
                          const QString &xmlFileName, QString *errorMessage) {
  if (headerBytes.size() != kAtsHeaderBytes) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("Internal header size mismatch.");
    }
    return false;
  }

  const quint32 startEpochLe = qToLittleEndian(startEpoch);
  memcpy(headerBytes.data() + 0x000C, &startEpochLe, sizeof(startEpochLe));

  const quint32 samples32Le = qToLittleEndian(samples32);
  memcpy(headerBytes.data() + 0x0004, &samples32Le, sizeof(samples32Le));

  const quint64 samples64Le = qToLittleEndian(samples64);
  memcpy(headerBytes.data() + 0x00F0, &samples64Le, sizeof(samples64Le));

  QByteArray xmlHeaderBytes = xmlFileName.toLatin1();
  if (xmlHeaderBytes.size() > 63) {
    xmlHeaderBytes = xmlHeaderBytes.left(63);
  }

  char *xmlHeaderField = headerBytes.data() + 0x01C0;
  memset(xmlHeaderField, 0, 64);
  memcpy(xmlHeaderField, xmlHeaderBytes.constData(), xmlHeaderBytes.size());

  return true;
}

bool copyAtsRange(const QFileInfo &sourceAtsFile, const QString &targetAtsPath, qint64 startSample, qint64 sampleCount,
                  quint32 startEpoch, const QString &targetXmlFileName, QString *errorMessage) {
  QFile source(sourceAtsFile.absoluteFilePath());
  if (!source.open(QIODevice::ReadOnly)) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("Could not open source ATS file: %1").arg(sourceAtsFile.absoluteFilePath());
    }
    return false;
  }

  const qint64 payloadBytes = source.size() - kAtsHeaderBytes;
  if (payloadBytes < 0 || (payloadBytes % kBytesPerSample) != 0) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("Invalid ATS payload size: %1").arg(sourceAtsFile.absoluteFilePath());
    }
    return false;
  }

  const qint64 totalSamples = payloadBytes / kBytesPerSample;
  if (startSample < 0 || sampleCount < 0 || startSample + sampleCount > totalSamples) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("Requested ATS slice is out of range: %1").arg(sourceAtsFile.absoluteFilePath());
    }
    return false;
  }

  QByteArray headerBytes;
  if (!readAtsHeader(sourceAtsFile.absoluteFilePath(), headerBytes, errorMessage)) {
    return false;
  }

  const bool needs64BitSamples = sampleCount >= std::numeric_limits<std::uint32_t>::max();
  const quint32 samples32 = needs64BitSamples ? std::numeric_limits<std::uint32_t>::max() : static_cast<quint32>(sampleCount);
  const quint64 samples64 = static_cast<quint64>(sampleCount);

  if (!writeAtsHeaderFields(headerBytes, startEpoch, samples32, samples64, targetXmlFileName, errorMessage)) {
    return false;
  }

  QFile target(targetAtsPath);
  if (!target.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("Could not create target ATS file: %1").arg(targetAtsPath);
    }
    return false;
  }

  if (target.write(headerBytes) != headerBytes.size()) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("Could not write ATS header: %1").arg(targetAtsPath);
    }
    return false;
  }

  const qint64 sourceOffset = kAtsHeaderBytes + (startSample * kBytesPerSample);
  if (!source.seek(sourceOffset)) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("Could not seek source ATS file: %1").arg(sourceAtsFile.absoluteFilePath());
    }
    return false;
  }

  qint64 remainingBytes = sampleCount * kBytesPerSample;
  QByteArray buffer;
  buffer.resize(1024 * 1024);

  while (remainingBytes > 0) {
    const qint64 thisRead = qMin<qint64>(buffer.size(), remainingBytes);
    const qint64 bytesRead = source.read(buffer.data(), thisRead);
    if (bytesRead <= 0) {
      if (errorMessage != nullptr) {
        *errorMessage = QStringLiteral("Could not read ATS payload: %1").arg(sourceAtsFile.absoluteFilePath());
      }
      return false;
    }

    if (target.write(buffer.constData(), bytesRead) != bytesRead) {
      if (errorMessage != nullptr) {
        *errorMessage = QStringLiteral("Could not write ATS payload: %1").arg(targetAtsPath);
      }
      return false;
    }

    remainingBytes -= bytesRead;
  }

  return true;
}

bool atsSamplesFromFile(const QString &atsPath, qint64 &samples, QString *errorMessage) {
  QFile source(atsPath);
  if (!source.open(QIODevice::ReadOnly)) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("Could not open ATS file: %1").arg(atsPath);
    }
    return false;
  }

  const qint64 payloadBytes = source.size() - kAtsHeaderBytes;
  if (payloadBytes <= 0 || (payloadBytes % kBytesPerSample) != 0) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("Invalid ATS payload size: %1").arg(atsPath);
    }
    return false;
  }

  samples = payloadBytes / kBytesPerSample;
  return true;
}

QDateTime parseUtcDateTime(const QString &date, const QString &time) {
  const QDate qDate = QDate::fromString(date, QStringLiteral("yyyy-MM-dd"));
  const QTime qTime = QTime::fromString(time, QStringLiteral("HH:mm:ss"));
  return QDateTime(qDate, qTime, QTimeZone::utc());
}

QString runNumberPadded(int runIndex) {
  return QStringLiteral("R%1").arg(runIndex, 3, 10, QChar('0'));
}

QString updateRunNumber(const QString &baseName, int runIndex) {
  const QRegularExpression re(QStringLiteral("_R\\d{3}"));
  QString updated = baseName;
  if (updated.contains(re)) {
    updated.replace(re, QStringLiteral("_") + runNumberPadded(runIndex));
  } else {
    updated += QStringLiteral("_") + runNumberPadded(runIndex);
  }
  return updated;
}

QString renamedAtsFileForRun(const QString &originalAtsName, int runIndex) {
  QFileInfo info(originalAtsName);
  return updateRunNumber(info.completeBaseName(), runIndex) + QStringLiteral(".") + info.suffix();
}

QString renamedXmlFileForRun(const QString &sourceXmlName, int runIndex, const QDateTime &startUtc, const QDateTime &stopUtc) {
  QFileInfo info(sourceXmlName);
  QString baseName = info.completeBaseName();
  baseName = updateRunNumber(baseName, runIndex);

  const QRegularExpression startStopRe(
      QStringLiteral("_(\\d{4}-\\d{2}-\\d{2})_(\\d{2}-\\d{2}-\\d{2})_(\\d{4}-\\d{2}-\\d{2})_(\\d{2}-\\d{2}-\\d{2})_"));

  const QString replacement =
      QStringLiteral("_%1_%2_%3_%4_")
          .arg(startUtc.date().toString(QStringLiteral("yyyy-MM-dd")), startUtc.time().toString(QStringLiteral("HH-mm-ss")),
               stopUtc.date().toString(QStringLiteral("yyyy-MM-dd")), stopUtc.time().toString(QStringLiteral("HH-mm-ss")));

  if (baseName.contains(startStopRe)) {
    baseName.replace(startStopRe, replacement);
  }

  return baseName + QStringLiteral(".") + info.suffix();
}

} // namespace

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle(QStringLiteral("split"));
  resize(760, 820);
  setAcceptDrops(true);

  auto *central = new QWidget(this);
  central->setAcceptDrops(true);
  central->installEventFilter(this);

  auto *layout = new QVBoxLayout(central);

  dropLabel = new QLabel(QStringLiteral("Drop a single measdoc XML file here"), central);
  dropLabel->setAcceptDrops(true);
  dropLabel->installEventFilter(this);
  dropLabel->setAlignment(Qt::AlignCenter);
  dropLabel->setMinimumHeight(120);
  dropLabel->setFrameShape(QFrame::StyledPanel);

  measdocLabel = new QLabel(QStringLiteral("No measdoc selected"), central);
  measdocLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

  auto *chooseButton = new QPushButton(QStringLiteral("Choose measdoc..."), central);

  auto *sizeGroupBox = new QGroupBox(QStringLiteral("Split Size"), central);
  auto *sizeLayout = new QVBoxLayout(sizeGroupBox);
  sizeButtons = new QButtonGroup(this);

  auto *rb256 = new QRadioButton(QStringLiteral("256 MB"), sizeGroupBox);
  auto *rb512 = new QRadioButton(QStringLiteral("512 MB"), sizeGroupBox);
  auto *rb1g = new QRadioButton(QStringLiteral("1 GB"), sizeGroupBox);
  auto *rb2g = new QRadioButton(QStringLiteral("2 GB"), sizeGroupBox);

  splitSizeByButtonId.insert(0, 256LL * 1024LL * 1024LL);
  splitSizeByButtonId.insert(1, 512LL * 1024LL * 1024LL);
  splitSizeByButtonId.insert(2, 1024LL * 1024LL * 1024LL);
  splitSizeByButtonId.insert(3, 2LL * 1024LL * 1024LL * 1024LL);

  sizeButtons->addButton(rb256, 0);
  sizeButtons->addButton(rb512, 1);
  sizeButtons->addButton(rb1g, 2);
  sizeButtons->addButton(rb2g, 3);
  rb256->setChecked(true);

  sizeLayout->addWidget(rb256);
  sizeLayout->addWidget(rb512);
  sizeLayout->addWidget(rb1g);
  sizeLayout->addWidget(rb2g);

  splitButton = new QPushButton(QStringLiteral("split"), central);
  splitButton->setEnabled(false);

  logText = new QTextEdit(central);
  logText->setAcceptDrops(false);
  logText->viewport()->setAcceptDrops(false);
  logText->setReadOnly(true);

  layout->addWidget(dropLabel);
  layout->addWidget(measdocLabel);
  layout->addWidget(chooseButton);
  layout->addWidget(sizeGroupBox);
  layout->addWidget(splitButton);
  layout->addWidget(logText, 1);

  setCentralWidget(central);
  statusBar()->showMessage(QStringLiteral("Drop a measdoc XML to begin."));

  connect(chooseButton, &QPushButton::clicked, this, &MainWindow::chooseMeasdoc);
  connect(splitButton, &QPushButton::clicked, this, &MainWindow::splitMeasdoc);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
  if (watched == centralWidget() || watched == dropLabel) {
    if (event->type() == QEvent::DragEnter) {
      auto *dragEvent = static_cast<QDragEnterEvent *>(event);
      if (!directoryFromMimeData(dragEvent->mimeData()).isEmpty()) {
        dragEvent->acceptProposedAction();
        return true;
      }
    }

    if (event->type() == QEvent::DragMove) {
      auto *dragEvent = static_cast<QDragMoveEvent *>(event);
      if (!directoryFromMimeData(dragEvent->mimeData()).isEmpty()) {
        dragEvent->acceptProposedAction();
        return true;
      }
    }

    if (event->type() == QEvent::Drop) {
      auto *dropEvent = static_cast<QDropEvent *>(event);
      const QString file = directoryFromMimeData(dropEvent->mimeData());
      if (!file.isEmpty()) {
        setMeasdoc(file);
        dropEvent->acceptProposedAction();
        return true;
      }
    }
  }

  return QMainWindow::eventFilter(watched, event);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
  if (!directoryFromMimeData(event->mimeData()).isEmpty()) {
    event->acceptProposedAction();
    return;
  }

  event->ignore();
}

void MainWindow::dropEvent(QDropEvent *event) {
  const QString file = droppedMeasdoc(event);
  if (file.isEmpty()) {
    event->ignore();
    return;
  }

  setMeasdoc(file);
  event->acceptProposedAction();
}

void MainWindow::chooseMeasdoc() {
  const QString filePath = QFileDialog::getOpenFileName(this, QStringLiteral("Choose measdoc"), QString(), QStringLiteral("XML files (*.xml *.XML)"));
  if (!filePath.isEmpty()) {
    setMeasdoc(filePath);
  }
}

void MainWindow::setMeasdoc(const QString &path) {
  const QFileInfo info(path);
  measdocFile = info;
  measdocLabel->setText(measdocFile.absoluteFilePath());
  splitButton->setEnabled(measdocFile.exists());
  logText->append(QStringLiteral("Selected measdoc: %1").arg(measdocFile.absoluteFilePath()));
  statusBar()->showMessage(QStringLiteral("Ready."));
}

QString MainWindow::droppedMeasdoc(const QDropEvent *event) const {
  return directoryFromMimeData(event->mimeData());
}

qint64 MainWindow::selectedChunkBytes() const {
  const int id = sizeButtons->checkedId();
  if (!splitSizeByButtonId.contains(id)) {
    return 256LL * 1024LL * 1024LL;
  }

  return splitSizeByButtonId.value(id);
}

void MainWindow::splitMeasdoc() {
  if (!measdocFile.exists()) {
    statusBar()->showMessage(QStringLiteral("No measdoc selected."));
    return;
  }

  const qint64 chunkBytes = selectedChunkBytes();
  if (chunkBytes <= 0 || (chunkBytes % kBytesPerSample) != 0) {
    statusBar()->showMessage(QStringLiteral("Invalid split size selected."));
    return;
  }

  const qint64 chunkSamples = chunkBytes / kBytesPerSample;

  QString errorMessage;
  QDomDocument sourceDocument;
  if (!loadXmlDocument(measdocFile, sourceDocument, &errorMessage)) {
    logText->append(errorMessage);
    statusBar()->showMessage(QStringLiteral("Could not read measdoc."));
    return;
  }

  QDomElement sourceRecording = recordingElement(sourceDocument);
  QDomElement sourceStartDate = childElement(sourceRecording, QStringLiteral("start_date"));
  QDomElement sourceStartTime = childElement(sourceRecording, QStringLiteral("start_time"));
  QDomElement sourceStopDate = childElement(sourceRecording, QStringLiteral("stop_date"));
  QDomElement sourceStopTime = childElement(sourceRecording, QStringLiteral("stop_time"));
  QDomElement sourceConfiguration = atsWriterConfiguration(sourceDocument);

  if (sourceRecording.isNull() || sourceStartDate.isNull() || sourceStartTime.isNull() || sourceStopDate.isNull() || sourceStopTime.isNull() ||
      sourceConfiguration.isNull()) {
    logText->append(QStringLiteral("Source measdoc has no expected recording/ATSWriter structure."));
    statusBar()->showMessage(QStringLiteral("Unsupported measdoc layout."));
    return;
  }

  QHash<QString, QDomElement> sourceChannels = channelsById(sourceConfiguration);
  if (sourceChannels.isEmpty()) {
    logText->append(QStringLiteral("No ATSWriter channels found in measdoc."));
    statusBar()->showMessage(QStringLiteral("No channels found."));
    return;
  }

  const QDateTime measurementStartUtc = parseUtcDateTime(sourceStartDate.text(), sourceStartTime.text());
  if (!measurementStartUtc.isValid()) {
    logText->append(QStringLiteral("Invalid start date/time in measdoc."));
    statusBar()->showMessage(QStringLiteral("Invalid start date/time."));
    return;
  }

  const QDateTime measurementStopUtc = parseUtcDateTime(sourceStopDate.text(), sourceStopTime.text());
  if (!measurementStopUtc.isValid()) {
    logText->append(QStringLiteral("Invalid stop date/time in measdoc."));
    statusBar()->showMessage(QStringLiteral("Invalid stop date/time."));
    return;
  }

  const QHash<QString, QString> sourceAtsFiles = atsFilesByChannel(measdocFile, &errorMessage);
  if (sourceAtsFiles.isEmpty()) {
    logText->append(errorMessage.isEmpty() ? QStringLiteral("No ATS files found in measdoc.") : errorMessage);
    statusBar()->showMessage(QStringLiteral("No ATS files found."));
    return;
  }

  QMap<QString, qint64> totalSamplesByChannel;
  QMap<QString, double> sampleFreqByChannel;
  qint64 totalSamplesReference = -1;

  for (auto it = sourceChannels.constBegin(); it != sourceChannels.constEnd(); ++it) {
    const QDomElement sampleFreqElement = childElement(it.value(), QStringLiteral("sample_freq"));

    if (!sourceAtsFiles.contains(it.key())) {
      logText->append(QStringLiteral("Missing ats_data_file for channel %1").arg(it.key()));
      statusBar()->showMessage(QStringLiteral("Missing ATS mapping."));
      return;
    }

    qint64 samples = 0;
    const QFileInfo sourceAtsFile(measdocFile.dir(), sourceAtsFiles.value(it.key()));
    if (!atsSamplesFromFile(sourceAtsFile.absoluteFilePath(), samples, &errorMessage)) {
      logText->append(errorMessage);
      statusBar()->showMessage(QStringLiteral("Could not read ATS samples."));
      return;
    }

    bool freqOk = false;
    const double sampleFreq = sampleFreqElement.text().toDouble(&freqOk);
    if (!freqOk || sampleFreq <= 0.0) {
      logText->append(QStringLiteral("Invalid sample_freq for channel %1").arg(it.key()));
      statusBar()->showMessage(QStringLiteral("Invalid sample_freq."));
      return;
    }

    totalSamplesByChannel.insert(it.key(), samples);
    sampleFreqByChannel.insert(it.key(), sampleFreq);

    if (totalSamplesReference < 0) {
      totalSamplesReference = samples;
    } else {
      totalSamplesReference = qMin(totalSamplesReference, samples);
    }

    const QDomElement numSamplesElement = childElement(it.value(), QStringLiteral("num_samples"));
    if (!numSamplesElement.isNull()) {
      bool xmlSamplesOk = false;
      const qint64 xmlSamples = numSamplesElement.text().toLongLong(&xmlSamplesOk);
      if (xmlSamplesOk && xmlSamples != samples) {
        logText->append(QStringLiteral("Warning: XML num_samples (%1) differs from ATS size (%2) for channel %3")
                            .arg(xmlSamples)
                            .arg(samples)
                            .arg(it.key()));
      }
    }
  }

  if (totalSamplesReference <= 0) {
    logText->append(QStringLiteral("No positive sample count found."));
    statusBar()->showMessage(QStringLiteral("No data samples found."));
    return;
  }

  const qint64 fullSplits = totalSamplesReference / chunkSamples;
  const qint64 remainderSamples = totalSamplesReference % chunkSamples;
  const qint64 numSplits = fullSplits + (remainderSamples > 0 ? 1 : 0);
  if (numSplits <= 0) {
    logText->append(QStringLiteral("Could not calculate split chunks."));
    statusBar()->showMessage(QStringLiteral("Split calculation failed."));
    return;
  }

  QDir baseDirectory(measdocFile.absolutePath());
  if (!baseDirectory.mkpath(QStringLiteral("tmp"))) {
    logText->append(QStringLiteral("Could not create tmp directory in: %1").arg(baseDirectory.absolutePath()));
    statusBar()->showMessage(QStringLiteral("Could not create tmp folder."));
    return;
  }

  QDir tmpDirectory(baseDirectory.filePath(QStringLiteral("tmp")));

  logText->append(QStringLiteral("split started: %1 chunk(s), %2 bytes/chunk (%3 samples/chunk)")
                      .arg(numSplits)
                      .arg(chunkBytes)
                      .arg(chunkSamples));
  if (remainderSamples > 0) {
    logText->append(QStringLiteral("Last slice will be smaller: %1 samples.").arg(remainderSamples));
  }

  for (qint64 splitIndex = 0; splitIndex < numSplits; ++splitIndex) {
    const qint64 startSample = splitIndex * chunkSamples;
    const qint64 sampleCount = qMin(chunkSamples, totalSamplesReference - startSample);
    if (sampleCount <= 0) {
      continue;
    }

    const int runIndex = static_cast<int>(splitIndex);
    QDomDocument splitDocument = sourceDocument;

    QDomElement splitRecording = recordingElement(splitDocument);
    QDomElement splitStartDate = childElement(splitRecording, QStringLiteral("start_date"));
    QDomElement splitStartTime = childElement(splitRecording, QStringLiteral("start_time"));
    QDomElement splitStopDate = childElement(splitRecording, QStringLiteral("stop_date"));
    QDomElement splitStopTime = childElement(splitRecording, QStringLiteral("stop_time"));
    QDomElement splitConfiguration = atsWriterConfiguration(splitDocument);
    QHash<QString, QDomElement> splitChannels = channelsById(splitConfiguration);

    const double refSampleFreq = sampleFreqByChannel.first();
    const qint64 startSecs = static_cast<qint64>(startSample / refSampleFreq);
    const qint64 durationSecs = static_cast<qint64>(sampleCount / refSampleFreq);

    const QDateTime splitStartUtc = measurementStartUtc.addSecs(startSecs);
    const QDateTime splitStopUtc = splitStartUtc.addSecs(durationSecs);

    const QString measDirName = QStringLiteral("meas_%1_%2")
                                    .arg(splitStartUtc.date().toString(QStringLiteral("yyyy-MM-dd")),
                                         splitStartUtc.time().toString(QStringLiteral("HH-mm-ss")));
    if (!tmpDirectory.mkpath(measDirName)) {
      logText->append(QStringLiteral("Could not create measdir in tmp: %1").arg(measDirName));
      statusBar()->showMessage(QStringLiteral("Could not create measdir."));
      return;
    }
    QDir outputDirectory(tmpDirectory.filePath(measDirName));

    setElementText(splitStartDate, splitStartUtc.date().toString(QStringLiteral("yyyy-MM-dd")));
    setElementText(splitStartTime, splitStartUtc.time().toString(QStringLiteral("HH:mm:ss")));
    setElementText(splitStopDate, splitStopUtc.date().toString(QStringLiteral("yyyy-MM-dd")));
    setElementText(splitStopTime, splitStopUtc.time().toString(QStringLiteral("HH:mm:ss")));

    const QString targetXmlName = renamedXmlFileForRun(measdocFile.fileName(), runIndex, splitStartUtc, splitStopUtc);

    for (auto it = splitChannels.begin(); it != splitChannels.end(); ++it) {
      QDomElement numSamplesElement = childElement(it.value(), QStringLiteral("num_samples"));
      setElementText(numSamplesElement, QString::number(sampleCount));

      QDomElement atsDataFileElement = childElement(it.value(), QStringLiteral("ats_data_file"));
      if (!atsDataFileElement.isNull()) {
        const QString oldName = atsDataFileElement.text();
        setElementText(atsDataFileElement, renamedAtsFileForRun(oldName, runIndex));
      }

      QDomElement channelStartDate = childElement(it.value(), QStringLiteral("start_date"));
      QDomElement channelStartTime = childElement(it.value(), QStringLiteral("start_time"));
      QDomElement channelStopDate = childElement(it.value(), QStringLiteral("stop_date"));
      QDomElement channelStopTime = childElement(it.value(), QStringLiteral("stop_time"));
      if (!channelStartDate.isNull()) {
        setElementText(channelStartDate, splitStartUtc.date().toString(QStringLiteral("yyyy-MM-dd")));
      }
      if (!channelStartTime.isNull()) {
        setElementText(channelStartTime, splitStartUtc.time().toString(QStringLiteral("HH:mm:ss")));
      }
      if (!channelStopDate.isNull()) {
        setElementText(channelStopDate, splitStopUtc.date().toString(QStringLiteral("yyyy-MM-dd")));
      }
      if (!channelStopTime.isNull()) {
        setElementText(channelStopTime, splitStopUtc.time().toString(QStringLiteral("HH:mm:ss")));
      }
    }

    for (auto it = sourceAtsFiles.constBegin(); it != sourceAtsFiles.constEnd(); ++it) {
      if (!splitChannels.contains(it.key())) {
        continue;
      }

      const QFileInfo sourceAtsFile(measdocFile.dir(), it.value());
      const QString targetAtsName = renamedAtsFileForRun(it.value(), runIndex);
      const QString targetAtsPath = outputDirectory.filePath(targetAtsName);

      if (QFileInfo::exists(targetAtsPath) && !QFile::remove(targetAtsPath)) {
        logText->append(QStringLiteral("Could not replace target ATS file: %1").arg(targetAtsPath));
        statusBar()->showMessage(QStringLiteral("Split failed."));
        return;
      }

      if (!copyAtsRange(sourceAtsFile, targetAtsPath, startSample, sampleCount,
                        static_cast<quint32>(splitStartUtc.toSecsSinceEpoch()), targetXmlName, &errorMessage)) {
        logText->append(errorMessage);
        statusBar()->showMessage(QStringLiteral("Split failed."));
        return;
      }
    }

    const QFileInfo targetXmlFile(outputDirectory.filePath(targetXmlName));
    if (!saveXmlDocument(targetXmlFile, splitDocument, &errorMessage)) {
      logText->append(errorMessage);
      statusBar()->showMessage(QStringLiteral("Split failed."));
      return;
    }

    logText->append(QStringLiteral("Created split run %1: %2 (%3 samples)")
                        .arg(runNumberPadded(runIndex), targetXmlName)
                        .arg(sampleCount));
    logText->append(QStringLiteral("  output dir: %1").arg(outputDirectory.absolutePath()));
  }

  logText->append(QStringLiteral("split finished, output in subdirectories of: %1").arg(tmpDirectory.absolutePath()));
  statusBar()->showMessage(QStringLiteral("split finished."));
}
