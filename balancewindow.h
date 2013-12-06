#ifndef BALANCEWINDOW_H
#define BALANCEWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>

#include <QSystemTrayIcon>

#include <QSettings>

#include <QSettings>
#include <QNetworkProxy>
#include <QNetworkAccessManager>
#include <QAuthenticator>

#include <QWebView>
#include <QWebFrame>
#include <QWebElement>
#include <QWebElementCollection>

#include <QDebug>

#include<QDateTime>

#include <QTimer>

#include <QTextCodec>

#include <QProgressBar>

namespace Ui {
class BalanceWindow;
}

class BalanceWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BalanceWindow(QWidget *parent = 0);
    ~BalanceWindow();

private:
        Ui::BalanceWindow *ui;

    QNetworkProxy *proxy;
    QNetworkAccessManager *manager;

    QWebView *webView;
    bool isAuthPage;

    QProgressBar *progress;

    QSystemTrayIcon *trayIcon;
    QPixmap ico;
    QIcon icon;

    QTimer *updateTimer;
    QTimer *trayMessageTimer;

    QSettings *settings;



    void getSettings();
    void setupStatusBar();
    void setupWebView();
    void setupTrayIcon();
    void setupConnection();
    void updateTrayIcon(double b);
    void updateMainSettings();

    double _balance;
    double _payment;
    QDateTime lastUpdate;

    void closeEvent(QCloseEvent * event);



private slots:
    void updateBalance();

    void onproxyAuthenticationRequired(QNetworkProxy proxy, QAuthenticator *manager);

    void finishLoad(bool finished);
    void progressLoad(int);

    void updateBalance(double b);

    void setBalance(double b);

    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showMessage();
    void on_quitAction_triggered();
};

#endif // BALANCEWINDOW_H
