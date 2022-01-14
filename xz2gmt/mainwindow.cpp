#include "mainwindow.h"
#include "ui_mainwindow.h"

mainwindow::mainwindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mainwindow)
{
    ui->setupUi(this);


   // this->statusBar()->showMessage("no file");
    ui->x_EW_max_doubleSpinBox->setRange(-10000000, 10000000);
    ui->x_EW_min_doubleSpinBox->setRange(-10000000, 10000000);
    ui->yz_NS_min_doubleSpinBox->setRange(-10000000, 10000000);
    ui->yz_NS_max_doubleSpinBox->setRange(-10000000, 10000000);


    ui->x_EW_scale_doubleSpinBox->setRange(-10000000, 1000000000);
    ui->yz_NS_scale_doubleSpinBox->setRange(-10000000, 1000000000);

    ui->delta_ew_doubleSpinBox->setRange(-10000000, 1000000000);
    ui->delta_ns_doubleSpinBox->setRange(-10000000, 1000000000);
    ui->advals_doubleSpinBox->setRange(-10000000, 1000000000);

    this->ui->mul_x_inputby_doubleSpinBox->setRange(-10000000, 1000000000);
    this->ui->mul_x_inputby_doubleSpinBox->setValue(this->mul_x_inputby);
    this->ui->mul_yz_inputby_doubleSpinBox->setRange(-10000000, 1000000000);
    this->ui->mul_yz_inputby_doubleSpinBox->setValue(this->mul_yz_inputby);
    this->ui->mul_vals_inputby_doubleSpinBox->setRange(-10000000, 1000000000);
    this->ui->mul_vals_inputby_doubleSpinBox->setValue(this->mul_vals);

    this->ui->y_offset_doubleSpinBox->setRange(-10000000, 1000000000);
    this->ui->y_offset_doubleSpinBox->setValue(this->y_offset);


    this->ui->Scale_annot_doubleSpinBox->setRange(0.0000001, 1000000000);
    this->ui->Scale_annot_doubleSpinBox->setValue(this->Scale_annot);

    this->ui->mul_profile_ycol_inputby_doubleSpinBox->setRange(-10000000, 1000000000);
    this->ui->mul_profile_ycol_inputby_doubleSpinBox->setValue(this->mul_profile_ycol_inputby);



    ui->x_EW_scale_doubleSpinBox->setDecimals(4);
    ui->yz_NS_scale_doubleSpinBox->setDecimals(4);

    ui->advals_doubleSpinBox->setDecimals(4);


    ui->x_EW_max_doubleSpinBox->setDecimals(4);
    ui->x_EW_min_doubleSpinBox->setDecimals(4);


    ui->yz_NS_max_doubleSpinBox->setDecimals(4);
    ui->yz_NS_min_doubleSpinBox->setDecimals(4);

    this->ui->IncX_doubleSpinBox->setRange(-10000000, 1000000000);
    this->ui->IncXsub_doubleSpinBox->setRange(-10000000, 1000000000);
    this->ui->IncY_doubleSpinBox->setRange(-10000000, 1000000000);
    this->ui->IncYsub_doubleSpinBox->setRange(-10000000, 1000000000);

    this->ui->IncX_doubleSpinBox->setDecimals(4);
    this->ui->IncXsub_doubleSpinBox->setDecimals(4);
    this->ui->IncY_doubleSpinBox->setDecimals(4);
    this->ui->IncYsub_doubleSpinBox->setDecimals(4);

    this->ui->ann_x_maj_doubleSpinBox->setDecimals(4);
    this->ui->ann_x_min_doubleSpinBox->setDecimals(4);

    this->ui->ann_y_maj_doubleSpinBox->setDecimals(4);
    this->ui->ann_y_min_doubleSpinBox->setDecimals(4);

    this->ui->ann_x_maj_doubleSpinBox->setRange(0.000001, 1000000000);
    this->ui->ann_x_min_doubleSpinBox->setRange(0.000001, 1000000000);

    this->ui->ann_y_maj_doubleSpinBox->setRange(0.000001, 1000000000);
    this->ui->ann_y_min_doubleSpinBox->setRange(0.000001, 1000000000);


    this->ui->xcol_spinBox->setValue(this->xcol);
    this->ui->ycol_spinBox->setValue(this->yzcol);
    this->ui->valcol_spinBox->setValue(this->valcol);

    this->ui->profile_rot_doubleSpinBox->setValue(this->profile_rot_deg);
    this->ui->profile_rot_doubleSpinBox->setRange(-360, 360);
    this->ui->profile_ycol_spinBox->setRange(-1, 360);
    this->ui->profile_ycol_spinBox->setValue(-1);




    ui->centralWidget->acceptDrops();
    this->setWindowTitle("xz2gmt");

    this->exec_shellProcess = new QProcess(this);
}

