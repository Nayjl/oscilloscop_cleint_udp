#include "tcpsocket.h"

tcpSocketServer::tcpSocketServer() {

}

tcpSocketServer::tcpSocketServer(uint16_t port) {
    createSocket();
    setIPADDR(port);
    bindSocketServer();
    setupSocketNotifier();
}

void tcpSocketServer::setIPADDR(uint16_t port) {
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
}


int tcpSocketServer::createSocket() {
    fd = socket(AF_INET, SOCK_STREAM, 0);
    status = fd;
    return status;
}


int tcpSocketServer::bindSocketServer() {
    status = bind(fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    return status;
}

int tcpSocketServer::listenSocket() {
    status = listen(fd, numberClient);
    return status;
}

tcpSocketClient tcpSocketServer::acceptIncommingConnection() {
    tcpSocketClient socketClient;
    socketClient.addrLen = sizeof(socketClient.addr_);
    socketClient.fd = accept(fd, (struct sockaddr *)&(socketClient.addr_), &(socketClient.addrLen));
    setupSocketNotifier(socketClient.fd);
    return socketClient;
}

void tcpSocketServer::setupSocketNotifier(int fd) {
    if (fd < 0 || notifier)
        return;
    notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    QObject::connect(notifier, &QSocketNotifier::activated, this, [this]() {emit incommingConnection();});
}

void tcpSocketServer::setupSocketNotifier() {
    if (fd < 0 || notifier)
        return;
    notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    QObject::connect(notifier, &QSocketNotifier::activated, this, [this]() {emit incommingConnection();});
}

bool tcpSocketServer::hasPendingDatagrams() const {
    if (fd < 0) return false;
    if (ioctl(fd, FIONREAD, &sizeByteReadyRead) == 0) {
        return sizeByteReadyRead > 0;
    }
    return false;
}

tcpSocketClient::tcpSocketClient(){

}

tcpSocketClient::tcpSocketClient(char *ip_addr, uint16_t port) {
    createSocket();
    setIPADDR(ip_addr, port);
    // connectToServ();
}

tcpSocketClient::~tcpSocketClient() {
    if (fd >= 0) {
        close(fd);
        fd = -1;
    }
    delete notifier;
}


int tcpSocketClient::connectToServ() {
    addrLen = sizeof(addr_);
    status = connect(fd, (struct sockaddr *)&addr_, addrLen);
    setupSocketNotifier();
    return status;
}

int tcpSocketClient::writeSocket(void *buffPrt, size_t sizeBuff, int flags) {
    bytesRxTx = (int)send(fd, buffPrt, sizeBuff, flags);
    return bytesRxTx;
}

int tcpSocketClient::readSocket(void *buffPrt, size_t sizeBuff, int flags) {
    bytesRxTx = (int)recv(fd, buffPrt, sizeBuff, flags);
    return bytesRxTx;
}

void tcpSocketClient::setIPADDR(char *ip_addr, uint16_t port) {
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = inet_addr(ip_addr);
    addr_.sin_port = htons(port);
}

void tcpSocketClient::setupSocketNotifier() {
    if (fd < 0 || notifier)
        return;
    notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    QObject::connect(notifier, &QSocketNotifier::activated, this, [this]() {emit readyRead();});
}

bool tcpSocketClient::hasPendingDatagrams() const {
    if (fd < 0) return false;
    if (ioctl(fd, FIONREAD, &sizeByteReadyRead) == 0) {
        return sizeByteReadyRead > 0;
    }
    return false;
}

void tcpSocketClient::disconnectSocket() {
    if (fd >= 0) {
        close(fd);
    }
}

int tcpSocketClient::createSocket() {
    fd = socket(AF_INET, SOCK_STREAM, 0);
    status = fd;
    return status;
}
