#include <includes.h>
#include "app_serv.h"
#include "lcd.h"
#include "spi.h"
#include "keyboard.h"

static  OS_STK     AppTaskStartStk[APP_TASK_START_STK_SIZE];
static  void  AppTaskStart(void  *p_arg);


/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

void  main (void)
{
    BSP_IntDisAll();                                            /* Disable all interrupts until we are ready to accept them */

    OSInit();                                                   /* Initialize "uC/OS-II, The Real-Time Kernel"              */

    OSTaskCreate(AppTaskStart, (void *)0, (OS_STK *)&AppTaskStartStk[APP_TASK_START_STK_SIZE-1], APP_TASK_START_PRIO);
    
    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)       */
}

/*
*********************************************************************************************************
*                                          AppTaskStart()
*
* Description : The startup task.  The uC/OS-II ticker should only be initialize once multitasking starts.
*
* Argument(s) : p_arg       Argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Note(s)     : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*
*               (2) Interrupts are enabled once the task starts because the I-bit of the CCR register was
*                   set to 0 by 'OSTaskCreate()'.
*********************************************************************************************************
*/

unsigned char arr[256];

static  void  AppTaskStart (void *p_arg)
{
    (void)p_arg;

    BSP_Init();                                                 /* Initialize BSP functions                                 */

#if OS_TASK_STAT_EN > 0
    OSStatInit();                                               /* Determine CPU capacity                                   */
#endif

    OSTimeDly(1000);
    // инициализация периферии
    InitLcd();
    InitKbrd();
    SpiInit();
    
    // пользовательская инициализация
    UserStartupFunc();
    
    while (1) {                                          /* Task body, always written as an infinite loop.           */
      OSTimeDly(1000);
      //PostUserEvent(EVENT_SEC); 
    }
}

