#ifndef UART_H_
#define UART_H_
#include "stm32f401xe.h"

constexpr int TEXT_BUFFER_SIZE = 30;
//constexpr int MAX_MSG = 4;

class UART{
    volatile char TXBuffer[TEXT_BUFFER_SIZE];  // RING BUFFER TO HOLD THE DATA TO TRANSMIT
    volatile uint8_t bufferHead = 0;
    volatile uint8_t bufferTail = 0;
    volatile bool txBusy = false;

    volatile char rxTextBuffer[TEXT_BUFFER_SIZE] = {0};   //  RING BUFFER TO HOLD THE RECEIVED DATA
    volatile uint8_t rxBufferHead = 0;
    volatile uint8_t rxBufferTail = 0;
    volatile bool lineIdle = false;

    char outputBuffer[TEXT_BUFFER_SIZE] = {0};
public:
    void init(int baudrate);
    void sendString(const char* data);

    char receiveChar();
    bool dataAvailable();
    bool Idle();

    uint8_t readLine();
    void echoOnReceived();

    void handleIRQ();
};




#endif