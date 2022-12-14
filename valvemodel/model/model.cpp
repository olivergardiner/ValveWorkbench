#include "model.h"

/**
 * @brief Model::anodeVoltage
 * @param ia The desired anode current
 * @param vg1 The grid voltage
 * @param vg2 For pentodes, the screen grid voltage
 * @return The anode voltage that will result in the desired anode current
 *
 * Uses a gradient based search to find the anode voltage that will result in the specified
 * anode current given the specified grid voltages. This is provided to enable the accurate
 * determination of a cathode load line.
 */
double Model::anodeVoltage(double ia, double vg1, double vg2)
{
    double va = 100.0;
    double tolerance = 1.2;

    double iaTest = anodeCurrent(va, vg1, vg2);
    double gradient = iaTest - anodeCurrent(va - 1.0, vg1, vg2);
    double iaErr = ia - iaTest;

    while ((abs(iaErr) / ia) > 0.01) {
        if (gradient != 0.0) {
            double vaNext = va + iaErr / gradient;
            if (vaNext < va / tolerance) { // use the gradient but limit step to 20%
                vaNext = va / tolerance;
            }
            if (vaNext > tolerance * va) { // use the gradient but limit step to 20%
                vaNext = tolerance * va;
            }
            va = vaNext;
        } else {
            va = 2 * va;
        }
        iaTest = anodeCurrent(va, vg1, vg2);
        gradient = iaTest - anodeCurrent(va - 1.0, vg1, vg2);
        iaErr = ia - iaTest;
    }

    return va;
}

void Model::addMeasurement(Measurement *measurement)
{
    int sweeps = measurement->count();
    for (int s = 0; s < sweeps; s++) {
        Sweep *sweep = measurement->at(s);

        int samples = sweep->count();
            for (int i = 0; i < samples; i++) {
            Sample *sample = sweep->at(i);

            addSample(sample->getVa(), sample->getIa(), sample->getVg1(), sample->getVg2());
        }
    }
}

void Model::addMeasurements(QList<Measurement *> *measurements)
{
    for (int m = 0; m < measurements->count(); m++) {
        addMeasurement(measurements->at(m));
    }
}

void Model::setEstimate(Estimate *estimate)
{
    parameter[PAR_MU]->setValue(estimate->getMu());
    parameter[PAR_KG1]->setValue(estimate->getKg1());
    parameter[PAR_X]->setValue(estimate->getX());
    parameter[PAR_KP]->setValue(estimate->getKp());
    parameter[PAR_KVB]->setValue(estimate->getKvb());
    parameter[PAR_KVB1]->setValue(estimate->getKvb1());
    parameter[PAR_VCT]->setValue(estimate->getVct());

    // TODO: Add pentode parameter estimates
}

void Model::solve()
{
    setOptions();

    Solver::Summary summary;
    Solve(options, &problem, &summary);

    qInfo(summary.FullReport().c_str());
}

QTreeWidgetItem *Model::buildTree(QTreeWidgetItem *parent)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(parent, TYP_MODEL);

    item->setText(0, "Model");
    item->setIcon(0, QIcon(":/icons/estimate32.png"));
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    item->setData(0, Qt::UserRole, QVariant::fromValue((void *) this));

    parent->setExpanded(true);

    return item;
}

void Model::setLowerBound(Parameter* parameter, double lowerBound)
{
    problem.SetParameterLowerBound(parameter->getPointer(), 0, lowerBound);
}

void Model::setUpperBound(Parameter* parameter, double upperBound)
{
    problem.SetParameterUpperBound(parameter->getPointer(), 0, upperBound);
}

void Model::setLimits(Parameter* parameter, double lowerBound, double upperBound)
{
    problem.SetParameterLowerBound(parameter->getPointer(), 0, lowerBound);
    problem.SetParameterUpperBound(parameter->getPointer(), 0, upperBound);
}
