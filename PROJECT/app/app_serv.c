#include <includes.h>
#include "app_serv.h"
#include "modem.h"
#include "validator.h"
#include "coin.h"
#include "time.h"
#include "fiscal.h"
#include "menu.h"
#include "data.h"
#include "mode.h"
#include "menudesc.h"
#include "datadesc.h"
#include "control.h"
#include "validator.h"
#include "CCRSProtocol.h"
#include "menu.h"
#include "journal.h"
#include "fr.h"
#include "CRC16.h"
#include "modem_task.h"

// если определить этот макрос, будут вноситься деньги по кнопке F1
//#define _DEBUG_MONEY

CPU_INT32U SystemTime;
CPU_INT08U EnabledChannelsNum;
CPU_INT08U RecentChannel;
CPU_INT08U UserMenuState;
  #define USER_STATE_FIRST_PAGE                 0
  #define USER_STATE_ACCEPT_MONEY               1
  #define USER_STATE_SHOW_THANKS                2
CPU_INT08U ThanksCtr;

CPU_INT08U ChannelsState[CHANNELS_NUM];
  #define CHANNEL_STATE_EMPTY           0
  #define CHANNEL_STATE_PAUSE_BEFORE    1
  #define CHANNEL_STATE_WORK            2
  #define CHANNEL_STATE_PAUSE_AFTER     3
CPU_INT32U ChannelsCounters[CHANNELS_NUM];
CPU_INT32U ChannelsPayedTime[CHANNELS_NUM];
CPU_INT32U ChannelsWaitDeffered[CHANNELS_NUM] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

extern CPU_INT32U BillNominals[24];
CPU_INT32U incas_bill_nom_counter[24];
CPU_INT32U incas_common_bill_counter;

CPU_INT08U max_msg = 0;

#define USER_QUERY_LEN  64

OS_STK    UserTaskStk[USER_TASK_STK_SIZE];
OS_EVENT *UserQuery = NULL;
void     *UserTbl[USER_QUERY_LEN];

int GetUserEvent(int* event);
void UserPrintMoneyMenu(void);
void IncRecentChannel(void);
void DecRecentChannel(void);
void WorkServer(void);
void UserPrintThanksMenu(void);
void UserPrintFirstMenu(CPU_INT08U recentchannel);
void UserPrintErrorMenu(void);
CPU_INT32U GetChannelsTimeForFree(CPU_INT08U ch);
void LoadAcceptedMoney(void);
void SetAcceptedMoney(CPU_INT32U money);
void ClearAcceptedMoney(void);
CPU_INT32U GetAcceptedMoney(void);
void SetAcceptedBankMoney(CPU_INT32U money);
void ClearAcceptedBankMoney(void);
CPU_INT32U GetAcceptedBankMoney(void);
void InitPass(void);
void UserPrintDeferredWaitMenu(int channel);

