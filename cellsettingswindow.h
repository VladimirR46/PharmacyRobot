#ifndef CELLSETTINGSWINDOW_H
#define CELLSETTINGSWINDOW_H

#include <QWidget>

namespace Ui {
class CellSettingsWindow;
}

class CellSettingsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CellSettingsWindow(QWidget *parent = nullptr);
    ~CellSettingsWindow();

    void ShowSettings(QJsonObject& db, int box_, int line_, int cell_);


private slots:
    void on_SaveButton_clicked();

signals:
    void  SaveCellConfig(QJsonObject& db, int box_, int line_, int cell_);

private:
    Ui::CellSettingsWindow *ui;

    int box;
    int line;
    int cell;
};

#endif // CELLSETTINGSWINDOW_H
