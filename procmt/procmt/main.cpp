#include <QCoreApplication>
#include <QDir>
#include "mc_data.h"
#include "mt_site.h"
#include "math_vector.h"
#include "procmt_alldefines.h"

#include <iostream>
#include <climits>
#include <cfloat>
#include <memory>





int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    int isites;
    int iprocs = 1;
    int iruns = 1;

    std::shared_ptr<prc_com> cmdline = std::make_shared<prc_com>(false);


    for (int i_procs = 0; i_procs < iprocs; ++i_procs) {


        // for each processing we create one edi file
        std::shared_ptr<mt_site> mtsite = std::make_shared<mt_site>(8, mttype::mt);


        std::cout << "procmt processing types " << i_procs << " of " << iprocs << "   ************************************************************" << std::endl;

        for (int i_runs = 0; i_runs < iruns; ++i_runs) {

            QList<QFileInfo> qfis;

            std::cout << "procmt processing runs  " << i_runs << " of " << iruns << "   ************************************************************" << std::endl;


            std::shared_ptr<mc_data> mxcd = std::make_shared<mc_data>();
            mxcd->insert_prc_com(cmdline);

            std::shared_ptr<math_vector> mathvector = std::make_shared<math_vector>();


            QObject::connect(mxcd.get(), &mc_data::signal_got_freq_targets, mathvector.get(), &math_vector::slot_parzen_vector, Qt::DirectConnection);
            //QObject::connect(mathvector.get(), &math_vector::frequencies_selected, mtsite.get(), &mt_site::slot_set_freqs, Qt::DirectConnection);
            QObject::connect(mathvector.get(), &math_vector::parzenvector_created, mxcd.get(), &mc_data::slot_append_parzenvector, Qt::DirectConnection);



            mxcd->open_files(qfis, true, mttype::rr, threadctrl::threadctrl_continuous_join);
           // mxcd->prepare_mt_spectra(3, 256, 0.3, 0.1,  0, 0, 0, 0.13, spectracollector,atsfileout::atsfileout_calibrated_spectra, true);


            // push data to site

            std::cout << "procmt processing runs  finished" << i_runs << " of " << iruns << "   ************************************************************" << std::endl;


        }




        // push data to EDI file
        mtsite.reset();


    }



    exit(0);
}
