#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSql>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlTableModel>

class DataBase : public QObject
{
    Q_OBJECT
public:
    explicit DataBase(QObject *parent = nullptr);
    ~DataBase();


    bool inserIntoTable(const QVariantList &data);
    bool UpdateTable(QJsonObject& obj, int box_, int line_, int cell_);

    int GetMaxBoxCount();
    int GetMaxLineCount(int box);

signals:

private:
    // Сам объект базы данных, с которым будет производиться работа
    QSqlDatabase db;
    QSqlTableModel* model;

};

#endif // DATABASE_H
