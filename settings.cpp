#include "settings.h"

Settings::Settings(QObject *parent) :
    QSettings(QString("%1\\config.ini").arg(QApplication::applicationDirPath()), QSettings::IniFormat, parent)
{
    setIniCodec(QTextCodec::codecForName("UTF-8"));
    getSettings();
}

void Settings::getSettings()
{
    QSettings reg("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
                   QSettings::NativeFormat);

    _autoStart = reg.childKeys().contains("BalanceManager",Qt::CaseSensitive);

    beginGroup("MAIN");
    _payment = value("Payment",12.91).toDouble();
    _lastUpdate = value("LastUpdate",QDateTime::fromString("01.01.01 00:00","dd.MM.yy hh:mm")).toDateTime();
    _autoUpdate = value("AutoUpdate",true).toBool();
    _updatePeriod = value("UpdatePeriod",6).toInt();
    calculate(value("Balance",0.0).toDouble());
    endGroup();

    beginGroup("AUTH");
    _login = value("Login","anonymous").toString();
    _pass = value("Pass","12345").toString();
    endGroup();

    beginGroup("NOTICE");
    _trayNotice = value("TrayNotice",true).toBool();
    _emailNotice = value("EmailNotice",true).toBool();
    _lowerWarning = value("LowerWarning",true).toBool();
    _lowerCritical = value("LowerCritical",true).toBool();
    _warningBalance = value("WarningBalance",70.0).toDouble();
    _criticalBalance = value("CriticalBalance",15.0).toDouble();
    endGroup();

    beginGroup("SMTP");
    _smtpServer = value("Server","smtp.gmail.com").toString();
    _smtpPort = value("Port",554).toInt();
    _smtpLogin = value("Login","anonymous").toString();
    _smtpPass = value("Pass","1245").toString();
    _smtpEmail = value("Email","").toString();
    endGroup();

    beginGroup("PROXY");
    _proxyType = value("ProxyType","0").toInt();
    _proxyAdress = value("ProxyAdress","").toString();
    _proxyPort = value("ProxyPort","").toInt();
    _proxyAuth = value("ProxyAuth",false).toBool();
    _proxyLogin = value("ProxyLogin","").toString();
    _proxyPass = value("ProxyPass","").toString();
    endGroup();

    emit balanceChanged();
}

double Settings::balance()
{
    return _balance;
}
void Settings::setBalance(double balance)
{
    _lastUpdate = QDateTime::currentDateTime();

    calculate(balance);

    beginGroup("MAIN");
    setValue("Balance",_balance);
    setValue("Payment",_payment);
    setValue("LastUpdate",_lastUpdate);
    endGroup();

    emit balanceChanged();
}
void Settings::calculate(double balance)
{
    if(balance < _balance)
        _payment = _balance - balance;

    _balance = balance;

    _balance >= 0 ?
                _dayToPay = _balance/_payment:
            _dayToPay = 0;

    _needPay = QDate::currentDate().addDays((_balance/_payment)-1);
}

QString Settings::needPay()
{
    return _needPay.toString("dd.MM.yyг.");
}

QString Settings::lastUpdate()
{
    return _lastUpdate.toString("dd.MM.yyг. hh:mm");
}

int Settings::dayToPay()
{
    return _dayToPay;
}

double Settings::payment()
{
    return _payment;
}

bool Settings::autoUpdate()
{
    return _autoUpdate;
}

bool Settings::autoStart()
{
    return _autoStart;
}

int Settings::updatePeriod()
{
    return _updatePeriod;
}

int Settings::updatePeriodMsec()
{
    return _updatePeriod*1000*60*60;
}

void Settings::setMainSettings(bool autoStart, bool autoUpdate, int updatePeriod)
{
    _autoStart = autoStart;
    _autoUpdate = autoUpdate;
    _updatePeriod = updatePeriod;

    QSettings reg("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
                   QSettings::NativeFormat);
    if(autoStart)
        reg.setValue("BalanceManager",QString("\"%1\"").arg(QApplication::applicationFilePath().replace("/","\\")));
    else
        reg.remove("BalanceManager");

    beginGroup("MAIN");
    setValue("AutoUpdate",autoUpdate);
    setValue("UpdatePeriod",updatePeriod);
    endGroup();
}

