#include "balancewindow.h"
#include "ui_balancewindow.h"

BalanceWindow::BalanceWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BalanceWindow)
{
    ui->setupUi(this);

    setCentralWidget(ui->verticalLayoutWidget);

    proxy = new QNetworkProxy();
    manager = new QNetworkAccessManager(this);
    manager->setProxy(*proxy);
    //connect(manager, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)), this, SLOT(onproxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));

    proxy->setType(QNetworkProxy::NoProxy);

    setupWebView();

    setupTrayIcon();

}

void BalanceWindow::setupTrayIcon(){
    trayIcon = new QSystemTrayIcon(this);

    trayIcon->show();
    updateTrayIcon(120.);
}

void BalanceWindow::updateTrayIcon(double b){
    QPainter i;

    ico = QPixmap(16,16);
    ico.fill(QColor("#fff"));
    i.begin(&ico);

    i.setFont(QFont("Arial", 7));
    i.drawText(QRect(0,0,16,16), Qt::AlignCenter, QString("%1").arg(b,0,'D' ,0));
    i.end();

    trayIcon->setIcon(QIcon(ico));
}

void BalanceWindow::setupWebView(){
    webView = new QWebView();

    //this->setCentralWidget(webView);
    //webView->show();

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
        if(webView->page()->mainFrame()->toPlainText().indexOf("Лицевой счет") < 0){ //Login to bill system
            QWebElement doc = webView->page()->mainFrame()->documentElement().findFirst("input[type=\"text\"]");
            doc.setAttribute("value", "241591301");

            doc = webView->page()->mainFrame()->documentElement().findFirst("input[type=\"password\"]");
            doc.setAttribute("value", "89233206703");

            doc = webView->page()->mainFrame()->documentElement().findFirst("input[type=\"image\"]");

            QString name = doc.attribute("name");
            webView->page()->mainFrame()->evaluateJavaScript("document.getElementsByName(\""+name+"\").item(0).click();");
        }
        else{   //Get balance
            QString balanceString = webView->page()->mainFrame()->documentElement().findFirst("div[class=\"main\"]").nextSibling().nextSibling().lastChild().lastChild().toPlainText();

            //qDebug()<<balanceString;

            QRegExp rx("(\\d*\\,\\d+)");
            rx.indexIn(balanceString);

            if(balanceString.indexOf(QString("Задолженность %1").arg(rx.capturedTexts().last())) >= 0)
                setBalance(-QString(rx.capturedTexts().last()).replace(",",".").toDouble());
            else
                setBalance(QString(rx.capturedTexts().last()).replace(",",".").toDouble());
        }

    }
    else
    {
        qDebug()<<"load failed";
    }
}

void BalanceWindow::setBalance(double b)
{
    _balance = b;
     ui->balance->setText(QString("%1").arg(b));
     updateTrayIcon(b);
}
