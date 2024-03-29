#include "gardinerpentode.h"

//#include <cmath>

struct UnifiedPentodeIaResidual {
    UnifiedPentodeIaResidual(double va, double vg1, double ia, double vg2, double ig2) : va_(va), vg1_(vg1), ia_(ia), vg2_(vg2), ig2_(ig2) {}

    template <typename T>
    bool operator()(const T* const kg1, const T* const kp, const T* const kvb, const T* const kvb1, const T* const vct, const T* const x, const T* const mu, const T* const kg2, const T* const a, const T* const alpha, const T* const beta, const T* const gamma, const T* const os, T* residual) const {
        T f = sqrt(kvb[0] + kvb1[0] * vg2_ + vg2_ * vg2_);
        T epk = pow(vg2_ * log(1.0 + exp(kp[0] * (1.0 / mu[0] + (vg1_ + vct[0]) / f))) / kp[0], x[0]);
        T shift = beta[0] * (1.0 - alpha[0] * vg1_);
        T g = exp(-pow(shift * va_, gamma[0]));
        //T g = 1.0 / (1.0 + pow(shift * va_, gamma[0]));
        if (isnan(g)) { // Should only happen if Va is 0 and this is a better test than == 0.0
            g = mu[0] / mu[0];
        }
        T scale = 1.0 - g;
        T ia = epk * ((1.0 / kg1[0] - 1.0 / kg2[0]) * scale + a[0] * va_ / kg2[0]) + os[0] * vg2_;

        //double w = exp(va_/ 250.0);
        if (!(isnan(ia) || isinf(ia))) {
            //residual[0] = (ia_ - ia) * w;
            residual[0] = (ia_ - ia);
        } else {
            return false;
        }
        return true;
    }

private:
    const double va_;
    const double vg1_;
    const double ia_;
    const double vg2_;
    const double ig2_;
};

struct UnifiedPentodeIaSEResidual {
    UnifiedPentodeIaSEResidual(double va, double vg1, double ia, double vg2, double ig2) : va_(va), vg1_(vg1), ia_(ia), vg2_(vg2), ig2_(ig2) {}

    template <typename T>
    bool operator()(const T* const kg1, const T* const kp, const T* const kvb, const T* const kvb1, const T* const vct, const T* const x, const T* const mu, const T* const kg2, const T* const a, const T* const alpha, const T* const beta, const T* const gamma, const T* const os, const T* const omega, const T* const lambda, const T* const nu, const T* const s, const T* const ap, T* residual) const {
        T f = sqrt(kvb[0] + kvb1[0] * vg2_ + vg2_ * vg2_);
        T epk = pow(vg2_ * log(1.0 + exp(kp[0] * (1.0 / mu[0] + (vg1_ + vct[0]) / f))) / kp[0], x[0]);
        T shift = beta[0] * (1.0 - alpha[0] * vg1_);
        T g = exp(-pow(shift * va_, gamma[0]));
        //T g = 1.0 / (1.0 + pow(shift * va_, gamma[0]));
        if (isnan(g)) { // Should only happen if Va is 0 and this is a better test than == 0.0
            g = mu[0] / mu[0];
        }
        T scale = 1.0 - g;
        T vco = vg2_ / lambda[0] - vg1_ * nu[0] - omega[0];
        T psec = s[0] * va_ * (1.0 + tanh(-ap[0] * (va_ - vco)));
        T ia = epk * ((1.0 / kg1[0] - 1.0 / kg2[0]) * scale + a[0] * va_ / kg2[0] - psec / kg2[0]) + os[0] * vg2_;

        //double w = exp(va_/ 250.0);
        if (!(isnan(ia) || isinf(ia))) {
            //residual[0] = (ia_ - ia) * w;
            residual[0] = (ia_ - ia);
        } else {
            return false;
        }
        return true;
    }

private:
    const double va_;
    const double vg1_;
    const double ia_;
    const double vg2_;
    const double ig2_;
};

struct UnifiedPentodeIg2Residual {
    UnifiedPentodeIg2Residual(double va, double vg1, double ia, double vg2, double ig2) : va_(va), vg1_(vg1), ia_(ia), vg2_(vg2), ig2_(ig2) {}

