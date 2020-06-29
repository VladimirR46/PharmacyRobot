#ifndef RESTOCKINGWINDOW_H
#define RESTOCKINGWINDOW_H

#include <QWidget>

namespace Ui {
class RestockingWindow;
}

class RestockingWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RestockingWindow(QWidget *parent = nullptr);
    ~RestockingWindow();

    void ShowRestock(int row, int count);

private slots:
    void on_okButton_clicked();

signals:
    void  RestockCellSignal(int row, int count);

private:
    Ui::RestockingWindow *ui;

    int CurrentRow = 0;
};

#endif // RESTOCKINGWINDOW_H
