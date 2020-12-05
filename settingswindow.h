#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QtSerialBus/qtserialbusglobal.h>
#include <QDialog>
#if QT_CONFIG(modbus_serialport)
#include <QSerialPort>
#endif

#include <QSettings>
#include <QTableWidget>

#include "servoline.h"
#include "cellsettingswindow.h"
#include <QFileDialog>
#include <QFile>
#include <QDir>

#include "database.h"
#include "restockingwindow.h"

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    CellSettingsWindow* m_cellSettingsWindow;
    RestockingWindow *m_RestockWindow;

    bool FindProduct(DataBase::Cell& cell, int productCode);
    void DecreaseProductCount(DataBase::Cell& cell);

    void LoadBuyedList(QTableWidget *tableBuyed);
    void UpdateBuyedList();

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

        int OpenCartAngle = 0;
        int CloseCartAngle = 0;
        int UpCartAngle = 0;
        int DownCartAngle = 0;

        int CartPort = 0;

        int dTimeOpenClose = 400;
        int dTimeUpDown = 400;

        int LEDPort = 10;
    };
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

    void initServo(QVector<Servoline>* servo_ptr);


    QVector<int> RegistrEditList;

    void DisplayRegistr(int ServoAddres, int addres, quint16 value);

    Settings settings() const;

    void LoadDatabase();

    void saveSettings();
    void loadSettings();

    // Опишем сигналы:
    void SaveSettingsToStruct();
signals:
        void  WriteModbusSignal(int Server, int RegistrAddres, quint16 value);
        void  ReadModbusSignal(int Server, int RegistrAddres, quint16 size); // Прочитать N регистров
        void  GetCurrentPosSignal(QWidget* X, QWidget* Y);
        void  ledONSignal(int box, int id);
        void  ledOFFSignal(int box, int id);


private slots:
    void RestockedCell(int row, int count);

    void SaveCellConfigSlot(DataBase::Cell& cell);
    void DeleteCellSlot(DataBase::Cell& cell);
    void on_ButtonReadAll_clicked();

    void Click(int col,int row);
    void ClickCupboard(int col,int row);
    void ClickBuyed(int row,int col);


    void on_ButtonAddCupboard_clicked();
    void on_ButtonAddCell_clicked();

    void on_pushButton_clicked();

    void on_Update1Button_clicked();

    void on_Update2Button_clicked();

private:
    Settings m_settings;
    Ui::SettingsWindow *ui;
    QTableWidget *tableWidget;

    QVector<Servoline>* servo_array;

    QTableWidget* tableCupboard[5];

    QSettings* p_settings;

    DataBase database;

    QTableWidget *p_tableBuyed;
};

#endif // SETTINGSWINDOW_H