mainwindow::~mainwindow()
{
    if (this->f_in.isOpen()) this->f_in.close();
    if (this->f_out.isOpen()) this->f_out.close();
    delete ui;
}

void mainwindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void mainwindow::dropEvent(QDropEvent *event)
{

    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;
    this->qfi_in = QFileInfo(urls[0].toLocalFile());


    if (this->f_in.isOpen()) this->f_in.close();

    this->x.clear();
    this->yz.clear();
    this->vals.clear();
    this->prfy.clear();
    this->prfx.clear();

    this->f_in.setFileName(qfi_in.absoluteFilePath());
    if (this->f_in.open(QFile::ReadOnly)) {
        this->setWindowTitle(this->qfi_in.baseName());
        this->instr.setDevice(&this->f_in);

        if (this->info_lines) {
            for (int i = 0; i < this->info_lines; ++i) {
                this->instr.readLine();
            }
        }
        do {

            this->line = this->instr.readLine();
            this->line = this->line.simplified();
            if (!line.startsWith("#") && (line.size() > 2)) {
                this->line_items = this->line.split(" ");
                if (this->xcol < this->line_items.size()) this->x.append(this->line_items.at(this->xcol).toDouble());
                if (this->yzcol < this->line_items.size()) this->yz.append( this->line_items.at(this->yzcol).toDouble());
                if (this->valcol < this->line_items.size()) this->vals.append(this->line_items.at(this->valcol).toDouble());
                if ( (this->profile_ycol > -1) && (this->profile_ycol < this->line_items.size()))
                    this->prfy.append(this->line_items.at(this->profile_ycol).toDouble());
            }

        } while (!this->instr.atEnd());
        this->f_in.close();
        this->instr.resetStatus();
        for (auto  &v : this->vals) {this->vals_orig.append(v);}


        for (int j = 0; j < x.size(); ++j)  this->prfx.append(this->x.at(j));




        if (this->mul_x_inputby != 1) for (auto  &v : this->x) {v *= this->mul_x_inputby;}
        if (this->mul_yz_inputby != 1) for (auto  &v : this->yz) {v *= this->mul_yz_inputby;}


        this->on_mul_vals_inputby_doubleSpinBox_valueChanged(this->mul_vals);
        this->on_advals_doubleSpinBox_valueChanged(this->add_vals);

        //if (this->mul_vals != 1) for (auto  &v : this->vals) {v *= this->mul_vals;}
        //if (this->add_vals != 0) for (auto  &v : this->vals) {v *= this->add_vals;}
        if (this->mul_profile_ycol_inputby != 1) for (auto  &v : this->prfy) {v *= this->mul_profile_ycol_inputby;}


        if (this->ui->logx_EW_checkBox->isChecked()) for (auto  &v : this->x) {v = log10(v);}
        if (this->ui->log_y_NS_checkBox->isChecked()) for (auto  &v : this->yz) {v = log10(v);}
        //if (this->ui->log_vals_checkBox->isChecked()) for (auto  &v : this->vals) {v = log10(v);}
        if (this->ui->log_y_profile_checkBox->isChecked()) for (auto  &v : this->prfy) {v = log10(v);}



        // map to profile
        this->guess_profile();

        this->update();










    }

}

