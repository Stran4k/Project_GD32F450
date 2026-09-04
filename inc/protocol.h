#ifndef PROTOCOL_H
#define PROTOCOL_H
/*
 * Version 0.15
 *
 * #include "protocol.h"
 * 04.08.26
 *  
 *
 *
 *
 */
#include "main.h"


//#define mSTATIC_ASSERT(expr) typedef uint8_t static_assert_failed[(expr) ? 1 : -1]

#define MAGIC_YV      (0x4B43) // передать сообщение ЯВ
#define MAGIC_KASU_KP (0x4B41)// передать сообщение КАСУ-КП


#define SIZE_HEALTH_ABONENT		(30)


#define SIZE_MESSAGE_YLS  (8192)
#define SIZE_MESSAGE_YV   (80)

//================================================================================  USART  ================================================================================  
#define USART_BAUDRATE    (5000000)
// ==================== ПАКЕТ ЯЛС БКХО-А > БКД (16 байт) =========================
#define USART_BUFFER_TX_SIZE_YLS  (20)
typedef struct __attribute__((packed))
{
  uint8_t adress;                   // [    0    ] адрес ячейки
  uint8_t command;                  // [    1    ] Команда
  uint8_t yps_mask;                 // [    2    ] Маска пиросредств 
  int16_t ykp_angel_1;              // [   3-4   ] 1 угл поворота
  int16_t ykp_angel_2;              // [   5-6   ] 2 угл поворота
  uint8_t reserv1[12];              // [   7-18  ]
  uint8_t src;                      // [   19    ] crc8
}YLSToBKDPacket;
//mSTATIC_ASSERT(sizeof(YLSToBKDPacket) == USART_BUFFER_TX_SIZE_YLS);

