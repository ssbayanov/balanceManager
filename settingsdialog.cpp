#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    this->setLayout(ui->dialogLayout);
    ui->authTab->setLayout(ui->authLayout);
    ui->mainTab->setLayout(ui->mainSetLayout);
    ui->smtpTab->setLayout(ui->mailLayout);
    ui->noticeTab->setLayout(ui->noticeLayout);
    connect(ui->criticalSpinBox,SIGNAL(valueChanged(double)),this,SLOT(valueChanged()));

    loadSettings();
}

void SettingsDialog::loadSettings(){
    QSettings sett("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
                               QSettings::NativeFormat);

    ui->autoStartCheckBox->setChecked(sett.childKeys().contains("BalanceManager",Qt::CaseSensitive));

    settings = new QSettings(QString("%1\\config.ini").arg(QApplication::applicationDirPath()), QSettings::IniFormat);
    settings->setIniCodec(QTextCodec::codecForName("UTF-8"));

    settings->beginGroup("MAIN");
    ui->autoUpdateCheckBox->setChecked(settings->value("AutoUpdate",true).toBool());
    ui->periodSpinBox->setValue(settings->value("UpdatePeriod",6).toInt());
    settings->endGroup();

    settings->beginGroup("AUTH");
    ui->login->setText(settings->value("Login","anonymous").toString());
    ui->password->setText(settings->value("Pass","12345").toString());
    settings->endGroup();

    settings->beginGroup("NOTICE");
    ui->noticeTrayCheckBox->setChecked(settings->value("TrayNotice",true).toBool());
    ui->noticeEmailCheckBox->setChecked(settings->value("EmailNotice",true).toBool());
    ui->lowerWarning->setChecked(settings->value("LowerWarning",true).toBool());
    ui->lowerCritical->setChecked(settings->value("LowerCritical",true).toBool());
    ui->warningSpinBox->setValue(settings->value("WarningBalance",70.0).toDouble());
    ui->criticalSpinBox->setValue(settings->value("CriticalBalance",15.0).toDouble());
    settings->endGroup();

    settings->beginGroup("SMTP");
    ui->smtpServer->setText(settings->value("Server","smtp.gmail.com").toString());
    ui->smtpPort->setText(settings->value("Port",554).toString());
    ui->smtpLogin->setText(settings->value("Login","anonymous").toString());
    ui->smtpPass->setText(settings->value("Pass","1245").toString());
    settings->endGroup();

    settings->beginGroup("PROXY");
    switch(settings->value("ProxyType","0").toInt()){
    case 1:
        ui->httpProxyRadio->setChecked(true);
        break;
    case 2:
        ui->socks5ProxyRadio->setChecked(true);
    default:
        ui->withoutProxyRadio->setChecked(true);
        break;
    }
    ui->adressProxy->setText(settings->value("ProxyAdress","").toString());
    ui->portProxy->setText(settings->value("ProxyPort","").toString());

    settings->endGroup();

}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_buttonBox_accepted()
{
    QSettings sett("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
                               QSettings::NativeFormat);
    if(ui->autoStartCheckBox->isChecked())
        sett.setValue("BalanceManager",QString("\"%1\"").arg(QApplication::applicationFilePath().replace("/","\\")));
    else
        sett.remove("BalanceManager");

    settings->beginGroup("MAIN");
    settings->setValue("AutoUpdate",ui->autoUpdateCheckBox->isChecked());
    settings->setValue("UpdatePeriod",ui->periodSpinBox->value());
    settings->endGroup();

    settings->beginGroup("AUTH");
    settings->setValue("Login",ui->login->text());
    settings->setValue("Pass",ui->password->text());
    settings->endGroup();

    settings->beginGroup("NOTICE");
    settings->setValue("TrayNotice",ui->noticeTrayCheckBox->isChecked());
    settings->setValue("EmailNotice",ui->noticeEmailCheckBox->isChecked());
    settings->setValue("LowerWarning",ui->lowerWarning->isChecked());
    settings->setValue("LowerCritical",ui->lowerCritical->isChecked());
    settings->setValue("WarningBalance",ui->warningSpinBox->value());
    settings->setValue("CriticalBalance",ui->criticalSpinBox->value());
    settings->endGroup();

    settings->beginGroup("SMTP");
    settings->setValue("Server",ui->smtpServer->text());
    settings->setValue("Port",ui->smtpPort->text().toInt());
    settings->setValue("Login",ui->smtpLogin->text());
    settings->setValue("Pass",ui->smtpPass->text());
    settings->endGroup();

    settings->beginGroup("PROXY");
    if(ui->httpProxyRadio->isChecked())
        settings->setValue("ProxyType", 1);
    else if(ui->socks5ProxyRadio->isChecked())
        settings->setValue("ProxyType", 2);
    else settings->setValue("ProxyType", 0);

    settings->setValue("ProxyAdress",ui->adressProxy->text());
    settings->setValue("ProxyPort",ui->portProxy->text().toInt());
    settings->endGroup();
}

void SettingsDialog::on_buttonBox_rejected()
{

}

void SettingsDialog::on_autoUpdateCheckBox_clicked(bool checked)
{
    if(!checked){
        //        ui->periodLayout->setEnabled(false);
        ui->periodLabel->setEnabled(false);
        ui->periodSpinBox->setEnabled(false);
        ui->hourseLabel->setEnabled(false);
    }
    else{
        //        ui->periodLayout->setEnabled(true);
        ui->periodLabel->setEnabled(true);
        ui->periodSpinBox->setEnabled(true);
        ui->hourseLabel->setEnabled(true);
    }
}

void SettingsDialog::valueChanged()
{
    if(ui->warningSpinBox->value() < ui->criticalSpinBox->value())
        ui->warningSpinBox->setValue(ui->criticalSpinBox->value());
}
