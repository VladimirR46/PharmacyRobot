#include "settingswindow.h"
#include "ui_settingswindow.h"

#include "QHeaderView"

SettingsWindow::SettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    connect(ui->SettingsList,SIGNAL(currentRowChanged(int)),ui->stackedSettings,SLOT(setCurrentIndex(int)));
    connect(ui->SettingsList,SIGNAL(currentTextChanged(QString)),ui->label_8,SLOT(setText(QString)));
    connect(ui->ButtonQuit, &QPushButton::clicked, this, &SettingsWindow::hide);


    ui->parityCombo->setCurrentIndex(2);
#if QT_CONFIG(modbus_serialport)
    ui->baudCombo->setCurrentText(QString::number(m_settings.baud));
    ui->dataBitsCombo->setCurrentText(QString::number(m_settings.dataBits));
    ui->stopBitsCombo->setCurrentText(QString::number(m_settings.stopBits));
#endif
    ui->timeoutSpinner->setValue(m_settings.responseTime);
    ui->retriesSpinner->setValue(m_settings.numberOfRetries);
    ui->portSpinner->setValue(m_settings.Port);

    connect(ui->applyButton, &QPushButton::clicked, [this]() {
#if QT_CONFIG(modbus_serialport)
        m_settings.parity = ui->parityCombo->currentIndex();
        if (m_settings.parity > 0)
            m_settings.parity++;
        m_settings.baud = ui->baudCombo->currentText().toInt();
        m_settings.dataBits = ui->dataBitsCombo->currentText().toInt();
        m_settings.stopBits = ui->stopBitsCombo->currentText().toInt();
#endif
        m_settings.responseTime = ui->timeoutSpinner->value();
        m_settings.numberOfRetries = ui->retriesSpinner->value();
        m_settings.Port = ui->portSpinner->value();

        hide();
    });



    // Указываем реистры которые хотим видеть в настройках
    RegistrEditList.push_back(PA771);
    RegistrEditList.push_back(PA772);
    RegistrEditList.push_back(PA773);
    RegistrEditList.push_back(PA700);
    RegistrEditList.push_back(PA775);
    RegistrEditList.push_back(PA306);
    RegistrEditList.push_back(PA701);
    RegistrEditList.push_back(PA702);
    RegistrEditList.push_back(PA508);
    RegistrEditList.push_back(PA509);



}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}
//-------------------------------------------------------------------------------------------------------------------

void SettingsWindow::DisplayRegistr(int ServoAddres, int addres, quint16 value)
{

    // Перебираем все серво классы
    QVector<Servoline>::iterator iter= servo_array->begin();
    for (int i = 0;iter!=servo_array->end();++iter,i++)
    {
        if(ServoAddres == iter->ServoAddres)
        {
            QWidget *widget = ui->tabServo->widget(i);
            QTableWidget *tableWidget = static_cast<QTableWidget *>(widget);


            for (int j= 0; j < tableWidget->rowCount(); j++)
            {
                QString check;
                QTableWidgetItem *item = tableWidget->item(j,1);
                if (item != 0)
                {
                   if(item->text() == QString::number(addres))
                   {
                       if(iter->ServoParam[addres].Type == ServoRegistersType::d)
                       {
                           QTableWidgetItem *itemValue = new QTableWidgetItem(QString::number(value));
                           tableWidget->setItem(j,2,itemValue);
                       }

                       if(iter->ServoParam[addres].Type == ServoRegistersType::h)
                       {
                           QTableWidgetItem *itemValue = new QTableWidgetItem(QString::number(value,16));
                           tableWidget->setItem(j,2,itemValue);
                       }

                       if(iter->ServoParam[addres].Type == ServoRegistersType::h4 || iter->ServoParam[addres].Type == ServoRegistersType::b)
                       {
                          QTableWidget* tw = static_cast<QTableWidget *>(tableWidget->cellWidget(j,2));

                          quint8 v[4];
                          v[0] = (value & 0xff) & 0x0f;
                          v[1] = ((value & 0xff) & 0xf0) >> 4;
                          v[2] = ((value & 0xff00) >> 8) & 0x0f;
                          v[3] = (((value & 0xff00) >> 8) & 0xf0) >> 4;

                          for(int k = 0; k < 4; k++)
                          {
                              QTableWidgetItem *itemValue = new QTableWidgetItem(QString::number(v[k],16));
                              tw->setItem(k,1,itemValue);
                          }
                       }

                   }
                }
            }

        }

    }


}



void SettingsWindow::initServo(QVector<Servoline>* servo_ptr)
{
    servo_array = servo_ptr;

    QVector<Servoline>::iterator iter= servo_ptr->begin();

    for (;iter!=servo_ptr->end();++iter)
    {
        tableWidget = new QTableWidget();

        tableWidget->setRowCount(10);
        tableWidget->setColumnCount(3);
        tableWidget->setHorizontalHeaderLabels(QStringList() << "Описание" << "Регистр" << "Значение");

        QVector<int>::iterator it = RegistrEditList.begin();
        for (int i = 0;it!=RegistrEditList.end();++it,i++)
        {
            int RegistrAddres = *it;

            QTableWidgetItem *itemDescription = new QTableWidgetItem(iter->ServoParam[*it].Description);
            QTableWidgetItem *itemRegistr = new QTableWidgetItem(QString::number(*it));
            tableWidget->setItem(i,0,itemDescription);
            tableWidget->setItem(i,1,itemRegistr);

            if(iter->ServoParam[*it].Type == ServoRegistersType::d4 || iter->ServoParam[*it].Type == ServoRegistersType::h4 || iter->ServoParam[*it].Type == ServoRegistersType::b)
            {
                QTableWidget *tableWidget2 = new QTableWidget();
                tableWidget2->setRowCount(4);
                tableWidget2->setColumnCount(2);
                tableWidget2->horizontalHeader()->hide();
                tableWidget2->verticalHeader()->hide();


                for(int j = 0; j < 4; j++)
                {
                    QTableWidgetItem *itemDescription2 = new QTableWidgetItem(QString::number(*it)+"."+QString::number(j));
                    tableWidget2->setItem(j,0,itemDescription2);
                }

                tableWidget->setColumnWidth(2,204);
                tableWidget->setRowHeight(i,123);

                tableWidget->setCellWidget(i,2,tableWidget2);
            }

        }

        ui->tabServo->addTab(tableWidget, iter->GetName());


    }
}
//-------------------------------------------------------------------------------------------------------------------
SettingsWindow::Settings SettingsWindow::settings() const
{
    return m_settings;
}

void SettingsWindow::on_ButtonReadAll_clicked()
{   
    QVector<int>::iterator it = RegistrEditList.begin();
    for (int i = 0;it!=RegistrEditList.end();++it,i++)
    {
        int Addres = *it;
        emit ReadModbusSignal(Addres,1);

    }  
}
