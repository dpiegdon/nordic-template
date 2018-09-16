
#include <nrf_gpio.h>
#include <nrfx_gpiote.h>
#include <nrf_timer.h>
#include <nrfx_timer.h>


/* hardware setup + muxing */

// led
const uint32_t led = NRF_GPIO_PIN_MAP(0,13);
static const nrfx_gpiote_out_config_t led_config = {
	.init_state = GPIOTE_CONFIG_OUTINIT_Low,
	.task_pin = false
};

static nrfx_timer_t timer0 = NRFX_TIMER_INSTANCE(0);
static const nrfx_timer_config_t timer0_config = {
	.frequency = NRF_TIMER_FREQ_31250Hz,
	.mode = NRF_TIMER_MODE_TIMER,
	.bit_width = NRF_TIMER_BIT_WIDTH_16,
	.interrupt_priority = NRFX_TIMER_DEFAULT_CONFIG_IRQ_PRIORITY,
	.p_context = NULL
};

/* functions */


void timer0_callback(nrf_timer_event_t event_type, void * p_context)
{
	(void) p_context;

	switch(event_type) {
		case NRF_TIMER_EVENT_COMPARE0:
			nrfx_gpiote_out_toggle(led);
			break;
		default:
			break;
	};
}

int main(void)
{
	if(NRFX_SUCCESS != nrfx_gpiote_init())
		while(1) { /* endless */ };
	nrfx_gpiote_out_init(led, &led_config);

	if(NRFX_SUCCESS != nrfx_timer_init(&timer0, &timer0_config, timer0_callback))
		while(1) { /* endless */ };
	const uint32_t ticks = nrfx_timer_ms_to_ticks(&timer0, 100);
	nrfx_timer_extended_compare(&timer0,
					NRF_TIMER_CC_CHANNEL0,
					ticks,
					NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK,
					true);
	nrfx_timer_enable(&timer0);
	NRFX_STATIC_ASSERT(1);

	while(1) {
		__WFI();
	}
}

