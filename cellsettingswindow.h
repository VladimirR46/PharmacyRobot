#ifndef CELLSETTINGSWINDOW_H
#define CELLSETTINGSWINDOW_H

#include <QWidget>
#include "database.h"

namespace Ui {
class CellSettingsWindow;
}

class CellSettingsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CellSettingsWindow(QWidget *parent = nullptr);
    ~CellSettingsWindow();

    void ShowSettings(DataBase& db, int box_, int line_, int cell_);


private slots:
    void on_SaveButton_clicked();

signals:
    void  SaveCellConfig(DataBase::Cell& cell);

private:
    Ui::CellSettingsWindow *ui;

    DataBase::Cell cell;
};

#endif // CELLSETTINGSWINDOW_H