    template <typename T>
    bool operator()(const T* const kp, const T* const kvb, const T* const kvb1, const T* const vct, const T* const x, const T* const mu, const T* const kg3, const T* const a, const T* const tau, const T* const rho, const T* const theta, const T* const psi, T* residual) const {
        T f = sqrt(kvb[0] + kvb1[0] * vg2_ + vg2_ * vg2_);
        T epk = pow(vg2_ * log(1.0 + exp(kp[0] * (1.0 / mu[0] + (vg1_ + vct[0]) / f))) / kp[0], x[0]);
        T shift = rho[0] * (1.0 - tau[0] * vg1_);
        T h = exp(-pow(shift * va_, theta[0]));
        //T h = 1.0 / (1.0 + pow(shift * va_, theta[0]));
        if (isnan(h)) { // Should only happen if Va is 0 and this is a better test than == 0.0
            h = mu[0] / mu[0];
        }
        T ig2 = epk * (1.0 + psi[0] * h) / kg3[0] - epk * a[0] * va_ / kg3[0];
        //T ig2 = epk * (1.0 + psi[0] * h) / kg3[0];

        //double w = exp(va_/ 250.0);
        if (!(isnan(ig2) || isinf(ig2))) {
            //residual[0] = (ig2_ - ig2) * w;
            residual[0] = (ig2_ - ig2);
        } else {
            return false;
        }
        return true;
    }

private:
    const double va_;
    const double vg1_;
    const double ia_;
    const double vg2_;
    const double ig2_;
};

struct UnifiedPentodeIg2SEResidual {
    UnifiedPentodeIg2SEResidual(double va, double vg1, double ia, double vg2, double ig2) : va_(va), vg1_(vg1), ia_(ia), vg2_(vg2), ig2_(ig2) {}

    template <typename T>
    bool operator()(const T* const kp, const T* const kvb, const T* const kvb1, const T* const vct, const T* const x, const T* const mu, const T* const kg3, const T* const a, const T* const tau, const T* const rho, const T* const theta, const T* const psi, const T* const omega, const T* const lambda, const T* const nu, const T* const s,
                    const T* const ap, T* residual) const {
        T f = sqrt(kvb[0] + kvb1[0] * vg2_ + vg2_ * vg2_);
        T epk = pow(vg2_ * log(1.0 + exp(kp[0] * (1.0 / mu[0] + (vg1_ + vct[0]) / f))) / kp[0], x[0]);
        T shift = rho[0] * (1.0 - tau[0] * vg1_);
        T h = exp(-pow(shift * va_, theta[0]));
        //T h = 1.0 / (1.0 + pow(shift * va_, theta[0]));
        if (isnan(h)) { // Should only happen if Va is 0 and this is a better test than == 0.0
            h = mu[0] / mu[0];
        }
        T vco = vg2_ / lambda[0] - vg1_ * nu[0] - omega[0];
        T psec = s[0] * va_ * (1.0 + tanh(-ap[0] * (va_ - vco)));

        T ig2 = epk * (1.0 + psi[0] * h + psec) / kg3[0] - epk * a[0] * va_ / kg3[0];
        //T ig2 = epk * (1.0 + psi[0] * h + psec) / kg3[0];

        //double w = exp(va_/ 250.0);
        if (!(isnan(ig2) || isinf(ig2))) {
            //residual[0] = (ig2_ - ig2) * w;
            residual[0] = (ig2_ - ig2);
        } else {
            return false;
        }
        return true;
    }

private:
    const double va_;
    const double vg1_;
    const double ia_;
    const double vg2_;
    const double ig2_;
};

