#include "stm32f401xe.h"
#include "BSP.h"

void assert_failed(){
    NVIC_SystemReset();
}
