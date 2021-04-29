#include <includes.h>
#include <stddef.h>
#include "journal.h"
#include "fram.h"
#include "fram_map.h"
#include "time.h"
#include "fr.h"
#include "crc16.h"
#include "mode.h"

static CPU_INT32U  GlobalErrorsFlags[8] = {0,0,0,0,0,0,0,0};

void SetErrorFlag(CPU_INT08U error)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  GlobalErrorsFlags[error/32] |= (1L << (error%32));
  OS_EXIT_CRITICAL();
}

void ClrErrorFlag(CPU_INT08U error)
{
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  GlobalErrorsFlags[error/32] &= ~(1L << (error%32));
  OS_EXIT_CRITICAL();
}

int TstErrorFlag(CPU_INT08U error)
{
  CPU_INT32U temp = 0;
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  OS_ENTER_CRITICAL();
  temp = GlobalErrorsFlags[error/32] & (1L << (error%32));
  OS_EXIT_CRITICAL();
  return temp;
}

int TstCriticalErrors(void)
{
  CPU_INT32U errors = 0;
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  CPU_INT32U ignore_fiscal = 0;

  GetData(&DisableFiscalErrorsDesc, &ignore_fiscal, 0, DATA_FLAG_SYSTEM_INDEX);     

  OS_ENTER_CRITICAL();
  
  if (!ignore_fiscal)
  {
    errors |= TstCriticalFiscalError();  
    errors |= TstErrorFlag(ERROR_FR_CONN);  
    /*
    if (!FReportTest())
    { // ФР недоступен для любой печати 
      errors |= 0x1;
    }
    */
  }
  
  errors |= TstErrorFlag(ERROR_VALIDATOR_CONN);
  
  OS_EXIT_CRITICAL();
  if (errors) return 1;
  return 0;
}


int TstCriticalValidatorErrors(void)
{
  return 0;
}

void ClrValidatorErrors(void)
{
  for (CPU_INT08U i=ERROR_VALIDATOR_FAILURE; i<ERROR_MODEM_CONN; ++i)
    {
      ClrErrorFlag(i);
    }
}

// запись в журнал записи об ошибке
void SaveErrorRecord(CPU_INT32U error)
{
  TErrorRecord record;
  
  // найти самую старую по времени запись и добавить новую после неё
  CPU_INT32U i, ind=0, indm = 0, maxtime = 0;
  for (i=0; i<ERROR_RECORDS_NUM; i++)
    {
      ReadArrayFram(offsetof(TFramMap, ErrorRecords[0])+i*sizeof(TErrorRecord), sizeof(TErrorRecord), (unsigned char*)&record);
      if ((record.time == 0) || (record.time == 0xffffffff)) {ind = i; break;}
      if (record.time >= maxtime) {maxtime = record.time; indm = i;}
    }
  
  if (i >= ERROR_RECORDS_NUM) 
    { // все записи заполены - записываем на место самой старой
      ind = (indm+1)%ERROR_RECORDS_NUM;
    }
    
  record.time = GetTimeSec();
  record.error = error;
  WriteArrayFram(offsetof(TFramMap, ErrorRecords[0])+ind*sizeof(TErrorRecord), sizeof(TErrorRecord), (unsigned char*)&record);
}

// получить запись из журнала ошибок
int GetErrorRecord(TErrorRecord* record, CPU_INT32U index)
{
  if (index >= ERROR_RECORDS_NUM) return -1;

  ReadArrayFram(offsetof(TFramMap, ErrorRecords[0])+index*sizeof(TErrorRecord), sizeof(TErrorRecord), (unsigned char*)record);

  return 0;
}

// получить запись из журнала событий
int GetEventRecord(TEventRecord* record, CPU_INT32U index)
{
  if (index >= EVENT_RECORDS_NUM) return -1;

  ReadArrayFram(offsetof(TFramMap, EventRecords[0])+index*sizeof(TEventRecord), sizeof(TEventRecord), (unsigned char*)record);

  return 0;
}

