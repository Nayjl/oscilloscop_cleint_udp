#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include <QUdpSocket>
#include <QDebug>

#include "udpsocket.h"
#include "qcustomplot.h"

#define TEORIC_MAX_BYTE_SEND_UDP 65507
#define STANDART_SEND_BYTE_SOCKET 1472
#define REAL_SEND_BYTE_SOCKET 1400
#define NUMBER_SEND_WORD32_SOCKET (REAL_SEND_BYTE_SOCKET / 4)
#define OFFSET_SEND (NUMBER_SEND_WORD32_SOCKET)



QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();




private:
    int status;
    Ui::MainWindow *ui;
    QTimer dataTimer;
    QCustomPlot *customPlot;

    void setupPlot();
    void renderingPlot();
    void calcSizeShot();

    QVector<double> xTime;
    QVector<double> yAmpl;
    int sampleRate;
    double timeStep;
    int number_din_data;
    int *buffSock;

    QUdpSocket *socket_client;
    QHostAddress hostAddress;
    quint16 port;
    QLineEdit *hostLineEdit;
    QLineEdit *portLineEdit;
    QByteArray datagram;



private slots:
    void realtimeDataSlot();

    void on_pushButton_conn_clicked();
    void parseData();
    void readPendingDatagrams();
    void on_statrplot_clicked();
};
#endif // MAINWINDOW_H
