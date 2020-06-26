#include "database.h"
#include <QDebug>
#include <QSqlRecord>
#include <QJsonObject>

DataBase::DataBase(QObject *parent) : QObject(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("mytest.db");

    if(db.open())
    {
        model = new QSqlTableModel(parent, db);
        model->setTable("druglist");
        model->setEditStrategy(QSqlTableModel::OnManualSubmit);

        model->select();

        qDebug() << model->lastError().text();

        for (int i = 0; i < model->rowCount(); ++i) {
            QString name = model->record(i).value("name").toString();
            int salary = model->record(i).value("count").toInt();
            qDebug() << name << salary;
        }
    }
}

DataBase::~DataBase()
{
    db.close();
}

bool DataBase::inserIntoTable(const QVariantList &data)
{
     int row=model->rowCount();
     qDebug() << "rowCount" << row;
     model->insertRows(row, 1);
     model->setData(model->index(row, 0), data[0].toInt());
     model->setData(model->index(row, 1), data[1].toInt());
     model->setData(model->index(row, 2), data[2].toInt());
     model->setData(model->index(row, 3), data[3].toInt());
     model->setData(model->index(row, 4), data[4].toString());
     model->setData(model->index(row, 5), data[5].toInt());

     if(model->submitAll()) return true;
     else return false;
}

bool DataBase::UpdateTable(QJsonObject& obj, int box_, int line_, int cell_)
{
    bool ret = false;

    model->setFilter("box="+QString::number(box_)+" AND line="+QString::number(line_)+" AND cell="+QString::number(cell_));
    model->select();

    if(model->rowCount() == 1)
    {
        QSqlRecord record = model->record(0);
        record.setValue("name", obj["Name"].toString());
        model->setRecord(0, record);

        if(model->submitAll()) ret = true;
        else ret = false;
    }

    return ret;
}

int DataBase::GetMaxBoxCount()
{
    int max = 0;
    QSqlQuery query(db);
    query.exec("SELECT MAX(box) FROM druglist");
        if (query.next()) {
            qDebug() << query.value(0).toString();
            max = query.value(0).toInt();
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
            qDebug() << query.value(0).toString();
            max = query.value(0).toInt();
        }
    query.finish();
    return max;
}
