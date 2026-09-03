#include "gd32f4xx.h"
#include "netconf.h"
#include "main.h"
//#include "lwip/tcp.h"
#include "lwip/timeouts.h"
#include "lwip/udp.h"
#include "lwip/ip_addr.h"
#include "lwip/init.h"
#include "netif/etharp.h"
#include "gd32f4xx_timer.h"
#include <string.h> 


// ======================== Debags ========================
#if (BOARD_IP_ADDR3  == 232)
 #define RightLS
 //miso spi 2
 #define Blink_TX_PORT   GPIOB
 #define Blink_TX_PIN    GPIO_PIN_4
 //sck spi 2
 #define Blink_RX_PORT   GPIOB
 #define Blink_RX_PIN    GPIO_PIN_3
#elif (BOARD_IP_ADDR3  == 231)
 #define LeftYLS
 //nss spi 1
 #define Blink_TX_PORT   GPIOE
 #define Blink_TX_PIN    GPIO_PIN_4
 //sck spi 1
 #define Blink_RX_PORT   GPIOE
 #define Blink_RX_PIN    GPIO_PIN_2
#elif (BOARD_IP_ADDR3  == 230)
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

// ======================== Debugs ========================

#define IN_TOTAL_BKD  (12)
#define DEVICE_IN_BKD (4*3)

  __IO uint32_t g_localtime = 0; /* for creating a time reference incremented by 10ms */
  uint32_t g_timedelay;

 struct  udp_pcb *_udp_pcb_;
 uint8_t udp_create_socket(void);

 void udp_reciev    (void *arg, struct udp_pcb *pcb, struct pbuf *p,  // приём данных по upd
                                const  ip_addr_t *addr, u16_t port); // reciev data on enet

ip_addr_t yv_ip;
ip_addr_t kasu_kp_ip;

 
volatile uint8_t usart5_transmit    = 0;
volatile uint8_t usart5_counter     = 0;
volatile uint8_t usart5_buffer_rx [USART_RX_BUFFER_SIZE] = {0};
volatile _Bool timer4En = 0;// usart5 rx end


volatile uint8_t usart0_transmit    = 0;
volatile uint8_t usart0_counter     = 0;
volatile uint8_t usart0_buffer_rx [USART_RX_BUFFER_SIZE] = {0};
volatile _Bool timer2En = 0;// usart0 rx end


/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none

ping 192.168.17.230 -t

  192.168.17.230 - 101 

  192.168.17.246 - 200

 255.255.255.0
*/

volatile YLSToYVPacket YLSToYVPacket1={0},YLSToYVPacket2={0};
volatile YLSToYVPacket YLSToYVPacketSlow1={0xFF},YLSToYVPacketSlow2={0xFF};

/// переменная для переключения пакетов отправки по ethernet
volatile uint8_t current_tick_trasmition = 0;

// буферы для отправки по usart
uint8_t message_from_BKD [IN_TOTAL_BKD+2][USART_BUFFER_TX_SIZE_YLS]={0};// IN_TOTAL_BKD+2 -> 12 бкд + бкхо + внешние сервоприводы

// счетчики* для адресации процессоров по usart
volatile uint8_t current_BKD                  = 0;
volatile uint8_t current_device_in_BKDx       = 1;
volatile uint8_t current_device_in_BKHO_A     = 1;
volatile uint8_t current_device_in_OUT_SERVO  = 1;
enum
{
  YKP1_BKD=1,
  YKP2_BKD=2,
  YKP3_BKD=3,
  
  YAZ1_BKD=4,
  YAZ2_BKD=5,
  YAZ3_BKD=6,
  
  YPS1_BKD=7,
  YPS2_BKD=8,
  YPS3_BKD=9,
  
  YLK_96_1_BKD=10,
  YLK_96_2_BKD=11,
  YLK_96_3_BKD=12,
  
//BKHO-A
    START_ADR_BKHO_A  = 1+DEVICE_IN_BKD*IN_TOTAL_BKD,
  YPS1_BKHO_A       = 1+DEVICE_IN_BKD*IN_TOTAL_BKD,
  YPS2_BKHO_A       = 2+DEVICE_IN_BKD*IN_TOTAL_BKD,
  YPS3_BKHO_A       = 3+DEVICE_IN_BKD*IN_TOTAL_BKD,
  
  YTP1_1_BKHO_A     = 4+DEVICE_IN_BKD*IN_TOTAL_BKD,
  YTP1_2_BKHO_A     = 5+DEVICE_IN_BKD*IN_TOTAL_BKD,
  YTP1_3_BKHO_A     = 6+DEVICE_IN_BKD*IN_TOTAL_BKD,
  
  YTP2_1_BKHO_A     = 7+DEVICE_IN_BKD*IN_TOTAL_BKD,
  YTP2_2_BKHO_A     = 8+DEVICE_IN_BKD*IN_TOTAL_BKD,
  YTP2_3_BKHO_A     = 9+DEVICE_IN_BKD*IN_TOTAL_BKD,
  
  YTM1_1_BKHO_A     = 10+DEVICE_IN_BKD*IN_TOTAL_BKD,
  YTM1_2_BKHO_A     = 11+DEVICE_IN_BKD*IN_TOTAL_BKD,
  YTM1_3_BKHO_A     = 12+DEVICE_IN_BKD*IN_TOTAL_BKD,
  
  YTM2_1_BKHO_A     = 13+DEVICE_IN_BKD*IN_TOTAL_BKD,
  YTM2_2_BKHO_A     = 14+DEVICE_IN_BKD*IN_TOTAL_BKD,
  YTM2_3_BKHO_A     = 15+DEVICE_IN_BKD*IN_TOTAL_BKD,
  
  YGR1_BKHO_A       = 16+DEVICE_IN_BKD*IN_TOTAL_BKD,
  YGR2_BKHO_A       = 17+DEVICE_IN_BKD*IN_TOTAL_BKD,
  YGR3_BKHO_A       = 18+DEVICE_IN_BKD*IN_TOTAL_BKD,
  
  YLK_32_3_1_BKHO_A = 19+DEVICE_IN_BKD*IN_TOTAL_BKD,
  YLK_32_3_2_BKHO_A = 20+DEVICE_IN_BKD*IN_TOTAL_BKD,
  YLK_32_3_3_BKHO_A = 21+DEVICE_IN_BKD*IN_TOTAL_BKD,
  
  YVP1_BKHO_A       = 22+DEVICE_IN_BKD*IN_TOTAL_BKD,
  YVP2_BKHO_A       = 23+DEVICE_IN_BKD*IN_TOTAL_BKD,
  YVP3_BKHO_A       = 24+DEVICE_IN_BKD*IN_TOTAL_BKD,
    AND_ADR_BKHO_A    = 24+DEVICE_IN_BKD*IN_TOTAL_BKD,
    
//OUT_SERVO  
    START_ADR_OUT_SERVO = 25+DEVICE_IN_BKD*IN_TOTAL_BKD,
  OUT_SERVO_1     = 25+DEVICE_IN_BKD*IN_TOTAL_BKD,
  OUT_SERVO_2     = 26+DEVICE_IN_BKD*IN_TOTAL_BKD,
  OUT_SERVO_3     = 27+DEVICE_IN_BKD*IN_TOTAL_BKD,
  OUT_SERVO_4     = 28+DEVICE_IN_BKD*IN_TOTAL_BKD,
  OUT_SERVO_5     = 29+DEVICE_IN_BKD*IN_TOTAL_BKD,
  OUT_SERVO_6     = 30+DEVICE_IN_BKD*IN_TOTAL_BKD,
  OUT_SERVO_7     = 31+DEVICE_IN_BKD*IN_TOTAL_BKD,
  OUT_SERVO_8     = 32+DEVICE_IN_BKD*IN_TOTAL_BKD,
    AND_ADR_OUT_SERVO = 32+DEVICE_IN_BKD*IN_TOTAL_BKD
}adr_devices;