static char incassation;
static char was_critical_error;
/*!
 Сервер обработки событий пользователя
*/
void UserAppTask(void *p_arg)
{
  int event;
  CPU_INT32U temp;
  static CPU_INT08U fr_conn_ctr = 0;
  // время последнего внесения денег по импульсному входу
  static CPU_INT32U time_last_insert_coin = 0;

  {
    CPU_INT32U m=0;
    GetData(&AcceptedMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);     
    if (m)
    {
         EnabledChannelsNum = 0;
         for (CPU_INT08U i=0; i<CHANNELS_NUM; i++)
         {
            CPU_INT32U en = 0;
            GetData(&EnableChannelDesc, &en, i, DATA_FLAG_DIRECT_INDEX);
            if (en) {EnabledChannelsNum++;}
         }
         UserMenuState = USER_STATE_ACCEPT_MONEY;
      
    }
  }
    
  incassation = 0;
  was_critical_error = 0;
  
  GetData(&IncasSendFlagDesc, &temp, 0, DATA_FLAG_SYSTEM_INDEX);
  if (temp == INCAS_SEND_FLAG)
  {
      PostModemTask(MODEM_TASK_SEND_INCAS);
  }
  
  while (1)
    {
      if (GetUserEvent(&event))
        {
          switch (event){
            
            case EVENT_SEC:
              // проверка режима
              CheckMode();
             
              // прочитаем текущее время
              SystemTime = GetTimeSec();
                
              // рабочий сервер - счетчики, состояния и т.п.
              WorkServer();

              // проверим фискальник, если он отвалился
              if ((++fr_conn_ctr % 10) == 0)
              {
                 if ((FiscalConnState == FISCAL_NOCONN) || (TstCriticalFiscalError()))
                 {
                    if (ConnectFiscalFast() == 0)
                    {
                        CheckFiscalStatus();
                    }
                 }
              }

              // сервер ошибок
              ErrorServer();
                
              // дальше только в рабочем режиме
              if (GetMode() != MODE_WORK) break;
                
              // если есть ошибки, не работаем
              if (TstCriticalErrors()) 
              {
                UserPrintErrorMenu(); 
                RefreshMenu(); 
                LED_off();
                if (UserMenuState == USER_STATE_ACCEPT_MONEY)
                {
                    // выключим прием денег, если в меню приема денег
                    if (was_critical_error == 0)
                    {
                        if (IsValidatorConnected()) CC_CmdBillType(0x000000, 0x000000, ADDR_FL);
                        CoinDisable();
                        was_critical_error = 1;
                    }
                }
                else
                {
                    was_critical_error = 0;
                }
                break;
              }

              if (ChannelsWaitDeffered[RecentChannel] != 0)
              {
                  UserPrintDeferredWaitMenu(RecentChannel);
                  RefreshMenu();
                  LED_off();
              }
              else if (UserMenuState == USER_STATE_ACCEPT_MONEY)
                { // принимаем деньги
                  UserPrintMoneyMenu();
                  RefreshMenu();
                  CheckFiscalStatus();
                  // включим заново прием денег, если была ошибка в этом же меню
                  if (was_critical_error)
                  {
                    if (IsValidatorConnected()) CC_CmdBillType(0xffffff, 0xffffff, ADDR_FL);
                    CPU_INT32U price=1, pricetime=0, maxtime = 0xffffffff, accmoney = GetAcceptedMoney() + GetAcceptedBankMoney();
                    GetRecentChannelPrice(RecentChannel, &price, &pricetime);
                    GetData(&MaxWorkTimeDesc, &maxtime, RecentChannel, DATA_FLAG_DIRECT_INDEX);
                    
                    if ((pricetime*accmoney*60)/price > maxtime*60)
                    {
                      CoinDisable();
                    }
                    else
                    {
                      CoinEnable();
                    }
                    was_critical_error = 0;
                  }
                }
              else if (UserMenuState == USER_STATE_SHOW_THANKS)
                { // показываем "СПАСИБО"
                  LED_off();
                  CoinDisable();
                  if (ThanksCtr) {UserPrintThanksMenu(); ThanksCtr--;}
                  else 
                    {
                      UserMenuState = USER_STATE_FIRST_PAGE;
                      UserPrintFirstMenu(RecentChannel);
                      RefreshMenu();
                    }
                }
              else if (UserMenuState == USER_STATE_FIRST_PAGE)
                { // обновляем первую страницу
                  LED_off(); 
                  CoinDisable();
                  UserPrintFirstMenu(RecentChannel);
                  RefreshMenu(); 
                }
              break;
            case EVENT_INCASSATION:
              {
                  CPU_INT32U incas_sum = 0, temp;              
                  for (CPU_INT32U i = 0; i < 24; i++)
                  {
                      CPU_INT32U val = 0;
                      GetData(&BillnomCountersDesc, &val, i, DATA_FLAG_DIRECT_INDEX);
                      incas_sum += val*BillNominals[i];
                  }              
                  incassation = 1;
                  sprintf((char*)str_IncasMenu_3, "  СУММА %u руб.", incas_sum);
                  // вешаем меню инкассация
                  GoToMenu(IncasMenuPanel);
                  // сохраним событие с указанием суммы денег
                  SaveEventRecord(0, JOURNAL_EVENT_INCASSATION, incas_sum);
                  GetData(&BillCounterDesc, &incas_common_bill_counter, 0, DATA_FLAG_SYSTEM_INDEX);
                  for (CPU_INT32U i = 0; i < 24; i++)
                  {
                     GetData(&BillnomCountersDesc, &incas_bill_nom_counter[i], i, DATA_FLAG_DIRECT_INDEX);
                  }
                  
                  SetData(&IncasMoneyDesc, &incas_sum, 0, DATA_FLAG_SYSTEM_INDEX);
                  
                  temp = GetTimeSec();
                  SetData(&IncasTimeDesc, &temp, 0, DATA_FLAG_SYSTEM_INDEX);
                  
                  temp = INCAS_SEND_FLAG;
                  SetData(&IncasSendFlagDesc, &temp, 0, DATA_FLAG_SYSTEM_INDEX);
                  PostModemTask(MODEM_TASK_SEND_INCAS);
                  // очищаем счетчики купюр
                  ClearBillnomCounter();
              }
              break;
            case EVENT_INCASSATION_FINISH:
              incassation = 0;
              GoToPreviousMenu();
              break;
            case EVENT_MODE_CHANGE:
              ReInitMenu();
              SaveEventRecord(0, JOURNAL_EVENT_CHANGE_MODE, GetMode());
              break;
            case EVENT_COIN_INSERTED:
              {
                CPU_INT32U cpp = 1;
                CPU_INT32U money, accmoney;
                GetData(&CoinPerPulseDesc, &cpp, 0, DATA_FLAG_SYSTEM_INDEX);
                
                // время внесения монеты (+1 дополнительная пауза против ложного срабатывания в случае если секунда произойдет сразу же)
                time_last_insert_coin = GetTimeSec() + 1;

                money = cpp*GetResetCoinCount();
                accmoney = GetAcceptedBankMoney();
                accmoney += money;
                SetAcceptedBankMoney(accmoney);
                
                if (UserMenuState == USER_STATE_ACCEPT_MONEY)
                {
                  UserPrintMoneyMenu();  
                  RefreshMenu();
                }
                
                CPU_INT32U price=1, pricetime=0, maxtime = 0xffffffff;
                GetRecentChannelPrice(RecentChannel, &price, &pricetime);
                GetData(&MaxWorkTimeDesc, &maxtime, RecentChannel, DATA_FLAG_DIRECT_INDEX);
                
                // добавим еще и наличные
                accmoney += GetAcceptedMoney();
                
                if ((pricetime*accmoney*60)/price > maxtime*60)
                  {
                    CoinDisable();
                  }
              }
              break;
            case EVENT_BILL_ESCROW:
              // купюра в положении возврата
              {
                CPU_INT32U billnom_index;
                CPU_INT32U billnom = GetResetBillCount(&billnom_index);
                CPU_INT32U price=1, pricetime=0, maxtime = 0xffffffff, accmoney;
                
                GetRecentChannelPrice(RecentChannel, &price, &pricetime);
                GetData(&MaxWorkTimeDesc, &maxtime, RecentChannel, DATA_FLAG_DIRECT_INDEX);
                accmoney = GetAcceptedMoney() + GetAcceptedBankMoney();
                
                if ((pricetime*(accmoney+billnom)*60)/price > maxtime*60)
                  {
                    max_msg = 3;
                    if (IsValidatorConnected()) if (!CC_CmdReturn(ADDR_FL)) SetErrorFlag(ERROR_VALIDATOR_CONN);
                  }
                else
                  {
                    max_msg = 0;
                    if (IsValidatorConnected()) if (!CC_CmdPack(ADDR_FL)) SetErrorFlag(ERROR_VALIDATOR_CONN);
                  }
              }
              break;
            case EVENT_BILL_STACKED:
              // купюра уложена
              {
                CPU_INT32U billnom_index;
                CPU_INT32U note,accmoney;
                note = GetResetBillCount(&billnom_index);
                accmoney = GetAcceptedMoney();
                accmoney += note;
                SetAcceptedMoney(accmoney);
                if (UserMenuState == USER_STATE_ACCEPT_MONEY)
                {
                  UserPrintMoneyMenu();  
                  RefreshMenu();
                }
                if (IsValidatorConnected()) CC_CmdBillType(0xffffff, 0xffffffff, ADDR_FL);
                SaveEventRecord(RecentChannel, JOURNAL_EVENT_MONEY_NOTE, note);
                IncBillnomCounter(billnom_index);
              }
              break;                  
          
            case EVENT_KEY_F1:
              #ifdef _DEBUG_MONEY 
              {
                  CPU_INT32U accmoney;
                  accmoney = GetAcceptedMoney();
                  accmoney += 10;
                  SetAcceptedMoney(accmoney);
                  if (UserMenuState == USER_STATE_ACCEPT_MONEY)
                  {
                      UserPrintMoneyMenu();  
                      RefreshMenu();
                  }
              }
              #endif
              break;
            case EVENT_KEY_F2:
              break;
            case EVENT_KEY_F3:
              break;

            case EVENT_KEY_LEFT:
              if ((GetMode() != MODE_WORK) || (incassation)) break;
              if (TstCriticalErrors()) {UserPrintErrorMenu(); RefreshMenu(); break;}
              
              if ((UserMenuState == USER_STATE_FIRST_PAGE) && (EnabledChannelsNum > 1))
              {
                  DecRecentChannel();
                  if (ChannelsWaitDeffered[RecentChannel] != 0)
                  {
                      UserPrintDeferredWaitMenu(RecentChannel);
                      RefreshMenu();
                  }
                  else
                  {                 
                      UserPrintFirstMenu(RecentChannel);
                      RefreshMenu();
                  }
              }
              break;
            case EVENT_KEY_RIGHT:
              if ((GetMode() != MODE_WORK) || (incassation)) break;
              if (TstCriticalErrors()) {UserPrintErrorMenu(); RefreshMenu(); break;}

              if ((UserMenuState == USER_STATE_FIRST_PAGE) && (EnabledChannelsNum > 1))
              {
                  IncRecentChannel();
                  if (ChannelsWaitDeffered[RecentChannel] != 0)
                  {
                      UserPrintDeferredWaitMenu(RecentChannel);
                      RefreshMenu();
                  }
                  else
                  {                 
                      UserPrintFirstMenu(RecentChannel);
                      RefreshMenu();
                  }              
              }
              break;
            case EVENT_KEY_DOWN:
              break;
            case EVENT_KEY_UP:
              break;

            case EVENT_KEY_STOP:
              if ((GetMode() != MODE_WORK) || (incassation)) break;
              if (TstCriticalErrors()) {UserPrintErrorMenu(); RefreshMenu(); break;}
              UserMenuState = USER_STATE_FIRST_PAGE;
              UserPrintFirstMenu(RecentChannel);
              RefreshMenu();
              if (IsValidatorConnected()) CC_CmdBillType(0x000000, 0x000000, ADDR_FL);
              break;
            case EVENT_KEY_DEFERRED_CH1:
            case EVENT_KEY_DEFERRED_CH2:
            case EVENT_KEY_DEFERRED_CH3:
            case EVENT_KEY_DEFERRED_CH4:
            case EVENT_KEY_DEFERRED_CH5:
            case EVENT_KEY_DEFERRED_CH6:
            case EVENT_KEY_DEFERRED_CH7:
            case EVENT_KEY_DEFERRED_CH8:
            case EVENT_KEY_DEFERRED_CH9:
            case EVENT_KEY_DEFERRED_CH10:
              if (ChannelsWaitDeffered[event - EVENT_KEY_DEFERRED_CH1] != 0)
              {
                  // переходим к работе
                  ChannelsWaitDeffered[event - EVENT_KEY_DEFERRED_CH1] = 0;
                  ChannelOn(event - EVENT_KEY_DEFERRED_CH1);
                  ChannelsCounters[event - EVENT_KEY_DEFERRED_CH1] = ChannelsPayedTime[event - EVENT_KEY_DEFERRED_CH1];
                  ChannelsState[event - EVENT_KEY_DEFERRED_CH1] = CHANNEL_STATE_WORK;  
                  // запишем событие
                  SaveEventRecord(event - EVENT_KEY_DEFERRED_CH1, JOURNAL_EVENT_START_SESSION, ChannelsPayedTime[event - EVENT_KEY_DEFERRED_CH1]);
                  // повесим меню "СПАСИБО"                      
                  if (IsFiscalConnected())
                    {
                      UserPrintThanksMenu();
                      RefreshMenu();
                      UserMenuState = USER_STATE_SHOW_THANKS;
                    }
                  else
                    {
                      UserMenuState = USER_STATE_FIRST_PAGE;
                    }
                  ThanksCtr = 3;
                  LED_off();
              }
              break;
            case EVENT_KEY_START:
            case EVENT_KEY_USER_START:
              if (incassation) break;
              if (GetMode() != MODE_WORK)
                {
                  if (!FlagForPrintReport) break;
                  if (GetCurrentMenu() == xReportMenuPanel)
                    { // печатаем X-отчет
                     CPU_INT08U err;
                     if (IsFiscalConnected())
                      {
                        FPend();
                        FiscPrintDayReportNoClear(30, &err);
                        FPost();
                        if (err) {SetFiscalErrorByCode(err);}
                        SaveEventRecord(0, JOURNAL_EVENT_PRINT_X, GetTimeSec());
                        GoToPreviousMenu();
                      }
                    }
                  else if (GetCurrentMenu() == zReportMenuPanel)
                   { // печатаем Z-отчет
                     CPU_INT08U err;
                     if (IsFiscalConnected()) 
                      {
                        FPend();
                        FiscPrintDayReportClear(30, &err);
                        FPost();
                        if (err) {SetFiscalErrorByCode(err);}
                        SaveEventRecord(0, JOURNAL_EVENT_PRINT_Z, GetTimeSec());
                        GoToPreviousMenu();
                        ClrFiscalErrorByCode(FR_ERROR_CODE_4E);
                      }
                   }
                  else if (GetCurrentMenu() == bufReportMenuPanel)
                   { // печатаем Z-отчеты из буфера
                     CPU_INT08U err;
                     if (IsFiscalConnected()) 
                      {
                        FPend();
                        FiscPrintDayReportsFromBuf(30, &err);
                        FPost();
                        if (err) {SetFiscalErrorByCode(err);}
                        SaveEventRecord(0, JOURNAL_EVENT_PRINT_BUF, GetTimeSec());
                        GoToPreviousMenu();
                        ClrFiscalErrorByCode(FR_ERROR_CODE_4D);
                      }
                   }
                   else if (GetCurrentMenu() == CanselCheckMenuPanel)
                      {
                        int res = CanselFiscalBill();
                        SaveEventRecord(0, JOURNAL_EVENT_PRINT_X, res);
                        CheckFiscalStatus();
                        GoToPreviousMenu();
                      }
                  break;
                }
            
              if (TstCriticalErrors()) 
              {
                UserPrintErrorMenu(); 
                RefreshMenu(); 
                break;
              }
              
              // --------------------------
              // находимся в рабочем режиме
              // --------------------------
              
              if (EnabledChannelsNum == 0) break;              
                 
              if (UserMenuState == USER_STATE_FIRST_PAGE)
                { // пользователь выбрал канал и нажал СТАРТ
                  if (IsChannelOn(RecentChannel)) break;
                  FPend();
                  if ((CheckFiscalStatus() < 0) && (TstCriticalErrors()))
                  {
                    FPost();
                    UserPrintErrorMenu();
                    RefreshMenu();
                    break;
                  }
                  FPost();
                  UserMenuState = USER_STATE_ACCEPT_MONEY;
                  UserPrintMoneyMenu();
                  RefreshMenu();
                  if (IsValidatorConnected()) CC_CmdBillType(0xffffff, 0xffffffff, ADDR_FL);
                  CoinEnable();
                  max_msg = 0;
                  break;
                }

              // еще не прошло время с последнего внесения монеты (больше секунды должно пройти)
              if(GetTimeSec() <= time_last_insert_coin) break;

              if (UserMenuState == USER_STATE_ACCEPT_MONEY)
                { // пользователь внес деньги и нажал СТАРТ
                  CPU_INT32U price=1, pricetime=0, mintime=0,accmoney,accmoneyBank;
                  GetRecentChannelPrice(RecentChannel, &price, &pricetime);
                  
                  accmoney = GetAcceptedMoney();
                  accmoneyBank = GetAcceptedBankMoney();
                  
                  ChannelsPayedTime[RecentChannel] = (pricetime*(accmoney + accmoneyBank)*60)/price;
                  GetData(&MinWorkTimeDesc, &mintime, RecentChannel, DATA_FLAG_DIRECT_INDEX);
                  
                  if (ChannelsPayedTime[RecentChannel] >= mintime*60)
                    { // оплаченное время больше минимума, можно работать
                      if (IsValidatorConnected()) CC_CmdBillType(0x000000, 0x000000, ADDR_FL);
                      CoinDisable();

                      if(accmoney)
                      {
                          // напечатаем чек - наличка
                          if (IsFiscalConnected())
                          {
                              if (PrintFiscalBill(accmoney, (pricetime*accmoney*60)/price, 0, price, pricetime) == 0)
                              {
                                  SaveEventRecord(RecentChannel, JOURNAL_EVENT_PRINT_BILL, GetTimeSec());
                              }
                              else
                              {
                                  // ошибка печати чека
                                  
                              }
                          }
                      }
 
                      if(accmoneyBank)
                      {
                          SaveEventRecord(RecentChannel, JOURNAL_EVENT_MONEY_COIN, accmoneyBank);

                          // напечатаем чек - банк
                          if (IsFiscalConnected())
                          {
                              if (PrintFiscalBill(accmoneyBank, (pricetime*accmoneyBank*60)/price, 1, price, pricetime) == 0)
                              {
                                  SaveEventRecord(RecentChannel, JOURNAL_EVENT_PRINT_BILL_ONLINE, GetTimeSec());
                              }
                              else
                              {
                                  // ошибка печати чека
                                  
                              }
                          }
                      }

                      IncCounter(RecentChannel, ChannelsPayedTime[RecentChannel], accmoney, accmoneyBank);
                      SetAcceptedMoney(0);
                      SetAcceptedBankMoney(0);
                      
                      // грузим в счетчик время паузу до
                                   
                      CPU_INT32U deferred = 0;
                      GetData(&DeferredStartDesc, &deferred, RecentChannel, DATA_FLAG_DIRECT_INDEX);
                      if (deferred)
                      {
                          ChannelsCounters[RecentChannel] = 0;
                          ChannelsState[RecentChannel] = CHANNEL_STATE_PAUSE_BEFORE;
                          ChannelsWaitDeffered[RecentChannel] = 1;
                          LED_off();
                          UserMenuState = USER_STATE_FIRST_PAGE;
                      }
                      else
                      {
                          ChannelsWaitDeffered[RecentChannel] = 0;
                          GetData(&TimeOutBeforeDesc, &ChannelsCounters[RecentChannel], RecentChannel, DATA_FLAG_DIRECT_INDEX);
                          ChannelsState[RecentChannel] = CHANNEL_STATE_PAUSE_BEFORE;
                          // повесим меню "СПАСИБО"                      
                          if (IsFiscalConnected())
                            {
                              UserPrintThanksMenu();
                              RefreshMenu();
                              UserMenuState = USER_STATE_SHOW_THANKS;
                            }
                          else
                            {
                              UserMenuState = USER_STATE_FIRST_PAGE;
                            }
                          ThanksCtr = 5;
                      }
                    }
                  else
                    { // денег недостаточно для работы
                      
                    
                    
                    }
                  break;
                }
                
                  
              break;
              
          }
        }
      else 
        {
          OSTimeDly(1);
        }
    }
}

