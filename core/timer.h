#ifndef __TIMER_H__
#define __TIMER_H__

typedef void (*TIMER_CALLBACK)(void);

typedef enum {
  TIMER_1,
  TIMER_2,
} timer_id_t;

/**
 * @brief Initializes the timer peripheral. Must be called before timer can be used.
 *
 */
void timer_init();

/**
 * @brief Starts the timer that triggers an interrupt after the specified microseconds.
 *
 */
void timer_start_us(timer_id_t timer, uint32_t microseconds);

/**
 * @brief Starts the timer that triggers an interrupt after the specified milliseconds.
 *
 */
void timer_start_ms(timer_id_t timer, uint32_t milliseconds);

/**
 * @brief Stops the timer.
 *
 */
void timer_stop(timer_id_t timer);

/**
 * @brief Registers a callback in the timer isr.
 *
 * @param timer Timer for which callback is registered
 * @param cb Pointer to callback function
 * @return int
 */
int timer_cb_register(timer_id_t timer, TIMER_CALLBACK cb);

/**
 * @brief Unregisters a previously registered callback.
 *
 * @param timer Timer for which callback was registered.
 * @return int
 */
int timer_cb_unregister(timer_id_t timer);

#endif /* __TIMER_H__ */