void Qwerty_all_Device        ( void )
{
  
  if(++current_BKD>(IN_TOTAL_BKD+1)){//if(++current_BKD>(IN_TOTAL_BKD-1)){
       current_BKD=0;
    if(++current_device_in_BKDx>(DEVICE_IN_BKD)){
        current_device_in_BKDx=1;
    }
  }
  
  
if(current_BKD<IN_TOTAL_BKD){//  Speaking to device BKD
  message_from_BKD [current_BKD] [0]= current_device_in_BKDx+current_BKD*DEVICE_IN_BKD;
 switch (current_device_in_BKDx)//  Check Device
  {
//YKP    
  	case YKP1_BKD:
    { 
     if(current_tick_trasmition){  
       if(current_BKD<8){
         YLSToYVPacket1.data.health_abonent[0] &= ~(1<<(current_BKD));// ((((YLSToYVPacket1.data.health_abonent[0] >>(current_BKD))&0x01)==0x01) ? 1 : 2);
       }else{
         YLSToYVPacket1.data.health_abonent[1] &= ~(1<<(current_BKD-8));
       }
     }
     else{
      if(current_BKD<8){
        YLSToYVPacket2.data.health_abonent[0] &= ~(1<<(current_BKD));
       }else{
        YLSToYVPacket2.data.health_abonent[1] &= ~(1<<(current_BKD-8));
       }
     }
        break;
    }
    case YKP2_BKD:
    {
     if(current_tick_trasmition){
      if(current_BKD<4){
        YLSToYVPacket1.data.health_abonent[1] &= ~(1<<(current_BKD+4));
       }else{
        YLSToYVPacket1.data.health_abonent[2] &= ~(1<<(current_BKD-4));
       }
     }
     else{
      if(current_BKD<4){
        YLSToYVPacket2.data.health_abonent[1] &= ~(1<<(current_BKD+4));
       }else{
        YLSToYVPacket2.data.health_abonent[2] &= ~(1<<(current_BKD-4));
       }
     }
  	 break;
    }
  	case YKP3_BKD:
    {
     if(current_tick_trasmition){
       if(current_BKD<8){
        YLSToYVPacket1.data.health_abonent[3] &= ~(1<<(current_BKD));
       }else{
        YLSToYVPacket1.data.health_abonent[4] &= ~(1<<(current_BKD-8));
       }
     }
     else{
       if(current_BKD<8){
        YLSToYVPacket2.data.health_abonent[3] &= ~(1<<(current_BKD));
       }else{
        YLSToYVPacket2.data.health_abonent[4] &= ~(1<<(current_BKD-8));
       }
     }
  	 break;
    }
//YAZ
  	case YAZ1_BKD:
    {
     if(current_tick_trasmition){
      if(current_BKD<4){
        YLSToYVPacket1.data.health_abonent[4] &= ~(1<<(current_BKD+4));
       }else{
        YLSToYVPacket1.data.health_abonent[5] &= ~(1<<(current_BKD-4));
       }
    }
     else{
      if(current_BKD<4){
        YLSToYVPacket2.data.health_abonent[4] &= ~(1<<(current_BKD+4));
       }else{
        YLSToYVPacket2.data.health_abonent[5] &= ~(1<<(current_BKD-4));
       }
    }      
  	 break;
    }
  	case YAZ2_BKD:
    {
     if(current_tick_trasmition){
       if(current_BKD<8){
        YLSToYVPacket1.data.health_abonent[6] &= ~(1<<(current_BKD));
       }else{
        YLSToYVPacket1.data.health_abonent[7] &= ~(1<<(current_BKD-8));
       }
    }
     else{
       if(current_BKD<8){
        YLSToYVPacket2.data.health_abonent[6] &= ~(1<<(current_BKD));
       }else{
        YLSToYVPacket2.data.health_abonent[7] &= ~(1<<(current_BKD-8));
       }
    }
  	 break;
    }
  	case YAZ3_BKD:
    {
      if(current_tick_trasmition){
      if(current_BKD<4){
        YLSToYVPacket1.data.health_abonent[7] &= ~(1<<(current_BKD+4));
       }else{
        YLSToYVPacket1.data.health_abonent[8] &= ~(1<<(current_BKD-4));
       }
    }
      else{
      if(current_BKD<4){
        YLSToYVPacket2.data.health_abonent[7] &= ~(1<<(current_BKD+4));
       }else{
        YLSToYVPacket2.data.health_abonent[8] &= ~(1<<(current_BKD-4));
       }
    }
  	 break;
    }
//YPS
  	case YPS1_BKD:
    {
     if(current_tick_trasmition){
       if(current_BKD<8){
        YLSToYVPacket1.data.health_abonent[9]  &= ~(1<<(current_BKD));
       }else{
        YLSToYVPacket1.data.health_abonent[10] &= ~(1<<(current_BKD-8));
       }
     }
     else{
       if(current_BKD<8){
        YLSToYVPacket2.data.health_abonent[9]  &= ~(1<<(current_BKD));
       }else{
        YLSToYVPacket2.data.health_abonent[10] &= ~(1<<(current_BKD-8));
       }
     }       
  	 break;
    }
  	case YPS2_BKD:
    {
     if(current_tick_trasmition){
      if(current_BKD<4){
        YLSToYVPacket1.data.health_abonent[10] &= ~(1<<(current_BKD+4));
       }else{
        YLSToYVPacket1.data.health_abonent[11] &= ~(1<<(current_BKD-4));
       }
     }
     else{
      if(current_BKD<4){
        YLSToYVPacket2.data.health_abonent[10] &= ~(1<<(current_BKD+4));
       }else{
        YLSToYVPacket2.data.health_abonent[11] &= ~(1<<(current_BKD-4));
       }
     }
  	 break;
    }
  	case YPS3_BKD:
    {
     if(current_tick_trasmition){
       if(current_BKD<8){
        YLSToYVPacket1.data.health_abonent[12] &= ~(1<<(current_BKD));
       }else{
        YLSToYVPacket1.data.health_abonent[13] &= ~(1<<(current_BKD-8));
       }
     }
     else{
       if(current_BKD<8){
        YLSToYVPacket2.data.health_abonent[12] &= ~(1<<(current_BKD));
       }else{
        YLSToYVPacket2.data.health_abonent[13] &= ~(1<<(current_BKD-8));
       }
     }
  	 break;
    }
//YLK_96_1_BKD1
  	case YLK_96_1_BKD:
    {
     if(current_tick_trasmition){
      if(current_BKD<4){
        YLSToYVPacket1.data.health_abonent[13] &= ~(1<<(current_BKD+4));
       }else{
        YLSToYVPacket1.data.health_abonent[14] &= ~(1<<(current_BKD-4));
       }
     }
     else{
      if(current_BKD<4){
        YLSToYVPacket2.data.health_abonent[13] &= ~(1<<(current_BKD+4));
       }else{
        YLSToYVPacket2.data.health_abonent[14] &= ~(1<<(current_BKD-4));
       }
     }
  	 break;
    }
  	case YLK_96_2_BKD:
    {
     if(current_tick_trasmition){
       if(current_BKD<8){
        YLSToYVPacket1.data.health_abonent[15] &= ~(1<<(current_BKD));
       }else{
        YLSToYVPacket1.data.health_abonent[16] &= ~(1<<(current_BKD-8));
       }
     }
     else{
       if(current_BKD<8){
        YLSToYVPacket2.data.health_abonent[15] &= ~(1<<(current_BKD));
       }else{
        YLSToYVPacket2.data.health_abonent[16] &= ~(1<<(current_BKD-8));
       }
     }
  	 break;
    }
  	case YLK_96_3_BKD:
    {
     if(current_tick_trasmition){
      if(current_BKD<4){
        YLSToYVPacket1.data.health_abonent[16] &= ~(1<<(current_BKD+4));
       }else{
        YLSToYVPacket1.data.health_abonent[17] &= ~(1<<(current_BKD-4));
       }
     }
     else{
      if(current_BKD<4){
        YLSToYVPacket2.data.health_abonent[16] &= ~(1<<(current_BKD+4));
       }else{
        YLSToYVPacket2.data.health_abonent[17] &= ~(1<<(current_BKD-4));
       }
     }
  	 break;
    }
  	default:
    {
//      current_BKD=0;
//      current_device_in_BKDx=1;
  		break;
    }
  }

}else{
   if(current_BKD == IN_TOTAL_BKD){ //  Speaking to device in BKHO_A
    if(++current_device_in_BKHO_A > AND_ADR_BKHO_A){
         current_device_in_BKHO_A = START_ADR_BKHO_A;
    }
    message_from_BKD [current_BKD] [0]= current_device_in_BKHO_A;
    switch (current_device_in_BKHO_A)
    {
  //YPS_BKHO_A
      case YPS1_BKHO_A:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[18]  &= ~1;
        }
        else{
          YLSToYVPacket2.data.health_abonent[18]  &= ~1;
        }       
       break;
      }
      case YPS2_BKHO_A:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[18] &= ~2;
        }
        else{
          YLSToYVPacket2.data.health_abonent[18] &= ~2;
        }
       break;
      }
      case YPS3_BKHO_A:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[18] &= ~4;
        }
        else{
          YLSToYVPacket2.data.health_abonent[18] &= ~4;
        }
       break;
      }
  //YTP
      case YTP1_1_BKHO_A:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[18] &= ~8;   
        }
        else{
          YLSToYVPacket2.data.health_abonent[18] &= ~8;
        }
       break;
      }
      case YTP1_2_BKHO_A:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[18] &= ~16;
        }
        else{
          YLSToYVPacket2.data.health_abonent[18] &= ~16;
        }
       break;
      }
      case YTP1_3_BKHO_A:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[18] &= ~32;
        }
        else{
          YLSToYVPacket2.data.health_abonent[18] &= ~32;
        }
       break;
      }
      case YTP2_1_BKHO_A:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[18] &= ~64;   
        }
        else{
          YLSToYVPacket2.data.health_abonent[18] &= ~64;
        }
       break;
      }
      case YTP2_2_BKHO_A:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[18] &= ~128;
        }
        else{
          YLSToYVPacket2.data.health_abonent[18] &= ~128;
        }
       break;
      }
      case YTP2_3_BKHO_A:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[19] &= ~1;
        }
        else{
          YLSToYVPacket2.data.health_abonent[19] &= ~1;
        }
       break;
      }
  //YTM
      case YTM1_1_BKHO_A:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[19] &= ~2;   
        }
        else{
          YLSToYVPacket2.data.health_abonent[19] &= ~2;
        }
       break;
      }
      case YTM1_2_BKHO_A:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[19] &= ~4;
        }
        else{
          YLSToYVPacket2.data.health_abonent[19] &= ~4;
        }
       break;
      }
      case YTM1_3_BKHO_A:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[19] &= ~8;
        }
        else{
          YLSToYVPacket2.data.health_abonent[19] &= ~8;
        }
       break;
      }
      case YTM2_1_BKHO_A:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[19] &= ~16;   
        }
        else{
          YLSToYVPacket2.data.health_abonent[19] &= ~16;
        }
       break;
      }
      case YTM2_2_BKHO_A:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[19] &= ~32;
        }
        else{
          YLSToYVPacket2.data.health_abonent[19] &= ~32;
        }
       break;
      }
      case YTM2_3_BKHO_A:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[19] &= ~64;
        }
        else{
          YLSToYVPacket2.data.health_abonent[19] &= ~64;
        }
       break;
      }
  //YGR_BKHO_A
      case YGR1_BKHO_A:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[19] &= ~128;
        }
        else{
          YLSToYVPacket2.data.health_abonent[19] &= ~128;
        }       
       break;
      }
      case YGR2_BKHO_A:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[20] &= ~1;
        }
        else{
          YLSToYVPacket2.data.health_abonent[20] &= ~1;
        }
       break;
      }
      case YGR3_BKHO_A:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[20] &= ~2;
        }
        else{
          YLSToYVPacket2.data.health_abonent[20] &= ~2;
        }
       break;
      }
  //YLK_32_3_BKHO_A
      case YLK_32_3_1_BKHO_A:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[20] &= ~4;
        }
        else{
          YLSToYVPacket2.data.health_abonent[20] &= ~4;
        }       
       break;
      }
      case YLK_32_3_2_BKHO_A:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[20] &= ~8;
        }
        else{
          YLSToYVPacket2.data.health_abonent[20] &= ~8;
        }
       break;
      }
      case YLK_32_3_3_BKHO_A:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[20] &= ~16;
        }
        else{
          YLSToYVPacket2.data.health_abonent[20] &= ~16;
        }
       break;
      }
  //YVP_BKHO_A
      case YVP1_BKHO_A:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[20] &= ~32;
        }
        else{
          YLSToYVPacket2.data.health_abonent[20] &= ~32;
        }       
       break;
      }
      case YVP2_BKHO_A:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[20] &= ~64;
        }
        else{
          YLSToYVPacket2.data.health_abonent[20] &= ~64;
        }
       break;
      }
      case YVP3_BKHO_A:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[20] &= ~128;
        }
        else{
          YLSToYVPacket2.data.health_abonent[20] &= ~128;
        }
       break;
      }

      default:
      {
  //      current_BKD=0;
  //      current_device_in_BKDx=1;
        break;
      }
    }
   }
   else{                            //  Speaking to Out Servo
    if(++current_device_in_OUT_SERVO > AND_ADR_OUT_SERVO){
         current_device_in_OUT_SERVO = START_ADR_OUT_SERVO;
    }
    message_from_BKD [current_BKD] [0]= current_device_in_OUT_SERVO;
    switch (current_device_in_OUT_SERVO)
    {
      case OUT_SERVO_1:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[21] &= ~1;
        }
        else{
          YLSToYVPacket2.data.health_abonent[21] &= ~1;
        }       
       break;
      }
      case OUT_SERVO_2:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[21] &= ~2;
        }
        else{
          YLSToYVPacket2.data.health_abonent[21] &= ~2;
        }
       break;
      }
      case OUT_SERVO_3:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[21] &= ~4;
        }
        else{
          YLSToYVPacket2.data.health_abonent[21] &= ~4;
        }
       break;
      }
      case OUT_SERVO_4:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[21] &= ~8;   
        }
        else{
          YLSToYVPacket2.data.health_abonent[21] &= ~8;
        }
       break;
      }
      case OUT_SERVO_5:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[21] &= ~16;
        }
        else{
          YLSToYVPacket2.data.health_abonent[21] &= ~16;
        }
       break;
      }
      case OUT_SERVO_6:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[21] &= ~32;
        }
        else{
          YLSToYVPacket2.data.health_abonent[21] &= ~32;
        }
       break;
      }
      case OUT_SERVO_7:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[21] &= ~64;   
        }
        else{
          YLSToYVPacket2.data.health_abonent[21] &= ~64;
        }
       break;
      }
      case OUT_SERVO_8:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[21] &= ~128;
        }
        else{
          YLSToYVPacket2.data.health_abonent[21] &= ~128;
        }
       break;
      }
      default:
      {
  //      current_BKD=0;
  //      current_device_in_BKDx=1;
        break;
      }
    }
   }

}  
    message_from_BKD [current_BKD][USART_BUFFER_TX_SIZE_YLS-1]= Crc8(message_from_BKD[current_BKD],USART_BUFFER_TX_SIZE_YLS-1);
  
  uint32_t usart = USART5;
  
