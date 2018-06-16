
#include <stdint.h>
#include <stdio.h>

#include "nrfx_uart.h"
#include "nrf_gpio.h"

const nrfx_uart_config_t uart_config = {
	.pseltxd = NRF_GPIO_PIN_MAP(0,6),
	.pselrxd = NRF_GPIO_PIN_MAP(0,8),
	.pselcts = NRF_UART_PSEL_DISCONNECTED,
	.pselrts = NRF_UART_PSEL_DISCONNECTED,
	.hwfc = NRFX_UART_DEFAULT_CONFIG_HWFC,
	.parity = NRFX_UART_DEFAULT_CONFIG_PARITY,
	.baudrate = NRFX_UART_DEFAULT_CONFIG_BAUDRATE,
	.interrupt_priority = NRFX_UART_DEFAULT_CONFIG_IRQ_PRIORITY
};

nrfx_uart_t uart0 = NRFX_UART_INSTANCE(0);

int main(void)
{
	nrfx_err_t err_code;

	if(NRFX_SUCCESS != nrfx_uart_init(&uart0, &uart_config, NULL))
		while(1) { /* endless */ };

	printf("\r\nUART example started.\r\n");

	uint32_t c = 0;
	while (1) {
		printf("-- %lu\r\n", c);
		++c;
	};
}

