#include "uibridge.h"

UIBridge::UIBridge()
{

}

void UIBridge::updateProperties(QTableWidget *properties)
{
    properties->clear();
}

void UIBridge::clearProperties(QTableWidget *properties)
{
    int count = properties->rowCount();
    for (int i = 0; i < count; i++) {
        properties->removeRow(0);
    }
}

void UIBridge::addProperty(QTableWidget *properties, QString label, QString value)
{
    int row = properties->rowCount();
    properties->insertRow(row);
    properties->setItem(row, 0, new QTableWidgetItem(label));
    properties->setItem(row, 1, new QTableWidgetItem(value));
}

void UIBridge::updateParameter(QLabel *uiLabel, QLineEdit *uiValue, QString label, double value)
{
    uiLabel->setText(label);
    uiLabel->setVisible(true);

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

    uiValue->setText(number);
    uiValue->setVisible(true);
}

void UIBridge::updateParameter(QLabel *uiLabel, QLineEdit *uiValue, Parameter *parameter)
{
    updateParameter(uiLabel, uiValue, parameter->getName(), parameter->getValue());
}