#if ( defined (DMA_TX_USART))
         if( usart == USART0){
            usart0_transmit=1;
           USART0_TX
           //dma_channel_disable                  (DMA1, DMA_CH7  );                                // Выключаем dma для передачи
           dma_flag_clear                       (DMA1, DMA_CH7, DMA_FLAG_FTF );
           dma_memory_address_config            (DMA1, DMA_CH7, DMA_MEMORY_0, (uint32_t)message_from_BKD [current_BKD] );  // Указываем адрес буфера
           //dma_periph_address_config            (DMA1, DMA_CH7, USARTn_DATA_ADDRESS(USART0) );
           dma_transfer_number_config           (DMA1, DMA_CH7, USART_BUFFER_TX_SIZE_YLS );               // Указываем количество данных
           dma_channel_enable                   (DMA1, DMA_CH7  );                                // Включаем dma для передачи
         }else{
            usart5_transmit=1;

           USART5_TX
           //dma_channel_disable                  (DMA1, DMA_CH6  );                                // Выключаем dma для передачи
           dma_memory_address_config            (DMA1, DMA_CH6, DMA_MEMORY_0, (uint32_t)message_from_BKD [current_BKD] );  // Указываем адрес буфера
           //dma_periph_address_config            (DMA1, DMA_CH6, USARTn_DATA_ADDRESS(USART5) );
           dma_transfer_number_config           (DMA1, DMA_CH6, USART_BUFFER_TX_SIZE_YLS );               // Указываем количество данных
           dma_channel_enable                   (DMA1, DMA_CH6  );                                // Включаем dma для передачи
         }
