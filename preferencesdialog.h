#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QSerialPortInfo>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = nullptr);
    ~PreferencesDialog();

    void setPort(QString port);
    QString getPort();

    int getPentodeModelType();
    int getSamplingType();
    bool useRemodelling();
    bool useSecondaryEmission();
    bool fixSecondaryEmission();
    bool fixTriodeParameters();
    bool showScreenCurrent();

private slots:

private:
    Ui::PreferencesDialog *ui;
};

#endif // PREFERENCESDIALOG_H
