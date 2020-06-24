#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setOrganizationName("Innopolis");
    QApplication::setApplicationName("PharmRobot");

    MainWindow w;
    w.show();
    return a.exec();
}