#endif
#if (!defined (DMA_TX_USART))
     if( usart == USART0){
       USART0_TX
        Usart_send_buf(message_from_BKD [current_BKD], USART0, USART_BUFFER_TX_SIZE_YLS); 
       USART0_RX
     }else{
       USART5_TX
        Usart_send_buf(message_from_BKD [current_BKD], USART5, USART_BUFFER_TX_SIZE_YLS); 
       USART5_RX
     }
#endif
     
}

#if (defined (TEST_UDP))
void Check_message_YV(void  *message,const ip_addr_t *addr, uint16_t port)
#else
void Check_message_YV         (  void  *message)
#endif
{
  if (message == NULL) return;
  YVToYLSPacket *rxpak = message;
  uint8_t command=0x00;
  
  if(rxpak->data.command){
    if(rxpak->data.command == YV_COMMAND_TEST1){
      command=YV_COMMAND_TEST1;
    }
    if(rxpak->data.command == YV_COMMAND_TEST2){
      command=YV_COMMAND_TEST2;
    }
  }
  
   for (uint8_t i=0; i< IN_TOTAL_BKD+1;i++){
      message_from_BKD [i][1] =(command);
      message_from_BKD [i][2]= (rxpak->data.yps_mask[i]);//yps 
      message_from_BKD [i][4] =(rxpak->data.ykp_angel[i][0]);
      message_from_BKD [i][5] =(rxpak->data.ykp_angel[i][0])>>8;
      message_from_BKD [i][6] =(rxpak->data.ykp_angel[i][1]);
      message_from_BKD [i][7] =(rxpak->data.ykp_angel[i][1])>>8;

      message_from_BKD [i][15]= Crc8(message_from_BKD [i],15);
   }
   
  if(current_tick_trasmition){
       current_tick_trasmition=0;
#if (defined (TEST_UDP))
      udp_send_buf      (&YLSToYVPacket1,1432,addr,port);
#else
      udp_send_buf      (&YLSToYVPacket1,1432,&yv_ip,YV_PORT);
#endif
      
  }else{
       current_tick_trasmition=1;
#if (defined (TEST_UDP))
      udp_send_buf      (&YLSToYVPacket2,1432,addr,port);
#else
      udp_send_buf      (&YLSToYVPacket2,1432,&yv_ip,YV_PORT);
#endif
  }

}
void Check_message_UART       (uint8_t *message, uint8_t size)
{
      if (message == NULL) return;
  if(size > 1 && message[size-1]==Crc8(message,size-1)){
     
    if(current_BKD != IN_TOTAL_BKD){   //  Check Device in BKD
        Check_message_BKD       (message,size);
    }
    else{
     if(current_BKD == IN_TOTAL_BKD){ //  Check Device in BKHO_A
        Check_message_BKHO_A    (message,size);
     }
     else{                            //  Check Out Servo
        Check_message_OUT_SERVO (message,size);
     }
    }
  }
}


