#include "atmfile.h"


atmfile::atmfile(QObject *parent):  QObject(parent)
{
    this->atm.siHeaderLength = 8;
    this->atm.siHeaderVers = 10;
    this->atm.iSamples = 0;
}



atmfile::atmfile(const QFileInfo &atsfile_atmfile, QObject *parent):  QObject(parent)
{
    QString f(atsfile_atmfile.absolutePath() + "/" + atsfile_atmfile.completeBaseName() + ".atm" );
    this->setFile(f);
}

void atmfile::merge(const atmfile &other, const size_t starts, const size_t stops)
{
    if ((starts == 0) && (stops == 0)) {
        if (other.b.size() != this->b.size()) return;
        this->b |= other.b;
    }

}

QBitArray atmfile::get_slice(const size_t starts_at, const size_t stops_at) const
{
    QBitArray slice(stops_at - starts_at);
    size_t j = 0;
    for (size_t i = starts_at; i < stops_at; ++i) {
        slice.setBit(j++, this->b.testBit(i));
    }

    return slice;
}

void atmfile::set_slice(const QBitArray in, const size_t starts_at, const size_t stops_at)
{
    size_t j = 0;
    if ((stops_at <= this->b.size()) && (starts_at < stops_at) && (stops_at <= in.size())) {
        for (size_t i = starts_at; i < stops_at; ++i) {
            this->b.setBit(i, in.testBit(j++));
        }
    }
}

void atmfile::set_skip_slice(const size_t starts_at, const size_t stops_at)
{
    if (stops_at < this->size()) {
        for (size_t i = starts_at; i < stops_at; ++i) {
            this->b.setBit(i);
        }
    }
}



atmfile::~atmfile()
{
    if (this->file.isOpen()) this->file.close();
}

size_t atmfile::resize(const size_t &size)
{
    this->b.resize(size);
    this->atm.iSamples = uint32_t(b.size());

    return size_t(b.size());

}

size_t atmfile::free_atm_window(const size_t &sample_start, const size_t &window_length)
{
    if ((sample_start == SIZE_MAX) ||  (window_length == SIZE_MAX)) return SIZE_MAX;
    if (sample_start + window_length > b.size()) return SIZE_MAX;

    size_t sum = 0;
    size_t j = 0;

    do {
        if ((sample_start + j) == this->b.size()) {
            qDebug() << "atm EOF, seek for free window ends";
            return SIZE_MAX;
        }
        else if (!this->b.testBit(sample_start + j)) {
            ++sum;                                    // ok, bit is false (not marked) free sum up intil we reach a window length
        }
        else {
            sum = 0;                                  // reset in case we have true (marked, excluded), count again
        }
        ++j;

    }  while (sum != window_length);

    if (sample_start + j >= this->b.size()) return SIZE_MAX;
    //  qDebug() << sample_start + j - window_length;
    if ( (sample_start + j) < window_length) return SIZE_MAX; // could not find a free window anymore
    return sample_start + j  - window_length;

}

void atmfile::fir_filter(const atmfile &in, const size_t starts_at, const size_t filter_factor)
{
    size_t i, j = 0;
    i = starts_at;
    while (i < in.b.size() && j < this->b.size()) {

        if(in.b.testBit(i)) this->b.setBit(j);
        ++j;
        i += filter_factor;
    }
}

size_t atmfile::read_all()
{
    if (this->file.isOpen()) this->file.close();
    this->file.setFileName(this->absoluteFilePath());
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream qds(&file);
        qds.setByteOrder(QDataStream::LittleEndian);
        qds >> this->atm.siHeaderLength;
        qds >> this->atm.siHeaderVers;

        qds >> this->atm.iSamples;
        file.seek(sizeof(this->atm.siHeaderLength) + sizeof(this->atm.siHeaderVers) );


        if (this->atm.siHeaderVers > 80) {
            this->file.close();
            return 0;

        }

        b.resize(this->atm.iSamples);
        qds >> b;
        this->file.close();
        return b.size();

    }

    return 0;
}

size_t atmfile::write_all()
{
    if (!this->b.size()) return 0;
    if (this->file.isOpen()) this->file.close();
    this->file.setFileName(this->absoluteFilePath());

    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
        QDataStream qds(&file);
        qds.setByteOrder(QDataStream::LittleEndian);
        qds << qint16 (this->atm.siHeaderLength);
        qds << qint16 (this->atm.siHeaderVers);
        // qds << quint32(this->b.size()); NO!
        // the selector has a quint32 header
        qds << b;
        this->file.close();
        return b.size();

    }
    return 0;
}
