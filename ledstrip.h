#ifndef LEDSTRIP_H
#define LEDSTRIP_H

#include <QWidget>
#include <QtSerialPort>
#include <QTimer>
#include "settingswindow.h"



class LEDStrip : public QWidget
{
    Q_OBJECT
public:

    explicit LEDStrip(SettingsWindow *settingsWindow, QWidget *parent = nullptr);

    unsigned short message[4];
    void SendMessage();

    ~LEDStrip();


public slots:
    bool Connect();
    void Disconnect();
    void SerialReadyRead();
    void ledON(int box, int id);
    void ledOFF(int box, int id);
    void SendTimeout();


signals:

private:
    QSerialPort* serial;

    QTimer* sendTimer;

    SettingsWindow *p_settingsWindow;

    int TryCount = 0;

};

#endif // LEDSTRIP_H
