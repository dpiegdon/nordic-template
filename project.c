
#include <nrf_gpio.h>
#include <nrf_802154.h>

#define MAX_MESSAGE_SIZE 16
#define CHANNEL          23

static volatile bool m_tx_in_progress;
static volatile bool m_tx_done;

void nop(void)
{
	__asm__ __volatile__("nop":::);
}

const uint32_t led1 = NRF_GPIO_PIN_MAP(0,13);
const uint32_t led2 = NRF_GPIO_PIN_MAP(0,14);
const uint32_t led3 = NRF_GPIO_PIN_MAP(0,15);
const uint32_t led4 = NRF_GPIO_PIN_MAP(0,16);

int main(void)
{
	uint8_t message[MAX_MESSAGE_SIZE];

	for (uint32_t i = 0; i < sizeof(message) / sizeof(message[0]); i++)
		message[i] = i;

	message[0] = 0x41;                // Set MAC header: short addresses, no ACK
	message[1] = 0x98;                // Set MAC header

	m_tx_in_progress = false;
	m_tx_done        = false;

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

	while(1) {
		nrf_gpio_pin_toggle(led1);
		if (m_tx_done)
		{
			m_tx_in_progress = false;
			m_tx_done        = false;
		}

		if (!m_tx_in_progress)
		{
			m_tx_in_progress = nrf_802154_transmit(message, sizeof(message), true);
			nrf_gpio_pin_toggle(led2);
		}

		for(uint32_t i = 0; i < 0x320000; ++i)
			nop();
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

