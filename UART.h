#ifndef UART_H_
#define UART_H_
#include "stm32f401xe.h"

#define TEXT_BUFFER_SIZE 20

class UART{
    volatile char textBuffer[TEXT_BUFFER_SIZE] = {0};  // RING BUFFER TO HOLD THE DATA TO TRANSMIT
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