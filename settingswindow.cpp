#include "settingswindow.h"
#include "ui_settingswindow.h"

#include "QHeaderView"
#include<QMessageBox>



SettingsWindow::SettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    m_cellSettingsWindow =  new CellSettingsWindow();

    connect(m_cellSettingsWindow, &CellSettingsWindow::SaveCellConfig, this, &SettingsWindow::SaveCellConfigSlot);

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

        m_settings.TcpIP = ui->IPEdit->text();
        m_settings.TcpPort = ui->PortSpinner->value();

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
    delete m_cellSettingsWindow;
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
//-----------------------------------------------------------------------------------------------------------
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
void SettingsWindow::SaveCellConfigSlot(QJsonObject& obj, int box_, int line_, int cell_)
{
    // Добавить в базу данных
    QJsonArray boxes = db["modules"].toArray(); // Массив шкафов
    QJsonArray Lines = boxes[box_].toArray();
    QJsonArray Cells = Lines[line_].toArray();
    Cells[cell_] = obj;

    // Сохраняем
    Lines[line_] = Cells;
    boxes[box_] = Lines;
    db["modules"] = boxes;

    //цвет
    QTableWidget* tw = static_cast<QTableWidget *>(tableCupboard[box_]->cellWidget(line_,1));
    if(tw && obj["Count"].toInt() > 0)
    {
        tw->item(0,cell_)->setTextAlignment(Qt::AlignCenter);
        tw->item(0,cell_)->setText(QString::number(obj["Count"].toInt()));
        tw->item(0,cell_)->setBackground(Qt::green);
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
        emit ReadModbusSignal(1, Addres,1);
        emit ReadModbusSignal(2, Addres,1);
    }  
}
//--------------------------------------------------------------------------------------------------
void SettingsWindow::Click(int row,int col)
{
    int index = ui->tabCupboard->currentIndex();
    int rows = tableCupboard[index]->currentRow();
    //QMessageBox::information(0, "Information", QString::number(tableCupboard[index]->currentRow()) + " " + QString::number(col));

    // Добавить в базу данных
    m_cellSettingsWindow->ShowSettings(db, index, tableCupboard[index]->currentRow(), col);

}
//--------------------------------------------------------------------------------------------------
void SettingsWindow::LoadDBFromFile()
{
    // Выбираем файл
    QString openFileName = QFileDialog::getOpenFileName(this,
                                                        tr("Open Json File"),
                                                        QString(),
                                                        tr("JSON (*.json)"));
    QFileInfo fileInfo(openFileName);   // С помощью QFileInfo
    QDir::setCurrent(fileInfo.path());  // установим текущую рабочую директорию, где будет файл
    // Создаём объект файла и открываем его на чтение
    QFile jsonFile(openFileName);
    if (!jsonFile.open(QIODevice::ReadOnly))
    {
        return;
    }

    // Считываем весь файл
    QByteArray saveData = jsonFile.readAll();
    // Создаём QJsonDocument
    QJsonDocument jsonDocument(QJsonDocument::fromJson(saveData));
    // Из которого выделяем объект в текущий рабочий QJsonObject
    db = jsonDocument.object();
}
//--------------------------------------------------------------------------------------------------
void SettingsWindow::SaveDBFromFile()
{
    // С помощью диалогового окна получаем имя файла с абсолютным путём
    QString saveFileName = QFileDialog::getSaveFileName(this,
                                                        tr("Save Json File"),
                                                        QString(),
                                                        tr("JSON (*.json)"));
    QFileInfo fileInfo(saveFileName);   // С помощью QFileInfo
    QDir::setCurrent(fileInfo.path());  // установим текущую рабочую директорию, где будет файл, иначе может не заработать
    // Создаём объект файла и открываем его на запись
    QFile jsonFile(saveFileName);
    if (!jsonFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    // Записываем текущий объект Json в файл
    jsonFile.write(QJsonDocument(db).toJson(QJsonDocument::Indented));
    jsonFile.close();   // Закрываем файл
}
//--------------------------------------------------------------------------------------------------
void SettingsWindow::ClickCupboard(int row,int col)
{
    // Кнопка добавить
    if(col == 0)
    {
        int index = ui->tabCupboard->currentIndex();
        //QMessageBox::information(0, "Information", QString::number(row) + " " + QString::number(col));
        QTableWidget* tw = static_cast<QTableWidget *>(tableCupboard[index]->cellWidget(row,1));
        if(!tw)
        {
            tw = new QTableWidget();
            tw->setRowCount(1);
            //tw->horizontalHeader()->hide();
            tw->verticalHeader()->hide();
            connect(tw, SIGNAL(cellClicked(int,int)), this, SLOT(Click(int,int)));
        }
        tw->setSelectionMode(QAbstractItemView::NoSelection);
        tw->setEditTriggers(0);
        tw->setColumnCount(tw->columnCount()+1);
        //tw->setRowHeight(0,50);

        //цвет
        tw->setItem(0,tw->columnCount()-1, new QTableWidgetItem);
        tw->item(0,tw->columnCount()-1)->setBackground(Qt::blue);

        tw->setColumnWidth(tw->columnCount()-1,20);

        // Resize
        int iWidth = 0;
        for (int i=0; i<tw->columnCount(); i++)
        {
            iWidth += tw->horizontalHeader()->sectionSize(i);
        }
        iWidth += tw->verticalHeader()->width();
        tw->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        if(tableCupboard[index]->columnWidth(1) < iWidth )
            tableCupboard[index]->setColumnWidth(1, iWidth);

        tableCupboard[index]->setCellWidget(row,1,tw);


        // Добавить в базу данных
        QJsonArray modules = db["modules"].toArray();
        QJsonArray Line = modules[index].toArray();

        QJsonArray Cells = Line[row].toArray();

        QJsonObject obj;
        obj["Name"] = "Цитрамон";
        obj["ProductCode"] = 0;
        obj["Count"] = 0;
        obj["X"] = 0;
        obj["Y"] = 0;
        Cells.append(obj);

        // Сохраняем
        Line[row] = Cells;
        modules[index] = Line;
        db["modules"] = modules;

    }
}
void SettingsWindow::LoadDatabase()
{
    ui->tabCupboard->clear();
    for(int i = 0; i < db["modules"].toArray().count(); i++)
    {
        tableCupboard[i] = new QTableWidget();
        tableCupboard[i]->setEditTriggers(0);

        tableCupboard[i]->setStyleSheet( // "background-color: #2F2F2F;"
                                       "border: 1px solid #4181C0;"
                                       "color: #4181C0;"
                                       "selection-background-color: #4181C0;"
                                       "selection-color: #FFF;"

                                       "QHeaderView::section {"
                                       "border-top: 0px solid 4181C0;"
                                       "border-bottom: 1px solid 4181C0;"
                                       "border-right: 1px solid 4181C0;"
                                       "background:#2F2F2F;"
                                       "color: #4181C0;"
                                       "}");

        connect(tableCupboard[i], SIGNAL(cellClicked(int,int)), this, SLOT(ClickCupboard(int,int)));


        tableCupboard[i]->setRowCount(db["modules"].toArray()[i].toArray().count());
        tableCupboard[i]->setColumnCount(2);
        tableCupboard[i]->setHorizontalHeaderLabels(QStringList() << "" << "Ячейки");

        for(int j = 0; j < db["modules"].toArray()[i].toArray().count(); j++)
        {
            QTableWidgetItem *item = new QTableWidgetItem("Добавить");
            tableCupboard[i]->setRowHeight(j,57);
            tableCupboard[i]->setItem(j,0,item);

            // Есть ли ячейки в строке
            if(db["modules"].toArray()[i].toArray()[j].toArray().count() > 0)
            {
                QTableWidget* tw = new QTableWidget();
                tw->setRowCount(1);
                tw->verticalHeader()->hide();
                connect(tw, SIGNAL(cellClicked(int,int)), this, SLOT(Click(int,int)));
                // Загружаем ячейки
                for(int k = 0; k < db["modules"].toArray()[i].toArray()[j].toArray().count(); k++)
                {
                    tw->setColumnCount(k+1);
                    tw->setColumnWidth(k,20);

                    //цвет
                    tw->setItem(0,k, new QTableWidgetItem);
                    int count = db["modules"].toArray()[i].toArray()[j].toArray()[k].toObject()["Count"].toInt();
                    if(count > 0)
                    {
                        tw->item(0,k)->setTextAlignment(Qt::AlignCenter);
                        tw->item(0,k)->setText(QString::number(count));
                        tw->item(0,k)->setBackground(Qt::green);
                    }
                    else
                    {
                       tw->item(0,k)->setBackground(Qt::blue);
                    }


                    // Resize
                    int iWidth = 0;
                    for (int i=0; i<tw->columnCount(); i++)
                    {
                        iWidth += tw->horizontalHeader()->sectionSize(i);
                    }
                    iWidth += tw->verticalHeader()->width();
                    tw->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    if(tableCupboard[i]->columnWidth(1) < iWidth )
                        tableCupboard[i]->setColumnWidth(1, iWidth);

                }
                tableCupboard[i]->setCellWidget(j,1,tw);
            }
        }

        tableCupboard[i]->setColumnWidth(0,60);
        ui->tabCupboard->addTab(tableCupboard[i],"Шкаф " + QString::number(i+1));
    }

}


void SettingsWindow::on_ButtonAddCupboard_clicked()
{
    int index = ui->tabCupboard->count();
    if(index > 4) return;

    tableCupboard[index] = new QTableWidget();
    tableCupboard[index]->setEditTriggers(0);

    tableCupboard[index]->setStyleSheet( // "background-color: #2F2F2F;"
                                   "border: 1px solid #4181C0;"
                                   "color: #4181C0;"
                                   "selection-background-color: #4181C0;"
                                   "selection-color: #FFF;"

                                   "QHeaderView::section {"
                                   "border-top: 0px solid 4181C0;"
                                   "border-bottom: 1px solid 4181C0;"
                                   "border-right: 1px solid 4181C0;"
                                   "background:#2F2F2F;"
                                   "color: #4181C0;"
                                   "}");

    connect(tableCupboard[index], SIGNAL(cellClicked(int,int)), this, SLOT(ClickCupboard(int,int)));


    tableCupboard[index]->setRowCount(ui->spinCupboard->value());
    tableCupboard[index]->setColumnCount(2);
    tableCupboard[index]->setHorizontalHeaderLabels(QStringList() << "" << "Ячейки");

    for(int i = 0; i < ui->spinCupboard->value(); i++)
    {
        QTableWidgetItem *item = new QTableWidgetItem("Добавить");
        tableCupboard[index]->setRowHeight(i,57);
        tableCupboard[index]->setItem(i,0,item);
    }

    tableCupboard[index]->setColumnWidth(0,60);
    ui->tabCupboard->addTab(tableCupboard[index],"Шкаф " + QString::number(index+1));

    // Добавляем шкаф в базу данных
    QJsonArray modules = db["modules"].toArray();

    QJsonArray Lines;
    for(int i = 0; i < ui->spinCupboard->value(); i++ )
    {
       QJsonArray Line;
       Lines.append(Line);
    }
    //message["Module"+QString::number(k)] = Lines;
    modules.append(Lines);


    db["modules"] = modules;

}

void SettingsWindow::on_pushButton_clicked()
{
    LoadDBFromFile();
    LoadDatabase();
}

void SettingsWindow::on_pushButton_2_clicked()
{
    SaveDBFromFile();
}
