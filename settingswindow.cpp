#include "settingswindow.h"
#include "ui_settingswindow.h"

#include "QHeaderView"
#include<QMessageBox>
#include <QElapsedTimer>
#include <QDebug>



SettingsWindow::SettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    p_settings = new QSettings();
    m_cellSettingsWindow =  new CellSettingsWindow();

    m_RestockWindow = new RestockingWindow();
    connect(m_RestockWindow, &RestockingWindow::RestockCellSignal, this, &SettingsWindow::RestockedCell);

    connect(m_cellSettingsWindow, &CellSettingsWindow::SaveCellConfig, this, &SettingsWindow::SaveCellConfigSlot);

    connect(ui->SettingsList,SIGNAL(currentRowChanged(int)),ui->stackedSettings,SLOT(setCurrentIndex(int)));
    connect(ui->SettingsList,SIGNAL(currentTextChanged(QString)),ui->label_8,SLOT(setText(QString)));
    connect(ui->ButtonQuit, &QPushButton::clicked, this, &SettingsWindow::hide);


    connect(ui->applyButton, &QPushButton::clicked, [this]()
    {
        saveSettings();
        hide();
    });

    loadSettings();

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

bool SettingsWindow::FindProduct(DataBase::Cell& cell, int productCode)
{
    QElapsedTimer timer;
    timer.start();

    if(database.GetCell(cell,productCode))
    {
        qDebug() << "FindProduct operation took" << timer.elapsed() << "milliseconds";
        return true;
    }
    return false;
}
//--------------------------------------------------------------------------------------
void SettingsWindow::DecreaseProductCount(DataBase::Cell& cell)
{
    if(cell.Count > 0)
    {
        cell.Count--;
        database.SetCellProductCount(cell.Count, cell.ProductCode);

        // Добавляем в список проданных товаров
        database.inserIntoBuyedTable(cell);
    }
}

void SettingsWindow::LoadBuyedList(QTableWidget *tableBuyed)
{
    p_tableBuyed = tableBuyed; // Сохраняем указатель

    tableBuyed->setEditTriggers(0);
    tableBuyed->setSelectionBehavior(QAbstractItemView::SelectRows);
   /*
    tableBuyed->setStyleSheet( // "background-color: #2F2F2F;"
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
                                   */

    connect(tableBuyed, SIGNAL(cellClicked(int,int)), this, SLOT(ClickBuyed(int,int)));

    int row = database.buyed_model->rowCount();

    tableBuyed->setRowCount(row);
    tableBuyed->setColumnCount(7);
    tableBuyed->setHorizontalHeaderLabels(QStringList() << "Шкаф №" << "Строка №" << "Ячейка №" << "Код товара" << "Название" << "Кол-во" << "Продано");

    for(int i = 0; i < row; i++)
    {
        QSqlRecord record = database.buyed_model->record(i);
        int productcode = record.value("productcode").toInt();
        int buyed_count = record.value("count").toInt();

        DataBase::Cell cell;
        database.GetCell(cell, productcode);

        tableBuyed->setItem(i,0,new QTableWidgetItem(QString::number(cell.Box+1)));
        tableBuyed->setItem(i,1,new QTableWidgetItem(QString::number(cell.Line+1)));
        tableBuyed->setItem(i,2,new QTableWidgetItem(QString::number(cell.Cell+1)));
        tableBuyed->setItem(i,3,new QTableWidgetItem(QString::number(cell.ProductCode)));
        tableBuyed->setItem(i,4,new QTableWidgetItem(cell.Name));
        tableBuyed->setItem(i,5,new QTableWidgetItem(QString::number(cell.Count)));
        tableBuyed->setItem(i,6,new QTableWidgetItem(QString::number(buyed_count)));
    }
}

