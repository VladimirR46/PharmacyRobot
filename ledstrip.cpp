#include "ledstrip.h"

LEDStrip::LEDStrip(SettingsWindow *settingsWindow, QWidget *parent) : QWidget(parent)
{
    p_settingsWindow = settingsWindow;

    serial = new QSerialPort(this);
    connect(serial, SIGNAL(readyRead()), this, SLOT(SerialReadyRead()));
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

void LEDStrip::SendMessage(int box, int index, int state)
{
    unsigned short message[4];
    message[0] = box;
    message[1] = index;
    message[2] = state;
    message[3] = Crc16((unsigned char*)&message,6);

    serial->clear();
    serial->flush();
    serial->write((char*)&message,sizeof(message));

    char v[4];
    v[0] = 23;
    serial->write(v,4);
}

void LEDStrip::SerialReadyRead()
{
    // read
    QByteArray serial_buffer = serial->readAll();
    qDebug() << serial_buffer;
}

void LEDStrip::ledON(int box, int id)
{
    SendMessage(box, id, 1);
}

void LEDStrip::ledOFF(int box, int id)
{
    SendMessage(box, id, 0);
}
