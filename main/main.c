#include <sys/cdefs.h>
#include <esp_log.h>
#include <hal/timer_types.h>
#include <esp_spp_api.h>
#include <hal/gpio_hal.h>
#include "freertos/FreeRTOS.h"
#include <freertos/semphr.h>
#include <freertos/task.h>
#include "freertos/queue.h"
#include "hal/gpio_types.h"
#include "btc_driver.h"
#include "timer_driver.h"
#include "gpio_driver.h"

bool generic_timer_callback(void* args);

static timer_handler tmr_handler = BASIC_TIMER(0, 0, 2, 500, generic_timer_callback);
static SemaphoreHandle_t tmr_semaphore;

/// \brief Callback for the High Precision Timer
/// \param args Timer info
void IRAM_ATTR timer_callback(void* args)
{
    xSemaphoreGiveFromISR(tmr_semaphore, NULL);
}

/// \brief Task to handle timer interrupts
_Noreturn void task_timer_handler()
{
    gpio_dev_t* gpio = GPIO_HAL_GET_HW(GPIO_PORT_0);
    uint8_t gpio_level = 0;

    for(;;)
    {
        //Wait for timer semaphore
        xSemaphoreTake(tmr_semaphore, portMAX_DELAY);

        //Toggle gpio level
        gpio_level ^= 1;

        //Test HAL functions vs lower level gpio access
        //gpio_set_level(GPIO_NUM_13,gpio_level);

        //If high level, write to the w1ts (write 1 to set) register, otherwise write to w1tc (write 1 to clear)
        if (gpio_level)
            gpio->out_w1ts = (1 << GPIO_NUM_13);
        else gpio->out_w1tc = (1 << GPIO_NUM_13);

        //TODO: If it's not fast enough, maybe try inline assembly
    }
    vTaskDelete(NULL);
}


/// \brief Bluetooth data receive callback
/// \param data Structure containing bluetooth reception data
void bt_on_rec(esp_spp_cb_param_t* data)
{
    uint64_t period;
    //TODO: Improve conversion and data reception, maybe redirect data to a queue and a task
    sscanf((char*)data->data_ind.data, "p=%lld", &period);

    //If using High Precision Timer minimum period is 50us
    if (period < 2)
        return;

    //Un-Comment for High Precision Timer use

    /*
    esp_timer_stop(tmr_handler);
    esp_timer_start_periodic(tmr_handler,period);
     */

    timer_change_alarm(&tmr_handler, period);

    ESP_LOGI(BT_TAG, "Period changed to: %lld", period);
}

/// \brief Bluetooth callback when a device gets connected
void bt_on_con()
{
    ESP_LOGI(BT_TAG, "Device Connected");
}

bool IRAM_ATTR generic_timer_callback(void* ignored)
{
    BaseType_t awoke_task = pdFALSE;
    xSemaphoreGiveFromISR(tmr_semaphore, &awoke_task);
    return awoke_task == pdFALSE;
}


void app_main(void)
{
    config_gpio();

    config_bluetooth(bt_on_rec, bt_on_con);

    tmr_semaphore = xSemaphoreCreateBinary();

    timer_create(&tmr_handler);

    if(xTaskCreate(task_timer_handler, "timer_cb_handler", configMINIMAL_STACK_SIZE * 4, NULL, 2, NULL) != pdPASS)
        ESP_LOGE("TASK", "Error creating task");

    //Un-Comment for High Precision Timer use

    /*
    esp_timer_create_args_t configs =
            {
                    .callback = timer_callback,
                    .arg = NULL,
                    .dispatch_method = ESP_TIMER_TASK,
            };
    esp_timer_create(&configs, &tmr_handler);
    esp_timer_start_periodic(tmr_handler,1000000);
     */


}

