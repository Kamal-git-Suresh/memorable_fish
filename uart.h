#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <iostream>
#include <vector>
#include <stdint.h>

class UART
{
public:
    UART(const std::string& device, int baudRate);
    ~UART();    
    bool sendData(const std::vector<uint8_t>& data);
private:
    int serialPort;
    struct termios tty;
};