double GardinerPentode::anodeCurrent(double va, double vg1, double vg2, bool secondaryEmission)
{
    double epk = cohenHelieEpk(vg2, vg1);
    double k = 1.0 / parameter[PAR_KG1]->getValue() - 1.0 / parameter[PAR_KG2]->getValue();
    double shift = parameter[PAR_BETA]->getValue() * (1.0 - parameter[PAR_ALPHA]->getValue() * vg1);
    double g = exp(-pow(shift * va, parameter[PAR_GAMMA]->getValue()));
    //double g = 1.0 / (1.0 + pow(shift * va, parameter[PAR_GAMMA]->getValue()));
    double scale = 1.0 - g;
    double vco = vg2 / parameter[PAR_LAMBDA]->getValue() - vg1 * parameter[PAR_NU]->getValue() - parameter[PAR_OMEGA]->getValue();
    double psec = parameter[PAR_S]->getValue() * va * (1.0 + tanh(-parameter[PAR_AP]->getValue() * (va - vco)));
    double ia = epk * (k * scale + parameter[PAR_A]->getValue() * va / parameter[PAR_KG2]->getValue()) + parameter[PAR_OS]->getValue() * vg2;

    if(secondaryEmission) {
        ia = ia - epk * psec / parameter[PAR_KG2]->getValue();
    }

    return ia;
}

double GardinerPentode::screenCurrent(double va, double vg1, double vg2, bool secondaryEmission)
{
    double epk = cohenHelieEpk(vg2, vg1);
    double shift = parameter[PAR_RHO]->getValue() * (1.0 - parameter[PAR_TAU]->getValue() * vg1);
    double h = exp(-pow(shift * va, parameter[PAR_THETA]->getValue() * 0.9));
    double vco = vg2 / parameter[PAR_LAMBDA]->getValue() - vg1 * parameter[PAR_NU]->getValue() - parameter[PAR_OMEGA]->getValue();
    double psec = parameter[PAR_S]->getValue() * va * (1.0 + tanh(-parameter[PAR_AP]->getValue() * (va - vco)));
    double ig2 = epk * (1.0 + parameter[PAR_PSI]->getValue() * h) / parameter[PAR_KG2A]->getValue() - epk * parameter[PAR_A]->getValue() * va / parameter[PAR_KG2A]->getValue();
    //double ig2 = epk * (1.0 + parameter[PAR_PSI]->getValue() * h) / parameter[PAR_KG3]->getValue();

    if(secondaryEmission) {
        ig2 = ig2 + epk * psec / parameter[PAR_KG2A]->getValue();
    }

    return ig2;
}

GardinerPentode::GardinerPentode()
{
    secondaryEmission = false;
}

