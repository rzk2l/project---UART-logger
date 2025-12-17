# Non-Blocking UART Logger using Interrupts and DMA
## Introduction 
The following is a UART driver, It allows to use the UART2 (UART connected to the ST-Link debbuger, and thus to the computer when plugged) facilities of the STM32(F401RE) Nucleo board. The Blocking UART mode has been skipped, this driver only contains non-blocking UART implementation using interrupts and DMA.

## Documentation
## Using Interrupts

The UART class, which should be instantiated firstly in the code. Its members are the TX/RX ring buffers, flags and an output buffer used to echo any received messages. Below is a description of the data members :
- **RX/TX Buffers** : Ring buffers that can hold up to 30Bytes. They are traversed using the head/tail members.
- **TXBusy/LineIdle** : Booleans that respectively signal if the transmitter is actively transmitting, and if the line is idle which (not only) means that reception is finished.
- **Output buffer** : It is used to echo the received messages.
The following section describes the methods of the UART class :
- **void init(int baudrate)*** : This method enables the clock to GPIOA and sets the pins as UART2_rx and UART2_tx. It also sets takes in the baudrate as argument (for the moment, the supported baudrates are 115200 and 9600). It also enables the transmitter, receiver, their respective interrupts, as well as the UART2_IRQn.
### TRANSMISSION :
- **void sendString(const char\* data)** : Is the function responsible for sending the argument "data" provided by the user. It loads the data to the TXBuffer and activates (if transmitter is not busy (TXBusy)) the TXE interrupt that fires if the UART DR is empty. We use this interrupt to load the current TXBuffer into the UART DR to be transmitted. Once the TXBuffer is empty, the TXE interrupt is disabled otherwise It'll keep firing every clock cycle.
### RECEPTION
The RXNE interrupt is activated. Everytime it fires, the ISR will read the value present at the UART DR into the RXBuffer.
- **bool dataAvailable()** : returns false if the RXBuffer is empty.
- **bool Idle()** : returns true if the reception is finished.
- **char receiveChar()** : returns characters from the RXBuffer.
- **uint8_t readLine()** : returns how many characters compose the message; It's role is to extract characters from the RXBuffer (by calling receiveChar), and aappend the "\0" delimiter to the end of the message so it can be properly echoed.
- **void echoOnReceived()** : Calls the latter function and then calls sendString(outputBuffer) to echo the received message. The driver implements this function inside the IRQ handler, more precisely inside the IDLE condition, meaning that it will fire everytime the reception of a message ends.
- The IRQ handler also clears other errors that can occur and cause an interrupt, such as the overrun error. However, no mechanism is in place to deal with them.

## Using DMA
