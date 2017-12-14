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

    void on_open_close_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    bool serialOpened;
    uint32_t ReadDataTimes;
};

#endif // MAINWINDOW_H
