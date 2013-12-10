#ifndef BALANCEWEBVIEW_H
#define BALANCEWEBVIEW_H

#include <QWebView>
#include <QWebFrame>
#include <QWebElement>
#include <QWebElementCollection>

class BalanceWebView : public QWebView
{
    Q_OBJECT
public:
    explicit BalanceWebView(QWidget *parent = 0);

    bool isHaveText(QString text);

    double getBalance();

    void tryAuth(QString login, QString pass);

signals:

public slots:

};

#endif // BALANCEWEBVIEW_H
