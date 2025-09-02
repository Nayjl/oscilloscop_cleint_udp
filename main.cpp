#include "mainwindow.h"

#include <QApplication>


// #define PORT_UDP 8085
// char *ip_addr = "192.168.1.33";





int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
