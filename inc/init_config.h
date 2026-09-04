#ifndef INIT_CONFIG_H
#define INIT_CONFIG_H
/*
  #include "init_config.h"
*/
#include "main.h"

//#define HXTAL_VALUE    ((uint32_t)25000000)
//#define HXTAL_VALUE    ((uint32_t)16000000)


//======================= PWM =======================
  #define PWM_1_AF                GPIO_AF_2 //\  TIMER3 Channel 0
  #define PWM_1_PORT              GPIOB
  #define PWM_1_PIN               GPIO_PIN_6

  #define PWM_2_AF                GPIO_AF_2 //\  TIMER3 Channel 1
  #define PWM_2_PORT              GPIOB
  #define PWM_2_PIN               GPIO_PIN_7
//======================= USART0 =======================
  #define USART0_PORT             GPIOA
  #define USART0_AF               GPIO_AF_7
  #define USART0_TX_PIN           GPIO_PIN_9
  #define USART0_RX_PIN           GPIO_PIN_10
  #define USART0_DIR_PIN          GPIO_PIN_11
//======================= USART5 =======================
  #define USART5_PORT             GPIOC
  #define USART5_AF               GPIO_AF_8
  #define USART5_TX_PIN           GPIO_PIN_6
  #define USART5_RX_PIN           GPIO_PIN_7
  #define USART5_DIR_PIN          GPIO_PIN_8
//=======================  =======================

#define SHIM_PRESC  50 // min 1
#define SHIM_PERIOD 20 // min 10 from stable work 

#define PRESCALER_USAT_TIM (5)  // 50 MHz
#define TIMERATE_USART       ((24*((10000000/USART_BAUDRATE)))-1)// X000 ns => X0 tick timer 5MHz = 57 tick |29

#define USART0_RX          { for(volatile int i = 0; i <66; i++); \
                             gpio_bit_reset( USART0_PORT, USART0_DIR_PIN ); }

#define USART0_TX          { gpio_bit_set  ( USART0_PORT, USART0_DIR_PIN ); \
                             for(volatile int i = 0; i < 4; i++); } 

#define USART5_RX          { for(volatile int i = 0; i <66; i++); \
                             gpio_bit_reset( USART5_PORT, USART5_DIR_PIN ); } 

#define USART5_TX          { gpio_bit_set  ( USART5_PORT, USART5_DIR_PIN ); \
                             for(volatile int i = 0; i < 4; i++); }
                        
 void SettingSystemClock16MHz  (void);
 void SettingSystemClock25MHz  (void);

 void Rcu_config    (void);// ¬ключение тактировани€ 
 void Gpio_config   (void);// инициализаци€ лапок мк
 
 void Timer2_config (void);// конец приЄма данный по usart0
 void Timer3_config (void);// шим RS485 
 void Timer4_config (void);// конец приЄма данный по usart5
 void Timer6_config (void);// опрос по usart 125 us
 

#endif
