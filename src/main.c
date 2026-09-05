
#include "main.h"
#include "netconf.h"
//#include "lwip/tcp.h"
//#include "lwip/timeouts.h"
//#include "lwip/udp.h"
//#include "lwip/ip_addr.h"
//#include "lwip/init.h"
//#include "netif/etharp.h"
//#include "gd32f4xx_timer.h"
  



  __IO uint32_t g_localtime = 0; /* for creating a time reference incremented by 10ms */
  uint32_t g_timedelay;

extern ip_addr_t yv_ip;

/// переменная для переключения пакетов отправки по ethernet
//extern volatile uint8_t current_tick_trasmition;
//// буферы для отправки по ethernet
//extern volatile YLSToYVPacket YLSToYVPacket1,YLSToYVPacket2;

//// буферы для отправки по usart
extern uint8_t message_from_BKD ;

//// счетчики* для адресации процессоров по usart
//extern volatile uint8_t current_BKD                  ;
//extern volatile uint8_t current_device_in_BKDx       ;
//extern volatile uint8_t current_device_in_BKHO_A     ;
//extern volatile uint8_t current_device_in_OUT_SERVO  ;



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
  
  Rcu_config();
  Gpio_config();
  
  IP4_ADDR( &     yv_ip,      YV_ADDR0,      YV_ADDR1,      YV_ADDR2,      YV_ADDR3 ); // или ipaddr_aton(IP_ADDR, &ip) #define IP_ADDR "192.168.1.188"
  
  enet_system_setup();
   /* initilaize the LwIP stack */
  lwip_stack_init();
  
  if (udp_create_socket() != 0) {
    //  printf("UDP demo initialization failed\n");      
    NVIC_SystemReset();
  }

  ConfigUsart (USART0,USART_BAUDRATE,USART_MSBF_LSB,USART_OVSMOD_8,receiveRBNE,0,3);
  ConfigUsartDMA_Tx (USART0, &message_from_BKD,USART_BUFFER_TX_SIZE_YLS,0,DMA_PRIORITY_HIGH,0,6,iRQn_full_transmit_Dma);
  ConfigUsart (USART5,USART_BAUDRATE,USART_MSBF_LSB,USART_OVSMOD_8,receiveRBNE,0,3);
  ConfigUsartDMA_Tx (USART5, &message_from_BKD,USART_BUFFER_TX_SIZE_YLS,0,DMA_PRIORITY_HIGH,0,6,iRQn_full_transmit_Dma);

 // Настройка таймеров
    
  Timer2_config();  // End rx usart0
  Timer4_config();  // End rx usart5
  Timer3_config();  // PWM From work rs485
  Timer6_config();// ( Qwerty usart )
    
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



//end File