// ==================== ПАКЕТ     ЯКП  БКД > ЯЛС БКХО-А  =========================
#define USART_BUFFER_TX_SIZE_YKP  (32)
typedef struct __attribute__((packed))
{
  int16_t ykp_angel_X2;              // [   0-1   ] 1 угл поворота
  int16_t ykp_angel_X3;              // [   2-3   ] 2 угл поворота
  uint8_t reserv1[27];               // [   7-14  ]
  uint8_t src;                       // [   31    ] crc8
}YKPToYLSPacket;
//mSTATIC_ASSERT(sizeof(YKPToYLSPacket) == USART_BUFFER_TX_SIZE_YKP);
// ==================== ПАКЕТ     ЯПС  БКД > ЯЛС БКХО-А  =========================
#define USART_BUFFER_TX_SIZE_YPS  (32)
typedef struct __attribute__((packed))
{
  uint8_t mask;                      // [    0    ] crc8
  uint8_t reserv1[30];               // [   7-14  ]
  uint8_t src;                       // [   31    ] crc8
}YPSToYLSPacket;
//mSTATIC_ASSERT(sizeof(YPSToYLSPacket) == USART_BUFFER_TX_SIZE_YPS);
// ==================== ПАКЕТ     ЯАЗ  БКД > ЯЛС БКХО-А  =========================
#define USART_BUFFER_TX_SIZE_YAZ  (32)
typedef struct __attribute__((packed))
{
  uint16_t engine_revs1;              // [   0-1   ] Обороты двигателя измерене 1
  uint16_t engine_revs2;              // [   2-3   ] Обороты двигателя измерене 2
  uint16_t engine_revs3;              // [   4-5   ] Обороты двигателя измерене 3
  uint16_t engine_revs4;              // [   6-7   ] Обороты двигателя измерене 4
  uint16_t engine_revs5;              // [   8-9   ] Обороты двигателя измерене 5

  uint16_t tp_data1_2_3;              // [  10-11  ] ТП Датчик №1 (МК2: №2, МК3: №3). Вход 1 
  uint16_t tp_data1_2_3_;             // [  12-13  ] ТП Датчик №1 (МК2: №2, МК3: №3). Вход 2 
  uint16_t tp_data4;                  // [  14-15  ] ТП Датчик №4 (МК3: №3 )          Вход 1
  uint16_t tp_data5;                  // [  16-17  ] ТП Датчик №5 (МК3: №3 )          Вход 2
  
    uint8_t  danger     :1;           // [    18   ] Признак аварии
    uint8_t  reserv1    :1;           // [    18   ]
    uint8_t  pressure20 :3;           // [    18   ] Давление 20
    uint8_t  pressure30 :3;           // [    18   ] Давление 30
    
    uint8_t  rotation_deviation1 :1;  // [    19   ] Признак отличия оборотов между датчиками более, чем на 300 (МК1: 2 и 1, МК2: 1 и 2, МК3: 1 и 3)
    uint8_t  rotation_deviation2 :1;  // [    19   ] Признак отличия оборотов между датчиками более, чем на 300 (МК1: 3 и 1, МК2: 3 и 2, МК3: 2 и 3)
    uint8_t  rotation_deviation3 :1;  // [    19   ] Признак отличия оборотов между датчиками более, чем на 300 (МК1: 4 и 1, МК2: 4 и 2, МК3: 4 и 3)
    uint8_t  reserv2             :1;  // [    19   ] 
    uint8_t  control_pressure20  :1;  // [    19   ] Признак контроля давлений 20 
    uint8_t  control_pressure30  :1;  // [    19   ] Признак контроля давлений 30 
    uint8_t  control_temper      :1;  // [    19   ] Признак контроля температур 
    uint8_t  control_revs        :1;  // [    19   ] Признак контроля оборотов
    
  uint32_t time_ms;                   // [  20-23  ] время

  uint8_t reserv[7];                  // [   7-14  ]

  uint8_t src;                        // [    24   ] crc8
}YAZToYLSPacket;
//mSTATIC_ASSERT(sizeof(YAZToYLSPacket) == USART_BUFFER_TX_SIZE_YAZ);
// ==================== ПАКЕТ   ЯЛК-96 БКД > ЯЛС БКХО-А  =========================
#define USART_BUFFER_TX_SIZE_YLK_96  (32)
typedef struct __attribute__((packed))
{
  uint8_t reserv1[USART_BUFFER_TX_SIZE_YLK_96-1];               // [     ]
  uint8_t src;                      // [   31    ] crc8
}YLK_96ToYLSPacket;
//mSTATIC_ASSERT(sizeof(YLK_96ToYLSPacket) == USART_BUFFER_TX_SIZE_YLK_96);
// ==================== ПАКЕТ ЯЛК-32-3 БКД > ЯЛС БКХО-А  =========================
#define USART_BUFFER_TX_SIZE_YLK32_3  (32)
typedef struct __attribute__((packed))
{
  uint8_t reserv1[USART_BUFFER_TX_SIZE_YLK32_3-1];                 // [     ]
  uint8_t src;                      // [   31    ] crc8 
}YLK_32_3ToYLSPacket;
//mSTATIC_ASSERT(sizeof(YLK_32_3ToYLSPacket) == USART_BUFFER_TX_SIZE_YLK32_3);
// ==================== ПАКЕТ   ЯВП    БКД > ЯЛС БКХО-А  =========================
#define USART_BUFFER_TX_SIZE_YVP  (32)
typedef struct __attribute__((packed))
{
  uint8_t reserv[USART_BUFFER_TX_SIZE_YVP-1];               // [   7-14  ]
  uint8_t src;                      // [   31    ] crc8
}YVPToYLSPacket;
//mSTATIC_ASSERT(sizeof(YVPToYLSPacket) == USART_BUFFER_TX_SIZE_YVP);
// ==================== ПАКЕТ   ЯТП    БКД > ЯЛС БКХО-А  =========================
#define USART_BUFFER_TX_SIZE_YTP  (32)
typedef struct __attribute__((packed))
{
  uint8_t reserv1[USART_BUFFER_TX_SIZE_YTP-1];  // [         ]
  uint8_t src;                                  // [   31    ] crc8
}YTPToYLSPacket;
//mSTATIC_ASSERT(sizeof(YTPToYLSPacket) == USART_BUFFER_TX_SIZE_YTP);
// ==================== ПАКЕТ   ЯТМ    БКД > ЯЛС БКХО-А  =========================
#define USART_BUFFER_TX_SIZE_YTM  (32)
typedef struct __attribute__((packed))
{
  uint8_t reserv1[USART_BUFFER_TX_SIZE_YTM-1];  // [   7-14  ]
  uint8_t src;                                  // [    1    ] crc8
}YTMToYLSPacket;
//mSTATIC_ASSERT(sizeof(YTMToYLSPacket) == USART_BUFFER_TX_SIZE_YTM);
// ==================== ПАКЕТ   ЯГР    БКД > ЯЛС БКХО-А  =========================
#define USART_BUFFER_TX_SIZE_YGR  (32)
typedef struct __attribute__((packed))
{
  uint8_t reserv1[USART_BUFFER_TX_SIZE_YGR-1];  // [   7-14  ]
  uint8_t src;                                  // [    1    ] crc8
}YGRToYLSPacket;
//mSTATIC_ASSERT(sizeof(YGRToYLSPacket) == USART_BUFFER_TX_SIZE_YGR);
//================================================================================