void Check_message_BKD        (uint8_t *message, uint8_t size)
{
      if (message == NULL) return;
      
    switch (current_device_in_BKDx)
    {
  //YKP    
      case YKP1_BKD:
      {
        if(size==USART_BUFFER_TX_SIZE_YKP){
         if(current_tick_trasmition){
           if(current_BKD<8){
            YLSToYVPacket1.data.health_abonent[0] |= 1<<(current_BKD);
           }else{
            YLSToYVPacket1.data.health_abonent[1] |= 1<<(current_BKD-8);
           }
          YLSToYVPacket1.data.ykp_angel[current_BKD][0][0]=(((uint16_t)message[0])<<8)|message[1];
          YLSToYVPacket1.data.ykp_angel[current_BKD][0][1]=(((uint16_t)message[2])<<8)|message[3];
         }
         else{
          if(current_BKD<8){
            YLSToYVPacket2.data.health_abonent[0] |= 1<<(current_BKD);
           }else{
            YLSToYVPacket2.data.health_abonent[1] |= 1<<(current_BKD-8);
           }
           
          YLSToYVPacket2.data.ykp_angel[current_BKD][0][0]=(((uint16_t)message[0])<<8)|message[1];
          YLSToYVPacket2.data.ykp_angel[current_BKD][0][1]=(((uint16_t)message[2])<<8)|message[3];
         }
       }
       break;
      }
      case YKP2_BKD:
      {
        if(size==USART_BUFFER_TX_SIZE_YKP){
         if(current_tick_trasmition){
          if(current_BKD<4){
            YLSToYVPacket1.data.health_abonent[1] |= 1<<(current_BKD+4);
           }else{
            YLSToYVPacket1.data.health_abonent[2] |= 1<<(current_BKD-4);
           }
          YLSToYVPacket1.data.ykp_angel[current_BKD][1][0]=(((uint16_t)message[0])<<8)|message[1];
          YLSToYVPacket1.data.ykp_angel[current_BKD][1][1]=(((uint16_t)message[2])<<8)|message[3];
         }
         else{
          if(current_BKD<4){
            YLSToYVPacket2.data.health_abonent[1] |= 1<<(current_BKD+4);
           }else{
            YLSToYVPacket2.data.health_abonent[2] |= 1<<(current_BKD-4);
           }
          YLSToYVPacket2.data.ykp_angel[current_BKD][1][0]=(((uint16_t)message[0])<<8)|message[1];
          YLSToYVPacket2.data.ykp_angel[current_BKD][1][1]=(((uint16_t)message[2])<<8)|message[3];
         }
        }
       break;
      }
      case YKP3_BKD:
      {
        if(size==USART_BUFFER_TX_SIZE_YKP){
         if(current_tick_trasmition){
           if(current_BKD<8){
            YLSToYVPacket1.data.health_abonent[3] |= 1<<(current_BKD);
           }else{
            YLSToYVPacket1.data.health_abonent[4] |= 1<<(current_BKD-8);
           }
          YLSToYVPacket1.data.ykp_angel[current_BKD][2][0]=(((uint16_t)message[0])<<8)|message[1];
          YLSToYVPacket1.data.ykp_angel[current_BKD][2][1]=(((uint16_t)message[2])<<8)|message[3];
         }
         else{
           if(current_BKD<8){
            YLSToYVPacket2.data.health_abonent[3] |= 1<<(current_BKD);
           }else{
            YLSToYVPacket2.data.health_abonent[4] |= 1<<(current_BKD-8);
           }
          YLSToYVPacket2.data.health_abonent[0] |=4;
          YLSToYVPacket2.data.ykp_angel[current_BKD][2][0]=(((uint16_t)message[0])<<8)|message[1];
          YLSToYVPacket2.data.ykp_angel[current_BKD][2][1]=(((uint16_t)message[2])<<8)|message[3];
         }
       }
       break;
      }
  //YAZ
      case YAZ1_BKD:
      {
        if(size==USART_BUFFER_TX_SIZE_YAZ){
           if(current_tick_trasmition){
            if(current_BKD<4){
              YLSToYVPacket1.data.health_abonent[4] |= 1<<(current_BKD+4);
             }else{
              YLSToYVPacket1.data.health_abonent[5] |= 1<<(current_BKD-4);
             }
             
             for(uint8_t i=0; i<24;i++){
               YLSToYVPacket1.data.yaz_data[current_BKD][0][i]  = message[i];
             }
          }
           else{
            if(current_BKD<4){
              YLSToYVPacket2.data.health_abonent[4] |= 1<<(current_BKD+4);
             }else{
              YLSToYVPacket2.data.health_abonent[5] |= 1<<(current_BKD-4);
             }
             for(uint8_t i=0; i<24;i++){
               YLSToYVPacket2.data.yaz_data[current_BKD][0][i]  = message[i];
             }
          }   
        }
       break;
      }
      case YAZ2_BKD:
      {
       if(size==USART_BUFFER_TX_SIZE_YAZ){
         if(current_tick_trasmition){
           if(current_BKD<8){
            YLSToYVPacket1.data.health_abonent[6] |= 1<<(current_BKD);
           }else{
            YLSToYVPacket1.data.health_abonent[7] |= 1<<(current_BKD-8);
           }
           for(uint8_t i=0; i<24;i++){
             YLSToYVPacket1.data.yaz_data[current_BKD][1][i]  = message[i];
           }
         }
         else{
           if(current_BKD<8){
            YLSToYVPacket2.data.health_abonent[6] |= 1<<(current_BKD);
           }else{
            YLSToYVPacket2.data.health_abonent[7] |= 1<<(current_BKD-8);
           }
           for(uint8_t i=0; i<24;i++){
             YLSToYVPacket2.data.yaz_data[current_BKD][1][i]  = message[i];
           }
         }
       }
       break;
      }
      case YAZ3_BKD:
      {
       if(size==USART_BUFFER_TX_SIZE_YAZ){
        if(current_tick_trasmition){
          if(current_BKD<4){
            YLSToYVPacket1.data.health_abonent[7] |= 1<<(current_BKD+4);
           }else{
            YLSToYVPacket1.data.health_abonent[8] |= 1<<(current_BKD-4);
           }
           for(uint8_t i=0; i<24;i++){
             YLSToYVPacket1.data.yaz_data[current_BKD][2][i]  = message[i];
           }
        }
        else{
          if(current_BKD<4){
            YLSToYVPacket2.data.health_abonent[7] |= 1<<(current_BKD+4);
           }else{
            YLSToYVPacket2.data.health_abonent[8] |= 1<<(current_BKD-4);
           }
           for(uint8_t i=0; i<24;i++){
             YLSToYVPacket2.data.yaz_data[current_BKD][2][i]  = message[i];
           }
        }
       }
       break;
      }
  //YPS
      case YPS1_BKD:
      {
        if(size==USART_BUFFER_TX_SIZE_YPS){
         if(current_tick_trasmition){
           if(current_BKD<8){
            YLSToYVPacket1.data.health_abonent[9]  |= 1<<(current_BKD);
           }else{
            YLSToYVPacket1.data.health_abonent[10] |= 1<<(current_BKD-8);
           }
          YLSToYVPacket1.data.yps_mask[current_BKD][0]=message[0];
         }
         else{
           if(current_BKD<8){
            YLSToYVPacket2.data.health_abonent[9]  |= 1<<(current_BKD);
           }else{
            YLSToYVPacket2.data.health_abonent[10] |= 1<<(current_BKD-8);
           }
          YLSToYVPacket2.data.yps_mask[current_BKD][0]=message[0];
         }
       }
       break;
      }
      case YPS2_BKD:
      {
        if(size==USART_BUFFER_TX_SIZE_YPS){
         if(current_tick_trasmition){
          if(current_BKD<4){
            YLSToYVPacket1.data.health_abonent[10] |= 1<<(current_BKD+4);
           }else{
            YLSToYVPacket1.data.health_abonent[11] |= 1<<(current_BKD-4);
           }
          YLSToYVPacket1.data.yps_mask[current_BKD][1]=message[0];
         }
         else{
          if(current_BKD<4){
            YLSToYVPacket2.data.health_abonent[10] |= 1<<(current_BKD+4);
           }else{
            YLSToYVPacket2.data.health_abonent[11] |= 1<<(current_BKD-4);
           }
          YLSToYVPacket2.data.yps_mask[current_BKD][1]=message[0];
         }
       }
       break;
      }
      case YPS3_BKD:
      {
        if(size==USART_BUFFER_TX_SIZE_YPS){
         if(current_tick_trasmition){
           if(current_BKD<8){
            YLSToYVPacket1.data.health_abonent[12] |= 1<<(current_BKD);
           }else{
            YLSToYVPacket1.data.health_abonent[13] |= 1<<(current_BKD-8);
           }
          YLSToYVPacket1.data.yps_mask[current_BKD][2]=message[0];
         }
         else{
           if(current_BKD<8){
            YLSToYVPacket2.data.health_abonent[12] |= 1<<(current_BKD);
           }else{
            YLSToYVPacket2.data.health_abonent[13] |= 1<<(current_BKD-8);
           }
          YLSToYVPacket2.data.yps_mask[current_BKD][2]=message[0];
         }
        }
       break;
      }
  //YLK_96
      case YLK_96_1_BKD:
      {
        if(size==USART_BUFFER_TX_SIZE_YLK_96){
          if(current_tick_trasmition){
            if(current_BKD<4){
              YLSToYVPacket1.data.health_abonent[13] |= 1<<(current_BKD+4);
             }else{
              YLSToYVPacket1.data.health_abonent[14] |= 1<<(current_BKD-4);
             }
          }
          else{
            if(current_BKD<4){
              YLSToYVPacket2.data.health_abonent[13] |= 1<<(current_BKD+4);
             }else{
              YLSToYVPacket2.data.health_abonent[14] |= 1<<(current_BKD-4);
             }
          }
        }
       break;
      }
      case YLK_96_2_BKD:
      {
        if(size==USART_BUFFER_TX_SIZE_YLK_96){
          if(current_tick_trasmition){
             if(current_BKD<8){
              YLSToYVPacket1.data.health_abonent[15] |= 1<<(current_BKD);
             }else{
              YLSToYVPacket1.data.health_abonent[16] |= 1<<(current_BKD-8);
             }
          }
          else{
             if(current_BKD<8){
              YLSToYVPacket2.data.health_abonent[15] |= 1<<(current_BKD);
             }else{
              YLSToYVPacket2.data.health_abonent[16] |= 1<<(current_BKD-8);
             }
          }
        }
       break;
      }
      case YLK_96_3_BKD:
      {
        if(size==USART_BUFFER_TX_SIZE_YLK_96){
          if(current_tick_trasmition){
            if(current_BKD<4){
              YLSToYVPacket1.data.health_abonent[16] |= 1<<(current_BKD+4);
             }else{
              YLSToYVPacket1.data.health_abonent[17] |= 1<<(current_BKD-4);
             }
          }
          else{
            if(current_BKD<4){
              YLSToYVPacket2.data.health_abonent[16] |= 1<<(current_BKD+4);
             }else{
              YLSToYVPacket2.data.health_abonent[17] |= 1<<(current_BKD-4);
             }
          }
        }
       break;
      }

      default:
      {
  //      current_BKD=0;
  //      current_device_in_BKDx=1;
        break;
      }
    }
 
}