void GardinerPentode::addSample(double va, double ia, double vg1, double vg2, double ig2)
{
    if (!preferences->useSecondaryEmission()) {
        anodeProblem.AddResidualBlock(
            new AutoDiffCostFunction<UnifiedPentodeIaResidual, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1>(
                new UnifiedPentodeIaResidual(va, vg1, ia, vg2, ig2)),
            NULL,
            parameter[PAR_KG1]->getPointer(),
            parameter[PAR_KP]->getPointer(),
            parameter[PAR_KVB]->getPointer(),
            parameter[PAR_KVB1]->getPointer(),
            parameter[PAR_VCT]->getPointer(),
            parameter[PAR_X]->getPointer(),
            parameter[PAR_MU]->getPointer(),
            parameter[PAR_KG2]->getPointer(),
            parameter[PAR_A]->getPointer(),
            parameter[PAR_ALPHA]->getPointer(),
            parameter[PAR_BETA]->getPointer(),
            parameter[PAR_GAMMA]->getPointer(),
            parameter[PAR_OS]->getPointer());

        anodeRemodelProblem.AddResidualBlock(
            new AutoDiffCostFunction<UnifiedPentodeIaResidual, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1>(
                new UnifiedPentodeIaResidual(va, vg1, ia, vg2, ig2)),
            NULL,
            parameter[PAR_KG1]->getPointer(),
            parameter[PAR_KP]->getPointer(),
            parameter[PAR_KVB]->getPointer(),
            parameter[PAR_KVB1]->getPointer(),
            parameter[PAR_VCT]->getPointer(),
            parameter[PAR_X]->getPointer(),
            parameter[PAR_MU]->getPointer(),
            parameter[PAR_KG2]->getPointer(),
            parameter[PAR_A]->getPointer(),
            parameter[PAR_ALPHA]->getPointer(),
            parameter[PAR_BETA]->getPointer(),
            parameter[PAR_GAMMA]->getPointer(),
            parameter[PAR_OS]->getPointer());

        screenProblem.AddResidualBlock(
            new AutoDiffCostFunction<UnifiedPentodeIg2Residual, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1>(
                new UnifiedPentodeIg2Residual(va, vg1, ia, vg2, ig2)),
            NULL,
            parameter[PAR_KP]->getPointer(),
            parameter[PAR_KVB]->getPointer(),
            parameter[PAR_KVB1]->getPointer(),
            parameter[PAR_VCT]->getPointer(),
            parameter[PAR_X]->getPointer(),
            parameter[PAR_MU]->getPointer(),
            parameter[PAR_KG2A]->getPointer(),
            parameter[PAR_A]->getPointer(),
            parameter[PAR_TAU]->getPointer(),
            parameter[PAR_RHO]->getPointer(),
            parameter[PAR_THETA]->getPointer(),
            parameter[PAR_PSI]->getPointer());
    } else {
        anodeProblem.AddResidualBlock(
            new AutoDiffCostFunction<UnifiedPentodeIaSEResidual, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1>(
                new UnifiedPentodeIaSEResidual(va, vg1, ia, vg2, ig2)),
            NULL,
            parameter[PAR_KG1]->getPointer(),
            parameter[PAR_KP]->getPointer(),
            parameter[PAR_KVB]->getPointer(),
            parameter[PAR_KVB1]->getPointer(),
            parameter[PAR_VCT]->getPointer(),
            parameter[PAR_X]->getPointer(),
            parameter[PAR_MU]->getPointer(),
            parameter[PAR_KG2]->getPointer(),
            parameter[PAR_A]->getPointer(),
            parameter[PAR_ALPHA]->getPointer(),
            parameter[PAR_BETA]->getPointer(),
            parameter[PAR_GAMMA]->getPointer(),
            parameter[PAR_OS]->getPointer(),
            parameter[PAR_OMEGA]->getPointer(),
            parameter[PAR_LAMBDA]->getPointer(),
            parameter[PAR_NU]->getPointer(),
            parameter[PAR_S]->getPointer(),
            parameter[PAR_AP]->getPointer());

        anodeRemodelProblem.AddResidualBlock(
            new AutoDiffCostFunction<UnifiedPentodeIaSEResidual, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1>(
                new UnifiedPentodeIaSEResidual(va, vg1, ia, vg2, ig2)),
            NULL,
            parameter[PAR_KG1]->getPointer(),
            parameter[PAR_KP]->getPointer(),
            parameter[PAR_KVB]->getPointer(),
            parameter[PAR_KVB1]->getPointer(),
            parameter[PAR_VCT]->getPointer(),
            parameter[PAR_X]->getPointer(),
            parameter[PAR_MU]->getPointer(),
            parameter[PAR_KG2]->getPointer(),
            parameter[PAR_A]->getPointer(),
            parameter[PAR_ALPHA]->getPointer(),
            parameter[PAR_BETA]->getPointer(),
            parameter[PAR_GAMMA]->getPointer(),
            parameter[PAR_OS]->getPointer(),
            parameter[PAR_OMEGA]->getPointer(),
            parameter[PAR_LAMBDA]->getPointer(),
            parameter[PAR_NU]->getPointer(),
            parameter[PAR_S]->getPointer(),
            parameter[PAR_AP]->getPointer());

        screenProblem.AddResidualBlock(
            new AutoDiffCostFunction<UnifiedPentodeIg2SEResidual, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1>(
                new UnifiedPentodeIg2SEResidual(va, vg1, ia, vg2, ig2)),
            NULL,
            parameter[PAR_KP]->getPointer(),
            parameter[PAR_KVB]->getPointer(),
            parameter[PAR_KVB1]->getPointer(),
            parameter[PAR_VCT]->getPointer(),
            parameter[PAR_X]->getPointer(),
            parameter[PAR_MU]->getPointer(),
            parameter[PAR_KG2A]->getPointer(),
            parameter[PAR_A]->getPointer(),
            parameter[PAR_TAU]->getPointer(),
            parameter[PAR_RHO]->getPointer(),
            parameter[PAR_THETA]->getPointer(),
            parameter[PAR_PSI]->getPointer(),
            parameter[PAR_OMEGA]->getPointer(),
            parameter[PAR_LAMBDA]->getPointer(),
            parameter[PAR_NU]->getPointer(),
            parameter[PAR_S]->getPointer(),
            parameter[PAR_AP]->getPointer());
    }
}

