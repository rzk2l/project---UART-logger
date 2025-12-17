#include <stm32f4xx.h>

#include "BSP.h"
#include "UART.h"
using namespace std;


extern "C" int cpp_main(void){
	__enable_irq();

	UART uart;
	uart.init(115200);

	uart.sendString("text 123\n");
	uart.sendString("abcde\n");
	uart.sendString("kdaa\n");
	uart.sendString("koilkj");
	while(1){
	}
	
	return 0;
}