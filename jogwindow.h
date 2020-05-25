#ifndef JOG_H
#define JOG_H

#include <QDialog>

namespace Ui {
class JOGWindow;
}

class JOGWindow : public QDialog
{
    Q_OBJECT

public:
    explicit JOGWindow(QWidget *parent = nullptr);
    ~JOGWindow();

    void SetJOGSpeed(quint16 speed);

// Опишем сигналы:
signals:
        void JOGSignal(int id, int jogState);

private slots:
        void on_ButtonStart_clicked();
        void showEvent(QShowEvent *ev);
        void hideEvent(QHideEvent *event);
        void on_ButtonBack_released();
        void on_ButtonForward_released();
        void on_ButtonBack_pressed();
        void on_ButtonForward_pressed();

private:
    Ui::JOGWindow *ui;
};

#endif // JOG_H
