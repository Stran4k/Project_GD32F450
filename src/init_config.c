#include "init_config.h"
void SettingSystemClock16MHz  (void)
{
 RCU_CTL |= 1<<16; //включен внешний кварцевый генератор на 16 мГц		
 while((RCU_CTL & (1<<17))==0);	 //ждём пока внешний генератор запустится	
	
 RCU_APB1EN |= 1<<28;  //Enabled PMU clock
 PMU_CTL |= (1<<14)|(1<<15); //LDO output voltage high mode
 	
 /* AHB = SYSCLK = 100 */
 RCU_CFG0 |= RCU_AHB_CKSYS_DIV1; //RCU_AHB_CKSYS_DIV1
 /* APB2 = AHB = 100 */
 RCU_CFG0 |= RCU_APB2_CKAHB_DIV2;  //1 (2 RCU_APB2_CKAHB_DIV2/* APB2 = AHB/2 = 50 */)
 /* APB1 = AHB/2 = 50 */ 
 RCU_CFG0 |= RCU_APB1_CKAHB_DIV4;  //4					
	
//Configure the main PLL, PLL_PSC(1..2MHz. 16/16=1) , PLL_N(1*200) = 200, PLL_P(200/2=100) = 2, PLL_Q(200/9=22.22) = 9 */  //FREQ = 100MHz 
/*!
    \brief    configure the main PLL clock
    \param[in]  pll_src: PLL clock source selection
      \arg        RCU_PLLSRC_IRC16M: select IRC16M as PLL source clock
      \arg        RCU_PLLSRC_HXTAL: select HXTAL as PLL source clock
    \param[in]  pll_psc: the PLL VCO source clock prescaler
      \arg         this parameter should be selected between 2 and 63
    \param[in]  pll_n: the PLL VCO clock multi factor
      \arg        this parameter should be selected between 64 and 500
    \param[in]  pll_p: the PLLP output frequency division factor from PLL VCO clock
      \arg        this parameter should be selected 2,4,6,8
    \param[in]  pll_q: the PLL Q output frequency division factor from PLL VCO clock
      \arg        this parameter should be selected between 2 and 15
    \param[out] none
    \retval     ErrStatus: SUCCESS or ERROR
*/
// rcu_pll_config(RCU_PLLSRC_HXTAL, 8, 320, 2);
                                 // PLL = (16MHz / 16) * 200 / 2 = 100MHz
//    RCU_PLL = 16 | (200<<6) | (0<<16) | (1<<22) | (9<<24);	//   = 100MHz   
    RCU_PLL = 8  | (200<<6) | (0<<16) | (1<<22) | (9<<24);	//   = 200MHz  

                                 // PLL = (16MHz / 4) * 80 / 2 = 160MHz
//    RCU_PLL =  4 | (80<<6)  | (0<<16) | (1<<22) | (9<<24);	//   = 160MHz 

                                 // PLL = (16MHz / 8)  * 60  / 1 = 120MHz 
//    RCU_PLL =  8 | (60<<6)  | (0<<16) | (1<<22) | (9<<24);	//   = 120MHz 
                                 // PLL = (16MHz / 4) * 60 / 1 = 240MHz
//    RCU_PLL = 2 | (60<<6) | (0<<16) | (1<<22) | (9<<24);	//   = 240MHz 	//*/

 RCU_CTL |= 1<<24;  //PLL Enable
 while((RCU_CTL & (1<<25))==0);  //ждём пока PLL станет стабильна	
	
 /* Enable the high-drive to extend the clock frequency to 100 Mhz */
 PMU_CTL |= PMU_CTL_HDEN;
 while((PMU_CS & PMU_CS_HDRF)==0){}
    
 /* select the high-drive mode */
 PMU_CTL |= PMU_CTL_HDS;
 while((PMU_CS & PMU_CS_HDSRF)==0){}  
	 
 /* select PLL as system clock */
 RCU_CFG0 &= ~RCU_CFG0_SCS;
 RCU_CFG0 |= RCU_CKSYSSRC_PLLP;

 /* wait until PLL is selected as system clock */
  while((RCU_CFG0 & RCU_SCSS_PLLP)==0){} 	
}