void mainwindow::make_script()
{
    this->qfi_out.setFile(this->qfi_in.absolutePath() + "/" + "plane.sh");
    this->f_out.setFileName(this->qfi_out.absoluteFilePath());

    this->exec_shell_str = "/bin/bash " + qfi_out.absoluteFilePath();

    if(this->f_out.open(QIODevice::WriteOnly | QIODevice::Text)) {
        this->qtx.setDevice(&f_out);
        this->qtx << "#!/bin/bash\n";
        this->qtx << "cd " << this->qfi_in.absolutePath() << Qt::endl;
        this->qtx << "#defines fot GMT script" << Qt::endl;
        this->qtx << "REGION='-R/" << this->x_EW_min - delta_ew << "/" << this->x_EW_max + delta_ew << "/";
        this->qtx <<          this->yz_NS_min - delta_ns << "/" << this->yz_NS_max + delta_ns << "'" << Qt::endl;
        this->qtx << "#output file "<< Qt::endl;
        this->qtx << "# grid files "<< Qt::endl;
        this->qtx << "# grid final "<< Qt::endl;
        this->qtx << "GRD_PLAN='planview.grd' "<< Qt::endl;
        this->qtx << "GRD_PLAN_SAMPLE='planview_sample.grd' "<< Qt::endl;
        this->qtx << "#"<< Qt::endl;
        this->qtx << "GRD_TXT='area.txt' "<< Qt::endl;
        this->qtx << "GRD_SUR='surface.grd' "<< Qt::endl;
        this->qtx << "GRD_SHAD='shadow.grd' "<< Qt::endl;
        this->qtx << "#colour scale "<< Qt::endl;
        this->qtx << "CPT='-Carea.cpt' "<< Qt::endl;
        this->qtx << "CNT='-Carea.contourlines' "<< Qt::endl;
        this->qtx << "#Orientation '-L' or '' "<< Qt::endl;
        this->qtx << "ORIENTATION='' "<< Qt::endl;
        this->qtx << "INCR='-I" << this->IncX << "/" << this->IncY << "'"<< Qt::endl;
        this->qtx << "INCR_SAMPLE='-I" << this->IncXsub << "/" << this->IncYsub << "'"<< Qt::endl;
        this->qtx << "BASEMAP='-Jx1:" << this->x_EW_scale << "/1:" << this->yz_NS_scale << " -Ba" << this->ann_x_maj << "f" << this->ann_x_min << "'" << Qt::endl;
        this->qtx << "AB='" << this->EW_x_label << "'" << Qt::endl;
        this->qtx << "AC='/a" << this->ann_y_maj << "f" << this->ann_y_min << "'" << Qt::endl;
        this->qtx << "AD='" << this->NS_yz_label << "'" << Qt::endl;
        this->qtx << "AE='WeSn'" << Qt::endl;
        this->qtx << "TITLE=':." << this->Title << ":'" << Qt::endl;
        if (this->Scale_annot != 0) {
            this->qtx << "COLORSCALE=\"-D22c/8c/8c/1.25c $CPT -B" << this->Scale_annot <<"\"" << Qt::endl;
        }
        else {
            this->qtx << "COLORSCALE=\"-D22c/8c/8c/1.25c $CPT -L " << "\"" << Qt::endl;
        }
        this->qtx << "CA='" << this->ScaleUnit << "'" << Qt::endl;
        this->qtx << "CB='" << this->ScaleTitle << "'" << Qt::endl;



        this->qtx << "datain=\"plane.dat\"" << Qt::endl;
        QString ofile = qfi_out.fileName();
        ofile.remove(".sh");
        ofile.append(".ps");
        this->qtx << "OUT=\"" << ofile <<"\" "<< Qt::endl;
        ofile.remove(".ps");
        ofile.append(".png");
        this->qtx << "OUTPNG=\"" << ofile <<"\" "<< Qt::endl;
        this->qtx << "gmt blockmean $datain $REGION $INCR > plan.tmp " << Qt::endl;
        this->qtx << "gmt surface plan.tmp $REGION $INCR -G$GRD_PLAN " << Qt::endl;
        this->qtx << "gmt grdsample $GRD_PLAN -G$GRD_PLAN_SAMPLE $INCR_SAMPLE $REGION " << Qt::endl;
        this->qtx << "gmt psbasemap $REGION $BASEMAP:\"$AB\":\"$AC\":\"$AD\":\"$AE\"\"$TITLE\"  -K > \"$OUT\" " << Qt::endl;

        if (this->ui->clip_above_topline_checkBox->isChecked()) {
            this->qtx << "gmt psclip polytop_clipabove.txt -J -R -O -K >> \"$OUT\" " << Qt::endl;
        }
        if (this->ui->clip_below_bottomline_checkBox->isChecked()) {
            this->qtx << "gmt psclip polytop_clipbelow.txt -J -R -O -K >> \"$OUT\" " << Qt::endl;
        }

        this->qtx << "gmt grdimage  $GRD_PLAN_SAMPLE -J -R $CPT -O -K >> \"$OUT\" " << Qt::endl;

        if (this->ui->contourlines_checkBox->isChecked()) {
            this->qtx << "grdcontour $GRD_PLAN_SAMPLE -J $CNT -K -O >> \"$OUT\" " << Qt::endl;
        }

        if (this->ui->clip_above_topline_checkBox->isChecked() || this->ui->clip_below_bottomline_checkBox->isChecked() ) {
            this->qtx << "gmt psclip -C -O -K >>  \"$OUT\" " << Qt::endl;
        }


        //if ( t < 2) this->qtx << "gmt pstext gmt_sites_x_y.txt -J $REGION -B+t'phi xy, f " << QString::number(freq) << " Hz' -O -K >> \"$OUT\" " << Qt::endl;
        //this->qtx << "gmt pstext gmt_sites_x_y.txt -J $REGION -B+t'rhoa xy, f  Hz' -O -K >> \"$OUT\" " << Qt::endl;
        // this->qtx << "gmt pstext -J $REGION -B+t'" << this->Title << "' -O -K >> \"$OUT\" " << Qt::endl;

        if (this->ui->topline_checkBox->isChecked() ) {
            this->qtx << "gmt psxy polytop.txt -J -R -O -K >> \"$OUT\" " << Qt::endl;

        }

        if (this->ui->bottomline_checkBox->isChecked()) {
            this->qtx << "gmt psxy polybottom.txt -J -R -O -K >> \"$OUT\" " << Qt::endl;

        }

        if (this->Scale_annot != 0) {
            this->qtx << "gmt psscale $COLORSCALE:\"$CA\":/:\"$CB\": -O -K >> \"$OUT\" " << Qt::endl;
        }
        else {
            this->qtx << "gmt psscale $COLORSCALE -O -K >> \"$OUT\" " << Qt::endl;
            this->qtx << "gmt pstext -Jx1c -R/0/20/0/10 -N -O -K << END >> \"$OUT\" " << Qt::endl;
            this->qtx << "23 13 $CB $CA" << Qt::endl;
            this->qtx << "END" << Qt::endl;
        }

        // close doc manually
        this->qtx << "echo %%PageTrailer >> \"$OUT\"" << Qt::endl;
        this->qtx << "echo U >> \"$OUT\"" << Qt::endl;
        this->qtx << "echo showpage >> \"$OUT\"" << Qt::endl;
        this->qtx << "echo %%Trailer >> \"$OUT\"" << Qt::endl;
        this->qtx << "echo end >> \"$OUT\"" << Qt::endl;
        this->qtx << "echo %%EOF >> \"$OUT\""<< Qt::endl;

        this->qtx << "sync" << Qt::endl;

        this->qtx << "convert -density 300x300 -rotate 90 \"$OUT\" \"$OUTPNG\"" << Qt::endl;



        if (this->ui->exec_ghostview_checkBox->isChecked()) {
            this->qtx << "/usr/bin/gv \"$OUT\" " << Qt::endl;
        }



        this->qtx.flush();

    }
    this->f_out.close();
    this->qtx.resetStatus();

    this->qfi_out.setFile(this->qfi_in.absolutePath() + "/" + "plane.dat");
    this->f_out.setFileName(this->qfi_out.absoluteFilePath());

    if(this->f_out.open(QIODevice::WriteOnly | QIODevice::Text)) {
        this->qtx.setDevice(&f_out);

        for (int i = 0; i < this->x.size(); ++i) {

            this->qtx << this->x.at(i) << " " << this->yz.at(i) << " " << this->vals.at(i) << Qt::endl;
        }

        this->qtx.flush();

    }

    this->f_out.close();
    this->qtx.resetStatus();


}

