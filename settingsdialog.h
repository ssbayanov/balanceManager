#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QTextCodec>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

private:
    QSettings *settings;

    public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

private slots:
    void loadSettings();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_autoUpdateCheckBox_clicked(bool checked);

    void valueChanged();

private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
