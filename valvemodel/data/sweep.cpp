#include "sweep.h"

Sweep::Sweep(eSweepType type_) : type(type_)
{

}

Sweep::Sweep(int deviceType, int testType)
{
    if (deviceType == TRIODE) {
        type = SWEEP_TRIODE_ANODE;
    } else {
        switch (testType) {
        case ANODE_CHARACTERISTICS:
            type = SWEEP_PENTODE_ANODE;
            break;
        case SCREEN_CHARACTERISTICS:
            type = SWEEP_PENTODE_SCREEN;
            break;
        }
    }
}

void Sweep::addSample(Sample *sample)
{
    samples.append(sample);
}

int Sweep::count()
{
    return samples.size();
}

Sample *Sweep::at(int index)
{
    return samples.at(index);
}

void Sweep::fromJson(QJsonObject source)
{
    switch(type) {
    case SWEEP_TRIODE_ANODE:
        if (source.contains("vg1Nominal") && source["vg1Nominal"].isDouble()) {
            vg1Nominal = source["vg1Nominal"].toDouble();
        }
        break;
    case SWEEP_PENTODE_ANODE:
        if (source.contains("vg1Nominal") && source["vg1Nominal"].isDouble()) {
            vg1Nominal = source["vg1Nominal"].toDouble();
        }

        if (source.contains("vg2Nominal") && source["vg2Nominal"].isDouble()) {
            vg2Nominal = source["vg2Nominal"].toDouble();
        }
        break;
    case SWEEP_PENTODE_SCREEN:
        if (source.contains("vg1Nominal") && source["vg1Nominal"].isDouble()) {
            vg1Nominal = source["vg1Nominal"].toDouble();
        }

        if (source.contains("vaNominal") && source["vaNominal"].isDouble()) {
            vaNominal = source["vaNominal"].toDouble();
        }
        break;
    }

    samples.clear();

    if (source.contains("samples") && source["samples"].isArray()) {
        QJsonArray jsonSamples = source["samples"].toArray();
        for (int i = 0; i < jsonSamples.size(); i++) {
            if (jsonSamples.at(i).isObject()) {
                Sample *sample = new Sample();
                sample->fromJson(jsonSamples.at(i).toObject());

                samples.append(sample);
            }
        }
    }
}

void Sweep::toJson(QJsonObject &destination)
{
    switch(type) {
    case SWEEP_TRIODE_ANODE:
        destination["vg1Nominal"] = vg1Nominal;
        break;
    case SWEEP_PENTODE_ANODE:
        destination["vg1Nominal"] = vg1Nominal;
        destination["vg2Nominal"] = vg2Nominal;
        break;
    case SWEEP_PENTODE_SCREEN:
        destination["vg1Nominal"] = vg1Nominal;
        destination["vaNominal"] = vaNominal;
        break;
    }

    QJsonArray jsonSamples;
    for (int i = 0; i < samples.length(); i++) {
        QJsonObject sample;
        samples.at(i)->toJson(sample);

        jsonSamples.append(sample);
    }
    destination["samples"] = jsonSamples;
}

QTreeWidgetItem *Sweep::buildTree(QTreeWidgetItem *parent)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(parent, TYP_SWEEP);

    item->setText(0, sweepName());
    item->setIcon(0, QIcon(":/icons/sweep32.png"));
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    item->setData(0, Qt::UserRole, QVariant::fromValue((void *) this));

    for (int i = 0; i < samples.size(); i++) {
        samples.at(i)->buildTree(item);
    }

    return item;
}

QString Sweep::sweepName()
{
    switch(type) {
    case SWEEP_TRIODE_ANODE:
        return QString("Vg1 = %1").arg(vg1Nominal);
        break;
    case SWEEP_PENTODE_ANODE:
        return QString("Vg1 = %1, Vg2 = %2").arg(vg1Nominal).arg(vg2Nominal);
        break;
    case SWEEP_PENTODE_SCREEN:
        return QString("Vg1 = %1, Va = %2").arg(vg1Nominal).arg(vaNominal);
        break;
    }

    return QString("");
}

void Sweep::updateProperties(QTableWidget *properties)
{
    clearProperties(properties);

    switch(type) {
    case SWEEP_TRIODE_ANODE:
        addProperty(properties, "Type", "Triode Anode");
        addProperty(properties, "Vg1", QString("%1").arg(vg1Nominal));
        break;
    case SWEEP_PENTODE_ANODE:
        addProperty(properties, "Type", "Pentode Anode");
        addProperty(properties, "Vg1", QString("%1").arg(vg1Nominal));
        addProperty(properties, "Vg2", QString("%1").arg(vg2Nominal));
        break;
    case SWEEP_PENTODE_SCREEN:
        addProperty(properties, "Type", "Pentode Screen");
        addProperty(properties, "Vg1", QString("%1").arg(vg1Nominal));
        addProperty(properties, "Va", QString("%1").arg(vaNominal));
        break;
    }
}

void Sweep::updatePlot(Plot *plot)
{

}

double Sweep::getVaNominal() const
{
    return vaNominal;
}

void Sweep::setVaNominal(double newVaNominal)
{
    vaNominal = newVaNominal;
}

double Sweep::getVg1Nominal() const
{
    return vg1Nominal;
}

void Sweep::setVg1Nominal(double newVg1Nominal)
{
    vg1Nominal = newVg1Nominal;
}

double Sweep::getVg2Nominal() const
{
    return vg2Nominal;
}

void Sweep::setVg2Nominal(double newVg2Nominal)
{
    vg2Nominal = newVg2Nominal;
}