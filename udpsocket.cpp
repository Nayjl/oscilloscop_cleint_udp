#include "udpsocket.h"

udpSocket::udpSocket() {

}

udpSocket::udpSocket(char *ip_addr, uint16_t port) {
    this->port = port;
    createSocket();
    setIPADDR(ip_addr, port);
    client_len_ = sizeof(client_addr_);
    setupSocketNotifier();
}

udpSocket::udpSocket(uint16_t port) {
    this->port = port;
    createSocket();
    setIPADDR(port);
    client_len_ = sizeof(client_addr_);
    bindSocketServer();
    setupSocketNotifier();
}

udpSocket::~udpSocket() {
    if (notifier) {
        delete notifier;
        notifier = nullptr;
    }
    if (fd >= 0) {
        close(fd);
        fd = -1;
    }
}

int udpSocket::writeSocket(void *buffPrt, size_t sizeBuff, int flags) {
    bytesRxTx = sendto(fd, buffPrt, sizeBuff, flags, (struct sockaddr*)&client_addr_, client_len_);
    return bytesRxTx;
}

int udpSocket::writeSocket(void *buffPrt, size_t sizeBuff, int flags, sockaddr_in *client_addr, size_t client_len) {
    bytesRxTx = sendto(fd, buffPrt, sizeBuff, flags, (struct sockaddr*)client_addr, client_len);
    return bytesRxTx;
}

int udpSocket::readSocket(void *buffPrt, size_t sizeBuff, int flags) {
    client_len_ = sizeof(client_addr_);
    bytesRxTx = recvfrom(fd, buffPrt, sizeBuff, flags, (struct sockaddr*)&client_addr_, &client_len_);
    return bytesRxTx;
}

// int udpSocket::readSocket(void *buffPrt, size_t sizeBuff, int flags, struct sockaddr_in *client_addr , size_t *client_len) {
//     bytesRxTx = recvfrom(fd, buffPrt, sizeBuff, flags, (struct sockaddr*)client_addr, client_len);
//     return bytesRxTx;
// }

int udpSocket::bindSocketServer() {
    status = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    return status;
}

void udpSocket::setIPADDR(char *ip_addr, uint16_t port) {
    // memset(addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip_addr);
}


void udpSocket::setIPADDR(uint16_t port) {
    // memset(addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
}


int udpSocket::createSocket() {
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    status = fd;
    return status;
}

void udpSocket::setupSocketNotifier() {
    if (fd < 0 || notifier)
        return;
    notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    connect(notifier, &QSocketNotifier::activated, this, [this]() {emit readyRead();});
}

bool udpSocket::hasPendingDatagrams() const {
    if (fd < 0) return false;
    if (ioctl(fd, FIONREAD, &sizeByteReadyRead) == 0) {
        return sizeByteReadyRead > 0;
    }
    return false;
}


bool udpSocket::hasPendingDatagrams(int *sizeByteReadyRead) const {
    if (fd < 0) return false;
    if (ioctl(fd, FIONREAD, sizeByteReadyRead) == 0) {
        return *sizeByteReadyRead > 0;
    }
    return false;
}


