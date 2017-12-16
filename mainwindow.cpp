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
    FIRM_WARE_06,
} __msg_function_t;

typedef enum {
    ElandNone = 0,
    ElandBegin,
    APStatus,
    APStatusClosed,
    HttpServerStatus,
    HttpServerStop,
    ELAPPConnected,
    WifyConnected,
    WifyDisConnected,
    WifyConnectedFailed,
    ElandAliloPlay,
    ElandAliloPause,
    ElandAliloStop,
    HTTP_Get_HOST_INFO,
    TCPConnectedELSV,
    TCPHealthCheck,
} Eland_Status_type_t;

void MainWindow::Read_Data()
{
    static QByteArray buf;
    buf += serial->readAll();
    if(!buf.isEmpty())
    {
        ReadDataTimes++;
        if(ui->comboBox->currentIndex() == 0)//字符
        {
            QString str = ui->receivedtext->toPlainText();
            str += tr(buf);
            ui->receivedtext->clear();
            ui->receivedtext->append(str);
            buf.clear();
        }
        else if(ui->comboBox->currentIndex() == 1)//協議
        {
            while (!buf.isEmpty())
            {
                if(buf.startsWith((char)0x55))
                {
                    if(buf.length() < 3)
                        return;
                    else if(buf.length() < (buf.at(2) + 3))
                        return;
                    else if((uint8_t)buf.at(3 + buf.at(2)) != 0xaa)
                    {
                        buf.remove(0,1);
                        continue;
                    }
                    QString str;
                    str.clear();
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
                        str += tr("STATE:");
                        if(buf.at(3) == ElandBegin)
                            str += tr("ElandBegin \r\n");
                        else if(buf.at(3) == APStatus)
                            str += tr("StartAPStatus  \r\n");
                        else if(buf.at(3) == APStatusClosed)
                            str += tr("APStatusClosed\r\n");
                        else if(buf.at(3) == HttpServerStatus)
                            str += tr("HttpServer Start \r\n");
                        else if(buf.at(3) == HttpServerStop)
                            str += tr("HttpServerStop \r\n");
                        else if(buf.at(3) == ELAPPConnected)
                            str += tr("ELAPPConnected \r\n");
                        else if(buf.at(3) == WifyConnected)
                            str += tr("WifyConnected  \r\n");
                        else if(buf.at(3) == WifyDisConnected)
                            str += tr("WifyDisConnected  \r\n");
                        else if(buf.at(3) == WifyConnectedFailed)
                            str += tr("WifyFailed  \r\n");
                        else if(buf.at(3) == HTTP_Get_HOST_INFO)
                            str += tr("HTTP_Get_HOST_INFO  \r\n");
                        else if(buf.at(3) == TCPConnectedELSV)
                            str += tr("TCPConnectedELSV  \r\n");
                        ui->receivedtext->insertPlainText(str);
                        break;
                    case FIRM_WARE_06:
                        str += tr("firmware :") +tr((buf.data()+3)) +tr("\r\n");
                        ui->receivedtext->insertPlainText(str);
                        break;
                    default:
                        break;
                    }
                    buf.remove(0,4 + buf.at(2));
                    continue;
                }
                else
                {
                    buf.remove(0,1);
                }

            }
            qDebug() << "buf.size:  " << QString::number(buf.size(),10);
        }
        else if(ui->comboBox->currentIndex() == 2)//hex輸出
        {
            QString str;
            for(int i = 0;i < buf.size();i++)
            {
                //str.setNum()
                if((buf.at(i) < 0x10)&&(buf.at(i) >= 0))
                    str += tr("0");

                str += QString::number(buf.at(i)&0xFF,16) + tr(" ");
            }
            ui->receivedtext->insertPlainText(str);
            buf.clear();
        }

    }

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

void MainWindow::on_receivedtext_textChanged()
{
    QTextCursor cursor = ui->receivedtext->textCursor();
    ui->receivedtext->moveCursor(QTextCursor::End);
    ui->receivedtext->setTextCursor(cursor);
}