void mainwindow::on_make_shell_pb_clicked()
{
    this->make_script();

}

void mainwindow::on_exec_shell_pb_clicked()
{
    this->make_script();
    this->exec_shellProcess->setProcessChannelMode(QProcess::MergedChannels);
    this->exec_shellProcess->startDetached(exec_shell_str);
}


void mainwindow::on_x_EW_min_doubleSpinBox_valueChanged(double arg1)
{
    this->x_EW_min = arg1;
    this->IncX = abs(this->x_EW_max - this->x_EW_min) / 50;
    this->IncXsub = this->IncX / 10;

    this->ui->IncX_doubleSpinBox->setValue(this->IncX);
    this->ui->IncXsub_doubleSpinBox->setValue(this->IncXsub);
}

void mainwindow::on_x_EW_max_doubleSpinBox_valueChanged(double arg1)
{
    this->x_EW_max = arg1;
    this->IncX = abs(this->x_EW_max - this->x_EW_min) / 50;
    this->IncXsub = this->IncX / 10;
    this->ui->IncX_doubleSpinBox->setValue(this->IncX);
    this->ui->IncXsub_doubleSpinBox->setValue(this->IncXsub);
}

void mainwindow::on_yz_NS_min_doubleSpinBox_valueChanged(double arg1)
{
    this->yz_NS_min = arg1;

    this->IncY = abs(this->yz_NS_max - this->yz_NS_min) / 50;
    this->IncYsub = this->IncY / 10;

    this->ui->IncY_doubleSpinBox->setValue(this->IncY);
    this->ui->IncYsub_doubleSpinBox->setValue(this->IncYsub);
}