// запись в журнал записи о событии
void SaveEventRecord(CPU_INT08U channel, CPU_INT08U event, CPU_INT32U data)
{
  TEventRecord record;

  // найти самую старую по времени запись и добавить новую после неё
  CPU_INT32U i, ind=0, indm = 0, maxtime = 0;
  for (i=0; i<EVENT_RECORDS_NUM; i++)
    {
      ReadArrayFram(offsetof(TFramMap, EventRecords[0])+i*sizeof(TEventRecord), sizeof(TEventRecord), (unsigned char*)&record);
      if ((record.time == 0) || (record.time == 0xffffffff)) {ind = i; break;}
      if (record.time >= maxtime) {maxtime = record.time; indm = i;}
    }
  
  if (i >= ERROR_RECORDS_NUM) 
    { // все записи заполены - записываем на место самой старой
      ind = (indm+1)%ERROR_RECORDS_NUM;
    }
    
  record.time = GetTimeSec();
  record.channel = channel;
  record.event = event;
  record.data = data;
  WriteArrayFram(offsetof(TFramMap, EventRecords[0])+ind*sizeof(TEventRecord), sizeof(TEventRecord), (unsigned char*)&record);
}


void ClearErrorJournal(void)
{
  SetArrayFram(offsetof(TFramMap, ErrorRecords), sizeof(TErrorRecord)*ERROR_RECORDS_NUM, 0x00);
}

void ClearEventJournal(void)
{
  SetArrayFram(offsetof(TFramMap, EventRecords), sizeof(TEventRecord)*EVENT_RECORDS_NUM, 0x00);
}

void GetEventStr(char* str, char event)
{
  switch (event){
    case JOURNAL_EVENT_MONEY_NOTE:
      sprintf(str, "Вн.купюра ");
      break;
    case JOURNAL_EVENT_MONEY_COIN:
      sprintf(str, "Безнал.оплата ");
      break;
    case JOURNAL_EVENT_START_SESSION:
      sprintf(str, "Нач.сеанса ");
      break;
    case JOURNAL_EVENT_END_SESSION:
      sprintf(str, "Кон.сеанса ");
      break;
    case JOURNAL_EVENT_DEVICE_ON:
      sprintf(str, "Включение");
      break;
    case JOURNAL_EVENT_PRINT_BILL:
      sprintf(str, "Печать чека");
      break;
   case JOURNAL_EVENT_PRINT_BILL_ONLINE:
      sprintf(str, "Печать чека безнал");
      break;
    case JOURNAL_EVENT_PRINT_Z:
      sprintf(str, "Печать Z-отчета");
      break;
    case JOURNAL_EVENT_PRINT_X:
      sprintf(str, "Печать X-отчета");
      break;
    case JOURNAL_EVENT_PRINT_BUF:
      sprintf(str, "Печать отч.из буф.");
      break;
    case JOURNAL_EVENT_CHANGE_MODE:
      sprintf(str, "Смена режима");
      break;
    case JOURNAL_EVENT_INCASSATION:
      sprintf(str, "Инкассация");
      break;  
    case JOURNAL_EVENT_PASS_FAIL:
      sprintf(str, "Неверный пароль");
      break;  
    case JOURNAL_EVENT_EMAIL_FAIL:
      sprintf(str, "Ошибка отпр.e-mail");
      break;  
    case JOURNAL_EVENT_EMAIL_OK:
      sprintf(str, "E-mail отпр.успешно");
      break;  
    default:
      sprintf(str, "нет");
      break;
  }
}

