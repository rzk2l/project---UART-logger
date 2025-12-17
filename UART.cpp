#include "UART.h"

UART* uart2_ptr = nullptr;  // GLOBAL UART OBJECT POINTER

void UART::init(int baudrate){
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN_Msk;    //  ENABLE CLOCK TO GPIOA 

    RCC->APB1ENR |= RCC_APB1ENR_USART2EN_Msk;   //  ENABLES THE CLOCK TO ONBOARD UART2 CONNECTED TO THE PORT

    GPIOA->MODER &= ~(GPIO_MODER_MODER2_Msk | GPIO_MODER_MODER3_Msk);
    GPIOA->MODER |= GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1;  //  SETS PINS 2 AND 3 TO ALTERNATE FUNCTION
    
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL2_Msk | GPIO_AFRL_AFSEL3_Msk);
    GPIOA->AFR[0] |= (7 << GPIO_AFRL_AFSEL2_Pos | 7 << GPIO_AFRL_AFSEL3_Pos);     // SETS THE ALTERNATE FUCNTION OF PIN2 AND PIN3 TO UART

    switch(baudrate){
    case 115200:
        USART2->BRR |=  0x8B;    //(0x8 << 4) | 11 SETTING THE BAUD RATE TO 115200, SEE FORMULA IN DATASHEET, WITH CLOCK FREQ 16MHZ
        break;
    case 9600:
        USART2->BRR |=  0xD6;   //(0xD << 4) | 6 SETTING THE BAUDRATE TO 9600
        break;
    default:
        USART2->BRR |=  0x8B;
        break;
    }
    
    /////////////////////////////////////////////////////////////////////////////////////////////
    uart2_ptr = this;   // SET THE GLOBAL POINTER TO POINT TO THIS OBJECT
    USART2->CR1 |= USART_CR1_TE | USART_CR1_UE | USART_CR1_RE | USART_CR1_RXNEIE | USART_CR1_IDLEIE;     // ENABLE USART2 AND THE TRANSMITTER AND THE RECEIVER (+ RECEIVER INTERRUPT)
    NVIC_EnableIRQ(USART2_IRQn);
    NVIC_SetPriority(USART2_IRQn, 8);
}

void UART::sendString(const char* data){
    int i = 0;
    while(data[i]!= '\0'){
        uint8_t nextHead = (bufferHead + 1) % TEXT_BUFFER_SIZE;     // CALCULATES THE INDEX OF THE NEXT CHARACTER (OR THE HEAD OF THE BUFFER)
        if(nextHead != bufferTail){     // CHECKS FOR OVERFLOW, IF NEXT HEAD IS THE TAIL (0), IT MEANS THE BUFFER OVERFLOWED
            TXBuffer[bufferHead] = data[i];   // PUTS THE CHARACTER IN THE BUFFER
            bufferHead = nextHead;  //  UPDATES THE HEAD 
        }
        ++i;    
    }
    if(!txBusy){
        txBusy = true;
        USART2->CR1 |= USART_CR1_TXEIE;     // DR IS EMPTY SO THE MOMENT THIS IS DONE AN INTERRUPT HAPPENS, AND AN INTERRUPT WILL HAPPEN EVERYTIME DR IS EMPTY, IE EVERYTIME TXBUSY IS FALSE
    }
    
}

char UART::receiveChar(){
    if(rxBufferHead == rxBufferTail){
        return 0;
    }

    char c = rxTextBuffer[rxBufferTail];
    rxBufferTail = (rxBufferTail + 1) % TEXT_BUFFER_SIZE;
    return c;
}

bool UART::dataAvailable(){
    return rxBufferHead != rxBufferTail;    //  RETURNS TRUE IF THE BUFFER IS NOT EMPTY
}

bool UART::Idle(){
    return lineIdle;
}

uint8_t UART::readLine(){
    static uint8_t currentStrLen = 0;

    while(((currentStrLen < TEXT_BUFFER_SIZE - 1))){
        char c = receiveChar();
        if(c == '\r' || c == '\n' || c == '\0'){
            outputBuffer[currentStrLen] = '\0';
            
            uint8_t finalSize = currentStrLen;
            currentStrLen = 0;
            
            lineIdle = false;
            return finalSize;
        }

        outputBuffer[currentStrLen] = c;
        ++currentStrLen;    
    }
    return 0;
}

void UART::echoOnReceived(){
        uint8_t receivedBytes = readLine();

        if(receivedBytes > 0){
            sendString(outputBuffer);
        }
}

void UART::handleIRQ(){
    uint32_t sr = USART2->SR;
    
    if (sr & (USART_SR_ORE | USART_SR_NE | USART_SR_FE)) {
        // Must read DR to clear the flags completely (especially ORE)
        (void)USART2->DR; 
    }
    if ((sr & USART_SR_IDLE)){
        (void)USART2->DR;
        if(dataAvailable()){
            echoOnReceived();
        }
        UART::lineIdle = true;  // SETS THE IDLE LINE TO TRUE WHICH MEANS RECEPTION TERMINATED
    }
    if ((sr & USART_SR_RXNE)) {  //  CHECKS IF THE INTERRUPT WAS CAUSED BY THE RECIEVER
        lineIdle = false;
        char c = (char)USART2->DR;

        uint8_t nextHead = (rxBufferHead + 1) % TEXT_BUFFER_SIZE;

        if(nextHead != rxBufferTail){    
            rxTextBuffer[rxBufferHead] = c;    // WRITES THE CONTENT OF THE DR REGISTER (WHERE RECIEVER SAVED INCOMING DATA INTO BUFFER)
            
            rxBufferHead = (rxBufferHead + 1) % TEXT_BUFFER_SIZE;   // INCREASE TO THE NEXT INDEX WHERE THE NEXT CHARACTER WILL BE PLACED
        } 
    }
    if((sr & USART_SR_TXE) && (USART2->CR1 & USART_CR1_TXEIE)){  // CHECK IF THE INTERRUPT IS VALID
        if(bufferHead != bufferTail){   //  THIS CHECKS IF THE BUFFER IS EMPTY
            USART2->DR = TXBuffer[bufferTail];    // WRITES THE FIRST CHARACTER THAT GOT INTO THE BUFFER

            bufferTail = (bufferTail + 1) % TEXT_BUFFER_SIZE;   // INCREASE TO THE NEXT CHARACTER

        } else {
            USART2->CR1 &= ~USART_CR1_TXEIE;    // DISABLE INTERRUPT WHEN BUFFER IS EMPTY, OTHERWISE IT'LL KEEP FIRING BECAUSE DR WILL BE EMPTY
            txBusy = false;
        }
        
    } 
    
}
extern "C"{
void USART2_IRQHandler(){
    if(uart2_ptr != nullptr){
        uart2_ptr->handleIRQ();
    }
}
}