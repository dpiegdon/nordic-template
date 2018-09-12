
#include <stdint.h>
#include <stdio.h>

#include "nrfx_uarte.h"
#include "nrf_gpio.h"

nrfx_uarte_t uart0 = NRFX_UARTE_INSTANCE(0);
const nrfx_uarte_config_t uart0_config = {
	.pseltxd = NRF_GPIO_PIN_MAP(1,10),
	.pselrxd = NRF_GPIO_PIN_MAP(1,11),
	.pselcts = NRF_UARTE_PSEL_DISCONNECTED,
	.pselrts = NRF_UARTE_PSEL_DISCONNECTED,
	.hwfc = NRFX_UART_DEFAULT_CONFIG_HWFC,
	.parity = NRFX_UART_DEFAULT_CONFIG_PARITY,
	.baudrate = NRFX_UART_DEFAULT_CONFIG_BAUDRATE,
	.interrupt_priority = NRFX_UART_DEFAULT_CONFIG_IRQ_PRIORITY
};

int main(void)
{
	if(NRFX_SUCCESS != nrfx_uarte_init(&uart0, &uart0_config, NULL))
		while(1) { /* endless */ };

	printf("\r\nUART example started.\r\n");

	uint32_t c = 0;
	while (1) {
		printf("-- %lu\r\n", c);
		++c;
	};
}

