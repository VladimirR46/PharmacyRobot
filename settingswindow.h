#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QtSerialBus/qtserialbusglobal.h>
#include <QDialog>
#if QT_CONFIG(modbus_serialport)
#include <QSerialPort>
#endif

#include <QSettings>

#include <QTableWidget>
//#include <QVector>

#include "servoline.h"
#include "cellsettingswindow.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include <QFileDialog>
#include <QFile>
#include <QDir>

#include "database.h"

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:

    QJsonObject FindProduct(int code);
    void DecreaseCount(int code);

    struct Cashbox
    {
        int X = 0;
        int Y = 0;
    };

    struct Settings {
        int parity = QSerialPort::OddParity;
        int baud = QSerialPort::Baud57600;
        int dataBits = QSerialPort::Data8;
        int stopBits = QSerialPort::OneStop;
        int responseTime = 1000;
        int numberOfRetries = 3;
        int Port = 10;

        QString TcpIP = "127.0.0.1";
        int TcpPort = 4442;

        Cashbox cashbox[2];

    };
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

    void initServo(QVector<Servoline>* servo_ptr);


    QVector<int> RegistrEditList;

    void DisplayRegistr(int ServoAddres, int addres, quint16 value);

    Settings settings() const;

    void LoadDatabase();
    void LoadDBFromFile(QString openFileName);
    void SaveDBFromFile(QString saveFileName);

    void saveSettings();
    void loadSettings();

    // Опишем сигналы:
    void SaveSettingsToStruct();
signals:
        void  WriteModbusSignal(int Server, int RegistrAddres, quint16 value);
        void  ReadModbusSignal(int Server, int RegistrAddres, quint16 size); // Прочитать N регистров


private slots:

    void SaveCellConfigSlot(QJsonObject& obj, int box_, int line_, int cell_);
    void on_ButtonReadAll_clicked();

    void Click(int col,int row);
    void ClickCupboard(int col,int row);


    void on_ButtonAddCupboard_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_ButtonAddCell_clicked();

private:
    Settings m_settings;
    Ui::SettingsWindow *ui;
    CellSettingsWindow* m_cellSettingsWindow;

    QTableWidget *tableWidget;

    QVector<Servoline>* servo_array;

    QJsonObject db;

    QTableWidget* tableCupboard[5];

    QSettings* p_settings;

    DataBase database;
};

#endif // SETTINGSWINDOW_H
