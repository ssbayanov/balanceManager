﻿#include "balancewindow.h"
#include "ui_balancewindow.h"

BalanceWindow::BalanceWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BalanceWindow)
{
    ui->setupUi(this);

    INFO = QSystemTrayIcon::Information;
    WARNING = QSystemTrayIcon::Warning;
    CRYTICAL = QSystemTrayIcon::Critical;


    isAuthPage = true;

    setCentralWidget(ui->verticalLayoutWidget);

    setupStatusBar();

    setupTrayIcon();

    trayMessageTimer = new QTimer();
    trayMessageTimer->setSingleShot(true);
    updateTimer = new QTimer();
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateBalance()));

    connect(ui->updateAction, SIGNAL(triggered()), this, SLOT(updateBalance()));



    getSettings();

    if(!d)
        updateBalance();


}

BalanceWindow::~BalanceWindow()
{
    delete ui;
}

void BalanceWindow::closeEvent(QCloseEvent * event)
{
    setVisible(false);
    if(!d)
        event->ignore();
}

void BalanceWindow::getSettings(){

    settings=new QSettings(QString("%1\\config.ini").arg(QApplication::applicationDirPath()), QSettings::IniFormat);
    settings->setIniCodec(QTextCodec::codecForName("UTF-8"));

    settings->beginGroup("MAIN");
    double b = settings->value("LastBalance", 0.00).toDouble();
    lastUpdate = settings->value("LastUpdate", QDateTime()).toDateTime();
    updateTimer->setInterval(settings->value("UpdatePeriod", 3600000*60).toInt()*3600000);
    _payment = settings->value("Payment",12.90).toDouble();
    if(settings->value("AutoUpdate", true).toBool())
        updateTimer->start();
    else
        updateTimer->stop();
    settings->endGroup();

    setBalance(b);

}

void BalanceWindow::setupStatusBar(){

    status = new QLabel(ui->statusBar);
    ui->statusBar->addWidget(status);
    status->setMinimumWidth(200);
    setStatus("Готово");


    progress = new QProgressBar(ui->statusBar);
    ui->statusBar->addWidget(progress);
    progress->setValue(100);
}

void BalanceWindow::setupTrayIcon(){
    trayIcon = new QSystemTrayIcon(this);
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    trayIcon->show();
    updateTrayIcon(120.);

    trayMenu = new QMenu();
    trayIcon->setContextMenu(trayMenu);

    trayMenu->addAction(ui->updateAction);
    trayMenu->addSeparator();
    trayMenu->addAction(ui->settingsAction);
    trayMenu->addSeparator();
    trayMenu->addAction(ui->quitAction);

}

void BalanceWindow::setupConnection(){
    proxy = new QNetworkProxy();
    manager = new QNetworkAccessManager(this);
    manager->setProxy(*proxy);
    connect(manager, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)), this, SLOT(onproxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));

    proxy->setType(QNetworkProxy::NoProxy);
}

void BalanceWindow::setupWebView(){
    webView = new BalanceWebView();

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

//        if(d)
//            webView->show();
}

void BalanceWindow::setStatus(QString text){
    status->setText(text);
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

void BalanceWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        if(trayMessageTimer)
            trayMessageTimer->stop();
        isVisible() ? setVisible(false) : setVisible(true);
        setFocus();
        break;
    case QSystemTrayIcon::Trigger:
        connect(trayMessageTimer,SIGNAL(timeout()),this,SLOT(showMessage()));
        trayMessageTimer->start(700);

        //showMessage();
        break;
    case QSystemTrayIcon::MiddleClick:
        showMessage();
    case QSystemTrayIcon::Context:
        trayIcon->contextMenu()->show();
        break;
    default:
        ;
    }
}

void BalanceWindow::showMessage(QSystemTrayIcon::MessageIcon icon)
{
    //    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(
    //            typeComboBox->itemData(typeComboBox->currentIndex()).toInt());
    trayIcon->showMessage("Текущее состояние счёта",
                          QString("Баланс: %1 руб.\n"+(icon = QSystemTrayIcon::Information)||(icon = QSystemTrayIcon::Warning)?"Рекомендуется пополнить счёт до %2":"Сегодня")
                          .arg(_balance)
                          .arg(needPay.toString("dd.MM.yy")),
                          icon, 1000);
    
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
        if(webView->isHaveText("Лицевой счет")){ //Get balance
            setStatus("Авторизация прошла успешно");

            updateBalance(webView->getBalance());

            isAuthPage = true;

            delete webView;
        }
        else if(webView->isHaveText("Введенная информация неверна")){
            setStatus("Ошибка авторизации");
            progress->setValue(100);
            delete webView;
        }
        else{   //Login to bill system
            if(isAuthPage){
                setStatus("Авторизация");
                settings->beginGroup("AUTH");
                webView->tryAuth(settings->value("Login").toString(),settings->value("Pass").toString());
                settings->endGroup();

                isAuthPage = false;}
            else{
                setStatus("Что то пошло не так");
                delete webView;
            }

        }

    }
    else
    {
        //qDebug()<<"load failed";
        setStatus("Ошибка загрузки страницы");
        delete webView;
    }


}

void BalanceWindow::updateBalance(double b)
{
    lastUpdate = QDateTime::currentDateTime();

    if(b < _balance)
        _payment = _balance - b;

    setBalance(b);

    updateMainSettings();

    setStatus("Готово");
}

void BalanceWindow::updateMainSettings(){
    settings->beginGroup("MAIN");
    settings->setValue("LastBalance",_balance);
    settings->setValue("Payment",_payment);
    settings->setValue("LastUpdate",lastUpdate);

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



    needPay = QDate::currentDate().addDays((_balance/_payment)-1);
    ui->payTo->setText(needPay.toString("dd.MM.yy"));

    settings->beginGroup("NOTICE");
    if(_balance < settings->value("CriticalBalance", 15).toDouble()){
        showMessage(QSystemTrayIcon::Critical);}
    else if(_balance < settings->value("WarningBalance", 70).toDouble()){
        showMessage(QSystemTrayIcon::Warning);}
    settings->endGroup();
}


void BalanceWindow::on_quitAction_triggered()
{
    QCoreApplication::exit();
}

void BalanceWindow::on_settingsAction_triggered()
{
    SettingsDialog *d = new SettingsDialog(this);
    d->setModal(true);
    d->show();
}


