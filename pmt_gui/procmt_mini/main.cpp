#include "procmt_mini.h"
// #include "procmt_lib.h"
#include <QApplication>
#include <QStringList>

int main(int argc, char *argv[]) {

// use OpenGL
#ifdef QCUSTOMPLOT_USE_OPENGL
  Q_INIT_RESOURCE(default_resources);
  QApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);
  // QApplication::setAttribute(Qt::AA_DisableHighDpiScaling, true);
  QSurfaceFormat fmt;
  fmt.setDepthBufferSize(16);
  fmt.setProfile(QSurfaceFormat::CoreProfile);
  fmt.setVersion(4, 1);
  QSurfaceFormat::setDefaultFormat(fmt);
#endif
  QApplication a(argc, argv);
  for (auto screen : QApplication::screens()) {
    qDebug() << "device pxr: " << screen->devicePixelRatio();
  }

  //    for (auto screen : QApplication::screens()) {
  //        qDebug() <<  screen->devicePixelRatio();
  //    }
  //    std::shared_ptr<mt_site> mtsite = std::make_shared<mt_site>();
  //    std::vector<std::shared_ptr<edi_file>> edifiles;
  std::shared_ptr<prc_com> cmdline = std::make_shared<prc_com>();

  std::shared_ptr<msg_logger> msg = std::make_shared<msg_logger>();
  QStringList allfiles;
  QStringList allatsfiles;
  QStringList allmeasdocs;
  cmdline->insert("target_frequencies_table", QString(""));
  cmdline->insert("basedir", QString(""));
  cmdline->scan_cmdline(a.arguments(), allfiles, allatsfiles, allmeasdocs, true, true, true);

  procmt_mini pmtmini(cmdline, msg);

  if (cmdline->contains("basedir")) {
    if (cmdline->svalue("basedir").size()) {
      pmtmini.slot_set_survey_base_dir(cmdline->svalue("basedir"));
    }
  }

  //    if (a.arguments().size()) {
  //        for (const auto &str: a.arguments()) {
  //            if (str.contains("survey")) {
  //                pmtmini.slot_set_survey_base_dir(str);
  //            }
  //        }
  //    }

  pmtmini.show();

  return a.exec();
}
