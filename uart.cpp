#include "uart.h"

UART::UART(const std::string& device, int baudRate)
{
    serialPort = open(device.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (serialPort == -1)
    {
        std::cerr << "Error opening serial port " << device << std::endl;
        return;
    }

    tcgetattr(serialPort, &tty);
    cfsetospeed(&tty, baudRate);
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; 
    tty.c_iflag = ~IGNBRK;
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 5;
    tcsetattr(serialPort, TCSANOW, &tty);
}

UART::~UART()
{
    if (serialPort != -1) close(serialPort);
}

bool UART::sendData(const std::vector<uint8_t>& data)
{
    if (serialPort == -1) return false;

    ssize_t bytesWritten = write(serialPort, data.data(), data.size());
    if (bytesWritten == -1)
    {
        std::cerr << "Error writing to serial port" << std::endl;
        return false;
    }
    return true;
}