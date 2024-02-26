#include <string.h>
#include "nrf.h"
#include "timer.h"

TIMER_CALLBACK cb_timer1 = NULL;
TIMER_CALLBACK cb_timer2 = NULL;

// Initializes TIMER1 and TIMER2
void timer_init(void) {
  NRF_TIMER1->PRESCALER = 4;  // 1us period
  NRF_TIMER1->CC[0] = 0;
  NRF_TIMER1->INTENSET |= TIMER_INTENSET_COMPARE0_Msk;
  NRF_TIMER1->SHORTS |= TIMER_SHORTS_COMPARE0_STOP_Msk;

  NRF_TIMER2->PRESCALER = 4;  // 1us period
  NRF_TIMER2->CC[0] = 0;
  NRF_TIMER2->INTENSET |= TIMER_INTENSET_COMPARE0_Msk;
  NRF_TIMER2->SHORTS |= TIMER_SHORTS_COMPARE0_STOP_Msk;
}

// Starts a timer with microseconds
void timer_start_us(timer_id_t timer, uint32_t microseconds) {
  if (timer == TIMER_1) {
    NRF_TIMER1->CC[0] = microseconds;
    NRF_TIMER1->TASKS_CLEAR = 1;
    NRF_TIMER1->TASKS_START = 1;
  } else if (timer == TIMER_2) {
    NRF_TIMER2->CC[0] = microseconds;
    NRF_TIMER2->TASKS_CLEAR = 1;
    NRF_TIMER2->TASKS_START = 1;
  }
}

// Starts a timer with milliseconds
void timer_start_ms(timer_id_t timer, uint32_t milliseconds) {
  timer_start_us(timer, milliseconds * 1000);
}

// Stops a timer
void timer_stop(timer_id_t timer) {
  if (timer == TIMER_1) {
    NRF_TIMER1->TASKS_STOP = 1;
  } else if (timer == TIMER_2) {
    NRF_TIMER2->TASKS_STOP = 1;
  }
}

// Registers a timer callback (and enables the corresponding timer ISR)
int timer_cb_register(timer_id_t timer, TIMER_CALLBACK cb) {
  int ret = 0;
  if (timer == TIMER_1) {
    cb_timer1 = cb;
    NRF_TIMER1->EVENTS_COMPARE[0] = 0;
    NVIC_EnableIRQ(TIMER1_IRQn);
  } else if (timer == TIMER_2) {
    cb_timer2 = cb;
    NRF_TIMER2->EVENTS_COMPARE[0] = 0;
    NVIC_EnableIRQ(TIMER2_IRQn);
  } else {
    ret = -1;
  }

  return ret;
}

// Unregisters a timer callback (and disables the corresponding timer ISR)
int timer_cb_unregister(timer_id_t timer) {
  int ret = 0;
  if (timer == TIMER_1) {
    cb_timer1 = NULL;
    NVIC_DisableIRQ(TIMER1_IRQn);
  } else if (timer == TIMER_2) {
    cb_timer2 = NULL;
    NVIC_DisableIRQ(TIMER2_IRQn);
  } else {
    ret = -1;
  }

  return ret;
}

// TIMER1 ISR
void TIMER1_IRQHandler(void) {
  if ((NRF_TIMER1->EVENTS_COMPARE[0] == 1) && (NRF_TIMER1->INTENSET & TIMER_INTENSET_COMPARE0_Msk)) {
    NRF_TIMER1->EVENTS_COMPARE[0] = 0;
    if (cb_timer1 != NULL)  // note: should always be true
      cb_timer1();
  }
}

// TIMER2 ISR
void TIMER2_IRQHandler(void) {
  if ((NRF_TIMER2->EVENTS_COMPARE[0] == 1) && (NRF_TIMER2->INTENSET & TIMER_INTENSET_COMPARE0_Msk)) {
    NRF_TIMER2->EVENTS_COMPARE[0] = 0;
    if (cb_timer2 != NULL)  // note: should always be true
      cb_timer2();
  }
}
