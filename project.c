
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "nrfx_spim.h"
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

nrfx_spim_t spim0 = NRFX_SPIM_INSTANCE(0);
const nrfx_spim_config_t spim0_config = {
	.sck_pin = NRF_GPIO_PIN_MAP(0, 5),
	.mosi_pin = NRF_GPIO_PIN_MAP(0, 6),
	.miso_pin = NRF_GPIO_PIN_MAP(0, 7),
	.ss_pin = NRF_GPIO_PIN_MAP(0, 8),
	.ss_active_high = false,
	.irq_priority = NRFX_SPIM_DEFAULT_CONFIG_IRQ_PRIORITY,
	.orc = 0xFF,
	.frequency = NRF_SPIM_FREQ_1M,
	.mode = NRF_SPIM_MODE_3,
	.bit_order = NRF_SPIM_BIT_ORDER_MSB_FIRST,
	NRFX_SPIM_DEFAULT_EXTENDED_CONFIG
};

int main(void)
{
	if(NRFX_SUCCESS != nrfx_uarte_init(&uart0, &uart0_config, NULL))
		while(1) { /* endless */ };

	printf("\r\nUART ok.\r\n");

	if(NRFX_SUCCESS != nrfx_spim_init(&spim0, &spim0_config, NULL, NULL)) {
		printf("spi init failed\r\n");
		while(1) { };
	}

	while (1) {
		uint8_t txbuf[2];
		uint8_t rxbuf[2];

		// this reads the accelerometer data registers from an
		// MPU-9250 motion sensor and prints them on UART:
		for(int r = 0x3b; r <= 0x40; ++r) {
			const bool read = true;
			memset(txbuf, 0, sizeof(txbuf));
			txbuf[0] = (read?1:0)<<7 | r;
			nrfx_spim_xfer_desc_t xfer = NRFX_SPIM_SINGLE_XFER(
								txbuf, sizeof(txbuf),
								rxbuf, sizeof(rxbuf));
			if(NRFX_SUCCESS == nrfx_spim_xfer(&spim0, &xfer, 0)) {
				for(unsigned i = 1; i < sizeof(rxbuf); ++i)
					printf("r0x%02x=%02x ", r, rxbuf[1]);
			}
		}
		printf("\r\n");
	};
}

