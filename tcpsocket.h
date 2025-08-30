#ifndef TCPSOCKET_H
#define TCPSOCKET_H

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


class tcpSocketClient : public QObject {
    Q_OBJECT
public:
    tcpSocketClient();
    tcpSocketClient(char* ip_addr, uint16_t port);
    ~tcpSocketClient();
    int fd;
    int status;
    size_t bytesRxTx;
    struct sockaddr_in addr_;
    socklen_t addrLen;
    uint16_t port;
    int sizeByteReadyRead;
    void setIPADDR(char* ip_addr, uint16_t port);
    int connectToServ();
    int writeSocket(void *buffPrt, size_t sizeBuff, int flags);
    // int writeSocket(void *buffPrt, size_t sizeBuff, int flags, struct sockaddr_in *client_addr, size_t client_len);
    int readSocket(void *buffPrt, size_t sizeBuff, int flags);
    // int readSocket(void *buffPrt, size_t sizeBuff, int flags, struct sockaddr_in *client_addr, size_t *client_len);
    bool hasPendingDatagrams() const;
    void disconnectSocket();
private:
    int createSocket();
    void setupSocketNotifier();
    QSocketNotifier *notifier = nullptr;

signals:
    void readyRead();
};




class tcpSocketServer : public QObject
{
    Q_OBJECT
public:
    tcpSocketServer();
    tcpSocketServer(uint16_t port);
    ~tcpSocketServer();
    int fd;
    int client_fd;
    int status;
    size_t bytesRxTx;
    struct sockaddr_in serv_addr;
    uint16_t port;
    int numberClient;
    int sizeByteReadyRead;
    void setIPADDR(uint16_t port);
    int bindSocketServer();
    int listenSocket();
    tcpSocketClient acceptIncommingConnection();
    bool hasPendingDatagrams() const;


private:
    int createSocket();
    void setupSocketNotifier();
    void setupSocketNotifier(int fd);
    QSocketNotifier *notifier = nullptr;


signals:
    void incommingConnection();

};

#endif // TCPSOCKET_H