void GardinerPentode::fromJson(QJsonObject source)
{
    CohenHelieTriode::fromJson(source);
    
    if (source.contains("kg2") && source["kg2"].isDouble()) {
        parameter[PAR_KG2]->setValue(source["kg2"].toDouble() / 1000.0);
    }
    
    if (source.contains("a") && source["a"].isDouble()) {
        parameter[PAR_A]->setValue(source["a"].toDouble());
    }
    
    if (source.contains("alpha") && source["alpha"].isDouble()) {
        parameter[PAR_ALPHA]->setValue(source["alpha"].toDouble());
    }
    
    if (source.contains("beta") && source["beta"].isDouble()) {
        parameter[PAR_BETA]->setValue(source["beta"].toDouble());
    }
    
    if (source.contains("gamma") && source["gamma"].isDouble()) {
        parameter[PAR_GAMMA]->setValue(source["gamma"].toDouble());
    }
    
    if (source.contains("kg2a") && source["kg2a"].isDouble()) {
        parameter[PAR_KG2A]->setValue(source["kg2a"].toDouble() / 1000.0);
    }
    
    if (source.contains("tau") && source["tau"].isDouble()) {
        parameter[PAR_TAU]->setValue(source["tau"].toDouble());
    }
    
    if (source.contains("rho") && source["rho"].isDouble()) {
        parameter[PAR_RHO]->setValue(source["rho"].toDouble());
    }
    
    if (source.contains("theta") && source["theta"].isDouble()) {
        parameter[PAR_THETA]->setValue(source["theta"].toDouble());
    }
    
    if (source.contains("psi") && source["psi"].isDouble()) {
        parameter[PAR_PSI]->setValue(source["psi"].toDouble());
    }
    
    if (source.contains("omega") && source["omega"].isDouble()) {
        parameter[PAR_OMEGA]->setValue(source["omega"].toDouble());
    }
    
    if (source.contains("lambda") && source["lambda"].isDouble()) {
        parameter[PAR_LAMBDA]->setValue(source["lambda"].toDouble());
    }
    
    if (source.contains("nu") && source["nu"].isDouble()) {
        parameter[PAR_NU]->setValue(source["nu"].toDouble());
    }
    
    if (source.contains("s") && source["s"].isDouble()) {
        parameter[PAR_S]->setValue(source["s"].toDouble());
    }

    if (source.contains("ap") && source["ap"].isDouble()) {
        parameter[PAR_AP]->setValue(source["ap"].toDouble());
    }

    if (source.contains("os") && source["os"].isDouble()) {
        parameter[PAR_OS]->setValue(source["os"].toDouble());
    }
}

void GardinerPentode::toJson(QJsonObject &model)
{
    CohenHelieTriode::toJson(model);

    model["kg2"] = parameter[PAR_KG2]->getValue() * 1000.0;
    model["a"] = parameter[PAR_A]->getValue();
    model["alpha"] = parameter[PAR_ALPHA]->getValue();
    model["beta"] = parameter[PAR_BETA]->getValue();
    model["gamma"] = parameter[PAR_GAMMA]->getValue();

    model["kg2a"] = parameter[PAR_KG2A]->getValue() * 1000.0;
    model["tau"] = parameter[PAR_TAU]->getValue();
    model["rho"] = parameter[PAR_RHO]->getValue();
    model["theta"] = parameter[PAR_THETA]->getValue();
    model["psi"] = parameter[PAR_PSI]->getValue();

    model["omega"] = parameter[PAR_OMEGA]->getValue();
    model["lambda"] = parameter[PAR_LAMBDA]->getValue();
    model["nu"] = parameter[PAR_NU]->getValue();
    model["s"] = parameter[PAR_S]->getValue();
    model["ap"] = parameter[PAR_AP]->getValue();

    model["os"] = parameter[PAR_OS]->getValue();

    model["device"] = "pentode";
    model["type"] = "gardiner";
}

