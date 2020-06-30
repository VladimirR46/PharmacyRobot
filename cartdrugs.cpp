#include "cartdrugs.h"

CartDrugs::CartDrugs(SettingsWindow *settingsWindow, QObject *parent) : QObject(parent)
{
    p_settingsWindow = settingsWindow;


    GatherTimer = new QTimer();
    connect(GatherTimer, SIGNAL(timeout()), this, SLOT(GatherTimeout()));

    DropTimer = new QTimer();
    connect(DropTimer, SIGNAL(timeout()), this, SLOT(DropTimeout()));
}

CartDrugs::~CartDrugs()
{
    DisconnectCart();
}

/** Opens a handle to a serial port in Windows using CreateFile.
 * portName: The name of the port.
 * baudRate: The baud rate in bits per second.
 * Returns INVALID_HANDLE_VALUE if it fails.  Otherwise returns a handle to the port.
 *   Examples: "COM4", "\\\\.\\USBSER000", "USB#VID_1FFB&PID_0089&MI_04#6&3ad40bf600004# */
HANDLE CartDrugs::openPort(const char * portName, unsigned int baudRate)
{
    HANDLE port;
    DCB commState;
    BOOL success;
    COMMTIMEOUTS timeouts;

    /* Open the serial port. */
    port = CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (port == INVALID_HANDLE_VALUE)
    {
        switch(GetLastError())
        {
        case ERROR_ACCESS_DENIED:
            fprintf(stderr, "Error: Access denied.  Try closing all other programs that are using the device.\n");
            break;
        case ERROR_FILE_NOT_FOUND:
            fprintf(stderr, "Error: Serial port not found.  "
                "Make sure that \"%s\" is the right port name.  "
                "Try closing all programs using the device and unplugging the "
                "device, or try rebooting.\n", portName);
            break;
        default:
            fprintf(stderr, "Error: Unable to open serial port.  Error code 0x%x.\n", GetLastError());
            break;
        }
        return INVALID_HANDLE_VALUE;
    }

    /* Set the timeouts. */
    success = GetCommTimeouts(port, &timeouts);
    if (!success)
    {
        fprintf(stderr, "Error: Unable to get comm timeouts.  Error code 0x%x.\n", GetLastError());
        CloseHandle(port);
        return INVALID_HANDLE_VALUE;
    }
    timeouts.ReadIntervalTimeout = 1000;
    timeouts.ReadTotalTimeoutConstant = 1000;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 1000;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    success = SetCommTimeouts(port, &timeouts);
    if (!success)
    {
        fprintf(stderr, "Error: Unable to set comm timeouts.  Error code 0x%x.\n", GetLastError());
        CloseHandle(port);
        return INVALID_HANDLE_VALUE;
    }

    /* Set the baud rate. */
    success = GetCommState(port, &commState);
    if (!success)
    {
        fprintf(stderr, "Error: Unable to get comm state.  Error code 0x%x.\n", GetLastError());
        CloseHandle(port);
        return INVALID_HANDLE_VALUE;
    }
    commState.BaudRate = baudRate;
    success = SetCommState(port, &commState);
    if (!success)
    {
        fprintf(stderr, "Error: Unable to set comm state.  Error code 0x%x.\n", GetLastError());
        CloseHandle(port);
        return INVALID_HANDLE_VALUE;
    }

    /* Flush out any bytes received from the device earlier. */
    success = FlushFileBuffers(port);
    if (!success)
    {
        fprintf(stderr, "Error: Unable to flush port buffers.  Error code 0x%x.\n", GetLastError());
        CloseHandle(port);
        return INVALID_HANDLE_VALUE;
    }

    return port;
}

/** Implements the Maestro's Get Position serial command.
 * channel: Channel number from 0 to 23
 * position: A pointer to the returned position value (for a servo channel, the units are quarter-milliseconds)
 * Returns 1 on success, 0 on failure.
 * For more information on this command, see the "Serial Servo Commands"
 * section of the Maestro User's Guide: http://www.pololu.com/docs/0J40 */
BOOL CartDrugs::maestroGetPosition(HANDLE port, unsigned char channel, unsigned short * position)
{
    unsigned char command[2];
    unsigned char response[2];
    BOOL success;
    DWORD bytesTransferred;

    // Compose the command.
    command[0] = 0x90;
    command[1] = channel;

    // Send the command to the device.
    success = WriteFile(port, command, sizeof(command), &bytesTransferred, NULL);
    if (!success)
    {
        fprintf(stderr, "Error: Unable to write Get Position command to serial port.  Error code 0x%x.", GetLastError());
        return 0;
    }
    if (sizeof(command) != bytesTransferred)
    {
        fprintf(stderr, "Error: Expected to write %d bytes but only wrote %d.", sizeof(command), bytesTransferred);
        return 0;
    }

    // Read the response from the device.
    success = ReadFile(port, response, sizeof(response), &bytesTransferred, NULL);
    if (!success)
    {
        fprintf(stderr, "Error: Unable to read Get Position response from serial port.  Error code 0x%x.", GetLastError());
        return 0;
    }
    if (sizeof(response) != bytesTransferred)
    {
        fprintf(stderr, "Error: Expected to read %d bytes but only read %d (timeout). "
            "Make sure the Maestro's serial mode is USB Dual Port or USB Chained.", sizeof(command), bytesTransferred);
        return 0;
    }

    // Convert the bytes received in to a position.
    *position = response[0] + 256*response[1];

    return 1;
}

