#ifndef CARTDRUGS_H
#define CARTDRUGS_H

#include <QObject>
#include <QDebug>
#include <stdio.h>
#include <windows.h>

class CartDrugs : public QObject
{
    Q_OBJECT
public:
    explicit CartDrugs(QObject *parent = nullptr);
    ~CartDrugs();

    HANDLE openPort(const char * portName, unsigned int baudRate);
    BOOL maestroGetPosition(HANDLE port, unsigned char channel, unsigned short * position);
    BOOL maestroSetTarget(HANDLE port, unsigned char channel, unsigned short target);

    bool SetTarget(int id, int target);
    int GetPosition(int id);

public slots:
    void ConnectCart();
    void DisconnectCart();

private:
    HANDLE port;
    char * portName;
    int baudRate;
    BOOL success;
    unsigned short target, position;
};

#endif // CARTDRUGS_H
