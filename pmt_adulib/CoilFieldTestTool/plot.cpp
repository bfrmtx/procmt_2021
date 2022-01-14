#include "plot.h"
#include "ui_plot.h"




void ManageCursor(QCustomPlot *customPlot, QCPCursor *cursor, QVector<double> qvecPoint, QPen pen)
{
    QVector<double> qvecX;
    QVector<double> qvecY;

    if(cursor->hLine) customPlot->removeItem(cursor->hLine);
    cursor->hLine = new QCPItemLine(customPlot);
    cursor->hLine->setPen(pen);
    cursor->hLine->start->setCoords(QCPRange::minRange, qvecPoint [1]);
    cursor->hLine->end->setCoords  (QCPRange::maxRange, qvecPoint [1]);

    if(cursor->vLine) customPlot->removeItem(cursor->vLine);
    cursor->vLine = new QCPItemLine(customPlot);
    pen.setColor(Qt::gray);
    cursor->vLine->setPen(pen);
    cursor->vLine->start->setCoords(qvecPoint [0], QCPRange::minRange);
    cursor->vLine->end->setCoords  (qvecPoint [0], QCPRange::maxRange);

    qvecX.clear();
    qvecX.push_back(qvecPoint [0]);
    qvecY.push_back(qvecPoint [1]);

    customPlot->replot();
}


void Plot::slotMouseMove(QMouseEvent* event, QCustomPlot* pclPlot, const QString qstrUnit)
{
    int  iIndex;

    // cal plots do not work yet as they do not have any data on them
    if ((pclPlot == this->ui->plotCalChopperOnAmpl)  ||
            (pclPlot == this->ui->plotCalChopperOffAmpl) ||
            (pclPlot == this->ui->plotCalChopperOnPhase) ||
            (pclPlot == this->ui->plotCalChopperOffPhase))
    {
        return;
    }

    // only replot cursor, if the mouse does not move too fast
    if (this->atomCoursorUpdateActive.load () == false)
    {
        this->atomCoursorUpdateActive.store (true);

        QCustomPlot* pCustomPlot = pclPlot;

        QVector<double> qvecPointGraph1;
        QVector<double> qvecPointGraph2;
        QVector<double> qvecPointGraph3;

        if (qvecCursorMagPlot.size() == 0)
        {
            qvecCursorMagPlot.resize(3);
        }

        if (qvecCursorCohPlot.size() == 0)
        {
            qvecCursorCohPlot.resize(3);
        }

        if (qvecCursorNoisePlot.size() == 0)
        {
            qvecCursorNoisePlot.resize(3);
        }

        if (qvecCursorCalChopperOnAmplPlot.size() == 0)
        {
            qvecCursorCalChopperOnAmplPlot.resize(3);
        }

        if (qvecCursorCalChopperOnPhasePlot.size() == 0)
        {
            qvecCursorCalChopperOnPhasePlot.resize(3);
        }

        if (qvecCursorCalChopperOffAmplPlot.size() == 0)
        {
            qvecCursorCalChopperOffAmplPlot.resize(3);
        }

        if (qvecCursorCalChopperOffPhasePlot.size() == 0)
        {
            qvecCursorCalChopperOffPhasePlot.resize(3);
        }

        qvecPointGraph1.clear();
        qvecPointGraph1.push_back (pCustomPlot->xAxis->pixelToCoord(event->pos().x()));
        for (iIndex = 0; iIndex < pCustomPlot->graph(0)->data()->size(); iIndex++)
        {
            if (pCustomPlot->graph(0)->dataMainKey(iIndex) > qvecPointGraph1[0])
            {
                break;
            }
        }
        qvecPointGraph1 [0] = pCustomPlot->graph(0)->data()->at(iIndex)->key;
        qvecPointGraph1.push_back (pCustomPlot->graph(0)->data ()->at(iIndex)->value);

        qvecPointGraph2.push_back(qvecPointGraph1[0]);
        qvecPointGraph2.push_back (pCustomPlot->graph(1)->data ()->at(iIndex)->value);

        qvecPointGraph3.push_back(qvecPointGraph1[0]);
        qvecPointGraph3.push_back (pCustomPlot->graph(2)->data ()->at(iIndex)->value);

        quint64 qMillis = (unsigned int) qvecPointGraph1 [0];
        qMillis *= 1000;
        qMillis += ((qvecPointGraph1 [0] - (qMillis / 1000)) * 1000);

        this->ui->labFreq->setText (QString::number (qvecPointGraph1 [0], 'E', 5) + " [Hz]");
        if (pclPlot == this->ui->plotMag)
        {
            ManageCursor(pCustomPlot, &(qvecCursorMagPlot[0]), qvecPointGraph1, QPen(this->qvecColours.at(0)));
            ManageCursor(pCustomPlot, &(qvecCursorMagPlot[1]), qvecPointGraph2, QPen(this->qvecColours.at(1)));
            ManageCursor(pCustomPlot, &(qvecCursorMagPlot[2]), qvecPointGraph3, QPen(this->qvecColours.at(2)));

            this->ui->labValueGraph1->setText (QString::number (qvecPointGraph1 [1], 'E', 5) + " " + qstrUnit);
            this->ui->labValueGraph2->setText (QString::number (qvecPointGraph2 [1], 'E', 5) + " " + qstrUnit);
            this->ui->labValueGraph3->setText (QString::number (qvecPointGraph3 [1], 'E', 5) + " " + qstrUnit);

            double dTemp     = ((qvecPointGraph2 [1] * 100.0) / qvecPointGraph1 [1]) - 100.0;
            QString qstrTemp = (QString::number (dTemp, 'g', 4)) + " %";
            if (dTemp >= 0.0)
            {
                qstrTemp.prepend("+");
            }
            this->ui->labDiff1Value->setText (qstrTemp);

            dTemp    = ((qvecPointGraph3 [1] * 100.0) / qvecPointGraph1 [1]) - 100.0;
            qstrTemp = (QString::number (dTemp, 'g', 4)) + " %";
            if (dTemp >= 0.0)
            {
                qstrTemp.prepend("+");
            }
            this->ui->labDiff2Value->setText (qstrTemp);
        }
        else if (pclPlot == this->ui->plotCoh)
        {
            ManageCursor(pCustomPlot, &(qvecCursorCohPlot[0]), qvecPointGraph1, QPen(this->qvecColours.at(0)));
            ManageCursor(pCustomPlot, &(qvecCursorCohPlot[1]), qvecPointGraph2, QPen(this->qvecColours.at(1)));
            ManageCursor(pCustomPlot, &(qvecCursorCohPlot[2]), qvecPointGraph3, QPen(this->qvecColours.at(2)));

            this->ui->labValueGraph1->setText (QString::number (qvecPointGraph1 [1], 'g', 5) + " " + qstrUnit);
            this->ui->labValueGraph2->setText (QString::number (qvecPointGraph2 [1], 'g', 5) + " " + qstrUnit);
            this->ui->labValueGraph3->setText (QString::number (qvecPointGraph3 [1], 'g', 5) + " " + qstrUnit);

            this->ui->labDiff1Value->clear();
            this->ui->labDiff2Value->clear();
        }
        else if (pclPlot == this->ui->plotNoise)
        {
            ManageCursor(pCustomPlot, &(qvecCursorNoisePlot[0]), qvecPointGraph1, QPen(this->qvecColours.at(0)));
            ManageCursor(pCustomPlot, &(qvecCursorNoisePlot[1]), qvecPointGraph2, QPen(this->qvecColours.at(1)));
            ManageCursor(pCustomPlot, &(qvecCursorNoisePlot[2]), qvecPointGraph3, QPen(this->qvecColours.at(2)));

            this->ui->labValueGraph1->setText (QString::number (qvecPointGraph1 [1], 'E', 5) + " " + qstrUnit);
            this->ui->labValueGraph2->setText (QString::number (qvecPointGraph2 [1], 'E', 5) + " " + qstrUnit);
            this->ui->labValueGraph3->setText (QString::number (qvecPointGraph3 [1], 'E', 5) + " " + qstrUnit);

            this->ui->labDiff1Value->clear();
            this->ui->labDiff2Value->clear();
        }
        /*
        else if (pclPlot == this->ui->plotCalChopperOnAmpl)
        {
            ManageCursor(pCustomPlot, &(qvecCursorCalChopperOnAmplPlot[0]), qvecPointGraph1, QPen(this->qvecColours.at(0)));
            ManageCursor(pCustomPlot, &(qvecCursorCalChopperOnAmplPlot[1]), qvecPointGraph2, QPen(this->qvecColours.at(1)));
            ManageCursor(pCustomPlot, &(qvecCursorCalChopperOnAmplPlot[2]), qvecPointGraph3, QPen(this->qvecColours.at(2)));

            this->ui->labValueGraph1->setText (QString::number (qvecPointGraph1 [1], 'E', 5) + " " + qstrUnit);
            this->ui->labValueGraph2->setText (QString::number (qvecPointGraph2 [1], 'E', 5) + " " + qstrUnit);
            this->ui->labValueGraph3->setText (QString::number (qvecPointGraph3 [1], 'E', 5) + " " + qstrUnit);

            this->ui->labDiff1Value->clear();
            this->ui->labDiff2Value->clear();
        }
        else if (pclPlot == this->ui->plotCalChopperOffAmpl)
        {
            ManageCursor(pCustomPlot, &(qvecCursorCalChopperOffAmplPlot[0]), qvecPointGraph1, QPen(this->qvecColours.at(0)));
            ManageCursor(pCustomPlot, &(qvecCursorCalChopperOffAmplPlot[1]), qvecPointGraph2, QPen(this->qvecColours.at(1)));
            ManageCursor(pCustomPlot, &(qvecCursorCalChopperOffAmplPlot[2]), qvecPointGraph3, QPen(this->qvecColours.at(2)));

            this->ui->labValueGraph1->setText (QString::number (qvecPointGraph1 [1], 'E', 5) + " " + qstrUnit);
            this->ui->labValueGraph2->setText (QString::number (qvecPointGraph2 [1], 'E', 5) + " " + qstrUnit);
            this->ui->labValueGraph3->setText (QString::number (qvecPointGraph3 [1], 'E', 5) + " " + qstrUnit);

            this->ui->labDiff1Value->clear();
            this->ui->labDiff2Value->clear();
        }
        else if (pclPlot == this->ui->plotCalChopperOnPhase)
        {
            ManageCursor(pCustomPlot, &(qvecCursorCalChopperOnPhasePlot[0]), qvecPointGraph1, QPen(this->qvecColours.at(0)));
            ManageCursor(pCustomPlot, &(qvecCursorCalChopperOnPhasePlot[1]), qvecPointGraph2, QPen(this->qvecColours.at(1)));
            ManageCursor(pCustomPlot, &(qvecCursorCalChopperOnPhasePlot[2]), qvecPointGraph3, QPen(this->qvecColours.at(2)));

            this->ui->labValueGraph1->setText (QString::number (qvecPointGraph1 [1], 'g', 5) + " " + qstrUnit);
            this->ui->labValueGraph2->setText (QString::number (qvecPointGraph2 [1], 'g', 5) + " " + qstrUnit);
            this->ui->labValueGraph3->setText (QString::number (qvecPointGraph3 [1], 'g', 5) + " " + qstrUnit);

            this->ui->labDiff1Value->clear();
            this->ui->labDiff2Value->clear();
        }
        else if (pclPlot == this->ui->plotCalChopperOffPhase)
        {
            ManageCursor(pCustomPlot, &(qvecCursorCalChopperOffPhasePlot[0]), qvecPointGraph1, QPen(this->qvecColours.at(0)));
            ManageCursor(pCustomPlot, &(qvecCursorCalChopperOffPhasePlot[1]), qvecPointGraph2, QPen(this->qvecColours.at(1)));
            ManageCursor(pCustomPlot, &(qvecCursorCalChopperOffPhasePlot[2]), qvecPointGraph3, QPen(this->qvecColours.at(2)));

            this->ui->labValueGraph1->setText (QString::number (qvecPointGraph1 [1], 'g', 5) + " " + qstrUnit);
            this->ui->labValueGraph2->setText (QString::number (qvecPointGraph2 [1], 'g', 5) + " " + qstrUnit);
            this->ui->labValueGraph3->setText (QString::number (qvecPointGraph3 [1], 'g', 5) + " " + qstrUnit);

            this->ui->labDiff1Value->clear();
            this->ui->labDiff2Value->clear();
        }
        */


        this->atomCoursorUpdateActive.store (false);
    }
    this->uiMousePosX = event->pos().x();
}


