#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QtSerialBus/qtserialbusglobal.h>
#include <QDialog>
#if QT_CONFIG(modbus_serialport)
#include <QSerialPort>
#endif

#include <QTableWidget>
//#include <QVector>

#include "servoline.h"

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    struct Settings {
#if QT_CONFIG(modbus_serialport)
        int parity = QSerialPort::OddParity;
        int baud = QSerialPort::Baud57600;
        int dataBits = QSerialPort::Data8;
        int stopBits = QSerialPort::OneStop;
#endif
        int responseTime = 1000;
        int numberOfRetries = 3;
        int Port = 10;

        QString TcpIP = "127.0.0.1";
        int TcpPort = 4442;
    };
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

    void initServo(QVector<Servoline>* servo_ptr);


    QVector<int> RegistrEditList;

    void DisplayRegistr(int ServoAddres, int addres, quint16 value);

    Settings settings() const;


    // Опишем сигналы:
signals:
        void  WriteModbusSignal(int Server, int RegistrAddres, quint16 value);
        void  ReadModbusSignal(int Server, int RegistrAddres, quint16 size); // Прочитать N регистров


private slots:
    void on_ButtonReadAll_clicked();


private:
    Settings m_settings;
    Ui::SettingsWindow *ui;

    QTableWidget *tableWidget;

    QVector<Servoline>* servo_array;
};

#endif // SETTINGSWINDOW_H
