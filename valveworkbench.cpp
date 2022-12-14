#include "valveworkbench.h"
#include "ui_valveworkbench.h"

#include "valvemodel/circuit/triodecommoncathode.h"

ValveWorkbench::ValveWorkbench(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ValveWorkbench)
{
    logFile = new QFile("analyser.log");
    if (!logFile->open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open log file.");
        logFile = nullptr;
    }

    readConfig(tr("analyser.json"));

    ui->setupUi(this);

    ui->deviceType->addItem("Triode", TRIODE);
    ui->deviceType->addItem("Pentode", PENTODE);
    //ui->deviceType->addItem("Double Triode", TRIODE);
    //ui->deviceType->addItem("Diode", DIODE);

    loadTemplate(0);

    //buildModelSelection();

    ui->runButton->setEnabled(false);

    ui->progressBar->setRange(0, 100);
    ui->progressBar->reset();
    ui->progressBar->setVisible(false);

    heaterIndicator = new LedIndicator();
    heaterIndicator->setOffColor(QColorConstants::LightGray);
    ui->heaterLayout->addWidget(heaterIndicator);

    ui->graphicsView->setScene(plot.getScene());

    connect(&serialPort, &QSerialPort::readyRead, this, &ValveWorkbench::handleReadyRead);
    connect(&serialPort, &QSerialPort::errorOccurred, this, &ValveWorkbench::handleError);
    connect(&timeoutTimer, &QTimer::timeout, this, &ValveWorkbench::handleTimeout);
    connect(&heaterTimer, &QTimer::timeout, this, &ValveWorkbench::handleHeaterTimeout);

    checkComPorts();

    analyser = new Analyser(this, &serialPort, &timeoutTimer, &heaterTimer);

    ui->graphicsView->setScene(plot.getScene());

    int count = ui->properties->rowCount();
    for (int i = 0; i < count; i++) {
        ui->properties->removeRow(0);
    }

    buildCircuitParameters();
    buildStdDeviceSelection();

    circuits.append(new TriodeCommonCathode());
}

ValveWorkbench::~ValveWorkbench()
{
    delete ui;
}

void ValveWorkbench::buildStdDeviceSelection()
{
    ui->stdDeviceSelection->addItem("Select...", -1);
    QString modelPath = tr("../models");
    QDir modelDir(modelPath);

    QStringList filters;
    filters << "*.json";
    modelDir.setNameFilters(filters);

    QStringList models = modelDir.entryList();

    for (int i = 0; i < models.size(); i++) {
        QString modelFileName = modelPath + "/" + models.at(i);
        QFile modelFile(modelFileName);
        if (!modelFile.open(QIODevice::ReadOnly)) {
            qWarning("Couldn't open model file: ", modelFile.fileName().toStdString().c_str());
        }
        else {
            QByteArray modelData = modelFile.readAll();
            QJsonDocument modelDoc(QJsonDocument::fromJson(modelData));

            Device* model = new Device(modelDoc);
            this->devices.append(model);
            ui->stdDeviceSelection->addItem(model->getName(), i);
        }
    }
}

void ValveWorkbench::buildCircuitParameters()
{
    circuitLabels[0] = ui->cir1Label;
    circuitLabels[1] = ui->cir2Label;
    circuitLabels[2] = ui->cir3Label;
    circuitLabels[3] = ui->cir4Label;
    circuitLabels[4] = ui->cir5Label;
    circuitLabels[5] = ui->cir6Label;
    circuitLabels[6] = ui->cir7Label;

    circuitValues[0] = ui->cir1Value;
    circuitValues[1] = ui->cir2Value;
    circuitValues[2] = ui->cir3Value;
    circuitValues[3] = ui->cir4Value;
    circuitValues[4] = ui->cir5Value;
    circuitValues[5] = ui->cir6Value;
    circuitValues[6] = ui->cir7Value;

    for (int i=0; i < 7; i++) { // Parameters all initially hidden
        circuitValues[i]->setVisible(false);
        circuitLabels[i]->setVisible(false);
    }
}

