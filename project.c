
#include <nrf_gpio.h>
#include <nrf_802154.h>
#include "nrfx_uart.h"
#include <stdint.h>
#include <stdio.h>
#include <strings.h>

#define MAX_MESSAGE_SIZE 16
#define CHANNEL          23

static volatile bool m_tx_done;

void nop(void)
{
	__asm__ __volatile__("nop":::);
}

const uint32_t led1 = NRF_GPIO_PIN_MAP(0,13);
const uint32_t led2 = NRF_GPIO_PIN_MAP(0,14);
const uint32_t led3 = NRF_GPIO_PIN_MAP(0,15);
const uint32_t led4 = NRF_GPIO_PIN_MAP(0,16);


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
	uint8_t message[MAX_MESSAGE_SIZE];

	bzero(message, sizeof(message));
	message[0] = 0x41;                // Set MAC header: short addresses, no ACK
	message[1] = 0x98;                // Set MAC header

	nrf_802154_init();
	nrf_802154_channel_set(CHANNEL);
	nrf_802154_receive();

	nrf_gpio_cfg_output(led1);
	nrf_gpio_pin_clear(led1);
	nrf_gpio_cfg_output(led2);
	nrf_gpio_pin_clear(led2);
	nrf_gpio_cfg_output(led3);
	nrf_gpio_pin_clear(led3);
	nrf_gpio_cfg_output(led4);
	nrf_gpio_pin_clear(led4);

	if(NRFX_SUCCESS != nrfx_uart_init(&uart0, &uart_config, NULL))
		while(1) { /* endless */ };

	printf("\r\nstarting\r\n");

	uint32_t packet_id = 0;

	while(1) {
		nrf_gpio_pin_toggle(led1);

		message[2] = 0xff & ( packet_id >> 24 );
		message[3] = 0xff & ( packet_id >> 16 );
		message[4] = 0xff & ( packet_id >> 8 );
		message[5] = 0xff & ( packet_id >> 0 );

		printf("tx starting transmit %lu ... ", packet_id);
		m_tx_done = false;
		unsigned delay_loops = 0;
		if(nrf_802154_transmit(message, sizeof(message), true)) {
			packet_id++;

			while(!m_tx_done && (delay_loops < 4)) {
				nrf_gpio_pin_set(led2);
				for(uint32_t i = 0; i < 0x50000; ++i)
					nop();
				++delay_loops;
				nrf_gpio_pin_clear(led2);
			}
			if(m_tx_done)
				printf(" finished.\r\n");
			else
				printf(" TIMEOUT!\r\n");
		} else {
			printf(" TX FAILED!\r\n");
		}
	}
}

void nrf_802154_transmitted(const uint8_t * p_frame, uint8_t * p_ack, uint8_t length, int8_t power, uint8_t lqi)
{
	(void) p_frame;
	(void) length;
	(void) power;
	(void) lqi;

	m_tx_done = true;

	nrf_gpio_pin_toggle(led3);
	if (p_ack != NULL)
	{
		nrf_802154_buffer_free(p_ack);
		nrf_gpio_pin_toggle(led4);
	}
}