void SettingSystemClock25MHz  (void)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;
    __IO uint32_t reg_temp;
    
    /* enable HXTAL */
    RCU_CTL |= RCU_CTL_HXTALEN;

    /* wait until HXTAL is stable or the startup time is longer than HXTAL_STARTUP_TIMEOUT */
    do{
        timeout++;
        stab_flag = (RCU_CTL & RCU_CTL_HXTALSTB);
    }while((0U == stab_flag) && (HXTAL_STARTUP_TIMEOUT != timeout));

    /* if fail */
    if(0U == (RCU_CTL & RCU_CTL_HXTALSTB)){
        while(0U == (RCU_CTL & RCU_CTL_HXTALSTB))
        {
        }
    }
         
    RCU_APB1EN |= RCU_APB1EN_PMUEN;
    PMU_CTL |= PMU_CTL_LDOVS;

    /* HXTAL is stable */
    /* AHB = SYSCLK */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
    /* APB2 = AHB/2 */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV2;
    /* APB1 = AHB/4 */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV4;

    /* Configure the main PLL, PSC = 25, PLL_N = 400, PLL_P = 2, PLL_Q = 9 */ 
    RCU_PLL = (25U | (400U << 6U) | (((2U >> 1U) - 1U) << 16U) |
                   (RCU_PLLSRC_HXTAL) | (9U << 24U));

    /* enable PLL */
    RCU_CTL |= RCU_CTL_PLLEN;

    /* wait until PLL is stable */
    while(0U == (RCU_CTL & RCU_CTL_PLLSTB)){
    }
    
    /* Enable the high-drive to extend the clock frequency to 200 Mhz */
    PMU_CTL |= PMU_CTL_HDEN;
    while(0U == (PMU_CS & PMU_CS_HDRF)){
    }
    
    /* select the high-drive mode */
    PMU_CTL |= PMU_CTL_HDS;
    while(0U == (PMU_CS & PMU_CS_HDSRF)){
    } 

    reg_temp = RCU_CFG0;
    /* select PLL as system clock */
    reg_temp &= ~RCU_CFG0_SCS;
    reg_temp |= RCU_CKSYSSRC_PLLP;
    RCU_CFG0 = reg_temp;

    /* wait until PLL is selected as system clock */
    while(0U == (RCU_CFG0 & RCU_SCSS_PLLP)){
    }
}

void Rcu_config     (void)// Включение тактирования 
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);
  
    rcu_periph_clock_enable(RCU_TIMER1);
    rcu_periph_clock_enable(RCU_TIMER2);
    rcu_periph_clock_enable(RCU_TIMER3);
    rcu_periph_clock_enable(RCU_TIMER4);
    rcu_periph_clock_enable(RCU_TIMER6);
  
    rcu_periph_clock_enable(RCU_USART0);
    rcu_periph_clock_enable(RCU_USART5);
  
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_DMA1);
  
}

void Gpio_config    (void)// инициализация лапок мк
{ 
//======================== PWM  ===========================
  gpio_af_set             (PWM_1_PORT, PWM_1_AF,                          PWM_1_PIN); 
  gpio_mode_set           (PWM_1_PORT, GPIO_MODE_AF,  GPIO_PUPD_NONE,     PWM_1_PIN); 
  gpio_output_options_set (PWM_1_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,  PWM_1_PIN);  

  gpio_af_set             (PWM_2_PORT, PWM_1_AF,                          PWM_2_PIN); 
  gpio_mode_set           (PWM_2_PORT, GPIO_MODE_AF,  GPIO_PUPD_NONE,     PWM_2_PIN); 
  gpio_output_options_set (PWM_2_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,  PWM_2_PIN); 

//// ==================== USART0 ====================  
  gpio_mode_set           (USART0_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,  USART0_DIR_PIN);
  gpio_output_options_set (USART0_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,  USART0_DIR_PIN);

  gpio_af_set             (USART0_PORT, USART0_AF,                         USART0_TX_PIN);
  gpio_mode_set           (USART0_PORT, GPIO_MODE_AF,   GPIO_PUPD_PULLUP,  USART0_TX_PIN);
  gpio_output_options_set (USART0_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,  USART0_TX_PIN);
  
  gpio_af_set             (USART0_PORT, USART0_AF,                         USART0_RX_PIN);
  gpio_mode_set           (USART0_PORT, GPIO_MODE_AF,     GPIO_PUPD_NONE,  USART0_RX_PIN); 
  gpio_output_options_set (USART0_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,  USART0_RX_PIN);  
  
USART0_RX;
//// ==================== USART5 ====================  
  gpio_mode_set           (USART5_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,  USART5_DIR_PIN);  
  gpio_output_options_set (USART5_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,  USART5_DIR_PIN);

  gpio_af_set             (USART5_PORT, USART5_AF,                         USART5_TX_PIN);
  gpio_mode_set           (USART5_PORT, GPIO_MODE_AF,   GPIO_PUPD_PULLUP,  USART5_TX_PIN);
  gpio_output_options_set (USART5_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,  USART5_TX_PIN);
  
  gpio_af_set             (USART5_PORT, USART5_AF,                         USART5_RX_PIN);
  gpio_mode_set           (USART5_PORT, GPIO_MODE_AF,     GPIO_PUPD_NONE,  USART5_RX_PIN); 
  gpio_output_options_set (USART5_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,  USART5_RX_PIN); 
  
USART5_RX;
//// ====================  ====================  

}