void ValveWorkbench::buildCircuitSelection()
{
    ui->circuitSelection->clear();

    if (currentDevice != nullptr) {
        if (currentDevice->getDeviceType() == MODEL_TRIODE) {
            ui->circuitSelection->addItem("Common Cathode", TRIODE_COMMON_CATHODE);
        } else if (currentDevice->getDeviceType() == MODEL_PENTODE) {
            ui->circuitSelection->addItem("Common Cathode", PENTODE_COMMON_CATHODE);
        }
    }
}

void ValveWorkbench::selectStdDevice(int device)
{
    if (device < 0) {
        currentDevice = customDevice;
        //setCustomModelEnabled(true);
    }
    else {
        if (device < devices.size()) {
            currentDevice = devices.at(device);
            //setCustomModelEnabled(false);
            currentDevice->anodeAxes(&plot);
            modelPlot = nullptr;
            currentDevice->updateModelSelect(ui->stdModelSelection);
            buildCircuitSelection();
            selectCircuit(ui->circuitSelection->currentData().toInt());
        }
    }
}

void ValveWorkbench::selectStdModel(int model)
{
    currentDevice->selectModel(model);
    //currentDevice->updateUI(parameterLabels, parameterValues);
    plotModel();
}

void ValveWorkbench::selectModel(int modelType)
{
    customDevice->setModelType(modelType);
    //customDevice->updateUI(parameterLabels, parameterValues);
}

void ValveWorkbench::selectCircuit(int circuitType)
{
    Circuit *circuit = circuits.at(circuitType);
    if (circuitType < circuits.size()) {
        circuit->updateUI(circuitLabels, circuitValues);
        circuit->plot(&plot, currentDevice);
        circuit->updateUI(circuitLabels, circuitValues);
    }
}

void ValveWorkbench::selectPlot(int plotType)
{
    plotModel();
}

void ValveWorkbench::plotModel()
{
    if (modelPlot) {
       plot.getScene()->removeItem(modelPlot);
    }

    if (currentDevice != nullptr) {
        modelPlot = currentDevice->anodePlot(&plot);
    }
}

double ValveWorkbench::checkDoubleValue(QLineEdit *input, double oldValue)
{
    float parsedValue;

    const char *value = _strdup(input->text().toStdString().c_str());

    int n = sscanf_s(value, "%f.3", &parsedValue);

    if (n < 1) {
        return oldValue;
    }

    if (parsedValue < 0) {
        return 0.0;
    }

    return parsedValue;
}

void ValveWorkbench::updateDoubleValue(QLineEdit *input, double value)
{
    char number[32];

    sprintf(number, "%.3f", value);

    int length = strlen(number);
    for (int i=length-1;i >= 0; i--) {
        char test = number[i];
        if (test == '0' || test == '.') {
            number[i] = 0;
        }

        if (test != '0') {
            break;
        }
    }

    input->setText(number);
}

void ValveWorkbench::updateCircuitParameter(int index)
{
    Circuit *circuit = circuits.at(ui->circuitSelection->currentData().toInt());
    double value = checkDoubleValue(circuitValues[index], circuit->getParameter(index));

    updateDoubleValue(circuitValues[index], value);
    circuit->setParameter(index, value);
    circuit->updateUI(circuitLabels, circuitValues);
    circuit->plot(&plot, currentDevice);
    circuit->updateUI(circuitLabels, circuitValues);
}

void ValveWorkbench::updateHeater(double vh, double ih)
{
    // Update the heater display
    if (vh >= 0.0) {
        QString vhValue = QString {"%1"}.arg(vh, -6, 'f', 3, '0');
        ui->heaterVlcd->display(vhValue);
    }

    if (ih >= 0.0) {
        QString ihValue = QString {"%1"}.arg(ih, -6, 'f', 3, '0');
        ui->heaterIlcd->display(ihValue);
    }
}

void ValveWorkbench::testProgress(int progress)
{
    ui->progressBar->setValue(progress);
}

void ValveWorkbench::testFinished()
{
    ui->runButton->setChecked(false);
    ui->progressBar->setVisible(false);
    ui->btnSaveMeasurement->setEnabled(true);
    ui->btnAddToProject->setEnabled(true);

    analyser->getResult()->updatePlot(&plot);
}

