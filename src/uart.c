#include "uart.h"

/// переменная для переключения пакетов отправки по ethernet
extern volatile uint8_t current_tick_tx_eth;
// буферы для отправки по ethernet
extern volatile YLSToYVPacket YLSToYVPack[2];
// буферы для отправки по usart
uint8_t message_from_BKD [IN_TOTAL_BKD+2][USART_BUFFER_TX_SIZE_YLS]={0};// IN_TOTAL_BKD+2 -> 12 бкд + бкхо + внешние сервоприводы

// счетчики* для адресации процессоров по usart
volatile uint8_t current_BKD                  = 0;
volatile uint8_t current_device_in_BKDx       = 1;
volatile uint8_t current_device_in_BKHO_A     = 1;
volatile uint8_t current_device_in_OUT_SERVO  = 1;


volatile uint8_t usart5_transmit    = 0;
volatile uint8_t usart5_counter     = 0;
volatile uint8_t usart5_buffer_rx [USART_RX_BUFFER_SIZE] = {0};
volatile _Bool timer4En = 0;// usart5 rx end


volatile uint8_t usart0_transmit    = 0;
volatile uint8_t usart0_counter     = 0;
volatile uint8_t usart0_buffer_rx [USART_RX_BUFFER_SIZE] = {0};
volatile _Bool timer2En = 0;// usart0 rx end

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
  
  YLK32_3_1_BKHO_A = 19+DEVICE_IN_BKD*IN_TOTAL_BKD,
  YLK32_3_2_BKHO_A = 20+DEVICE_IN_BKD*IN_TOTAL_BKD,
  YLK32_3_3_BKHO_A = 21+DEVICE_IN_BKD*IN_TOTAL_BKD,
  
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
                  //  YKP    
  	case YKP1_BKD:
    { 
      YLSToYVPack[current_tick_tx_eth].data.health_abonent.ykp_poc_1 &= ~(1<<(current_BKD));
        break;
    }
    case YKP2_BKD:
    {
      YLSToYVPack[current_tick_tx_eth].data.health_abonent.ykp_poc_2 &= ~(1<<(current_BKD));
  	 break;
    }
  	case YKP3_BKD:
    {
      YLSToYVPack[current_tick_tx_eth].data.health_abonent.ykp_poc_3 &= ~(1<<(current_BKD));
  	 break;
    }
                  //  YAZ
  	case YAZ1_BKD:
    {
      YLSToYVPack[current_tick_tx_eth].data.health_abonent.yaz_poc_1 &= ~(1<<(current_BKD));
  	 break;
    }
  	case YAZ2_BKD:
    {
      YLSToYVPack[current_tick_tx_eth].data.health_abonent.yaz_poc_2 &= ~(1<<(current_BKD));
  	 break;
    }
  	case YAZ3_BKD:
    {
      YLSToYVPack[current_tick_tx_eth].data.health_abonent.yaz_poc_3 &= ~(1<<(current_BKD));
  	 break;
    }
                  //  YPS
  	case YPS1_BKD:
    {
      YLSToYVPack[current_tick_tx_eth].data.health_abonent.yps_poc_1 &= ~(1<<(current_BKD));
  	 break;
    }
  	case YPS2_BKD:
    {
      YLSToYVPack[current_tick_tx_eth].data.health_abonent.yps_poc_2 &= ~(1<<(current_BKD));
  	 break;
    }
  	case YPS3_BKD:
    {
      YLSToYVPack[current_tick_tx_eth].data.health_abonent.yps_poc_3 &= ~(1<<(current_BKD));
  	 break;
    }
                  //  YLK_96_1_BKD1
  	case YLK_96_1_BKD:
    {
      YLSToYVPack[current_tick_tx_eth].data.health_abonent.ylk96_poc_1 &= ~(1<<(current_BKD));
  	 break;
    }
  	case YLK_96_2_BKD:
    {
      YLSToYVPack[current_tick_tx_eth].data.health_abonent.ylk96_poc_2 &= ~(1<<(current_BKD));
  	 break;
    }
  	case YLK_96_3_BKD:
    {
      YLSToYVPack[current_tick_tx_eth].data.health_abonent.ylk96_poc_3 &= ~(1<<(current_BKD));
  	 break;
    }
  	default:
    {
      
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
                  //  YPS_BKHO_A
      case YPS1_BKHO_A:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.yps_bkho_a_poc_1 &= ~(1);
       break;
      }
      case YPS2_BKHO_A:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.yps_bkho_a_poc_2 &= ~(1);
       break;
      }
      case YPS3_BKHO_A:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.yps_bkho_a_poc_3 &= ~(1);
       break;
      }
                  //  YTP
      case YTP1_1_BKHO_A:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.ytp1_bkho_a_poc_1 &= ~(1);
       break;
      }
      case YTP1_2_BKHO_A:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.ytp1_bkho_a_poc_2 &= ~(1);
       break;
      }
      case YTP1_3_BKHO_A:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.ytp1_bkho_a_poc_3 &= ~(1);
       break;
      }
      case YTP2_1_BKHO_A:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.ytp2_bkho_a_poc_1 &= ~(1);
       break;
      }
      case YTP2_2_BKHO_A:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.ytp2_bkho_a_poc_2 &= ~(1);
       break;
      }
      case YTP2_3_BKHO_A:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.ytp2_bkho_a_poc_3 &= ~(1);
       break;
      }
                  //  YTM
      case YTM1_1_BKHO_A:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.ytm1_bkho_a_poc_1 &= ~(1);
       break;
      }
      case YTM1_2_BKHO_A:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.ytm1_bkho_a_poc_2 &= ~(1);
       break;
      }
      case YTM1_3_BKHO_A:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.ytm1_bkho_a_poc_3 &= ~(1);
       break;
      }
      case YTM2_1_BKHO_A:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.ytm2_bkho_a_poc_1 &= ~(1);
       break;
      }
      case YTM2_2_BKHO_A:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.ytm2_bkho_a_poc_2 &= ~(1);
       break;
      }
      case YTM2_3_BKHO_A:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.ytm2_bkho_a_poc_3 &= ~(1);
       break;
      }
                  //  YGR_BKHO_A
      case YGR1_BKHO_A:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.ygr_bkho_a_poc_1 &= ~(1);     
       break;
      }
      case YGR2_BKHO_A:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.ygr_bkho_a_poc_2 &= ~(1);
       break;
      }
      case YGR3_BKHO_A:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.ygr_bkho_a_poc_3 &= ~(1);
       break;
      }
                  //  YLK32_3_BKHO_A
      case YLK32_3_1_BKHO_A:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.ylk32_3_bkho_a_poc_1 &= ~(1);
       break;
      }
      case YLK32_3_2_BKHO_A:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.ylk32_3_bkho_a_poc_2 &= ~(1);
       break;
      }
      case YLK32_3_3_BKHO_A:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.ylk32_3_bkho_a_poc_3 &= ~(1);
       break;
      }
                  //  YVP_BKHO_A
      case YVP1_BKHO_A:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.yvp_bkho_a_poc_1 &= ~(1);
       break;
      }
      case YVP2_BKHO_A:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.yvp_bkho_a_poc_2 &= ~(1);
       break;
      }
      case YVP3_BKHO_A:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.yvp_bkho_a_poc_3 &= ~(1);
       break;
      }

      default:
      {
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
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.out_servo1 &= ~1;
       break;
      }
      case OUT_SERVO_2:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.out_servo2 &= ~1;
       break;
      }
      case OUT_SERVO_3:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.out_servo3 &= ~1;
       break;
      }
      case OUT_SERVO_4:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.out_servo4 &= ~1;
       break;
      }
      case OUT_SERVO_5:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.out_servo5 &= ~1;
       break;
      }
      case OUT_SERVO_6:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.out_servo6 &= ~1;
       break;
      }
      case OUT_SERVO_7:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.out_servo7 &= ~1;
       break;
      }
      case OUT_SERVO_8:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.out_servo8 &= ~1;
       break;
      }
      default:
      {
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
                  //  YKP    
      case YKP1_BKD:
      {
        if(size==USART_BUFFER_TX_SIZE_YKP){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.ykp_poc_1 |= (1<<(current_BKD));
          YLSToYVPack[current_tick_tx_eth].data.ykp_angel[current_BKD][0][0]=(((uint16_t)message[0])<<8)|message[1];
          YLSToYVPack[current_tick_tx_eth].data.ykp_angel[current_BKD][0][1]=(((uint16_t)message[2])<<8)|message[3];
        }
       break;
      }
      case YKP2_BKD:
      {
        if(size==USART_BUFFER_TX_SIZE_YKP){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.ykp_poc_2 |= (1<<(current_BKD));
          YLSToYVPack[current_tick_tx_eth].data.ykp_angel[current_BKD][1][0]=(((uint16_t)message[0])<<8)|message[1];
          YLSToYVPack[current_tick_tx_eth].data.ykp_angel[current_BKD][1][1]=(((uint16_t)message[2])<<8)|message[3];
        }
       break;
      }
      case YKP3_BKD:
      {
        if(size==USART_BUFFER_TX_SIZE_YKP){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.ykp_poc_3 |= (1<<(current_BKD));
          YLSToYVPack[current_tick_tx_eth].data.ykp_angel[current_BKD][2][0]=(((uint16_t)message[0])<<8)|message[1];
          YLSToYVPack[current_tick_tx_eth].data.ykp_angel[current_BKD][2][1]=(((uint16_t)message[2])<<8)|message[3];
        }
       break;
      }
                  //  YAZ
      case YAZ1_BKD:
      {
        if(size==USART_BUFFER_TX_SIZE_YAZ){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.yaz_poc_1 |= (1<<(current_BKD));
             for(uint8_t i=0; i<24;i++){
               YLSToYVPack[current_tick_tx_eth].data.yaz_data[current_BKD][0][i]  = message[i];
             }
        }
       break;
      }
      case YAZ2_BKD:
      {
        if(size==USART_BUFFER_TX_SIZE_YAZ){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.yaz_poc_2 |= (1<<(current_BKD));
             for(uint8_t i=0; i<24;i++){
               YLSToYVPack[current_tick_tx_eth].data.yaz_data[current_BKD][1][i]  = message[i];
             }
        }
       break;
      }
      case YAZ3_BKD:
      {
        if(size==USART_BUFFER_TX_SIZE_YAZ){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.yaz_poc_3 |= (1<<(current_BKD));
             for(uint8_t i=0; i<24;i++){
               YLSToYVPack[current_tick_tx_eth].data.yaz_data[current_BKD][2][i]  = message[i];
             }
        }
       break;
      }
                  //  YPS
      case YPS1_BKD:
      {
        if(size==USART_BUFFER_TX_SIZE_YPS){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.yps_poc_1 |= (1<<(current_BKD));
          YLSToYVPack[current_tick_tx_eth].data.yps_mask[current_BKD][0]=message[0];
        }
       break;
      }
      case YPS2_BKD:
      {
        if(size==USART_BUFFER_TX_SIZE_YPS){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.yps_poc_2 |= (1<<(current_BKD));
          YLSToYVPack[current_tick_tx_eth].data.yps_mask[current_BKD][1]=message[0];
        }
       break;
      }
      case YPS3_BKD:
      {
        if(size==USART_BUFFER_TX_SIZE_YPS){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.yps_poc_3 |= (1<<(current_BKD));
          YLSToYVPack[current_tick_tx_eth].data.yps_mask[current_BKD][2]=message[0];
        }
       break;
      }
                  //  YLK_96
      case YLK_96_1_BKD:
      {
        if(size==USART_BUFFER_TX_SIZE_YLK_96){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.ylk96_poc_1 |= (1<<(current_BKD));
             for(uint8_t i=0; i<24;i++){
               YLSToYVPack[current_tick_tx_eth].data.ylk96_data[current_BKD][0][i]  = message[i];
             }
        }
       break;
      }
      case YLK_96_2_BKD:
      {
        if(size==USART_BUFFER_TX_SIZE_YLK_96){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.ylk96_poc_2 |= (1<<(current_BKD));
             for(uint8_t i=0; i<24;i++){
               YLSToYVPack[current_tick_tx_eth].data.ylk96_data[current_BKD][1][i]  = message[i];
             }
        }
       break;
      }
      case YLK_96_3_BKD:
      {
        if(size==USART_BUFFER_TX_SIZE_YLK_96){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.ylk96_poc_3 |= (1<<(current_BKD));
             for(uint8_t i=0; i<24;i++){
               YLSToYVPack[current_tick_tx_eth].data.ylk96_data[current_BKD][2][i]  = message[i];
             }
        }
       break;
      }
      default:
      {
        break;
      }
    }
 
}

