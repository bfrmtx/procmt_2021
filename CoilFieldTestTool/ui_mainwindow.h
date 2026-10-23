/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow {
public:
  QWidget *centralWidget;
  QVBoxLayout *verticalLayout;
  QFrame *frame;
  QHBoxLayout *horizontalLayout_2;
  QPushButton *pbOpenData;
  QPushButton *pbCloseData;
  QPushButton *pbSave;
  QFrame *line;
  QFrame *frame_5;
  QHBoxLayout *horizontalLayout_5;
  QLabel *label_2;
  QComboBox *cbCalType;
  QFrame *line_2;
  QPushButton *pbComputeTF;
  QPushButton *pbShowPlot;
  QSpacerItem *horizontalSpacer;
  QFrame *frame_2;
  QHBoxLayout *horizontalLayout;
  QTableWidget *tableTSFiles;
  QFrame *frameProgress;
  QHBoxLayout *horizontalLayout_4;
  QLabel *labProgress;
  QProgressBar *progress;
  QFrame *frame_4;
  QMenuBar *menuBar;
  QToolBar *mainToolBar;
  QStatusBar *statusBar;

  void setupUi(QMainWindow *MainWindow) {
    if (MainWindow->objectName().isEmpty())
      MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
    MainWindow->resize(1560, 755);
    MainWindow->setMinimumSize(QSize(800, 600));
    MainWindow->setAcceptDrops(true);
    centralWidget = new QWidget(MainWindow);
    centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
    verticalLayout = new QVBoxLayout(centralWidget);
    verticalLayout->setSpacing(6);
    verticalLayout->setContentsMargins(11, 11, 11, 11);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    frame = new QFrame(centralWidget);
    frame->setObjectName(QString::fromUtf8("frame"));
    frame->setFrameShape(QFrame::Panel);
    frame->setFrameShadow(QFrame::Raised);
    horizontalLayout_2 = new QHBoxLayout(frame);
    horizontalLayout_2->setSpacing(6);
    horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
    horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
    pbOpenData = new QPushButton(frame);
    pbOpenData->setObjectName(QString::fromUtf8("pbOpenData"));
    pbOpenData->setMinimumSize(QSize(150, 0));
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/new/Icons/pics/OpenFolder.png"), QSize(), QIcon::Normal, QIcon::Off);
    pbOpenData->setIcon(icon);
    pbOpenData->setIconSize(QSize(30, 30));

    horizontalLayout_2->addWidget(pbOpenData);

    pbCloseData = new QPushButton(frame);
    pbCloseData->setObjectName(QString::fromUtf8("pbCloseData"));
    pbCloseData->setMinimumSize(QSize(150, 0));
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/new/Icons/pics/CloseFolder.png"), QSize(), QIcon::Normal, QIcon::Off);
    pbCloseData->setIcon(icon1);
    pbCloseData->setIconSize(QSize(30, 30));

    horizontalLayout_2->addWidget(pbCloseData);

    pbSave = new QPushButton(frame);
    pbSave->setObjectName(QString::fromUtf8("pbSave"));
    pbSave->setMinimumSize(QSize(150, 0));
    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/new/Icons/pics/Save.png"), QSize(), QIcon::Normal, QIcon::Off);
    pbSave->setIcon(icon2);
    pbSave->setIconSize(QSize(30, 30));

    horizontalLayout_2->addWidget(pbSave);

    line = new QFrame(frame);
    line->setObjectName(QString::fromUtf8("line"));
    line->setFrameShadow(QFrame::Raised);
    line->setFrameShape(QFrame::VLine);

    horizontalLayout_2->addWidget(line);

    frame_5 = new QFrame(frame);
    frame_5->setObjectName(QString::fromUtf8("frame_5"));
    frame_5->setMinimumSize(QSize(160, 0));
    frame_5->setFrameShape(QFrame::Panel);
    frame_5->setFrameShadow(QFrame::Raised);
    horizontalLayout_5 = new QHBoxLayout(frame_5);
    horizontalLayout_5->setSpacing(6);
    horizontalLayout_5->setContentsMargins(11, 11, 11, 11);
    horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
    label_2 = new QLabel(frame_5);
    label_2->setObjectName(QString::fromUtf8("label_2"));

    horizontalLayout_5->addWidget(label_2);

    cbCalType = new QComboBox(frame_5);
    cbCalType->addItem(QString());
    cbCalType->addItem(QString());
    cbCalType->addItem(QString());
    cbCalType->setObjectName(QString::fromUtf8("cbCalType"));

    horizontalLayout_5->addWidget(cbCalType);

    horizontalLayout_2->addWidget(frame_5);

    line_2 = new QFrame(frame);
    line_2->setObjectName(QString::fromUtf8("line_2"));
    line_2->setFrameShadow(QFrame::Raised);
    line_2->setFrameShape(QFrame::VLine);

    horizontalLayout_2->addWidget(line_2);

    pbComputeTF = new QPushButton(frame);
    pbComputeTF->setObjectName(QString::fromUtf8("pbComputeTF"));
    pbComputeTF->setMinimumSize(QSize(150, 0));
    QIcon icon3;
    icon3.addFile(QString::fromUtf8(":/new/Icons/pics/Button_StartNow.png"), QSize(), QIcon::Normal, QIcon::Off);
    pbComputeTF->setIcon(icon3);
    pbComputeTF->setIconSize(QSize(30, 30));

    horizontalLayout_2->addWidget(pbComputeTF);

    pbShowPlot = new QPushButton(frame);
    pbShowPlot->setObjectName(QString::fromUtf8("pbShowPlot"));
    pbShowPlot->setMinimumSize(QSize(150, 0));
    QIcon icon4;
    icon4.addFile(QString::fromUtf8(":/new/Icons/pics/ShowPlot.png"), QSize(), QIcon::Normal, QIcon::Off);
    pbShowPlot->setIcon(icon4);
    pbShowPlot->setIconSize(QSize(30, 30));

    horizontalLayout_2->addWidget(pbShowPlot);

    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_2->addItem(horizontalSpacer);

    verticalLayout->addWidget(frame);

    frame_2 = new QFrame(centralWidget);
    frame_2->setObjectName(QString::fromUtf8("frame_2"));
    frame_2->setFrameShape(QFrame::StyledPanel);
    frame_2->setFrameShadow(QFrame::Raised);
    horizontalLayout = new QHBoxLayout(frame_2);
    horizontalLayout->setSpacing(6);
    horizontalLayout->setContentsMargins(11, 11, 11, 11);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    tableTSFiles = new QTableWidget(frame_2);
    if (tableTSFiles->columnCount() < 9)
      tableTSFiles->setColumnCount(9);
    QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
    tableTSFiles->setHorizontalHeaderItem(0, __qtablewidgetitem);
    QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
    tableTSFiles->setHorizontalHeaderItem(1, __qtablewidgetitem1);
    QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
    tableTSFiles->setHorizontalHeaderItem(2, __qtablewidgetitem2);
    QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
    tableTSFiles->setHorizontalHeaderItem(3, __qtablewidgetitem3);
    QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
    tableTSFiles->setHorizontalHeaderItem(4, __qtablewidgetitem4);
    QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
    tableTSFiles->setHorizontalHeaderItem(5, __qtablewidgetitem5);
    QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
    tableTSFiles->setHorizontalHeaderItem(6, __qtablewidgetitem6);
    QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
    tableTSFiles->setHorizontalHeaderItem(7, __qtablewidgetitem7);
    QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
    tableTSFiles->setHorizontalHeaderItem(8, __qtablewidgetitem8);
    tableTSFiles->setObjectName(QString::fromUtf8("tableTSFiles"));
    tableTSFiles->setAcceptDrops(true);
    tableTSFiles->setDragEnabled(true);
    tableTSFiles->setDragDropMode(QAbstractItemView::DragDrop);

    horizontalLayout->addWidget(tableTSFiles);

    verticalLayout->addWidget(frame_2);

    frameProgress = new QFrame(centralWidget);
    frameProgress->setObjectName(QString::fromUtf8("frameProgress"));
    frameProgress->setFrameShape(QFrame::Box);
    frameProgress->setFrameShadow(QFrame::Raised);
    horizontalLayout_4 = new QHBoxLayout(frameProgress);
    horizontalLayout_4->setSpacing(6);
    horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
    horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
    labProgress = new QLabel(frameProgress);
    labProgress->setObjectName(QString::fromUtf8("labProgress"));
    labProgress->setMinimumSize(QSize(150, 0));
    labProgress->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);

    horizontalLayout_4->addWidget(labProgress);

    progress = new QProgressBar(frameProgress);
    progress->setObjectName(QString::fromUtf8("progress"));
    progress->setAutoFillBackground(true);
    progress->setValue(24);
    progress->setTextVisible(false);

    horizontalLayout_4->addWidget(progress);

    verticalLayout->addWidget(frameProgress);

    frame_4 = new QFrame(centralWidget);
    frame_4->setObjectName(QString::fromUtf8("frame_4"));
    frame_4->setFrameShape(QFrame::StyledPanel);
    frame_4->setFrameShadow(QFrame::Raised);

    verticalLayout->addWidget(frame_4);

    MainWindow->setCentralWidget(centralWidget);
    menuBar = new QMenuBar(MainWindow);
    menuBar->setObjectName(QString::fromUtf8("menuBar"));
    menuBar->setGeometry(QRect(0, 0, 1560, 34));
    MainWindow->setMenuBar(menuBar);
    mainToolBar = new QToolBar(MainWindow);
    mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
    MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
    statusBar = new QStatusBar(MainWindow);
    statusBar->setObjectName(QString::fromUtf8("statusBar"));
    MainWindow->setStatusBar(statusBar);

    retranslateUi(MainWindow);

    cbCalType->setCurrentIndex(0);

    QMetaObject::connectSlotsByName(MainWindow);
  } // setupUi

  void retranslateUi(QMainWindow *MainWindow) {
    MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
    pbOpenData->setText(QCoreApplication::translate("MainWindow", "Open TS", nullptr));
    pbCloseData->setText(QCoreApplication::translate("MainWindow", "Close TS", nullptr));
    pbSave->setText(QCoreApplication::translate("MainWindow", "Save", nullptr));
    label_2->setText(QCoreApplication::translate("MainWindow", "Cal Type", nullptr));
    cbCalType->setItemText(0, QCoreApplication::translate("MainWindow", "auto", nullptr));
    cbCalType->setItemText(1, QCoreApplication::translate("MainWindow", "theo", nullptr));
    cbCalType->setItemText(2, QCoreApplication::translate("MainWindow", "off", nullptr));

    pbComputeTF->setText(QCoreApplication::translate("MainWindow", "Compute PST", nullptr));
    pbShowPlot->setText(QCoreApplication::translate("MainWindow", "Show Plot", nullptr));
    QTableWidgetItem *___qtablewidgetitem = tableTSFiles->horizontalHeaderItem(0);
    ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "Use for TF", nullptr));
    QTableWidgetItem *___qtablewidgetitem1 = tableTSFiles->horizontalHeaderItem(1);
    ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "Sample Freq. [Hz]", nullptr));
    QTableWidgetItem *___qtablewidgetitem2 = tableTSFiles->horizontalHeaderItem(2);
    ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "Calibration Freq. [Hz]", nullptr));
    QTableWidgetItem *___qtablewidgetitem3 = tableTSFiles->horizontalHeaderItem(3);
    ___qtablewidgetitem3->setText(QCoreApplication::translate("MainWindow", "Sensor Ex", nullptr));
    QTableWidgetItem *___qtablewidgetitem4 = tableTSFiles->horizontalHeaderItem(4);
    ___qtablewidgetitem4->setText(QCoreApplication::translate("MainWindow", "Sensor Ey", nullptr));
    QTableWidgetItem *___qtablewidgetitem5 = tableTSFiles->horizontalHeaderItem(5);
    ___qtablewidgetitem5->setText(QCoreApplication::translate("MainWindow", "Sensor Hx", nullptr));
    QTableWidgetItem *___qtablewidgetitem6 = tableTSFiles->horizontalHeaderItem(6);
    ___qtablewidgetitem6->setText(QCoreApplication::translate("MainWindow", "Sensor Hy", nullptr));
    QTableWidgetItem *___qtablewidgetitem7 = tableTSFiles->horizontalHeaderItem(7);
    ___qtablewidgetitem7->setText(QCoreApplication::translate("MainWindow", "Sensor Hz", nullptr));
    QTableWidgetItem *___qtablewidgetitem8 = tableTSFiles->horizontalHeaderItem(8);
    ___qtablewidgetitem8->setText(QCoreApplication::translate("MainWindow", "Target Directory", nullptr));
    labProgress->setText(QString());
    progress->setFormat(QString());
  } // retranslateUi
};

namespace Ui {
class MainWindow : public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