void ValveWorkbench::testAborted()
{
    ui->runButton->setChecked(false);
    ui->progressBar->setVisible(false);
}

void ValveWorkbench::checkComPorts() {
    serialPorts = QSerialPortInfo::availablePorts();

    for (const QSerialPortInfo &serialPortInfo : serialPorts) {
        if (serialPortInfo.vendorIdentifier() == 0x1a86 && serialPortInfo.productIdentifier() == 0x7523) {
            port = serialPortInfo.portName();

            setSerialPort(port);
            return;
        }
    }

    ui->tab_3->setEnabled(false);
}

void ValveWorkbench::setSerialPort(QString portName)
{
    if (serialPort.isOpen()) {
        serialPort.close();
    }

    if (portName == "") {
        ui->tab_3->setEnabled(false);
        return;
    }

    serialPort.setPortName(portName);
    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setParity(QSerialPort::NoParity);
    serialPort.setStopBits(QSerialPort::OneStop);
    serialPort.setBaudRate(QSerialPort::Baud115200);
    serialPort.open(QSerialPort::ReadWrite);

    ui->tab_3->setEnabled(true);
}

void ValveWorkbench::saveSamples(QString filename)
{
    QFile samplelFile(filename);

    if (!samplelFile.open(QIODevice::ReadWrite)) {
        qWarning("Couldn't open model file.");
    } else {
        QJsonObject samplesObject;

        samplesObject["name"] = ui->deviceName->text();

        samplesObject["deviceType"] = deviceType;
        samplesObject["testType"] = testType;

        samplesObject["anodeStart"] = anodeStart;
        samplesObject["anodeStop"] = anodeStop;
        samplesObject["anodeStop"] = anodeStep;

        samplesObject["gridStart"] = gridStart;
        samplesObject["gridStop"] = gridStop;
        samplesObject["gridStop"] = gridStep;

        samplesObject["screenStart"] = screenStart;
        samplesObject["screenStop"] = screenStop;
        samplesObject["screenStop"] = screenStep;

        samplesObject["vh"] = heaterVoltage;

        samplesObject["iaMax"] = iaMax;
        samplesObject["paMax"] = pMax;

        analyser->getResult()->toJson(samplesObject);

        samplelFile.write(QJsonDocument(samplesObject).toJson());
    }
}

void ValveWorkbench::readConfig(QString filename)
{
    QFile configFile(filename);

    if (!configFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open config file.");
    } else {
        QByteArray configData = configFile.readAll();

        QJsonDocument configDoc(QJsonDocument::fromJson(configData));
        if (configDoc.isObject()) {
            config = configDoc.object();
        }

        if (config.contains("templates") && config["templates"].isArray()) {
            QJsonArray tpls = config["templates"].toArray();
            for (int i=0; i < tpls.count(); i++) {
                QJsonValue currentTemplate = tpls.at(i);
                if (currentTemplate.isObject()) {
                    Template *tpl = new Template();
                    tpl->read(currentTemplate.toObject());
                    templates.append(*tpl);
                }
            }
        }
    }
}

void ValveWorkbench::loadTemplate(int index)
{
    Template tpl = templates.at(index);

    ui->deviceName->setText(tpl.getName());
    heaterVoltage = tpl.getVHeater();
    anodeStart = tpl.getVaStart();
    anodeStop = tpl.getVaStop();
    anodeStep = tpl.getVaStep();
    gridStart = tpl.getVgStart();
    gridStop = tpl.getVgStop();
    gridStep = tpl.getVgStep();
    screenStart = tpl.getVsStart();
    screenStop = tpl.getVsStop();
    screenStep = tpl.getVsStep();
    pMax = tpl.getPaMax();
    iaMax = tpl.getIaMax();

    updateParameterDisplay();

    ui->deviceType->setCurrentIndex(tpl.getDeviceType());
    on_deviceType_currentIndexChanged(tpl.getDeviceType());

    ui->testType->setCurrentIndex(tpl.getTestType());
    on_testType_currentIndexChanged(tpl.getTestType());
}