void Check_message_BKHO_A     (uint8_t *message, uint8_t size)
{
      if (message == NULL) return;
      
 switch (current_device_in_BKHO_A)
    {
  //YPS_BKHO_A
      case YPS1_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YPS){ 
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[18]  |= 1;
          YLSToYVPacket1.data.yps_mask_BKHO_A[0]=message[0];
        }
        else{
          YLSToYVPacket2.data.health_abonent[18]  |= 1;
          YLSToYVPacket2.data.yps_mask_BKHO_A[0]=message[0];
        }
       }        
       break;
      }
      case YPS2_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YPS){
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[18] |= 2;
          YLSToYVPacket1.data.yps_mask_BKHO_A[1]=message[0];
        }
        else{
          YLSToYVPacket2.data.health_abonent[18] |= 2;
          YLSToYVPacket2.data.yps_mask_BKHO_A[1]=message[0];
        }
       }
       break;
      }
      case YPS3_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YPS){
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[18] |= 4;
          YLSToYVPacket1.data.yps_mask_BKHO_A[2]=message[0];
        }
        else{
          YLSToYVPacket2.data.health_abonent[18] |= 4;
          YLSToYVPacket2.data.yps_mask_BKHO_A[2]=message[0];
        }
       }
       break;
      }
  //YTP
      case YTP1_1_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YTP){
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[18] |= 8;   
        }
        else{
          YLSToYVPacket2.data.health_abonent[18] |= 8;
        }
       }
       break;
      }
      case YTP1_2_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YTP){
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[18] |= 16;
        }
        else{
          YLSToYVPacket2.data.health_abonent[18] |= 16;
        }
       }
       break;
      }
      case YTP1_3_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YTP){
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[18] |= 32;
        }
        else{
          YLSToYVPacket2.data.health_abonent[18] |= 32;
        }
       }
       break;
      }
      case YTP2_1_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YTP){
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[18] |= 64;   
        }
        else{
          YLSToYVPacket2.data.health_abonent[18] |= 64;
        }
       }
       break;
      }
      case YTP2_2_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YTP){
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[18] |= 128;
        }
        else{
          YLSToYVPacket2.data.health_abonent[18] |= 128;
        }
       }
       break;
      }
      case YTP2_3_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YTP){
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[19] |= 1;
        }
        else{
          YLSToYVPacket2.data.health_abonent[19] |= 1;
        }
       }
       break;
      }
  //YTM
      case YTM1_1_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YTM){
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[19] |= 2;   
        }
        else{
          YLSToYVPacket2.data.health_abonent[19] |= 2;
        }
       }
       break;
      }
      case YTM1_2_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YTM){
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[19] |= 4;
        }
        else{
          YLSToYVPacket2.data.health_abonent[19] |= 4;
        }
       }
       break;
      }
      case YTM1_3_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YTM){
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[19] |= 8;
        }
        else{
          YLSToYVPacket2.data.health_abonent[19] |= 8;
        }
       }
       break;
      }
      case YTM2_1_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YTM){
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[19] |= 16;   
        }
        else{
          YLSToYVPacket2.data.health_abonent[19] |= 16;
        }
       }
       break;
      }
      case YTM2_2_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YTM){
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[19] |= 32;
        }
        else{
          YLSToYVPacket2.data.health_abonent[19] |= 32;
        }
       }
       break;
      }
      case YTM2_3_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YTM){
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[19] |= 64;
        }
        else{
          YLSToYVPacket2.data.health_abonent[19] |= 64;
        }
       }
       break;
      }
  //YGR_BKHO_A
      case YGR1_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YGR){
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[19]  |= 128;
        }
        else{
          YLSToYVPacket2.data.health_abonent[19]  |= 128;
        }    
       }        
       break;
      }
      case YGR2_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YGR){
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[20] |= 1;
        }
        else{
          YLSToYVPacket2.data.health_abonent[20] |= 1;
        }
       }
       break;
      }
      case YGR3_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YGR){
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[20] |= 2;
        }
        else{
          YLSToYVPacket2.data.health_abonent[20] |= 2;
        }
       }
       break;
      }
  //YLK_32_3_BKHO_A
      case YLK_32_3_1_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YLK_32_3){
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[20]  |= 4;
        }
        else{
          YLSToYVPacket2.data.health_abonent[20]  |= 4;
        }
       }        
       break;
      }
      case YLK_32_3_2_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YLK_32_3){
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[20] |= 8;
        }
        else{
          YLSToYVPacket2.data.health_abonent[20] |= 8;
        }
       }
       break;
      }
      case YLK_32_3_3_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YLK_32_3){
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[20] |= 16;
        }
        else{
          YLSToYVPacket2.data.health_abonent[20] |= 16;
        }
       }
       break;
      }
  //YVP_BKHO_A
      case YVP1_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YVP){
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[20]  |= 32;
        }
        else{
          YLSToYVPacket2.data.health_abonent[20]  |= 32;
        }       
       break;
       }
      }
      case YVP2_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YVP){
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[20] |= 64;
        }
        else{
          YLSToYVPacket2.data.health_abonent[20] |= 64;
        }
       }
       break;
      }
      case YVP3_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YVP){
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[20] |= 128;
        }
        else{
          YLSToYVPacket2.data.health_abonent[20] |= 128;
        }
       }
       break;
      }

      default:
      {
  //      current_BKD=0;
  //      current_device_in_BKDx=1;
        break;
      }
    }
   
}
void Check_message_OUT_SERVO  (uint8_t *message, uint8_t size)
{
      if (message == NULL) return;
      
   switch (current_device_in_OUT_SERVO)
    {
  //YPS_BKHO_A
      case OUT_SERVO_1:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[21]  |= 1;
        }
        else{
          YLSToYVPacket2.data.health_abonent[21]  |= 1;
        }       
       break;
      }
      case OUT_SERVO_2:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[21] |= 2;
        }
        else{
          YLSToYVPacket2.data.health_abonent[21] |= 2;
        }
       break;
      }
      case OUT_SERVO_3:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[21] |= 4;
        }
        else{
          YLSToYVPacket2.data.health_abonent[21] |= 4;
        }
       break;
      }
      case OUT_SERVO_4:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[21] |= 8;   
        }
        else{
          YLSToYVPacket2.data.health_abonent[21] |= 8;
        }
       break;
      }
      case OUT_SERVO_5:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[21] |= 16;
        }
        else{
          YLSToYVPacket2.data.health_abonent[21] |= 16;
        }
       break;
      }
      case OUT_SERVO_6:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[21] |= 32;
        }
        else{
          YLSToYVPacket2.data.health_abonent[21] |= 32;
        }
       break;
      }
      case OUT_SERVO_7:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[21] |= 64;   
        }
        else{
          YLSToYVPacket2.data.health_abonent[21] |= 64;
        }
       break;
      }
      case OUT_SERVO_8:
      {
        if(current_tick_trasmition){
          YLSToYVPacket1.data.health_abonent[21] |= 128;
        }
        else{
          YLSToYVPacket2.data.health_abonent[21] |= 128;
        }
       break;
      }
      default:
      {
  //      current_BKD=0;
  //      current_device_in_BKDx=1;
        break;
      }
    }
}

