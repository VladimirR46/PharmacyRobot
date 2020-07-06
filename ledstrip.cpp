#include "ledstrip.h"

LEDStrip::LEDStrip(SettingsWindow *settingsWindow, QWidget *parent) : QWidget(parent)
{
    p_settingsWindow = settingsWindow;

    serial = new QSerialPort(this);
    connect(serial, SIGNAL(readyRead()), this, SLOT(SerialReadyRead()));


    sendTimer = new QTimer();
    connect(sendTimer, SIGNAL(timeout()), this, SLOT(SendTimeout()));
}

unsigned short Crc16(unsigned char *pcBlock, unsigned short len)
{
    unsigned short crc = 0xFFFF;
    unsigned char i;

    while (len--)
    {
        crc ^= *pcBlock++ << 8;

        for (i = 0; i < 8; i++)
            crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
    }
    return crc;
}

bool LEDStrip::Connect()
{
    serial->close();
    serial->setPortName("COM"+QString::number(p_settingsWindow->settings().LEDPort));

    //setup COM port
    serial->setBaudRate(QSerialPort::BaudRate(9600));
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if(serial->open(QIODevice::ReadWrite)) return true;
    else return false;
}

void LEDStrip::SendMessage()
{
    serial->clear();
    serial->flush();
    serial->write((char*)&message,sizeof(message));
    qDebug() << "SEND " << TryCount;
}
//-------------------------------------------------------------------
void LEDStrip::SendTimeout()
{
    SendMessage();
    qDebug() << "SEND Timer";
    TryCount++;
    if(TryCount >= 3)
    {
        TryCount = 0;
        sendTimer->stop();
    }
}
//-------------------------------------------------------------------
void LEDStrip::SerialReadyRead()
{
    // read
    QByteArray serial_buffer = serial->readAll();

    if(serial_buffer.toStdString() == "OK") sendTimer->stop();
    qDebug() << serial_buffer;
}

void LEDStrip::ledON(int box, int id)
{
    message[0] = box;
    message[1] = id;
    message[2] = 1;
    message[3] = Crc16((unsigned char*)&message,6);
    SendMessage();

    TryCount = 0;
    sendTimer->start(200);
    qDebug() << "led on " << message[0] << message[1];
}

void LEDStrip::ledOFF(int box, int id)
{
    message[0] = box;
    message[1] = id;
    message[2] = 0;
    message[3] = Crc16((unsigned char*)&message,6);
    SendMessage();

    TryCount = 0;
    sendTimer->start(200);
    qDebug() << "led off";
}