/*!
 Пользовательская инициализация
*/
void UserStartupFunc(void)
{
  // инициализация режима работы
  InitMode();
    
  // инициализация данных
  CheckAllData();
    
  // проверим длинные счетчики
  CheckLongCounters();
      
  // восстановим деньги
  LoadAcceptedMoney();
  
  // проверим пароль
  InitPass();
      
  //нициализация каналов
  InitChannels();
    
  // инициализация подсветки
  PINSEL3_bit.P1_21 = 0x0;
  PINMODE3_bit.P1_21 = 0;
  FIO1DIR_bit.P1_21= 1;
  FIO1MASK_bit.P1_21 = 0;
  LED_off();
  
  // инициализация меню
  InitMenu();

  OSTimeDly(1000);
  
  // запустим валидатор
  StartUpValidator();
  
  OSTimeDly(1000);
  InitFiscal();
  
  // проинициализируем часы
  InitRTC();

  // сделаем запись о включении
  SaveEventRecord(0, JOURNAL_EVENT_DEVICE_ON, GetTimeSec());

  // запустим монетник
  InitCoin();
  
  // создадим очередь и задачу
  if (UserQuery == NULL)
    {    
      UserQuery = OSQCreate(&UserTbl[0], USER_QUERY_LEN);
      OSTaskCreate(UserAppTask, (void *)0, (OS_STK *)&UserTaskStk[USER_TASK_STK_SIZE-1], USER_TASK_PRIO);
    }

  SystemTime = GetTimeSec();
  
  // перейдем в стартовое меню, если работа
  if (GetMode() == MODE_WORK) {SetMenu(WORK_MENU);}
  else SetMenu(SERVICE_MENU);
}

