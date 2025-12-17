# Non-Blocking UART Logger using Interrupts and DMA
## Introduction 
The following is a UART driver, It allows to use the UART2 (UART connected to the ST-Link debbuger, and thus to the computer when plugged) facilities of the STM32(F401RE) Nucleo board. The Blocking UART mode has been skipped, this driver only contains non-blocking UART implementation using interrupts and DMA.

## Documentation 
### UART Class
The UART class, which should be instantiated firstly in the code. Its members are the TX/RX ring buffers, flags and an output buffer used to echo any received messages. Below is a description of the data members :
- **RX/TX Buffers** : The RX buffer is a 2D array that can hold up to 3 messages each 20Bytes long. This allows the queing of messages that are to be sent while the the DMA is still busy sending the previous message. Thus, the sent messages aren't corrupted;