void GetEventStrEng(char* str, char event)
{
  switch (event){
    case JOURNAL_EVENT_MONEY_NOTE:
      sprintf(str, " |  Vnesena kupura ");
      break;
    case JOURNAL_EVENT_MONEY_COIN:
      sprintf(str, " |  Вн.безнал ");
      break;
    case JOURNAL_EVENT_START_SESSION:
      sprintf(str, " |  Nachalo seansa ");
      break;
    case JOURNAL_EVENT_END_SESSION:
      sprintf(str, " |  Koneс seansa ");
      break;
    case JOURNAL_EVENT_DEVICE_ON:
      sprintf(str, " |  Vkluchenie ");
      break;
    case JOURNAL_EVENT_PRINT_BILL:
      sprintf(str, " |  Pechat' checka ");
      break;
    case JOURNAL_EVENT_PRINT_Z:
      sprintf(str, " |  Pechat' Z-otcheta ");
      break;
    case JOURNAL_EVENT_PRINT_X:
      sprintf(str, " |  Pechat' X-otcheta ");
      break;
    case JOURNAL_EVENT_PRINT_BUF:
      sprintf(str, " |  Pechat' otcheta iz bufera ");
      break;
    case JOURNAL_EVENT_CHANGE_MODE:
      sprintf(str, " |  Smena rejima ");
      break;
    case JOURNAL_EVENT_INCASSATION:
      sprintf(str, " |  Incassaciya ");
      break;  
    case JOURNAL_EVENT_PASS_FAIL:
      sprintf(str, " |  Neverniy parol' ");
      break;  
    case JOURNAL_EVENT_EMAIL_FAIL:
      sprintf(str, " |  Oshibka otpravki e-mail ");
      break;  
    case JOURNAL_EVENT_EMAIL_OK:
      sprintf(str, " |  E-mail otpravleno uspeshno ");
      break;
   case JOURNAL_EVENT_PRINT_BILL_ONLINE:
      sprintf(str, "Печать чека безнал");
      break;
    default:
      sprintf(str, " |  Net sobytiya ");
      break;
  }
}

void PrintEventJournalRecordEng(char* str, TEventRecord *record)
{
  if (record->event)
    {
      TRTC_Data rtc_data;
      
      // напечатаем время 
      Sec2Date(&rtc_data, record->time);
      sprintf(str, "|  ");
      PrintRTCDateTimeString(&str[strlen(str)], &rtc_data);
      // напечатаем событие
      GetEventStrEng(&str[strlen(str)], record->event);
      
      // напечатаем дополнительные поля
      if ((record->event == JOURNAL_EVENT_MONEY_NOTE) || (record->event == JOURNAL_EVENT_MONEY_COIN))
        {
          sprintf(&str[strlen(str)], "kanal %d ", record->channel+1);
          sprintf(&str[strlen(str)], "%d rub.", record->data);
        }
      else if (record->event == JOURNAL_EVENT_START_SESSION)
        {
          sprintf(&str[strlen(str)], "kanal %d ", record->channel+1);
          PrintSecToHourMinSec(&str[strlen(str)], record->data);
        }
      else if (record->event == JOURNAL_EVENT_END_SESSION)
        {
          sprintf(&str[strlen(str)], "kanal %d ", record->channel+1);
          sprintf(&str[strlen(str)], "");
        }
      else if (record->event == JOURNAL_EVENT_DEVICE_ON)
        {
          sprintf(&str[strlen(str)], "");
        }
      else if (record->event == JOURNAL_EVENT_PRINT_BILL)
        {
          sprintf(&str[strlen(str)], "kanal %d ", record->channel+1);
        }
      else if (record->event == JOURNAL_EVENT_PRINT_Z)
        {
          sprintf(&str[strlen(str)], "");
        }
      else if (record->event == JOURNAL_EVENT_PRINT_X)
        {
          sprintf(&str[strlen(str)], "");
        }
      else if (record->event == JOURNAL_EVENT_PRINT_BUF)
        {
          sprintf(&str[strlen(str)], "");
        }
      else if (record->event == JOURNAL_EVENT_CHANGE_MODE)
      {
          if (record->data == MODE_WORK) sprintf(&str[strlen(str)], "rabota");
          else sprintf(&str[strlen(str)], "nastroika");
      }
      else if (record->event == JOURNAL_EVENT_INCASSATION)
      {
          sprintf(&str[strlen(str)], "%u rub.", record->data);
      }
      else if (record->event == JOURNAL_EVENT_PASS_FAIL)
      {
          sprintf(&str[strlen(str)], "%u", record->data);
      }
      else if ((record->event == JOURNAL_EVENT_EMAIL_OK) || (record->event == JOURNAL_EVENT_EMAIL_FAIL))
      {
          sprintf(&str[strlen(str)], "");
      }
      sprintf(&str[strlen(str)], "\r\n");
    }
  else
    { // пустая запись
      sprintf(str, "net zapisi\r\n");
    }
}

