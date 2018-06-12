
#include <nrf_gpio.h>

void nop(void)
{
	__asm__ __volatile__("nop":::);
}

int main(void)
{
	const uint32_t pin = NRF_GPIO_PIN_MAP(0,13); // LED1
	nrf_gpio_cfg_output(pin);

	while(1) {
		nrf_gpio_pin_toggle(pin);
		for(uint32_t i = 0; i < 0x320000; ++i)
			nop();
	}
}

