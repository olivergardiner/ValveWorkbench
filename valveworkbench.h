#pragma once

#include <QMainWindow>
#include <QFileDialog>
#include <QDir>
#include <QList>
#include <QJsonDocument>
#include <QTreeWidget>
#include <QSerialPortInfo>
#include <QTimer>

#include "valvemodel/data/project.h"
#include "valvemodel/model/estimate.h"
#include "valvemodel/model/device.h"
#include "valvemodel/model/modelfactory.h"
#include "valvemodel/circuit/circuit.h"
#include "valvemodel/ui/plot.h"
#include "valvemodel/model/template.h"

#include "analyser/analyser.h"
#include "analyser/client.h"

#include "preferencesdialog.h"

#include "ledindicator/ledindicator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ValveWorkbench; }
QT_END_NAMESPACE

class ValveWorkbench : public QMainWindow, public Client
{
    Q_OBJECT

public:
    ValveWorkbench(QWidget *parent = nullptr);
    ~ValveWorkbench();

    // Methods required to be implemented by the Analyser Client class
    virtual void updateHeater(double vh, double ih);
    virtual void testProgress(int progress);
    virtual void testFinished();
    virtual void testAborted();

private slots:
    void on_actionExit_triggered();

    void on_actionPrint_triggered();

    void on_actionOptions_triggered();

    void on_actionLoad_Model_triggered();

    void on_stdDeviceSelection_currentIndexChanged(int index);

    void on_stdModelSelection_currentIndexChanged(int index);

    void on_circuitSelection_currentIndexChanged(int index);

    void on_cir1Value_editingFinished();

    void on_cir2Value_editingFinished();

    void on_cir3Value_editingFinished();

    void on_cir4Value_editingFinished();

    void on_cir5Value_editingFinished();

    void on_cir6Value_editingFinished();

    void on_cir7Value_editingFinished();

    void on_actionNew_Project_triggered();

    void on_actionLoad_Measurement_triggered();

    void on_projectTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_heaterButton_clicked();

    void on_runButton_clicked();

    void handleReadyRead();

    void handleError(QSerialPort::SerialPortError error);

    void handleTimeout();

    void handleHeaterTimeout();

    void on_deviceType_currentIndexChanged(int index);

    void on_testType_currentIndexChanged(int index);

    void on_anodeStart_editingFinished();

    void on_anodeStop_editingFinished();

    void on_anodeStep_editingFinished();

    void on_gridStart_editingFinished();

    void on_gridStop_editingFinished();

    void on_gridStep_editingFinished();

    void on_screenStart_editingFinished();

    void on_screenStop_editingFinished();

    void on_screenStep_editingFinished();

    void on_heaterVoltage_editingFinished();

    void on_iaMax_editingFinished();

    void on_pMax_editingFinished();

    void on_btnSaveMeasurement_clicked();

    void on_btnAddToProject_clicked();

    void on_estimateButton_clicked();

    void on_fitButton_clicked();

private:
    Ui::ValveWorkbench *ui;

    // UI related member variables
    QLineEdit *circuitValues[8];
    QLabel *circuitLabels[8];

    Plot plot;
    QGraphicsItemGroup *modelPlot = nullptr;

    // Non-UI related member variables
    QList<Device *> devices;
    Device *currentDevice = nullptr;
    Device *customDevice = nullptr;

    QTreeWidgetItem *currentProject = nullptr;

    QList<Circuit *> circuits;
    QTreeWidgetItem *projectTree;

    int deviceType = TRIODE;
    int testType = ANODE_CHARACTERISTICS;

    double heaterVoltage;

    double anodeStart;
    double anodeStop;
    double anodeStep;

    double gridStart;
    double gridStop;
    double gridStep;

    double screenStart;
    double screenStop;
    double screenStep;

    double iaMax;
    double pMax;

    bool heaters = false;

    QFile *logFile;

    LedIndicator *heaterIndicator;
    QGraphicsItemGroup *measuredCurves = nullptr;
    QGraphicsItemGroup *modelledCurves = nullptr;

    Analyser *analyser;
    QString port;
    QSerialPort serialPort;

    QTimer timeoutTimer;
    QTimer heaterTimer;

    QList<QSerialPortInfo> serialPorts;

    QJsonObject config;
    QList<Template> templates;

    void checkComPorts();
    void setSerialPort(QString portName);

    void readConfig(QString filename);

    //void buildModelSelection();
    void buildCircuitParameters();
    void buildCircuitSelection();
    void buildStdDeviceSelection();
    void selectStdDevice(int device);
    void selectStdModel(int model);
    void selectDevice(int deviceType);
    void selectModel(int modelType);
    void selectCircuit(int circuitType);
    void selectPlot(int plotType);
    void plotModel();
    double checkDoubleValue(QLineEdit *input, double oldValue);
    void updateDoubleValue(QLineEdit *input, double value);
    void updateCircuitParameter(int index);
    void loadTemplate(int index);
    void saveSamples(QString filename);
    void pentodeMode();
    void triodeMode(bool doubleTriode);
    void diodeMode();
    void log(QString message);
    double updateVoltage(QLineEdit *input, double oldValue, int electrode);
    double updatePMax();
    double updateIaMax();
    void updateParameterDisplay();
    void doPlot();
    QTreeWidgetItem *getProject(QTreeWidgetItem *current);
};