void mainwindow::on_yz_NS_max_doubleSpinBox_valueChanged(double arg1)
{
    this->yz_NS_max = arg1;

    this->IncY = abs(this->yz_NS_max - this->yz_NS_min) / 50;
    this->IncYsub = this->IncY / 10;

    this->ui->IncY_doubleSpinBox->setValue(this->IncY);
    this->ui->IncYsub_doubleSpinBox->setValue(this->IncYsub);
}

void mainwindow::on_IncX_doubleSpinBox_valueChanged(double arg1)
{
    this->IncX = arg1;
}

void mainwindow::on_IncY_doubleSpinBox_valueChanged(double arg1)
{
    this->IncY = arg1;
}

void mainwindow::on_IncXsub_doubleSpinBox_valueChanged(double arg1)
{
    this->IncXsub = arg1;
}

void mainwindow::on_IncYsub_doubleSpinBox_valueChanged(double arg1)
{
    this->IncYsub = arg1;
}

void mainwindow::on_ann_x_maj_doubleSpinBox_valueChanged(double arg1)
{
    this->ann_x_maj = arg1;
}

void mainwindow::on_ann_x_min_doubleSpinBox_valueChanged(double arg1)
{
    this->ann_x_min = arg1;
}

void mainwindow::on_ann_y_maj_doubleSpinBox_valueChanged(double arg1)
{
    this->ann_y_maj = arg1;
}

void mainwindow::on_ann_y_min_doubleSpinBox_valueChanged(double arg1)
{
    this->ann_y_min = arg1;
}

void mainwindow::on_delta_ns_doubleSpinBox_valueChanged(double arg1)
{
    this->delta_ns = arg1;
}

void mainwindow::on_delta_ew_doubleSpinBox_valueChanged(double arg1)
{
    this->delta_ew = arg1;
}



void mainwindow::on_Title_lineEdit_textChanged(const QString &arg1)
{
    this->Title = arg1;
}

void mainwindow::on_EW_x_label_lineEdit_textChanged(const QString &arg1)
{
    this->EW_x_label = arg1;
}

void mainwindow::on_NS_yz_label_lineEdit_textChanged(const QString &arg1)
{
    this->NS_yz_label = arg1;

}

void mainwindow::on_ScaleTitle_lineEdit_textChanged(const QString &arg1)
{
    this->ScaleTitle = arg1;

}

void mainwindow::on_ScaleUnit_lineEdit_textChanged(const QString &arg1)
{
    this->ScaleUnit = arg1;
}

void mainwindow::on_x_EW_scale_doubleSpinBox_valueChanged(double arg1)
{
    this->x_EW_scale = arg1;
}

void mainwindow::on_yz_NS_scale_doubleSpinBox_valueChanged(double arg1)
{
    this->yz_NS_scale = arg1;
}

void mainwindow::on_mul_x_inputby_doubleSpinBox_valueChanged(double arg1)
{
    this->mul_x_inputby = arg1;
    if (this->mul_x_inputby == 0.0) this->mul_x_inputby = 1;
}


void mainwindow::on_mul_yz_inputby_doubleSpinBox_valueChanged(double arg1)
{
    this->mul_yz_inputby = arg1;
    if (this->mul_yz_inputby == 0.0) this->mul_yz_inputby = 1;
}


void mainwindow::on_Scale_annot_doubleSpinBox_valueChanged(double arg1)
{
    this->Scale_annot = arg1;
}

