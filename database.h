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

    struct Cell
    {
        int Box = 0;
        int Line = 0;
        int Cell = 0;
        int ProductCode = 0;
        QString Name;
        int Count = 0;
        int X = 0;
        int Y = 0;
    };

    QSqlTableModel* model;


    bool inserIntoTable(const Cell& cell);

    bool GetCell(Cell& cell);
    bool GetCell(Cell& cell, int productcode);
    bool SetCell(Cell& cell);

    bool SetCellProductCount(int count, int productcode);

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