void Check_message_BKHO_A     (uint8_t *message, uint8_t size)
{
      if (message == NULL) return;
      
 switch (current_device_in_BKHO_A)
    {
                  //  YPS_BKHO_A
      case YPS1_BKHO_A:
      {
        if(size==USART_BUFFER_TX_SIZE_YPS){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.yps_bkho_a_poc_1 |= (1);
          YLSToYVPack[current_tick_tx_eth].data.yps_mask_BKHO_A[0]=message[0];
        }
       break;
      }
      case YPS2_BKHO_A:
      {
        if(size==USART_BUFFER_TX_SIZE_YPS){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.yps_bkho_a_poc_2 |= (1);
          YLSToYVPack[current_tick_tx_eth].data.yps_mask_BKHO_A[1]=message[0];
        }
       break;
      }
      case YPS3_BKHO_A:
      {
        if(size==USART_BUFFER_TX_SIZE_YPS){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.yps_bkho_a_poc_3 |= (1);
          YLSToYVPack[current_tick_tx_eth].data.yps_mask_BKHO_A[2]=message[0];
        }
       break;
      }
                  //  YTP
      case YTP1_1_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YTP){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.ytp1_bkho_a_poc_1 |= (1);
       }
       break;
      }
      case YTP1_2_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YTP){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.ytp1_bkho_a_poc_2 |= (1);
       }
       break;
      }
      case YTP1_3_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YTP){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.ytp1_bkho_a_poc_3 |= (1);
       }
       break;
      }
      case YTP2_1_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YTP){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.ytp2_bkho_a_poc_1 |= (1);
       }
       break;
      }
      case YTP2_2_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YTP){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.ytp2_bkho_a_poc_2 |= (1);
       }
       break;
      }
      case YTP2_3_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YTP){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.ytp2_bkho_a_poc_3 |= (1);
       }
       break;
      }
                  //  YTM
      case YTM1_1_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YTM){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.ytm1_bkho_a_poc_1 |= (1);
       }
       break;
      }
      case YTM1_2_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YTM){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.ytm1_bkho_a_poc_2 |= (1);
       }
       break;
      }
      case YTM1_3_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YTM){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.ytm1_bkho_a_poc_3 |= (1);
       }
       break;
      }
      case YTM2_1_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YTM){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.ytm2_bkho_a_poc_1 |= (1);
       }
       break;
      }
      case YTM2_2_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YTM){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.ytm2_bkho_a_poc_2 |= (1);
       }
       break;
      }
      case YTM2_3_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YTM){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.ytm2_bkho_a_poc_3 |= (1);
       }
       break;
      }
                  //  YGR_BKHO_A
      case YGR1_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YGR){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.ygr_bkho_a_poc_1 |= (1);
       }
       break;
      }
      case YGR2_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YGR){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.ygr_bkho_a_poc_2 |= (1);
       }
       break;
      }
      case YGR3_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YGR){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.ygr_bkho_a_poc_3 |= (1);
       }
       break;
      }
                  //  YLK_32_3_BKHO_A
      case YLK32_3_1_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YLK32_3){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.ylk32_3_bkho_a_poc_1 |= (1);
       }
       break;
      }
      case YLK32_3_2_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YLK32_3){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.ylk32_3_bkho_a_poc_2 |= (1);
       }
       break;
      }
      case YLK32_3_3_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YLK32_3){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.ylk32_3_bkho_a_poc_3 |= (1);
       }
       break;
      }
                  //  YVP_BKHO_A
      case YVP1_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YVP){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.yvp_bkho_a_poc_1 |= (1);
       }
       break;
      }
      case YVP2_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YVP){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.yvp_bkho_a_poc_2 |= (1);
       }
       break;
      }
      case YVP3_BKHO_A:
      {
       if(size==USART_BUFFER_TX_SIZE_YVP){
          YLSToYVPack[current_tick_tx_eth].data.health_abonent.yvp_bkho_a_poc_3 |= (1);
       }
       break;
      }
      default:
      {
        break;
      }
    }
   
}
void Check_message_OUT_SERVO  (uint8_t *message, uint8_t size)
{
      if (message == NULL) return;
      
   switch (current_device_in_OUT_SERVO)
    {
      case OUT_SERVO_1:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.out_servo1 |= 1;
       break;
      }
      case OUT_SERVO_2:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.out_servo2 |= 1;
       break;
      }
      case OUT_SERVO_3:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.out_servo3 |= 1;
       break;
      }
      case OUT_SERVO_4:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.out_servo4 |= 1;
       break;
      }
      case OUT_SERVO_5:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.out_servo5 |= 1;
       break;
      }
      case OUT_SERVO_6:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.out_servo6 |= 1;
       break;
      }
      case OUT_SERVO_7:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.out_servo7 |= 1;
       break;
      }
      case OUT_SERVO_8:
      {
        YLSToYVPack[current_tick_tx_eth].data.health_abonent.out_servo8 |= 1;
       break;
      }
      default:
      {
        break;
      }
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
void DMA1_Channel7_IRQHandler (void) //    Tx End USART0
{
  if (dma_flag_get  (DMA1, DMA_CH7,DMA_FLAG_FTF)!= RESET) {
      dma_flag_clear(DMA1, DMA_CH7,DMA_FLAG_FTF);
     USART0_RX
        usart0_transmit=0;
       dma_channel_disable(DMA1,DMA_CH7);
  }
}
void DMA1_Channel6_IRQHandler (void) //    Tx End USART5
{
  if (dma_flag_get  (DMA1, DMA_CH6,DMA_FLAG_FTF)!= RESET) {
      dma_flag_clear(DMA1, DMA_CH6,DMA_FLAG_FTF);
     USART5_RX
        usart5_transmit=0;
       dma_channel_disable(DMA1,DMA_CH6);
  }
}
unsigned char Crc8 (unsigned char *pcBlock, unsigned char size)
{
    unsigned char crc = 0xFF;
 
    while (size--)
        crc = Crc8Table[crc ^ *pcBlock++];
 
    return crc;
}

// End File