int GetUserEvent(int* event)
{
  CPU_INT08U err = 0; 
  int evt  = (int)OSQPend(UserQuery, 1, &err);
  if (err != 0) return 0;
  *event = evt;
  return 1;  
}   


void PostUserEvent(int event)
{
  OSQPost(UserQuery, (void *)event);
}


void InitUserMenu(void)
{
  for (RecentChannel=0; RecentChannel<CHANNELS_NUM; RecentChannel++)
  {
    CPU_INT32U en = 0;
    GetData(&EnableChannelDesc, &en, RecentChannel, DATA_FLAG_DIRECT_INDEX);
    if (en) {break;}
  }
}

void UserPrintDeferredWaitMenu(int channel)
{
  if (ChannelsWaitDeffered[channel] != 0)
  {
      char buf[32];
      if (EnabledChannelsNum > 1)
      {
          GetDataStr(&NameChannelDesc, (CPU_INT08U*)buf, channel, DATA_FLAG_DIRECT_INDEX);
          sprintf(&buf[strlen(buf)], " %d", channel+1);
      }
      else
      {
          sprintf(buf, " ");
      }
      PrintUserMenuStr(buf, 0);
      sprintf(buf, "ОЖИДАНИЕ");
      PrintUserMenuStr(buf, 1);
      sprintf(buf, "ВКЛЮЧЕНИЯ");
      PrintUserMenuStr(buf, 2);
    
      if (EnabledChannelsNum > 1)
      {
          sprintf(buf, "ВЫБЕРИТЕ ДР.КАНАЛ");
      }
      else
      {
          sprintf(buf, " ");
      }
      PrintUserMenuStr(buf, 3);
  }
}