void Plot::mouseMoveMagPlot (QMouseEvent* event)
{
    if ((QDateTime::currentDateTime().toMSecsSinceEpoch() - this->qiLastCursourUpdate) > 30)
    {
        emit (this->sigMouseMove(event, this->ui->plotMag, "[nT/sqrt(Hz)]"));
        this->qiLastCursourUpdate = QDateTime::currentDateTime().toMSecsSinceEpoch();
    }
}


void Plot::mouseMoveCohPlot (QMouseEvent* event)
{
    if ((QDateTime::currentDateTime().toMSecsSinceEpoch() - this->qiLastCursourUpdate) > 30)
    {
        emit (this->sigMouseMove(event, this->ui->plotCoh, ""));
        this->qiLastCursourUpdate = QDateTime::currentDateTime().toMSecsSinceEpoch();
    }
}


void Plot::mouseMoveNoisePlot (QMouseEvent* event)
{
    if ((QDateTime::currentDateTime().toMSecsSinceEpoch() - this->qiLastCursourUpdate) > 30)
    {
        emit (this->sigMouseMove(event, this->ui->plotNoise, "[nT/sqrt(Hz)]"));
        this->qiLastCursourUpdate = QDateTime::currentDateTime().toMSecsSinceEpoch();
    }
}


void Plot::mouseMoveCalChopperOnAmplPlot(QMouseEvent* event)
{
    if ((QDateTime::currentDateTime().toMSecsSinceEpoch() - this->qiLastCursourUpdate) > 30)
    {
        emit (this->sigMouseMove(event, this->ui->plotCalChopperOnAmpl, "[V/(nT * Hz)]"));
        this->qiLastCursourUpdate = QDateTime::currentDateTime().toMSecsSinceEpoch();
    }
}


void Plot::mouseMoveCalChopperOnPhasePlot(QMouseEvent* event)
{
    if ((QDateTime::currentDateTime().toMSecsSinceEpoch() - this->qiLastCursourUpdate) > 30)
    {
        emit (this->sigMouseMove(event, this->ui->plotCalChopperOnPhase, "[degree]"));
        this->qiLastCursourUpdate = QDateTime::currentDateTime().toMSecsSinceEpoch();
    }
}


void Plot::mouseMoveCalChopperOffAmplPlot(QMouseEvent* event)
{
    if ((QDateTime::currentDateTime().toMSecsSinceEpoch() - this->qiLastCursourUpdate) > 30)
    {
        emit (this->sigMouseMove(event, this->ui->plotCalChopperOffAmpl, "[V/(nT * Hz)]"));
        this->qiLastCursourUpdate = QDateTime::currentDateTime().toMSecsSinceEpoch();
    }
}


