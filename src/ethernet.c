#include "ethernet.h"




extern uint8_t message_from_BKD [IN_TOTAL_BKD+2][USART_BUFFER_TX_SIZE_YLS];

struct  udp_pcb *_udp_pcb_;
 ip_addr_t yv_ip;



/// переменная для переключения пакетов отправки по ethernet
volatile uint8_t current_tick_tx_eth = 0;
// буферы для отправки по ethernet
volatile YLSToYVPacket YLSToYVPack[2];


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
      message_from_BKD [i][4] =(rxpak->data.ykp_angle[i][0]);
      message_from_BKD [i][5] =(rxpak->data.ykp_angle[i][0])>>8;
      message_from_BKD [i][6] =(rxpak->data.ykp_angle[i][1]);
      message_from_BKD [i][7] =(rxpak->data.ykp_angle[i][1])>>8;

      message_from_BKD [i][15]= Crc8(message_from_BKD [i],15);
   }
   
  if(current_tick_tx_eth){
       current_tick_tx_eth=0;     
  }else{
       current_tick_tx_eth=1;
  }  
  #if (defined (TEST_UDP))
        udp_send_buf      ((void*)&YLSToYVPack[current_tick_tx_eth],1432,addr,port);
  #else
        udp_send_buf      ((void*)&YLSToYVPack[current_tick_tx_eth],1432,&yv_ip,YV_PORT);
  #endif

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
	{
 	 if (udp_bind (_udp_pcb_, IP_ADDR_ANY, LOCAL_PORT) == ERR_OK) // bind the local port IP, IP_ADDR_ANY 0, in fact, explain the use of local IP address, use the recommended priority. Because in the case of DHCP, we can not know in advance the IP.
		{
		 udp_recv (_udp_pcb_, udp_reciev, NULL); // register a callback message processing				
		}else
			return 1;
	}
	return 0;
}
// End File
