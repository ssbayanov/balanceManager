#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(Settings *set, QWidget *parent) :
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
    connect(ui->withoutProxyRadio,SIGNAL(toggled(bool)),this,SLOT(proxyTypeCahnged()));
    connect(ui->httpProxyRadio,SIGNAL(toggled(bool)),this,SLOT(proxyTypeCahnged()));
    connect(ui->socks5ProxyRadio,SIGNAL(toggled(bool)),this,SLOT(proxyTypeCahnged()));

    connect(ui->socks5ProxyRadio,SIGNAL(toggled(bool)),this,SLOT(proxyTypeCahnged()));

    connect(ui->proxyAuth,SIGNAL(toggled(bool)),this,SLOT(proxyAuthChanged()));

    settings = set;

    loadSettings();

    proxyTypeCahnged();

    proxyAuthChanged();
}

void SettingsDialog::loadSettings(){

    ui->autoStartCheckBox->setChecked(settings->autoStart());

    ui->autoUpdateCheckBox->setChecked(settings->autoUpdate());
    ui->periodSpinBox->setValue(settings->updatePeriod());

    ui->login->setText(settings->login());
    ui->password->setText(settings->pass());

    ui->noticeTrayCheckBox->setChecked(settings->trayNotice());
    ui->noticeEmailCheckBox->setChecked(settings->emailNotice());
    ui->lowerWarning->setChecked(settings->lowerWarning());
    ui->lowerCritical->setChecked(settings->lowerCritical());
    ui->warningSpinBox->setValue(settings->warningBalance());
    ui->criticalSpinBox->setValue(settings->criticalBalance());

    ui->smtpServer->setText(settings->smtpServer());
    ui->smtpPort->setText(QString("%1").arg(settings->smtpPort()));
    ui->smtpLogin->setText(settings->smtpLogin());
    ui->smtpPass->setText(settings->smtpPass());
    ui->smtpEmail->setText(settings->smtpEmail());

    switch(settings->proxyType()){
    case 1:
        ui->httpProxyRadio->setChecked(true);
        break;
    case 2:
        ui->httpCashingProxyRadio->setChecked(true);
        break;
    case 3:
        ui->socks5ProxyRadio->setChecked(true);
        break;
    default:
        ui->withoutProxyRadio->setChecked(true);
        break;
    }
    ui->adressProxy->setText(settings->proxyAdress());
    ui->portProxy->setText(QString("%1").arg(settings->proxyPort()));
    ui->proxyLogin->setText(settings->proxyLogin());
    ui->proxyPass->setText(settings->proxyPass());

}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_buttonBox_accepted()
{
    settings->setMainSettings(ui->autoStartCheckBox->isChecked(),
                              ui->autoUpdateCheckBox->isChecked(),
                              ui->periodSpinBox->value());

    settings->setAuthSettings(ui->login->text(),
                              ui->password->text());

    settings->setupNoticeSettings(ui->noticeTrayCheckBox->isChecked(),
                                  ui->noticeEmailCheckBox->isChecked(),
                                  ui->lowerWarning->isChecked(),
                                  ui->lowerCritical->isChecked(),
                                  ui->warningSpinBox->value(),
                                  ui->criticalSpinBox->value());

    settings->setupSmtpSettings(ui->smtpServer->text(),
                                ui->smtpPort->text().toInt(),
                                ui->smtpLogin->text(),
                                ui->smtpPass->text(),
                                ui->smtpEmail->text());


    int proxyType;
    if(ui->httpProxyRadio->isChecked())
        proxyType = 1;
    else if(ui->httpCashingProxyRadio->isChecked())
        proxyType = 2;
    else if(ui->socks5ProxyRadio->isChecked())
        proxyType = 3;
    else
        proxyType = 0;

    settings->setupProxySettings(proxyType,
                                 ui->adressProxy->text(),
                                 ui->portProxy->text().toInt(),
                                 ui->proxyAuth->isChecked(),
                                 ui->proxyLogin->text(),
                                 ui->proxyPass->text());
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

void SettingsDialog::proxyTypeCahnged(){

    if(ui->withoutProxyRadio->isChecked())
    {
        ui->proxyLogin->setEnabled(false);
        ui->proxyPass->setEnabled(false);
        ui->proxyLoginLabel->setEnabled(false);
        ui->proxyPassLabel->setEnabled(false);
        ui->adressProxy->setEnabled(false);
        ui->adressProxyLabel->setEnabled(false);
        ui->portProxy->setEnabled(false);
        ui->proxyPortLabel->setEnabled(false);
        ui->proxyAuth->setEnabled(false);
    }
    else
    {
        ui->proxyLogin->setEnabled(true);
        ui->proxyPass->setEnabled(true);
        ui->proxyLoginLabel->setEnabled(true);
        ui->proxyPassLabel->setEnabled(true);
        ui->adressProxy->setEnabled(true);
        ui->adressProxyLabel->setEnabled(true);
        ui->portProxy->setEnabled(true);
        ui->proxyPortLabel->setEnabled(true);
        ui->proxyAuth->setEnabled(true);
    }
    proxyAuthChanged();
}

void SettingsDialog::proxyAuthChanged(){
    if(ui->proxyAuth->isChecked() && ui->proxyAuth->isEnabled()){
        ui->proxyLogin->setEnabled(true);
        ui->proxyPass->setEnabled(true);
        ui->proxyLoginLabel->setEnabled(true);
        ui->proxyPassLabel->setEnabled(true);
    }
    else{
        ui->proxyLogin->setEnabled(false);
        ui->proxyPass->setEnabled(false);
        ui->proxyLoginLabel->setEnabled(false);
        ui->proxyPassLabel->setEnabled(false);
    }

}


void SettingsDialog::on_pushButton_clicked()
{
    emit testEmail();
}
