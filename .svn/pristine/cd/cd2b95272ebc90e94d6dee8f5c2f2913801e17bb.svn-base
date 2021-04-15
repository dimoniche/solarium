#include <includes.h>
#include "control.h"
#include "data.h"
#include "datadesc.h"
#include <stdlib.h>
#include <string.h>

/*
P2.2	MK_P73	Канал 1
P0.9	MK_P76	Канал 2
P0.8	MK_P77	Канал 3
P0.7	MK_P78	Канал 4
P0.6	MK_P79	Канал 5
P0.5	MK_P80	Канал 6
P0.4	MK_P81	Канал 7
P4.28	MK_P82	Канал 8
P1.27	MK_P43	Канал 9
P1.28	MK_P44	Канал 10
*/

CPU_INT16U ChannelStatus; // побитно текущие статусы каналов



// включить канал
void ChannelOn(CPU_INT08U ch)
{
  ChannelStatus |= (1L << ch);
  switch (ch)
    {
      case 0:
        FIO2SET_bit.P2_2 = 1;
        break;
      case 1:
        FIO0SET_bit.P0_9 = 1;
        break;
      case 2:
        FIO0SET_bit.P0_8 = 1;
        break;
      case 3:
        FIO0SET_bit.P0_7 = 1;
        break;
      case 4:
        FIO0SET_bit.P0_6 = 1;
        break;
      case 5:
        FIO0SET_bit.P0_5 = 1;
        break;
      case 6:
        FIO0SET_bit.P0_4 = 1;
        break;
      case 7:
        FIO4SET_bit.P4_28 = 1;
        break;
      case 8:
        FIO1SET_bit.P1_27 = 1;
        break;
      case 9:
        FIO1SET_bit.P1_28 = 1;
        break;
    }    
}

// выключить канал
void ChannelOff(CPU_INT08U ch)
{
  ChannelStatus &= ~(1L << ch);
  switch (ch)
    {
      case 0:
        FIO2CLR_bit.P2_2 = 1;
        break;
      case 1:
        FIO0CLR_bit.P0_9 = 1;
        break;
      case 2:
        FIO0CLR_bit.P0_8 = 1;
        break;
      case 3:
        FIO0CLR_bit.P0_7 = 1;
        break;
      case 4:
        FIO0CLR_bit.P0_6 = 1;
        break;
      case 5:
        FIO0CLR_bit.P0_5 = 1;
        break;
      case 6:
        FIO0CLR_bit.P0_4 = 1;
        break;
      case 7:
        FIO4CLR_bit.P4_28 = 1;
        break;
      case 8:
        FIO1CLR_bit.P1_27 = 1;
        break;
      case 9:
        FIO1CLR_bit.P1_28 = 1;
        break;
    }  
}



// инициализация каналов при старте системы
void InitChannels(void)
{
    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr = 0;
    #endif

    OS_ENTER_CRITICAL();

    // канал 1
    PINSEL4_bit.P2_2 = 0x0;
    PINMODE4_bit.P2_2 = 0;
    FIO2DIR_bit.P2_2 = 1;
    FIO2MASK_bit.P2_2 = 0;
    FIO2CLR_bit.P2_2 = 1;

    // канал 2
    PINSEL0_bit.P0_9 = 0x0;
    PINMODE0_bit.P0_9 = 0;
    FIO0DIR_bit.P0_9 = 1;
    FIO0MASK_bit.P0_9 = 0;
    FIO0CLR_bit.P0_9 = 1;

    // канал 3
    PINSEL0_bit.P0_8 = 0x0;
    PINMODE0_bit.P0_8 = 0;
    FIO0DIR_bit.P0_8 = 1;
    FIO0MASK_bit.P0_8 = 0;
    FIO0CLR_bit.P0_8 = 1;

    // канал 4
    PINSEL0_bit.P0_7 = 0x0;
    PINMODE0_bit.P0_7 = 0;
    FIO0DIR_bit.P0_7 = 1;
    FIO0MASK_bit.P0_7 = 0;
    FIO0CLR_bit.P0_7 = 1;

    // канал 5
    PINSEL0_bit.P0_6 = 0x0;
    PINMODE0_bit.P0_6 = 0;
    FIO0DIR_bit.P0_6 = 1;
    FIO0MASK_bit.P0_6 = 0;
    FIO0CLR_bit.P0_6 = 1;

    // канал 6
    PINSEL0_bit.P0_5 = 0x0;
    PINMODE0_bit.P0_5 = 0;
    FIO0DIR_bit.P0_5 = 1;
    FIO0MASK_bit.P0_5 = 0;
    FIO0CLR_bit.P0_5 = 1;

    // канал 7
    PINSEL0_bit.P0_4 = 0x0;
    PINMODE0_bit.P0_4 = 0;
    FIO0DIR_bit.P0_4 = 1;
    FIO0MASK_bit.P0_4 = 0;
    FIO0CLR_bit.P0_4 = 1;

    // канал 8
    PINSEL9_bit.P4_28 = 0x0;
    PINMODE9_bit.P4_28 = 0;
    FIO4DIR_bit.P4_28 = 1;
    FIO4MASK_bit.P4_28 = 0;
    FIO4CLR_bit.P4_28 = 1;
    
    // канал 9
    PINSEL3_bit.P1_27 = 0x0;
    PINMODE3_bit.P1_27 = 0;
    FIO1DIR_bit.P1_27 = 1;
    FIO1MASK_bit.P1_27 = 0;
    FIO1CLR_bit.P1_27 = 1;

    // канал 10
    PINSEL3_bit.P1_28= 0x0;
    PINMODE3_bit.P1_28 = 0;
    FIO1DIR_bit.P1_28 = 1;
    FIO1MASK_bit.P1_28 = 0;
    FIO1CLR_bit.P1_28 = 1;

    OS_EXIT_CRITICAL();
}


