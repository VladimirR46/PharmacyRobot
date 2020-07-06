#include "restockingwindow.h"
#include "ui_restockingwindow.h"
#include <QtDebug>

RestockingWindow::RestockingWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RestockingWindow)
{
    ui->setupUi(this);

    connect(ui->closeButton, &QPushButton::clicked, this, &RestockingWindow::hide);
}

RestockingWindow::~RestockingWindow()
{
    delete ui;
}

void RestockingWindow::ShowRestock(int row, int count, int box, int led_id)
{
   CurrentBox = box;
   this->led_id = led_id;
   CurrentRow = row;
   ui->spinBox->setValue(count);
   show();
}

void RestockingWindow::hideEvent(QHideEvent * /* event */)
{
    emit ledOFFSignal(CurrentBox, led_id);
}

void RestockingWindow::on_okButton_clicked()
{
    emit RestockCellSignal(CurrentRow, ui->spinBox->value());
    hide();
}