void mainwindow::on_rescale_pushButton_clicked()
{



    this->on_one_by_one_checkBox_clicked(this->ui->one_by_one_checkBox->isChecked());

    qDebug() << "read" << this->x.size() << this->yz.size() << this->vals.size();
    qDebug() << "vals" << x_EW_min << x_EW_max << yz_NS_min << yz_NS_max;

    this->IncX = abs(this->x_EW_max - this->x_EW_min) / 50;
    this->IncXsub = this->IncX / 10;

    this->IncY = abs(this->yz_NS_max - this->yz_NS_min) / 50;
    this->IncYsub = this->IncY / 10;

    this->ann_x_maj = abs(this->x_EW_max - this->x_EW_min) / 4;
    this->ann_x_min = this->ann_x_maj / 4;

    this->ann_y_maj = abs(this->yz_NS_max - this->yz_NS_min) / 4;
    this->ann_y_min = this->ann_y_maj / 4;


    this->ui->x_EW_min_doubleSpinBox->setValue(this->x_EW_min);
    this->ui->x_EW_max_doubleSpinBox->setValue(this->x_EW_max);

    this->ui->yz_NS_max_doubleSpinBox->setValue(this->yz_NS_max);
    this->ui->yz_NS_min_doubleSpinBox->setValue(this->yz_NS_min);

    this->ui->IncX_doubleSpinBox->setValue(this->IncX);
    this->ui->IncXsub_doubleSpinBox->setValue(this->IncXsub);
    this->ui->IncY_doubleSpinBox->setValue(this->IncY);
    this->ui->IncYsub_doubleSpinBox->setValue(this->IncYsub);

    this->ui->ann_x_maj_doubleSpinBox->setValue(this->ann_x_maj);
    this->ui->ann_x_min_doubleSpinBox->setValue(this->ann_x_min);

    this->ui->ann_y_maj_doubleSpinBox->setValue(this->ann_y_maj);
    this->ui->ann_y_min_doubleSpinBox->setValue(this->ann_y_min);

}

void mainwindow::on_one_by_one_checkBox_clicked(bool checked)
{


    if (this->x.size() && this->yz.size()) {
        x_EW_scale = ceil(   ((abs(this->x_EW_max - this->x_EW_min) * 100) / (1. * x_EW_max_len)));
        yz_NS_scale = ceil(  ((abs(this->yz_NS_max - this->yz_NS_min) * 100) / (1. * yz_NS_act_len))) ;

        qDebug() << "map scale x 1:" << x_EW_scale << "  map scale  y 1:" << yz_NS_scale;
        qDebug() << "x 1:" << (1. * x_EW_max_len) / abs(this->x_EW_max - this->x_EW_min) << "    y 1:" << (1. * yz_NS_act_len) / abs(this->yz_NS_max - this->yz_NS_min);
        if (checked) {

            if (this->x_EW_scale > this->yz_NS_scale) this->yz_NS_scale = this->x_EW_scale;
            else this->x_EW_scale = this->yz_NS_scale;

        }


        this->ui->x_EW_scale_doubleSpinBox->setValue(x_EW_scale);
        this->ui->yz_NS_scale_doubleSpinBox->setValue(yz_NS_scale);
    }
}

void mainwindow::on_xcol_spinBox_valueChanged(int arg1)
{
    this->xcol = arg1;
}

void mainwindow::on_ycol_spinBox_valueChanged(int arg1)
{
    this->yzcol = arg1;
}

void mainwindow::on_valcol_spinBox_valueChanged(int arg1)
{
    this->valcol = arg1;

}

void mainwindow::on_profile_ycol_spinBox_valueChanged(int arg1)
{
    this->profile_ycol = arg1;
}

void mainwindow::on_profile_rot_doubleSpinBox_valueChanged(double arg1)
{
    this->profile_rot_deg = arg1;
    this->profile_rot_rad = (this->profile_rot_deg * M_PI)/180.;
    qDebug() << this->profile_rot_deg  << " rot[deg]";
    this->update();
}

void mainwindow::on_mul_profile_ycol_inputby_doubleSpinBox_valueChanged(double arg1)
{
    this->mul_profile_ycol_inputby = arg1;





}

