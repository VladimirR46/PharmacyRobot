#include "restockingwindow.h"
#include "ui_restockingwindow.h"

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

void RestockingWindow::ShowRestock(int row, int count)
{
   CurrentRow = row;
   ui->spinBox->setValue(count);
   show();
}

void RestockingWindow::on_okButton_clicked()
{
    emit RestockCellSignal(CurrentRow, ui->spinBox->value());
    hide();
}
