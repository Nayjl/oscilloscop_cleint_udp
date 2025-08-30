#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <QObject>
#include <QSocketNotifier>

#include <unistd.h> // Unix Standard Header
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> // Sokets libs
#include <netinet/in.h> // Internet Protocol Family Header
#include <arpa/inet.h>
#include <string.h>
#include <sys/ioctl.h>


class udpSocket : public QObject
{
    Q_OBJECT
public:
    udpSocket();
    udpSocket(char* ip_addr, uint16_t port);
    explicit udpSocket(uint16_t port);
    ~udpSocket();
    int fd;
    int status;
    size_t bytesRxTx;
    struct sockaddr_in addr;
    struct sockaddr_in client_addr_;
    socklen_t client_len_;
    uint16_t port;
    int sizeByteReadyWrite;
    int sizeByteReadyRead;
    int writeSocket(void *buffPrt, size_t sizeBuff, int flags);
    int writeSocket(void *buffPrt, size_t sizeBuff, int flags, struct sockaddr_in *client_addr, size_t client_len);
    int readSocket(void *buffPrt, size_t sizeBuff, int flags);
    // int readSocket(void *buffPrt, size_t sizeBuff, int flags, struct sockaddr_in *client_addr, size_t *client_len);
    int bindSocketServer();
    void setIPADDR(uint16_t port);
    void setIPADDR(char* ip_addr, uint16_t port);
    bool hasPendingDatagrams() const;
    bool hasPendingDatagrams(int *) const;
private:
    int createSocket();
    void setupSocketNotifier();
    QSocketNotifier *notifier = nullptr;
signals:
    void readyRead();

};

#endif // UDPSOCKET_H