/*
    $data = [byte[]]((,0xFF) * 13 + (,0x00) * 67);
    $udp = New-Object System.Net.Sockets.UdpClient; 
    $udp.Client.Bind((New-Object System.Net.IPEndPoint([System.Net.IPAddress]::Any, 200))); 
    $udp.Send($data, $data.Length, "192.168.17.231", 101); Start-Sleep -Milliseconds 100; 
    $udp.Close()
*/
int main(void)
{  
  SettingSystemClock25MHz();
  SystemCoreClockUpdate();
  
uint32_t  FREQ_AHB  = rcu_clock_freq_get(CK_AHB);
uint32_t  FREQ_APB2 = rcu_clock_freq_get(CK_APB2);
uint32_t  FREQ_APB1 = rcu_clock_freq_get(CK_APB1);
uint32_t  FREQ_Sys  = SystemCoreClock;

  

  YLSToYVPacket1.data.yps_mask_BKHO_A[0]=0X55;
  YLSToYVPacket1.data.yps_mask_BKHO_A[1]=0Xff;
  YLSToYVPacket1.data.yps_mask_BKHO_A[2]=0X00;
  
  YLSToYVPacket2.data.yps_mask_BKHO_A[0]=0XAA;
  YLSToYVPacket2.data.yps_mask_BKHO_A[1]=0X55;
  YLSToYVPacket2.data.yps_mask_BKHO_A[2]=0XAA;
  Rcu_config();
  Gpio_config();
  
      IP4_ADDR( &     yv_ip,      YV_ADDR0,      YV_ADDR1,      YV_ADDR2,      YV_ADDR3 ); // или ipaddr_aton(IP_ADDR, &ip) #define IP_ADDR "192.168.1.188"
      IP4_ADDR( &kasu_kp_ip, KASU_KP_ADDR0, KASU_KP_ADDR1, KASU_KP_ADDR2, KASU_KP_ADDR3 ); // или ipaddr_aton(IP_ADDR, &ip) #define IP_ADDR "192.168.1.188"
 
    enet_system_setup();
    /* initilaize the LwIP stack */
    lwip_stack_init();
    if (udp_create_socket() != 0) {
      //  printf("UDP demo initialization failed\n");      
      NVIC_SystemReset();
        return 1;
    }

  ConfigUsart(USART0,USART_BAUDRATE,USART_MSBF_LSB,USART_OVSMOD_8,receiveRBNE,0,3);
  ConfigUsart(USART5,USART_BAUDRATE,USART_MSBF_LSB,USART_OVSMOD_8,receiveRBNE,0,3);
#if (defined(DMA_TX_USART))
  ConfigUsartDMA_Tx (USART0, message_from_BKD[0],USART_BUFFER_TX_SIZE_YLS,0,DMA_PRIORITY_HIGH,0,6,iRQn_full_transmit_Dma);
  ConfigUsartDMA_Tx (USART5, message_from_BKD[0],USART_BUFFER_TX_SIZE_YLS,0,DMA_PRIORITY_HIGH,0,6,iRQn_full_transmit_Dma);
#endif
        
 // Настройка таймеров
    
  Timer2_config();  timer_disable(TIMER2); // End rx usart0
  Timer4_config();  timer_disable(TIMER4); // End rx usart5
  Timer3_config();  // PWM From work rs485
  Timer6_config();// ( Qwerty usart )
//  timer1_config(); not used
    
    gpio_mode_set           (Blink_TX_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, Blink_TX_PIN);
    gpio_output_options_set (Blink_TX_PORT, GPIO_OTYPE_PP,   GPIO_OSPEED_50MHZ, Blink_TX_PIN);
  
    gpio_bit_reset(Blink_TX_PORT,Blink_TX_PIN);
    
    gpio_mode_set           (Blink_RX_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, Blink_RX_PIN);
    gpio_output_options_set (Blink_RX_PORT, GPIO_OTYPE_PP,   GPIO_OSPEED_50MHZ, Blink_RX_PIN);
  
    gpio_bit_reset(Blink_RX_PORT,Blink_RX_PIN);
    
 while(1) {
      
#ifndef USE_ENET_INTERRUPT
        /* check if any packet received */
        if(enet_rxframe_size_get()) {
            /* process received ethernet packet */
            lwip_frame_recv();
        }
#endif /* USE_ENET_INTERRUPT */

        /* handle periodic timers for LwIP */
#ifdef TIMEOUT_CHECK_USE_LWIP
        sys_check_timeouts();

#ifdef USE_DHCP
        lwip_dhcp_address_get();
#endif /* USE_DHCP */

#else
        lwip_timeouts_check(g_localtime);
#endif /* TIMEOUT_CHECK_USE_LWIP */
 }
}
  
/*!
    \brief      after the netif is fully configured, it will be called to initialize the function of telnet, client and udp
    \param[in]  netif: the struct used for lwIP network interface
    \param[out] none
    \retval     none
*/
void lwip_netif_status_callback(struct netif *netif)
{
    if((netif->flags & NETIF_FLAG_UP) != 0) {
    }
}

/*!
    \brief      insert a delay time
    \param[in]  ncount: number of 10ms periods to wait for
    \param[out] none
    \retval     none
*/





void delay_10ms   (uint32_t ncount)
{
    /* capture the current local time */
    g_timedelay = g_localtime + ncount;

    /* wait until the desired delay finish */
    while(g_timedelay > g_localtime) {
    }
}

