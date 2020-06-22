#include "cellsettingswindow.h"
#include "ui_cellsettingswindow.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

CellSettingsWindow::CellSettingsWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CellSettingsWindow)
{
    ui->setupUi(this);

    connect(ui->CancelButton , &QPushButton::clicked, this, &CellSettingsWindow::hide);
}

void CellSettingsWindow::ShowSettings(QJsonObject& db, int box_, int line_, int cell_)
{
    box = box_;
    line = line_;
    cell = cell_;

    QJsonObject obj = db["modules"].toArray()[box].toArray()[line].toArray()[cell].toObject();

    ui->NameEdit->setText(obj["Name"].toString());
    ui->ProductCodeEdit->setText(QString::number(obj["ProductCode"].toInt()));
    ui->CountSpin->setValue(obj["Count"].toInt());
    ui->XEdit->setText(QString::number(obj["X"].toInt()));
    ui->YEdit->setText(QString::number(obj["Y"].toInt()));

   show();
}

CellSettingsWindow::~CellSettingsWindow()
{
    delete ui;
}


void CellSettingsWindow::on_SaveButton_clicked()
{
    QJsonObject obj;
    obj["Name"] = ui->NameEdit->text();
    obj["ProductCode"] = ui->ProductCodeEdit->text().toInt();
    obj["Count"] = ui->CountSpin->value();
    obj["X"] = ui->XEdit->text().toInt();
    obj["Y"] = ui->YEdit->text().toInt();

    emit SaveCellConfig(obj, box, line, cell);

    hide();
}
