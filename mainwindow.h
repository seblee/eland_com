#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QKeyEvent>
#include "tcp_ssl.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void Read_Data();

    void handleError(QSerialPort::SerialPortError error);

    void on_open_close_clicked();

    void on_comboBox_currentIndexChanged(int index);  

    void on_Connect_clicked();

private:
    Ui::MainWindow *ui;

    QSerialPort *serial;

    bool serialOpened;

    uint32_t ReadDataTimes;

    void openserial();

    void closeserial();

    void keyPressEvent(QKeyEvent *event);
};

#endif // MAINWINDOW_H
