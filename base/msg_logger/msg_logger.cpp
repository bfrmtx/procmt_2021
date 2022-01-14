#include "msg_logger.h"


msg_logger::msg_logger(QObject *parent) : QObject(parent)
{
}

void msg_logger::add_message(const QVariant &qc0, const QVariant &qc1, const QVariant &qc2, const QVariant &qc3)
{
    this->c0.emplace(qc0);
    this->c1.emplace(qc1);
    this->c2.emplace(qc2);
    this->c3.emplace(qc3);
}

void msg_logger::show_and_empty()
{
    std::unique_lock<std::mutex> l(this->lock);

    if (this->c0.empty()) return;
    while (!this->c0.empty())
    {
        std::cout << this->c0.front().toString().toStdString() << "  ";
        std::cout << this->c1.front().toString().toStdString() << "  ";
        std::cout << this->c2.front().toString().toStdString() << "  ";
        std::cout << this->c3.front().toString().toStdString() << std::endl;
        this->c0.pop(); this->c1.pop(); this->c2.pop(); this->c3.pop();
    }
}

int msg_logger::size()
{
    std::unique_lock<std::mutex> l(this->lock);
    return this->c0.size();
}

QStringList msg_logger::get_items()
{
    std::unique_lock<std::mutex> l(this->lock);

    QStringList ret;

    if (this->c1.empty()) return ret;
    ret.append(this->c0.front().toString());
    ret.append(this->c1.front().toString());
    ret.append(this->c2.front().toString());
    ret.append(this->c3.front().toString());

    c0.pop(); c1.pop(); c2.pop();c3.pop();

    return ret;
}

QStringList msg_logger::get_all_as_single_lines()
{
    std::unique_lock<std::mutex> l(this->lock);

    QStringList strlst;
    QString line;
    if (this->c0.empty()) return strlst;
    while (!this->c0.empty())
    {
        line.append(this->c0.front().toString()); line.append(" ");
        line.append(this->c1.front().toString()); line.append(" ");
        line.append(this->c2.front().toString()); line.append(" ");
        line.append(this->c3.front().toString()); line.append(" ");
        this->c0.pop(); this->c1.pop(); this->c2.pop(); this->c3.pop();
        strlst.append(line);
        line.clear();
    }

    return strlst;
}

void msg_logger::clear() {

    std::unique_lock<std::mutex> l(this->lock);

    // instead of pop one by one I replace by an empty container
    std::queue<QVariant> d0;
    std::queue<QVariant> d1;
    std::queue<QVariant> d2;
    std::queue<QVariant> d3;

    std::swap(this->c0, d0);
    std::swap(this->c1, d1);
    std::swap(this->c2, d2);
    std::swap(this->c3, d3);

}

bool msg_logger::isEmpty()
{
    std::unique_lock<std::mutex> l(this->lock);
    return c0.empty();
}

void msg_logger::parzen_radius_changed(const double &old_prz, const double &prz)
{
    std::unique_lock<std::mutex> l(this->lock);

    QVariant c0(QString("parzen_radius_changed"));
    QString m("from "); m.append(QString::number(old_prz, 'f', 3)); m.append(" to ");
    m.append(QString::number(prz, 'f', 3));
    QVariant c1(QString(" "));
    QVariant c2(QString(" "));
    QVariant c3(m);
    this->add_message(c0, c1, c2, c3);

}

void msg_logger::rx_cal_message(const int &channel, const int &slot, const QString &message)
{
    std::unique_lock<std::mutex> l(this->lock);

    QVariant c0(QString("cal message"));
    QVariant c1(QString("channel " + QString::number(channel)));
    QVariant c2(QString("slot " + QString::number(slot)));
    QVariant c3(message);
    this->add_message(c0, c1, c2, c3);
}

void msg_logger::rx_adu_msg(const int &system_serial_number, const int &channel, const int &slot, const QString &message)
{
    std::unique_lock<std::mutex> l(this->lock);

    QVariant c0(QString("adu message "  + QString::number(system_serial_number)));
    QVariant c1(QString("channel " + QString::number(channel)));
    QVariant c2(QString("slot " + QString::number(slot)));
    QVariant c3(message);
    this->add_message(c0, c1, c2, c3);
}

void msg_logger::slot_general_msg(const QString &sender, const QString &component, const QString &message)
{
    std::unique_lock<std::mutex> l(this->lock);

    QVariant c0(sender);
    QVariant c1(QString(""));
    QVariant c2(component);
    QVariant c3(message);
    this->add_message(c0, c1, c2, c3);
}

void msg_logger::slot_general_msg_nums(const QString &sender_and_msg, const QVariant num_1, const QString &num_seperator, const QVariant num_2)
{
    std::unique_lock<std::mutex> l(this->lock);

    QVariant c0(sender_and_msg);
    QVariant c1(num_1);
    QVariant c2(num_seperator);
    QVariant c3(num_2);
    this->add_message(c0, c1, c2, c3);
}

void msg_logger::slot_general_msg_4strs(const QString &sender_and_msg, const QString &field2, const QString &field3, const QString &field4)
{
    std::unique_lock<std::mutex> l(this->lock);

    QVariant c0(sender_and_msg);
    QVariant c1(field2);
    QVariant c2(field3);
    QVariant c3(field4);
    this->add_message(c0, c1, c2, c3);
}
