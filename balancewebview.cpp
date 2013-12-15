#include "balancewebview.h"

BalanceWebView::BalanceWebView(QWidget *parent) :
    QWebView(parent)
{
}

void BalanceWebView::setupProxyConnection(int proxyType, QString proxyAdress, int proxyPort,
                                          bool proxyAuth, QString proxyLogin, QString proxyPass)
{
    proxy = new QNetworkProxy();
    manager = new QNetworkAccessManager(this);
    manager->setProxy(*proxy);
    connect(manager, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)), this, SLOT(onproxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));

    switch(proxyType){
    case 0: proxy->setType(QNetworkProxy::NoProxy); break;
    case 1: proxy->setType(QNetworkProxy::HttpProxy); break;
    case 2: proxy->setType(QNetworkProxy::HttpCachingProxy); break;
    case 3: proxy->setType(QNetworkProxy::Socks5Proxy); break;
    }

    if (proxyType) {
        proxy->setHostName(proxyAdress);
        proxy->setPort(proxyPort);

        if(proxyAuth) {
            proxy->setUser(proxyLogin);
            proxy->setPassword(proxyPass);}

        page()->setNetworkAccessManager(manager);}
}

void BalanceWebView::onproxyAuthenticationRequired(QNetworkProxy proxy, QAuthenticator *manager)
{
    manager->setUser(proxy.user());
    manager->setPassword(proxy.password());
}

bool BalanceWebView::isHaveText(QString text){
    return page()->mainFrame()->toPlainText().indexOf(text) > 0;
}

double BalanceWebView::getBalance(){
    QString balanceString = page()->mainFrame()->documentElement().findFirst("div[class=\"main\"]").nextSibling().nextSibling().lastChild().lastChild().toPlainText();

    QRegExp rx("(\\d*\\,\\d+)");
    rx.indexIn(balanceString);

    double balance;
    balanceString.indexOf(QString("Задолженность %1").arg(rx.capturedTexts().last())) >= 0 ?
                balance = -QString(rx.capturedTexts().last()).replace(",",".").toDouble():
            balance = QString(rx.capturedTexts().last()).replace(",",".").toDouble();
    return balance;
}

void BalanceWebView::tryAuth(QString login, QString pass){
    QWebElement doc = page()->mainFrame()->documentElement().findFirst("input[type=\"text\"]");

    doc.setAttribute("value", login);

    doc = page()->mainFrame()->documentElement().findFirst("input[type=\"password\"]");
    doc.setAttribute("value", pass);

    doc = page()->mainFrame()->documentElement().findFirst("input[type=\"image\"]");

    QString name = doc.attribute("name");
    page()->mainFrame()->evaluateJavaScript("document.getElementsByName(\""+name+"\").item(0).click();");
}