/*!
    \brief      updates the system local time
    \param[in]  none
    \param[out] none
    \retval     none
*/
void time_update  (void)
{
    g_localtime += SYSTEMTICK_PERIOD_MS;
}


uint8_t udp_create_socket (void)
{
	//struct udp_pcb *pcb;
	ip_addr_t remote_ip;
	//pcb	= udp_new();
  _udp_pcb_ = udp_new();
	 if (_udp_pcb_ == NULL) // application fails
	{
		return 1;
	}else
	{///LOCAL_PORT = 101 ||| REMOTE_PORT = 200
 	 if (udp_bind (_udp_pcb_, IP_ADDR_ANY, LOCAL_PORT) == ERR_OK) // bind the local port IP, IP_ADDR_ANY 0, in fact, explain the use of local IP address, use the recommended priority. Because in the case of DHCP, we can not know in advance the IP.
		{
		 udp_recv (_udp_pcb_, udp_reciev, NULL); // register a callback message processing				
		}else
			return 1;
	}
	return 0;
}

void udp_reciev           (void *arg, struct udp_pcb *pcb, struct pbuf *p,  // приём данных по upd
                                      const ip_addr_t *addr, u16_t port)   // reciev data on enet
{
  gpio_bit_set(Blink_RX_PORT,Blink_RX_PIN);

    if (p == NULL) return;
 if( SIZE_MESSAGE_YV == p->len ){
#if (defined (TEST_UDP))
    Check_message_YV(p->payload,addr, (uint16_t)port);
#else
    Check_message_YV(p->payload);
#endif
   
 }else{// *( (uint8_t *)p->payload + i )
   uint16_t magic_value =(*( (uint16_t *)p->payload + 0));
   if((*( (uint16_t *)p->payload + 0))==MAGIC_KASU_KP){//from KASU-KP
     udp_send_buf(p->payload,p->len,&kasu_kp_ip,KASU_KP_PORT);
   }
   if((*( (uint16_t *)p->payload + 0))==MAGIC_YV) {//from BCVM
     udp_send_buf(p->payload,p->len,&yv_ip,YV_PORT);
   }
 }
  pbuf_free(p);
 
  gpio_bit_reset(Blink_RX_PORT,Blink_RX_PIN);
}
 
void udp_send_buf         (void * buf,  uint32_t size, const ip_addr_t *addr, u16_t port) // отправка данных по upd
{
  gpio_bit_set(Blink_TX_PORT, Blink_TX_PIN);
    struct pbuf *p;
    err_t err;

    if (!_udp_pcb_) {
        return; // Ошибка создания
    }
    // Создание pbuf
    p = pbuf_alloc(PBUF_TRANSPORT, size, PBUF_ROM);
    if (!p) {
        return; // Ошибка выделения памяти
    }
    
    p->payload = (void*)buf; 
    // Отправка пакета
    err = udp_sendto(_udp_pcb_, p, addr, port);
        // Освобождение ресурсов
    pbuf_free(p);
    if (err != ERR_OK) {
        // Обработка ошибки отправки
    }
  gpio_bit_reset(Blink_TX_PORT ,Blink_TX_PIN);
}

unsigned char Crc8 (unsigned char *pcBlock, unsigned char size)
{
    unsigned char crc = 0xFF;
 
    while (size--)
        crc = Crc8Table[crc ^ *pcBlock++];
 
    return crc;
}

// Обработчики прерывания
void TIMER1_IRQHandler(void)// not used'
{
 if(timer_interrupt_flag_get  (TIMER1, TIMER_INT_UP) != RESET) {
    timer_interrupt_flag_clear(TIMER1, TIMER_INT_UP);

 }
}
void TIMER2_IRQHandler(void) // End Rx USART0 
{
 if(timer_interrupt_flag_get  (TIMER2, TIMER_INT_FLAG_UP) != RESET) {
		timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);
    {
     Check_message_UART((uint8_t*)usart0_buffer_rx,usart0_counter);
      for(uint32_t i =0; i<usart0_counter;){
        usart0_buffer_rx[i++]=0;
      }
     usart0_counter = 0;
     timer_disable(TIMER2);
     timer2En = 0;
    }
 }
}
void TIMER4_IRQHandler(void) // End Rx USART5
{
 if(timer_interrupt_flag_get  (TIMER4, TIMER_INT_FLAG_UP) != RESET) {
    timer_interrupt_flag_clear(TIMER4, TIMER_INT_FLAG_UP);
    {
     Check_message_UART((uint8_t*)usart5_buffer_rx,usart5_counter);
      for(uint32_t i =0; i<usart5_counter;){
        usart5_buffer_rx[i++]=0;
      }  
     usart5_counter= 0;
     timer4En=0;
     timer_disable(TIMER4);
    }
 }
}
void TIMER6_IRQHandler(void) // 125 us
{
 if(timer_interrupt_flag_get  (TIMER6, TIMER_INT_FLAG_UP) != RESET) {
		timer_interrupt_flag_clear(TIMER6, TIMER_INT_FLAG_UP);   
    Qwerty_all_Device();
 }
}
void USART0_IRQHandler(void) // Rx data 
{
 if(usart_interrupt_flag_get  (USART0, USART_INT_FLAG_RBNE) != RESET){
    usart_interrupt_flag_clear(USART0, USART_INT_FLAG_RBNE);
    uint8_t received_byte = (uint8_t)usart_data_receive(USART0);

   if(usart0_transmit == 0){
    if((usart0_counter< USART_RX_BUFFER_SIZE)){
        TIMER_CNT   (TIMER2) = 0;
      if(!timer2En){
        timer2En=1;
        timer_enable(TIMER2);
      }
      usart0_buffer_rx[usart0_counter++] = received_byte;
    }else{
      memset((void*)usart0_buffer_rx, 0x00, usart0_counter);
      usart0_counter = 0;
    }
  }
 }
}
void USART5_IRQHandler(void) // Rx data 
{
 if(usart_interrupt_flag_get  (USART5, USART_INT_FLAG_RBNE) != RESET ){//USART_INT_FLAG_IDLE
    usart_interrupt_flag_clear(USART5, USART_INT_FLAG_RBNE);
    
   uint8_t received_byte = (uint8_t)usart_data_receive(USART5);
   
   if(usart5_transmit==0){
    if(usart5_counter< USART_RX_BUFFER_SIZE){
      TIMER_CNT   (TIMER4) = 0;
    if(!timer4En){
      timer4En=1;
      timer_enable(TIMER4);
    }
    usart5_buffer_rx[usart5_counter++] = received_byte;
    }else{
      memset((void*)usart5_buffer_rx, 0x00, usart5_counter);
      usart5_counter = 0;
    }
  }
 }
}
void DMA1_Channel7_IRQHandler (void) //    Tx USART0
{
  if (dma_flag_get  (DMA1, DMA_CH7,DMA_FLAG_FTF)!= RESET) {
      dma_flag_clear(DMA1, DMA_CH7,DMA_FLAG_FTF);
     USART0_RX
        usart0_transmit=0;
       dma_channel_disable(DMA1,DMA_CH7);
  }
}
void DMA1_Channel6_IRQHandler (void) //    Tx USART5
{
  if (dma_flag_get  (DMA1, DMA_CH6,DMA_FLAG_FTF)!= RESET) {
      dma_flag_clear(DMA1, DMA_CH6,DMA_FLAG_FTF);
     USART5_RX
        usart5_transmit=0;
       dma_channel_disable(DMA1,DMA_CH6);
  }
}
//end File

