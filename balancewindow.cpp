#include "balancewindow.h"
#include "ui_balancewindow.h"

BalanceWindow::BalanceWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BalanceWindow)
{
    ui->setupUi(this);

    _payment = 12.90;



    isAuthPage = true;

    setCentralWidget(ui->tabWidget);

    ui->tab->setLayout(ui->verticalLayout);

    setupStatusBar();

    setupTrayIcon();

        trayMessageTimer = new QTimer();
        trayMessageTimer->setSingleShot(true);
    updateTimer = new QTimer();
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateBalance()));

    getSettings();

    updateBalance();
}

BalanceWindow::~BalanceWindow()
{
    delete ui;
}

void BalanceWindow::closeEvent(QCloseEvent * event)
{
    setVisible(false);
    event->ignore();
}

void BalanceWindow::getSettings(){

    settings=new QSettings(QString("%1\\config.ini").arg(QApplication::applicationDirPath()), QSettings::IniFormat);
    settings->setIniCodec(QTextCodec::codecForName("UTF-8"));

    settings->beginGroup("MAIN");
    double b = settings->value("LASTBALANCE", 0.00).toDouble();
    lastUpdate = settings->value("LASTUPDATE", QDateTime()).toDateTime();
    updateTimer->setInterval(settings->value("AUPDINTERVAL", 60000).toInt());
    if(settings->value("AUTOUPDATE", true).toBool())
            updateTimer->start();
    else
            updateTimer->stop();

    settings->endGroup();

    setBalance(b);

}

void BalanceWindow::setupStatusBar(){
    progress = new QProgressBar(ui->statusBar);
    ui->statusBar->addWidget(progress);
    ui->statusBar->addWidget(new QProgressBar(ui->statusBar));
    QProgressBar(ui->statusBar->childAt(0,0)).setValue(100);
}

void BalanceWindow::setupTrayIcon(){
    trayIcon = new QSystemTrayIcon(this);
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    trayIcon->show();
    updateTrayIcon(120.);
}

void BalanceWindow::updateBalance(){
    setupWebView();
    setupConnection();
}

void BalanceWindow::updateTrayIcon(double b){
    QPainter i;

    ico = QPixmap(64,64);
    ico.fill(QColor("#fff"));
    i.begin(&ico);

    b < 1000 ?  i.setFont(QFont("Agency FB", 38)) : i.setFont(QFont("Agency FB", 30));
    i.drawText(QRect(0,0,64,64), Qt::AlignCenter, QString("%1").arg(b,0,'D' ,0));
    i.end();

    trayIcon->setIcon(QIcon(ico.scaled(16,16,Qt::KeepAspectRatio,Qt::SmoothTransformation)));
}

void BalanceWindow::setupConnection(){
    proxy = new QNetworkProxy();
    manager = new QNetworkAccessManager(this);
    manager->setProxy(*proxy);
    connect(manager, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)), this, SLOT(onproxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));

    proxy->setType(QNetworkProxy::NoProxy);
}

void BalanceWindow::setupWebView(){
    webView = new QWebView();

    //this->setCentralWidget(webView);
    //webView->show();

    webView->page()->settings()->setAttribute(QWebSettings::PluginsEnabled, false);
    webView->page()->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    webView->page()->settings()->setAttribute(QWebSettings::AutoLoadImages, false);
    webView->page()->settings()->setAttribute(QWebSettings::WebGLEnabled, false);

    //connect(webView,SIGNAL(loadStarted()),this,SLOT(startLoad()));
    connect(webView,SIGNAL(loadProgress(int)),this,SLOT(progressLoad(int)));
    connect(webView,SIGNAL(loadFinished(bool)),this,SLOT(finishLoad(bool)));

    webView->load(QUrl("https:////bill.sibttk.ru//login"));
}



void BalanceWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        if(trayMessageTimer)
            trayMessageTimer->stop();
        isVisible() ? setVisible(false) : setVisible(true);
        break;
    case QSystemTrayIcon::Trigger:
        connect(trayMessageTimer,SIGNAL(timeout()),this,SLOT(showMessage()));
        trayMessageTimer->start(700);

        //showMessage();
        break;
    case QSystemTrayIcon::MiddleClick:
        showMessage();
        break;
    default:
        ;
    }
}

void BalanceWindow::showMessage()
{
    //    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(
    //            typeComboBox->itemData(typeComboBox->currentIndex()).toInt());
    trayIcon->showMessage("Текущее состояние счёта",
                          QString("Баланс: %1 руб.\nРекомендуется пополнить счёт до %2")
                          .arg(_balance)
                          .arg("01.01.2001"),
                          QSystemTrayIcon::Information, 1000);
}

void BalanceWindow::onproxyAuthenticationRequired(QNetworkProxy proxy, QAuthenticator *manager)
{
    manager->setUser("Anonumouse");
    manager->setPassword("12345");
}

void BalanceWindow::progressLoad(int f){

    isAuthPage ? progress->setValue(f/2) : progress->setValue(50+(f/2));

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
            isAuthPage = false;
        }
        else{   //Get balance
            QString balanceString = webView->page()->mainFrame()->documentElement().findFirst("div[class=\"main\"]").nextSibling().nextSibling().lastChild().lastChild().toPlainText();

            //qDebug()<<balanceString;

            QRegExp rx("(\\d*\\,\\d+)");
            rx.indexIn(balanceString);

            if(balanceString.indexOf(QString("Задолженность %1").arg(rx.capturedTexts().last())) >= 0)
                updateBalance(-QString(rx.capturedTexts().last()).replace(",",".").toDouble());
            else
                updateBalance(QString(rx.capturedTexts().last()).replace(",",".").toDouble());
            isAuthPage = true;
        }

    }
    else
    {
        qDebug()<<"load failed";
    }
}

void BalanceWindow::updateBalance(double b)
{
    lastUpdate = QDateTime::currentDateTime();

    if(b < _balance)
        _payment = _balance - b;

    setBalance(b);

    updateMainSettings();
}

void BalanceWindow::updateMainSettings(){
    settings->beginGroup("MAIN");
    settings->setValue("LASTBALANCE",_balance);
    settings->setValue("PAYMENT",_payment);
    settings->setValue("LASTUPDATE",lastUpdate);

    settings->endGroup();
}

void BalanceWindow::setBalance(double b){
    _balance = b;
    ui->balance->setText(QString("%1").arg(b));
    updateTrayIcon(b);
    ui->lastUpdate->setText(lastUpdate.toString("dd.MM.yyг. hh:mm"));

    _balance >= 0 ?
                ui->dayTo->setText(QString("%1").arg((_balance/_payment),0,'D',0)):
                ui->dayTo->setText(0);

    ui->payTo->setText(QDate::currentDate().addDays((_balance/_payment)-1).toString("dd.MM.yy"));

}



void BalanceWindow::on_quitAction_triggered()
{
    QCoreApplication::exit();
}
