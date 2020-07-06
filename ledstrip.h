#ifndef LEDSTRIP_H
#define LEDSTRIP_H

#include <QWidget>
#include <QtSerialPort>
#include "settingswindow.h"



class LEDStrip : public QWidget
{
    Q_OBJECT
public:

    explicit LEDStrip(SettingsWindow *settingsWindow, QWidget *parent = nullptr);




    void SendMessage(int box, int index, int state);



public slots:
    bool Connect();
    void SerialReadyRead();
    void ledON(int box, int id);
    void ledOFF(int box, int id);


signals:

private:
    QSerialPort* serial;

    SettingsWindow *p_settingsWindow;

};

#endif // LEDSTRIP_H
