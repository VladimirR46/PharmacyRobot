#ifndef CARTDRUGS_H
#define CARTDRUGS_H

#include <QObject>
#include <QDebug>
#include <stdio.h>
#include <windows.h>
#include "settingswindow.h"
#include <QElapsedTimer>
#include <QTimer>

class CartDrugs : public QObject
{
    Q_OBJECT
public:
    explicit CartDrugs(SettingsWindow *settingsWindow, QObject *parent = nullptr);
    ~CartDrugs();

    HANDLE openPort(const char * portName, unsigned int baudRate);
    BOOL maestroGetPosition(HANDLE port, unsigned char channel, unsigned short * position);
    BOOL maestroSetTarget(HANDLE port, unsigned char channel, unsigned short target);

    bool SetTarget(int id, int target);
    int GetPosition(int id);

    void OpenCart();    // Открыть корзину
    void CloseCart();   // Закрыть корзину

    void UpGrip();
    void DownGrip();

    bool Gather(); // Собрать
    bool Drop();

public slots:
    void ConnectCart();
    void DisconnectCart();

    void GatherTimeout();
    void DropTimeout();

signals:
        void CartDrugsConnectedSignal();

private:
    HANDLE port;
    QString portName;
    int baudRate;
    BOOL success;
    unsigned short target, position;

    SettingsWindow *p_settingsWindow;

    QTimer* GatherTimer;
    QTimer* DropTimer;

    bool isGather = false;
    bool isDrop = false;
    bool isDownGripActive = false;
    bool isCloseCartActive = false;
};

#endif // CARTDRUGS_H
