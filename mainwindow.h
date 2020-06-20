#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModbusDataUnit>
#include "servoline.h"
#include "tcpclient.h"
#include <QTimer>
#include <QTime>

QT_BEGIN_NAMESPACE

class QModbusClient;
class QModbusReply;

namespace Ui
{
    class MainWindow;
    class SettingsWindow;
    class JOGWindow;
}

QT_END_NAMESPACE

class SettingsWindow;
class JOGWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private:
    void initActions();
    int GetEncoderFeedback(const QModbusDataUnit& unit);

    void WriteModbus(int Serves, QModbusDataUnit writeUnit);
    void SetBit(quint16 &value, quint8 bit, quint8 index);
    void ProcessPA508(int ServoAddres, quint16 value);
    void ProcessPA509(int ServoAddres, quint16 value);
    void isHOME(int ServoAddres, quint16 value);

    QVector<Servoline>::iterator FindServo(int ServoAddres); // Поиск обьекта сервы по адресу в массиве


private slots:
    void JOGSlot(int id, int state);

    void Update_dP13();

    void  WriteModbusRequest(int Server, int RegistrAddres, quint16 value);
    void  WriteIntModbusRequest(int Server, int RegistrAddres, qint32 value);
    void  ReadModbusRequest(int Server, int RegistrAddres, quint16 size); // Прочитать N регистров

    void onStateChanged(int state);
    void readReady();
    void ConnectToPort();
    void ConnectToTCP();

    void ServoInitialization();
    void on_UpdateCurPosButton_clicked();
    void on_checkUpdatePos_stateChanged(int arg1);
    void on_ReadPointButton_clicked();
    void on_PowerButton_clicked();
    void on_SHomeButton_clicked();

    void on_SetPoint0Button_clicked();

    void on_SetPoint1Button_clicked();

    void on_MoveP0Button_clicked();

    void on_PauseButton_toggled(bool checked);

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    SettingsWindow *m_settingsWindow;
    JOGWindow *m_jogWindow;

    QModbusReply *lastRequest;
    QModbusClient *modbusDevice;

    QVector<Servoline> servo_array;

    QTimer *timerUpdatePos;
    QTimer *timerIsHome;

    TcpClient* p_TcpClient;


};
#endif // MAINWINDOW_H
