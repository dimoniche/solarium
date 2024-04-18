#include <includes.h>
#include "keyboard.h"
#include "app_serv.h"


#define   KBRD_QUERY_LEN      4

OS_STK  KbrdTaskStk[KBRD_TASK_STK_SIZE];
OS_EVENT  *KbrdQuery = NULL;
void     *KbrdTbl[KBRD_QUERY_LEN];


void  KbrdTask(void *p_arg);

unsigned long kbrd_state;

void InitKbrd()
{
 // проинициализируем порты
/*      
P0.28	MK_P24	«1»
P0.27	MK_P25	«2»
P3.26	MK_P26	«3»
P3.25	MK_P27	«a»
P1.18	MK_P32	«b»
P1.19	MK_P33	«c»
*/
  // сканирующие линии
  PINSEL1_bit.P0_28 = 0x0;
  PINSEL1_bit.P0_27 = 0x0;
  PINSEL7_bit.P3_26 = 0x0;

  PINMODE1_bit.P0_28 = 0;
  PINMODE1_bit.P0_27 = 0;
  PINMODE7_bit.P3_26 = 0;
    
  FIO0DIR_bit.P0_28 = 1;
  FIO0DIR_bit.P0_27 = 1;
  FIO3DIR_bit.P3_26 = 1;

  FIO0MASK_bit.P0_28 = 0;
  FIO0MASK_bit.P0_27 = 0;
  FIO3MASK_bit.P3_26 = 0;
  
  // входные линии
  PINSEL3_bit.P1_18 = 0x0;
  PINSEL3_bit.P1_19 = 0x0;
  PINSEL7_bit.P3_25 = 0x0;

  PINMODE3_bit.P1_18 = 0;
  PINMODE3_bit.P1_19 = 0;
  PINMODE7_bit.P3_25 = 0;
    
  FIO1DIR_bit.P1_18 = 0;
  FIO1DIR_bit.P1_19 = 0;
  FIO3DIR_bit.P3_25 = 0;

  FIO1MASK_bit.P1_18 = 0;
  FIO1MASK_bit.P1_19 = 0;
  FIO3MASK_bit.P3_25 = 0;

  // кнопка пользователя  
  PINSEL3_bit.P1_20 = 0x0;
  PINMODE3_bit.P1_20 = 0;
  FIO1DIR_bit.P1_20= 0;
  FIO1MASK_bit.P1_20 = 0;
  
  // кнопки отложенного старта
/*
    P1.25	MK_P39
    P1.23	MK_P37
    P1.31	MK_P20
    P0.25	MK_P7
    P1.24	MK_P38
    P0.26	MK_P6
    P1.29	MK_P45
    P0.1	MK_P47
    P2.13	MK_P50
    P2.11	MK_P52
*/  
  // 1
  PINSEL3_bit.P1_25 = 0x0;
  PINMODE3_bit.P1_25 = 0;
  FIO1DIR_bit.P1_25 = 0;
  FIO1MASK_bit.P1_25 = 0;

  // 2
  PINSEL3_bit.P1_23 = 0x0;
  PINMODE3_bit.P1_23 = 0;
  FIO1DIR_bit.P1_23 = 0;
  FIO1MASK_bit.P1_23 = 0;
  
  // 3
  PINSEL3_bit.P1_31 = 0x0;
  PINMODE3_bit.P1_31 = 0;
  FIO1DIR_bit.P1_31 = 0;
  FIO1MASK_bit.P1_31 = 0;

  // 4
  /*PINSEL1_bit.P0_25 = 0x0;
  PINMODE1_bit.P0_25 = 0;
  FIO0DIR_bit.P0_25 = 0;
  FIO0MASK_bit.P0_25 = 0;*/
  
  // 5
  PINSEL3_bit.P1_24 = 0x0;
  PINMODE3_bit.P1_24 = 0;
  FIO1DIR_bit.P1_24 = 0;
  FIO1MASK_bit.P1_24 = 0;

  // 6
  PINSEL1_bit.P0_26 = 0x0;
  PINMODE1_bit.P0_26 = 0;
  FIO0DIR_bit.P0_26 = 0;
  FIO0MASK_bit.P0_26 = 0;

  // 7
  PINSEL3_bit.P1_29 = 0x0;
  PINMODE3_bit.P1_29 = 0;
  FIO1DIR_bit.P1_29 = 0;
  FIO1MASK_bit.P1_29 = 0;

  // 8
  PINSEL0_bit.P0_1 = 0x0;
  PINMODE0_bit.P0_1 = 0;
  FIO0DIR_bit.P0_1 = 0;
  FIO0MASK_bit.P0_1 = 0;

  // 9
  PINSEL4_bit.P2_13 = 0x0;
  PINMODE4_bit.P2_13 = 0;
  FIO2DIR_bit.P2_13 = 0;
  FIO2MASK_bit.P2_13 = 0;

  // 10
  PINSEL4_bit.P2_11 = 0x0;
  PINMODE4_bit.P2_11 = 0;
  FIO2DIR_bit.P2_11 = 0;
  FIO2MASK_bit.P2_11 = 0;
  
  kbrd_state = 0;
  
  // создадим очередь событий клавиатуры и задачу опроса клавиатуры
  if (KbrdQuery == NULL)
    {
      KbrdQuery = OSQCreate(&KbrdTbl[0], KBRD_QUERY_LEN);
      OSTaskCreate(KbrdTask, (void *)0, (OS_STK *)&KbrdTaskStk[KBRD_TASK_STK_SIZE-1], KBRD_TASK_PRIO);
    }
}