void mainwindow::guess_profile()
{
    if (this->x.size() && this->yz.size() && this->prfy.size()) {
        double  varx, vary, stdx, stdy, meanx, meany,
                Qx, Qy, Qxy, covxy, corr_coeff, reg_coeff,stdxy, stdyx, abscissa, std_abscissa, std_reg_coeff;
        size_t n;



        xz2gmt::variance2D<double>(x.begin(), x.end(), prfy.begin(), prfy.end(), varx, vary, stdx, stdy, meanx, meany,
                                Qx, Qy, Qxy, n, covxy, corr_coeff, this->slope,stdxy, stdyx, abscissa, std_abscissa, std_reg_coeff );


        qDebug() << "abscissa:" << abscissa << "regression coef (slope):" << this->slope;
        qDebug() << "rot deg: " <<  (180. * atan(this->slope)) / M_PI;

        this->ui->profile_rot_doubleSpinBox->setValue((180. * atan(this->slope)) / M_PI);

    }
}

void mainwindow::update()
{
    if (this->profile_rot_deg == 0) {

        this->x = this->prfx;

        if (this->ui->zero_x_ax_startcheckBox->isChecked()) {
            double prf_off =  *min_element(x.begin(),x.end());

            for (int i = 0; i < this->x.size(); ++i) {
                x[i] -= prf_off;
            }

        }
        this->yz_NS_max = *max_element(yz.begin(),yz.end());
        this->yz_NS_min = *min_element(yz.begin(),yz.end());
        this->x_EW_max =  *max_element(x.begin(),x.end());
        this->x_EW_min =  *min_element(x.begin(),x.end());


        this->on_rescale_pushButton_clicked();
        this->polyline_bottom_top();

        return;

    }
    if ((this->profile_ycol > -1) && this->prfy.size()) {


        // copy orig
        this->x = this->prfx;
        for (int i = 0; i < this->x.size(); ++i) {
            x[i] = cos(this->profile_rot_rad) * prfx.at(i) + sin(this->profile_rot_rad) * prfy.at(i);
            qDebug() << "new x:" << x.at(i) << " orig x " << this->prfx.at(i);
        }

        if (this->ui->zero_x_ax_startcheckBox->isChecked()) {
            double prf_off =  *min_element(x.begin(),x.end());

            for (int i = 0; i < this->x.size(); ++i) {
                x[i] -= prf_off;
            }

        }
        for (int i = 0; i < this->x.size(); ++i) {
            qDebug() << "new x:" << x.at(i) << " orig x " << this->prfx.at(i);
        }


    }


    this->yz_NS_max = *max_element(yz.begin(),yz.end());
    this->yz_NS_min = *min_element(yz.begin(),yz.end());
    this->x_EW_max =  *max_element(x.begin(),x.end());
    this->x_EW_min =  *min_element(x.begin(),x.end());

    this->on_rescale_pushButton_clicked();
    this->polyline_bottom_top();




}

