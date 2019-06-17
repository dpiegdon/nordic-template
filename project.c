
#include <nrfx_clock.h>
#include <nrf_gpio.h>

#include <FreeRTOS.h>
#include <task.h>

const uint32_t led1_pin = NRF_GPIO_PIN_MAP(0,13);
const uint32_t led2_pin = NRF_GPIO_PIN_MAP(0,14);
const uint32_t led3_pin = NRF_GPIO_PIN_MAP(0,15);
const uint32_t led4_pin = NRF_GPIO_PIN_MAP(0,16);

static void clockEventHandler(nrfx_clock_evt_type_t event)
{
	(void) event;
}

static void init_hardware(void)
{
	// setup RTC for FreeRTOS.
	// either systick or RTC can be used as sys-tick.
	// RTC consumes less power if tickless idle is chosen as well.
	nrfx_clock_init(clockEventHandler);
	nrf_clock_lf_src_set((nrf_clock_lfclk_t)NRFX_CLOCK_CONFIG_LF_SRC);
	nrfx_clock_lfclk_start();

	// setup LED pins of PCA10056
	nrf_gpio_cfg_output(led1_pin);
	nrf_gpio_cfg_output(led2_pin);
	nrf_gpio_cfg_output(led3_pin);
	nrf_gpio_cfg_output(led4_pin);
}

static void ledFlasherTask(void * led)
{
	vTaskDelay(16 * (0xf & (unsigned) led));
	while(1) {
		nrf_gpio_pin_toggle(*(const uint32_t *)led);
		vTaskDelay(configTICK_RATE_HZ/2);
	}
}

int main(void)
{
	TaskHandle_t ledFlasher1 = NULL;
	TaskHandle_t ledFlasher2 = NULL;
	TaskHandle_t ledFlasher3 = NULL;
	TaskHandle_t ledFlasher4 = NULL;

	BaseType_t ret;

	init_hardware();

	ret = xTaskCreate(ledFlasherTask, "led1", 128, (void*)&led1_pin, 3, &ledFlasher1);
	assert(ret == pdPASS);
	ret = xTaskCreate(ledFlasherTask, "led2", 128, (void*)&led2_pin, 3, &ledFlasher2);
	assert(ret == pdPASS);
	ret = xTaskCreate(ledFlasherTask, "led3", 128, (void*)&led3_pin, 2, &ledFlasher3);
	assert(ret == pdPASS);
	ret = xTaskCreate(ledFlasherTask, "led4", 128, (void*)&led4_pin, 1, &ledFlasher4);
	assert(ret == pdPASS);

	vTaskStartScheduler();
}

