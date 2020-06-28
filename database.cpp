#include "database.h"
#include <QDebug>

DataBase::DataBase(QObject *parent) : QObject(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("mytest.db");

    if(db.open())
    {
        if(db.tables().count() == 0)
           createTable();


        model = new QSqlTableModel(parent, db);
        model->setTable("druglist");
        model->setEditStrategy(QSqlTableModel::OnManualSubmit);
        model->select();
        qDebug() << model->lastError().text();

    }
}

DataBase::~DataBase()
{
    db.close();
}

bool DataBase::inserIntoTable(const Cell& cell)
{
     int row=model->rowCount();
     model->insertRows(row, 1);
     model->setData(model->index(row, 0), cell.Box);
     model->setData(model->index(row, 1), cell.Line);
     model->setData(model->index(row, 2), cell.Cell);
     model->setData(model->index(row, 3), cell.ProductCode);
     model->setData(model->index(row, 4), cell.Name);
     model->setData(model->index(row, 5), cell.Count);
     model->setData(model->index(row, 6), cell.X);
     model->setData(model->index(row, 7), cell.Y);

     return true;
}

bool DataBase::GetCell(Cell& cell)
{
    model->setFilter("box="+QString::number(cell.Box)+" AND line="+QString::number(cell.Line)+" AND cell="+QString::number(cell.Cell));
    if(!model->select()) return false;

    if(model->rowCount() == 1)
    {
        QSqlRecord record = model->record(0);
        cell.ProductCode = record.value("productcode").toInt();
        cell.Name = record.value("name").toString();
        cell.Count = record.value("count").toInt();
        cell.X = record.value("x").toInt();
        cell.Y = record.value("y").toInt();
        return true;
    }

    return false;
}

bool DataBase::GetCell(Cell& cell, int productcode)
{
    model->setFilter("productcode="+QString::number(productcode));
    if(!model->select()) return false;

    if(model->rowCount() == 1)
    {
        QSqlRecord record = model->record(0);
        cell.Box = record.value("box").toInt();
        cell.Line = record.value("line").toInt();
        cell.Cell = record.value("cell").toInt();
        cell.ProductCode = record.value("productcode").toInt();
        cell.Name = record.value("name").toString();
        cell.Count = record.value("count").toInt();
        cell.X = record.value("x").toInt();
        cell.Y = record.value("y").toInt();
        return true;
    }

    return false;
}

bool DataBase::SetCell(DataBase::Cell &cell)
{
    model->setFilter("box="+QString::number(cell.Box)+" AND line="+QString::number(cell.Line)+" AND cell="+QString::number(cell.Cell));
    if(!model->select()) return false;

    if(model->rowCount() == 1)
    {
        QSqlRecord record = model->record(0);
        record.setValue("productcode", cell.ProductCode);
        record.setValue("name", cell.Name);
        record.setValue("count", cell.Count);
        record.setValue("x", cell.X);
        record.setValue("y", cell.Y);
        model->setRecord(0, record);
        return true;
    }
    return false;
}

bool DataBase::SetCellProductCount(int count, int productcode)
{
    QSqlQuery query;
    if(query.exec("UPDATE druglist SET count = "+QString::number(count)+" WHERE productcode = "+QString::number(productcode)))
    {
        return true;
    }
    return false;
}

int DataBase::GetMaxCellCount(int box, int line)
{
    int max = 0;
    QSqlQuery query(db);
    query.exec("SELECT MAX(cell) FROM druglist WHERE box="+QString::number(box)+" AND line="+QString::number(line));
        if (query.next()) {
            qDebug() << query.value(0).toString();
            max = query.value(0).toInt()+1;
        }
    query.finish();
    return max;
}

bool DataBase::createTable()
{
    QSqlQuery query;
    if(!query.exec( "CREATE TABLE druglist ("
                    "box INTEGER,"
                    "line INTEGER,"
                    "cell INTEGER,"
                    "productcode INTEGER,"
                    "name VARCHAR(255),"
                    "count INTEGER,"
                    "x INTEGER,"
                    "y INTEGER"
                        " )"
                    )){
        qDebug() << "DataBase: error of create ";
        qDebug() << query.lastError().text();
        return false;
    } else {
        return true;
    }
    return false;
}

int DataBase::GetMaxBoxCount()
{
    int max = 0;
    QSqlQuery query(db);
    query.exec("SELECT MAX(box) FROM druglist");
        if (query.next())
        {
            if(query.value(0).toString() != "")
                max = query.value(0).toInt()+1;
        }
    query.finish();
    return max;
}

int DataBase::GetMaxLineCount(int box)
{
    int max = 0;
    QSqlQuery query(db);
    query.exec("SELECT MAX(line) FROM druglist WHERE box="+QString::number(box));
        if (query.next()) {
            max = query.value(0).toInt()+1;
        }
    query.finish();
    return max;
}