void ValveWorkbench::updateParameterDisplay()
{
    updateDoubleValue(ui->heaterVoltage, heaterVoltage);
    updateDoubleValue(ui->anodeStart, anodeStart);
    updateDoubleValue(ui->anodeStop, anodeStop);
    updateDoubleValue(ui->anodeStep, anodeStep);
    updateDoubleValue(ui->gridStart, gridStart);
    updateDoubleValue(ui->gridStop, gridStop);
    updateDoubleValue(ui->gridStep, gridStep);
    updateDoubleValue(ui->screenStart, screenStart);
    updateDoubleValue(ui->screenStop, screenStop);
    updateDoubleValue(ui->screenStep, screenStop);
    updateDoubleValue(ui->pMax, pMax);
    updateDoubleValue(ui->iaMax, iaMax);
}

void ValveWorkbench::pentodeMode()
{
    deviceType = PENTODE;

    ui->testType->clear();
    ui->testType->addItem("Anode Characteristics", ANODE_CHARACTERISTICS);
    ui->testType->addItem("Transfer Characteristics", TRANSFER_CHARACTERISTICS);
    ui->testType->addItem("Screen Characteristics", SCREEN_CHARACTERISTICS);

    ui->gridLabel->setEnabled(true);
    ui->gridStart->setEnabled(true);
    ui->gridStop->setEnabled(true);
    ui->gridStep->setEnabled(true);

    ui->screenLabel->setEnabled(true);
    ui->screenStart->setEnabled(true);
    ui->screenStop->setEnabled(true);
    ui->screenStep->setEnabled(true);
}

void ValveWorkbench::triodeMode(bool doubleTriode)
{
    deviceType = TRIODE;

    ui->testType->clear();
    ui->testType->addItem("Anode Characteristics", ANODE_CHARACTERISTICS);
    ui->testType->addItem("Transfer Characteristics", TRANSFER_CHARACTERISTICS);

    ui->gridLabel->setEnabled(true);
    ui->gridStart->setEnabled(true);
    ui->gridStop->setEnabled(true);
    ui->gridStep->setEnabled(true);

    ui->screenLabel->setEnabled(false);
    ui->screenStart->setEnabled(false);
    ui->screenStop->setEnabled(false);
    ui->screenStep->setEnabled(false);
}

void ValveWorkbench::diodeMode()
{
    deviceType = DIODE;

    ui->testType->clear();
    ui->testType->addItem("Anode Charcteristics", ANODE_CHARACTERISTICS);

    ui->gridLabel->setEnabled(false);
    ui->gridStart->setEnabled(false);
    ui->gridStop->setEnabled(false);
    ui->gridStep->setEnabled(false);

    ui->screenLabel->setEnabled(false);
    ui->screenStart->setEnabled(false);
    ui->screenStop->setEnabled(false);
    ui->screenStep->setEnabled(false);
}

void ValveWorkbench::log(QString message)
{
    if (logFile != nullptr) {
        logFile->write(message.toLatin1());
        logFile->write("\n");
    }
}

double ValveWorkbench::updateVoltage(QLineEdit *input, double oldValue, int electrode)
{
    double value = checkDoubleValue(input, oldValue);

    switch (electrode) {
    case HEATER:
        if (value > 16.0) {
            value = 16.0;
        }
        break;
    case GRID:
        if (value > 66.0) {
            value = 66.0;
        }
        break;
    case ANODE:
    case SCREEN:
        if (value > 540.0) {
            value = 540.0;
        }
        break;
    default:
        break;
    }

    updateDoubleValue(input, value);

    return value;
}

double ValveWorkbench::updatePMax()
{
    double value = checkDoubleValue(ui->pMax, pMax);

    if (value > 50.0) {
        value = 50.0;
    }

    updateDoubleValue(ui->pMax, value);
    pMax = value;

    return value;
}

double ValveWorkbench::updateIaMax()
{
    double value = checkDoubleValue(ui->iaMax, iaMax);

    if (value > 200.0) {
        value = 200.0;
    }

    updateDoubleValue(ui->iaMax, value);
    iaMax = value;

    return value;
}

//
// Slots
//

void ValveWorkbench::handleReadyRead()
{
    analyser->handleReadyRead();
}