//================================================================================  Ethernet ================================================================================  
// ==================== ПАКЕТ ЯВ>ЯЛС (80 байт) ==================================
enum YV_COMMANDS{
  YV_COMMAND_           = 0x00,
  YV_COMMAND_TEST1      = 0x01,
  YV_COMMAND_TEST2      = 0x02,
  YV_COMMAND_FAST_ANSWER= 0x10,
  YV_COMMAND_SLOW_ANSWER= 0x20
};
typedef struct __attribute__((packed))
{
  uint8_t  yps_mask_BKHO_A;   // [   0   ] Маска пиросредств для БКХО-А
  uint8_t  yps_mask [12];     // [  1-12 ] Маски пиросредств для 12 блоков
  int16_t  ykp_angel[12][2];  // [ 13-60 ] 32 Угла для 12 блоков 
  int16_t  out_ykp_angel[8];  // [ 61-76 ] 32 Угла 12 блоков c каждого из 3х  процессоров
  uint8_t  command;           // [  77   ] Маска пиросредств для БКХО-А
}YVToYLSData;
typedef struct __attribute__((packed))
{
  YVToYLSData data;
  uint8_t  reserv[SIZE_MESSAGE_YV - sizeof(YVToYLSData)];        // [ X - 79 ] Резерв
}YVToYLSPacket;

//mSTATIC_ASSERT(SIZE_MESSAGE_YV == sizeof(YVToYLSPacket));

// ==================== ПАКЕТ ЯЛС БКХО-А > ЯВ (8192 байт) ========================
//#define MAP_HEALFS

typedef struct __attribute__((packed))
{
                                    // BKD 
  //  ykp
  uint16_t  ykp_poc_1   :12;//0
  uint16_t  ykp_poc_2   :12;//0
  uint16_t  ykp_poc_3   :12;//0

  //  yaz
  uint16_t  yaz_poc_1   :12;//0
  uint16_t  yaz_poc_2   :12;//0
  uint16_t  yaz_poc_3   :12;//0
  
  //  yps
  uint16_t  yps_poc_1   :12;//0
  uint16_t  yps_poc_2   :12;//0
  uint16_t  yps_poc_3   :12;//0
  
  //  ylk96
  uint16_t  ylk96_poc_1 :12;//0
  uint16_t  ylk96_poc_2 :12;//0
  uint16_t  ylk96_poc_3 :12;//0
  
                                    // BKHO_A 
  //  yps
  uint16_t  yps_bkho_a_poc_1  :1;//0
  uint16_t  yps_bkho_a_poc_2  :1;//0
  uint16_t  yps_bkho_a_poc_3  :1;//0
  //  ytp1
  uint16_t  ytp1_bkho_a_poc_1 :1;//0
  uint16_t  ytp1_bkho_a_poc_2 :1;//0
  uint16_t  ytp1_bkho_a_poc_3 :1;//0
  //  ytp2
  uint16_t  ytp2_bkho_a_poc_1 :1;//0
  uint16_t  ytp2_bkho_a_poc_2 :1;//0
  uint16_t  ytp2_bkho_a_poc_3 :1;//0
  //  ytm1
  uint16_t  ytm1_bkho_a_poc_1 :1;//0
  uint16_t  ytm1_bkho_a_poc_2 :1;//0
  uint16_t  ytm1_bkho_a_poc_3 :1;//0
  //  ytm2
  uint16_t  ytm2_bkho_a_poc_1 :1;//0
  uint16_t  ytm2_bkho_a_poc_2 :1;//0
  uint16_t  ytm2_bkho_a_poc_3 :1;//0
  //  ygr
  uint16_t  ygr_bkho_a_poc_1  :1;//0
  uint16_t  ygr_bkho_a_poc_2  :1;//0
  uint16_t  ygr_bkho_a_poc_3  :1;//0
  //  ylk-32-3
  uint16_t  ylk32_3_bkho_a_poc_1  :1;//0
  uint16_t  ylk32_3_bkho_a_poc_2  :1;//0
  uint16_t  ylk32_3_bkho_a_poc_3  :1;//0
  //  yvp
  uint16_t  yvp_bkho_a_poc_1  :1;//0
  uint16_t  yvp_bkho_a_poc_2  :1;//0
  uint16_t  yvp_bkho_a_poc_3  :1;//0
  
                      // Out servo driver
  uint8_t  out_servo8 :1;//21
  uint8_t  out_servo7 :1;
  uint8_t  out_servo6 :1;
  uint8_t  out_servo5 :1;
  uint8_t  out_servo4 :1;
  uint8_t  out_servo3 :1;
  uint8_t  out_servo2 :1;
  uint8_t  out_servo1 :1;                    

}health_bit_map;