void GardinerPentode::updateUI(QLabel *labels[], QLineEdit *values[])
{
    int i = 0;

    updateParameter(labels[i], values[i], parameter[PAR_MU]); i++;
    updateParameter(labels[i], values[i], parameter[PAR_KG1]); i++;
    updateParameter(labels[i], values[i], parameter[PAR_X]); i++;
    updateParameter(labels[i], values[i], parameter[PAR_KP]); i++;
    updateParameter(labels[i], values[i], parameter[PAR_KVB]); i++;
    updateParameter(labels[i], values[i], parameter[PAR_KVB1]); i++;
    updateParameter(labels[i], values[i], parameter[PAR_VCT]); i++;

    updateParameter(labels[i], values[i], parameter[PAR_KG2]); i++;
    updateParameter(labels[i], values[i], parameter[PAR_KG2A]); i++;
    updateParameter(labels[i], values[i], parameter[PAR_A]); i++;
    updateParameter(labels[i], values[i], parameter[PAR_ALPHA]); i++;
    updateParameter(labels[i], values[i], parameter[PAR_BETA]); i++;
    updateParameter(labels[i], values[i], parameter[PAR_GAMMA]); i++;
    updateParameter(labels[i], values[i], parameter[PAR_OS]); i++;

    updateParameter(labels[i], values[i], parameter[PAR_TAU]); i++;
    updateParameter(labels[i], values[i], parameter[PAR_RHO]); i++;
    updateParameter(labels[i], values[i], parameter[PAR_THETA]); i++;
    updateParameter(labels[i], values[i], parameter[PAR_PSI]); i++;

    if (preferences->useSecondaryEmission()) {
        updateParameter(labels[i], values[i], parameter[PAR_OMEGA]); i++;
        updateParameter(labels[i], values[i], parameter[PAR_LAMBDA]); i++;
        updateParameter(labels[i], values[i], parameter[PAR_NU]); i++;
        updateParameter(labels[i], values[i], parameter[PAR_S]); i++;
        updateParameter(labels[i], values[i], parameter[PAR_AP]); i++;
    }
}

QString GardinerPentode::getName()
{
    return "Cohen Helie Pentode";
}

int GardinerPentode::getType()
{
    return GARDINER_PENTODE;
}

void GardinerPentode::updateProperties(QTableWidget *properties)
{
    clearProperties(properties);

    addProperty(properties, "Mu", QString("%1").arg(parameter[PAR_MU]->getValue()));
    addProperty(properties, "Kg1", QString("%1").arg(parameter[PAR_KG1]->getValue() * 1000.0));
    addProperty(properties, "X", QString("%1").arg(parameter[PAR_X]->getValue()));
    addProperty(properties, "Kp", QString("%1").arg(parameter[PAR_KP]->getValue()));
    addProperty(properties, "Kvb", QString("%1").arg(parameter[PAR_KVB]->getValue()));
    addProperty(properties, "Kvb1", QString("%1").arg(parameter[PAR_KVB1]->getValue()));
    addProperty(properties, "vct", QString("%1").arg(parameter[PAR_VCT]->getValue()));

    addProperty(properties, "Kg2", QString("%1").arg(parameter[PAR_KG2]->getValue() * 1000.0));
    addProperty(properties, "A", QString("%1").arg(parameter[PAR_A]->getValue()));
    addProperty(properties, "alpha", QString("%1").arg(parameter[PAR_ALPHA]->getValue()));
    addProperty(properties, "beta", QString("%1").arg(parameter[PAR_BETA]->getValue()));
    addProperty(properties, "gamma", QString("%1").arg(parameter[PAR_GAMMA]->getValue()));

    addProperty(properties, "Kg2a", QString("%1").arg(parameter[PAR_KG2A]->getValue() * 1000.0));
    addProperty(properties, "tau", QString("%1").arg(parameter[PAR_TAU]->getValue()));
    addProperty(properties, "rho", QString("%1").arg(parameter[PAR_RHO]->getValue()));
    addProperty(properties, "theta", QString("%1").arg(parameter[PAR_THETA]->getValue()));
    addProperty(properties, "psi", QString("%1").arg(parameter[PAR_PSI]->getValue()));

    if (preferences->useSecondaryEmission()) {
        addProperty(properties, "omega", QString("%1").arg(parameter[PAR_OMEGA]->getValue()));
        addProperty(properties, "lambda", QString("%1").arg(parameter[PAR_LAMBDA]->getValue()));
        addProperty(properties, "nu", QString("%1").arg(parameter[PAR_NU]->getValue()));
        addProperty(properties, "S", QString("%1").arg(parameter[PAR_S]->getValue()));
        addProperty(properties, "Ap", QString("%1").arg(parameter[PAR_AP]->getValue()));
    }

    addProperty(properties, "os", QString("%1").arg(parameter[PAR_OS]->getValue()));
}

