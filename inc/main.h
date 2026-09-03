#ifndef MAIN_H
#define MAIN_H


#include "gd32f4xx.h"
#include "ConfigurationUSARTn.h"
#include "stdint.h"
//#include "C:\Users\SVI\Desktop\GD32F4xx_Firmware_Library_V3.3.1\GD32F4xx_Firmware_Library_V3.3.1\GD32F4xx_Firmware_Library_V3.3.1\GD32F4xx_Firmware_Library\Firmware\GD32F4xx_standard_peripheral\Include\gd32f4xx_enet.h"
//#include "gd32f4xx_enet.h"
#include "gd32f4xx_enet_eval.h"
#include "init_config.h"
#include "protocol.h"

#include "timers.h"
//#define USE_DHCP       1 /* enable DHCP, if disabled static address is used */

//#define USE_ENET_INTERRUPT
//#define TIMEOUT_CHECK_USE_LWIP

#define SYSTEMTICK_PERIOD_MS 0

#define LOCAL_PORT  101
/* MAC address: BOARD_MAC_ADDR0:BOARD_MAC_ADDR1:BOARD_MAC_ADDR2:BOARD_MAC_ADDR3:BOARD_MAC_ADDR4:BOARD_MAC_ADDR5 */
#define BOARD_MAC_ADDR0   0x20
#define BOARD_MAC_ADDR1   0x40
#define BOARD_MAC_ADDR2   0x60
#define BOARD_MAC_ADDR3   0x80
#define BOARD_MAC_ADDR4   0xA0
#define BOARD_MAC_ADDR5   0xC0

#define LOCAL_MAC_ADDR0   0x20
#define LOCAL_MAC_ADDR1   0x40
#define LOCAL_MAC_ADDR2   0x60
#define LOCAL_MAC_ADDR3   0x80
#define LOCAL_MAC_ADDR4   0xA0
#define LOCAL_MAC_ADDR5   0xC0
 
/* static IP address: BOARD_IP_ADDR0.BOARD_IP_ADDR1.BOARD_IP_ADDR2.BOARD_IP_ADDR3 */
#define BOARD_IP_ADDR0   192
#define BOARD_IP_ADDR1   168
#define BOARD_IP_ADDR2   17
#if    (BOARD_MAC_ADDR5 ==  0xC2)
#define BOARD_IP_ADDR3   232
#elif  (BOARD_MAC_ADDR5  == 0xC1)
#define BOARD_IP_ADDR3   231
#elif  (BOARD_MAC_ADDR5  == 0xC0)
#define BOARD_IP_ADDR3   230
#else
 #error
#endif
/* net mask */
#define BOARD_NETMASK_ADDR0   255
#define BOARD_NETMASK_ADDR1   255
#define BOARD_NETMASK_ADDR2   255
#define BOARD_NETMASK_ADDR3   0

/* gateway address */
#define BOARD_GW_ADDR0   192
#define BOARD_GW_ADDR1   168
#define BOARD_GW_ADDR2   17
#define BOARD_GW_ADDR3   1


#define YV_PORT   200
#define YV_ADDR0  192
#define YV_ADDR1  168
#define YV_ADDR2  17
#define YV_ADDR3  246

#define KASU_KP_PORT  15000
#define KASU_KP_ADDR0 192
#define KASU_KP_ADDR1 168
#define KASU_KP_ADDR2 17
#define KASU_KP_ADDR3 25
/* MII and RMII mode selection */
#define RMII_MODE  // user have to provide the 50 MHz clock by soldering a 50 MHz oscillator
//#define MII_MODE
/* clock the PHY from external 25MHz crystal (only for MII mode) */
#ifdef  MII_MODE
#define PHY_CLOCK_MCO
#endif

//================================================================================
//================================================================================

#define  DMA_TX_USART
#define  USART_RX_BUFFER_SIZE       (200)
//================================================================================
/* function declarations */

void udp_send_buf         (void * buf,  uint32_t size, const ip_addr_t *addr, u16_t port);

#define TEST_UDP


#if (defined (TEST_UDP))
void Check_message_YV(void  *message,const ip_addr_t *addr, uint16_t port);
#else
void Check_message_YV         (  void  *message);
#endif

void Check_message_UART       (uint8_t *message, uint8_t size);
void Check_message_BKD        (uint8_t *message, uint8_t size);
void Check_message_BKHO_A     (uint8_t *message, uint8_t size);
void Check_message_OUT_SERVO  (uint8_t *message, uint8_t size);
void Qwerty_all_Device        (void);