void IncCounter(CPU_INT08U ch, CPU_INT32U time, CPU_INT32U money, CPU_INT32U moneyBank)
{
  CPU_INT32U r, t, m, b;
  TCountersLong long_ctrs;

  // увеличим канальные счетчики
  ReadArrayFram(offsetof(TFramMap, Counters.CounterChannelRun)+sizeof(CPU_INT32U)*ch, sizeof(CPU_INT32U), (unsigned char*)&r);
  ReadArrayFram(offsetof(TFramMap, Counters.CounterChannelTime)+sizeof(CPU_INT32U)*ch, sizeof(CPU_INT32U), (unsigned char*)&t);
  ReadArrayFram(offsetof(TFramMap, Counters.CounterChannelMoney)+sizeof(CPU_INT32U)*ch, sizeof(CPU_INT32U), (unsigned char*)&m);
  ReadArrayFram(offsetof(TFramMap, Counters.CounterChannelBankMoney)+sizeof(CPU_INT32U)*ch, sizeof(CPU_INT32U), (unsigned char*)&b);
  r++;
  t+=time;
  m+=money;
  b+=moneyBank;
  WriteArrayFram(offsetof(TFramMap, Counters.CounterChannelRun)+sizeof(CPU_INT32U)*ch, sizeof(CPU_INT32U), (unsigned char*)&r);
  WriteArrayFram(offsetof(TFramMap, Counters.CounterChannelTime)+sizeof(CPU_INT32U)*ch, sizeof(CPU_INT32U), (unsigned char*)&t);
  WriteArrayFram(offsetof(TFramMap, Counters.CounterChannelMoney)+sizeof(CPU_INT32U)*ch, sizeof(CPU_INT32U), (unsigned char*)&m);
  WriteArrayFram(offsetof(TFramMap, Counters.CounterChannelBankMoney)+sizeof(CPU_INT32U)*ch, sizeof(CPU_INT32U), (unsigned char*)&b);
  
  // увеличим общие счетчики
  ReadArrayFram(offsetof(TFramMap, Counters.CounterRun), sizeof(CPU_INT32U), (unsigned char*)&r);
  ReadArrayFram(offsetof(TFramMap, Counters.CounterTime), sizeof(CPU_INT32U), (unsigned char*)&t);
  ReadArrayFram(offsetof(TFramMap, Counters.CounterMoney), sizeof(CPU_INT32U), (unsigned char*)&m);
  r++;
  t+=time;
  m+=(money + moneyBank);
  WriteArrayFram(offsetof(TFramMap, Counters.CounterRun), sizeof(CPU_INT32U), (unsigned char*)&r);
  WriteArrayFram(offsetof(TFramMap, Counters.CounterTime), sizeof(CPU_INT32U), (unsigned char*)&t);
  WriteArrayFram(offsetof(TFramMap, Counters.CounterMoney), sizeof(CPU_INT32U), (unsigned char*)&m);

  // увеличим длинные счетчики
  ReadArrayFram(offsetof(TFramMap, CountersLong), sizeof(TCountersLong), (unsigned char*)&long_ctrs);
  long_ctrs.CounterChannelRunLong[ch]++;
  long_ctrs.CounterChannelTimeLong[ch] += time;
  long_ctrs.CounterChannelMoneyLong[ch] += money;
  long_ctrs.CounterChannelBankMoneyLong[ch] += moneyBank;
  long_ctrs.CounterRunLong++;
  long_ctrs.CounterTimeLong += time;
  long_ctrs.CounterMoneyLong += (money + moneyBank);
  long_ctrs.crc = CRC16((unsigned char*)&long_ctrs, offsetof(TCountersLong, crc));
  WriteArrayFram(offsetof(TFramMap, CountersLong), sizeof(TCountersLong), (unsigned char*)&long_ctrs);
}  

