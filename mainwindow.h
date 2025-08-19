#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>




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




private slots:
    void realtimeDataSlot();

};
#endif // MAINWINDOW_H
