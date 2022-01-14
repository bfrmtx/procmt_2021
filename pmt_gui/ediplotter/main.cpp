#include "ediplotter.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    std::shared_ptr<msg_logger> msg = std::make_shared<msg_logger>();

    ediplotter w;
    w.set_msg_logger(msg);
    w.show();

    return a.exec();
}
