/********************************************************************************
** Form generated from reading UI file 'plot.ui'
**
** Created by: Qt User Interface Compiler version 5.15.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLOT_H
#define UI_PLOT_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Plot
{
public:
    QHBoxLayout *horizontalLayout_2;
    QTabWidget *qtabPlots;
    QWidget *tab;
    QVBoxLayout *verticalLayout_4;
    QCustomPlot *plotMag;
    QFrame *line;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_3;
    QCustomPlot *plotCoh;
    QFrame *line_2;
    QSpacerItem *horizontalSpacer_2;
    QWidget *tab_3;
    QVBoxLayout *verticalLayout_7;
    QCustomPlot *plotNoise;
    QFrame *line_3;
    QWidget *tab_4;
    QVBoxLayout *verticalLayout_8;
    QCustomPlot *plotCalChopperOnAmpl;
    QCustomPlot *plotCalChopperOnPhase;
    QWidget *tab_5;
    QVBoxLayout *verticalLayout_9;
    QCustomPlot *plotCalChopperOffAmpl;
    QCustomPlot *plotCalChopperOffPhase;
    QFrame *frame;
    QVBoxLayout *verticalLayout;
    QPushButton *pbAutoScale;
    QPushButton *pbPrint;
    QGroupBox *gpGraph1;
    QVBoxLayout *verticalLayout_2;
    QPushButton *pbShowCurveGraph1;
    QPushButton *pbShowDatapointsGraph1;
    QGroupBox *gpGraph2;
    QVBoxLayout *verticalLayout_5;
    QPushButton *pbShowCurveGraph2;
    QPushButton *pbShowDatapointsGraph2;
    QGroupBox *gbGraph3;
    QVBoxLayout *verticalLayout_6;
    QPushButton *pbShowCurveGraph3;
    QPushButton *pbShowDatapointsGraph3;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLabel *label;
    QLabel *label_4;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *labFreq;
    QLabel *labValueGraph1;
    QLabel *labValueGraph2;
    QLabel *labValueGraph3;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QLabel *labDiff1Name;
    QLabel *labDiff1Value;
    QLabel *labDiff2Value;
    QLabel *labDiff2Name;
    QSpacerItem *verticalSpacer;

    void setupUi(QDialog *Plot)
    {
        if (Plot->objectName().isEmpty())
            Plot->setObjectName(QString::fromUtf8("Plot"));
        Plot->resize(1339, 882);
        Plot->setMinimumSize(QSize(1024, 600));
        Plot->setBaseSize(QSize(1200, 800));
        Plot->setSizeGripEnabled(true);
        horizontalLayout_2 = new QHBoxLayout(Plot);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        qtabPlots = new QTabWidget(Plot);
        qtabPlots->setObjectName(QString::fromUtf8("qtabPlots"));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        verticalLayout_4 = new QVBoxLayout(tab);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        plotMag = new QCustomPlot(tab);
        plotMag->setObjectName(QString::fromUtf8("plotMag"));

        verticalLayout_4->addWidget(plotMag);

        line = new QFrame(tab);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout_4->addWidget(line);

        qtabPlots->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        verticalLayout_3 = new QVBoxLayout(tab_2);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        plotCoh = new QCustomPlot(tab_2);
        plotCoh->setObjectName(QString::fromUtf8("plotCoh"));

        verticalLayout_3->addWidget(plotCoh);

        line_2 = new QFrame(tab_2);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        verticalLayout_3->addWidget(line_2);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        verticalLayout_3->addItem(horizontalSpacer_2);

        qtabPlots->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        verticalLayout_7 = new QVBoxLayout(tab_3);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        plotNoise = new QCustomPlot(tab_3);
        plotNoise->setObjectName(QString::fromUtf8("plotNoise"));

        verticalLayout_7->addWidget(plotNoise);

        line_3 = new QFrame(tab_3);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);

        verticalLayout_7->addWidget(line_3);

        qtabPlots->addTab(tab_3, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        verticalLayout_8 = new QVBoxLayout(tab_4);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        plotCalChopperOnAmpl = new QCustomPlot(tab_4);
        plotCalChopperOnAmpl->setObjectName(QString::fromUtf8("plotCalChopperOnAmpl"));

        verticalLayout_8->addWidget(plotCalChopperOnAmpl);

        plotCalChopperOnPhase = new QCustomPlot(tab_4);
        plotCalChopperOnPhase->setObjectName(QString::fromUtf8("plotCalChopperOnPhase"));

        verticalLayout_8->addWidget(plotCalChopperOnPhase);

        qtabPlots->addTab(tab_4, QString());
        tab_5 = new QWidget();
        tab_5->setObjectName(QString::fromUtf8("tab_5"));
        verticalLayout_9 = new QVBoxLayout(tab_5);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        plotCalChopperOffAmpl = new QCustomPlot(tab_5);
        plotCalChopperOffAmpl->setObjectName(QString::fromUtf8("plotCalChopperOffAmpl"));

        verticalLayout_9->addWidget(plotCalChopperOffAmpl);

        plotCalChopperOffPhase = new QCustomPlot(tab_5);
        plotCalChopperOffPhase->setObjectName(QString::fromUtf8("plotCalChopperOffPhase"));

        verticalLayout_9->addWidget(plotCalChopperOffPhase);

        qtabPlots->addTab(tab_5, QString());

        horizontalLayout_2->addWidget(qtabPlots);

        frame = new QFrame(Plot);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setMinimumSize(QSize(400, 0));
        frame->setFrameShape(QFrame::Panel);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout = new QVBoxLayout(frame);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        pbAutoScale = new QPushButton(frame);
        pbAutoScale->setObjectName(QString::fromUtf8("pbAutoScale"));
        pbAutoScale->setMinimumSize(QSize(150, 0));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/new/Icons/pics/AutoScale.png"), QSize(), QIcon::Normal, QIcon::Off);
        pbAutoScale->setIcon(icon);
        pbAutoScale->setIconSize(QSize(35, 35));

        verticalLayout->addWidget(pbAutoScale);

        pbPrint = new QPushButton(frame);
        pbPrint->setObjectName(QString::fromUtf8("pbPrint"));
        pbPrint->setMinimumSize(QSize(150, 0));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/new/Icons/pics/Print.png"), QSize(), QIcon::Normal, QIcon::Off);
        pbPrint->setIcon(icon1);
        pbPrint->setIconSize(QSize(35, 35));

        verticalLayout->addWidget(pbPrint);

        gpGraph1 = new QGroupBox(frame);
        gpGraph1->setObjectName(QString::fromUtf8("gpGraph1"));
        verticalLayout_2 = new QVBoxLayout(gpGraph1);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        pbShowCurveGraph1 = new QPushButton(gpGraph1);
        pbShowCurveGraph1->setObjectName(QString::fromUtf8("pbShowCurveGraph1"));
        pbShowCurveGraph1->setMinimumSize(QSize(150, 0));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/new/Icons/pics/DoNotShowDataPointsBlue.png"), QSize(), QIcon::Normal, QIcon::Off);
        pbShowCurveGraph1->setIcon(icon2);
        pbShowCurveGraph1->setIconSize(QSize(35, 35));
        pbShowCurveGraph1->setCheckable(true);
        pbShowCurveGraph1->setChecked(true);

        verticalLayout_2->addWidget(pbShowCurveGraph1);

        pbShowDatapointsGraph1 = new QPushButton(gpGraph1);
        pbShowDatapointsGraph1->setObjectName(QString::fromUtf8("pbShowDatapointsGraph1"));
        pbShowDatapointsGraph1->setMinimumSize(QSize(150, 0));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/new/Icons/pics/DoNotShowCurve.png"), QSize(), QIcon::Normal, QIcon::Off);
        pbShowDatapointsGraph1->setIcon(icon3);
        pbShowDatapointsGraph1->setIconSize(QSize(35, 35));
        pbShowDatapointsGraph1->setCheckable(true);
        pbShowDatapointsGraph1->setChecked(true);

        verticalLayout_2->addWidget(pbShowDatapointsGraph1);


        verticalLayout->addWidget(gpGraph1);

        gpGraph2 = new QGroupBox(frame);
        gpGraph2->setObjectName(QString::fromUtf8("gpGraph2"));
        verticalLayout_5 = new QVBoxLayout(gpGraph2);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        pbShowCurveGraph2 = new QPushButton(gpGraph2);
        pbShowCurveGraph2->setObjectName(QString::fromUtf8("pbShowCurveGraph2"));
        pbShowCurveGraph2->setMinimumSize(QSize(150, 0));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/new/Icons/pics/DoNotShowDataPointsRed.png"), QSize(), QIcon::Normal, QIcon::Off);
        pbShowCurveGraph2->setIcon(icon4);
        pbShowCurveGraph2->setIconSize(QSize(35, 35));
        pbShowCurveGraph2->setCheckable(true);
        pbShowCurveGraph2->setChecked(true);

        verticalLayout_5->addWidget(pbShowCurveGraph2);

        pbShowDatapointsGraph2 = new QPushButton(gpGraph2);
        pbShowDatapointsGraph2->setObjectName(QString::fromUtf8("pbShowDatapointsGraph2"));
        pbShowDatapointsGraph2->setMinimumSize(QSize(150, 0));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/new/Icons/pics/DoNotShowCurveRed.png"), QSize(), QIcon::Normal, QIcon::Off);
        pbShowDatapointsGraph2->setIcon(icon5);
        pbShowDatapointsGraph2->setIconSize(QSize(35, 35));
        pbShowDatapointsGraph2->setCheckable(true);
        pbShowDatapointsGraph2->setChecked(true);

        verticalLayout_5->addWidget(pbShowDatapointsGraph2);


        verticalLayout->addWidget(gpGraph2);

        gbGraph3 = new QGroupBox(frame);
        gbGraph3->setObjectName(QString::fromUtf8("gbGraph3"));
        verticalLayout_6 = new QVBoxLayout(gbGraph3);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        pbShowCurveGraph3 = new QPushButton(gbGraph3);
        pbShowCurveGraph3->setObjectName(QString::fromUtf8("pbShowCurveGraph3"));
        pbShowCurveGraph3->setMinimumSize(QSize(150, 0));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/new/Icons/pics/DoNotShowDataPointsGreen.png"), QSize(), QIcon::Normal, QIcon::Off);
        pbShowCurveGraph3->setIcon(icon6);
        pbShowCurveGraph3->setIconSize(QSize(35, 35));
        pbShowCurveGraph3->setCheckable(true);
        pbShowCurveGraph3->setChecked(true);

        verticalLayout_6->addWidget(pbShowCurveGraph3);

        pbShowDatapointsGraph3 = new QPushButton(gbGraph3);
        pbShowDatapointsGraph3->setObjectName(QString::fromUtf8("pbShowDatapointsGraph3"));
        pbShowDatapointsGraph3->setMinimumSize(QSize(150, 0));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/new/Icons/pics/DoNotShowCurveGreen.png"), QSize(), QIcon::Normal, QIcon::Off);
        pbShowDatapointsGraph3->setIcon(icon7);
        pbShowDatapointsGraph3->setIconSize(QSize(35, 35));
        pbShowDatapointsGraph3->setCheckable(true);
        pbShowDatapointsGraph3->setChecked(true);

        verticalLayout_6->addWidget(pbShowDatapointsGraph3);


        verticalLayout->addWidget(gbGraph3);

        groupBox = new QGroupBox(frame);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 4, 0, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 2, 0, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 3, 0, 1, 1);

        labFreq = new QLabel(groupBox);
        labFreq->setObjectName(QString::fromUtf8("labFreq"));
        labFreq->setMinimumSize(QSize(260, 0));
        QFont font;
        font.setFamily(QString::fromUtf8("Courier New"));
        font.setPointSize(12);
        labFreq->setFont(font);
        labFreq->setFrameShape(QFrame::Box);
        labFreq->setFrameShadow(QFrame::Raised);

        gridLayout->addWidget(labFreq, 0, 1, 1, 1);

        labValueGraph1 = new QLabel(groupBox);
        labValueGraph1->setObjectName(QString::fromUtf8("labValueGraph1"));
        labValueGraph1->setFont(font);
        labValueGraph1->setFrameShape(QFrame::Box);
        labValueGraph1->setFrameShadow(QFrame::Raised);

        gridLayout->addWidget(labValueGraph1, 2, 1, 1, 1);

        labValueGraph2 = new QLabel(groupBox);
        labValueGraph2->setObjectName(QString::fromUtf8("labValueGraph2"));
        labValueGraph2->setFont(font);
        labValueGraph2->setFrameShape(QFrame::Box);
        labValueGraph2->setFrameShadow(QFrame::Raised);

        gridLayout->addWidget(labValueGraph2, 3, 1, 1, 1);

        labValueGraph3 = new QLabel(groupBox);
        labValueGraph3->setObjectName(QString::fromUtf8("labValueGraph3"));
        labValueGraph3->setFont(font);
        labValueGraph3->setFrameShape(QFrame::Box);
        labValueGraph3->setFrameShadow(QFrame::Raised);

        gridLayout->addWidget(labValueGraph3, 4, 1, 1, 1);


        verticalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(frame);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout_2 = new QGridLayout(groupBox_2);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        labDiff1Name = new QLabel(groupBox_2);
        labDiff1Name->setObjectName(QString::fromUtf8("labDiff1Name"));

        gridLayout_2->addWidget(labDiff1Name, 0, 0, 1, 1);

        labDiff1Value = new QLabel(groupBox_2);
        labDiff1Value->setObjectName(QString::fromUtf8("labDiff1Value"));
        labDiff1Value->setFont(font);
        labDiff1Value->setFrameShape(QFrame::Box);
        labDiff1Value->setFrameShadow(QFrame::Raised);
        labDiff1Value->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(labDiff1Value, 0, 1, 1, 1);

        labDiff2Value = new QLabel(groupBox_2);
        labDiff2Value->setObjectName(QString::fromUtf8("labDiff2Value"));
        labDiff2Value->setFont(font);
        labDiff2Value->setFrameShape(QFrame::Box);
        labDiff2Value->setFrameShadow(QFrame::Raised);
        labDiff2Value->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(labDiff2Value, 1, 1, 1, 1);

        labDiff2Name = new QLabel(groupBox_2);
        labDiff2Name->setObjectName(QString::fromUtf8("labDiff2Name"));

        gridLayout_2->addWidget(labDiff2Name, 1, 0, 1, 1);


        verticalLayout->addWidget(groupBox_2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout_2->addWidget(frame);


        retranslateUi(Plot);

        qtabPlots->setCurrentIndex(4);


        QMetaObject::connectSlotsByName(Plot);
    } // setupUi

    void retranslateUi(QDialog *Plot)
    {
        Plot->setWindowTitle(QCoreApplication::translate("Plot", "Dialog", nullptr));
        qtabPlots->setTabText(qtabPlots->indexOf(tab), QCoreApplication::translate("Plot", "Tab 1", nullptr));
        qtabPlots->setTabText(qtabPlots->indexOf(tab_2), QCoreApplication::translate("Plot", "Tab 2", nullptr));
        qtabPlots->setTabText(qtabPlots->indexOf(tab_3), QCoreApplication::translate("Plot", "Noise", nullptr));
        qtabPlots->setTabText(qtabPlots->indexOf(tab_4), QCoreApplication::translate("Plot", "Cal Chopper On", nullptr));
        qtabPlots->setTabText(qtabPlots->indexOf(tab_5), QCoreApplication::translate("Plot", "Cal Chopper Off", nullptr));
        pbAutoScale->setText(QCoreApplication::translate("Plot", "AutoScale", nullptr));
        pbPrint->setText(QCoreApplication::translate("Plot", "Print", nullptr));
        gpGraph1->setTitle(QString());
        pbShowCurveGraph1->setText(QCoreApplication::translate("Plot", "Show Data Curve", nullptr));
        pbShowDatapointsGraph1->setText(QCoreApplication::translate("Plot", "Show Data Points", nullptr));
        gpGraph2->setTitle(QString());
        pbShowCurveGraph2->setText(QCoreApplication::translate("Plot", "Show Data Curve", nullptr));
        pbShowDatapointsGraph2->setText(QCoreApplication::translate("Plot", "Show Data Points", nullptr));
        gbGraph3->setTitle(QString());
        pbShowCurveGraph3->setText(QCoreApplication::translate("Plot", "Show Data Curve", nullptr));
        pbShowDatapointsGraph3->setText(QCoreApplication::translate("Plot", "Show Data Points", nullptr));
        groupBox->setTitle(QCoreApplication::translate("Plot", "Values", nullptr));
        label->setText(QCoreApplication::translate("Plot", "Freq.", nullptr));
        label_4->setText(QCoreApplication::translate("Plot", "Hz", nullptr));
        label_2->setText(QCoreApplication::translate("Plot", "Hx", nullptr));
        label_3->setText(QCoreApplication::translate("Plot", "Hy", nullptr));
        labFreq->setText(QString());
        labValueGraph1->setText(QString());
        labValueGraph2->setText(QString());
        labValueGraph3->setText(QString());
        groupBox_2->setTitle(QCoreApplication::translate("Plot", "Difference [%]", nullptr));
        labDiff1Name->setText(QCoreApplication::translate("Plot", "Diff", nullptr));
        labDiff1Value->setText(QString());
        labDiff2Value->setText(QString());
        labDiff2Name->setText(QCoreApplication::translate("Plot", "Diff", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Plot: public Ui_Plot {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLOT_H
