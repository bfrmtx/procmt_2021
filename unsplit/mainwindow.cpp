#include "mainwindow.h"

#include <QDir>
#include <QDirIterator>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QEvent>
#include <QFileDialog>

#include <QDateTime>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QLabel>
#include <QMimeData>
#include <QPushButton>
#include <QRegularExpression>
#include <QStatusBar>
#include <QTextEdit>
#include <QTextStream>
#include <QUrl>
#include <QVBoxLayout>
#include <QtConcurrent>
#include <QtEndian>

#include <cstdint>
#include <limits>

namespace {
QString directoryFromMimeData(const QMimeData *mimeData) {
  if (!mimeData->hasUrls()) {
    return QString();
  }

  for (const QUrl &url : mimeData->urls()) {
    const QFileInfo fileInfo(url.toLocalFile());
    if (url.isLocalFile() && fileInfo.isDir()) {
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

QString fileTime(const QString &xmlTime) {
  QString time = xmlTime;
  return time.replace(QChar(':'), QChar('-'));
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

bool appendAtsPayload(const QFileInfo &sourceAtsFile, QFile &targetAtsFile, QString *errorMessage) {
  QFile source(sourceAtsFile.absoluteFilePath());
  if (!source.open(QIODevice::ReadOnly)) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("Could not open source ATS file: %1").arg(sourceAtsFile.absoluteFilePath());
    }
    return false;
  }

  if (!source.seek(1024)) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("Could not skip ATS header in: %1").arg(sourceAtsFile.absoluteFilePath());
    }
    return false;
  }

  QByteArray buffer;
  buffer.resize(1024 * 1024);
  while (!source.atEnd()) {
    const qint64 bytesRead = source.read(buffer.data(), buffer.size());
    if (bytesRead < 0) {
      if (errorMessage != nullptr) {
        *errorMessage = QStringLiteral("Could not read ATS file: %1").arg(sourceAtsFile.absoluteFilePath());
      }
      return false;
    }

    if (targetAtsFile.write(buffer.constData(), bytesRead) != bytesRead) {
      if (errorMessage != nullptr) {
        *errorMessage = QStringLiteral("Could not append to ATS file: %1").arg(targetAtsFile.fileName());
      }
      return false;
    }
  }

  return true;
}

bool updateAtsSamples(const QString &targetAtsPath, QString *errorMessage) {
  QFile targetAtsFile(targetAtsPath);
  if (!targetAtsFile.open(QIODevice::ReadWrite)) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("Could not open ATS file for sample header update: %1").arg(targetAtsPath);
    }
    return false;
  }

  const qint64 payloadBytes = targetAtsFile.size() - 1024;
  if (payloadBytes < 0 || (payloadBytes % 4) != 0) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("ATS file has invalid payload size: %1").arg(targetAtsPath);
    }
    return false;
  }

  const qint64 samples = payloadBytes / 4;
  if (samples < 0) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("ATS sample count is invalid: %1").arg(targetAtsPath);
    }
    return false;
  }

  const bool needs64BitSamples = samples >= std::numeric_limits<std::uint32_t>::max();
  const std::uint32_t samples32 = needs64BitSamples ? std::numeric_limits<std::uint32_t>::max() : static_cast<std::uint32_t>(samples);
  const std::uint32_t littleEndianSamples = qToLittleEndian(samples32);
  if (!targetAtsFile.seek(0x0004)) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("Could not seek to ATS sample field: %1").arg(targetAtsPath);
    }
    return false;
  }

  const auto *sampleBytes = reinterpret_cast<const char *>(&littleEndianSamples);
  if (targetAtsFile.write(sampleBytes, sizeof(littleEndianSamples)) != qsizetype(sizeof(littleEndianSamples))) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("Could not write ATS sample field: %1").arg(targetAtsPath);
    }
    return false;
  }

  if (needs64BitSamples) {
    const std::uint64_t littleEndianSamples64 = qToLittleEndian(static_cast<std::uint64_t>(samples));
    if (!targetAtsFile.seek(0x00F0)) {
      if (errorMessage != nullptr) {
        *errorMessage = QStringLiteral("Could not seek to ATS 64-bit sample field: %1").arg(targetAtsPath);
      }
      return false;
    }

    const auto *sample64Bytes = reinterpret_cast<const char *>(&littleEndianSamples64);
    if (targetAtsFile.write(sample64Bytes, sizeof(littleEndianSamples64)) != qsizetype(sizeof(littleEndianSamples64))) {
      if (errorMessage != nullptr) {
        *errorMessage = QStringLiteral("Could not write ATS 64-bit sample field: %1").arg(targetAtsPath);
      }
      return false;
    }
  }

  return true;
}