void UserPrintMoneyMenu(void)
{
  char buf[32];
  CPU_INT32U accmoney;
    
  sprintf(buf, "Внесите деньги");
  PrintUserMenuStr(buf, 0);
  accmoney = GetAcceptedMoney() + GetAcceptedBankMoney();
  sprintf(buf, "Принято %d руб.", accmoney);
  PrintUserMenuStr(buf, 1);

  CPU_INT32U price=1, pricetime=0, time=0;
  GetRecentChannelPrice(RecentChannel, &price, &pricetime);
  time = (pricetime*accmoney*60)/price;
  sprintf(buf, "Оплачено %d:%02d", time/60, time%60);
  PrintUserMenuStr(buf, 2);

  if (max_msg)
    {
      --max_msg;
      sprintf(buf, "ПРЕВЫШЕНО МАКС.ВР.");
    }
  else
    {
      CPU_INT32U mintime=0, maxtime=0;
      GetData(&MinWorkTimeDesc, &mintime, RecentChannel, DATA_FLAG_DIRECT_INDEX);  
      GetData(&MaxWorkTimeDesc, &maxtime, RecentChannel, DATA_FLAG_DIRECT_INDEX);  
      if (EnabledChannelsNum > 1) sprintf(buf, "КАН.%d ", RecentChannel+1);
      else sprintf(buf, "");
      if (time >= mintime*60)
        {
          LED_on();
          if (time >= maxtime*60) sprintf(&buf[strlen(buf)], "МАКС.СЕАНС");
          else sprintf(&buf[strlen(buf)], "АППАРАТ ГОТОВ");
        }
      else
        {
          LED_off();
          sprintf(&buf[strlen(buf)], "НЕДОСТ.ДЕНЕГ");
        }
    }
  PrintUserMenuStr(buf, 3);
}

