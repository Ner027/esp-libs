#ifndef TIMER_DRIVER_H
#define TIMER_DRIVER_H

#include <esp_err.h>
#include <hal/timer_types.h>
#include <driver/timer.h>
#include <esp_log.h>

#define TIMER_TAG "TMR"

//Most boards use APB Bus clock which runs at 80Mhz
#define TMR_CLK 80000000

/// \brief Macro that helps with the creation of a basic timer that will trigger a callback "cb" every "a" milliseconds
/// \param i Timer ID, a value between 0 and 1 which identifies the timer inside a timer group
/// \param grp Timer Group, a value between 0 and 1 which identifies the timer group
/// \param p Clock Divider, a value between 2 and 65536, Tick Time= SRC_CLK / P, lower Clock Divider result in higher resolution
/// \param a Alarm Value, Value in milliseconds that defines the rate at which interrupts will be generated
/// \param cb Callback, function which is called when the timer alarm is triggered
#define BASIC_TIMER(i, grp, p, a, cb)       \
{                                           \
    .alarm = TIMER_ALARM_EN,                \
    .state = TIMER_START,                   \
    .intr_type = TIMER_INTR_LEVEL,          \
    .autoreload = TIMER_AUTORELOAD_EN,      \
    .reload_value = 0,                      \
    .id = i,                                \
    .group = grp,                           \
    .callback = cb,                         \
    .psc = p,                               \
    .alarm_value = a                        \
}

/*
 * @brief Generic Timer handler, contains all needed timer configurations and info
 */
struct timer_handler_t
{
    timer_idx_t id;
    timer_group_t group;
    timer_intr_mode_t intr_type;
    timer_start_t state;
    timer_alarm_t alarm;
    timer_autoreload_t autoreload;
    uint64_t reload_value;
    uint32_t psc;
    timer_isr_t callback;
    uint64_t alarm_value;
};
typedef struct timer_handler_t timer_handler;

///
/// \param handler Timer Handler structure containing the configurations for initializing the timer
void timer_create(timer_handler* handler);

///
/// \param handler Timer Handler structure that identifies the timer to stop
void timer_stop(timer_handler* handler);

///
/// \param handler Timer Handler structure that identifies the timer to resume
void timer_resume(timer_handler* handler);


///
/// \param handler Timer Handler structure that identifies the timer to edit
/// \param new_alarm New alarm value to use
void timer_change_alarm(timer_handler* handler, uint64_t new_alarm);

#endif