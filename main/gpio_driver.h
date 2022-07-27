#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

#define GPIO_TAG "GPIO"

#include <hal/gpio_types.h>
#include <esp_err.h>
#include <esp_log.h>
#include <driver/gpio.h>

/// \brief Initialize needed GPIO pins
void config_gpio();

#endif