// вывод меню о невозможости работы
void UserPrintErrorMenu(void)
{
  char buf[32];
  
  if (TstErrorFlag(ERROR_VALIDATOR_CONN) || TstCriticalValidatorErrors())
    {
      sprintf(buf, "ОШИБКА");
      PrintUserMenuStr(buf, 0);
      sprintf(buf, "КУПЮРОПРИЕМНИКА");
      PrintUserMenuStr(buf, 1);
      if (TstErrorFlag(ERROR_VALIDATOR_CONN)) 
        {
          sprintf(buf, "НЕТ СВЯЗИ");
          PrintUserMenuStr(buf, 2);
          sprintf(buf, "");
          PrintUserMenuStr(buf, 3);
        }
    }
  else if (TstErrorFlag(ERROR_FR_CONN))
    {
      sprintf(buf, "ОШИБКА");
      PrintUserMenuStr(buf, 0);
      sprintf(buf, "НЕТ СВЯЗИ С ФР");
      PrintUserMenuStr(buf, 1);
      sprintf(buf, "");
      PrintUserMenuStr(buf, 2);
      PrintUserMenuStr(buf, 3);
    }
  else if (TstCriticalFiscalError())
    {
      sprintf(buf, "ОШИБКА");
      PrintUserMenuStr(buf, 0);
      CPU_INT08U errcode = 0;
      sprintf(buf, "ОШИБКА ФР");
      PrintUserMenuStr(buf, 1);
      GetFirstCriticalFiscalError(&errcode);
      GetDataItem(&JournalErrorNumberDesc0, (CPU_INT08U*)buf, errcode);
      PrintUserMenuStr(buf, 2);
      GetDataItem(&JournalErrorNumberDesc1, (CPU_INT08U*)buf, errcode);
      PrintUserMenuStr(buf, 3);
    }
  /*  
  else if (!FReportTest())
    {
      sprintf(buf, "ПЕЧАТb ОТЧЕТА");
      PrintUserMenuStr(buf, 0);
      sprintf(buf, "ЖДИТЕ");
      PrintUserMenuStr(buf, 1);
      sprintf(buf, "ПРИНТЕР");
      PrintUserMenuStr(buf, 2);
      sprintf(buf, "НЕДОСТУПЕН");
      PrintUserMenuStr(buf, 3);
    }
  */
}

// получение текущей цены
int GetRecentChannelPrice(CPU_INT08U ch, CPU_INT32U* price, CPU_INT32U* time)
{
  TRTC_Data rtc;
  
  if (EnabledChannelsNum == 0) return -1;

  RTC_ReadTime(&rtc);
  
  // определим, будни или выходные
  CPU_INT32U wend = 0;
  CPU_INT08U iswend = 0; // флаг выходного дня
  GetData(&WeekEndDesc, &wend, RecentChannel, DATA_FLAG_DIRECT_INDEX);
 
  switch (wend)
  {
    case WEEKEND_FRIDAY_SUNDAY:
      if ((rtc.day == 0) || (rtc.day > 4)) iswend = 1;
      break;
    case WEEKEND_SATURDAY_SUNDAY:
      if ((rtc.day == 0) || (rtc.day == 6)) iswend = 1;
      break;
    case WEEKEND_FRIDAY_SATURDAY:
      if ((rtc.day == 5) || (rtc.day == 6)) iswend = 1;
      break;
    case WEEKEND_FRIDAY_MONDAY:
      if ((rtc.day > 4) || (rtc.day < 2)) iswend = 1;
      break;
    default:
      iswend = 0;
      break;
  }
  
  // определим интервал времени
  CPU_INT08U i;
  for (i=0; i<PRICE_PERIODS_NUM; i++)
    {
      CPU_INT32U start, end;
      if (iswend)
        {
          GetData(&T_Start_WeekendDesc, &start, RecentChannel*PRICE_PERIODS_NUM+i, DATA_FLAG_DIRECT_INDEX);  
          GetData(&T_End_WeekendDesc, &end, RecentChannel*PRICE_PERIODS_NUM+i, DATA_FLAG_DIRECT_INDEX);  
        }
      else
        {
          GetData(&T_Start_WeekdaysDesc, &start, RecentChannel*PRICE_PERIODS_NUM+i, DATA_FLAG_DIRECT_INDEX);  
          GetData(&T_End_WeekdaysDesc, &end, RecentChannel*PRICE_PERIODS_NUM+i, DATA_FLAG_DIRECT_INDEX);  
        }
      if ((rtc.hour >= start) && (rtc.hour < end)){break;}   
    }
  
  if (i >= PRICE_PERIODS_NUM) return -2;
  
  // прочитаем цену и время за цену
  if (iswend)  
    {
      GetData(&PriceTimeWeekendDesc, time, RecentChannel*PRICE_PERIODS_NUM+i, DATA_FLAG_DIRECT_INDEX);     
      GetData(&PriceWeekendDesc, price, RecentChannel*PRICE_PERIODS_NUM+i, DATA_FLAG_DIRECT_INDEX);     
    }
  else
    {
      GetData(&PriceTimeWeekdaysDesc, time, RecentChannel*PRICE_PERIODS_NUM+i, DATA_FLAG_DIRECT_INDEX);     
      GetData(&PriceWeekdaysDesc, price, RecentChannel*PRICE_PERIODS_NUM+i, DATA_FLAG_DIRECT_INDEX);     
    }
  
  return 0;
}


void IncRecentChannel(void)
{
  CPU_INT08U i;

  for (i=RecentChannel+1; i<CHANNELS_NUM; i++)
  {
    CPU_INT32U en = 0;
    GetData(&EnableChannelDesc, &en, i, DATA_FLAG_DIRECT_INDEX);
    if (en) {break;}
  }
  
  if (i<CHANNELS_NUM) {RecentChannel = i; return;}
    
  for (i=0; i<CHANNELS_NUM; i++)
  {
    CPU_INT32U en = 0;
    GetData(&EnableChannelDesc, &en, i, DATA_FLAG_DIRECT_INDEX);
    if (en) {break;}
  }

  RecentChannel = i;
}