void SettingsWindow::UpdateBuyedList()
{
    database.buyed_model->setFilter("");
    database.buyed_model->select();
    int row = database.buyed_model->rowCount();
    p_tableBuyed->setRowCount(row);
    for(int i = 0; i < row; i++)
    {
        QSqlRecord record = database.buyed_model->record(i);
        int productcode = record.value("productcode").toInt();
        int buyed_count = record.value("count").toInt();

        DataBase::Cell cell;
        database.GetCell(cell, productcode);

        QTableWidgetItem *item = p_tableBuyed->item(i,0);
        if (!item)
        {
            p_tableBuyed->setItem(i,0,new QTableWidgetItem(QString::number(cell.Box+1)));
            p_tableBuyed->setItem(i,1,new QTableWidgetItem(QString::number(cell.Line+1)));
            p_tableBuyed->setItem(i,2,new QTableWidgetItem(QString::number(cell.Cell+1)));
            p_tableBuyed->setItem(i,3,new QTableWidgetItem(QString::number(cell.ProductCode)));
            p_tableBuyed->setItem(i,4,new QTableWidgetItem(cell.Name));
            p_tableBuyed->setItem(i,5,new QTableWidgetItem(QString::number(cell.Count)));
            p_tableBuyed->setItem(i,6,new QTableWidgetItem(QString::number(buyed_count)));
        }
        else
        {
            p_tableBuyed->item(i,0)->setText(QString::number(cell.Box+1));
            p_tableBuyed->item(i,1)->setText(QString::number(cell.Line+1));
            p_tableBuyed->item(i,2)->setText(QString::number(cell.Cell+1));
            p_tableBuyed->item(i,3)->setText(QString::number(cell.ProductCode));
            p_tableBuyed->item(i,4)->setText(cell.Name);
            p_tableBuyed->item(i,5)->setText(QString::number(cell.Count));
            p_tableBuyed->item(i,6)->setText(QString::number(buyed_count));
        }
    }
}