QString Settings::login()
{
    return _login;
}

QString Settings::pass()
{
    return _pass;
}

void Settings::setAuthSettings(QString login, QString pass)
{
    _login = login;
    _pass = pass;

    beginGroup("AUTH");
    setValue("Login",login);
    setValue("Pass",pass);
    endGroup();
}

bool Settings::trayNotice()
{
    return _trayNotice;
}

bool Settings::emailNotice()
{
    return _emailNotice;
}

bool Settings::lowerWarning()
{
    return _lowerWarning;
}

bool Settings::lowerCritical()
{
    return _lowerCritical;
}

double Settings::warningBalance()
{
    return _warningBalance;
}

bool Settings::isWarningBalance()
{
    return _balance < _warningBalance;
}

double Settings::criticalBalance()
{
    return _criticalBalance;
}

bool Settings::isCriticalBalance()
{
    return _balance < _criticalBalance;
}

Settings::LevelBalance Settings::levelBalance(){
    return isCriticalBalance()? CRITICAL : isWarningBalance()? WARNING : NORMAL;
}

void Settings::setupNoticeSettings(bool trayNotice,
                                   bool emailNotice,
                                   bool lowerWarning,
                                   bool lowerCritical,
                                   double warningBalance,
                                   double criticalBalance)
{

    _trayNotice = trayNotice;
    _emailNotice = emailNotice;
    _lowerWarning = lowerWarning;
    _lowerCritical = lowerCritical;
    _warningBalance = warningBalance;
    _criticalBalance = criticalBalance;

    beginGroup("NOTICE");
    setValue("TrayNotice",trayNotice);
    setValue("EmailNotice",emailNotice);
    setValue("LowerWarning",lowerWarning);
    setValue("LowerCritical",lowerCritical);
    setValue("WarningBalance",warningBalance);
    setValue("CriticalBalance",criticalBalance);
    endGroup();
}

QString Settings::smtpServer()
{
    return _smtpServer;
}

int Settings::smtpPort()
{
    return _smtpPort;
}

QString Settings::smtpLogin()
{
    return _smtpLogin;
}

QString Settings::smtpPass()
{
    return _smtpPass;
}

QString Settings::smtpEmail()
{
    return _smtpEmail;
}

void Settings::setupSmtpSettings(QString smtpServer,
                                 int smtpPort,
                                 QString smtpLogin,
                                 QString smtpPass,
                                 QString smtpEmail)
{
    _smtpServer = smtpServer;
    _smtpPort = smtpPort;
    _smtpLogin = smtpLogin;
    _smtpPass = smtpPass;
    _smtpEmail = smtpEmail;

    beginGroup("SMTP");
    setValue("Server",smtpServer);
    setValue("Port",smtpPort);
    setValue("Login",smtpLogin);
    setValue("Pass",smtpPass);
    setValue("Email",smtpEmail);
    endGroup();
}

int Settings::proxyType()
{
    return _proxyType;
}

QString Settings::proxyAdress()
{
    return _proxyAdress;
}

int Settings::proxyPort()
{
    return _proxyPort;
}

bool Settings::proxyAuth()
{
    return _proxyAuth;
}

QString Settings::proxyLogin()
{
    return _proxyLogin;
}

QString Settings::proxyPass()
{
    return _proxyPass;
}

void Settings::setupProxySettings(int proxyType,
                                  QString proxyAdress,
                                  int proxyPort,
                                  bool proxyAuth,
                                  QString proxyLogin,
                                  QString proxyPass)
{
    _proxyType = proxyType;
    _proxyAdress = proxyAdress;
    _proxyPort = proxyPort;
    _proxyAuth = proxyAuth;
    _proxyLogin = proxyLogin;
    _proxyPass = proxyPass;

    beginGroup("PROXY");
    setValue("ProxyType", proxyType);
    setValue("ProxyAdress",proxyAdress);
    setValue("ProxyPort",proxyPort);
    setValue("ProxyAuth",proxyAuth);
    setValue("ProxyLogin",proxyLogin);
    setValue("ProxyPass",proxyPass);
    endGroup();
}