void USART0_IRQHandler        (void);
void USART5_IRQHandler        (void);
void DMA1_Channel6_IRQHandler (void); //    tx uart5
void DMA1_Channel7_IRQHandler (void); //    tx uart0

void TIMER1_IRQHandler(void);
void TIMER2_IRQHandler(void);
void TIMER3_IRQHandler(void);
void TIMER4_IRQHandler(void);
void TIMER6_IRQHandler(void);

const static unsigned char Crc8Table[256] = {
    0x00, 0x31, 0x62, 0x53, 0xC4, 0xF5, 0xA6, 0x97,
    0xB9, 0x88, 0xDB, 0xEA, 0x7D, 0x4C, 0x1F, 0x2E,
    0x43, 0x72, 0x21, 0x10, 0x87, 0xB6, 0xE5, 0xD4,
    0xFA, 0xCB, 0x98, 0xA9, 0x3E, 0x0F, 0x5C, 0x6D,
    0x86, 0xB7, 0xE4, 0xD5, 0x42, 0x73, 0x20, 0x11,
    0x3F, 0x0E, 0x5D, 0x6C, 0xFB, 0xCA, 0x99, 0xA8,
    0xC5, 0xF4, 0xA7, 0x96, 0x01, 0x30, 0x63, 0x52,
    0x7C, 0x4D, 0x1E, 0x2F, 0xB8, 0x89, 0xDA, 0xEB,
    0x3D, 0x0C, 0x5F, 0x6E, 0xF9, 0xC8, 0x9B, 0xAA,
    0x84, 0xB5, 0xE6, 0xD7, 0x40, 0x71, 0x22, 0x13,
    0x7E, 0x4F, 0x1C, 0x2D, 0xBA, 0x8B, 0xD8, 0xE9,
    0xC7, 0xF6, 0xA5, 0x94, 0x03, 0x32, 0x61, 0x50,
    0xBB, 0x8A, 0xD9, 0xE8, 0x7F, 0x4E, 0x1D, 0x2C,
    0x02, 0x33, 0x60, 0x51, 0xC6, 0xF7, 0xA4, 0x95,
    0xF8, 0xC9, 0x9A, 0xAB, 0x3C, 0x0D, 0x5E, 0x6F,
    0x41, 0x70, 0x23, 0x12, 0x85, 0xB4, 0xE7, 0xD6,
    0x7A, 0x4B, 0x18, 0x29, 0xBE, 0x8F, 0xDC, 0xED,
    0xC3, 0xF2, 0xA1, 0x90, 0x07, 0x36, 0x65, 0x54,
    0x39, 0x08, 0x5B, 0x6A, 0xFD, 0xCC, 0x9F, 0xAE,
    0x80, 0xB1, 0xE2, 0xD3, 0x44, 0x75, 0x26, 0x17,
    0xFC, 0xCD, 0x9E, 0xAF, 0x38, 0x09, 0x5A, 0x6B,
    0x45, 0x74, 0x27, 0x16, 0x81, 0xB0, 0xE3, 0xD2,
    0xBF, 0x8E, 0xDD, 0xEC, 0x7B, 0x4A, 0x19, 0x28,
    0x06, 0x37, 0x64, 0x55, 0xC2, 0xF3, 0xA0, 0x91,
    0x47, 0x76, 0x25, 0x14, 0x83, 0xB2, 0xE1, 0xD0,
    0xFE, 0xCF, 0x9C, 0xAD, 0x3A, 0x0B, 0x58, 0x69,
    0x04, 0x35, 0x66, 0x57, 0xC0, 0xF1, 0xA2, 0x93,
    0xBD, 0x8C, 0xDF, 0xEE, 0x79, 0x48, 0x1B, 0x2A,
    0xC1, 0xF0, 0xA3, 0x92, 0x05, 0x34, 0x67, 0x56,
    0x78, 0x49, 0x1A, 0x2B, 0xBC, 0x8D, 0xDE, 0xEF,
    0x82, 0xB3, 0xE0, 0xD1, 0x46, 0x77, 0x24, 0x15,
    0x3B, 0x0A, 0x59, 0x68, 0xFF, 0xCE, 0x9D, 0xAC
};
unsigned char Crc8(unsigned char *pcBlock, unsigned char size);

/* updates the system local time */
void time_update(void);
/* insert a delay time */
void delay_10ms(uint32_t ncount);

#endif /* MAIN_H */
