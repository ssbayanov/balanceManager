#include "balancewindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BalanceWindow w;
    w.show();

    return a.exec();
}
