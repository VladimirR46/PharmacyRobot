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

    void ShowRestock(int row, int count, int box, int led_id);

private slots:
    void on_okButton_clicked();

signals:
    void  RestockCellSignal(int row, int count);
    void  ledOFFSignal(int box, int led_id);

protected:
   void hideEvent(QHideEvent * event);

private:
    Ui::RestockingWindow *ui;

    int CurrentRow = 0;
    int CurrentBox = 0;
    int led_id = 0;
};

#endif // RESTOCKINGWINDOW_H