void ValveWorkbench::handleError(QSerialPort::SerialPortError error)
{
    analyser->handleError(error);
}

void ValveWorkbench::handleTimeout()
{
    analyser->handleCommandTimeout();
}

void ValveWorkbench::handleHeaterTimeout()
{
    analyser->handleHeaterTimeout();
}

void ValveWorkbench::on_actionExit_triggered()
{
    QCoreApplication::quit();
}

void ValveWorkbench::on_actionPrint_triggered()
{

}

void ValveWorkbench::on_actionLoad_Model_triggered()
{
    QString modelName = QFileDialog::getOpenFileName(this, "Open model", "", "*.json");

    if (modelName.isNull()) {
        return;
    }

    QFile modelFile(modelName);

    if (!modelFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open config file.");
    } else {
        QByteArray modelData = modelFile.readAll();
        currentDevice = new Device(QJsonDocument::fromJson(modelData));
    }
}

void ValveWorkbench::on_stdDeviceSelection_currentIndexChanged(int index)
{
    selectStdDevice(ui->stdDeviceSelection->currentData().toInt());
}

void ValveWorkbench::on_stdModelSelection_currentIndexChanged(int index)
{
    selectStdModel(ui->stdModelSelection->currentIndex());
}

void ValveWorkbench::on_circuitSelection_currentIndexChanged(int index)
{
    selectCircuit(ui->circuitSelection->currentData().toInt());
}

void ValveWorkbench::on_cir1Value_editingFinished()
{
    updateCircuitParameter(0);
}

void ValveWorkbench::on_cir2Value_editingFinished()
{
    updateCircuitParameter(1);
}

void ValveWorkbench::on_cir3Value_editingFinished()
{
    updateCircuitParameter(2);
}

void ValveWorkbench::on_cir4Value_editingFinished()
{
    updateCircuitParameter(3);
}

void ValveWorkbench::on_cir5Value_editingFinished()
{
    updateCircuitParameter(4);
}


void ValveWorkbench::on_cir6Value_editingFinished()
{
    updateCircuitParameter(5);
}

void ValveWorkbench::on_cir7Value_editingFinished()
{
    updateCircuitParameter(6);
}

void ValveWorkbench::on_actionNew_Project_triggered()
{
    currentProject = new QTreeWidgetItem(ui->projectTree, TYP_PROJECT);
    currentProject->setText(0, "New project");
    currentProject->setIcon(0, QIcon(":/icons/valve32.png"));
    currentProject->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    currentProject->setData(0, Qt::UserRole, QVariant::fromValue((void *) new Project()));
}

void ValveWorkbench::on_actionLoad_Measurement_triggered()
{
    if (currentProject != nullptr) {
        QString measurementlName = QFileDialog::getOpenFileName(this, "Open measurement", "", "*.json");

        if (measurementlName.isNull()) {
            return;
        }

        QFile measurementFile(measurementlName);

        if (!measurementFile.open(QIODevice::ReadOnly)) {
            qWarning("Couldn't open measurement file.");
        } else {
            QByteArray measurementData = measurementFile.readAll();
            Measurement *measurement = new Measurement();
            measurement->fromJson(QJsonDocument::fromJson(measurementData).object());
            measurement->buildTree(currentProject);
            Project *project = (Project *) currentProject->data(0, Qt::UserRole).value<void *>();
            project->addMeasurement(measurement);
        }
    }
}

void ValveWorkbench::on_projectTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    ui->estimateButton->setEnabled(false);
    ui->fitButton->setEnabled(false);

    void *data = current->data(0, Qt::UserRole).value<void *>();

    switch(current->type()) {
    case TYP_PROJECT:
        currentProject = current;
        break;
    case TYP_MEASUREMENT:
        ui->estimateButton->setEnabled(true);
        ui->fitButton->setEnabled(true);
    case TYP_SWEEP:
    case TYP_SAMPLE: {
            currentProject = getProject(current);
            DataSet *dataSet = (DataSet *) data;
            dataSet->updateProperties(ui->properties);
            dataSet->updatePlot(&plot);
            break;
        }
    case TYP_ESTIMATE: {
            currentProject = getProject(current);
            Estimate *estimate = (Estimate *) data;
            estimate->updateProperties(ui->properties);
            break;
        }
    case TYP_MODEL: {
            currentProject = getProject(current);
            Model *model = (Model *) data;
            model->updateProperties(ui->properties);
            break;
        }
    }
}

