#ifndef BALANCEWEBVIEW_H
#define BALANCEWEBVIEW_H

#include <QWebView>
#include <QWebFrame>
#include <QWebElement>
#include <QWebElementCollection>

#include <QNetworkProxy>
#include <QNetworkAccessManager>
#include <QAuthenticator>

class BalanceWebView : public QWebView
{
    Q_OBJECT
public:
    explicit BalanceWebView(QWidget *parent = 0);

    bool isHaveText(QString text);

    double getBalance();

    void tryAuth(QString login, QString pass);

signals:

private:
    QNetworkProxy *proxy;
    QNetworkAccessManager *manager;

public slots:
    void onproxyAuthenticationRequired(QNetworkProxy proxy, QAuthenticator *manager);
    void setupProxyConnection(int proxyType, QString proxyAdress, int proxyPort,
                              bool proxyAuth, QString proxyLogin, QString proxyPass);

};

#endif // BALANCEWEBVIEW_H