void GardinerPentode::setOptions()
{
    if (preferences->fixTriodeParameters()) {
        anodeProblem.SetParameterBlockConstant(parameter[PAR_MU]->getPointer());
        anodeProblem.SetParameterBlockConstant(parameter[PAR_X]->getPointer());
        anodeProblem.SetParameterBlockConstant(parameter[PAR_KP]->getPointer());
        //anodeProblem.SetParameterBlockConstant(parameter[PAR_KG1]->getPointer());
        anodeProblem.SetParameterBlockConstant(parameter[PAR_KVB]->getPointer());
        anodeProblem.SetParameterBlockConstant(parameter[PAR_KVB1]->getPointer());
        anodeProblem.SetParameterBlockConstant(parameter[PAR_VCT]->getPointer());
        screenProblem.SetParameterBlockConstant(parameter[PAR_MU]->getPointer());
        screenProblem.SetParameterBlockConstant(parameter[PAR_X]->getPointer());
        screenProblem.SetParameterBlockConstant(parameter[PAR_KP]->getPointer());
        screenProblem.SetParameterBlockConstant(parameter[PAR_KVB]->getPointer());
        screenProblem.SetParameterBlockConstant(parameter[PAR_KVB1]->getPointer());
        screenProblem.SetParameterBlockConstant(parameter[PAR_VCT]->getPointer());
    }

    if (mode == NORMAL_MODE) {
        //parameter[PAR_LAMBDA]->setValue(parameter[PAR_MU]->getValue());
        //problem.SetParameterBlockConstant(parameter[PAR_A]->getPointer());
        //problem.SetParameterBlockConstant(parameter[PAR_PSI]->getPointer());

        anodeProblem.SetParameterLowerBound(parameter[PAR_A]->getPointer(), 0, 0.0);
        anodeProblem.SetParameterLowerBound(parameter[PAR_ALPHA]->getPointer(), 0, 0.0);
        anodeProblem.SetParameterLowerBound(parameter[PAR_BETA]->getPointer(), 0, 0.00001);
        anodeProblem.SetParameterLowerBound(parameter[PAR_GAMMA]->getPointer(), 0, 0.0);
        anodeProblem.SetParameterUpperBound(parameter[PAR_GAMMA]->getPointer(), 0, 2.0);
        anodeProblem.SetParameterLowerBound(parameter[PAR_OS]->getPointer(), 0, 0.0);

        if (preferences->useSecondaryEmission()) {
            anodeProblem.SetParameterUpperBound(parameter[PAR_LAMBDA]->getPointer(), 0, 2.0 * parameter[PAR_MU]->getValue());
            anodeProblem.SetParameterLowerBound(parameter[PAR_OMEGA]->getPointer(), 0, 0.0);
            anodeProblem.SetParameterLowerBound(parameter[PAR_LAMBDA]->getPointer(), 0, 0.0);
            anodeProblem.SetParameterLowerBound(parameter[PAR_NU]->getPointer(), 0, 0.0);
            anodeProblem.SetParameterLowerBound(parameter[PAR_S]->getPointer(), 0, 0.0);
            anodeProblem.SetParameterLowerBound(parameter[PAR_AP]->getPointer(), 0, 0.0);

            //anodeProblem.SetParameterBlockConstant(parameter[PAR_OMEGA]->getPointer());
            //anodeProblem.SetParameterBlockConstant(parameter[PAR_LAMBDA]->getPointer());
            //anodeProblem.SetParameterBlockConstant(parameter[PAR_NU]->getPointer());
            //anodeProblem.SetParameterBlockConstant(parameter[PAR_S]->getPointer());
            //anodeProblem.SetParameterBlockConstant(parameter[PAR_PHI]->getPointer());
            //anodeProblem.SetParameterBlockConstant(parameter[PAR_AP]->getPointer());
        }

        //problem.SetParameterUpperBound(parameter[PAR_KG2]->getPointer(), 0, parameter[PAR_KG1]->getValue() * 6.0);

        options.max_num_iterations = 400;
        options.max_num_consecutive_invalid_steps = 20;
        //options.use_inner_iterations = true;
        //options.use_nonmonotonic_steps = true;
        //options.trust_region_strategy_type = ceres::LEVENBERG_MARQUARDT;
        //options.trust_region_strategy_type = ceres::DOGLEG;
        options.linear_solver_type = ceres::DENSE_QR;
        //options.linear_solver_type = ceres::DENSE_NORMAL_CHOLESKY;
        //options.preconditioner_type = ceres::JACOBI;
        //options.preconditioner_type = ceres::SUBSET;
    } else if (mode == SCREEN_MODE) {
        parameter[PAR_TAU]->setValue(parameter[PAR_ALPHA]->getValue());
        parameter[PAR_RHO]->setValue(parameter[PAR_BETA]->getValue());
        parameter[PAR_THETA]->setValue(parameter[PAR_GAMMA]->getValue());
        parameter[PAR_KG2A]->setValue(parameter[PAR_KG2]->getValue());

        //screenProblem.SetParameterBlockConstant(parameter[PAR_KG2]->getPointer());
        screenProblem.SetParameterBlockConstant(parameter[PAR_A]->getPointer());

        if (preferences->useSecondaryEmission()) {
            if (preferences->fixSecondaryEmission()) {
                screenProblem.SetParameterBlockConstant(parameter[PAR_OMEGA]->getPointer());
                screenProblem.SetParameterBlockConstant(parameter[PAR_LAMBDA]->getPointer());
                screenProblem.SetParameterBlockConstant(parameter[PAR_NU]->getPointer());
                screenProblem.SetParameterBlockConstant(parameter[PAR_S]->getPointer());
                screenProblem.SetParameterBlockConstant(parameter[PAR_AP]->getPointer());
            }
        }

        screenProblem.SetParameterLowerBound(parameter[PAR_TAU]->getPointer(), 0, 0.0);
        screenProblem.SetParameterLowerBound(parameter[PAR_RHO]->getPointer(), 0, 0.00001);
    } else if (mode == ANODE_REMODEL_MODE) {
        anodeRemodelProblem.SetParameterBlockConstant(parameter[PAR_MU]->getPointer());
        //anodeRemodelProblem.SetParameterBlockConstant(parameter[PAR_KG1]->getPointer());
        anodeRemodelProblem.SetParameterBlockConstant(parameter[PAR_X]->getPointer());
        anodeRemodelProblem.SetParameterBlockConstant(parameter[PAR_KP]->getPointer());
        anodeRemodelProblem.SetParameterBlockConstant(parameter[PAR_KVB]->getPointer());
        anodeRemodelProblem.SetParameterBlockConstant(parameter[PAR_KVB1]->getPointer());
        anodeRemodelProblem.SetParameterBlockConstant(parameter[PAR_VCT]->getPointer());

        anodeRemodelProblem.SetParameterBlockConstant(parameter[PAR_A]->getPointer());

        anodeRemodelProblem.SetParameterLowerBound(parameter[PAR_OS]->getPointer(), 0, 0.0);

        if (preferences->useSecondaryEmission()) {
            anodeRemodelProblem.SetParameterBlockConstant(parameter[PAR_OMEGA]->getPointer());
            anodeRemodelProblem.SetParameterBlockConstant(parameter[PAR_LAMBDA]->getPointer());
            anodeRemodelProblem.SetParameterBlockConstant(parameter[PAR_NU]->getPointer());
            anodeRemodelProblem.SetParameterBlockConstant(parameter[PAR_S]->getPointer());
            anodeRemodelProblem.SetParameterBlockConstant(parameter[PAR_AP]->getPointer());
        }

        //anodeRemodelProblem.SetParameterBlockConstant(parameter[PAR_KG2]->getPointer());
    }
}