typedef struct __attribute__((packed))
{
                      // BKD 
//ykp
  uint8_t  ykp_8_1  :1;//0
  uint8_t  ykp_7_1  :1;
  uint8_t  ykp_6_1  :1;
  uint8_t  ykp_5_1  :1;
  uint8_t  ykp_4_1  :1;
  uint8_t  ykp_3_1  :1;
  uint8_t  ykp_2_1  :1;
  uint8_t  ykp_1_1  :1;
  
  uint8_t  ykp_4_2  :1;//1
  uint8_t  ykp_3_2  :1;
  uint8_t  ykp_2_2  :1;
  uint8_t  ykp_1_2  :1;
  uint8_t  ykp_12_1 :1;
  uint8_t  ykp_11_1 :1;
  uint8_t  ykp_10_1 :1;
  uint8_t  ykp_9_1  :1;
  
  uint8_t  ykp_12_2 :1;//2
  uint8_t  ykp_11_2 :1;
  uint8_t  ykp_10_2 :1;
  uint8_t  ykp_9_2  :1;
  uint8_t  ykp_8_2  :1;
  uint8_t  ykp_7_2  :1;
  uint8_t  ykp_6_2  :1;
  uint8_t  ykp_5_2  :1;
  
  uint8_t  ykp_8_3  :1;//3
  uint8_t  ykp_7_3  :1;
  uint8_t  ykp_6_3  :1;
  uint8_t  ykp_5_3  :1;
  uint8_t  ykp_4_3  :1;
  uint8_t  ykp_3_3  :1;
  uint8_t  ykp_2_3  :1;
  uint8_t  ykp_1_3  :1;
  
  uint8_t  ykp_12_3 :1;//4
  uint8_t  ykp_11_3 :1;
  uint8_t  ykp_10_3 :1;
  uint8_t  ykp_9_3  :1;
//yaz
  uint8_t  yaz_4_1  :1;
  uint8_t  yaz_3_1  :1;
  uint8_t  yaz_2_1  :1;
  uint8_t  yaz_1_1  :1;
  
  uint8_t  yaz_12_1 :1;//5
  uint8_t  yaz_11_1 :1;
  uint8_t  yaz_10_1 :1;
  uint8_t  yaz_9_1  :1;
  uint8_t  yaz_8_1  :1;
  uint8_t  yaz_7_1  :1;
  uint8_t  yaz_6_1  :1;
  uint8_t  yaz_5_1  :1;
  
  uint8_t  yaz_8_2  :1;//6
  uint8_t  yaz_7_2  :1;
  uint8_t  yaz_6_2  :1;
  uint8_t  yaz_5_2  :1;
  uint8_t  yaz_4_2  :1;
  uint8_t  yaz_3_2  :1;  
  uint8_t  yaz_2_2  :1;
  uint8_t  yaz_1_2  :1;
  
  uint8_t  yaz_4_3  :1;//7
  uint8_t  yaz_3_3  :1;
  uint8_t  yaz_2_3  :1;
  uint8_t  yaz_1_3  :1;
  uint8_t  yaz_12_2 :1;
  uint8_t  yaz_11_2 :1;
  uint8_t  yaz_10_2 :1;
  uint8_t  yaz_9_2  :1;
  
  uint8_t  yaz_12_3 :1;//8
  uint8_t  yaz_11_3 :1;
  uint8_t  yaz_10_3 :1;
  uint8_t  yaz_9_3  :1;
  uint8_t  yaz_8_3  :1;
  uint8_t  yaz_7_3  :1;
  uint8_t  yaz_6_3  :1;
  uint8_t  yaz_5_3  :1;
//yps
  uint8_t  yps_8_1  :1;//9
  uint8_t  yps_7_1  :1;
  uint8_t  yps_6_1  :1;
  uint8_t  yps_5_1  :1;
  uint8_t  yps_4_1  :1;
  uint8_t  yps_3_1  :1;
  uint8_t  yps_2_1  :1;
  uint8_t  yps_1_1  :1;
  
  uint8_t  yps_4_2  :1;//10
  uint8_t  yps_3_2  :1;
  uint8_t  yps_2_2  :1;
  uint8_t  yps_1_2  :1;
  uint8_t  yps_12_1 :1;
  uint8_t  yps_11_1 :1;
  uint8_t  yps_10_1 :1;
  uint8_t  yps_9_1  :1;
  
  uint8_t  yps_12_2 :1;//11
  uint8_t  yps_11_2 :1;
  uint8_t  yps_10_2 :1;
  uint8_t  yps_9_2  :1;
  uint8_t  yps_8_2  :1;
  uint8_t  yps_7_2  :1;
  uint8_t  yps_6_2  :1;
  uint8_t  yps_5_2  :1;
  
  uint8_t  yps_8_3  :1;//12
  uint8_t  yps_7_3  :1;
  uint8_t  yps_6_3  :1;
  uint8_t  yps_5_3  :1;
  uint8_t  yps_4_3  :1;
  uint8_t  yps_3_3  :1;
  uint8_t  yps_2_3  :1;
  uint8_t  yps_1_3  :1;
  
  uint8_t  yps_12_3 :1;//13
  uint8_t  yps_11_3 :1;
  uint8_t  yps_10_3 :1;
  uint8_t  yps_9_3  :1;
//ylk96
  uint8_t  ylk96_4_1 :1;
  uint8_t  ylk96_3_1 :1;
  uint8_t  ylk96_2_1 :1;
  uint8_t  ylk96_1_1 :1;
  
  uint8_t  ylk96_12_1:1;//14
  uint8_t  ylk96_11_1:1;
  uint8_t  ylk96_10_1:1;
  uint8_t  ylk96_9_1 :1;
  uint8_t  ylk96_8_1 :1;
  uint8_t  ylk96_7_1 :1;
  uint8_t  ylk96_6_1 :1;
  uint8_t  ylk96_5_1 :1;
  
  uint8_t  ylk96_8_2 :1;//15
  uint8_t  ylk96_7_2 :1;
  uint8_t  ylk96_6_2 :1;
  uint8_t  ylk96_5_2 :1;
  uint8_t  ylk96_4_2 :1;
  uint8_t  ylk96_3_2 :1;
  uint8_t  ylk96_2_2 :1;
  uint8_t  ylk96_1_2 :1;
  
  uint8_t  ylk96_4_3 :1;//16
  uint8_t  ylk96_3_3 :1;
  uint8_t  ylk96_2_3 :1;
  uint8_t  ylk96_1_3 :1;
  uint8_t  ylk96_12_2:1;
  uint8_t  ylk96_11_2:1;
  uint8_t  ylk96_10_2:1;
  uint8_t  ylk96_9_2 :1;
  
  uint8_t  ylk96_12_3:1;//17
  uint8_t  ylk96_11_3:1;
  uint8_t  ylk96_10_3:1;
  uint8_t  ylk96_9_3 :1;
  uint8_t  ylk96_8_3 :1;
  uint8_t  ylk96_7_3 :1;
  uint8_t  ylk96_6_3 :1;
  uint8_t  ylk96_5_3 :1;
  
                      // BKHO_A 
  uint8_t  ytp_bkho_a_2_2:1;//18
  uint8_t  ytp_bkho_a_2_1:1;
  uint8_t  ytp_bkho_a_1_3:1;
  uint8_t  ytp_bkho_a_1_2:1;
  uint8_t  ytp_bkho_a_1_1:1;
  uint8_t  yps_bkho_a_3  :1;
  uint8_t  yps_bkho_a_2  :1;
  uint8_t  yps_bkho_a_1  :1;  
  
  uint8_t  ygr_bkho_a_1  :1;//19
  uint8_t  ytm_bkho_a_2_3:1;
  uint8_t  ytm_bkho_a_2_2:1;
  uint8_t  ytm_bkho_a_2_1:1;
  uint8_t  ytm_bkho_a_1_3:1;
  uint8_t  ytm_bkho_a_1_2:1;
  uint8_t  ytm_bkho_a_1_1:1;
  uint8_t  ytp_bkho_a_2_3:1;
  
  uint8_t  yvp_bkho_a_3     :1;//20
  uint8_t  yvp_bkho_a_2     :1;
  uint8_t  yvp_bkho_a_1     :1;
  uint8_t  ylk32_3_bkho_a_3 :1;
  uint8_t  ylk32_3_bkho_a_2 :1;
  uint8_t  ylk32_3_bkho_a_1 :1;
  uint8_t  ygr_bkho_a_3     :1;
  uint8_t  ygr_bkho_a_2     :1;
                      
                      // Out servo driver
  uint8_t  out_servo8 :1;//21
  uint8_t  out_servo7 :1;
  uint8_t  out_servo6 :1;
  uint8_t  out_servo5 :1;
  uint8_t  out_servo4 :1;
  uint8_t  out_servo3 :1;
  uint8_t  out_servo2 :1;
  uint8_t  out_servo1 :1;                    

}health_bit_map_old;
typedef struct __attribute__((packed))
{
  health_bit_map data;
  uint8_t  reserv      [ SIZE_HEALTH_ABONENT - sizeof(health_bit_map) ];  // [ X - 8191 ] 
}health_bit;

