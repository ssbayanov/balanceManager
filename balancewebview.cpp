#include "balancewebview.h"

BalanceWebView::BalanceWebView(QWidget *parent) :
    QWebView(parent)
{
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
