#include "balancewindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QApplication a(argc, argv);
    BalanceWindow w;
    w.show();

    return a.exec();
}