/** Implements the Maestro's Set Target serial command.
 * channel: Channel number from 0 to 23
 * target: The target value (for a servo channel, the units are quarter-milliseconds)
 * Returns 1 on success, 0 on failure.
 * Fore more information on this command, see the "Serial Servo Commands"
 * section of the Maestro User's Guide: http://www.pololu.com/docs/0J40 */
BOOL CartDrugs::maestroSetTarget(HANDLE port, unsigned char channel, unsigned short target)
{
    unsigned char command[4];
    DWORD bytesTransferred;
    BOOL success;

    // Compose the command.
    command[0] = 0x84;
    command[1] = channel;
    command[2] = target & 0x7F;
    command[3] = (target >> 7) & 0x7F;

    // Send the command to the device.
    success = WriteFile(port, command, sizeof(command), &bytesTransferred, NULL);
    if (!success)
    {
        fprintf(stderr, "Error: Unable to write Set Target command to serial port.  Error code 0x%x.", GetLastError());
        return 0;
    }
    if (sizeof(command) != bytesTransferred)
    {
        fprintf(stderr, "Error: Expected to write %d bytes but only wrote %d.", sizeof(command), bytesTransferred);
        return 0;
    }

    return 1;
}

bool CartDrugs::SetTarget(int id, int target)
{

    /* Set the target of channel 0. */
    success = maestroSetTarget(port, id, (target*40)+3600);
    if (!success) return false;
    qDebug() << "(" << (target*40)+3600 <<" градусов)";
    return true;
}

int CartDrugs::GetPosition(int id)
{
    /* Get the current position of channel 0. */
    success = maestroGetPosition(port, id, &position);
    if (!success) return -1;
    qDebug() << "(" << position/4 <<" us)";

    return (position-3600)/40;
}

void CartDrugs::OpenCart()
{
    SetTarget(0,p_settingsWindow->settings().OpenCartAngle);
}

void CartDrugs::CloseCart()
{
    SetTarget(0,p_settingsWindow->settings().CloseCartAngle);
}

void CartDrugs::UpGrip()
{
    SetTarget(1,p_settingsWindow->settings().UpCartAngle);
}

void CartDrugs::DownGrip()
{
    SetTarget(1,p_settingsWindow->settings().DownCartAngle);
}
//------------------------------------------------------------------------
void CartDrugs::GatherTimeout()
{
    if(!isDownGripActive)
    {
       DownGrip();
       isDownGripActive = true;
    }
    else
    {
        isDownGripActive = false;
        isGather = true;
        GatherTimer->stop();
    }
}
//------------------------------------------------------------------------
void CartDrugs::DropTimeout()
{
    if(!isCloseCartActive)
    {
       CloseCart();
       isCloseCartActive = true;
    }
    else
    {
        isCloseCartActive = false;
        isDrop = true;
        DropTimer->stop();
    }
}
//------------------------------------------------------------------------
bool CartDrugs::Gather()
{
    if(!GatherTimer->isActive() && !isGather)
    {
        UpGrip();
        GatherTimer->start(400);
    }

    if(isGather)
    {
       isGather = false;
       return true;
    }

    return false;
}
//------------------------------------------------------------------------
bool CartDrugs::Drop()
{
    if(!DropTimer->isActive() && !isDrop)
    {
        OpenCart();
        DropTimer->start(400);
    }

    if(isDrop)
    {
       isDrop = false;
       return true;
    }
    return false;
}
//------------------------------------------------------------------------
void CartDrugs::ConnectCart()
{
    /* portName should be the name of the Maestro's Command Port (e.g. "COM4")
         * as shown in your computer's Device Manager.
         * Alternatively you can use \\.\USBSER000 to specify the first virtual COM
         * port that uses the usbser.sys driver.  This will usually be the Maestro's
         * command port. */
        portName = "\\\\.\\COM"+QString::number(p_settingsWindow->settings().CartPort); //"\\\\.\\USBSER000";  // Each double slash in this source code represents one slash in the actual name.
        //portName = "\\\\.\\USBSER000"; //"\\\\.\\USBSER000";

        /* Choose the baud rate (bits per second).
         * If the Maestro's serial mode is USB Dual Port, this number does not matter. */
        baudRate = 9600;

        qDebug() << "Connected.....";

        /* Open the Maestro's serial port. */
        port = openPort(portName.toStdString().c_str(), baudRate);
        if (port == INVALID_HANDLE_VALUE) return;
        qDebug() << "CartDrugs connected";

}

void CartDrugs::DisconnectCart()
{
    /* Close the serial port so other programs can use it.
     * Alternatively, you can just terminate the process (return from main). */
    CloseHandle(port);
}
