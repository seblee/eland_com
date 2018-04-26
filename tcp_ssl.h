#ifndef TCP_SSL_H
#define TCP_SSL_H

#include <QWidget>
#include <QKeyEvent>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTextCodec>
#include <QTimer>

namespace Ui {
class tcp_ssl;
}

class tcp_ssl : public QWidget
{
    Q_OBJECT

public:
    explicit tcp_ssl(QWidget *parent = 0);
    ~tcp_ssl();

private slots:
    void replayfinished(QNetworkReply *reply);

    void on_lineEdit_eland_id_textChanged(const QString &arg1);

    void on_lineEdit_model_textChanged(const QString &arg1);

    void on_comboBox_mouth_currentIndexChanged(int index);

    void on_comboBox_year_currentIndexChanged(int index);

    void on_lineEdit_serial_textChanged(const QString &arg1);

    void on_pushButton_get_clicked();

    void on_pushButton_post_clicked();

    void handleTimeOut();
private:
    Ui::tcp_ssl *ui;

    int eland_id;

    void keyPressEvent(QKeyEvent *event);

    QNetworkAccessManager *m_manager;

    QNetworkReply *m_reply;

    QTimer tm;

    bool tcp_waiting;
};

#endif // TCP_SSL_H