SettingsWindow::~SettingsWindow()
{
    delete p_settings;
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
void SettingsWindow::SaveCellConfigSlot(DataBase::Cell& cell)
{
    database.SetCell(cell);
    //цвет
    QTableWidget* tw = static_cast<QTableWidget *>(tableCupboard[cell.Box]->cellWidget(cell.Line,1));
    if(tw && cell.Count > 0)
    {
        tw->item(0,cell.Cell)->setTextAlignment(Qt::AlignCenter);
        tw->item(0,cell.Cell)->setText(QString::number(cell.Count));
        tw->item(0,cell.Cell)->setBackground(Qt::green);
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
    m_cellSettingsWindow->ShowSettings(database, index, tableCupboard[index]->currentRow(), col);

}
//-------------------------------------------------------------------------------------------------
void SettingsWindow::SaveSettingsToStruct()
{
    int parity = ui->parityCombo->currentIndex();
    if (parity > 0) parity++;
    m_settings.parity = parity;
    m_settings.baud = ui->baudCombo->currentText().toInt();
    m_settings.dataBits = ui->dataBitsCombo->currentText().toInt();
    m_settings.stopBits = ui->stopBitsCombo->currentText().toInt();
    m_settings.responseTime = ui->timeoutSpinner->value();
    m_settings.numberOfRetries = ui->retriesSpinner->value();
    m_settings.Port = ui->portSpinner->value();

    m_settings.TcpIP = ui->IPEdit->text();
    m_settings.TcpPort = ui->PortSpinner->value();

    m_settings.cashbox[0].X = ui->spinBox1X->value();
    m_settings.cashbox[0].Y = ui->spinBox1Y->value();
    m_settings.cashbox[1].X = ui->spinBox2X->value();
    m_settings.cashbox[1].Y = ui->spinBox2Y->value();

    m_settings.OpenCartAngle = ui->spinOpenAngle->value();
    m_settings.CloseCartAngle = ui->spinCloseAngle->value();
    m_settings.UpCartAngle = ui->spinUpAngle->value();
    m_settings.DownCartAngle = ui->spinDownAngle->value();

    m_settings.CartPort = ui->spinCartPort->value();
}

void SettingsWindow::RestockedCell(int row, int count)
{
    QSqlRecord record = database.buyed_model->record(row);
    int productcode = record.value("productcode").toInt();
    int newCount = record.value("count").toInt() - count;

    int PCount = p_tableBuyed->item(row,5)->text().toInt() + count;
    p_tableBuyed->item(row,5)->setText(QString::number(PCount));
    database.SetCellProductCount(PCount,productcode);

    if(newCount == 0)
    {
        database.buyed_model->removeRows(row, 1);
        p_tableBuyed->removeRow(row);
    }
    else
    {
        record.setValue("count", newCount);
        database.buyed_model->setRecord(row, record);
        p_tableBuyed->item(row, 6)->setText(QString::number(newCount));
    }
    database.buyed_model->submitAll();
}
//-------------------------------------------------------------------------------------------------
void SettingsWindow::saveSettings()
{
    // Сохраняем параметры COM подключения для Modbus
    int parity = ui->parityCombo->currentIndex();
    if (parity > 0) parity++;
    p_settings->setValue("parity",parity);
    p_settings->setValue("baud", ui->baudCombo->currentText().toInt());
    p_settings->setValue("dataBits", ui->dataBitsCombo->currentText().toInt());
    p_settings->setValue("stopBits", ui->stopBitsCombo->currentText().toInt());
    p_settings->setValue("responseTime", ui->timeoutSpinner->value());
    p_settings->setValue("numberOfRetries", ui->retriesSpinner->value());
    p_settings->setValue("Port", ui->portSpinner->value());

    // Сохраняем TCP настройки
    p_settings->setValue("TcpIP",ui->IPEdit->text());
    p_settings->setValue("TcpPort",ui->PortSpinner->value());

    //Сохраняем координаты касс выдачи товара
    p_settings->setValue("Cashbox1X",ui->spinBox1X->value());
    p_settings->setValue("Cashbox1Y",ui->spinBox1Y->value());
    p_settings->setValue("Cashbox2X",ui->spinBox2X->value());
    p_settings->setValue("Cashbox2Y",ui->spinBox2Y->value());

    p_settings->setValue("OpenCartAngle",ui->spinOpenAngle->value());
    p_settings->setValue("CloseCartAngle",ui->spinCloseAngle->value());
    p_settings->setValue("UpCartAngle",ui->spinUpAngle->value());
    p_settings->setValue("DownCartAngle",ui->spinDownAngle->value());

    p_settings->setValue("CartPort",ui->spinCartPort->value());

    SaveSettingsToStruct();

    // Сохраняем базу данных
    database.SaveDataBase();
}
//-------------------------------------------------------------------------------------------------
void SettingsWindow::loadSettings()
{
    int parity = p_settings->value("parity",QSerialPort::OddParity).toInt();
    if (parity > 0) parity--;
    ui->parityCombo->setCurrentIndex(parity);
    ui->baudCombo->setCurrentText(QString::number(p_settings->value("baud",QSerialPort::Baud57600).toInt()));
    ui->dataBitsCombo->setCurrentText(QString::number(p_settings->value("dataBits",QSerialPort::Data8).toInt()));
    ui->stopBitsCombo->setCurrentText(QString::number(p_settings->value("stopBits",QSerialPort::OneStop).toInt()));
    ui->timeoutSpinner->setValue(p_settings->value("responseTime",1000).toInt());
    ui->retriesSpinner->setValue(p_settings->value("numberOfRetries",3).toInt());
    ui->portSpinner->setValue(p_settings->value("Port",10).toInt());

    ui->IPEdit->setText(p_settings->value("TcpIP","127.0.0.1").toString());
    ui->PortSpinner->setValue(p_settings->value("TcpPort",4442).toInt());

    ui->spinBox1X->setValue(p_settings->value("Cashbox1X",0).toInt());
    ui->spinBox1Y->setValue(p_settings->value("Cashbox1Y",0).toInt());
    ui->spinBox2X->setValue(p_settings->value("Cashbox2X",0).toInt());
    ui->spinBox2Y->setValue(p_settings->value("Cashbox2Y",0).toInt());

    ui->spinOpenAngle->setValue(p_settings->value("OpenCartAngle",0).toInt());
    ui->spinCloseAngle->setValue(p_settings->value("CloseCartAngle",0).toInt());
    ui->spinUpAngle->setValue(p_settings->value("UpCartAngle",0).toInt());
    ui->spinDownAngle->setValue(p_settings->value("DownCartAngle",0).toInt());

    ui->spinCartPort->setValue(p_settings->value("CartPort").toInt());

    SaveSettingsToStruct();

    LoadDatabase();

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

        QElapsedTimer timer;
        timer.start();

        DataBase::Cell cell;
        cell.Box = index;
        cell.Line = row;
        cell.Cell = tw->columnCount()-1;
        cell.ProductCode = 0;
        cell.Name = "Лекарство";
        cell.Count = 0;
        cell.X = 0;
        cell.Y = 0;
        database.inserIntoTable(cell);

        qDebug() << "The slow operation took" << timer.elapsed() << "milliseconds";

    }
}
//--------------------------------------------------------------------------------
void SettingsWindow::ClickBuyed(int row, int col)
{
    m_RestockWindow->ShowRestock(row, p_tableBuyed->item(row,6)->text().toInt());
}
//----------------------------------------------------------------------
void SettingsWindow::LoadDatabase()
{
    ui->tabCupboard->clear();

    // Перебираем все шкафы
    for(int i = 0; i < database.GetMaxBoxCount(); i++)
    {
        // Формируем выборку для шкафа
        database.model->setFilter("box="+QString::number(i));
        database.model->setSort(2, Qt::AscendingOrder);
        database.model->select();
        /*
        for (int i = 0; i < database.model->rowCount(); ++i) {
            int box_ = database.model->record(i).value("box").toInt();
            int line_ = database.model->record(i).value("line").toInt();
            int cell_ = database.model->record(i).value("cell").toInt();
            qDebug() << box_ << line_ << cell_;
        }
        */

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


        int maxline = database.GetMaxLineCount(i);
        tableCupboard[i]->setRowCount(maxline);
        tableCupboard[i]->setColumnCount(2);
        tableCupboard[i]->setHorizontalHeaderLabels(QStringList() << "" << "Ячейки");

        while (database.model->canFetchMore())
                 database.model->fetchMore();

        for (int j = 0; j < database.model->rowCount(); ++j)
        {

            int lineIndex = database.model->record(j).value("line").toInt();
            int cellIndex = database.model->record(j).value("cell").toInt();

            QTableWidgetItem *item = tableCupboard[i]->item(lineIndex,1);
            if (!item)
            {
                QTableWidgetItem *item = new QTableWidgetItem("Добавить");
                tableCupboard[i]->setRowHeight(lineIndex,57);
                tableCupboard[i]->setItem(lineIndex,0,item);
            }

            QTableWidget* tw = static_cast<QTableWidget *>(tableCupboard[i]->cellWidget(lineIndex,1));
            if(!tw)
            {
                tw = new QTableWidget();
                tw->setSelectionMode(QAbstractItemView::NoSelection);
                tw->setRowCount(1);
                tw->verticalHeader()->hide();
                connect(tw, SIGNAL(cellClicked(int,int)), this, SLOT(Click(int,int)));
            }
            tw->setColumnCount(cellIndex+1);
            tw->setColumnWidth(cellIndex,20);

            tw->setItem(0,cellIndex, new QTableWidgetItem);
            int count = database.model->record(j).value("count").toInt();
            if(count > 0)
            {
                tw->item(0,cellIndex)->setTextAlignment(Qt::AlignCenter);
                tw->item(0,cellIndex)->setText(QString::number(count));
                tw->item(0,cellIndex)->setBackground(Qt::green);
            }
            else tw->item(0,cellIndex)->setBackground(Qt::blue);

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

            tableCupboard[i]->setCellWidget(lineIndex,1,tw);
        }

        //Добавляем в пустые строки кнопку
        for(int j = 0; j < maxline; j++)
        {
            QTableWidgetItem *item = tableCupboard[i]->item(j,1);
            if (!item)
            {
                QTableWidgetItem *item = new QTableWidgetItem("Добавить");
                tableCupboard[i]->setRowHeight(j,57);
                tableCupboard[i]->setItem(j,0,item);
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
}

void SettingsWindow::on_ButtonAddCell_clicked()
{
    int index = ui->tabCupboard->currentIndex();
    int row = tableCupboard[index]->currentRow();
    for(int i = 0; i < ui->spinCellList->value(); i++)
   {
      ClickCupboard(row,0);
   }
}

void SettingsWindow::on_pushButton_clicked()
{
   if(ui->tabCupboard->count() == 0) return;

    for (int i = 0; i < ui->spinCupboard->value(); i++)
    {
        for(int j = 0; j < ui->spinCellList->value(); j++)
        {
           ClickCupboard(i,0);
        }
    }

}
//---------------------------------------------------------------------
void SettingsWindow::on_Update1Button_clicked()
{
    emit GetCurrentPosSignal(ui->spinBox1X, ui->spinBox1Y);
}

void SettingsWindow::on_Update2Button_clicked()
{
    emit GetCurrentPosSignal(ui->spinBox2X, ui->spinBox2Y);
}