QTreeWidgetItem *ValveWorkbench::getProject(QTreeWidgetItem *current)
{
    QTreeWidgetItem *parent = current->parent();
    if (parent != nullptr) {
        if (parent->type() == TYP_PROJECT) {
            //return (Project *) parent->data(0, Qt::UserRole).value<void *>();
            return parent;
        } else {
            return getProject(parent);
        }
    }

    return nullptr;
}

void ValveWorkbench::on_deviceType_currentIndexChanged(int index)
{
    switch (ui->deviceType->itemData(index).toInt()) {
    case PENTODE:
        pentodeMode();
        break;
    case TRIODE:
        triodeMode(index == DOUBLE_TRIODE);
        break;
    case DIODE:
        diodeMode();
        break;
    default:
        break;
    }

    ui->testType->setCurrentIndex(0);
    on_testType_currentIndexChanged(0);
}

void ValveWorkbench::on_testType_currentIndexChanged(int index)
{
    switch (ui->testType->itemData(index).toInt()) {
    case ANODE_CHARACTERISTICS: // Anode swept and Grid stepped
        ui->anodeStop->setEnabled(true);
        ui->anodeStep->setEnabled(false);
        if (deviceType != DIODE) {
            ui->gridStop->setEnabled(true);
            ui->gridStep->setEnabled(true);
        }
        if (deviceType == PENTODE) { // Screen fixed (if Pentode)
            ui->screenStop->setEnabled(false);
            ui->screenStep->setEnabled(false);
        }
        break;
    case TRANSFER_CHARACTERISTICS: // Grid swept
        ui->gridStop->setEnabled(true);
        ui->gridStep->setEnabled(false);
        if (deviceType == PENTODE) { // Anode fixed and Screen stepped
            ui->anodeStop->setEnabled(false);
            ui->anodeStep->setEnabled(false);
            ui->screenStop->setEnabled(true);
            ui->screenStep->setEnabled(true);
        } else { // (Triode) Anode stepped and no Screen
            ui->anodeStop->setEnabled(true);
            ui->anodeStep->setEnabled(true);
        }
        break;
    case SCREEN_CHARACTERISTICS: // Anode fixed, Screen swept and Grid stepped
        ui->anodeStop->setEnabled(false);
        ui->anodeStep->setEnabled(false);
        ui->gridStop->setEnabled(true);
        ui->gridStep->setEnabled(true);
        ui->screenStop->setEnabled(true);
        ui->screenStep->setEnabled(false);
        break;
    default:
        break;
    }

    testType = ui->testType->itemData(index).toInt();
}

void ValveWorkbench::on_anodeStart_editingFinished()
{
    anodeStart = updateVoltage(ui->anodeStart, anodeStart, ANODE);
}

void ValveWorkbench::on_anodeStop_editingFinished()
{
    anodeStop = updateVoltage(ui->anodeStop, anodeStop, ANODE);
}

void ValveWorkbench::on_anodeStep_editingFinished()
{
    anodeStep = updateVoltage(ui->anodeStep, anodeStep, ANODE);
}

void ValveWorkbench::on_gridStart_editingFinished()
{
    gridStart = updateVoltage(ui->gridStart, gridStart, GRID);
}

void ValveWorkbench::on_gridStop_editingFinished()
{
    gridStop = updateVoltage(ui->gridStop, gridStop, GRID);
}

void ValveWorkbench::on_gridStep_editingFinished()
{
    gridStep = updateVoltage(ui->gridStep, gridStep, GRID);
}

void ValveWorkbench::on_screenStart_editingFinished()
{
    screenStart = updateVoltage(ui->screenStart, screenStart, SCREEN);
}

void ValveWorkbench::on_screenStop_editingFinished()
{
    screenStop = updateVoltage(ui->screenStop, screenStop, SCREEN);
}

