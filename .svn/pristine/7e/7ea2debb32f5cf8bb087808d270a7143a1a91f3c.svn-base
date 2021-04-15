#include <includes.h>
#include "app_serv.h"
#include "fiscal.h"
#include "fr.h"
#include "uart0.h"
#include "data.h"
#include "datadesc.h"
#include "journal.h"
#include "time.h"

TFiscDevType FiscDevInfo;
static TFiscFullStatus FiscFullStatus;
CPU_INT08U FiscalConnState = FISCAL_NOCONN;


OS_EVENT *FLock = NULL;
//OS_EVENT *FReportLock = NULL;
//OS_STK    FiscalTaskStk[FISCAL_TASK_STK_SIZE];


// проверка и обработка текущего статуса ФР
int CheckFiscalStatus()
{
    CPU_INT08U err;
    int poll;
    CPU_INT32U time1, time2;
    CPU_INT32U enable;
    GetData(&EnableFiscalDesc, &enable, 0, DATA_FLAG_SYSTEM_INDEX);  
    if (!enable) return 0;
                    
    time1 = time2 = OSTimeGet();
    
    while (labs(time2 - time1) < WAIT_PRINT_TIMEOUT)
    {
        OSTimeDly(100);
        time2 = OSTimeGet();
        // сначала поллим
        poll = FiscPollExt();
        if (poll == FISC_UNDEF)
        {
            FiscalConnState = FISCAL_NOCONN;
            SetErrorFlag(ERROR_FR_CONN);
            return -1;
        }
        else if (poll == FISC_BUSY)
        {
            continue;
        }
        
        // теперь запрашиваем статус
        if (FiscGetFullStatus(DEFAULT_PASS, &FiscFullStatus, &err) != FISC_OK)
        {
            if (err)
            {
                // есть какая-то глобальная ошибка
                if (err) SetFiscalErrorByCode(err);
                FiscalConnState = FISCAL_CONN;
                ClrErrorFlag(ERROR_FR_CONN);
            }
            else
            { 
                // просто нет соединения
                ClearFiscalErrors();
                FiscalConnState = FISCAL_NOCONN;
                SetErrorFlag(ERROR_FR_CONN);
            }
            return -2;
        }
            
        // далее разбираем статус
        FiscalConnState = FISCAL_CONN;
        ClrErrorFlag(ERROR_FR_CONN);
              
        CPU_INT16U flags;
        memcpy(&flags, &FiscFullStatus.Flags, sizeof(CPU_INT16U));

        // КРИТИЧНЫЕ ФЛАГИ:
        // ЭКЛЗ (0 – нет, 1 – есть) 
        if (!(flags & (1L<<5)))
        {
            // нет ЭКЛЗ
            SetFiscalErrorByCode(FR_ERROR_CODE_a1);
            return -3;
        }
        else
        {
            ClrFiscalErrorByCode(FR_ERROR_CODE_a1);
        }
              
        ClrFiscalErrorByCode(FR_ERROR_CODE_6b);
        // ПОДРЕЖИМ
        switch (FiscFullStatus.SubMode)
        {
            case 1: 
                // 1.  Пассивное отсутствие бумаги – ФР не в фазе печати операции – не принимает от 
                //     хоста команды, связанные с печатью на том документе, датчик которого сообщает об 
                //     отсутствии бумаги. 
                SetFiscalErrorByCode(FR_ERROR_CODE_6b);
                return -4;
            case 2: 
                // 2.  Активное отсутствие бумаги – ФР в фазе печати операции – принимает только 
                //     команды, не связанные с печатью. Переход из этого подрежима только в подрежим 3. 
                SetFiscalErrorByCode(FR_ERROR_CODE_6b);
                return -5;
            case 3: 
                // 3.  После активного отсутствия бумаги – ФР ждет команду продолжения печати. Кроме 
                //     этого принимает команды, не связанные с печатью.
                SetFiscalErrorByCode(FR_ERROR_CODE_6b);
                // допечатываем чек
                FiscPrintContinue(DEFAULT_PASS, &err);
                return -6;
            case 4: //Фаза печати операции полных фискальных отчетов
            case 5: //Фаза печати операции 
                continue;
            default:
                break;
        }

        // РЕЖИМ
        switch (FiscFullStatus.Mode)
        {
            case 0: 
                // 0.  Принтер в рабочем режиме.
                // или печать отчетов из буфера
                // OK
                ClearFiscalErrors();
                goto check_exit;
            case 1: 
                // 1.  Выдача данных.
                // OK
                ClearFiscalErrors();
                goto check_exit;
            case 2: 
                // 2.  Открытая смена, 24 часа не кончились.
                // OK
                ClearFiscalErrors();
                goto check_exit;
            case 3: 
                // 3.  Открытая смена, 24 часа кончились.
                {
                    CPU_INT32U autoclose;
                    GetData(&EnableFiscalDayClearDesc, &autoclose, 0, DATA_FLAG_SYSTEM_INDEX);

                    if (autoclose==2)
                    {
                        // закрываем смену в буфер 
                        FiscPrintDayReportToBuf(30, &err);
                        // как только переполнение буфера, печатаем из буфера
                        if (err == FR_ERROR_CODE_4b)
                        {
                            SetFiscalErrorByCode(err);
                            SaveEventRecord(0, JOURNAL_EVENT_PRINT_BUF, GetTimeSec());
                            FiscPrintDayReportsFromBuf(30, &err);
                            if (err)
                            {
                                SetFiscalErrorByCode(err);
                                return -8;
                            }
                        }
                        else if (err)
                        {
                            SetFiscalErrorByCode(err);
                            return -9;
                        }
                    }
                    else if (autoclose==1)
                    {
                        // закроем смену Z-отчетом
                        FiscPrintDayReportClear(30, &err);
                        SaveEventRecord(0, JOURNAL_EVENT_PRINT_Z, GetTimeSec());
                        if (err)
                        {
                            SetFiscalErrorByCode(err);
                            return -10;
                        }
                    }
                    else if (autoclose==0)
                    {
                        // ошибка - должен прийти оператор и закрыть смену
                        SetFiscalErrorByCode(FR_ERROR_CODE_4e);
                        return -11;
                    }
                }
                goto check_exit;
            case 4: 
                // 4.  Закрытая смена.
                ClearFiscalErrors();
                goto check_exit;
            case 5: 
                // 5.  Блокировка по неправильному паролю налогового инспектора. 
                SetFiscalErrorByCode(FR_ERROR_CODE_4f);
                goto check_exit;
            case 6: 
                // 6.  Ожидание подтверждения ввода даты.
                SetFiscalErrorByCode(FR_ERROR_CODE_c0);
                goto check_exit;
            case 8: 
                // 8.  Открытый документ: 
                //  8.0. Продажа. 
                //  8.1. Покупка. 
                //  8.2. Возврат продажи. 
                //  8.3. Возврат покупки. 
                //  8.4. Нефискальный. 
                {
                    CPU_INT64U cash = 0;
                    CPU_INT08U tax[4] = {0,0,0,0};
                    FiscCloseBill(DEFAULT_PASS, &cash, &tax[0], "Спасибо за покупку!!!", &err);
                    if (err)
                    {
                        SetFiscalErrorByCode(err);
                        return -11;
                    }
                }
                goto check_exit;
            case 12: 
                // идет печать Z-отчета
                continue;
            default:
                goto check_exit;
        }
    }

check_exit:
    
    if (labs(time2 - time1) < WAIT_PRINT_TIMEOUT)
    {
        // OK
        return 0;
    }
    
    return -1;
}

