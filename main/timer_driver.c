#include "timer_driver.h"

void timer_create(timer_handler* handler)
{
    timer_config_t config =
    {
            .alarm_en = handler->alarm,
            .auto_reload = handler->autoreload,
            .counter_dir = TIMER_COUNT_UP,
            .divider = handler->psc,
            .intr_type = handler->intr_type,
    };


    esp_err_t ecode = timer_init(handler->group, handler->id, &config);

    if (ecode != ESP_OK)
    {
        ESP_LOGE(TIMER_TAG, "An error occurred while initializing timer %d from group %d, exited with error: %d",
                 handler->id, handler->group, ecode);
        return;
    }

    ecode = timer_set_counter_value(handler->group, handler->id, handler->reload_value);

    if (ecode != ESP_OK)
    {
        ESP_LOGE(TIMER_TAG, "An error occurred while setting timer %d from group %d counter value, exited with error: %d",
                 handler->id, handler->group, ecode);
    }

    ecode = timer_set_alarm_value(handler->group, handler->id,
                                  (handler->alarm_value * ((TMR_CLK/1000) / handler->psc)));

    if (ecode != ESP_OK)
    {
        ESP_LOGE(TIMER_TAG, "An error occurred while setting timer %d from group %d alarm value, exited with error: %d",
                 handler->id, handler->group, ecode);
    }

    ecode = timer_isr_callback_add(handler->group, handler->id,
                           handler->callback, handler, 0);

    if (ecode != ESP_OK)
    {
        ESP_LOGE(TIMER_TAG, "An error occurred while setting up an interrupt on timer "
                            "%d from group %d, exited with error: %d", handler->id, handler->group, ecode);
        return;
    }

    //If all timer initializations were successfully start the timer
    timer_resume(handler);
}

void timer_stop(timer_handler* handler)
{
    //If timer is not running return
    if (handler->state == TIMER_START)
        return;

    //Try to stop the timer
    esp_err_t ecode = timer_pause(handler->group, handler->id);

    if (ecode != ESP_OK)
    {
        ESP_LOGE(TIMER_TAG, "An error occurred while stopping timer %d from group %d, exited with error: %d",
                 handler->group, handler->id, ecode);
        return;
    }

    //Update the handler state if timer stopped successfully
    handler->state = TIMER_PAUSE;
}

void timer_resume(timer_handler* handler)
{
    //If timer is already running return
    if (handler->state != TIMER_START)
        return;

    //Try to start the timer
    esp_err_t ecode = timer_start(handler->group, handler->id);

    if (ecode != ESP_OK)
    {
        ESP_LOGE(TIMER_TAG, "An error occurred while starting timer %d from group %d, exited with error: %d",
                 handler->group, handler->id, ecode);
        return;
    }

    //Update the handler state if timer started successfully
    handler->state = TIMER_START;
}

void timer_change_alarm(timer_handler* handler, uint64_t new_alarm)
{
    //Timers should be stopped before changing the alarm value, otherwise undefined behaviour may happen
    timer_stop(handler);
    timer_set_alarm_value(handler->group, handler->id,
                          handler->alarm_value * ((TMR_CLK/1000) / handler->psc));
    //After changing the alarm value, restart the timer
    timer_resume(handler);
}