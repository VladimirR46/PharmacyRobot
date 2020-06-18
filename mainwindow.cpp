#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingswindow.h"
#include "jogwindow.h"

#include <QModbusRtuSerialMaster>
#include <QVector>

#include "ServoRegisters.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , lastRequest(nullptr)
    , modbusDevice(nullptr)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(":/images/robot.png"));

    // Создаем окно настроек
    m_settingsWindow = new SettingsWindow(this);

    connect(m_settingsWindow, &SettingsWindow::ReadModbusSignal, this, &MainWindow::ReadModbusRequest);
    connect(m_settingsWindow, &SettingsWindow::WriteModbusSignal, this, &MainWindow::WriteModbusRequest);

    m_jogWindow = new JOGWindow(this);
    connect(m_jogWindow, SIGNAL(JOGSignal(int,int)), this, SLOT(JOGSlot(int,int)));

    initActions();

    modbusDevice = new QModbusRtuSerialMaster(this);

    connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
        statusBar()->showMessage(modbusDevice->errorString(), 5000);
    });



    if (!modbusDevice)
            statusBar()->showMessage(tr("Could not create Modbus master."), 5000);
    else
        connect(modbusDevice, &QModbusClient::stateChanged, this, &MainWindow::onStateChanged);


    ServoInitialization();

    timerUpdatePos = new QTimer();
    connect(timerUpdatePos, SIGNAL(timeout()), this, SLOT(on_UpdateCurPosButton_clicked()));

    timerIsHome = new QTimer();
    connect(timerIsHome, SIGNAL(timeout()), this, SLOT(Update_dP13()));


    p_TcpClient = new TcpClient(); // Создаем TCP client

}
//-------------------------------------------------------------------------------------------------------------
void MainWindow::ServoInitialization()
{
    Servoline servoX("Ось X",1);
    servo_array.push_back(servoX);

    Servoline servoY("Ось Y",2);
    servo_array.push_back(servoY);


    m_settingsWindow->initServo(&servo_array);
}

// Реализация слота
void MainWindow::JOGSlot(int id, int state)
{
    /*
    if(id == 0) WriteModbusRequest(2304,quint16(state));
    if(id == 1) WriteModbusRequest(2305,quint16(state));
    if(id == 2) WriteModbusRequest(2306,quint16(state));
    if(id == 3) ReadModbusRequest(PA306,1);
    */
    statusBar()->showMessage(tr("JOG"), 2000);
}

void MainWindow::Update_dP13()
{
    QVector<Servoline>::iterator iter= servo_array.begin();
    for (;iter!=servo_array.end();++iter)
    {
        ReadModbusRequest(iter->ServoAddres, dP13,1);
    }
}
// Слот состояния подключения
void MainWindow::onStateChanged(int state)
{
    bool connected = (state != QModbusDevice::UnconnectedState);
    ui->actionConnect->setEnabled(!connected);
    ui->actionDisconnect->setEnabled(connected);
}
//------------------------------------------------------------------------------------------------------------

MainWindow::~MainWindow()
{
    if (modbusDevice)
        modbusDevice->disconnectDevice();
    delete modbusDevice;

    delete p_TcpClient;

    delete ui;
}
//------------------------------------------------------------------------------------------------------------
void MainWindow::ConnectToTCP()
{
    p_TcpClient->Connect(m_settingsWindow->settings().TcpIP, m_settingsWindow->settings().TcpPort);
}
//------------------------------------------------------------------------------------------------------------
void MainWindow::ConnectToPort()
{
    if (!modbusDevice)
        return;

    statusBar()->clearMessage();
    if (modbusDevice->state() != QModbusDevice::ConnectedState)
    {

        modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter,tr("COM%1").arg(m_settingsWindow->settings().Port));
#if QT_CONFIG(modbus_serialport)
        modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, m_settingsWindow->settings().parity);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, m_settingsWindow->settings().baud);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, m_settingsWindow->settings().dataBits);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, m_settingsWindow->settings().stopBits);
#endif

        modbusDevice->setTimeout(m_settingsWindow->settings().responseTime);
        modbusDevice->setNumberOfRetries(m_settingsWindow->settings().numberOfRetries);
        if (!modbusDevice->connectDevice())
        {
            statusBar()->showMessage(tr("Ошибка подключения: ") + modbusDevice->errorString(), 5000);
        } else {
            statusBar()->showMessage(tr("Успешно подключилось"), 5000);
            ui->actionConnect->setEnabled(false);
            ui->actionDisconnect->setEnabled(true);
        }
    } else {
        modbusDevice->disconnectDevice();
        ui->actionConnect->setEnabled(true);
        ui->actionDisconnect->setEnabled(false);
    }
}
//------------------------------------------------------------------------------------------------------------

void MainWindow::ReadModbusRequest(int Server, int RegistrAddres, quint16 size)
{
    if(Server == 1 && !ui->checkAxisX->isChecked()) return;
    if(Server == 2 && !ui->checkAxisY->isChecked()) return;

    if (!modbusDevice)
        return;

    statusBar()->clearMessage();

    QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters, RegistrAddres, size);

    if (auto *reply = modbusDevice->sendReadRequest(readUnit, Server))
    {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &MainWindow::readReady);
        else
            delete reply; // broadcast replies return immediately
    } else
    {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
}
//---------------------------------------------------------------------------------------------------------------