#define FISCAL_BAUDS_COUNT  7
static const CPU_INT32U fiscal_bauds[FISCAL_BAUDS_COUNT] = {2400, 4800, 9600, 19200, 38400, 57600, 115200};

// подключение ФР
int ConnectFiscal(void)
{
    int i, poll;
    CPU_INT08U err;
    CPU_INT32U enable;
    GetData(&EnableFiscalDesc, &enable, 0, DATA_FLAG_SYSTEM_INDEX);  
    FPend();
    if (enable)
    {
        int j;
        // поллим
        for (j = FISCAL_BAUDS_COUNT-1; j >= 0; j--)
        {
            Uart0_Init(fiscal_bauds[j]);
            i = 10;
            do
            {
                OSTimeDly(100);
                poll = FiscPollExt();
                if ((poll == FISC_READY) || (poll == FISC_BUSY)) break;
            } while (--i);
            // не нашли поллингом
            if (i)
            {
                break;
            }
        }
        
        if (j < 0)
        {
            SetErrorFlag(ERROR_FR_CONN); 
            FiscalConnState = FISCAL_NOCONN;
            FPost();
            return -1;
        }

        // запросим статус
        if (FiscGetFullStatus(DEFAULT_PASS, &FiscFullStatus, &err) != FISC_OK)
        {
            if (err)
            {
                // есть какая-то глобальная ошибка
                // надо разобрать подробно
                if (err) SetFiscalErrorByCode(err);
            }
            else
            {
                // просто нет соединения
                ClearFiscalErrors();
                FiscalConnState = FISCAL_NOCONN;
                SetErrorFlag(ERROR_FR_CONN);
                FPost();
                return -2;
            }
        }
        
        // подключиили 
        FiscalConnState = FISCAL_CONN;
        ClrErrorFlag(ERROR_FR_CONN);
    }
    else
    {
        // ФР отключен
        FiscalConnState = FISCAL_NOCONN;
        ClearFiscalErrors();
        ClrErrorFlag(ERROR_FR_CONN);
        FPost();
        return -3;
    }
    
    FPost();
    return 0;
}

