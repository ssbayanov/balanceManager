#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <settings.h>
#include <QTextCodec>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

private:
    Settings *settings;

public:
    explicit SettingsDialog(Settings *set, QWidget *parent = 0);
    ~SettingsDialog();

private slots:
    void loadSettings();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_autoUpdateCheckBox_clicked(bool checked);

    void valueChanged();

    void proxyAuthChanged();

    void proxyTypeCahnged();

    void on_pushButton_clicked();

private:
    Ui::SettingsDialog *ui;
signals:
    void testEmail();
};

#endif // SETTINGSDIALOG_H
