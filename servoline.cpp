#include "servoline.h"

Servoline::Servoline(QString name_, int address_)
{
    name = name_;
    ServoAddres = address_;
    InitServoParameters();
}

//--------------------------------------------------------------------------------
Servoline::~Servoline()
{

}
//---------------------------------------------------------------------------
QString Servoline::GetName()
{
    return name;
}

void Servoline::InitServoParameters()
{
    ServoParam[PA771].Description = "Вращение при поиске нулевого положения";
    ServoParam[PA771].Type = ServoRegistersType::d4;

    ServoParam[PA700].Description = "Установки 1. Внутреннее управление положением";
    ServoParam[PA700].Type = ServoRegistersType::h4;

    ServoParam[PA775].Description = "Скорость поиска нулевого положения";
    ServoParam[PA775].Type = ServoRegistersType::d;

    ServoParam[PA306].Description = "Скорость JOG";
    ServoParam[PA306].Type = ServoRegistersType::d;

    ServoParam[PA701].Description = "Точка 1. Младший разряд";
    ServoParam[PA701].Type = ServoRegistersType::h;

    ServoParam[PA702].Description = "Точка 1. Старший разряд";
    ServoParam[PA702].Type = ServoRegistersType::h;

    ServoParam[PA508].Description = "Уровни D1,D2,D3,D4";
    ServoParam[PA508].Type = ServoRegistersType::b;

    ServoParam[PA509].Description = "Уровни D5,D6,D7,D8";
    ServoParam[PA509].Type = ServoRegistersType::b;


}