// подключение ФР
int ConnectFiscalFast(void)
{
    int i, poll;
    CPU_INT08U err;
    CPU_INT32U enable;
    GetData(&EnableFiscalDesc, &enable, 0, DATA_FLAG_SYSTEM_INDEX);  
    FPend();
    if (enable)
    {
        int j;
        // поллим
        for (j = FISCAL_BAUDS_COUNT-1; j >= 0; j--)
        {
            Uart0_Init(fiscal_bauds[j]);
            i = 2;
            do
            {
                poll = FiscPollExt();
                if ((poll == FISC_READY) || (poll == FISC_BUSY)) break;
            } while (--i);
            // не нашли поллингом
            if (i)
            {
                break;
            }
        }
        
        if (j < 0)
        {
            SetErrorFlag(ERROR_FR_CONN); 
            FiscalConnState = FISCAL_NOCONN;
            FPost();
            return -1;
        }

        // запросим статус
        if (FiscGetFullStatus(DEFAULT_PASS, &FiscFullStatus, &err) != FISC_OK)
        {
            if (err)
            {
                // есть какая-то глобальная ошибка
                // надо разобрать подробно
                if (err) SetFiscalErrorByCode(err);
            }
            else
            {
                // просто нет соединения
                ClearFiscalErrors();
                FiscalConnState = FISCAL_NOCONN;
                SetErrorFlag(ERROR_FR_CONN);
                FPost();
                return -2;
            }
        }
        
        // подключиили 
        FiscalConnState = FISCAL_CONN;
        ClrErrorFlag(ERROR_FR_CONN);
    }
    else
    {
        // ФР отключен
        FiscalConnState = FISCAL_NOCONN;
        ClearFiscalErrors();
        ClrErrorFlag(ERROR_FR_CONN);
        FPost();
        return -3;
    }
    
    FPost();
    return 0;
}

// инициализация
void InitFiscal(void)
{ 
  if (!FLock)
    {
      FLock = OSSemCreate(1);
      //FReportLock = OSSemCreate(1);
      //OSTaskCreate(FiscalTask, (void *)0, (OS_STK *)&FiscalTaskStk[FISCAL_TASK_STK_SIZE-1], FISCAL_TASK_PRIO);
    }
    
  if (ConnectFiscal()) return;
  
  CheckFiscalStatus();
}

