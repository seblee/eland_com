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

typedef enum {
    KEY_FUN_NONE = 0x00, /*空命令*/
    KEY_READ_02 = 0X02,
    TIME_SET_03,
    TIME_READ_04,
    ELAND_STATES_05,
} __msg_function_t;

typedef enum {
    ElandNone = 0,
    ElandBegin,
    ElandAPStatus,
    ElandHttpServerStatus,
    ElandWifyConnectedSuccessed,
    ElandWifyConnectedFailed,
    ElandAliloPlay,
    ElandAliloPause,
    ElandAliloStop,
} Eland_Status_type_t;

void MainWindow::Read_Data()
{
    QByteArray buf;
    buf=serial->readAll();
    if(!buf.isEmpty())
    {
        ReadDataTimes++;
        if(ui->comboBox->currentIndex() == 0)
        {
            QString str = ui->receivedtext->toPlainText();
            str += tr(buf);
            ui->receivedtext->clear();
            ui->receivedtext->append(str);
        }
        else if(ui->comboBox->currentIndex() == 1)
        {
            QString str;
            str.clear();
            if(buf.at(0) == 0x55)
            {
                switch (buf.at(1)) {
                case KEY_READ_02:
                    //str += tr("read key ");
                    //ui->receivedtext->insertPlainText(str);
                    break;
                case TIME_SET_03:
                    str += tr("time set \r\n");
                    ui->receivedtext->insertPlainText(str);
                    break;
                case TIME_READ_04:
                    str += tr("time read \r\n");
                    ui->receivedtext->insertPlainText(str);
                    break;
                case ELAND_STATES_05:
                    str += tr("state ");
                    if(buf.at(3) == ElandBegin)
                        str += tr("ElandBegin \r\n");
                    else if(buf.at(3) == ElandAPStatus)
                        str += tr("ElandAPStatus  \r\n");
                    else if(buf.at(3) == ElandHttpServerStatus)
                        str += tr("HttpServer  \r\n");
                    else if(buf.at(3) == ElandWifyConnectedSuccessed)
                        str += tr("WifyConnected  \r\n");
                    else if(buf.at(3) == ElandWifyConnectedFailed)
                        str += tr("WifyFailed  \r\n");
                    ui->receivedtext->insertPlainText(str);
                    break;
                default:
                    break;
                }

            }
            qDebug() << "buf.size:  " << QString::number(buf.size(),10);


        }
        else if(ui->comboBox->currentIndex() == 2)
        {
            QString str;
            for(int i = 0;i < buf.size();i++)
            {
                //str.setNum()
                if((buf.at(i) < 10)&&(buf.at(i) >= 0))
                    str += tr("0");
                str += QString::number(buf.at(i)&0xFF,16) + tr(" ");
            }
            ui->receivedtext->append(str);
        }
    }
    buf.clear();
}
void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    qDebug() << "SerialPortError " << QString::number(error,10);
    if(error == QSerialPort::ResourceError)
    {
        QMessageBox::critical(this,tr("Critical Error"),serial->errorString());
        closeserial();
    }
    else if(error == QSerialPort::PermissionError)
    {
        QMessageBox::critical(this,tr("Critical Error"),tr("\r\nThe serial has been already opened\r\nPlease Check"));
        //closeserial();
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
            connect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error), this, &MainWindow::handleError);    //连接槽，串口出现问题连接到错误处理函数

            serial->setPortName(com_info.portName());
            if(serial->open(QIODevice::ReadWrite) == true)
            {
                serial->setBaudRate(115200);
                serial->setDataBits(QSerialPort::Data8);
                serial->setParity(QSerialPort::NoParity);
                serial->setStopBits(QSerialPort::OneStop);
                serialOpened = true;
                QString str = serial->portName() + " Opened";
                ui->receivedtext->append(str);
                QObject::connect(serial,&QSerialPort::readyRead,this,&MainWindow::Read_Data);
                qDebug() << "find the serial: " << com_info.description();
                ui->open_close->setText(tr("Close serial ") + serial->portName());
            }
        }
    }
}

void MainWindow::closeserial()
{
    QString str = serial->portName() + " Closed";
    serial->clear();
    serial->close();
    serial->deleteLater();
    serialOpened = false;

    ui->receivedtext->append(str);
    ui->open_close->setText(tr("Open serial"));
}

void MainWindow::on_cleartext_clicked()
{
    ui->receivedtext->clear();
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    QString str = QString::number(index,10);
    qDebug() <<  "currentIndex:%d" <<  str   ;
}