void Plot::mouseMoveCalChopperOffPhasePlot(QMouseEvent* event)
{
    if ((QDateTime::currentDateTime().toMSecsSinceEpoch() - this->qiLastCursourUpdate) > 30)
    {
        emit (this->sigMouseMove(event, this->ui->plotCalChopperOffPhase, "[degree]"));
        this->qiLastCursourUpdate = QDateTime::currentDateTime().toMSecsSinceEpoch();
    }
}


Plot::Plot(const QFileInfo &in_info_db, QFileInfo &in_master_cal_db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Plot)
{

    this->info_db = fall_back_default(in_info_db);
    this->master_cal_db = fall_back_default(in_master_cal_db);
    // fetch the infodb and mastercal db

    ui->setupUi(this);

    this->atomCoursorUpdateActive.store (false);

    // restore geometry
    pclSettings->beginGroup(this->objectName());
    this->restoreGeometry(pclSettings->value("geometry").toByteArray());
    pclSettings->endGroup();

    // save geometry
    pclSettings->beginGroup(this->objectName());
    pclSettings->setValue  ("geometry", this->saveGeometry());
    pclSettings->endGroup  ();

    qvechowCurveButtons.clear();
    qvechowCurveButtons.push_back(this->ui->pbShowCurveGraph1);
    qvechowCurveButtons.push_back(this->ui->pbShowCurveGraph2);
    qvechowCurveButtons.push_back(this->ui->pbShowCurveGraph3);

    qvechowPointsButtons.clear();
    qvechowPointsButtons.push_back(this->ui->pbShowDatapointsGraph1);
    qvechowPointsButtons.push_back(this->ui->pbShowDatapointsGraph2);
    qvechowPointsButtons.push_back(this->ui->pbShowDatapointsGraph3);

    qvecColours.clear();
    qvecColours.push_back(Qt::blue);
    qvecColours.push_back(Qt::red);
    qvecColours.push_back(Qt::green);

    qvecGraphButtons.clear();
    qvecGraphButtons.push_back(this->ui->gpGraph1);
    qvecGraphButtons.push_back(this->ui->gpGraph2);
    qvecGraphButtons.push_back(this->ui->gbGraph3);

    this->ui->labFreq->setStyleSheet        ("QLabel { background-color : white; }");
    this->ui->labValueGraph1->setStyleSheet ("QLabel { background-color : white; }");
    this->ui->labValueGraph2->setStyleSheet ("QLabel { background-color : white; }");
    this->ui->labValueGraph3->setStyleSheet ("QLabel { background-color : white; }");

    this->ui->labDiff1Value->setStyleSheet  ("QLabel { background-color : white; }");
    this->ui->labDiff2Value->setStyleSheet  ("QLabel { background-color : white; }");

    connect(this->ui->plotMag,   SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseMoveMagPlot  (QMouseEvent*)));
    connect(this->ui->plotCoh,   SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseMoveCohPlot  (QMouseEvent*)));
    connect(this->ui->plotNoise, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseMoveNoisePlot(QMouseEvent*)));

    connect(this->ui->plotCalChopperOnAmpl,  SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseMoveCalChopperOnAmplPlot (QMouseEvent*)));
    connect(this->ui->plotCalChopperOnPhase, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseMoveCalChopperOnPhasePlot(QMouseEvent*)));

    connect(this->ui->plotCalChopperOffAmpl,  SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseMoveCalChopperOffAmplPlot (QMouseEvent*)));
    connect(this->ui->plotCalChopperOffPhase, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseMoveCalChopperOffPhasePlot(QMouseEvent*)));

    connect (this, SIGNAL (sigMouseMove(QMouseEvent*, QCustomPlot*, const QString)), this, SLOT (slotMouseMove(QMouseEvent*, QCustomPlot*, const QString)));

    this->qiLastCursourUpdate = QDateTime::currentDateTime().toMSecsSinceEpoch();

    this->ui->plotMag->axisRect()->setRangeZoom  (Qt::Horizontal);
    this->ui->plotCoh->axisRect()->setRangeZoom  (Qt::Horizontal);
    this->ui->plotNoise->axisRect()->setRangeZoom(Qt::Horizontal);

    initRefCurves ();
}

Plot::~Plot()
{
    // save geometry
    pclSettings->beginGroup(this->objectName());
    pclSettings->setValue  ("geometry", this->saveGeometry());
    pclSettings->endGroup  ();


    delete ui;
}


