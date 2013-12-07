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

#include <QMenu>

const bool d = false;

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
    QMenu *trayMenu;
    enum {
        INFO = QSystemTrayIcon::Information,
        WARNING = QSystemTrayIcon::Warning,
        CRYTICAL = QSystemTrayIcon::Critical};

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
    QDate needPay;

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
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
};

#endif // BALANCEWINDOW_H
