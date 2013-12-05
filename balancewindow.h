#ifndef BALANCEWINDOW_H
#define BALANCEWINDOW_H

#include <QMainWindow>

#include <QSystemTrayIcon>

#include <QWebView>
#include <QSettings>
#include <QNetworkProxy>
#include <QNetworkAccessManager>
#include <QAuthenticator>
#include <QWebFrame>
#include <QWebElement>
#include <QWebElementCollection>
#include <QDebug>

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
    QNetworkProxy *proxy;
    QNetworkAccessManager *manager;
    QWebView *webView;
    Ui::BalanceWindow *ui;
    QSystemTrayIcon *trayIcon;
    QPixmap ico;
    QIcon icon;

    void setupWebView();
    void setupTrayIcon();
    void updateTrayIcon(double b);

    double _balance;

private slots:
    void onproxyAuthenticationRequired(QNetworkProxy proxy, QAuthenticator *manager);
    void finishLoad(bool finished);
    void setBalance(double b);
};

#endif // BALANCEWINDOW_H