bool moveFile(const QFileInfo &sourceFile, const QString &targetPath, QString *errorMessage) {
  if (QFileInfo::exists(targetPath)) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("Target already exists: %1").arg(targetPath);
    }
    return false;
  }

  if (!QFile::rename(sourceFile.absoluteFilePath(), targetPath)) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("Could not move %1 to %2").arg(sourceFile.absoluteFilePath(), targetPath);
    }
    return false;
  }

  return true;
}
} // namespace

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle(QStringLiteral("unsplit"));
  resize(640, 740);
  setAcceptDrops(true);

  auto *central = new QWidget(this);
  central->setAcceptDrops(true);
  central->installEventFilter(this);

  auto *layout = new QVBoxLayout(central);

  dropLabel = new QLabel(QStringLiteral("Drop a directory here"), central);
  dropLabel->setAcceptDrops(true);
  dropLabel->installEventFilter(this);
  dropLabel->setAlignment(Qt::AlignCenter);
  dropLabel->setMinimumHeight(120);
  dropLabel->setFrameShape(QFrame::StyledPanel);

  directoryLabel = new QLabel(QStringLiteral("No directory selected"), central);
  directoryLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

  auto *chooseButton = new QPushButton(QStringLiteral("Choose directory..."), central);
  scanButton = new QPushButton(QStringLiteral("scan"), central);
  scanButton->setEnabled(false);
  unsplitButton = new QPushButton(QStringLiteral("unsplit"), central);
  unsplitButton->setEnabled(false);
  swapFinallyButton = new QPushButton(QStringLiteral("swap finally?"), central);
  swapFinallyButton->setEnabled(false);

  logText = new QTextEdit(central);
  logText->setAcceptDrops(false);
  logText->viewport()->setAcceptDrops(false);
  logText->setReadOnly(true);

  layout->addWidget(dropLabel);
  layout->addWidget(directoryLabel);
  layout->addWidget(chooseButton);
  layout->addWidget(scanButton);
  layout->addWidget(unsplitButton);
  layout->addWidget(swapFinallyButton);
  layout->addWidget(logText, 1);

  setCentralWidget(central);
  statusBar()->showMessage(QStringLiteral("Drop a directory to begin."));

  connect(chooseButton, &QPushButton::clicked, this, &MainWindow::chooseDirectory);
  connect(scanButton, &QPushButton::clicked, this, &MainWindow::scanDirectory);
  connect(unsplitButton, &QPushButton::clicked, this, &MainWindow::unsplitDirectory);
  connect(swapFinallyButton, &QPushButton::clicked, this, &MainWindow::swapFinally);
  connect(&scanWatcher, &QFutureWatcher<QList<runs>>::finished, this, &MainWindow::scanFinished);
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
      const QString directory = directoryFromMimeData(dropEvent->mimeData());
      if (!directory.isEmpty()) {
        setDirectory(directory);
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
  const QString directory = droppedDirectory(event);
  if (directory.isEmpty()) {
    event->ignore();
    return;
  }

  setDirectory(directory);
  event->acceptProposedAction();
}

void MainWindow::chooseDirectory() {
  const QString directory = QFileDialog::getExistingDirectory(this, QStringLiteral("Choose directory"), directoryPath);
  if (!directory.isEmpty()) {
    setDirectory(directory);
  }
}

void MainWindow::scanDirectory() {
  if (directoryPath.isEmpty()) {
    statusBar()->showMessage(QStringLiteral("No directory selected."));
    return;
  }

  if (scanWatcher.isRunning()) {
    statusBar()->showMessage(QStringLiteral("Scan is already running."));
    return;
  }

  logText->append(QStringLiteral("scan requested for: %1").arg(directoryPath));
  logText->append(QStringLiteral("Scanning XML files..."));
  statusBar()->showMessage(QStringLiteral("Scanning XML files..."));
  scanButton->setEnabled(false);
  unsplitButton->setEnabled(false);

  const QString scanDirectory = directoryPath;
  scanWatcher.setFuture(QtConcurrent::run([scanDirectory]() { return scanXmlFiles(scanDirectory); }));
}

