#include "balancewindow.h"
#include "ui_balancewindow.h"

BalanceWindow::BalanceWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BalanceWindow)
{
    ui->setupUi(this);
    webView = new QWebView();
    //this->setCentralWidget(webView);
    //webView->show();

    proxy = new QNetworkProxy();
    manager = new QNetworkAccessManager(this);
    manager->setProxy(*proxy);
    //connect(manager, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)), this, SLOT(onproxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));

    proxy->setType(QNetworkProxy::NoProxy);

    webView->page()->settings()->setAttribute(QWebSettings::PluginsEnabled, false);
    webView->page()->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    webView->page()->settings()->setAttribute(QWebSettings::AutoLoadImages, false);
    webView->page()->settings()->setAttribute(QWebSettings::WebGLEnabled, false);

    connect(webView,SIGNAL(loadFinished(bool)),this,SLOT(finishLoad(bool)));
    webView->load(QUrl("https:////bill.sibttk.ru//login"));
}

BalanceWindow::~BalanceWindow()
{
    delete ui;
}

void BalanceWindow::onproxyAuthenticationRequired(QNetworkProxy proxy, QAuthenticator *manager)
{
    manager->setUser("Anonumouse");
    manager->setPassword("12345");
}

void BalanceWindow::finishLoad(bool finished){
    if(finished){
        if(webView->page()->mainFrame()->toPlainText().indexOf("Лицевой счет") < 0){
            QWebElement doc = webView->page()->mainFrame()->documentElement().findFirst("input[type=\"text\"]");
            doc.setAttribute("value", "241591301");

            doc = webView->page()->mainFrame()->documentElement().findFirst("input[type=\"password\"]");
            doc.setAttribute("value", "89233206703");

            doc = webView->page()->mainFrame()->documentElement().findFirst("input[type=\"image\"]");
            //qDebug()<<doc.toOuterXml();
            QString name = doc.attribute("name");
            webView->page()->mainFrame()->evaluateJavaScript("document.getElementsByName(\""+name+"\").item(0).click();");

            /*
            doc.evaluateJavaScript("mouseUp()");
            doc.evaluateJavaScript("click()");

            /* it not work :(
            doc = webView->page()->mainFrame()->documentElement().findFirst("form[action=\"/login\"]");
            QString id = doc.attribute("id");
            qDebug()<<id;
            webView->page()->mainFrame()->evaluateJavaScript("document.getElementById(\""+id+"\").submit();");*/

        }
        else{
            QWebElement doc = webView->page()->mainFrame()->documentElement().findFirst("div[class=\"main\"]");
            QString balanceString = doc.nextSibling().nextSibling().lastChild().lastChild().lastChild().toPlainText();

            QRegExp rx("(\\d*\\,\\d+)");
            rx.indexIn(balanceString);

            double balance;
            if(balanceString.indexOf("Задолженность") >= 0)
                balance = -QString(rx.capturedTexts().at(0)).replace(",",".").toDouble();
            else
                balance = QString(rx.capturedTexts().at(0)).replace(",",".").toDouble();
            //qDebug()<<
            ui->label->setText(QString("%1").arg(balance));
            //qDebug()<<doc.toPlainText();
            //doc.nextSibling();

        }

    }
    else
    {
        qDebug()<<"load failed";
    }
}
