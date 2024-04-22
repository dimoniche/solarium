#include "iolpc2368.h"
#include "ucos_ii.h"
#include "cpu.h"
#include "app_serv.h"
#include "coin.h"
#include "data.h"
#include "datadesc.h"
#include "modem.h"


OS_STK  CoinTaskStk[COIN_TASK_STK_SIZE];

void  InitImpInput(void);

CPU_INT32U  CoinImpCounter;

CPU_INT08U  coin_enabled;

void CoinTask(void *p_arg)
{
  
  while(1)
    {
      CPU_INT32U enable_coin;
      OSTimeDly(100);
      GetData(&EnableCoinDesc, &enable_coin, 0, DATA_FLAG_SYSTEM_INDEX);
      
      if (enable_coin && coin_enabled)
        {
          // для разрешения монетника выставим низкий уровень
          FIO0CLR_bit.P0_24  = 1;
          
          if (GetCoinCount())
          {
            PostUserEvent(EVENT_COIN_INSERTED);
          }
        }
      else
       {
          // для запрета монетника выставим высокий уровень
          FIO0SET_bit.P0_24  = 1;

          CoinDisable();
          GetResetCoinCount();
        }
    }

}

void CoinDisable(void)
{
    coin_enabled = 0;
}

void CoinEnable(void)
{
    coin_enabled = 1;
}

// получить число монет
CPU_INT32U GetCoinCount(void)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  CPU_INT32U ctr = CoinImpCounter;
  OS_EXIT_CRITICAL();
  return ctr;
}

// получить число монет и сбросить счетчик
CPU_INT32U GetResetCoinCount(void)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  CPU_INT32U ctr = CoinImpCounter;
  CoinImpCounter = 0;
  OS_EXIT_CRITICAL();
  return ctr;
}

// инициализация монетоприемника
void InitCoin(void)
{
  CoinImpCounter = 0;
  coin_enabled = 0;

  InitImpInput();
  
  OSTaskCreate(CoinTask, (void *)0, (OS_STK *)&CoinTaskStk[COIN_TASK_STK_SIZE-1], COIN_TASK_PRIO);
}


void InputCapture_ISR(void)
{
  CPU_INT08U ir = T3IR;
  static CPU_INT32U period = 0;
  T3IR = 0xFF;

  if (ir & 0x10)
    {// CR0 interrupt
  
      if (FIO0PIN_bit.P0_25)
        {// пришел задний фронт
          CPU_INT32U cr=T3CR0;
          if (((cr-period) > COIN_IMP_MIN_LEN)
               &&  ((cr-period) < COIN_IMP_MAX_LEN))
            CoinImpCounter++;
        }
      else
        {// пришел передний фронт
          period = T3CR0;
        }
    }
}


extern CPU_INT32U  BSP_CPU_PclkFreq (CPU_INT08U  pclk);

/*
P0.23	MK_P9	IMPULSE OUTPUT (импульсный выход монетоприемника) -> P0.25 MK_P7
P0.24	MK_P8	INHIBIT (блокировка)
*/
// инициализация импульсного входа
// используется CAP3.0
void  InitImpInput (void)
{
    #define INPUT_CAPTURE_FREQ  100000  // частота тактирования частотных входов
  
    CPU_INT32U  pclk_freq;
    CPU_INT32U  rld_cnts;

    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr = 0;
    #endif

    OS_ENTER_CRITICAL();

    PCONP_bit.PCTIM3 = 1;
    PCLKSEL1_bit.PCLK_TIMER3 = 2;

    PINSEL1_bit.P0_23 = 0x3;
    PINMODE1_bit.P0_23 = 0;
    FIO0DIR_bit.P0_23  = 0;
    FIO0MASK_bit.P0_23 = 0;

    PINSEL1_bit.P0_25 = 0x3;
    PINMODE1_bit.P0_25 = 0;
    FIO0DIR_bit.P0_25  = 0;
    FIO0MASK_bit.P0_25 = 0;

    // inhibit
    PINSEL1_bit.P0_24 = 0x0;
    PINMODE1_bit.P0_24 = 0;
    FIO0DIR_bit.P0_24  = 1;
    FIO0MASK_bit.P0_24 = 0;
    FIO0SET_bit.P0_24  = 1;
  
    pclk_freq         =   BSP_CPU_PclkFreq(23);
    rld_cnts          =   pclk_freq / INPUT_CAPTURE_FREQ;

    T3CTCR_bit.CTM    =   0;
    T3CTCR_bit.CIS    =   0;          // select CAP3.0 input
    T3PR              =   rld_cnts-1;
    T3MCR             =   0;
    T3CCR             =   0x07;
    T3EMR             =   0;
    T3TCR             =   0x03;
    T3TCR             =   0x01;
    
    VICINTSELECT     &= ~(1 << VIC_TIMER3);
    VICVECTADDR27     =  (CPU_INT32U)InputCapture_ISR;
    VICVECTPRIORITY27 =  10;                       
    VICINTENABLE      =  (1 << VIC_TIMER3);       

    T3IR = 0xFF;
    
    OS_EXIT_CRITICAL();
}