void DecRecentChannel(void)
{
  CPU_INT08S i;
  
  for (i=RecentChannel-1; i>=0; i--)
  {
    CPU_INT32U en = 0;
    GetData(&EnableChannelDesc, &en, i, DATA_FLAG_DIRECT_INDEX);
    if (en) {break;}
  }
  
  if (i>=0) {RecentChannel = i; return;}
    
  for (i=CHANNELS_NUM-1; i>RecentChannel; i--)
  {
    CPU_INT32U en = 0;
    GetData(&EnableChannelDesc, &en, i, DATA_FLAG_DIRECT_INDEX);
    if (en) {RecentChannel = i; return;}
  }
  
}


void WorkServer(void)
{
  for (CPU_INT08U i=0; i<CHANNELS_NUM; i++)
    {
      switch (ChannelsState[i])
      {
        case CHANNEL_STATE_EMPTY:
          ChannelOff(i);
          break;
        case CHANNEL_STATE_PAUSE_BEFORE:
          if (ChannelsCounters[i]) ChannelsCounters[i]--;
          if (ChannelsWaitDeffered[i] != 0)
          {
          
          }
          else
          {
              if (!ChannelsCounters[i])
                { // переходим к работе
                  ChannelOn(i);
                  ChannelsCounters[i] = ChannelsPayedTime[i];
                  ChannelsState[i] = CHANNEL_STATE_WORK;  
                  // запишем событие
                  SaveEventRecord(i, JOURNAL_EVENT_START_SESSION, ChannelsPayedTime[i]);
                }
          }
          break;
        case CHANNEL_STATE_WORK:
          if (ChannelsCounters[i]) ChannelsCounters[i]--;
          if (!ChannelsCounters[i])
            { // заканчиваем работу
              ChannelOff(i);
              GetData(&TimeOutAfterDesc, &ChannelsCounters[i], i, DATA_FLAG_DIRECT_INDEX);
              ChannelsCounters[i] *= 60;
              ChannelsState[i] = CHANNEL_STATE_PAUSE_AFTER;  
              SaveEventRecord(i, JOURNAL_EVENT_END_SESSION, GetTimeSec());
            }
          break;
        case CHANNEL_STATE_PAUSE_AFTER:
          if (ChannelsCounters[i]) ChannelsCounters[i]--;
          if (!ChannelsCounters[i])
            { // закончили паузу
              ChannelsState[i] = CHANNEL_STATE_EMPTY;  
              ChannelsPayedTime[i] = 0;
            }
          break;      
      }
    
    }
  
}


void UserPrintThanksMenu(void)
{
  char buf[32];
  sprintf(buf, "СПАСИБО ЗА");
  PrintUserMenuStr(buf, 0);
  sprintf(buf, "ПОЛbЗОВАНИЕ УСЛУГОй");
  PrintUserMenuStr(buf, 1);
  sprintf(buf, "ПОЖАЛУйСТА");
  PrintUserMenuStr(buf, 2);
  sprintf(buf, "ВОЗbМИТЕ ЧЕК");
  PrintUserMenuStr(buf, 3);
}


void UserPrintFirstMenu(CPU_INT08U recentchannel)
{
 char buf[32];
  
 EnabledChannelsNum = 0;
 for (CPU_INT08U i=0; i<CHANNELS_NUM; i++)
  {
    CPU_INT32U en = 0;
    GetData(&EnableChannelDesc, &en, i, DATA_FLAG_DIRECT_INDEX);
    if (en) {EnabledChannelsNum++;}
  }
   
  if (EnabledChannelsNum == 0)
    { // нет включенных каналов - работа невозможна
      sprintf(buf, "НЕТ ВКЛЮЧЕННЫХ");
      PrintUserMenuStr(buf, 0);
      sprintf(buf, "КАНАЛОВ!");
      PrintUserMenuStr(buf, 1);
      sprintf(buf, "РАБОТА НЕВОЗМОЖНА");
      PrintUserMenuStr(buf, 2);
      sprintf(buf, "!!!!!!!!!!!!!!!!!");
      PrintUserMenuStr(buf, 3);
      return;   
    }
  else if (EnabledChannelsNum == 1)
    { // включен только один канал
      CPU_INT32U price=1, time=0;
      GetRecentChannelPrice(recentchannel, &price, &time);
      sprintf(buf, "ЦЕНА %dруб./%dмин.", price, time);
      PrintUserMenuStr(buf, 0);
      
      if (IsChannelOn(recentchannel))
        {
          sprintf(buf, "АППАРАТ РАБОТАЕТ");
          PrintUserMenuStr(buf, 1);
          
          switch (ChannelsState[recentchannel])
          {
            case CHANNEL_STATE_EMPTY:
              break;
            case CHANNEL_STATE_PAUSE_BEFORE:
              sprintf(buf, "НАЧАЛО СЕАНСА:");
              break;
            case CHANNEL_STATE_WORK:
              sprintf(buf, "ИДЕТ СЕАНС ЗАГАРА:");
              break;
            case CHANNEL_STATE_PAUSE_AFTER:
              sprintf(buf, "ВЕНТИЛЯЦИЯ:");
              break;
          }
          PrintUserMenuStr(buf, 2);

          PrintSecToMinSec(buf, GetChannelsTimeForFree(recentchannel));
          PrintUserMenuStr(buf, 3);
        }
      else
        {
          sprintf(buf, "ДЛЯ НАЧАЛА РАБОТЫ");
          PrintUserMenuStr(buf, 1);
          sprintf(buf, "НАЖМИТЕ");
          PrintUserMenuStr(buf, 2);
          GetDataStr(&StartButtonNameDesc, (CPU_INT08U*)buf, 0, DATA_FLAG_SYSTEM_INDEX);
          PrintUserMenuStr(buf, 3);
        }
      return;       
    }
 
  GetDataStr(&NameChannelDesc, (CPU_INT08U*)buf, recentchannel, DATA_FLAG_DIRECT_INDEX);
  sprintf(&buf[strlen(buf)], " %d", recentchannel+1);
  PrintUserMenuStr(buf, 0);
  CPU_INT32U price=1, time=0;
  GetRecentChannelPrice(recentchannel, &price, &time);
  sprintf(buf, "ЦЕНА %dруб./%dмин.", price, time);
  PrintUserMenuStr(buf, 1);
  if (IsChannelOn(recentchannel))
    {
      sprintf(buf, "ВЫБЕРИТЕ ДР.КАНАЛ");
      PrintUserMenuStr(buf, 2);
      sprintf(buf, "ОСТАЛОСb: ");
      PrintSecToHourMinSec(&buf[strlen(buf)], GetChannelsTimeForFree(recentchannel));
    }
  else
    {
      sprintf(buf, "ВЫБЕРИТЕ КАНАЛ");
      PrintUserMenuStr(buf, 2);
      sprintf(buf, "И НАЖМИТЕ ");
      GetDataStr(&StartButtonNameDesc, (CPU_INT08U*)&buf[strlen(buf)], 0, DATA_FLAG_SYSTEM_INDEX);
    }
  PrintUserMenuStr(buf, 3);
}

