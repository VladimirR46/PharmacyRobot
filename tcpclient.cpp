#include "tcpclient.h"

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

TcpClient::TcpClient(QWidget *parent) : QWidget(parent)
{
    socket = new QTcpSocket(this);

    connect(socket, SIGNAL(connected()), this, SLOT(slotConnected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(slotDisconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));

}

void TcpClient::Connect(const QString& strHost, int nPort)
{
    qDebug() << "Connecting,..";

    socket->connectToHost(strHost, nPort);

    if(!socket->waitForDisconnected(2000))
    {
        qDebug() << "Error: " << socket->errorString();
    }
}
//---------------------------------------------------------------------------------------------------
void TcpClient::jsonReceived(const QJsonObject &docObj)
{

    int Cashbox = docObj["Cashbox"].toInt();
    qDebug() << Cashbox;

    QJsonArray array = docObj["CodeList"].toArray();

    foreach (const QJsonValue & v, array)
        qDebug() << v.toInt();

    const QByteArray jsonData = QJsonDocument(docObj).toJson(QJsonDocument::Compact);
    qDebug() << QString::fromUtf8(jsonData);
}
//---------------------------------------------------------------------------------------------------
void TcpClient::slotReadyRead()
{
    qDebug() << "Reading...";
    // prepare a container to hold the UTF-8 encoded JSON we receive from the socket
    QByteArray jsonData;
    // create a QDataStream operating on the socket
    QDataStream socketStream(socket);
    // set the version so that programs compiled with different versions of Qt can agree on how to serialise
  //socketStream.setVersion(QDataStream::Qt_5_7);
    // start an infinite loop
    for (;;) {
        // we start a transaction so we can revert to the previous state in case we try to read more data than is available on the socket
        socketStream.startTransaction();
        // we try to read the JSON data
        socketStream >> jsonData;
        if (socketStream.commitTransaction()) {
            // we successfully read some data
            // we now need to make sure it's in fact a valid JSON
            QJsonParseError parseError;
            // we try to create a json document with the data we received
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                // if the data was indeed valid JSON
                if (jsonDoc.isObject()) // and is a JSON object
                    jsonReceived(jsonDoc.object()); // parse the JSON
            }
            // loop and try to read more JSONs if they are available
        } else {
            // the read failed, the socket goes automatically back to the state it was in before the transaction started
            // we just exit the loop and wait for more data to become available
            break;
        }
    }
}
void TcpClient::slotError(QAbstractSocket::SocketError  err)
{
    QString strError =
            "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                         "The host was not found." :
                         err == QAbstractSocket::RemoteHostClosedError ?
                         "The remote host is closed." :
                         err == QAbstractSocket::ConnectionRefusedError ?
                         "The connection was refused." :
                         QString(socket->errorString())
                        );
    qDebug() << strError;
}

void TcpClient::slotConnected()
{
    qDebug() << "Connected!";
}
void TcpClient::slotDisconnected()
{
    qDebug() << "Disconnected!";
}