void MainWindow::unsplitDirectory() {
  if (allRuns.isEmpty()) {
    statusBar()->showMessage(QStringLiteral("Scan first before unsplit."));
    return;
  }

  QDir baseDirectory(directoryPath);
  if (!baseDirectory.mkpath(QStringLiteral("tmp"))) {
    logText->append(QStringLiteral("Could not create tmp directory in: %1").arg(directoryPath));
    statusBar()->showMessage(QStringLiteral("Could not create tmp directory."));
    return;
  }

  QDir tmpDirectory(baseDirectory.filePath(QStringLiteral("tmp")));

  logText->append(QStringLiteral("unsplit started for %1 run group(s).").arg(allRuns.size()));
  for (runs &runGroup : allRuns) {
    if (runGroup.list().isEmpty()) {
      logText->append(QStringLiteral("Skipped empty run group: %1 ... %2").arg(runGroup.startNumber(), runGroup.endingPattern()));
      continue;
    }

    const QFileInfo sourceXmlFile = runGroup.list().first().xmlfile;
    const QString targetXmlPath = tmpDirectory.filePath(sourceXmlFile.fileName());

    if (QFileInfo::exists(targetXmlPath) && !QFile::remove(targetXmlPath)) {
      logText->append(QStringLiteral("Could not replace target XML: %1").arg(targetXmlPath));
      continue;
    }

    if (!QFile::copy(sourceXmlFile.absoluteFilePath(), targetXmlPath)) {
      logText->append(QStringLiteral("Could not copy XML: %1").arg(sourceXmlFile.absoluteFilePath()));
      continue;
    }

    runGroup.targetXmlFile = QFileInfo(targetXmlPath);
    logText->append(QStringLiteral("Target XML for %1 ... %2: %3")
                        .arg(runGroup.startNumber(), runGroup.endingPattern(), runGroup.targetXmlFile.fileName()));

    QString errorMessage;
    if (!runGroup.unsplit_xml(&errorMessage)) {
      logText->append(errorMessage);
      continue;
    }

    logText->append(QStringLiteral("Updated target XML: %1").arg(runGroup.targetXmlFile.fileName()));

    if (!runGroup.unsplit_ats(&errorMessage)) {
      logText->append(errorMessage);
      continue;
    }

    logText->append(QStringLiteral("Concatenated ATS files for %1 ... %2").arg(runGroup.startNumber(), runGroup.endingPattern()));
  }

  statusBar()->showMessage(QStringLiteral("Concatenated ATS FINISHED"));
}

void MainWindow::swapFinally() {
  if (directoryPath.isEmpty()) {
    statusBar()->showMessage(QStringLiteral("No directory selected."));
    return;
  }

  QDir baseDirectory(directoryPath);
  QDir tmpDirectory(baseDirectory.filePath(QStringLiteral("tmp")));
  if (!tmpDirectory.exists()) {
    logText->append(QStringLiteral("No tmp directory found in: %1").arg(directoryPath));
    statusBar()->showMessage(QStringLiteral("No tmp directory found."));
    return;
  }

  QFileInfoList baseFiles = baseDirectory.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
  QStringList baseFileNames;
  for (qsizetype index = baseFiles.size() - 1; index >= 0; --index) {
    if (baseFiles.at(index).suffix().compare(QStringLiteral("kml"), Qt::CaseInsensitive) == 0) {
      baseFiles.removeAt(index);
    } else {
      baseFileNames.append(baseFiles.at(index).fileName());
    }
  }

  const QFileInfoList tmpFiles = tmpDirectory.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
  if (baseFiles.isEmpty() && tmpFiles.isEmpty()) {
    logText->append(QStringLiteral("Nothing to swap."));
    statusBar()->showMessage(QStringLiteral("Nothing to swap."));
    return;
  }

  const QString stagingName = QStringLiteral(".swap-finally-%1").arg(QDateTime::currentMSecsSinceEpoch());
  if (!baseDirectory.mkdir(stagingName)) {
    logText->append(QStringLiteral("Could not create swap staging directory in: %1").arg(directoryPath));
    statusBar()->showMessage(QStringLiteral("Could not create swap staging directory."));
    return;
  }

  QDir stagingDirectory(baseDirectory.filePath(stagingName));
  auto cleanStaging = [&stagingDirectory]() { stagingDirectory.removeRecursively(); };

  for (const QFileInfo &tmpFile : tmpFiles) {
    const QString targetPath = baseDirectory.filePath(tmpFile.fileName());
    if (QFileInfo::exists(targetPath) && !baseFileNames.contains(tmpFile.fileName())) {
      logText->append(QStringLiteral("Cannot move tmp file over target that will stay in place: %1").arg(targetPath));
      cleanStaging();
      statusBar()->showMessage(QStringLiteral("Swap blocked by target path."));
      return;
    }
  }

  QString errorMessage;
  for (const QFileInfo &baseFile : baseFiles) {
    if (!moveFile(baseFile, stagingDirectory.filePath(baseFile.fileName()), &errorMessage)) {
      logText->append(errorMessage);
      logText->append(QStringLiteral("Staging directory kept for manual recovery: %1").arg(stagingDirectory.absolutePath()));
      statusBar()->showMessage(QStringLiteral("Swap failed."));
      return;
    }
  }

  for (const QFileInfo &tmpFile : tmpFiles) {
    if (!moveFile(tmpFile, baseDirectory.filePath(tmpFile.fileName()), &errorMessage)) {
      logText->append(errorMessage);
      logText->append(QStringLiteral("Staging directory kept for manual recovery: %1").arg(stagingDirectory.absolutePath()));
      statusBar()->showMessage(QStringLiteral("Swap failed."));
      return;
    }
  }

  for (const QFileInfo &baseFile : baseFiles) {
    const QFileInfo stagedFile(stagingDirectory.filePath(baseFile.fileName()));
    if (!moveFile(stagedFile, tmpDirectory.filePath(baseFile.fileName()), &errorMessage)) {
      logText->append(errorMessage);
      logText->append(QStringLiteral("Staging directory kept for manual recovery: %1").arg(stagingDirectory.absolutePath()));
      statusBar()->showMessage(QStringLiteral("Swap failed."));
      return;
    }
  }

  cleanStaging();
  logText->append(QStringLiteral("swap finally finished: %1 file(s) moved down, %2 file(s) moved up.").arg(baseFiles.size()).arg(tmpFiles.size()));
  statusBar()->showMessage(QStringLiteral("swap finally finished."));
}

