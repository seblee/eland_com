#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    serialOpened = false;
    ReadDataTimes = 0;
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Read_Data()
{
    QByteArray buf;
    buf=serial->readAll();
    if(!buf.isEmpty())
    {
        ReadDataTimes++;
        QString str = ui->receivedtext->toPlainText();

        str += tr(buf);
        ui->receivedtext->clear();
        ui->receivedtext->append(str);
    }
    buf.clear();
}
void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if(error == QSerialPort::ResourceError)
    {
        QMessageBox::critical(this,tr("Critical Error"),serial->errorString());
        closeserial();
    }
}

void MainWindow::on_open_close_clicked()
{
    if(serialOpened == false)
    {
        openserial();
    }
    else
        closeserial();
}

void MainWindow::openserial()
{
    foreach (const QSerialPortInfo &com_info, QSerialPortInfo::availablePorts())
    {
        if(com_info.description()=="USB Serial Port")
        {
            serial = new QSerialPort;
            serial->setPortName(com_info.portName());
            serial->open(QIODevice::ReadWrite);
            serial->setBaudRate(115200);
            serial->setDataBits(QSerialPort::Data8);
            serial->setParity(QSerialPort::NoParity);
            serial->setStopBits(QSerialPort::OneStop);
            serialOpened = true;
            QString str = "Serial Opened";
            //ui->receivedtext->insertPlainText(str);
            ui->receivedtext->append(str);
            connect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
                    this, &MainWindow::handleError);    //连接槽，串口出现问题连接到错误处理函数
            QObject::connect(serial,&QSerialPort::readyRead,this,&MainWindow::Read_Data);
            qDebug() << "find the serial: " << com_info.description();
            ui->open_close->setText(tr("Close serial"));
        }
    }
}

void MainWindow::closeserial()
{
    serial->clear();
    serial->close();
    serial->deleteLater();
    serialOpened = false;
    QString str =  "Serial closed";
    ui->receivedtext->append(str);
    ui->open_close->setText(tr("Open serial"));
}

void MainWindow::on_cleartext_clicked()
{
    ui->receivedtext->clear();
}
