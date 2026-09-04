#ifndef MAIN_H
#define MAIN_H


#include "gd32f4xx.h"

#include "ConfigurationUSARTn.h"
#include "init_config.h"
#include "ethernet.h"
#include "uart.h"
#include "protocol.h"
#include <string.h> 

//#include "stdint.h"
//#include "gd32f4xx_enet_eval.h"

//#define USE_DHCP       1 /* enable DHCP, if disabled static address is used */

//#define USE_ENET_INTERRUPT
//#define TIMEOUT_CHECK_USE_LWIP

#define SYSTEMTICK_PERIOD_MS 0


/* MII and RMII mode selection */
#define RMII_MODE  // user have to provide the 50 MHz clock by soldering a 50 MHz oscillator
//#define MII_MODE
/* clock the PHY from external 25MHz crystal (only for MII mode) */
#ifdef  MII_MODE
#define PHY_CLOCK_MCO
#endif

//================================================================================
// ======================== Debugs ========================

//================================================================================
/* function declarations */

void TIMER1_IRQHandler(void);
void TIMER2_IRQHandler(void);
void TIMER3_IRQHandler(void);
void TIMER4_IRQHandler(void);
void TIMER6_IRQHandler(void);

/* updates the system local time */
void time_update(void);
/* insert a delay time */
void delay_10ms(uint32_t ncount);

#endif /* MAIN_H */