void MainWindow::initActions()
{
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);

    connect(ui->actionConnect, &QAction::triggered,
            this, &MainWindow::ConnectToPort);
    connect(ui->actionDisconnect, &QAction::triggered,
            this, &MainWindow::ConnectToPort);


    connect(ui->actionConnectTCP, &QAction::triggered,
            this, &MainWindow::ConnectToTCP);

    connect(ui->actionExit, &QAction::triggered, this, &QMainWindow::close);
    connect(ui->actionSettings, &QAction::triggered, m_settingsWindow, &QDialog::show);
    connect(ui->actionJOG, &QAction::triggered, m_jogWindow, &QDialog::show);
}
//---------------------------------------------------------------------------------------------------------------------
int MainWindow::GetEncoderFeedback(const QModbusDataUnit& unit)
{
    int value = qint16(unit.value(1)) * 10000 + qint16(unit.value(0));
    return value;
}
//----------------------------------------------------------------------------------------------------------------------
int uint16_To_int32(quint16 v1, quint16 v2)
{
    return ((v1<<16) | ((v2) & 0xffff));
}
//----------------------------------------------------------------------------------------------------------------------
void GetBit(quint8 DI[], quint16 value)
{
    DI[0] = (value & 0xff) & 0x0f;
    DI[1] = ((value & 0xff) & 0xf0) >> 4;
    DI[2] = ((value & 0xff00) >> 8) & 0x0f;
    DI[3] = (((value & 0xff00) >> 8) & 0xf0) >> 4;
}

//-----------------------------------------------------------------------------------------------------------------------
QVector<Servoline>::iterator MainWindow::FindServo(int ServoAddres)
{
    QVector<Servoline>::iterator iter= servo_array.begin();
    for (;iter!=servo_array.end();++iter)
    {
        if(iter->ServoAddres == ServoAddres) return iter;
    }

    return iter;
}
//---------------------------------------------------------------------------------
void MainWindow::ProcessPA508(int ServoAddres, quint16 value)
{
    quint8 DI[4];
    GetBit(DI,value);

    FindServo(ServoAddres)->isPower = bool(DI[0]);

    if((FindServo(1)->isPower || !ui->checkAxisX->isChecked()) &&
            (FindServo(2)->isPower || !ui->checkAxisY->isChecked())) // Питание включено
    {
        ui->PowerButton->setText("Отключить");
        ui->PowerButton->setIcon(QIcon(":/images/powerOff.png"));
    }
    else
    {
        ui->PowerButton->setText("Включить");
        ui->PowerButton->setIcon(QIcon(":/images/power.png"));
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void MainWindow::ProcessPA509(int ServoAddres, quint16 value)
{

}
//----------------------------------------------------------------------
void MainWindow::isHOME(int ServoAddres, quint16 value)
{
    if(value == 0xb) FindServo(ServoAddres)->isHome = true;
    else FindServo(ServoAddres)->isHome = false;

    if((FindServo(1)->isHome || !ui->checkAxisX->isChecked()) &&
            (FindServo(2)->isHome || !ui->checkAxisY->isChecked()))
    {
        ui->SHomeButton->setEnabled(true);
        timerIsHome->stop();
    }
}
//----------------------------------------------------------------------
void MainWindow::readReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError)
    {
        const QModbusDataUnit unit = reply->result();

        if(unit.valueCount() == 1)
            m_settingsWindow->DisplayRegistr(reply->serverAddress(),unit.startAddress(),unit.value(0));


        switch (unit.startAddress())
        {
        case PA306:
          m_jogWindow->SetJOGSpeed(unit.value(0));
          break;
        case dP01:
          ui->CurrPosEdit->setText(QString::number(GetEncoderFeedback(unit)));
          break;
        case PA701:
            ui->Point0Edit->setText(QString::number(uint16_To_int32(unit.value(1),unit.value(0)),10));
            break;
        case PA703:
            ui->Point1Edit->setText(QString::number(uint16_To_int32(unit.value(1),unit.value(0)),10));
            break;
        case PA508:
            ProcessPA508(reply->serverAddress(),unit.value(0));
            break;
        case dP13:
            isHOME(reply->serverAddress(),unit.value(0));
            break;
        default:
          //cout<<"Error, bad input, quitting\n";
          break;
        }

        /*
        for (uint i = 0; i < unit.valueCount(); i++)
        {
            const QString entry = tr("Address: %1, Value: %2").arg(unit.startAddress() + i)
                                     .arg(QString::number(unit.value(i),10));
            statusBar()->showMessage(entry, 5000);
            ui->listWidget->addItem(entry);
        }
        */

    } else if (reply->error() == QModbusDevice::ProtocolError)
        statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16), 5000);
     else
        statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").arg(reply->errorString()).arg(reply->error(), -1, 16), 5000);

    reply->deleteLater();

}
//--------------------------------------------------------------------------------------------------------------
void  MainWindow::WriteIntModbusRequest(int Server, int RegistrAddres, qint32 value)
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    const auto table = static_cast<QModbusDataUnit::RegisterType> (4);

    QModbusDataUnit writeUnit(table,RegistrAddres,2);

    quint16 loWord = value & 0x0000FFFF;
    quint16 hiWord = static_cast<quint16>(value >> 16);

    writeUnit.setValue(0,loWord);
    writeUnit.setValue(1,hiWord);

    WriteModbus(Server, writeUnit);
}
//---------------------------------------------------------------------------------------------------------------
void  MainWindow::WriteModbusRequest(int Server, int RegistrAddres, quint16 value)
{
    if(Server == 1 && !ui->checkAxisX->isChecked()) return;
    if(Server == 2 && !ui->checkAxisY->isChecked()) return;

    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    const auto table = static_cast<QModbusDataUnit::RegisterType> (4);

    QModbusDataUnit writeUnit(table,RegistrAddres,1);

    writeUnit.setValue(0,value);

    WriteModbus(Server, writeUnit);
}
//--------------------------------------------------------------------------------------------------------------
void  MainWindow::WriteModbus(int Server, QModbusDataUnit writeUnit)
{

    if (auto *reply = modbusDevice->sendWriteRequest(writeUnit, Server))
    {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [this, reply]() {
                if (reply->error() == QModbusDevice::ProtocolError)
                {
                    statusBar()->showMessage(tr("Write response error: %1 (Mobus exception: 0x%2)")
                        .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16),
                        5000);
                } else if (reply->error() != QModbusDevice::NoError)
                {
                    statusBar()->showMessage(tr("Write response error: %1 (code: 0x%2)").
                        arg(reply->errorString()).arg(reply->error(), -1, 16), 5000);
                }
                reply->deleteLater();
            });
        } else {
            // broadcast replies return immediately
            reply->deleteLater();
        }
    } else {
        statusBar()->showMessage(tr("Write error: ") + modbusDevice->errorString(), 5000);
    }
}
//----------------------------------------------------------------------------------------------------------------

