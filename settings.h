#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

#include <QDateTime>

#include <QTextCodec>

#include <QApplication>

#include <QStringList>

class Settings : public QSettings
{
    Q_OBJECT
public:
    enum LevelBalance {NORMAL = 0, WARNING = 1, CRITICAL = 2};

    explicit Settings(QObject *parent = 0);

    double balance();
    void setBalance(double balance);

    int dayToPay();
    QString needPay();

    double payment();
    QString lastUpdate();
    bool autoUpdate();
    bool autoStart();
    int updatePeriod();
    int updatePeriodMsec();
    void setMainSettings(bool autoStart, bool autoUpdate, int updatePeriod);

    QString login();
    QString pass();
    void setAuthSettings(QString login, QString pass);

    bool trayNotice();
    bool emailNotice();
    bool lowerWarning();
    bool lowerCritical();
    double warningBalance();
    bool isWarningBalance();

    double criticalBalance();
    bool isCriticalBalance();

    LevelBalance levelBalance();

    void setupNoticeSettings(bool trayNotice,
                             bool emailNotice,
                             bool lowerWarning,
                             bool lowerCritical,
                             double warningBalance,
                             double criticalBalance);

    QString smtpServer();
    int smtpPort();
    QString smtpLogin();
    QString smtpPass();
    QString smtpEmail();
    void setupSmtpSettings(QString smtpServer,
                           int smtpPort,
                           QString smtpLogin,
                           QString smtpPass,
                           QString smtpEmail);

    int proxyType();
    QString proxyAdress();
    int proxyPort();
    bool proxyAuth();
    QString proxyLogin();
    QString proxyPass();
    void setupProxySettings(int proxyType,
                            QString proxyAdress,
                            int proxyPort, bool proxyAuth,
                            QString proxyLogin,
                            QString proxyPass);
private:
    QDate _needPay;
    int _dayToPay;

    double _balance;
    double _lastBalance;
    double _payment;
    QDateTime _lastUpdate;
    bool _autoStart;
    bool _autoUpdate;
    int _updatePeriod;

    QString _login;
    QString _pass;

    bool _trayNotice;
    bool _emailNotice;
    bool _lowerWarning;
    bool _lowerCritical;
    double _warningBalance;
    double _criticalBalance;

    QString _smtpServer;
    int _smtpPort;
    QString _smtpLogin;
    QString _smtpPass;
    QString _smtpEmail;

    int _proxyType;
    QString _proxyAdress;
    int _proxyPort;
    bool _proxyAuth;
    QString _proxyLogin;
    QString _proxyPass;

    void calculate(double balance);

signals:
    void balanceChanged();
public slots:
    void getSettings();


};

#endif // SETTINGS_H