bool Plot::showData (QVector<QVector<QVector<QString> > >& qvecTFHeader, QVector<QVector<QVector<QVector<double> > > >& qvecTFData,
                     QVector<QVector<double>>& qvecCalChopperOnFreq,  QVector<QVector<double>>& qvecCalChopperOnAmpl,  QVector<QVector<double>>& qvecCalChopperOnPhase,
                     QVector<QVector<double>>& qvecCalChopperOffFreq, QVector<QVector<double>>& qvecCalChopperOffAmpl, QVector<QVector<double>>& qvecCalChopperOffPhase)
{
    bool         bRetValue = true;
    int SetCounter;
    int CHCounter;
    int Counter;

    double minf_ampl = 0.0;
    QVector<double> qvecFreq;
    QVector<double> qvecMag;
    QVector<double> qvecCoh;
    QVector<double> qvecNoise;
    QVector<double> qvecError;

    QVector<double> qvecMagLimLow;
    QVector<double> qvecMagLimUp;

    QCustomPlot* pclMagPlot   = this->ui->plotMag;
    QCustomPlot* pclCohPlot   = this->ui->plotCoh;
    QCustomPlot* pclNoisePlot = this->ui->plotNoise;

    QCustomPlot* pclCalChopperOnAmplPlot  = this->ui->plotCalChopperOnAmpl;
    QCustomPlot* pclCalChopperOnPhasePlot = this->ui->plotCalChopperOnPhase;

    QCustomPlot* pclCalChopperOffAmplPlot  = this->ui->plotCalChopperOffAmpl;
    QCustomPlot* pclCalChopperOffPhasePlot = this->ui->plotCalChopperOffPhase;

    unsigned int uiSetSmallestCalFreq = 0;
    int uiGraphCount = 0;
    unsigned int uiParallelChannel;

    QString qstrTemp;
    QString qstrTitle;

    double dRefPercent = 5.0;

    ui->qtabPlots->setTabText (0, "Stacked Spectra");
    ui->qtabPlots->setTabText (1, "Coherency");

    pclMagPlot->clearGraphs ();
    pclCohPlot->clearGraphs ();

    double dLastSampeFreq = 0.0;

    QList<QPair<double, int>> qlSetOrder;

    // as a first step order the sets according to sample frequency
    for (SetCounter = 0; SetCounter < qvecTFHeader.size (); SetCounter++)
    {
        if (qvecTFHeader [SetCounter][0].size() > 0)
        {
            qstrTemp = qvecTFHeader [SetCounter][0][0];
            qstrTemp = qstrTemp.left(qstrTemp.size() - 2);
            if (qstrTemp.toDouble() != 0.0)
            {
                qlSetOrder.append(QPair<double, int>(qstrTemp.toDouble(), SetCounter));
            }
        }
    }

    std::sort(qlSetOrder.begin(), qlSetOrder.end());

    qstrTitle  = "Stacked Spectra";

    qvecMagLimLow.clear ();
    qvecMagLimUp.clear  ();

    for (CHCounter = 0; CHCounter < qvecTFData [0].size (); CHCounter++)
    {
        qvecFreq.clear  ();
        qvecMag.clear   ();
        qvecCoh.clear   ();
        qvecNoise.clear ();
        qvecError.clear ();

        dLastSampeFreq = 0.0;

        uiParallelChannel = CHCounter + 1;
        if (uiParallelChannel >= qvecTFHeader[0].size ())
        {
            uiParallelChannel = 0;
        }

        for (SetCounter = 0; SetCounter < qlSetOrder.size (); SetCounter++)
        {
            int uiSetID = qlSetOrder.at(SetCounter).second;
            for (Counter = 0; Counter < qvecTFData [uiSetID][CHCounter][0].size (); Counter++)
            {
                // do not overlap the single bands
                if (qvecTFData [uiSetID][CHCounter][0] [Counter] > dLastSampeFreq)
                {
                    dLastSampeFreq = qvecTFData [uiSetID][CHCounter][0] [Counter];

                    // frequency
                    qvecFreq.push_back(qvecTFData [uiSetID][CHCounter][0] [Counter]);

                    // magnitude
                    qvecMag.push_back (qvecTFData [uiSetID][CHCounter][1] [Counter]);

                    // coherency
                    qvecCoh.push_back (qvecTFData [uiSetID][CHCounter][3][Counter]);

                    // noise
                    qvecNoise.push_back (qvecTFData [uiSetID][CHCounter][4][Counter]);

                    // error
                    qvecError.push_back(qvecTFData [uiSetID][CHCounter][5][Counter]);

                    // limit values (used from channel 0)
                    if (CHCounter == 0)
                    {
                        qvecMagLimLow.push_back(qvecMag.last() - (qvecMag.last() * (dRefPercent / 100.0)));
                        qvecMagLimUp.push_back (qvecMag.last() + (qvecMag.last() * (dRefPercent / 100.0)));
                    }
                }
            }
        }

        qstrTemp  = qvecTFHeader [0][CHCounter][3];
        qstrTemp += " - ";
        qstrTemp += qvecTFHeader [0][CHCounter][2];
        qstrTemp += " - ";
        qstrTemp += qvecTFHeader [uiSetSmallestCalFreq][CHCounter][4];

        qstrTitle += " - " + qvecTFHeader [0][CHCounter][2];

        qvecGraphButtons.at(uiGraphCount)->setTitle(qvecTFHeader [0][CHCounter][2]);

        this->ui->labDiff1Name->setText(qvecTFHeader [0][0][2] + " / " + qvecTFHeader [0][1][2]);
        this->ui->labDiff2Name->setText(qvecTFHeader [0][0][2] + " / " + qvecTFHeader [0][2][2]);

        QPen* pclPen = new QPen (qvecColours.at(uiGraphCount));
        pclPen->setWidth (1);



        if ((pclMagPlot != Q_NULLPTR) && qvecMag.size())
        {
            pclMagPlot->addGraph       ();
            pclMagPlot->graph          (uiGraphCount)->setName (qvecTFHeader [0][CHCounter][2]);
            pclMagPlot->graph          (uiGraphCount)->setVisible (true);
            pclMagPlot->graph          (uiGraphCount)->addData (qvecFreq, qvecMag);

            double amin = *std::min_element(qvecFreq.begin(), qvecFreq.end());

            if ( (amin < minf_ampl) || minf_ampl == 0.0) minf_ampl = amin;


            pclMagPlot->graph          (uiGraphCount)->setPen   (*pclPen);
            pclMagPlot->graph          (uiGraphCount)->setBrush (Qt::NoBrush);
            pclMagPlot->graph          (uiGraphCount)->rescaleAxes(true);
            pclMagPlot->graph          (uiGraphCount)->setBrush    (QBrush(QColor(0, 255, 0, 0)));

            pclMagPlot->graph          (uiGraphCount)->setScatterStyle(QCPScatterStyle (QCPScatterStyle::ssCross, pclPen->color(), 10.0));

            // add error bars:
            if (qvecErrorBars.size() < (CHCounter + 1))
            {
                qvecErrorBars.push_back(new QCPErrorBars(pclMagPlot->xAxis, pclMagPlot->yAxis));
            }
            qvecErrorBars[CHCounter]->removeFromLegend();
            qvecErrorBars[CHCounter]->setAntialiased(false);
            qvecErrorBars[CHCounter]->setDataPlottable(pclMagPlot->graph(uiGraphCount));
            qvecErrorBars[CHCounter]->setPen    (*pclPen);
            qvecErrorBars[CHCounter]->setData   (qvecError);
            qvecErrorBars[CHCounter]->setVisible(false);

            pclMagPlot->xAxis->setScaleType    (QCPAxis::stLogarithmic);
            pclMagPlot->yAxis->setScaleType    (QCPAxis::stLogarithmic);
            pclMagPlot->update                ();
            pclMagPlot->setInteractions        (QCP::iRangeDrag | QCP::iRangeZoom);
            pclMagPlot->rescaleAxes            ();

            pclMagPlot->legend->setVisible      (true);
            pclMagPlot->legend->setFont(QFont   ("Helvetica",9));
            pclMagPlot->setLocale               (QLocale(QLocale::English, QLocale::UnitedKingdom));

            pclMagPlot->graph (uiGraphCount)->keyAxis   ()->setLabel ("Frequency [Hz]");
            pclMagPlot->graph (uiGraphCount)->valueAxis ()->setLabel ("Magnitude [nT/sqrt(Hz)]");
        }


        if ((pclCohPlot != Q_NULLPTR) && qvecCoh.size())
        {
            pclCohPlot->addGraph       ();
            pclCohPlot->graph          (uiGraphCount)->setName (qvecTFHeader [0][CHCounter][2] + " / " + qvecTFHeader [0][uiParallelChannel][2]);
            pclCohPlot->graph          (uiGraphCount)->addData (qvecFreq, qvecCoh);
            pclCohPlot->graph          (uiGraphCount)->setVisible (true);

            pclCohPlot->graph          (uiGraphCount)->setPen   (*pclPen);
            pclCohPlot->graph          (uiGraphCount)->setBrush (Qt::NoBrush);
            pclCohPlot->graph          (uiGraphCount)->rescaleAxes(true);
            pclCohPlot->graph          (uiGraphCount)->setBrush    (QBrush(QColor(0, 255, 0, 0)));

            pclCohPlot->graph          (uiGraphCount)->setScatterStyle(QCPScatterStyle (QCPScatterStyle::ssCross, pclPen->color(), 10.0));

            pclCohPlot->xAxis->setScaleType    (QCPAxis::stLogarithmic);
            pclCohPlot->yAxis->setScaleType    (QCPAxis::stLinear);
            pclCohPlot->update                ();
            pclCohPlot->setInteractions        (QCP::iRangeDrag | QCP::iRangeZoom);

            pclCohPlot->legend->setVisible      (true);
            pclCohPlot->legend->setFont(QFont   ("Helvetica",9));
            pclCohPlot->setLocale               (QLocale(QLocale::English, QLocale::UnitedKingdom));

            pclCohPlot->graph (uiGraphCount)->keyAxis   ()->setLabel ("Frequency [Hz]");
            pclCohPlot->graph (uiGraphCount)->valueAxis ()->setLabel ("Coherency");

            pclCohPlot->rescaleAxes     ();
            pclCohPlot->yAxis->setRange (0.0, 1.1);

        }

        if ((pclNoisePlot != Q_NULLPTR) && qvecNoise.size())
        {
            pclNoisePlot->addGraph       ();
            pclNoisePlot->graph          (uiGraphCount)->setName (qvecTFHeader [0][CHCounter][2]);
            pclNoisePlot->graph          (uiGraphCount)->addData (qvecFreq, qvecNoise);
            pclNoisePlot->graph          (uiGraphCount)->setVisible (true);

            pclNoisePlot->graph          (uiGraphCount)->setPen   (*pclPen);
            pclNoisePlot->graph          (uiGraphCount)->setBrush (Qt::NoBrush);
            pclNoisePlot->graph          (uiGraphCount)->rescaleAxes(true);
            pclNoisePlot->graph          (uiGraphCount)->setBrush    (QBrush(QColor(0, 255, 0, 0)));

            pclNoisePlot->graph          (uiGraphCount)->setScatterStyle(QCPScatterStyle (QCPScatterStyle::ssCross, pclPen->color(), 10.0));

            pclNoisePlot->xAxis->setScaleType    (QCPAxis::stLogarithmic);
            pclNoisePlot->yAxis->setScaleType    (QCPAxis::stLogarithmic);
            pclNoisePlot->update                ();
            pclNoisePlot->setInteractions        (QCP::iRangeDrag | QCP::iRangeZoom);
            pclNoisePlot->rescaleAxes            ();

            pclNoisePlot->legend->setVisible      (true);
            pclNoisePlot->legend->setFont(QFont   ("Helvetica",9));
            pclNoisePlot->setLocale               (QLocale(QLocale::English, QLocale::UnitedKingdom));

            pclNoisePlot->graph (uiGraphCount)->keyAxis   ()->setLabel ("Frequency [Hz]");
            pclNoisePlot->graph (uiGraphCount)->valueAxis ()->setLabel ("Noise");
        }


        if ((pclCalChopperOnAmplPlot != Q_NULLPTR) && (qvecCalChopperOnAmpl.size() > CHCounter))
        {
            pclCalChopperOnAmplPlot->addGraph       ();
            pclCalChopperOnAmplPlot->graph          (uiGraphCount)->setName (qvecTFHeader [0][CHCounter][2]);
            pclCalChopperOnAmplPlot->graph          (uiGraphCount)->addData (qvecCalChopperOnFreq [CHCounter],
                                                                             qvecCalChopperOnAmpl [CHCounter]);
            pclCalChopperOnAmplPlot->graph          (uiGraphCount)->setVisible (true);

            pclCalChopperOnAmplPlot->graph          (uiGraphCount)->setPen   (*pclPen);
            pclCalChopperOnAmplPlot->graph          (uiGraphCount)->setBrush (Qt::NoBrush);
            pclCalChopperOnAmplPlot->graph          (uiGraphCount)->rescaleAxes(true);
            pclCalChopperOnAmplPlot->graph          (uiGraphCount)->setBrush    (QBrush(QColor(0, 255, 0, 0)));

            pclCalChopperOnAmplPlot->graph          (uiGraphCount)->setScatterStyle(QCPScatterStyle (QCPScatterStyle::ssCross, pclPen->color(), 10.0));

            pclCalChopperOnAmplPlot->xAxis->setScaleType    (QCPAxis::stLogarithmic);
            pclCalChopperOnAmplPlot->yAxis->setScaleType    (QCPAxis::stLogarithmic);
            //            pclCalChopperOnAmplPlot->yAxis->setAutoSubTicks (true);
            pclCalChopperOnAmplPlot->update                ();
            pclCalChopperOnAmplPlot->setInteractions        (QCP::iRangeDrag | QCP::iRangeZoom);
            pclCalChopperOnAmplPlot->rescaleAxes            ();

            pclCalChopperOnAmplPlot->legend->setVisible      (true);
            pclCalChopperOnAmplPlot->legend->setFont(QFont   ("Helvetica",9));
            pclCalChopperOnAmplPlot->setLocale               (QLocale(QLocale::English, QLocale::UnitedKingdom));

            pclCalChopperOnAmplPlot->graph (uiGraphCount)->keyAxis   ()->setLabel ("Frequency [Hz]");
            pclCalChopperOnAmplPlot->graph (uiGraphCount)->valueAxis ()->setLabel ("Amplitude [V/(nT * Hz)]");
        }

        if ((pclCalChopperOnPhasePlot != Q_NULLPTR) && (qvecCalChopperOnPhase.size() > CHCounter) )
        {
            pclCalChopperOnPhasePlot->addGraph       ();
            pclCalChopperOnPhasePlot->graph          (uiGraphCount)->setName (qvecTFHeader [0][CHCounter][2]);
            pclCalChopperOnPhasePlot->graph          (uiGraphCount)->addData (qvecCalChopperOnFreq  [CHCounter],
                                                                              qvecCalChopperOnPhase [CHCounter]);
            pclCalChopperOnPhasePlot->graph          (uiGraphCount)->setVisible (true);

            pclCalChopperOnPhasePlot->graph          (uiGraphCount)->setPen   (*pclPen);
            pclCalChopperOnPhasePlot->graph          (uiGraphCount)->setBrush (Qt::NoBrush);
            pclCalChopperOnPhasePlot->graph          (uiGraphCount)->rescaleAxes(true);
            pclCalChopperOnPhasePlot->graph          (uiGraphCount)->setBrush    (QBrush(QColor(0, 255, 0, 0)));

            pclCalChopperOnPhasePlot->graph          (uiGraphCount)->setScatterStyle(QCPScatterStyle (QCPScatterStyle::ssCross, pclPen->color(), 10.0));

            pclCalChopperOnPhasePlot->xAxis->setScaleType    (QCPAxis::stLogarithmic);
            pclCalChopperOnPhasePlot->yAxis->setScaleType    (QCPAxis::stLinear);
            //            pclCalChopperOnPhasePlot->yAxis->setAutoSubTicks (true);
            pclCalChopperOnPhasePlot->update                ();
            pclCalChopperOnPhasePlot->setInteractions        (QCP::iRangeDrag | QCP::iRangeZoom);
            pclCalChopperOnPhasePlot->rescaleAxes            ();

            pclCalChopperOnPhasePlot->legend->setVisible      (true);
            pclCalChopperOnPhasePlot->legend->setFont(QFont   ("Helvetica",9));
            pclCalChopperOnPhasePlot->setLocale               (QLocale(QLocale::English, QLocale::UnitedKingdom));

            pclCalChopperOnPhasePlot->graph (uiGraphCount)->keyAxis   ()->setLabel ("Frequency [Hz]");
            pclCalChopperOnPhasePlot->graph (uiGraphCount)->valueAxis ()->setLabel ("Phase [degree]");
        }

        if ((pclCalChopperOffAmplPlot != Q_NULLPTR) &&  (qvecCalChopperOffAmpl.size() > CHCounter))
        {
            pclCalChopperOffAmplPlot->addGraph       ();
            pclCalChopperOffAmplPlot->graph          (uiGraphCount)->setName (qvecTFHeader [0][CHCounter][2]);
            pclCalChopperOffAmplPlot->graph          (uiGraphCount)->addData (qvecCalChopperOffFreq [CHCounter],
                                                                              qvecCalChopperOffAmpl [CHCounter]);
            pclCalChopperOffAmplPlot->graph          (uiGraphCount)->setVisible (true);

            pclCalChopperOffAmplPlot->graph          (uiGraphCount)->setPen   (*pclPen);
            pclCalChopperOffAmplPlot->graph          (uiGraphCount)->setBrush (Qt::NoBrush);
            pclCalChopperOffAmplPlot->graph          (uiGraphCount)->rescaleAxes(true);
            pclCalChopperOffAmplPlot->graph          (uiGraphCount)->setBrush    (QBrush(QColor(0, 255, 0, 0)));

            pclCalChopperOffAmplPlot->graph          (uiGraphCount)->setScatterStyle(QCPScatterStyle (QCPScatterStyle::ssCross, pclPen->color(), 10.0));

            pclCalChopperOffAmplPlot->xAxis->setScaleType    (QCPAxis::stLogarithmic);
            pclCalChopperOffAmplPlot->yAxis->setScaleType    (QCPAxis::stLogarithmic);
            //           pclCalChopperOffAmplPlot->yAxis->setAutoSubTicks (true);
            pclCalChopperOffAmplPlot->update                ();
            pclCalChopperOffAmplPlot->setInteractions        (QCP::iRangeDrag | QCP::iRangeZoom);
            pclCalChopperOffAmplPlot->rescaleAxes            ();

            pclCalChopperOffAmplPlot->legend->setVisible      (true);
            pclCalChopperOffAmplPlot->legend->setFont(QFont   ("Helvetica",9));
            pclCalChopperOffAmplPlot->setLocale               (QLocale(QLocale::English, QLocale::UnitedKingdom));

            pclCalChopperOffAmplPlot->graph (uiGraphCount)->keyAxis   ()->setLabel ("Frequency [Hz]");
            pclCalChopperOffAmplPlot->graph (uiGraphCount)->valueAxis ()->setLabel ("Amplitude [V/(nT * Hz)]");
        }

        if ( (pclCalChopperOffPhasePlot != Q_NULLPTR) && (qvecCalChopperOffPhase.size() > CHCounter))
        {
            pclCalChopperOffPhasePlot->addGraph       ();
            pclCalChopperOffPhasePlot->graph          (uiGraphCount)->setName (qvecTFHeader [0][CHCounter][2]);
            pclCalChopperOffPhasePlot->graph          (uiGraphCount)->addData (qvecCalChopperOffFreq  [CHCounter],
                                                                               qvecCalChopperOffPhase [CHCounter]);
            pclCalChopperOffPhasePlot->graph          (uiGraphCount)->setVisible (true);

            pclCalChopperOffPhasePlot->graph          (uiGraphCount)->setPen   (*pclPen);
            pclCalChopperOffPhasePlot->graph          (uiGraphCount)->setBrush (Qt::NoBrush);
            pclCalChopperOffPhasePlot->graph          (uiGraphCount)->rescaleAxes(true);
            pclCalChopperOffPhasePlot->graph          (uiGraphCount)->setBrush    (QBrush(QColor(0, 255, 0, 0)));

            pclCalChopperOffPhasePlot->graph          (uiGraphCount)->setScatterStyle(QCPScatterStyle (QCPScatterStyle::ssCross, pclPen->color(), 10.0));

            pclCalChopperOffPhasePlot->xAxis->setScaleType    (QCPAxis::stLogarithmic);
            pclCalChopperOffPhasePlot->yAxis->setScaleType    (QCPAxis::stLinear);
            //           pclCalChopperOffPhasePlot->yAxis->setAutoSubTicks (true);
            pclCalChopperOffPhasePlot->update                ();
            pclCalChopperOffPhasePlot->setInteractions        (QCP::iRangeDrag | QCP::iRangeZoom);
            pclCalChopperOffPhasePlot->rescaleAxes            ();

            pclCalChopperOffPhasePlot->legend->setVisible      (true);
            pclCalChopperOffPhasePlot->legend->setFont(QFont   ("Helvetica",9));
            pclCalChopperOffPhasePlot->setLocale               (QLocale(QLocale::English, QLocale::UnitedKingdom));

            pclCalChopperOffPhasePlot->graph (uiGraphCount)->keyAxis   ()->setLabel ("Frequency [Hz]");
            pclCalChopperOffPhasePlot->graph (uiGraphCount)->valueAxis ()->setLabel ("Phase [degree]");
        }


        uiGraphCount++;
    }

    if ((pclMagPlot != Q_NULLPTR) && qvecMag.size()) {

        QVector<double> noise_f;
        QVector<double> noise_lev;
        this->master_calibration_db = QSqlDatabase::addDatabase("QSQLITE", "get_noise");
        this->master_calibration_db.setConnectOptions("QSQLITE_OPEN_READONLY");
        this->master_calibration_db.setDatabaseName(this->master_cal_db.absoluteFilePath());
        if (this->master_calibration_db.open()) {
            QSqlQuery query(this->master_calibration_db);
            QString query_str("SELECT `f`, `MFS-06e` from 'noise_levels'");
            if( query.exec(query_str) ) {
                while (query.next()) {
                    noise_f.push_back(query.value(0).toDouble());
                    noise_lev.push_back(query.value(1).toDouble());

                }
            }



        }
        this->master_calibration_db.close();

        if ( (noise_f.size()) && (noise_f.size() == noise_lev.size()) ) {

            size_t chop = std::upper_bound (noise_f.begin(), noise_f.end(), minf_ampl) - noise_f.begin();

            if (chop < noise_f.size() - 1) {


                noise_f.erase(noise_f.begin(), noise_f.begin() + chop);
                noise_lev.erase(noise_lev.begin(), noise_lev.begin() + chop);


                pclMagPlot->addGraph       ();

                pclMagPlot->graph          (uiGraphCount)->addData (noise_f, noise_lev);
                pclMagPlot->graph          (uiGraphCount)->setVisible (true);
                pclMagPlot->graph          (uiGraphCount)->setPen   (QPen(Qt::gray));
                pclMagPlot->graph          (uiGraphCount)->setBrush (Qt::NoBrush);

            }

        }

    }

    pclMagPlot->rescaleAxes   ();
    pclNoisePlot->rescaleAxes ();

    pclCalChopperOnAmplPlot->rescaleAxes  ();
    pclCalChopperOnPhasePlot->rescaleAxes ();
    pclCalChopperOffAmplPlot->rescaleAxes ();
    pclCalChopperOffPhasePlot->rescaleAxes();

    pclMagPlot->replot  ();
    pclCohPlot->replot  ();
    pclNoisePlot->replot();

    pclCalChopperOnAmplPlot->replot();
    pclCalChopperOnPhasePlot->replot();
    pclCalChopperOffAmplPlot->replot();
    pclCalChopperOffPhasePlot->replot();

    for (uiGraphCount = 0; uiGraphCount < this->qvechowPointsButtons.size(); uiGraphCount++)
    {
        this->qvechowPointsButtons.at(uiGraphCount)->setChecked(false);
        this->on_pbShowDatapoints_clicked (uiGraphCount);
    }

    return (bRetValue);
}