void MainWindow::setDirectory(const QString &newDirectoryPath) {
  directoryPath = newDirectoryPath;
  allRuns.clear();
  directoryLabel->setText(directoryPath);
  scanButton->setEnabled(!scanWatcher.isRunning());
  unsplitButton->setEnabled(false);
  swapFinallyButton->setEnabled(true);
  logText->append(QStringLiteral("Selected directory: %1").arg(directoryPath));
  statusBar()->showMessage(QStringLiteral("Ready."));
}

QString MainWindow::droppedDirectory(const QDropEvent *event) const {
  return directoryFromMimeData(event->mimeData());
}

void MainWindow::runs::addRun(const run &run) {
  const int newRunNumber = runNumber(run);

  for (qsizetype index = 0; index < m_runs.size(); ++index) {
    const int existingRunNumber = runNumber(m_runs.at(index));
    if (existingRunNumber < 0 || (newRunNumber >= 0 && newRunNumber < existingRunNumber)) {
      m_runs.insert(index, run);
      return;
    }
  }

  m_runs.append(run);
}

int MainWindow::runs::runNumber(const run &run) {
  static const QRegularExpression runNumberExpression(QStringLiteral("_R(\\d+)(?:_|$)"));

  const QRegularExpressionMatch match = runNumberExpression.match(run.xmlfile.completeBaseName());
  if (!match.hasMatch()) {
    return -1;
  }

  bool ok = false;
  const int number = match.captured(1).toInt(&ok);
  return ok ? number : -1;
}

