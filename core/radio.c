#include <string.h>
#include "nrf.h"

#include "radio.h"

RADIO_CALLBACK cb_disabled = NULL;
RADIO_CALLBACK cb_txready = NULL;
RADIO_CALLBACK cb_rxready = NULL;
RADIO_CALLBACK cb_crcok = NULL;
RADIO_CALLBACK cb_crcerr = NULL;
RADIO_CALLBACK cb_address = NULL;
RADIO_CALLBACK cb_end = NULL;

void radio_init() {
  // Clear all radio interrupts
  NRF_RADIO->INTENCLR = 0xFFFFFFFF;
  // Enable radio ISR
  NVIC_EnableIRQ(RADIO_IRQn);
}

void radio_start(radio_init_mode_t mode) {
  // Setup a PPI channel that connects the start event of the HF clock with the radio start-up
  NRF_PPI->CH[18].EEP = (uint32_t)&NRF_CLOCK->EVENTS_HFCLKSTARTED;
  if (mode == RADIO_INIT_MODE_RX) {
    NRF_PPI->CH[18].TEP = (uint32_t)&NRF_RADIO->TASKS_RXEN;
  } else {
    // TX by default
    NRF_PPI->CH[18].TEP = (uint32_t)&NRF_RADIO->TASKS_TXEN;
  }
  NRF_PPI->CHENSET = PPI_CHENSET_CH18_Msk;

  // Start the HF clock
  NRF_CLOCK->TASKS_HFCLKSTART = 1;
}

void radio_stop() {
  NRF_RADIO->TASKS_DISABLE = 1;
  NRF_CLOCK->TASKS_HFCLKSTOP = 1;
}

void radio_wait_stop_complete() {
  // Make sure HFXO has stopped so the next packet can be sent right after returning
  while ((NRF_CLOCK->HFCLKSTAT & CLOCK_HFCLKSTAT_SRC_Msk) == CLOCK_HFCLKSTAT_SRC_Xtal) {
  }
}

int radio_cb_register(radio_evt_t evt, RADIO_CALLBACK cb) {
  switch (evt) {
    case RADIO_EVT_DISABLED:
      cb_disabled = cb;
      NRF_RADIO->EVENTS_DISABLED = 0;
      NRF_RADIO->INTENSET = RADIO_INTENSET_DISABLED_Msk;
      break;
    case RADIO_EVT_RXREADY:
      cb_rxready = cb;
      NRF_RADIO->EVENTS_RXREADY = 0;
      NRF_RADIO->INTENSET = RADIO_INTENSET_RXREADY_Msk;
      break;
    case RADIO_EVT_TXREADY:
      cb_txready = cb;
      NRF_RADIO->EVENTS_TXREADY = 0;
      NRF_RADIO->INTENSET = RADIO_INTENSET_TXREADY_Msk;
      break;
    case RADIO_EVT_CRCOK:
      cb_crcok = cb;
      NRF_RADIO->EVENTS_CRCOK = 0;
      NRF_RADIO->INTENSET = RADIO_INTENSET_CRCOK_Msk;
      break;
    case RADIO_EVT_CRCERR:
      cb_crcerr = cb;
      NRF_RADIO->EVENTS_CRCERROR = 0;
      NRF_RADIO->INTENSET = RADIO_INTENSET_CRCERROR_Msk;
      break;
    case RADIO_EVT_ADDRESS:
      cb_address = cb;
      NRF_RADIO->EVENTS_ADDRESS = 0;
      NRF_RADIO->INTENSET = RADIO_INTENSET_ADDRESS_Msk;
      break;
    case RADIO_EVT_END:
      cb_end = cb;
      NRF_RADIO->EVENTS_END = 0;
      NRF_RADIO->INTENSET = RADIO_INTENSET_END_Msk;
      break;
    default:
      return -1;
  }
  return 0;
}