CPU_INT32U GetShortMoney()
{
  CPU_INT32U money;
  ReadArrayFram(offsetof(TFramMap, Counters.CounterMoney), sizeof(CPU_INT32U), (unsigned char*)&money);
  return money;
}

void CheckLongCounters(void)
{
    TCountersLong long_ctrs;
    CPU_INT16U crc;
    ReadArrayFram(offsetof(TFramMap, CountersLong), sizeof(TCountersLong), (unsigned char*)&long_ctrs);
    crc = CRC16((unsigned char*)&long_ctrs, offsetof(TCountersLong, crc));
    if (crc != long_ctrs.crc)
    {
        memset(&long_ctrs, 0, sizeof(TCountersLong));
        long_ctrs.crc = CRC16((unsigned char*)&long_ctrs, offsetof(TCountersLong, crc));
        WriteArrayFram(offsetof(TFramMap, CountersLong), sizeof(TCountersLong), (unsigned char*)&long_ctrs);    
        /// обычные счетчики тоже очистим
        ClearCounters();
        ClearBillnomCounter();
    }
}

void ClearCounters(void)
{
  SetArrayFram(offsetof(TFramMap, Counters), sizeof(CPU_INT32U)*(CHANNELS_NUM+1)*3, 0x00);
}

/// инкремент счетчика купюр по номиналам
void IncBillnomCounter(CPU_INT32U index)
{
    CPU_INT32U counter;
    if (index >= 24) return;
    ReadArrayFram(offsetof(TFramMap, Counters.CounterBillNominals)+sizeof(CPU_INT32U)*index, sizeof(CPU_INT32U), (unsigned char*)&counter);
    counter++;
    WriteArrayFram(offsetof(TFramMap, Counters.CounterBillNominals)+sizeof(CPU_INT32U)*index, sizeof(CPU_INT32U), (unsigned char*)&counter);
    
    ReadArrayFram(offsetof(TFramMap, Counters.BillsCount), sizeof(CPU_INT32U), (unsigned char*)&counter);
    counter++;
    WriteArrayFram(offsetof(TFramMap, Counters.BillsCount), sizeof(CPU_INT32U), (unsigned char*)&counter);
}

/// очистка счетчиков купюр
void ClearBillnomCounter(void)
{
    CPU_INT32U counter = 0;
    CPU_INT32U i;
    
    for (i = 0; i < 24; i++)
    {
        WriteArrayFram(offsetof(TFramMap, Counters.CounterBillNominals)+sizeof(CPU_INT32U)*i, sizeof(CPU_INT32U), (unsigned char*)&counter);    
    }
    
    WriteArrayFram(offsetof(TFramMap, Counters.BillsCount), sizeof(CPU_INT32U), (unsigned char*)&counter);
}

// сервер ошибок (занесение ошибок в журнал)
void ErrorServer(void)
{
  static CPU_INT32U PrevFlags[8] = {0,0,0,0,0,0,0,0};
  
  for (int i=0; i<ERRORS_NUM; i++)
    {
      if (!(PrevFlags[i/32] & (1L<<(i%32)))
           && (TstErrorFlag(i)))
        {
          // запишем в журнал
          SaveErrorRecord(i);
        }
    }
  
  memcpy(PrevFlags, GlobalErrorsFlags, sizeof(CPU_INT32U)*8);
}

