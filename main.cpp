#include "mainwindow.h"

#include <QApplication>


// #define PORT_UDP 8085
// char *ip_addr = "192.168.1.33";





int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    udp_clss udp_obj;
    if (udp_obj.status == -1) printf("Error: Object class udp_clss not successfully ctreate\n");
    else printf("Successfully: UDP client run\n");


    MainWindow w;
    w.show();


    return a.exec();
}