// получить время до окончания занятости канала
CPU_INT32U GetChannelsTimeForFree(CPU_INT08U ch)
{
  CPU_INT32U payedtime = ChannelsPayedTime[ch];
  CPU_INT32U beforetime = 0;
  CPU_INT32U aftertime = 0;
  CPU_INT32U res = 0;

  GetData(&TimeOutBeforeDesc, &beforetime, ch, DATA_FLAG_DIRECT_INDEX);
  GetData(&TimeOutAfterDesc, &aftertime, ch, DATA_FLAG_DIRECT_INDEX);
  aftertime *= 60;
    
  switch (ChannelsState[ch])
      {
        case CHANNEL_STATE_EMPTY:
          break;
        case CHANNEL_STATE_PAUSE_BEFORE:
          res = beforetime;
          break;
        case CHANNEL_STATE_WORK:
          res = payedtime;
          break;
        case CHANNEL_STATE_PAUSE_AFTER:
          res = aftertime;
          break;      
      }

  return res;
}

// включен ли канал
int IsChannelOn(CPU_INT08U ch)
{
  if (ChannelsState[ch]) return 1;
  return 0;
}

// проверка, были ли сохранены деньги до выключения питания
void LoadAcceptedMoney(void)
{
  CPU_INT32U m,crc,crct;

  // считаем cохраненные деньги из FRAM
  GetData(&AcceptedMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);    
  // считаем crc16 этих денег из FRAM 
  GetData(&AcceptedMoneyCRC16Desc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);    
  
  crct = CRC16((unsigned char*)&m, sizeof(CPU_INT32U));

  if (crct != crc)
    { // обнуляем, если crc не сошлась
      m = 0;
      crc = CRC16((unsigned char*)&m, sizeof(CPU_INT32U));
      SetData(&AcceptedMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);
      SetData(&AcceptedMoneyCRC16Desc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);
    }

   // считаем cохраненные деньги из FRAM
  GetData(&AcceptedBankMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);    
  // считаем crc16 этих денег из FRAM 
  GetData(&AcceptedBankMoneyCRC16Desc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);    
    
    crct = CRC16((unsigned char*)&m, sizeof(CPU_INT32U));
  
    if (crct != crc)
      { // обнуляем, если crc не сошлась
        m = 0;
        crc = CRC16((unsigned char*)&m, sizeof(CPU_INT32U));
        SetData(&AcceptedBankMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);
        SetData(&AcceptedBankMoneyCRC16Desc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);
      }
}

// добавить денег
void SetAcceptedMoney(CPU_INT32U money)
{
  CPU_INT32U m,crc;
  m=money;
  crc = CRC16((unsigned char*)&m, sizeof(CPU_INT32U));
  SetData(&AcceptedMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);
  SetData(&AcceptedMoneyCRC16Desc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);
}

// очистить счетчик денег
void ClearAcceptedMoney(void)
{
  CPU_INT32U m,crc;
  m=0;
  crc = CRC16((unsigned char*)&m, sizeof(CPU_INT32U));
  SetData(&AcceptedMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);
  SetData(&AcceptedMoneyCRC16Desc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);
}

// очистить счетчик денег
CPU_INT32U GetAcceptedMoney(void)
{
  CPU_INT32U m;
  GetData(&AcceptedMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);
  return m;
}

// добавить денег
void SetAcceptedBankMoney(CPU_INT32U money)
{
  CPU_INT32U m,crc;

  m=money;
  crc = CRC16((unsigned char*)&m, sizeof(CPU_INT32U));
  SetData(&AcceptedBankMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);
  SetData(&AcceptedBankMoneyCRC16Desc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);
}

// очистить счетчик денег
void ClearAcceptedBankMoney(void)
{
  CPU_INT32U m,crc;

  m=0;
  crc = CRC16((unsigned char*)&m, sizeof(CPU_INT32U));
  SetData(&AcceptedBankMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);
  SetData(&AcceptedBankMoneyCRC16Desc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);
}

// очистить счетчик денег
CPU_INT32U GetAcceptedBankMoney(void)
{
  CPU_INT32U m;

  GetData(&AcceptedBankMoneyDesc, &m, 0, DATA_FLAG_SYSTEM_INDEX);
  return m;
}

// инициализация пароля
void InitPass(void)
{
  CPU_INT32U pass,crc,crct;
  GetData(&PassDesc, &pass, 0, DATA_FLAG_SYSTEM_INDEX);    
  GetData(&PassCRCDesc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);    
  
  crct = CRC16((unsigned char*)&pass, sizeof(CPU_INT32U));

  if (crct != crc)
    { // обнуляем, если crc не сошлась
      pass = DEFAULT_PASSWORD;
      crc = CRC16((unsigned char*)&pass, sizeof(CPU_INT32U));
      SetData(&PassDesc, &pass, 0, DATA_FLAG_SYSTEM_INDEX);
      SetData(&PassCRCDesc, &crc, 0, DATA_FLAG_SYSTEM_INDEX);
    }
}
