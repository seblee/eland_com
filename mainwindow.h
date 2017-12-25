#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>

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

    void on_cleartext_clicked();

    void on_comboBox_currentIndexChanged(int index);  

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    bool serialOpened;
    uint32_t ReadDataTimes;
    void openserial();
    void closeserial();
};

#endif // MAINWINDOW_H
