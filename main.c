#include "blink.h"
#include "reg.h"
#include <stdint.h>

int main(void)
{
	button_init();
	while(! READ_BIT(GPIO_BASE(GPIO_PORTA) + GPIOx_IDR_OFFSET, IDRy_BIT(0)));
	blink(LED_BLUE);
}
