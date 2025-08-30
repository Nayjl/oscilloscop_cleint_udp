#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    customPlot = ui->plot;
    setupPlot();

    calcSizeShot();
    timeStep = 1.0 / (double)sampleRate;
    for (int i = 0; i < number_din_data; ++i) {
        xTime.append(i * timeStep);
    }
    datagram.resize(sizeof(int) * OFFSET_SEND);
    xTime.resize(number_din_data * sizeof(int));
    yAmpl.resize(number_din_data * sizeof(int));

    // buffSock = new int(number_din_data);

    socket_client = new QUdpSocket();
    connect(socket_client, &QUdpSocket::readyRead, this, &MainWindow::readPendingDatagrams);

    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(parseData()));
    dataTimer.start(0); // Interval 0 means to refresh as fast as possible
}

MainWindow::~MainWindow() {
    // delete(xTime);
    // delete(yAmpl);
    // delete(buffSock);
    delete(socket_client);
    delete ui;
}


void MainWindow::setupPlot() {
    customPlot->addGraph(); // blue line
    customPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));

    customPlot->xAxis->setLabel("Время (сек)"); // Или номер отсчета
    customPlot->yAxis->setLabel("Амплитуда (Чмсло)"); // Или просто условные единицы

    customPlot->xAxis->setRange(0, number_din_data * timeStep); // Показываем первые 10 секунд
    customPlot->yAxis->setRange(-100, 100); // Диапазон амплитуды от -2 до 2

    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

    customPlot->xAxis->grid()->setVisible(true);
    customPlot->yAxis->grid()->setVisible(true);
}



// void MainWindow::realtimeDataSlot() {
//     static QTime time(QTime::currentTime());
//     // calculate two new data points:
//     double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds
//     static double lastPointKey = 0;
//     if (key-lastPointKey > 0.002) // at most add point every 2 ms
//     {
//         // add data to lines:
//         customPlot->graph(0)->addData(key, qSin(key)+qrand()/(double)RAND_MAX*1*qSin(key/0.3843));
//         customPlot->graph(1)->addData(key, qCos(key)+qrand()/(double)RAND_MAX*0.5*qSin(key/0.4364));
//         // rescale value (vertical) axis to fit the current data:
//         //ui->customPlot->graph(0)->rescaleValueAxis();
//         //ui->customPlot->graph(1)->rescaleValueAxis(true);
//         lastPointKey = key;
//     }
//     // make key axis range scroll with the data (at a constant range size of 8):
//     customPlot->xAxis->setRange(key, 8, Qt::AlignRight);
//     customPlot->replot();

//     // calculate frames per second:
//     static double lastFpsKey;
//     static int frameCount;
//     ++frameCount;
//     if (key-lastFpsKey > 2) // average fps over 2 seconds
//     {
//         // ui->statusBar->showMessage(
//             // QString("%1 FPS, Total Data points: %2")
//             //     .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
//             //     .arg(ui->widget->graph(0)->data()->size()+ui->widget->graph(1)->data()->size())
//             // , 0);
//         lastFpsKey = key;
//         frameCount = 0;
//     }
// }





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

    qint32 networkInt;
    const double scaleFactor = 1.0;
    const double offset = 0.0;
    const char* intBytes;
    while (socket_client->hasPendingDatagrams()) {
        socket_client->readDatagram(datagram.data(), datagram.size(), &hostAddress, &port);
    }


    //const double* dataSize = reinterpret_cast<const double*>(datagram.constData());
    size_t sizeWorrdVect = datagram.size()/sizeof(double);
    QVector <double> dataARM;
    dataARM.resize(sizeWorrdVect);
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian)
    {
        memcpy((dataARM.data()), datagram.constData(), datagram.size());
        qDebug() << "Big Endian system";
    }
    else
    {
        const char* dataLit =datagram.constData();
        for(int i = 0; i < sizeWorrdVect; i++)
        {
            dataARM[i] =qFromBigEndian<double>(dataLit + i * sizeof(double));
        }
        qDebug() << "Little Endian system";
    }

    int rawData = datagram.toInt();

    for (int i = 0; i < OFFSET_SEND; ++i) {
        intBytes = rawData + (i * 4);
        memcpy(&networkInt, intBytes, 4);
        yAmpl[i] = networkInt * scaleFactor + offset;
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


void MainWindow::on_statrplot_clicked() {
    calcSizeShot();
    timeStep = 1.0 / (double)sampleRate;
    for (int i = 0; i < number_din_data; ++i) {
        xTime[i] = (i * timeStep);
    }
    xTime.resize(number_din_data * sizeof(int));
    yAmpl.resize(number_din_data * sizeof(int));
    customPlot->xAxis->setRange(0, number_din_data * timeStep);
}

