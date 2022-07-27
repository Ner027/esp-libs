#include "gpio_driver.h"

void config_gpio()
{
    gpio_config_t config =
            {
                    .pin_bit_mask = 1 << GPIO_NUM_13,
                    .mode = GPIO_MODE_OUTPUT,
                    .pull_up_en = GPIO_PULLUP_DISABLE,
                    .pull_down_en = GPIO_PULLDOWN_DISABLE,
                    .intr_type = GPIO_INTR_DISABLE
            };

    esp_err_t ecode = gpio_config(&config);

    if(ecode != ESP_OK)
        ESP_LOGE(GPIO_TAG, "Error initializing GPIO, exited with error %d", ecode);
    else ESP_LOGI(GPIO_TAG, "GPIO initialized with success");
}
