#include "txm.h"

TXM::TXM(const QString name_node, QObject *parent)   :  QObject(parent)
{
    this->name_node = name_node;
    this->measdoc = std::make_unique<measdocxml>(this);
    connect(this->measdoc.get(), &measdocxml::signal_txm_cycle_qmap_created, this, &TXM::slot_txm_cycle_qmap_created);
    connect(this->measdoc.get(), &measdocxml::signal_txm_sequence_qmap_created, this, &TXM::slot_txm_sequence_qmap_created);
    connect(this->measdoc.get(), &measdocxml::signal_txm_waveform_qmap_created, this, &TXM::slot_txm_waveform_qmap_created);
    connect(this->measdoc.get(), &measdocxml::signal_txm_epos_qmap_created, this, &TXM::slot_txm_epos_qmap_created);

    this->measdoc->create_txm(true); // get it by connections




}

bool TXM::set(const QString &what, const QVariant &value)
{

    if (txm_waveform.contains(what)) {
        this->txm_waveform.insert(what, get_native(value, txm_waveform.value(what)));
        this->is_set = true;
        return true;
    }

    if (txm_sequence.contains(what)) {
        this->txm_sequence.insert(what, get_native(value, txm_sequence.value(what)));
        this->is_set = true;
        return true;
    }

    if (txm_cycle.contains(what)) {
        this->txm_cycle.insert(what, get_native(value, txm_cycle.value(what)));
        this->is_set = true;
        return true;
    }

    if (txm_epos.contains(what)) {
        this->txm_epos.insert(what, get_native(value, txm_epos.value(what)));
        this->is_set = true;
        return true;
    }

    return false;
}

QVariant TXM::get(const QString &what) const
{

    if (txm_waveform.contains(what)) {
        return this->txm_waveform.value(what);
    }

    if (txm_sequence.contains(what)) {
        return this->txm_sequence.value(what);
    }

    if (txm_cycle.contains(what)) {
        return this->txm_cycle.value(what);
    }

    if (txm_epos.contains(what)) {
        return this->txm_epos.value(what);
    }

    return QVariant();
}

bool TXM::is_active()
{
    if (this->txm_cycle.value("BaseFrequency").toDouble() < 0.0000001) this->is_set = false;
    return this->is_set;
}

QString TXM::getXmlStr() const
{
    tinyxmlwriter tix;

    tix.clear(false);
    tix.push(this->name_node);

    tix.push("Waveform");
    tix.element("Name", txm_waveform.value("Waveform").toString());

    // limit, if 40 take all power if possible
    tix.element("MaxAmplitude", txm_waveform.value("MaxAmplitude").toDouble());
    tix.element("NumSamplPoints", txm_waveform.value("NumSamplPoints").toInt());
    tix.emptyelement("SamplePointList");
    tix.pop("Waveform");

    tix.push("Sequence");

    // is a remider
    tix.element("Name", txm_sequence.value("Name").toString());
    tix.element("ActivateRotDipole", txm_sequence.value("ActivateRotDipole").toInt());
    tix.element("ActivateAltDipole", txm_sequence.value("ActivateAltDipole").toInt());
    tix.element("DipoleRotFreq", txm_sequence.value("DipoleRotFreq").toDouble());


    tix.push("TransmissionList");
    tix.push("Cycle", "id", 0);
    tix.element("BaseFrequency", txm_cycle.value("BaseFrequency").toDouble());
    tix.element("Iterations", txm_cycle.value("Iterations").toInt());

    // 90 = E-W 0 = N-S orientation = geo (0 = N)
    tix.element("Polarisation", "dipole", 0, txm_cycle.value("Polarisation_dipole_0").toDouble());
    tix.element("Polarisation", "dipole", 1, txm_cycle.value("Polarisation_dipole_1").toDouble());
    tix.pop("Cycle");
    tix.pop("TransmissionList");

    tix.pop("Sequence");


    tix.push("ElectrodePositions");


    tix.push("E1");
    tix.push("Position", "id", 0);
    tix.element("Longitude", this->txm_epos.value("E1_Position_0_Longitude").toInt());
    tix.element("Latitude", this->txm_epos.value("E1_Position_0_Latitude").toInt());
    tix.element("Elevation", this->txm_epos.value("E1_Position_0_Elevation").toInt());
    tix.pop("Position");

    tix.push("Position", "id", 1);
    tix.element("Longitude", this->txm_epos.value("E1_Position_1_Longitude").toInt());
    tix.element("Latitude", this->txm_epos.value("E1_Position_1_Latitude").toInt());
    tix.element("Elevation", this->txm_epos.value("E1_Position_1_Elevation").toInt());
    tix.pop("Position");
    tix.pop("E1");

    tix.push("E2");
    tix.push("Position", "id", 0);
    tix.element("Longitude", this->txm_epos.value("E2_Position_0_Longitude").toInt());
    tix.element("Latitude", this->txm_epos.value("E2_Position_0_Latitude").toInt());
    tix.element("Elevation", this->txm_epos.value("E2_Position_0_Elevation").toInt());
    tix.pop("Position");

    tix.push("Position", "id", 1);
    tix.element("Longitude", this->txm_epos.value("E2_Position_1_Longitude").toInt());
    tix.element("Latitude", this->txm_epos.value("E2_Position_1_Latitude").toInt());
    tix.element("Elevation", this->txm_epos.value("E2_Position_1_Elevation").toInt());
    tix.pop("Position");
    tix.pop("E2");

    tix.push("E3");
    tix.push("Position", "id", 0);
    tix.element("Longitude", this->txm_epos.value("E3_Position_0_Longitude").toInt());
    tix.element("Latitude", this->txm_epos.value("E3_Position_0_Latitude").toInt());
    tix.element("Elevation", this->txm_epos.value("E3_Position_0_Elevation").toInt());
    tix.pop("Position");

    tix.push("Position", "id", 1);
    tix.element("Longitude", this->txm_epos.value("E3_Position_1_Longitude").toInt());
    tix.element("Latitude", this->txm_epos.value("E3_Position_1_Latitude").toInt());
    tix.element("Elevation", this->txm_epos.value("E3_Position_1_Elevation").toInt());
    tix.pop("Position");
    tix.pop("E3");


    tix.pop("ElectrodePositions");

    tix.pop(this->name_node);

    return tix.getXmlStr();

}

bool TXM::contains(const QString &what)
{
    if (this->txm_cycle.contains(what)) return true;
    if (this->txm_waveform.contains(what)) return true;
    if (this->txm_sequence.contains(what)) return true;
    if (this->txm_epos.contains(what)) return true;
    return false;
}

void TXM::slot_txm_cycle_qmap_created(const QMap<QString, QVariant> &data_map)
{
    this->txm_cycle = data_map;
}

void TXM::slot_txm_sequence_qmap_created(const QMap<QString, QVariant> &data_map)
{
    this->txm_sequence = data_map;
}

void TXM::slot_txm_waveform_qmap_created(const QMap<QString, QVariant> &data_map)
{
    this->txm_waveform = data_map;
}

void TXM::slot_txm_epos_qmap_created(const QMap<QString, QVariant> &data_map)
{
    this->txm_epos = data_map;
}
