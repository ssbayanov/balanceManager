#ifndef BALANCEWINDOW_H
#define BALANCEWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>

#include <QSystemTrayIcon>

#include <QDebug>

#include <QTimer>

#include <QTextCodec>

#include <QProgressBar>

#include <QMenu>

#include "smtp.h"

#include "settings.h"

#include "settingsdialog.h"

#include "balancewebview.h"

const bool d = true;

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
    QSystemTrayIcon::MessageIcon INFO, WARNING, CRYTICAL;

    Ui::BalanceWindow *ui;



    BalanceWebView *webView;
    bool isAuthPage;

    QProgressBar *progress;
    QLabel *status;

    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;



    QPixmap ico;
    QIcon icon;

    QTimer *updateTimer;
    QTimer *trayMessageTimer;

    Settings *settings;


    void setupStatusBar();
    void setupWebView();
    void setupTrayIcon();

    void updateTrayIcon(double b);
    void updateMainSettings();

    void closeEvent(QCloseEvent * event);
private slots:
    void setStatus(QString text);

    void updateBalance();

    void updateInfo();

    void sendEmail(Settings::LevelBalance level = Settings::NORMAL);

    void finishLoad(bool finished);
    void progressLoad(int);

    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showMessage(Settings::LevelBalance level = Settings::NORMAL);
    void on_quitAction_triggered();
    void on_settingsAction_triggered();

    void getSettings();

};

#endif // BALANCEWINDOW_H
