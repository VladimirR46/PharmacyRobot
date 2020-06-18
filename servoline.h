#ifndef SERVOLINE_H
#define SERVOLINE_H

#include <QMainWindow>
#include "ServoRegisters.h"

class Servoline
{

public:
    Servoline(QString name_, int address_);
    ~Servoline();

    int ServoAddres = 0;
    bool isPower = false;
    bool isHome = false;

    QString GetName();

    struct ServoParameters
    {
        QString Description;
        ServoRegistersType Type;
        int AddRegistr;
        unsigned short Value;
        //char v[6];
    };


    void InitServoParameters();


    ServoParameters ServoParam[999];


private:
    QString name;

};

#endif // SERVOLINE_H
