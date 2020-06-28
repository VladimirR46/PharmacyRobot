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
        ui->XEdit->setText(QString::number(cell.X));
        ui->YEdit->setText(QString::number(cell.Y));

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
    cell.X = ui->XEdit->text().toInt();
    cell.Y = ui->YEdit->text().toInt();

    emit SaveCellConfig(cell);

    hide();
}