void Plot::initRefCurves(void)
{
    // clear all arrays
//    qvecRefMFS06FreqChopperOn.clear ();
//    qvecRefMFS06MagChopperOn.clear ();
//    qvecRefMFS06PhaseChooperOn.clear ();

//    qvecRefMFS06FreqChopperOff.clear ();
//    qvecRefMFS06MagChopperOff.clear ();
//    qvecRefMFS06PhaseChooperOff.clear ();

//    qvecRefMFS07FreqChopperOn.clear ();
//    qvecRefMFS07MagChopperOn.clear ();
//    qvecRefMFS07PhaseChooperOn.clear ();

//    qvecRefMFS07FreqChopperOff.clear ();
//    qvecRefMFS07MagChopperOff.clear ();
//    qvecRefMFS07PhaseChooperOff.clear ();




//    std::unique_ptr<calibration> cal07 = std::make_unique<calibration>(info_db.absoluteFilePath());
//    cal07->sensortype = "MFS-07e";
//    cal07->open_master_cal(this->master_cal_db);
//    cal07->get_master_cal();
//    qvecRefMFS07FreqChopperOn = QVector<double>::fromStdVector(cal07->f_on_master);
//    qvecRefMFS07MagChopperOn = QVector<double>::fromStdVector(cal07->ampl_on_master);
//    qvecRefMFS07PhaseChooperOn = QVector<double>::fromStdVector(cal07->phase_grad_on_master);

//    qvecRefMFS07FreqChopperOff = QVector<double>::fromStdVector(cal07->f_off_master);
//    qvecRefMFS07MagChopperOff = QVector<double>::fromStdVector(cal07->ampl_off_master);
//    qvecRefMFS07PhaseChooperOff = QVector<double>::fromStdVector(cal07->phase_grad_off_master);

//    std::unique_ptr<calibration> cal06 = std::make_unique<calibration>(info_db.absoluteFilePath());
//    cal06->sensortype = "MFS-06e";
//    cal06->open_master_cal(this->master_cal_db);
//    cal06->get_master_cal();
//    qvecRefMFS06FreqChopperOn = QVector<double>::fromStdVector(cal06->f_on_master);
//    qvecRefMFS06MagChopperOn = QVector<double>::fromStdVector(cal06->ampl_on_master);
//    qvecRefMFS06PhaseChooperOn = QVector<double>::fromStdVector(cal06->phase_grad_on_master);

//    qvecRefMFS06FreqChopperOff = QVector<double>::fromStdVector(cal06->f_off_master);
//    qvecRefMFS06MagChopperOff = QVector<double>::fromStdVector(cal06->ampl_off_master);
//    qvecRefMFS06PhaseChooperOff = QVector<double>::fromStdVector(cal06->phase_grad_off_master);





}