bool MainWindow::runs::unsplit_ats(QString *errorMessage) {
  if (m_runs.isEmpty()) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("No runs available for ATS concatenation: %1 ... %2").arg(startNumber(), endingPattern());
    }
    return false;
  }

  if (!targetXmlFile.exists()) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("Missing target XML before ATS concatenation: %1 ... %2").arg(startNumber(), endingPattern());
    }
    return false;
  }

  QDir targetDirectory(targetXmlFile.absolutePath());
  const run &firstRun = m_runs.first();
  const QHash<QString, QString> targetAtsFiles = atsFilesByChannel(firstRun.xmlfile, errorMessage);
  if (targetAtsFiles.isEmpty()) {
    return false;
  }

  for (auto it = targetAtsFiles.constBegin(); it != targetAtsFiles.constEnd(); ++it) {
    const QFileInfo sourceAtsFile(firstRun.xmlfile.dir(), it.value());
    const QString targetAtsPath = targetDirectory.filePath(it.value());

    if (QFileInfo::exists(targetAtsPath) && !QFile::remove(targetAtsPath)) {
      if (errorMessage != nullptr) {
        *errorMessage = QStringLiteral("Could not replace target ATS file: %1").arg(targetAtsPath);
      }
      return false;
    }

    if (!QFile::copy(sourceAtsFile.absoluteFilePath(), targetAtsPath)) {
      if (errorMessage != nullptr) {
        *errorMessage = QStringLiteral("Could not copy ATS file from %1 to %2").arg(sourceAtsFile.absoluteFilePath(), targetAtsPath);
      }
      return false;
    }
  }

  for (qsizetype index = 1; index < m_runs.size(); ++index) {
    const QHash<QString, QString> sourceAtsFiles = atsFilesByChannel(m_runs.at(index).xmlfile, errorMessage);
    if (sourceAtsFiles.isEmpty()) {
      return false;
    }

    for (auto it = targetAtsFiles.constBegin(); it != targetAtsFiles.constEnd(); ++it) {
      if (!sourceAtsFiles.contains(it.key())) {
        continue;
      }

      QFile targetAtsFile(targetDirectory.filePath(it.value()));
      if (!targetAtsFile.open(QIODevice::Append)) {
        if (errorMessage != nullptr) {
          *errorMessage = QStringLiteral("Could not open target ATS file for append: %1").arg(targetAtsFile.fileName());
        }
        return false;
      }

      const QFileInfo sourceAtsFile(m_runs.at(index).xmlfile.dir(), sourceAtsFiles.value(it.key()));
      if (!appendAtsPayload(sourceAtsFile, targetAtsFile, errorMessage)) {
        return false;
      }
    }
  }

  for (auto it = targetAtsFiles.constBegin(); it != targetAtsFiles.constEnd(); ++it) {
    if (!updateAtsSamples(targetDirectory.filePath(it.value()), errorMessage)) {
      return false;
    }
  }

  return true;
}

bool MainWindow::runs::unsplit_xml(QString *errorMessage) {
  if (!targetXmlFile.exists()) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("Missing target XML file for %1 ... %2").arg(startNumber(), endingPattern());
    }
    return false;
  }

  QDomDocument targetDocument;
  if (!loadXmlDocument(targetXmlFile, targetDocument, errorMessage)) {
    return false;
  }

  QDomElement targetRecording = recordingElement(targetDocument);
  QDomElement targetStartDate = childElement(targetRecording, QStringLiteral("start_date"));
  QDomElement targetStartTime = childElement(targetRecording, QStringLiteral("start_time"));
  QDomElement targetStopDate = childElement(targetRecording, QStringLiteral("stop_date"));
  QDomElement targetStopTime = childElement(targetRecording, QStringLiteral("stop_time"));
  QDomElement targetConfiguration = atsWriterConfiguration(targetDocument);
  QHash<QString, QDomElement> targetChannels = channelsById(targetConfiguration);

  if (targetStartDate.isNull() || targetStartTime.isNull() || targetStopDate.isNull() || targetStopTime.isNull() || targetConfiguration.isNull()) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("Target XML has no expected recording/ATSWriter structure: %1").arg(targetXmlFile.absoluteFilePath());
    }
    return false;
  }

  for (qsizetype index = 1; index < m_runs.size(); ++index) {
    QDomDocument sourceDocument;
    if (!loadXmlDocument(m_runs.at(index).xmlfile, sourceDocument, errorMessage)) {
      return false;
    }

    const QDomElement sourceRecording = recordingElement(sourceDocument);
    const QDomElement sourceStopDate = childElement(sourceRecording, QStringLiteral("stop_date"));
    if (!sourceStopDate.isNull()) {
      setElementText(targetStopDate, sourceStopDate.text());
    }

    const QDomElement sourceStopTime = childElement(sourceRecording, QStringLiteral("stop_time"));
    if (!sourceStopTime.isNull()) {
      setElementText(targetStopTime, sourceStopTime.text());
    }

    const QDomElement sourceConfiguration = atsWriterConfiguration(sourceDocument);
    const QHash<QString, QDomElement> sourceChannels = channelsById(sourceConfiguration);

    for (auto it = sourceChannels.constBegin(); it != sourceChannels.constEnd(); ++it) {
      if (!targetChannels.contains(it.key())) {
        continue;
      }

      QDomElement targetNumSamples = childElement(targetChannels.value(it.key()), QStringLiteral("num_samples"));
      const QDomElement sourceNumSamples = childElement(it.value(), QStringLiteral("num_samples"));
      if (targetNumSamples.isNull() || sourceNumSamples.isNull()) {
        continue;
      }

      bool targetOk = false;
      bool sourceOk = false;
      const qint64 targetSamples = targetNumSamples.text().toLongLong(&targetOk);
      const qint64 sourceSamples = sourceNumSamples.text().toLongLong(&sourceOk);
      if (!targetOk || !sourceOk) {
        continue;
      }

      setElementText(targetNumSamples, QString::number(targetSamples + sourceSamples));
    }
  }

  if (!saveXmlDocument(targetXmlFile, targetDocument, errorMessage)) {
    return false;
  }

  const QString targetFileName = QStringLiteral("%1%2_%3_%4_%5_R000%6.xml")
                                     .arg(startNumber(), targetStartDate.text(), fileTime(targetStartTime.text()), targetStopDate.text(),
                                          fileTime(targetStopTime.text()), endingPattern());
  const QString renamedTargetXmlPath = targetXmlFile.dir().filePath(targetFileName);

  if (targetXmlFile.absoluteFilePath() == renamedTargetXmlPath) {
    return true;
  }

  if (QFileInfo::exists(renamedTargetXmlPath) && !QFile::remove(renamedTargetXmlPath)) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("Could not replace renamed target XML: %1").arg(renamedTargetXmlPath);
    }
    return false;
  }

  if (!QFile::rename(targetXmlFile.absoluteFilePath(), renamedTargetXmlPath)) {
    if (errorMessage != nullptr) {
      *errorMessage = QStringLiteral("Could not rename target XML from %1 to %2").arg(targetXmlFile.absoluteFilePath(), renamedTargetXmlPath);
    }
    return false;
  }

  targetXmlFile = QFileInfo(renamedTargetXmlPath);
  return true;
}

