
#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_gpiote.h"
#include "nrf_gpio.h"
#include "nrfx_ppi.h"
#include "nrfx_timer.h"
#include "nrfx_gpiote.h"

#define PIN_BUTTON1 NRF_GPIO_PIN_MAP(0,11)
#define PIN_BUTTON2 NRF_GPIO_PIN_MAP(0,12)
#define PIN_BUTTON3 NRF_GPIO_PIN_MAP(0,24)
#define PIN_BUTTON4 NRF_GPIO_PIN_MAP(0,25)
#define PIN_LED1    NRF_GPIO_PIN_MAP(0,13)
#define PIN_LED2    NRF_GPIO_PIN_MAP(0,14)
#define PIN_LED3    NRF_GPIO_PIN_MAP(0,15)
#define PIN_LED4    NRF_GPIO_PIN_MAP(0,16)

static nrfx_timer_t timer = NRFX_TIMER_INSTANCE(0);

void timer_dummy_handler(nrf_timer_event_t event_type, void * p_context)
{
	(void) event_type;
	(void) p_context;
}

// Setup PPI channel with event from TIMER compare and task GPIOTE pin toggle on LED4.
static void led_blinking_setup()
{
	uint32_t compare_evt_addr;
	uint32_t gpiote_task_addr;
	nrf_ppi_channel_t ppi_channel;
	nrfx_err_t err_code;
	nrfx_gpiote_out_config_t config = NRFX_GPIOTE_CONFIG_OUT_TASK_TOGGLE(false);

	err_code = nrfx_gpiote_out_init(PIN_LED4, &config);

	nrfx_timer_extended_compare(&timer, (nrf_timer_cc_channel_t)0, 200 * 1000UL, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, false);

	err_code = nrfx_ppi_channel_alloc(&ppi_channel);

	compare_evt_addr = nrfx_timer_event_address_get(&timer, NRF_TIMER_EVENT_COMPARE0);
	gpiote_task_addr = nrfx_gpiote_out_task_addr_get(PIN_LED4);

	err_code = nrfx_ppi_channel_assign(ppi_channel, compare_evt_addr, gpiote_task_addr);

	err_code = nrfx_ppi_channel_enable(ppi_channel);

	nrfx_gpiote_out_task_enable(PIN_LED4);

	(void) err_code;
}

void buttonEventHandler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	(void) action;
	if(PIN_BUTTON1 == pin) {
		if(nrfx_gpiote_in_is_set(PIN_BUTTON1))
			nrfx_gpiote_out_clear(PIN_LED1);
		else
			nrfx_gpiote_out_set(PIN_LED1);
	}
}

static void button_setup()
{
	// LED1 as output (active low)
	nrfx_gpiote_out_config_t led1Cfg;
	led1Cfg.init_state = NRF_GPIOTE_INITIAL_VALUE_LOW;
	led1Cfg.task_pin = false;
	if(NRFX_SUCCESS != nrfx_gpiote_out_init(PIN_LED1, &led1Cfg)) {
		while(1) { /* nil */ }
	}

	// button1 as input
	nrfx_gpiote_in_config_t buttonCfg;
	buttonCfg.sense = NRF_GPIOTE_POLARITY_TOGGLE;
	buttonCfg.pull = NRF_GPIO_PIN_PULLUP;
	buttonCfg.is_watcher = false;
	buttonCfg.hi_accuracy = true;
	buttonCfg.skip_gpio_setup = false;

	if(NRFX_SUCCESS != nrfx_gpiote_in_init( PIN_BUTTON1,
						&buttonCfg,
						buttonEventHandler )) {
		while(1) { /* nil */ }
	}
	nrfx_gpiote_in_event_enable(PIN_BUTTON1, true);
}

int main(void)
{
	nrfx_gpiote_init();

	// use interrupt and callback to change output
	button_setup();

	//blink using PPI channel and timer
	led_blinking_setup();

	// Enable timer
	nrfx_timer_config_t timer_cfg = NRFX_TIMER_DEFAULT_CONFIG;
	nrfx_timer_init(&timer, &timer_cfg, timer_dummy_handler);
	nrfx_timer_enable(&timer);

	while (true)
	{ /* nil */ }
}