#define KBRD_SCAN_LINE1()  {FIO0CLR_bit.P0_28 = 1; FIO0SET_bit.P0_27 = 1; FIO3SET_bit.P3_26 = 1;}
#define KBRD_SCAN_LINE2()  {FIO0SET_bit.P0_28 = 1; FIO0CLR_bit.P0_27 = 1; FIO3SET_bit.P3_26 = 1;}
#define KBRD_SCAN_LINE3()  {FIO0SET_bit.P0_28 = 1; FIO0SET_bit.P0_27 = 1; FIO3CLR_bit.P3_26 = 1;}

void  KbrdTask(void *p_arg)
{
  unsigned long prew_state = 0;
  
  while (1)
    {
      unsigned long state;
      
      OSTimeDly(17);

      state = 0;      
      KBRD_SCAN_LINE1();
      OSTimeDly(1);
      if (!FIO3PIN_bit.P3_25) state |= (1UL << KEY_F1);
      if (!FIO1PIN_bit.P1_18) state |= (1UL << KEY_F2);
      if (!FIO1PIN_bit.P1_19) state |= (1UL << KEY_F3);
      KBRD_SCAN_LINE2();
      OSTimeDly(1);
      if (!FIO3PIN_bit.P3_25) state |= (1UL << KEY_LEFT);
      if (!FIO1PIN_bit.P1_18) state |= (1UL << KEY_UP);
      if (!FIO1PIN_bit.P1_19) state |= (1UL << KEY_RIGHT);
      KBRD_SCAN_LINE3();
      OSTimeDly(1);
      if (!FIO3PIN_bit.P3_25) state |= (1UL << KEY_STOP);
      if (!FIO1PIN_bit.P1_18) state |= (1UL << KEY_DOWN);
      if (!FIO1PIN_bit.P1_19) state |= (1UL << KEY_START);

      if (!FIO1PIN_bit.P1_20) state |=  (1UL << KEY_USER_START);
      
      if (!FIO1PIN_bit.P1_25) state |=  (1UL << KEY_DEFERRED_CH1);
      if (!FIO1PIN_bit.P1_23) state |=  (1UL << KEY_DEFERRED_CH2);
      if (!FIO1PIN_bit.P1_31) state |=  (1UL << KEY_DEFERRED_CH3);
      //if (!FIO0PIN_bit.P0_25) state |=  (1UL << KEY_DEFERRED_CH4);
      if (!FIO1PIN_bit.P1_24) state |=  (1UL << KEY_DEFERRED_CH5);
      if (!FIO0PIN_bit.P0_26) state |=  (1UL << KEY_DEFERRED_CH6);
      if (!FIO1PIN_bit.P1_29) state |=  (1UL << KEY_DEFERRED_CH7);
      if (!FIO0PIN_bit.P0_1)  state |=  (1UL << KEY_DEFERRED_CH8);
      if (!FIO2PIN_bit.P2_13) state |=  (1UL << KEY_DEFERRED_CH9);
      if (!FIO2PIN_bit.P2_11) state |=  (1UL << KEY_DEFERRED_CH10);

      if (prew_state == state)
        { // устранили дребезг
          if (kbrd_state ^ state)
            { // есть изменения нажатия
              for (unsigned long i = 0; i < KEY_COUNT; i++)
                {
                  // ловим передний фронт при нажатии
                  if (!(kbrd_state & (1UL << i)) && (state & (1UL << i)))
                    {
                      OSQPost(KbrdQuery, (void *)i);
                      PostUserEvent(EVENT_KEY_EMPTY+i); // пользователю тоже постим
                    }
                }
            }
          kbrd_state = state;
        }
      
      prew_state = state;
    }
    
}

int GetKbrdEvent(int* event)
{
  CPU_INT08U err = 0; 
  int evt  = (int)OSQPend(KbrdQuery, 1, &err);
  if (err != 0) return 0;
  *event = evt;
  return 1;  
}   
    