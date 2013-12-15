#include "balancewindow.h"
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

    updateInfo();

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

    settings = new Settings();
    connect(settings, SIGNAL(balanceChanged()), this, SLOT(updateInfo()));

    updateTimer->setInterval(settings->updatePeriodMsec());
    if(settings->autoUpdate())
        updateTimer->start();
    else
        updateTimer->stop();



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



void BalanceWindow::updateBalance(){
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

    webView->setupProxyConnection(settings->proxyType(),
                                  settings->proxyAdress(),
                                  settings->proxyPort(),
                                  settings->proxyAuth(),
                                  settings->proxyLogin(),
                                  settings->proxyPass());

    webView->load(QUrl("https:////bill.sibttk.ru//login"));

    //        if(d)
    //            webView->show();
}

void BalanceWindow::setStatus(QString text){
    status->setText(text);
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

void BalanceWindow::showMessage(Settings::LevelBalance level)
{
    //    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(
    //            typeComboBox->itemData(typeComboBox->currentIndex()).toInt());
    QSystemTrayIcon::MessageIcon icon;
    switch(level){
    case Settings::WARNING:
        icon = QSystemTrayIcon::Warning;
        break;
    case Settings::CRITICAL:
        icon = QSystemTrayIcon::Critical;
        break;
    default:
        icon = QSystemTrayIcon::Information;

    }
    trayIcon->showMessage("Текущее состояние счёта",
                          QString("Баланс: %1 руб.\nРекомендуется пополнить счёт до %2")
                          .arg(settings->balance())
                          .arg(settings->needPay()),
                          icon, 1000);
    
}



void BalanceWindow::progressLoad(int f){

    isAuthPage ? progress->setValue(f/2) : progress->setValue(50+(f/2));

}

void BalanceWindow::finishLoad(bool finished){
    if(finished){
        if(webView->isHaveText("Лицевой счет")){ //Get balance
            setStatus("Авторизация прошла успешно");

            settings->setBalance(webView->getBalance());

            isAuthPage = true;

            //delete webView;
        }
        else if(webView->isHaveText("Введенная информация неверна")){
            setStatus("Ошибка авторизации");
            progress->setValue(100);
            //delete webView;
        }
        else{   //Login to bill system
            if(isAuthPage){
                setStatus("Авторизация");

                webView->tryAuth(settings->login(),settings->pass());
                settings->endGroup();

                isAuthPage = false;}
            else{
                setStatus("Что то пошло не так");
                //delete webView;
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

void BalanceWindow::updateInfo()
{
    double balance = settings->balance();
    ui->balance->setText(QString("%1").arg(balance));
    updateTrayIcon(balance);
    ui->lastUpdate->setText(settings->lastUpdate());
    ui->dayTo->setText(QString("%1").arg(settings->dayToPay()));

    ui->payTo->setText(settings->needPay());

    if(settings->levelBalance()){
        if(settings->trayNotice())
            showMessage(settings->levelBalance());
        if(settings->emailNotice())
            sendEmail(settings->levelBalance());
    }


    setStatus("Готово");
}

void BalanceWindow::sendEmail(Settings::LevelBalance level){
    Smtp* smtp = new Smtp(settings->smtpLogin(),
                          settings->smtpPass(),
                          settings->smtpServer(),
                          settings->smtpPort());

    //connect(smtp, SIGNAL(status(QString)), this, SLOT(mailSent(QString)));

    QString levelText;
    QString subject;
    switch(level){
    case Settings::WARNING:
        levelText = "ниже рекомендуемого";
        subject = "Рекомендуется пополнить баланс";
        break;
    case Settings::CRITICAL:
        levelText = "ниже критического";
        subject = "Рекомендуется пополнить баланс";
        break;
    default:
        levelText = "нормальный";
        subject = "Сведения о балансе";
    }

    QString textMessage;

    QFile message(QString("%1\\mailText.html").arg(QApplication::applicationDirPath()));
    if(message.open(QFile::ReadOnly)){
        textMessage = QString(message.readAll());
        message.close();
    }

    smtp->sendMail("Balance Manager",
                   settings->smtpEmail(),
                   subject,
                   textMessage.arg(levelText)
                   .arg(settings->balance())
                   .arg(settings->needPay())
                   .arg(settings->updatePeriod())
                   );
}


void BalanceWindow::on_quitAction_triggered()
{
    QCoreApplication::exit();
}

void BalanceWindow::on_settingsAction_triggered()
{
    SettingsDialog *d = new SettingsDialog(settings, this);
    connect(d,SIGNAL(testEmail()), this, SLOT(sendEmail()));
    d->setModal(true);
    d->show();
}


