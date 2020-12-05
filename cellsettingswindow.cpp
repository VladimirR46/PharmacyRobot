#include "cellsettingswindow.h"
#include "ui_cellsettingswindow.h"

CellSettingsWindow::CellSettingsWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CellSettingsWindow)
{
    ui->setupUi(this);

    connect(ui->CancelButton , &QPushButton::clicked, this, &CellSettingsWindow::hide);
}

void CellSettingsWindow::ShowSettings(DataBase& db, int box_, int line_, int cell_)
{
    cell.Box = box_;
    cell.Line = line_;
    cell.Cell = cell_;
    if(db.GetCell(cell))
    {
        ui->NameEdit->setText(cell.Name);
        ui->ProductCodeEdit->setText(QString::number(cell.ProductCode));
        ui->CountSpin->setValue(cell.Count);
        ui->spinX->setValue(cell.X);
        ui->spinY->setValue(cell.Y);

        show();
    }
}

CellSettingsWindow::~CellSettingsWindow()
{
    delete ui;
}


void CellSettingsWindow::on_SaveButton_clicked()
{
    cell.Name = ui->NameEdit->text();
    cell.ProductCode = ui->ProductCodeEdit->text().toInt();
    cell.Count = ui->CountSpin->value();
    cell.X = ui->spinX->value();
    cell.Y = ui->spinY->value();

    emit SaveCellConfig(cell);

    hide();
}

void CellSettingsWindow::on_CurrPosButton_clicked()
{
    emit GetCurrentPosSignal(ui->spinX, ui->spinY);
}

void CellSettingsWindow::on_DeleteButton_clicked()
{
    emit DeleteCell(cell);
    hide();
}
