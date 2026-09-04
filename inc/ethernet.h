#ifndef ETHERNET_H
#define ETHERNET_H


#include "gd32f4xx.h"
#include "lwip/udp.h"
#include "stdint.h"
#include "gd32f4xx_enet_eval.h"
#include "init_config.h"
#include "protocol.h"

/* MAC address: BOARD_MAC_ADDR0:BOARD_MAC_ADDR1:BOARD_MAC_ADDR2:BOARD_MAC_ADDR3:BOARD_MAC_ADDR4:BOARD_MAC_ADDR5 */
//#define BOARD_MAC_ADDR0   0x20
//#define BOARD_MAC_ADDR1   0x40
//#define BOARD_MAC_ADDR2   0x60
//#define BOARD_MAC_ADDR3   0x80
//#define BOARD_MAC_ADDR4   0xA0
//#define BOARD_MAC_ADDR5   0xC0

#define LOCAL_PORT  101
#define LOCAL_MAC_ADDR0   0x20
#define LOCAL_MAC_ADDR1   0x40
#define LOCAL_MAC_ADDR2   0x60
#define LOCAL_MAC_ADDR3   0x80
#define LOCAL_MAC_ADDR4   0xA0
#define LOCAL_MAC_ADDR5   0xC0
 
/* static IP address: BOARD_IP_ADDR0.BOARD_IP_ADDR1.BOARD_IP_ADDR2.BOARD_IP_ADDR3 */
#define LOCAL_IP_ADDR0   192
#define LOCAL_IP_ADDR1   168
#define LOCAL_IP_ADDR2   17
#if    (LOCAL_MAC_ADDR5 ==  0xC2)
#define LOCAL_IP_ADDR3   232
#elif  (LOCAL_MAC_ADDR5  == 0xC1)
#define LOCAL_IP_ADDR3   231
#elif  (LOCAL_MAC_ADDR5  == 0xC0)
#define LOCAL_IP_ADDR3   230
#else
 #error
#endif
/* net mask */
#define LOCAL_NETMASK_ADDR0   255
#define LOCAL_NETMASK_ADDR1   255
#define LOCAL_NETMASK_ADDR2   255
#define LOCAL_NETMASK_ADDR3   0

/* gateway address */
#define LOCAL_GW_ADDR0   192
#define LOCAL_GW_ADDR1   168
#define LOCAL_GW_ADDR2   17
#define LOCAL_GW_ADDR3   1


#define YV_PORT   200
#define YV_ADDR0  192
#define YV_ADDR1  168
#define YV_ADDR2  17
#define YV_ADDR3  246


// ======================== Debags ========================
#if (LOCAL_IP_ADDR3  == 232)
 #define RightLS
 //miso spi 2
 #define Blink_TX_PORT   GPIOB
 #define Blink_TX_PIN    GPIO_PIN_4
 //sck spi 2
 #define Blink_RX_PORT   GPIOB
 #define Blink_RX_PIN    GPIO_PIN_3
#elif (LOCAL_IP_ADDR3  == 231)
 #define LeftYLS
 //nss spi 1
 #define Blink_TX_PORT   GPIOE
 #define Blink_TX_PIN    GPIO_PIN_4
 //sck spi 1
 #define Blink_RX_PORT   GPIOE
 #define Blink_RX_PIN    GPIO_PIN_2
#elif (LOCAL_IP_ADDR3  == 230)
 #define LeftYLS
 //nss spi 1
 #define Blink_TX_PORT   GPIOE
 #define Blink_TX_PIN    GPIO_PIN_4
 //sck spi 1
 #define Blink_RX_PORT   GPIOE
 #define Blink_RX_PIN    GPIO_PIN_2
#else
 #error
#endif
// ======================== Debags ========================


/* function declarations */

 uint8_t udp_create_socket(void);

 void udp_reciev    (void *arg, struct udp_pcb *pcb, struct pbuf *p,  // приём данных по upd
                                const  ip_addr_t *addr, u16_t port); // reciev data on enet
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

/* updates the system local time */
void time_update(void);
/* insert a delay time */
void delay_10ms(uint32_t ncount);

#endif /* ETHERNET_H */
