#pragma once

#include "simpletriode.h"

class KorenTriode : public SimpleTriode
{
    Q_OBJECT
public:
    KorenTriode();

    virtual void addSample(double va, double ia, double vg1, double vg2 = 0.0, double ig2 = 0.0);
    virtual double triodeAnodeCurrent(double va, double vg1);
    virtual double anodeCurrent(double va, double vg1, double vg2 = 0.0, bool secondaryEmission = true);
    virtual void fromJson(QJsonObject source);
    virtual void toJson(QJsonObject &destination);
    virtual void updateUI(QLabel *labels[], QLineEdit *values[]);
    virtual QString getName();
    virtual int getType();

    virtual void updateProperties(QTableWidget *properties);

protected:
	void setOptions();
    double korenCurrent(double va, double vg, double kp, double kvb, double a, double mu);
};
