#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"


#define LED_GPIO_PIN 2
#define BUTTON_GPIO_PIN 0

// Sofware state flag
static bool led_enable = true;

//--- Led task---

void led_blink_task(void *pvParameter) {
	while (1) {
		if (led_enable) {
	// Turn on the led
			gpio_set_level(LED_GPIO_PIN, 1);
			vTaskDelay(pdMS_TO_TICKS(1000));
	//Turn of the led
			gpio_set_level(LED_GPIO_PIN, 0);
			vTaskDelay(pdMS_TO_TICKS(1000));
		}
		else {
	// Turn on the led
			gpio_set_level(LED_GPIO_PIN, 0);
			vTaskDelay(pdMS_TO_TICKS(1000));
		}
	}
}

//--- Button task ---
void button_task(void *pvParameter) {
	bool last_state = 1;
	while(1) {
		bool current_state = gpio_get_level(BUTTON_GPIO_PIN);
		if (last_state == 1 && current_state == 0) {
			led_enable = !led_enable;
		}
		last_state = current_state;
		vTaskDelay(pdMS_TO_TICKS(50));

	}
}

void app_main(void){
	// configure gpio
	gpio_config_t io_conf = {
		.pin_bit_mask = (1ULL << LED_GPIO_PIN),
		.mode = GPIO_MODE_OUTPUT,
		.pull_up_en = GPIO_PULLUP_ENABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_DISABLE
	};
gpio_config(&io_conf); // Apply this configuration
// Blink loop
	gpio_config_t button_config = {
		.pin_bit_mask = (1ULL << BUTTON_GPIO_PIN),
		.mode = GPIO_MODE_INPUT,
		.pull_up_en = GPIO_PULLUP_ENABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_DISABLE
	};
gpio_config(&button_config); // Apply this configuration

// --- Create the task ---
xTaskCreate(led_blink_task, "LED_task", 2048, NULL, 5,NULL);
xTaskCreate(button_task, "BUTTON_task", 2048, NULL, 5,NULL);
}