void Plot::on_pbAutoScale_clicked (void)
{
    ui->plotMag->rescaleAxes     ();
    ui->plotCoh->rescaleAxes     ();
    ui->plotCoh->yAxis->setRange (0.0, 1.1);
    ui->plotNoise->rescaleAxes   ();

    ui->plotCalChopperOnAmpl->rescaleAxes  ();
    ui->plotCalChopperOnPhase->rescaleAxes ();
    ui->plotCalChopperOffAmpl->rescaleAxes ();
    ui->plotCalChopperOffPhase->rescaleAxes();

    ui->plotMag->replot  ();
    ui->plotCoh->replot  ();
    ui->plotNoise->replot();

    ui->plotCalChopperOnAmpl->replot();
    ui->plotCalChopperOnPhase->replot();
    ui->plotCalChopperOffAmpl->replot();
    ui->plotCalChopperOffPhase->replot();
}


void Plot::on_pbPrint_clicked (void)
{
    int iRetValue;

    int iTop;
    int iLeft;
    int iHeight;
    int iWidth;

    unsigned int Counter;

    QRectF clrect;


    // remove cursors from plots
    for (Counter = 0; Counter < this->qvecCursorMagPlot.size(); Counter++)
    {
        if(this->qvecCursorMagPlot.at(Counter).hLine) this->ui->plotMag->removeItem(this->qvecCursorMagPlot.at(Counter).hLine);
        if(this->qvecCursorMagPlot.at(Counter).vLine) this->ui->plotMag->removeItem(this->qvecCursorMagPlot.at(Counter).vLine);
    }

    for (Counter = 0; Counter < this->qvecCursorCohPlot.size(); Counter++)
    {
        if(this->qvecCursorCohPlot.at(Counter).hLine) this->ui->plotCoh->removeItem(this->qvecCursorCohPlot.at(Counter).hLine);
        if(this->qvecCursorCohPlot.at(Counter).vLine) this->ui->plotCoh->removeItem(this->qvecCursorCohPlot.at(Counter).vLine);
    }

    for (Counter = 0; Counter < this->qvecCursorNoisePlot.size(); Counter++)
    {
        if(this->qvecCursorNoisePlot.at(Counter).hLine) this->ui->plotNoise->removeItem(this->qvecCursorNoisePlot.at(Counter).hLine);
        if(this->qvecCursorNoisePlot.at(Counter).vLine) this->ui->plotNoise->removeItem(this->qvecCursorNoisePlot.at(Counter).vLine);
    }


    QPrintDialog* pclPrintDialog = new QPrintDialog ();

    iRetValue = pclPrintDialog->exec ();
    if (iRetValue == 1)
    {
        clrect = pclPrintDialog->printer ()->pageRect(QPrinter::Millimeter);
        iTop   = clrect.top  ();
        iLeft  = clrect.left ();

        iHeight = clrect.height () - 150;
        iWidth  = clrect.width  ();

        QPainter painter;
        painter.begin(pclPrintDialog->printer ());
        painter.drawText   (iLeft, iTop + 20,                     "   Sensor Field Test Evaluation");
        painter.drawPixmap (iLeft, iTop + 40,                     ui->plotMag->toPixmap (iWidth, (iHeight / 2), 1.0));
        painter.drawPixmap (iLeft, iTop + (iHeight / 2)     + 45, ui->plotCoh->toPixmap (iWidth, (iHeight / 2), 1.0));
        painter.drawText   (iLeft, iTop + (iHeight / 2) * 2 + 50, "   " + this->windowTitle ());

        pclPrintDialog->printer ()->newPage();
        clrect = pclPrintDialog->printer ()->pageRect(QPrinter::Millimeter);
        iTop   = clrect.top  ();
        iLeft  = clrect.left ();

        iHeight = clrect.height () - 150;
        iWidth  = clrect.width  ();

        painter.drawText   (iLeft, iTop + 20,                     "   Sensor Field Test Evaluation");
        painter.drawPixmap (iLeft, iTop + 40,                     ui->plotNoise->toPixmap (iWidth, (iHeight / 2), 1.0));
        painter.drawText   (iLeft, iTop + (iHeight / 2) * 2 + 50, "   " + this->windowTitle ());

        painter.end();
    }


    delete (pclPrintDialog);

}


