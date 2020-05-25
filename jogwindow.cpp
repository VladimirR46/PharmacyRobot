#include "jogwindow.h"
#include "ui_jogwindow.h"

JOGWindow::JOGWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JOGWindow)
{
    ui->setupUi(this);

}

JOGWindow::~JOGWindow()
{
    delete ui;
}

void JOGWindow::on_ButtonStart_clicked()
{
    if(ui->ButtonStart->text() == "Включить")
    {
        emit JOGSignal(0,1);
        ui->ButtonStart->setText("Отключить");
    }
    else
    {
        emit JOGSignal(0,0);
        ui->ButtonStart->setText("Включить");
    }
}

void JOGWindow::SetJOGSpeed(quint16 speed)
{
    ui->SpeedEdit->setText(QString::number(speed,10));
}

void JOGWindow::showEvent(QShowEvent *ev)
{
    QDialog::showEvent(ev);
    emit JOGSignal(3,0);
}

void JOGWindow::hideEvent(QHideEvent *event)
{
    QDialog::hideEvent(event);
    emit JOGSignal(0,0);
}

void JOGWindow::on_ButtonBack_released()
{
    emit JOGSignal(1,0);

}

void JOGWindow::on_ButtonForward_released()
{
    emit JOGSignal(2,0);
}

void JOGWindow::on_ButtonBack_pressed()
{
    emit JOGSignal(1,1);
}

void JOGWindow::on_ButtonForward_pressed()
{
    emit JOGSignal(2,1);
}