void Timer3_config  (void)// Fmax - 100 MHz SHIM
{
  timer_deinit  (TIMER3);
  
  timer_oc_parameter_struct timer_ocinit;
  timer_parameter_struct timer_initpara;

/*!              alignedmode       = TIMER_COUNTER
    \brief      set TIMER counter alignment mode
    \param[in]  timer_periph: TIMERx(x=0..4,7)
    \param[in]  aligned:
                only one parameter can be selected which is shown as below:
      \arg        TIMER_COUNTER_EDGE: edge-aligned mode
      \arg        TIMER_COUNTER_CENTER_DOWN: center-aligned and counting down assert mode
      \arg        TIMER_COUNTER_CENTER_UP: center-aligned and counting up assert mode
      \arg        TIMER_COUNTER_CENTER_BOTH: center-aligned and counting up/down assert mode
    \param[out] none
    \retval     none
*/  
  timer_initpara.alignedmode       = TIMER_COUNTER_CENTER_BOTH; // Центрально-совмещенный ШИМ 
  timer_initpara.counterdirection  = TIMER_COUNTER_UP;
  timer_initpara.prescaler         = (SHIM_PRESC  - 1);      // 100 MHz 
  timer_initpara.period            = (uint16_t)(SHIM_PERIOD);      // 0|4000 ~12,5KHz
  timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
  timer_initpara.repetitioncounter = 0;
  timer_init(TIMER3, &timer_initpara);

  // Настройка каналов PWM
  timer_ocinit.outputstate  = TIMER_CCX_ENABLE;
  timer_ocinit.outputnstate = TIMER_CCXN_DISABLE;

  timer_ocinit.ocpolarity   = TIMER_OC_POLARITY_HIGH;
  timer_ocinit.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;

  timer_ocinit.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
  timer_ocinit.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
  
  // 0 2000  990/0  1000/1 = -1.63
  // 0 4000 1980/0  2100/1 25MHz work

//10 180 99/1 88/1 Ygr 50 MHz

  // Канал 1 PWM_1_PIN
  /* CH0 configuration in PWM mode0 */
  timer_channel_output_config             (TIMER3, TIMER_CH_0, &timer_ocinit);
  timer_channel_output_pulse_value_config (TIMER3, TIMER_CH_0, (SHIM_PERIOD/2 ) );  // 0|4000 1980=55% заполнение 11
  timer_channel_output_mode_config        (TIMER3, TIMER_CH_0, TIMER_OC_MODE_PWM0);
  timer_channel_output_shadow_config      (TIMER3, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);

  // Канал 2 PWM_2_PIN
  /* CH1 configuration in PWM mode1 */
  timer_channel_output_config             (TIMER3, TIMER_CH_1, &timer_ocinit);
  timer_channel_output_pulse_value_config (TIMER3, TIMER_CH_1, (SHIM_PERIOD/2 ) );  // 0|4000 2100=45% заполнение 9
  timer_channel_output_mode_config        (TIMER3, TIMER_CH_1, TIMER_OC_MODE_PWM1);
  timer_channel_output_shadow_config      (TIMER3, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);

  timer_primary_output_config             (TIMER3, ENABLE);// shim output on pins
  timer_auto_reload_shadow_enable         (TIMER3);

  // enable TIMER
  timer_enable(TIMER3);  //*/
}
void Timer2_config  (void) // Rx end USART0
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    /* TIMER1 configuration */
    //timer_initpara.prescaler         = (50-1); //   Fmax - 100 MHz
    timer_initpara.prescaler         = (PRESCALER_USAT_TIM-1); //   Fmax - 100 MHz
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = (TIMERATE_USART);  //  
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;//?
  
    nvic_irq_enable           (TIMER2_IRQn, 0,4);
    timer_interrupt_enable    (TIMER2,TIMER_INT_UP);
    timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);
    timer_enable              (TIMER2); 
  
}
void Timer4_config  (void) // Rx end USART5
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    /* TIMER4 configuration */
    timer_initpara.prescaler         = (PRESCALER_USAT_TIM-1); //   Fmax - 100 MHz
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = (TIMERATE_USART);  //  
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
  
    timer_init                (TIMER4, &timer_initpara);
    nvic_irq_enable           (TIMER4_IRQn, 0,4);
    timer_interrupt_enable    (TIMER4, TIMER_INT_UP);
    timer_interrupt_flag_clear(TIMER4, TIMER_INT_FLAG_UP);
    timer_enable              (TIMER4); 
  
}
void Timer6_config  (void)// Qwerty 125 us 
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    /* TIMER1 configuration */
    timer_initpara.prescaler         = (100-1);//(625-1); //   Fmax - 100 MHz
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = (125);
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init                (TIMER6, &timer_initpara);
    nvic_irq_enable           (TIMER6_IRQn, 0,2);
    timer_interrupt_enable    (TIMER6, TIMER_INT_UP);
    timer_interrupt_flag_clear(TIMER6, TIMER_INT_FLAG_UP);
    timer_enable              (TIMER6); 
  
}

// End File