void ValveWorkbench::on_screenStep_editingFinished()
{
    screenStep = updateVoltage(ui->screenStep, screenStep, SCREEN);
}

void ValveWorkbench::on_heaterVoltage_editingFinished()
{
    heaterVoltage = updateVoltage(ui->heaterVoltage, heaterVoltage, HEATER);
}

void ValveWorkbench::on_iaMax_editingFinished()
{
    updateIaMax();
}


void ValveWorkbench::on_pMax_editingFinished()
{
    updatePMax();
}

void ValveWorkbench::on_actionOptions_triggered()
{
    PreferencesDialog dialog;

    dialog.setPort(port);

    if (dialog.exec() == 1) {
        setSerialPort(dialog.getPort());

        analyser->reset();
    }
}

void ValveWorkbench::on_heaterButton_clicked()
{
    heaters = !heaters;

    heaterIndicator->setState(heaters);
    ui->runButton->setEnabled(heaters);
    analyser->setHeaterVoltage(heaterVoltage);
    analyser->setIsHeatersOn(heaters);
}

void ValveWorkbench::on_runButton_clicked()
{
    if (heaters) {
        ui->runButton->setChecked(true);
        ui->progressBar->reset();
        ui->progressBar->setVisible(true);
        ui->btnSaveMeasurement->setEnabled(false);
        ui->btnAddToProject->setEnabled(false);

        analyser->setDeviceType(deviceType);
        analyser->setTestType(testType);
        analyser->setPMax(pMax);
        analyser->setIaMax(iaMax);
        analyser->setSweepParameters(anodeStart, anodeStop, anodeStep, gridStart, gridStop, gridStep, screenStart, screenStop, screenStep);

        analyser->startTest();
    } else {
        ui->runButton->setChecked(false);
    }
}

void ValveWorkbench::on_btnSaveMeasurement_clicked()
{

}

void ValveWorkbench::on_btnAddToProject_clicked()
{
    if (currentProject == nullptr) {
        on_actionNew_Project_triggered();
    }

    Project *project = (Project *) currentProject->data(0, Qt::UserRole).value<void *>();
    Measurement *measurement = analyser->getResult();
    if (project->addMeasurement(measurement)) {
        measurement->buildTree(currentProject);
        ui->tabWidget->setCurrentWidget(ui->tab_2);
    }

    ui->btnAddToProject->setEnabled(false);
}


void ValveWorkbench::on_estimateButton_clicked()
{
    Measurement *measurement = (Measurement *) ui->projectTree->currentItem()->data(0, Qt::UserRole).value<void *>();
    if (measurement->getDeviceType() == TRIODE && measurement->getTestType() == ANODE_CHARACTERISTICS) {
        Estimate *estimate = new Estimate();
        estimate->estimate(measurement);

        Project *project = (Project *) currentProject->data(0, Qt::UserRole).value<void *>();
        estimate->buildTree(currentProject);
        project->addEstimate(estimate);
    }
}


void ValveWorkbench::on_fitButton_clicked()
{
    QList<QTreeWidgetItem *> items = ui->projectTree->selectedItems();

    QList<Measurement *> measurements;
    Estimate *estimate = new Estimate(); // If no Estimate is selected then default values will be used

    int estimates = 0;
    bool selectionValid = true;
    for (int i = 0; i < items.count(); i++) {
        QTreeWidgetItem *item = items.at(i);

        if (item->type() == TYP_MEASUREMENT) {
            measurements.append((Measurement *)item->data(0, Qt::UserRole).value<void *>());
        } else if (item->type() == TYP_ESTIMATE) {
            estimate = (Estimate *)item->data(0, Qt::UserRole).value<void *>();
            estimates++;
        } else { // If anything else is selected then it's not valid for fitting
            selectionValid = false;
        }

        if (selectionValid && estimates < 2 && measurements.count() > 0) {
            Model *model = ModelFactory::createModel(COHEN_HELIE_TRIODE);
            model->setEstimate(estimate);
            model->addMeasurements(&measurements);
            model->solve();

            Project *project = (Project *) currentProject->data(0, Qt::UserRole).value<void *>();
            project->addModel(model);
            model->buildTree(currentProject);
        }
    }
}