int radio_cb_unregister(radio_evt_t evt) {
  switch (evt) {
    case RADIO_EVT_DISABLED:
      cb_disabled = NULL;
      NRF_RADIO->INTENCLR = RADIO_INTENCLR_DISABLED_Msk;
      break;
    case RADIO_EVT_RXREADY:
      cb_rxready = NULL;
      NRF_RADIO->INTENCLR = RADIO_INTENCLR_RXREADY_Msk;
      break;
    case RADIO_EVT_TXREADY:
      cb_txready = NULL;
      NRF_RADIO->INTENCLR = RADIO_INTENCLR_TXREADY_Msk;
      break;
    case RADIO_EVT_CRCOK:
      cb_crcok = NULL;
      NRF_RADIO->INTENCLR = RADIO_INTENCLR_CRCOK_Msk;
      break;
    case RADIO_EVT_CRCERR:
      cb_crcerr = NULL;
      NRF_RADIO->INTENCLR = RADIO_INTENCLR_CRCERROR_Msk;
      break;
    case RADIO_EVT_ADDRESS:
      cb_address = NULL;
      NRF_RADIO->INTENCLR = RADIO_INTENCLR_ADDRESS_Msk;
      break;
    case RADIO_EVT_END:
      cb_end = NULL;
      NRF_RADIO->INTENCLR = RADIO_INTENCLR_END_Msk;
      break;
    default:
      return -1;
  }
  return 0;
}

void radio_cb_clear_all() {
  NRF_RADIO->INTENCLR = 0xFFFFFFFF;
  cb_disabled = NULL;
  cb_txready = NULL;
  cb_rxready = NULL;
  cb_crcok = NULL;
  cb_crcerr = NULL;
  cb_address = NULL;
  cb_end = NULL;
}

void RADIO_IRQHandler(void) {
  if ((NRF_RADIO->EVENTS_DISABLED == 1) && (NRF_RADIO->INTENSET & RADIO_INTENSET_DISABLED_Msk)) {
    NRF_RADIO->EVENTS_DISABLED = 0;
    if (cb_disabled != NULL)
      cb_disabled();
  }
  if ((NRF_RADIO->EVENTS_RXREADY == 1) && (NRF_RADIO->INTENSET & RADIO_INTENSET_RXREADY_Msk)) {
    NRF_RADIO->EVENTS_RXREADY = 0;
    if (cb_rxready != NULL)
      cb_rxready();
  }
  if ((NRF_RADIO->EVENTS_TXREADY == 1) && (NRF_RADIO->INTENSET & RADIO_INTENSET_TXREADY_Msk)) {
    NRF_RADIO->EVENTS_TXREADY = 0;
    if (cb_txready != NULL)
      cb_txready();
  }
  if ((NRF_RADIO->EVENTS_CRCOK == 1) && (NRF_RADIO->INTENSET & RADIO_INTENSET_CRCOK_Msk)) {
    NRF_RADIO->EVENTS_CRCOK = 0;
    if (cb_crcok != NULL)
      cb_crcok();
  }
  if ((NRF_RADIO->EVENTS_CRCERROR == 1) && (NRF_RADIO->INTENSET & RADIO_INTENSET_CRCERROR_Msk)) {
    NRF_RADIO->EVENTS_CRCERROR = 0;
    if (cb_crcerr != NULL)
      cb_crcerr();
  }
  if ((NRF_RADIO->EVENTS_ADDRESS == 1) && (NRF_RADIO->INTENSET & RADIO_INTENSET_ADDRESS_Msk)) {
    NRF_RADIO->EVENTS_ADDRESS = 0;
    if (cb_address != NULL)
      cb_address();
  }
  if ((NRF_RADIO->EVENTS_END == 1) && (NRF_RADIO->INTENSET & RADIO_INTENSET_END_Msk)) {
    NRF_RADIO->EVENTS_END = 0;
    if (cb_end != NULL)
      cb_end();
  }
}
