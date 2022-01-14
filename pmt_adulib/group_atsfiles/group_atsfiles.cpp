#include "group_atsfiles.h"

group_atsfiles::group_atsfiles(const QList<QFileInfo> &atsh_qfi)
{
    for (auto &qfi : atsh_qfi) {
        if (qfi.exists()) this->qlatsh.append(std::make_shared<atsheader>(qfi));
    }
    for (auto &atsh : qlatsh) {
        atsh->scan_header_close();
    }


}

group_atsfiles::~group_atsfiles()
{

}

QList<QList<QFileInfo> > group_atsfiles::same_recordings()
{
    QList<QList<QFileInfo>> records;
    QList<QList<std::shared_ptr<atsheader>>> records_ats;

    if (!this->qlatsh.size()) return records;
    QList<std::shared_ptr<atsheader>> qfil;
    while (this->qlatsh.size()) {
        qfil.append(qlatsh.first());
        qlatsh.removeFirst();
        auto it = qlatsh.begin();
        while (it != qlatsh.end()) {
            if (same_recording(qfil.first(), *it)) {
                qfil.append(*it);
                it = qlatsh.erase(it);
            }
            else ++it;
        }
        records_ats.append(qfil);
        qfil.clear();

    }

    for (auto const &glst : records_ats) {
        QList<QFileInfo> atsf;
         for (auto &llst : glst) {
             atsf.append(QFileInfo(llst->absoluteFilePath()));
         }
         records.append(atsf);
    }

    return records;
}