int IsFiscalConnected(void)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  int retval;
  if (FiscalConnState == FISCAL_CONN) retval=1;
  else retval=0;
  OS_EXIT_CRITICAL();
  return retval;
}

// занять
void FPend(void)
{
 CPU_INT08U err;  
 do{
   OSSemPend(FLock, 1, &err);
   if (!err) break;
   OSTimeDly(1);
 }while (err);
}

// освободить
void FPost(void)
{
  OSTimeDly(200);
  OSSemPost(FLock);
}
/*
// занять
void FReportPend(void)
{
 CPU_INT08U err;  
 do{
   OSSemPend(FReportLock, 1, &err);
   if (!err) break;
   OSTimeDly(1);
 }while (err);
}

// занять 2
int FReportGet(void)
{
 CPU_INT08U err;  
 OSSemPend(FReportLock, 1, &err);
 if (!err) return 1;
 return 0;
}

// освободить
void FReportPost(void)
{
  if (!FReportTest()) OSSemPost(FReportLock);
}

// проверить
// 0 - недоступен
// >0 - доступен
CPU_INT16U FReportTest(void)
{
  return OSSemCheck(FReportLock); 
}

*/
// очистка всех ошибок ФР
void ClearFiscalErrors(void)
{
  for (unsigned char i=ERROR_FR; i<ERROR_FR+FR_ERROR_NUMBER; i++)
    {
      ClrErrorFlag(i);
    }
}

  static const CPU_INT08U error_codes[FR_ERROR_NUMBER] = {
  0x1,//Неисправен накопитель ФП 1, ФП 2 или часы 
0x2,//Отсутствует ФП 1 
0x3,//Отсутствует ФП 2 
0x4,//Некорректные параметры в команде обращения к ФП 
0x5,//Нет запрошенных данных 
0x6,//ФП в режиме вывода данных 
0x7,//Некорректные параметры в команде для данной реализации ФП 
0x8,//Команда не поддерживается в данной реализации ФП 
0x9,//Некорректная длина команды 
0x0A,//Формат данных не BCD 
0x0B,//Неисправна ячейка памяти ФП при записи итога 
0x11,//Не введена лицензия 
0x12,//Заводской номер уже введен 
0x13,//Текущая дата меньше даты последней записи в ФП 
0x14,//Область сменных итогов ФП переполнена 
0x15,//Смена уже открыта 
0x16,//Смена не открыта,// 
0x17,//Номер первой смены больше номера последней смены,// 
0x18,//Дата первой смены больше даты последней смены,// 
0x19,//Нет данных в ФП,// 
0x1A,//Область перерегистраций в ФП переполнена,// 
0x1B,//Заводской номер не введен,// 
0x1C,//В заданном диапазоне есть поврежденная запись,// 
0x1D,//Повреждена последняя запись сменных итогов,// 
0x1F,//Отсутствует память регистров,// 
0x20,//Переполнение денежного регистра при добавлении,// 
0x21,//Вычитаемая сумма больше содержимого денежного регистра,// 
0x22,//Неверная дата,// 
0x23,//Нет записи активизации,// 
0x24,//Область активизаций переполнена,// 
0x25,//Нет активизации с запрашиваемым номером,// 
0x28,//В ФР более 2х сбойных записей,// 
0x33,//Некорректные параметры в команде,// 
0x35,//Некорректный параметр при данных настройках,// 
0x36,//Некорректные параметры в команде для данной реализации ФР,// 
0x37,//Команда не поддерживается в данной реализации ФР,// 
0x38,//Ошибка в ПЗУ,//+ 
0x39,//Внутренняя ошибка ПО ФР,// 
0x3A,//Переполнение накопления по надбавкам в смене,// 
0x3C,//ЭКЛЗ: неверный регистрационный номер,// 
0x3E,//Переполнение накопления по секциям в смене,// 
0x3F,//Переполнение накопления по скидкам в смене,// 
0x40,//Переполнение диапазона скидок,// 
0x41,//Переполнение диапазона оплаты наличными,// 
0x42,//Переполнение диапазона оплаты типом 2,// 
0x43,//Переполнение диапазона оплаты типом 3,// 
0x44,//Переполнение диапазона оплаты типом 4 
0x45,//Cумма всех типов оплаты меньше итога чека,// 
0x46,//Не хватает наличности в кассе,// 
0x47,//Переполнение накопления по налогам в смене,// 
0x48,//Переполнение итога чека,// 
0x4A,//Открыт чек - операция невозможна,// 
0x4B,//Буфер чека переполнен,// 
0x4C,//Переполнение накопления по обороту налогов в смене,// 
0x4D,//Вносимая безналичной оплатой сумма больше суммы чека,// 
0x4E,//Смена превысила 24 часа,// 
0x4F,//Неверный пароль,// 
0x50,//Идет печать предыдущей команды,// 
0x51,//Переполнение накоплений наличными в смене,// 
0x52,//Переполнение накоплений по типу оплаты 2 в смене,// 
0x53,//Переполнение накоплений по типу оплаты 3 в смене,// 
0x54,//Переполнение накоплений по типу оплаты 4 в смене,// 
0x56,//Нет документа для повтора,// 
0x57,//ЭКЛЗ: количество закрытых смен не совпадает с ФП,// 
0x58,//Ожидание команды продолжения печати,// 
0x59,//Документ открыт другим оператором,// 
0x5B,//Переполнение диапазона надбавок,// 
0x5C,//Понижено напряжение 24В
0x5D,//Таблица не определена,// 
0x5E,//Некорректная операция,// 
0x5F,//Отрицательный итог чека,// 
0x60,//Переполнение при умножении,// 
0x61,//Переполнение диапазона цены,// 
0x62,//Переполнение диапазона количества,// 
0x63,//Переполнение диапазона отдела,// 
0x64,//ФП отсутствует,//+ 
0x65,//Не хватает денег в секции,// 
0x66,//Переполнение денег в секции,// 
0x67,//Ошибка связи с ФП,//+ 
0x68,//Не хватает денег по обороту налогов,// 
0x69,//Переполнение денег по обороту налогов,// 
0x6A,//Ошибка питания в момент ответа по I
0x6B,//Нет чековой ленты,// 
0x6C,//Нет контрольной ленты,// 
0x6D,//Не хватает денег по налогу,// 
0x6E,//Переполнение денег по налогу,// 
0x6F,//Переполнение по выплате в смене,// 
0x70,//Переполнение ФП,// 
0x71,//Ошибка отрезчика,//+ 
0x72,//Команда не поддерживается в данном подрежиме,// 
0x73,//Команда не поддерживается в данном режиме,// 
0x74,//Ошибка ОЗУ,// 
0x75,//Ошибка питания,//+ 
0x76,//Ошибка принтера: нет импульсов с тахогенератора,//+ 
0x77,//Ошибка принтера: нет сигнала с датчиков,//+ 
0x78,//Замена ПО,// 
0x79,//Замена ФП,// 
0x7A,//Поле не редактируется 
0x7B,//Ошибка оборудования,// 
0x7C,//Не совпадает дата,// 
0x7D,//Неверный формат даты,// 
0x7E,//Неверное значение в поле длины,// 
0x7F,//Переполнение диапазона итога чека,// 
0x80,//Ошибка связи с ФП,//+ 
0x81,//Ошибка связи с ФП,//+ 
0x82,//Ошибка связи с ФП,//+ 
0x83,//Ошибка связи с ФП,//+ 
0x84,//Переполнение наличности,// 
0x85,//Переполнение по продажам в смене,// 
0x86,//Переполнение по покупкам в смене,// 
0x87,//Переполнение по возвратам продаж в смене,// 
0x88,//Переполнение по возвратам покупок в смене,// 
0x89,//Переполнение по внесению в смене,// 
0x8A,//Переполнение по надбавкам в чеке,// 
0x8B,//Переполнение по скидкам в чеке,// 
0x8C,//Отрицательный итог надбавки в чеке,// 
0x8D,//Отрицательный итог скидки в чеке,// 
0x8E,//Нулевой итог чека,// 
0x8F,//Касса не фискализирована,//,//
0x90,//Поле превышает размер, установленный в настройках
0x91,//Выход за границу поля печати при данных настройках шрифта 
0x92,//Наложение полей 
0x93,//Восстановление ОЗУ прошло успешно 
0x94,//Исчерпан лимит операций в чеке 
0xA0,//Ошибка связи с ЭКЛЗ 
0xA1,//ЭКЛЗ отсутствует 
0xA2,//ЭКЛЗ: Некорректный формат или параметр команды 
0xA3,//Некорректное состояние ЭКЛЗ 
0xA4,//Авария ЭКЛЗ 
0xA5,//Авария КС в составе ЭКЛЗ 
0xA6,//Исчерпан временной ресурс ЭКЛЗ 
0xA7,//ЭКЛЗ переполнена 
0xA8,//ЗКЛЗ: Неверные дата и время 
0xA9,//ЭКЛЗ: Нет запрошенных данных 
0xAA,//Переполнение ЭКЛЗ (отрицательный итог документа) 
0xB0,//ЭКЛЗ: Переполнение в параметре количество 
0xB1,//ЭКЛЗ: Переполнение в параметре сумма 
0xB2,//ЭКЛЗ: Уже активизирована 
0xC0,//Контроль даты и времени (подтвердите дату и время) 
0xC1,//ЭКЛЗ: суточный отч?т с гашением прервать нельзя 
0xC2,//Превышение напряжения в блоке питания 
0xC3,//Несовпадение итогов чека и ЭКЛЗ 
0xC4,//Несовпадение номеров смен 
//0xC5,//Буфер подкладного документа пуст 
//0xC6,//Подкладной документ отсутствует,//
//0xC7,//Поле не редактируется в данном режиме 
//0xC8,//Отсутствуют,//импульсы от таходатчика 
  };

