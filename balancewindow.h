#ifndef BALANCEWINDOW_H
#define BALANCEWINDOW_H

#include <QMainWindow>
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

private slots:
    void onproxyAuthenticationRequired(QNetworkProxy proxy, QAuthenticator *manager);
    void finishLoad(bool finished);
};

#endif // BALANCEWINDOW_H