void Plot::on_pbShowDatapoints_clicked (int iID)
{
    if (qvechowPointsButtons.at(iID)->isChecked() == true)
    {
        if (iID < ui->plotMag->graphCount())
        {
            ui->plotMag->graph (iID)->setScatterStyle  (QCPScatterStyle (QCPScatterStyle::ssCross, qvecColours.at(iID), 10.0));
            qvecErrorBars[iID]->setVisible(true);
        }
        if (iID < ui->plotCoh->graphCount())
        {
            ui->plotCoh->graph (iID)->setScatterStyle(QCPScatterStyle (QCPScatterStyle::ssCross, qvecColours.at(iID), 10.0));
        }
        if (iID < ui->plotNoise->graphCount())
        {
            ui->plotNoise->graph (iID)->setScatterStyle(QCPScatterStyle (QCPScatterStyle::ssCross, qvecColours.at(iID), 10.0));
        }

        if (iID < ui->plotCalChopperOnAmpl->graphCount())
        {
            ui->plotCalChopperOnAmpl->graph (iID)->setScatterStyle(QCPScatterStyle (QCPScatterStyle::ssCross, qvecColours.at(iID), 10.0));
        }
        if (iID < ui->plotCalChopperOffAmpl->graphCount())
        {
            ui->plotCalChopperOffAmpl->graph (iID)->setScatterStyle(QCPScatterStyle (QCPScatterStyle::ssCross, qvecColours.at(iID), 10.0));
        }
        if (iID < ui->plotCalChopperOnPhase->graphCount())
        {
            ui->plotCalChopperOnPhase->graph (iID)->setScatterStyle(QCPScatterStyle (QCPScatterStyle::ssCross, qvecColours.at(iID), 10.0));
        }
        if (iID < ui->plotCalChopperOffPhase->graphCount())
        {
            ui->plotCalChopperOffPhase->graph (iID)->setScatterStyle(QCPScatterStyle (QCPScatterStyle::ssCross, qvecColours.at(iID), 10.0));
        }
    }
    else
    {
        if (iID < ui->plotMag->graphCount())
        {
            ui->plotMag->graph (iID)->setScatterStyle  (QCPScatterStyle (QCPScatterStyle::ssNone, qvecColours.at(iID), 10.0));
            qvecErrorBars[iID]->setVisible(false);
        }
        if (iID < ui->plotCoh->graphCount())
        {
            ui->plotCoh->graph (iID)->setScatterStyle(QCPScatterStyle (QCPScatterStyle::ssNone, qvecColours.at(iID), 10.0));
        }
        if (iID < ui->plotNoise->graphCount())
        {
            ui->plotNoise->graph (iID)->setScatterStyle(QCPScatterStyle (QCPScatterStyle::ssNone, qvecColours.at(iID), 10.0));
        }

        if (iID < ui->plotCalChopperOnAmpl->graphCount())
        {
            ui->plotCalChopperOnAmpl->graph (iID)->setScatterStyle(QCPScatterStyle (QCPScatterStyle::ssNone, qvecColours.at(iID), 10.0));
        }
        if (iID < ui->plotCalChopperOffAmpl->graphCount())
        {
            ui->plotCalChopperOffAmpl->graph (iID)->setScatterStyle(QCPScatterStyle (QCPScatterStyle::ssNone, qvecColours.at(iID), 10.0));
        }
        if (iID < ui->plotCalChopperOnPhase->graphCount())
        {
            ui->plotCalChopperOnPhase->graph (iID)->setScatterStyle(QCPScatterStyle (QCPScatterStyle::ssNone, qvecColours.at(iID), 10.0));
        }
        if (iID < ui->plotCalChopperOffPhase->graphCount())
        {
            ui->plotCalChopperOffPhase->graph (iID)->setScatterStyle(QCPScatterStyle (QCPScatterStyle::ssNone, qvecColours.at(iID), 10.0));
        }
    }

    ui->plotMag->replot   ();
    ui->plotCoh->replot   ();
    ui->plotNoise->replot ();

    ui->plotCalChopperOnAmpl->replot();
    ui->plotCalChopperOnPhase->replot();
    ui->plotCalChopperOffAmpl->replot();
    ui->plotCalChopperOffPhase->replot();
}