void mainwindow::polyline_bottom_top()
{
    if (this->ui->bottomline_checkBox->isChecked() || this->ui->clip_below_bottomline_checkBox->isChecked()
            || this->ui->topline_checkBox->isChecked() || this->ui->clip_above_topline_checkBox->isChecked()) {

        double starts = x.at(0);
        int counter = 0;
        QVector<double> yvalstmp;
        for (int i = 0; i < x.size(); ++i) {
            if (x.at(i) == starts) {
                yvalstmp.append(yz.at(i));
            }
            if (x.at(i) != starts || i == x.size() - 1) {
                if (i) this->bx.append(x.at(i-1));
                if (i) this->bx_val.append(*min_element(yvalstmp.begin(),yvalstmp.end()));
                if (i) this->tx.append(x.at(i-1));
                if (i) this->tx_val.append(*max_element(yvalstmp.begin(),yvalstmp.end()));
                if (i && !counter) {
                    this->tx_bt_start_val = (*min_element(yvalstmp.begin(),yvalstmp.end()));
                    this->bt_tx_start_val = (*max_element(yvalstmp.begin(),yvalstmp.end()));
                }
                yvalstmp.clear();
                if (i < x.size() - 1) {
                    starts = x.at(i);
                    yvalstmp.append(yz.at(i));
                }

            }
        }
        this->tx_bt_end_val = (*min_element(yvalstmp.begin(),yvalstmp.end()));
        this->bt_tx_end_val = (*max_element(yvalstmp.begin(),yvalstmp.end()));

        yvalstmp.clear();

        QTextStream pqts;
        QFileInfo   pqfi;
        QFile       pfile;

        if (this->ui->clip_below_bottomline_checkBox->isChecked() ) {
            if ((bx.size() == bx_val.size()) && bx.size() > 2) {
                pqfi.setFile(this->qfi_in.absolutePath() + "/" + "polybottom.txt");
                pfile.setFileName(pqfi.absoluteFilePath());
                if(pfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    pqts.setDevice(&pfile);
                    for (int i = 0; i < bx.size(); ++i) {
                        pqts << bx.at(i) << " " << bx_val.at(i) << Qt::endl;
                    }
                    pfile.close();

                }
            }
        }

        if (this->ui->topline_checkBox->isChecked()) {
            if ((tx.size() == tx_val.size()) && tx.size() > 2) {

                pqfi.setFile(this->qfi_in.absolutePath() + "/" + "polytop.txt");
                pfile.setFileName(pqfi.absoluteFilePath());
                if(pfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    pqts.setDevice(&pfile);
                    for (int i = 0; i < bx.size(); ++i) {
                        pqts << tx.at(i) << " " << tx_val.at(i) << Qt::endl;
                    }
                    pfile.close();

                }
            }
        }

        if (this->ui->clip_above_topline_checkBox->isChecked()) {
            if ((tx.size() == tx_val.size()) && tx.size() > 2) {

                pqfi.setFile(this->qfi_in.absolutePath() + "/" + "polytop_clipabove.txt");
                pfile.setFileName(pqfi.absoluteFilePath());
                if(pfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    pqts.setDevice(&pfile);
                    for (int i = 0; i < bx.size(); ++i) {
                        pqts << tx.at(i) << " " << tx_val.at(i) << Qt::endl;
                    }
                    pqts << tx.last() << " " << tx_bt_end_val << Qt::endl;
                    pqts << tx.at(0) << " " << tx_bt_start_val << Qt::endl;
                    pfile.close();

                }
            }
        }

        if (this->ui->clip_below_bottomline_checkBox->isChecked()) {
            if ((tx.size() == tx_val.size()) && tx.size() > 2) {

                pqfi.setFile(this->qfi_in.absolutePath() + "/" + "polytop_clipbelow.txt");
                pfile.setFileName(pqfi.absoluteFilePath());
                if(pfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    pqts.setDevice(&pfile);
                    for (int i = 0; i < bx.size(); ++i) {
                        pqts << bx.at(i) << " " << bx_val.at(i) << Qt::endl;
                    }
                    pqts << bx.last() << bt_tx_end_val << Qt::endl;
                    pqts << bx.at(0) <<  bt_tx_start_val << Qt::endl;
                    pfile.close();

                }
            }
        }
    }
}

void mainwindow::on_y_offset_doubleSpinBox_valueChanged(double arg1)
{
    this->y_offset = arg1;
    this->update();

}


void mainwindow::on_mul_vals_inputby_doubleSpinBox_valueChanged(double arg1)
{
    this->mul_vals = arg1;
    if (this->mul_vals == 0.0) {
        this->mul_vals = 1;
    }
    if (this->mul_vals == 1 && this->vals.size()) {
        for (int i = 0; i < this->vals.size(); ++i) {
            this->vals[i] = this->vals_orig.at(i);
        }
    }
    if (this->mul_vals != 1 && this->vals.size()) {

        for (int i = 0; i < this->vals.size(); ++i) {
            this->vals[i] = this->vals_orig.at(i) * this->mul_vals;
        }
    }

    if (this->add_vals == 0) {
        if (this->ui->log_vals_checkBox->isChecked()) for (auto  &v : this->vals) {v = log10(v);}
    }



}

void mainwindow::on_advals_doubleSpinBox_valueChanged(double arg1)
{
    this->add_vals = arg1;

    if (this->add_vals != 0 && this->vals.size()) {
        this->on_mul_vals_inputby_doubleSpinBox_valueChanged(this->mul_vals);
        for (int i = 0; i < this->vals.size(); ++i) {
            this->vals[i] +=  this->add_vals;
        }

        if (this->ui->log_vals_checkBox->isChecked()) for (auto  &v : this->vals) {v = log10(v);}
    }
}

void mainwindow::on_zero_x_ax_startcheckBox_clicked()
{
    this->update();
}
