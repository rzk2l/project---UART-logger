#include <stm32f4xx.h>

#include "BSP.h"
#include "UART.h"
using namespace std;


extern "C" int cpp_main(void){
	__enable_irq();

	UART uart;
	uart.init(115200);

	uart.sendString("text 123");

	while(1){
		if(uart.dataAvailable() || uart.Idle()){
			uart.echoOnReceived();
		}
	}
	
	return 0;
}