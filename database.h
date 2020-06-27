#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSql>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlRecord>

class DataBase : public QObject
{
    Q_OBJECT
public:
    explicit DataBase(QObject *parent = nullptr);
    ~DataBase();

    QSqlTableModel* model;


    bool inserIntoTable(const QVariantList &data);
    bool UpdateTable(QJsonObject& obj, int box_, int line_, int cell_);

    int GetMaxBoxCount();
    int GetMaxLineCount(int box);
    int GetMaxCellCount(int box, int line);
    bool createTable();

signals:

private:
    // Сам объект базы данных, с которым будет производиться работа
    QSqlDatabase db;


};

#endif // DATABASE_H
