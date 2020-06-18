#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QTcpSocket>

class TcpClient : public QWidget
{
    Q_OBJECT
private:
    QTcpSocket* socket;

    void jsonReceived(const QJsonObject &doc);


public:
    explicit TcpClient(QWidget *parent = nullptr);

    void Connect(const QString& strHost, int nPort);
    void SendServerResponse();


signals:

private slots:
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError);
    //void slotSendToServer();
    void slotConnected();
    void slotDisconnected();

};

#endif // TCPCLIENT_H
