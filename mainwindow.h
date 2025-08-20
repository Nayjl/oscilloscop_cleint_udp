#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QVector>


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qcustomplot.h"
#include "udp_clss.h"



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
    Ui::MainWindow *ui;
    QTimer dataTimer;
    QCustomPlot *customPlot;
    QVector<double> xp;
    QVector<double> xy;
    void setupPlot();
    void renderingPlot();




private slots:
    void realtimeDataSlot();

};
#endif // MAINWINDOW_H