// установка глобального флага ошибки ФР по ошибки коду драйвера
void SetFiscalErrorByCode(CPU_INT08U err)
{
  if (!err) return;
  for (unsigned char i=0; i<FR_ERROR_NUMBER; i++)
    {
      if (error_codes[i] == err) 
        {
          SetErrorFlag(ERROR_FR+i);
          break;
        }
    }
}

// сброс глобального флага ошибки ФР по ошибки коду драйвера
void ClrFiscalErrorByCode(CPU_INT08U err)
{
  for (unsigned char i=0; i<FR_ERROR_NUMBER; i++)
    {
      if (error_codes[i] == err) 
        {
          ClrErrorFlag(ERROR_FR+i);
          break;
        }
    }
}

// печать чека на ФР
// time теперь в секундах
int PrintFiscalBill(CPU_INT32U money, CPU_INT32U time)
{
  CPU_INT08U err;
  CPU_INT64U count = (time*1000)/60;
  CPU_INT64U cash = money*100;
  CPU_INT64U price = (CPU_INT64U)(((double)money*100*60)/time);
  CPU_INT08U tax[4] = {0,0,0,0};
  CPU_INT32U format = 0;
  CPU_INT08U repeat;
  
  FPend();
  
  if (CheckFiscalStatus() < 0)
  {
    FPost();
    return -100;
  }
  
  repeat = 0;
  
repeat_open:

  if (FiscOpenBill(DEFAULT_PASS, FISC_BILL_SELL, &err) != FISC_OK)
    {
      if (err)
      {
        // ошибка
        SetFiscalErrorByCode(err);
      }
      else
      {
        // нет соединения
        ClearFiscalErrors();
        FiscalConnState = FISCAL_NOCONN;
        SetErrorFlag(ERROR_FR_CONN);
      }
      
      if (repeat)
      {
        FPost();
        return -1;
      }
      repeat++;
    }

  if (repeat)
  {
    if (CheckFiscalStatus() == 0)
    {
        goto repeat_open;
    }
    else
    {
        FPost();
        return -200;
    }
  }
    
  GetData(&BillFormatDesc, &format, 0, DATA_FLAG_SYSTEM_INDEX);

  if (format == 0)
    {
        repeat = 0;
  
repeat_sell1:

      // печатаем количество минут
      if (FiscMakeSell(DEFAULT_PASS, &count, &price, 0, &tax[0], "Услуги солярия, мин.", &err) != FISC_OK)
        {
          if (err) 
          {
            SetFiscalErrorByCode(err);
          }
          else
          {
            // нет соединения
            ClearFiscalErrors();
            FiscalConnState = FISCAL_NOCONN;
            SetErrorFlag(ERROR_FR_CONN);
          }
          if (repeat)
          {
            FPost();
            return -2;
          }
          repeat++;
        }
        
      if (repeat)
      {
        if (CheckFiscalStatus() == 0)
        {
            goto repeat_sell1;
        }
        else
        {
            FPost();
            return -300;
        }
      }

    }
  else if (format == 1)
    {
      repeat = 0;
  
repeat_sell2:

      // печатаем только общую сумму
      count = 1000;
      price = money*100;
      if (FiscMakeSell(DEFAULT_PASS, &count, &price, 0, &tax[0], "Услуги солярия", &err) != FISC_OK)
        {
          if (err)
          {
            SetFiscalErrorByCode(err);
          }
          else
          {
            // нет соединения
            ClearFiscalErrors();
            FiscalConnState = FISCAL_NOCONN;
            SetErrorFlag(ERROR_FR_CONN);
          }
          if (repeat)
          {
            FPost();
            return -3;
          }
          repeat++;
        }
      if (repeat)
      {
        if (CheckFiscalStatus() == 0)
        {
            goto repeat_sell2;
        }
        else
        {
            FPost();
            return -400;
        }
      }


    }

    repeat = 0;
  
repeat_close:

  if (FiscCloseBill(DEFAULT_PASS, &cash, &tax[0], "Спасибо за покупку!!!", &err) != FISC_OK)
    {
      if (err)
      {
        SetFiscalErrorByCode(err);
      }
      else
      {
        // нет соединения
        ClearFiscalErrors();
        FiscalConnState = FISCAL_NOCONN;
        SetErrorFlag(ERROR_FR_CONN);
      }
      if (repeat)
      {
        FPost();
        return -4;
      }
      repeat++;
    }
    
  if (repeat)
  {
    if (CheckFiscalStatus() == 0)
    {
        goto repeat_close;
    }
    else
    {
        FPost();
        return -400;
    }
  }
  
  FPost();
  return 0;  
}

