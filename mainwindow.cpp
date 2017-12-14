#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>

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

void MainWindow::on_open_close_clicked()
{
    if(serialOpened == false)
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

                str+="Serial Opened";
                //ui->receivedtext->insertPlainText(str);
                ui->receivedtext->append(str);

                QObject::connect(serial,&QSerialPort::readyRead,this,&MainWindow::Read_Data);
                qDebug() << "find the serial: " << com_info.description();
            }

        }
    }
    else
    {
        serial->clear();
        serial->close();
        serial->deleteLater();
        serialOpened = false;
        QString str =  "Serial closed";
        ui->receivedtext->append(str);
    }
}
