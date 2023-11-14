#ifndef PLOT_H
#define PLOT_H

#include <QDialog>

namespace Ui {
class Plot;
}

class Plot : public QDialog
{
    Q_OBJECT

public:
    explicit Plot(QWidget *parent = 0);
    ~Plot();

    bool showData (QVector<QVector<QVector<QString> > >& qvecTFHeader, QVector<QVector<QVector<QVector<double> > > >& qvecTFData, const bool bRawData) const;

private slots:
    void on_pbAutoScale_clicked (void);

    void on_pbPrint_clicked (void);

    void on_pbShowDatapoints_clicked (void);

    void on_pbShowCurve_clicked (void);

    void on_pbSave_clicked (void);

private:
    Ui::Plot *ui;

    QVector<double> qvecRefMFS06FreqChopperOn;
    QVector<double> qvecRefMFS06MagChopperOn;
    QVector<double> qvecRefMFS06PhaseChooperOn;

    QVector<double> qvecRefMFS06FreqChopperOff;
    QVector<double> qvecRefMFS06MagChopperOff;
    QVector<double> qvecRefMFS06PhaseChooperOff;

    QVector<double> qvecRefMFS07FreqChopperOn;
    QVector<double> qvecRefMFS07MagChopperOn;
    QVector<double> qvecRefMFS07PhaseChooperOn;

    QVector<double> qvecRefMFS07FreqChopperOff;
    QVector<double> qvecRefMFS07MagChopperOff;
    QVector<double> qvecRefMFS07PhaseChooperOff;

    void initRefCurves (void);

};

#endif // PLOT_H
