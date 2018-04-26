#include "tcp_ssl.h"
#include "ui_tcp_ssl.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>

tcp_ssl::tcp_ssl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::tcp_ssl)
{    
    eland_id = 1;
    tcp_waiting = false;
    ui->setupUi(this);
}

tcp_ssl::~tcp_ssl()
{
    delete ui;
}

void tcp_ssl::replayfinished(QNetworkReply *reply)
{
    if(tm.isActive())
        tm.stop();
    if(reply->error() == QNetworkReply::NoError)
    {
        QTextCodec *codec = QTextCodec::codecForName("utf-8");

        QString str = codec->toUnicode(reply->readAll());

        ui->textEdit_reponse->setText(str);
    }
    else
        ui->textEdit_reponse->setText(tr("error = ") + QString::number(reply->error(),10));
    QDateTime currenttime = QDateTime::currentDateTime();
    QString currenttime_str = currenttime.toString("yyyy-MM-dd hh:mm:ss.zzz dddd t");
    ui->textEdit_reponse->append(currenttime_str);
    reply->deleteLater();
    tcp_waiting = false;
}

void tcp_ssl::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        this->close();
    }
}

void tcp_ssl::on_lineEdit_eland_id_textChanged(const QString &arg1)
{
    QString eland_id_string;
    bool ok;
    eland_id = arg1.toInt(&ok,10);
    eland_id_string = QString("%1").arg(eland_id,6,10,QLatin1Char('0'));
    ui->label_9->setText(QString::number(eland_id,10));
    ui->lineEdit_serial->setText(eland_id_string);
}

void tcp_ssl::on_lineEdit_model_textChanged(const QString &arg1)
{
    QString model_str = arg1;
    QString year_str =  QString::number(((ui->comboBox_year->currentIndex() + 8) % 10),10);
    QString month_str = QChar(ui->comboBox_mouth->currentIndex() + 'A');
    QString serial_str = ui->lineEdit_serial->text();
    ui->label_7->setText(model_str + month_str + year_str +serial_str);
}


void tcp_ssl::on_comboBox_mouth_currentIndexChanged(int index)
{
    QString model_str = ui->lineEdit_model->text();
    QString year_str = QString::number(((ui->comboBox_year->currentIndex() + 8) % 10),10);
    QString month_str = QChar(index + 'A');
    QString serial_str = ui->lineEdit_serial->text();
    ui->label_7->setText(model_str + month_str + year_str +serial_str);
}

void tcp_ssl::on_comboBox_year_currentIndexChanged(int index)
{
    QString model_str = ui->lineEdit_model->text();
    QString year_str = QString::number(((index + 8) % 10),10);
    QString month_str = QChar(ui->comboBox_mouth->currentIndex() + 'A');
    QString serial_str = ui->lineEdit_serial->text();
    ui->label_7->setText(model_str + month_str + year_str +serial_str);
}

void tcp_ssl::on_lineEdit_serial_textChanged(const QString &arg1)
{
    QString model_str = ui->lineEdit_model->text();
    QString year_str = QString::number(((ui->comboBox_year->currentIndex() + 8) % 10),10);
    QString month_str = QChar(ui->comboBox_mouth->currentIndex() + 'A');
    QString serial_str = arg1;
    ui->label_7->setText(model_str + month_str + year_str +serial_str);
}

void tcp_ssl::on_pushButton_get_clicked()
{
    if(tcp_waiting)
        return;
    m_manager = new QNetworkAccessManager(this);
    QNetworkRequest request;

    request.setRawHeader("host","192.168.0.1");
    request.setUrl(QUrl("http://192.168.0.1/setting"));

    connect(m_manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(replayfinished(QNetworkReply*)));
    connect(&tm,SIGNAL(timeout()),this,SLOT(handleTimeOut()));

    m_reply = m_manager->get(request);
    tcp_waiting = true;
    if(tm.isActive())
        tm.stop();
    tm.start(2000);
}

void tcp_ssl::on_pushButton_post_clicked()
{
    if(tcp_waiting)
        return;
    QJsonObject json;
    json.insert("eland_id",eland_id);
    json.insert("serial_number",  ui->label_7->text()  );
    json.insert("eland_name", "eland" );
    json.insert("timezone_offset_sec",  32400 );

    json.insert("user_id", "");
    json.insert("dhcp_enabled",  1 );
    json.insert("ip_address",  "" );
    json.insert("subnet_mask",  "" );
    json.insert("default_gateway", ""  );
    json.insert("primary_dns",""   );
    json.insert("area_code",  43 );

    QJsonDocument document;
    document.setObject(json);
    QByteArray bytearray = document.toJson(QJsonDocument::Compact);

    m_manager = new QNetworkAccessManager(this);
    QNetworkRequest request;

    request.setRawHeader("Content-Type","application/json; charset=utf-8");
    request.setRawHeader("host","192.168.0.1");
    request.setUrl(QUrl("http://192.168.0.1/setting"));

    connect(m_manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(replayfinished(QNetworkReply*)));
    connect(&tm,SIGNAL(timeout()),this,SLOT(handleTimeOut()));

    m_reply = m_manager->post(request, bytearray);
    tcp_waiting = true;
    if(tm.isActive())
        tm.stop();
    tm.start(2000);
}

void tcp_ssl::handleTimeOut()
{
    m_reply->abort();
    m_reply->deleteLater();
    ui->textEdit_reponse->setText(tr("time out"));
    QDateTime currenttime = QDateTime::currentDateTime();
    QString currenttime_str = currenttime.toString("yyyy-MM-dd hh:mm:ss.zzz dddd t");
    ui->textEdit_reponse->append(currenttime_str);
}