/*
// печать чека на ФР с повторами, на случай закрытия смены
int PrintFiscalBillRepeated(CPU_INT32U money, CPU_INT32U time)
{
  int res = PrintFiscalBill(money, time);
  int i=4;
  
  while ((res != 0) && (i>0))
    {
      OSTimeDly(5000);
      res = PrintFiscalBill(money, time);
      i--;
    }
  return res;
}
*/

int TstFiscalErrorByCode(unsigned char code)
{
  for (unsigned char i=0; i<FR_ERROR_NUMBER; i++)
    {
      if (error_codes[i] == code) 
        {
          return TstErrorFlag(ERROR_FR+i);
        }
    }
  return 0;
}

CPU_INT08U GetFiscalErrorNumberByCode(unsigned char code)
{
  for (unsigned char i=0; i<FR_ERROR_NUMBER; i++)
    {
      if (error_codes[i] == code) 
        {
          return i;
        }
    }
  return 0;
}

#define FR_CRITICAL_ERROR_NUM   70

const CPU_INT08U  FiscalCriticalErrorsTable[FR_CRITICAL_ERROR_NUM] = {
         FR_ERROR_CODE_1,
         FR_ERROR_CODE_2,
         FR_ERROR_CODE_6,
         FR_ERROR_CODE_a,
         FR_ERROR_CODE_b,
         FR_ERROR_CODE_11,
         FR_ERROR_CODE_13,
         FR_ERROR_CODE_14,
         FR_ERROR_CODE_17,
         FR_ERROR_CODE_18,
         FR_ERROR_CODE_1a,
         FR_ERROR_CODE_1c,
         FR_ERROR_CODE_1d,
         FR_ERROR_CODE_1f,
         FR_ERROR_CODE_23,
         FR_ERROR_CODE_24,
         FR_ERROR_CODE_38,
         FR_ERROR_CODE_39,
         FR_ERROR_CODE_3a,
         FR_ERROR_CODE_3c,
         FR_ERROR_CODE_3e,
         FR_ERROR_CODE_3f,
         FR_ERROR_CODE_40,
         FR_ERROR_CODE_41,
         FR_ERROR_CODE_42,
         FR_ERROR_CODE_43,
         FR_ERROR_CODE_44,
         FR_ERROR_CODE_47,
         FR_ERROR_CODE_48,
         FR_ERROR_CODE_4c,
         FR_ERROR_CODE_4e,
         FR_ERROR_CODE_51,
         FR_ERROR_CODE_52,
         FR_ERROR_CODE_53,
         FR_ERROR_CODE_54,
         FR_ERROR_CODE_57,
         FR_ERROR_CODE_5b,
         FR_ERROR_CODE_64,
         FR_ERROR_CODE_67,
         FR_ERROR_CODE_6a,
         FR_ERROR_CODE_6b,
         FR_ERROR_CODE_70,
         FR_ERROR_CODE_71,
         FR_ERROR_CODE_74,
         FR_ERROR_CODE_75,
         FR_ERROR_CODE_76,
         FR_ERROR_CODE_77,
         FR_ERROR_CODE_78,
         FR_ERROR_CODE_79,
         FR_ERROR_CODE_7b,
         FR_ERROR_CODE_80,
         FR_ERROR_CODE_81,
         FR_ERROR_CODE_82,
         FR_ERROR_CODE_83,
         FR_ERROR_CODE_87,
         FR_ERROR_CODE_88,
         FR_ERROR_CODE_89,
         FR_ERROR_CODE_8a,
         FR_ERROR_CODE_8b,
         FR_ERROR_CODE_a0,
         FR_ERROR_CODE_a1,
         FR_ERROR_CODE_a3,
         FR_ERROR_CODE_a4,
         FR_ERROR_CODE_a5,
         FR_ERROR_CODE_a6,
         FR_ERROR_CODE_a7,
         FR_ERROR_CODE_a8,
         FR_ERROR_CODE_a9,
         FR_ERROR_CODE_c2,
         FR_ERROR_CODE_c4
};

// проверка критичеких флагов ФР
int TstCriticalFiscalError(void)
{
  int res = 0;
  for (CPU_INT08U i=0; i<FR_CRITICAL_ERROR_NUM; i++)
    {
      res |= TstFiscalErrorByCode(FiscalCriticalErrorsTable[i]);
    }
  return res;
}

// получение первого по порядку критического флага ФР
int GetFirstCriticalFiscalError(CPU_INT08U *err)
{
  int res = 0;
  *err = 0;
  for (CPU_INT08U i=0; i<FR_CRITICAL_ERROR_NUM; i++)
    {
      res = TstFiscalErrorByCode(FiscalCriticalErrorsTable[i]);
      if (res) {*err = ERROR_FR+GetFiscalErrorNumberByCode(FiscalCriticalErrorsTable[i]); break;}
    }
  return res;
}