typedef struct __attribute__((packed))
{
#if (defined (MAP_HEALFS))
  health_bit health_abonent ;                                             // [   0-30  ] Данные о живых процах 
#else
  //uint8_t    health_abonent [SIZE_HEALTH_ABONENT];                        // [   0-30  ] Данные о живых процах 
  health_bit_map health_abonent ;
#endif
  
  uint8_t  yps_mask_BKHO_A  [3];                                          // [  31-33  ] Маска пиросредств для БКХО-А
  uint8_t  yps_mask         [12][3];                                      // [  34-69  ] Маски пиросредств для 12 блоков
  int16_t  ykp_angel        [12][3][2];                                   // [  70-213 ] 32 Угла 12 блоков c каждого из 3х  процессоров
  int16_t  out_ykp_angel    [8];                                          // [ 214-229 ] 32 Угла 12 блоков c каждого из 3х  процессоров
  uint8_t  yaz_data         [12][3][24];                                  // [ 230-1093] Данные ЯАЗ 
  uint8_t  yvp_data         [2] [3][64];                                  // [3498-1940] Данные ЯВП 8 кналов по 2Б
  uint8_t  ylk32_3_data     [100];                                        // [1094-1193] Данные ЯЛК 32 100
  uint8_t  ylk96_data       [12][3][30];                                  // [1194-3497] Данные ЯЛК 
  
}YLSToYVData;

typedef struct __attribute__((packed))
{
  YLSToYVData data;
  uint8_t  reserv      [ SIZE_MESSAGE_YLS - sizeof(YLSToYVData) ];    // [ X - 8191 ] 
  
}YLSToYVPacket;
//mSTATIC_ASSERT(sizeof(YLSToYVPacket) == SIZE_MESSAGE_YLS);
//================================================================================

//================================================================================


#endif /* PROTOCOL_H */