void MainWindow::on_UpdateCurPosButton_clicked()
{
    ReadModbusRequest(1, dP01,2);
}

void MainWindow::on_checkUpdatePos_stateChanged(int arg1)
{
    if(!timerUpdatePos->isActive())
        timerUpdatePos->start(100); // И запустим таймер
    else
        timerUpdatePos->stop();
}

void MainWindow::SetBit(quint16 &value, quint8 bit, quint8 index)
{
    if (bit == 1)
        value |=1<<index*4;
    else
        value &=~(1<<index*4);
}

void MainWindow::on_ReadPointButton_clicked()
{
    ReadModbusRequest(1, PA701,2); // Точк =  0
    ReadModbusRequest(1, PA703,2); // Точка =  1
}

void MainWindow::on_PowerButton_clicked()
{
    quint16 pwr = 0x0000;

    if(ui->PowerButton->text() == "Включить")
    {
        SetBit(pwr,1,0);
        QVector<Servoline>::iterator iter= servo_array.begin();
        for (;iter!=servo_array.end();++iter)
        {
            WriteModbusRequest(iter->ServoAddres, PA508,pwr);
            ReadModbusRequest(iter->ServoAddres, PA508,1);
        }
    }
    else
    {
        SetBit(pwr,0,0);
        QVector<Servoline>::iterator iter= servo_array.begin();
        for (;iter!=servo_array.end();++iter)
        {
            WriteModbusRequest(iter->ServoAddres, PA508,pwr);
            ReadModbusRequest(iter->ServoAddres, PA508,1);
        }
    }
}

void MainWindow::on_SHomeButton_clicked()
{
    quint16 init = 0x0001; //!!!

    QVector<Servoline>::iterator iter= servo_array.begin();
    for (;iter!=servo_array.end();++iter)
    {
        SetBit(init,1,1);
        WriteModbusRequest(iter->ServoAddres, PA508,init);
        SetBit(init,0,1);
        WriteModbusRequest(iter->ServoAddres, PA508,init);
    }


    ui->SHomeButton->setEnabled(false);
    timerIsHome->start(200);
}

void MainWindow::on_SetPoint0Button_clicked()
{
     int point = ui->Point0Edit->text().toInt();
     WriteIntModbusRequest(1, PA701,point);
}

void MainWindow::on_SetPoint1Button_clicked()
{
    int point = ui->Point1Edit->text().toInt();
    WriteIntModbusRequest(1, PA703,point);
}

void MainWindow::on_MoveP0Button_clicked()
{
    quint16 init = 0x0001; // !!!!  0x0001
    SetBit(init,1,2);
    WriteModbusRequest(1, PA508,init);
    SetBit(init,0,2);
    WriteModbusRequest(1, PA508,init);
}

void MainWindow::on_PauseButton_toggled(bool checked)
{
    quint16 init = 0x0000; // !!!!  0x0001
    if(checked)
    {
        SetBit(init,1,3);
        //WriteModbusRequest(PA508,init);
    }
    else
    {
        SetBit(init,0,3);
        //WriteModbusRequest(PA508,init);
    }
}
