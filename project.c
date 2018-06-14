
#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_gpiote.h"
#include "nrf_gpio.h"
#include "nrfx_ppi.h"
#include "nrfx_timer.h"
#include "nrfx_gpiote.h"

#define GPIO_OUTPUT_PIN_NUMBER NRF_GPIO_PIN_MAP(0,13) // LED1

static nrfx_timer_t timer = NRFX_TIMER_INSTANCE(0);

void timer_dummy_handler(nrf_timer_event_t event_type, void * p_context)
{
	(void) event_type;
	(void) p_context;
}

static void led_blinking_setup()
{
	uint32_t compare_evt_addr;
	uint32_t gpiote_task_addr;
	nrf_ppi_channel_t ppi_channel;
	nrfx_err_t err_code;
	nrfx_gpiote_out_config_t config = NRFX_GPIOTE_CONFIG_OUT_TASK_TOGGLE(false);

	err_code = nrfx_gpiote_out_init(GPIO_OUTPUT_PIN_NUMBER, &config);

	nrfx_timer_extended_compare(&timer, (nrf_timer_cc_channel_t)0, 200 * 1000UL, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, false);

	err_code = nrfx_ppi_channel_alloc(&ppi_channel);

	compare_evt_addr = nrfx_timer_event_address_get(&timer, NRF_TIMER_EVENT_COMPARE0);
	gpiote_task_addr = nrfx_gpiote_out_task_addr_get(GPIO_OUTPUT_PIN_NUMBER);

	err_code = nrfx_ppi_channel_assign(ppi_channel, compare_evt_addr, gpiote_task_addr);

	err_code = nrfx_ppi_channel_enable(ppi_channel);

	nrfx_gpiote_out_task_enable(GPIO_OUTPUT_PIN_NUMBER);

	(void) err_code;
}

int main(void)
{
	nrfx_err_t err_code;

	err_code = nrfx_gpiote_init();

	nrfx_timer_config_t timer_cfg = NRFX_TIMER_DEFAULT_CONFIG;
	err_code = nrfx_timer_init(&timer, &timer_cfg, timer_dummy_handler);

	// Setup PPI channel with event from TIMER compare and task GPIOTE pin toggle.
	led_blinking_setup();

	// Enable timer
	nrfx_timer_enable(&timer);

	while (true)
	{
		// Do Nothing - GPIO can be toggled without software intervention.
	}

	(void) err_code;
}