void Plot::on_pbShowCurveGraph1_clicked (void)
{
    this->on_pbShowCurve_clicked(0);
}


void Plot::on_pbShowCurveGraph2_clicked (void)
{
    this->on_pbShowCurve_clicked(1);
}


void Plot::on_pbShowCurveGraph3_clicked (void)
{
    this->on_pbShowCurve_clicked(2);
}


void Plot::on_pbShowCurve_clicked (int iID)
{
    if (qvechowCurveButtons.at(iID)->isChecked() == true)
    {
        if (iID < ui->plotMag->graphCount())
        {
            ui->plotMag->graph (iID)->setLineStyle (QCPGraph::lsLine);
        }
        if (iID < ui->plotCoh->graphCount())
        {
            ui->plotCoh->graph (iID)->setLineStyle (QCPGraph::lsLine);
        }
        if (iID < ui->plotNoise->graphCount())
        {
            ui->plotNoise->graph (iID)->setLineStyle (QCPGraph::lsLine);
        }
        if (iID < ui->plotCalChopperOnAmpl->graphCount())
        {
            ui->plotCalChopperOnAmpl->graph (iID)->setLineStyle (QCPGraph::lsLine);
        }
        if (iID < ui->plotCalChopperOffAmpl->graphCount())
        {
            ui->plotCalChopperOffAmpl->graph (iID)->setLineStyle (QCPGraph::lsLine);
        }
        if (iID < ui->plotCalChopperOnPhase->graphCount())
        {
            ui->plotCalChopperOnPhase->graph (iID)->setLineStyle (QCPGraph::lsLine);
        }
        if (iID < ui->plotCalChopperOffPhase->graphCount())
        {
            ui->plotCalChopperOffPhase->graph (iID)->setLineStyle (QCPGraph::lsLine);
        }
    }
    else
    {
        if (iID < ui->plotMag->graphCount())
        {
            ui->plotMag->graph (iID)->setLineStyle (QCPGraph::lsNone);
        }
        if (iID < ui->plotCoh->graphCount())
        {
            ui->plotCoh->graph (iID)->setLineStyle (QCPGraph::lsNone);
        }
        if (iID < ui->plotNoise->graphCount())
        {
            ui->plotNoise->graph (iID)->setLineStyle (QCPGraph::lsNone);
        }
        if (iID < ui->plotCalChopperOnAmpl->graphCount())
        {
            ui->plotCalChopperOnAmpl->graph (iID)->setLineStyle (QCPGraph::lsNone);
        }
        if (iID < ui->plotCalChopperOffAmpl->graphCount())
        {
            ui->plotCalChopperOffAmpl->graph (iID)->setLineStyle (QCPGraph::lsNone);
        }
        if (iID < ui->plotCalChopperOnPhase->graphCount())
        {
            ui->plotCalChopperOnPhase->graph (iID)->setLineStyle (QCPGraph::lsNone);
        }
        if (iID < ui->plotCalChopperOffPhase->graphCount())
        {
            ui->plotCalChopperOffPhase->graph (iID)->setLineStyle (QCPGraph::lsNone);
        }
    }

    ui->plotMag->replot ();
    ui->plotCoh->replot ();
    ui->plotNoise->replot();

    ui->plotCalChopperOnAmpl->replot();
    ui->plotCalChopperOnPhase->replot();
    ui->plotCalChopperOffAmpl->replot();
    ui->plotCalChopperOffPhase->replot();
}


void Plot::on_pbShowDatapointsGraph1_clicked (void)
{
    this->on_pbShowDatapoints_clicked(0);
}


void Plot::on_pbShowDatapointsGraph2_clicked (void)
{
    this->on_pbShowDatapoints_clicked(1);
}


void Plot::on_pbShowDatapointsGraph3_clicked (void)
{
    this->on_pbShowDatapoints_clicked(2);
}

/*
void Plot::slt_pressQCP(QMouseEvent *mouse)
{
    double X = ui->widget->xAxis->pixelToCoord(mouse->x());
    double Y = ui->widget->yAxis->pixelToCoord(mouse->y());
    rectZoom->topLeft->setCoords(X, Y);
    topleftRect.setX(X);
    topleftRect.setY(Y);
    rectZoom->setVisible(true);
//    rectZoom->bottomRight->setCoords(1, 1);
    isQCPPressed = true;
//    ui->widget->replot();
    bottomRightRect.setX(0);
    bottomRightRect.setY(0);
}

void Plot::slt_releaseQCP(QMouseEvent *)
{
    if( (bottomRightRect.x() == 0) || (bottomRightRect.y() == 0))
    {
        rectZoom->setVisible(false);
        return;
    }

    ui->widget->xAxis->setRange(topleftRect.x(), bottomRightRect.x());
    ui->widget->yAxis->setRange(topleftRect.y(), bottomRightRect.y());
    rectZoom->topLeft->setCoords(0,0);
    rectZoom->bottomRight->setCoords(0,0);
    isQCPPressed = false;

    rectZoom->setVisible(false);
    ui->widget->replot();
}

void Plot::slt_moveQCP(QMouseEvent *mouse)
{
    if(isQCPPressed)
    {
        double X = ui->widget->xAxis->pixelToCoord(mouse->x());
        double Y = ui->widget->yAxis->pixelToCoord(mouse->y());
        rectZoom->bottomRight->setCoords(X, Y);
        ui->widget->replot();
        bottomRightRect.setX(X);
        bottomRightRect.setY(Y);
    }
}
*/