QList<MainWindow::runs> MainWindow::scanXmlFiles(const QString &scanDirectory) {
  QList<runs> scannedRuns;

  QDirIterator xmlFiles(scanDirectory, QStringList() << QStringLiteral("*.xml") << QStringLiteral("*.XML"), QDir::Files,
                        QDirIterator::Subdirectories);

  while (xmlFiles.hasNext()) {
    const QFileInfo xmlFile(xmlFiles.next());
    QString startNumber;
    QString endingPattern;

    if (!splitXmlRunName(xmlFile, startNumber, endingPattern)) {
      continue;
    }

    runs *matchingRuns = nullptr;
    for (runs &candidate : scannedRuns) {
      if (candidate.startNumber() == startNumber && candidate.endingPattern() == endingPattern) {
        matchingRuns = &candidate;
        break;
      }
    }

    if (matchingRuns == nullptr) {
      scannedRuns.append(runs(startNumber, endingPattern));
      matchingRuns = &scannedRuns.last();
    }

    run currentRun;
    currentRun.xmlfile = xmlFile;
    matchingRuns->addRun(currentRun);
  }

  return scannedRuns;
}

void MainWindow::scanFinished() {
  allRuns = scanWatcher.result();

  logText->append(QStringLiteral("Found %1 run group(s).").arg(allRuns.size()));
  for (const runs &runGroup : allRuns) {
    logText->append(QStringLiteral("%1 ... %2: %3 XML file(s)")
                        .arg(runGroup.startNumber(), runGroup.endingPattern())
                        .arg(runGroup.list().size()));
  }

  scanButton->setEnabled(!directoryPath.isEmpty());
  unsplitButton->setEnabled(!allRuns.isEmpty());
  // list all XML files found, group by group
  for (const runs &runGroup : allRuns) {
    logText->append(QStringLiteral("Run group: %1 ... %2").arg(runGroup.startNumber(), runGroup.endingPattern()));
    for (const run &run : runGroup.list()) {
      logText->append(QStringLiteral("  %1").arg(run.xmlfile.fileName()));
    }
  }
  statusBar()->showMessage(QStringLiteral("XML scan complete."));
}

bool MainWindow::splitXmlRunName(const QFileInfo &xmlFile, QString &startNumber, QString &endingPattern) {
  static const QRegularExpression runNameExpression(QStringLiteral("^(\\d+_).+(_[^_]+)$"));

  const QRegularExpressionMatch match = runNameExpression.match(xmlFile.completeBaseName());
  if (!match.hasMatch()) {
    return false;
  }

  startNumber = match.captured(1);
  endingPattern = match.captured(2);
  return true;
}