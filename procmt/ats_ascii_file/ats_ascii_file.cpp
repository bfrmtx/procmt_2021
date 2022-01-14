#include "ats_ascii_file.h"


ats_ascii_file::ats_ascii_file(const QFileInfo &qfi, const QString file_extension, const size_t slot, QObject *parent)  :  QObject(parent)
{
    this->setFile(qfi.absolutePath() + "/" + qfi.completeBaseName() + file_extension);
    this->slot = slot;


}

ats_ascii_file::ats_ascii_file(const QString &qfi_str, const QString file_extension, const size_t slot, QObject *parent)   :  QObject(parent), QFileInfo(qfi_str)
{
    this->setFile(qfi_str + file_extension);
    this->slot = slot;
}



ats_ascii_file::~ats_ascii_file()
{
    if (this->file.isOpen()) this->file.close();
    // only the producer will set the finished status
    // the consumer runs always up to that
    this->reset_buffer();
    qDebug() << "ats_ascii_file deleted, slot" << this->slot << " writings" << this->ats_write_counts;
}

void ats_ascii_file::set_buffer(std::shared_ptr<threadbuffer<double> > &buffer)
{
    std::cerr << "ats_ascii_file[" << this->slot <<  "] init from threadbuffer[" << buffer->get_id() << "] " << buffer->get_vector_size() << std::endl;
    this->buffer = buffer;
    this->buffer_type = buffer->get_buffer_type();
    this->ascii_data.reserve(buffer->get_vector_size());
    this->ascii_data.resize(buffer->get_vector_size());
}

void ats_ascii_file::reset_buffer()
{
    if (this->buffer != nullptr) this->buffer.reset();
}

void ats_ascii_file::set_frequencies(const std::vector<double> &f)
{
    this->freq = f;
}


bool ats_ascii_file::open()
{
    this->file.setFileName(this->absoluteFilePath());
    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate)) {

        this->qts.setDevice(&this->file);
        this->qts.setRealNumberNotation(QTextStream::ScientificNotation);
        this->qts.setRealNumberPrecision(10);

    }

    else {
        qWarning()<< "ats_ascii_file::open " << " ->   -> can not open" << this->absoluteFilePath();
    }

    return this->file.isOpen();
}

void ats_ascii_file::close()
{
    if (this->file.isOpen()) this->file.close();
}

void ats_ascii_file::set_eQDatetime(const eQDateTime &edt)
{
    if (this->edt != nullptr) this->edt.reset();
    this->edt = std::make_unique<eQDateTime>(edt);
    this->starts_at = this->edt->get_start_sample();
}

size_t ats_ascii_file::write_all_data(const std::vector<double> &all_ts_scaled, const int signal_guess_up_to_here, const int signal_guess_is_when_finished)
{

    this->ats_write_counts = 0;
    size_t hugh_count = 0;
    size_t slice;
    if (signal_guess_is_when_finished > signal_guess_up_to_here) {
        slice = all_ts_scaled.size()  / size_t(signal_guess_is_when_finished - signal_guess_up_to_here );
    }
    else slice = all_ts_scaled.size() / 40;

    int i = 0;
    if (this->open()) {
        if (this->edt == nullptr) {
        for (auto &dat : all_ts_scaled) {
            this->qts << dat << Qt::endl;
            this->ats_write_counts++;
            ++hugh_count;
            if (hugh_count == slice) {
                hugh_count = 0;
                emit this->signal_guess_100(signal_guess_up_to_here + ++i);
                //std::cerr << "u" << std::endl;
            }
        }
        }
        else {
            for (auto &dat : all_ts_scaled) {
                this->qts << this->edt->ISO_date_time_frac(this->starts_at++) << "  " << dat << Qt::endl;
                this->ats_write_counts++;
                ++hugh_count;
                if (hugh_count == slice) {
                    hugh_count = 0;
                    emit this->signal_guess_100(signal_guess_up_to_here + ++i);
                    //std::cerr << "u" << std::endl;
                }
            }
        }
    }
    this->close();
    return this->ats_write_counts;
}

QTextStream::Status ats_ascii_file::write()
{
    // the buffer size should change automatically

    // time series
    if (this->buffer_type < atsfileout::atsfileout_spectra) {
        do {
            buffer->fetch(this->ascii_data, this->buffer_status);
            //std::cerr << "atswriter[" << this->slot << "] writing" << std::endl;
            if (this->edt == nullptr) {
                for (auto &dat : this->ascii_data) {
                    this->qts << dat << Qt::endl;
                }
            }
            else {                                              //ISO_date_time_frac
                for (auto &dat : this->ascii_data) {
                    this->qts << this->edt->ISO_date_time_frac(this->starts_at++) << "  " << dat << Qt::endl;
                }
            }
            this->ats_write_counts++;

        } while (this->buffer_status == threadbuffer_status::running);
    }

    // that should be a single shot mostly; double type
    if ( (this->buffer_type >= atsfileout::atsfileout_stacked_spectra) && (this->buffer_type <= atsfileout::atsfileout_stacked_amplitude_spectra)) {
        size_t i = 0;
        do {
            buffer->fetch(this->ascii_data, this->buffer_status);
            i = 0;
            //std::cerr << "atswriter[" << this->slot << "] writing" << std::endl;
            for (auto &dat : this->ascii_data) {
                this->qts << this->freq[i++] << "  " << dat << Qt::endl;
            }

            this->ats_write_counts++;

        } while (this->buffer_status == threadbuffer_status::running);
    }
    return this->qts.status();
}

