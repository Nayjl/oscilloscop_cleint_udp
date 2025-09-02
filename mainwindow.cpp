#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    customPlot = ui->plot;
    writeIndex = 0;

    calcSizeShot();
    maxSizeBuffer = number_din_data * sizeof(double);
    datagram.resize(sizeof(int) * OFFSET_SEND);
    xTime.clear();
    yAmpl.clear();
    xTime.resize(number_din_data);
    yAmpl.resize(number_din_data);
    timeStep = 1.0 / (double)sampleRate;
    for (int i = 0; i < number_din_data; ++i) {
        xTime[i] = (i * timeStep);
    }

    socket_client = new QUdpSocket();
    connect(socket_client, &QUdpSocket::readyRead, this, &MainWindow::readPendingDatagrams);

    setupPlot();
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(parseData()));
    dataTimer.start(16); // Interval 0 means to refresh as fast as possible
}

MainWindow::~MainWindow() {
    delete(socket_client);
    delete ui;
}


void MainWindow::setupPlot() {
    customPlot->addGraph(); // blue line
    customPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));

    customPlot->xAxis->setLabel("Время (сек)");
    customPlot->yAxis->setLabel("Амплитуда (Чмсло)");

    customPlot->xAxis->setRange(0, number_din_data * timeStep);
    customPlot->yAxis->setRange(-100, 100);

    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

    customPlot->xAxis->grid()->setVisible(true);
    customPlot->yAxis->grid()->setVisible(true);
}


void MainWindow::on_pushButton_conn_clicked() {
    QString hostAddressStr = ui->lineEditipaddr->text();
    QString portStr = ui->lineEditport->text();
    QString message = ui->massageTextEdit->toPlainText();

    if (hostAddressStr.isEmpty() || portStr.isEmpty() || message.isEmpty()) {
        QMessageBox::warning(this, "Ошибка ввода", "Пожалуйста, заполните все поля.");
        return;
    }
    if (!hostAddress.setAddress(hostAddressStr)) {
        QMessageBox::warning(this, "Ошибка ввода", "Неверный формат IP адреса.");
        return;
    }
    port = portStr.toUShort();
    if (port == 0) {
        QMessageBox::warning(this, "Ошибка ввода", "Неверный номер порта.");
        return;
    }

    QByteArray datagram_tx;
    datagram_tx.setNum(message.toInt());

    qint64 written = socket_client->writeDatagram(datagram_tx, hostAddress, port);
    if (written == -1) {
        ui->logTextEdit->append("Ошибка отправки: " + socket_client->errorString());
    } else {
        ui->logTextEdit->append("Отправлено: " + message + " -> " + hostAddressStr + ":" + portStr);
    }
    ui->massageTextEdit->clear();
    message.clear();
}


void MainWindow::readPendingDatagrams() {
    qint64 sizeRecv;
    int *dataptr;
    while (socket_client->hasPendingDatagrams()) {
        sizeRecv = socket_client->readDatagram(datagram.data(), datagram.size(), &hostAddress, &port);
    }

    dataptr = (int*)datagram.data();
    if (sizeRecv >= (int)(sizeof(int) * OFFSET_SEND)) {
        double dataSocket;
        for (int i = 0; i < OFFSET_SEND; ++i) {
            dataSocket = (double)dataptr[i];
            yAmpl[writeIndex] = dataSocket;
            writeIndex++;
            if (writeIndex >= number_din_data) {
                writeIndex = 0;
            }
        }

        // if (yAmpl.size() > maxSizeBuffer) {
        //     int numToRemove = yAmpl.size() - maxSizeBuffer;
        //     yAmpl.remove(0, numToRemove);
        // }
    }


}


void MainWindow::parseData() {
    customPlot->graph(0)->setData(xTime, yAmpl);
    customPlot->yAxis->rescale();

    customPlot->replot();
}


void MainWindow::calcSizeShot() {
    sampleRate = ui->sampleRateLineEdit->text().toInt();
    number_din_data = ceil(sampleRate / NUMBER_SEND_WORD32_SOCKET);
    number_din_data = number_din_data * NUMBER_SEND_WORD32_SOCKET;
}


void MainWindow::on_statrplot_clicked() { // Для увуличения или уменьшения глубины отрисовки
    calcSizeShot();
    maxSizeBuffer = number_din_data * sizeof(double);
    writeIndex = 0;
    xTime.clear();
    yAmpl.clear();
    xTime.resize(number_din_data);
    yAmpl.resize(number_din_data);
    timeStep = 1.0 / (double)sampleRate;
    for (int i = 0; i < number_din_data; ++i) {
        xTime[i] = (i * timeStep);
    }
    customPlot->xAxis->setRange(0, number_din_data * timeStep);
}

