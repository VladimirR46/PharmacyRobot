#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModbusDataUnit>
#include "servoline.h"
#include "tcpclient.h"
#include <QTimer>
#include <QTime>
#include "database.h"

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

    enum State
    {
       NOT,
       EMPTY,
       FIND,
       MOVE,
       GATHER,
       FIND_CASHBOX,
       DROP
    };

    struct SStateManager
    {
    public:

        State GetOldState()
        {
            return OldState;
        }

        State GetState()
        {
            return CurrentState;
        }

        void SetState(State state)
        {
            OldState = CurrentState;
            CurrentState = state;
        }
    private:
        State CurrentState = State::EMPTY;
        State OldState = State::NOT;
    };

    SStateManager StateManager;
    DataBase::Cell CurrentCell;

    int Resolution_dP13 = 2; // Переменная не позволяет два раза отправлять запрос


private:
    void initActions();
    int GetEncoderFeedback(const QModbusDataUnit& unit);

    void WriteModbus(int Serves, QModbusDataUnit writeUnit);
    void SetBit(quint16 &value, quint8 bit, quint8 index);
    void ProcessPA508(int ServoAddres, quint16 value);
    void ProcessPA509(int ServoAddres, quint16 value);

    void WritePoint(int X, int Y);
    void MovePoint(int X, int Y);
    void Check_dP13(int ServoAddres, quint16 value);

    QVector<Servoline>::iterator FindServo(int ServoAddres); // Поиск обьекта сервы по адресу в массиве


private slots:

    void TaskTimerSlot();
    void RunTaskSlot();

    void JOGSlot(int id, int state);

    void SendRead_dP13();

    void  WriteModbusRequest(int Server, int RegistrAddres, quint16 value);
    void  WriteIntModbusRequest(int Server, int RegistrAddres, qint32 value);
    void  ReadModbusRequest(int Server, int RegistrAddres, quint16 size); // Прочитать N регистров

    void onStateChanged(int state);
    void readReady();
    void ConnectToPort();
    void ConnectToTCP();
    void Power();
    void SHome();

    void ServoInitialization();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    TcpClient* p_TcpClient;

    SettingsWindow *m_settingsWindow;
    JOGWindow *m_jogWindow;



    QModbusReply *lastRequest;
    QModbusClient *modbusDevice;

    QVector<Servoline> servo_array;

    QTimer *timerIsHome;
    QTimer *TaskTimer;

};
#endif // MAINWINDOW_H
