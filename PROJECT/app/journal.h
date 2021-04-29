#ifndef _JOURNAL_H_
#define _JOURNAL_H_

#include "control.h"
#include "fiscal.h"

#define ERROR_RECORDS_NUM  256 // число записей в журнале
#define EVENT_RECORDS_NUM  256 // число записей в журнале

// структура записи системного журнала
typedef struct{
  CPU_INT32U time;
  
  // тип ошибки, пока аксимум 256 ошибок
  CPU_INT32U error;
    // нет ошибки 
    #define ERROR_EMPTY                 0
    // ошибка связи с купюрником
    #define ERROR_VALIDATOR_CONN        1
    // критическая ошибка работы купюрника
    #define ERROR_VALIDATOR_FAILURE     2

    // некритические ошибки купюроприемника
    // Выброс купюры при замине
    #define ERROR_VALIDATOR_INSERTION   3
    // Выброс купюры по маг.датчику
    #define ERROR_VALIDATOR_MAGNETIC    4
    // Выброс купюры при транспортировке
    #define ERROR_VALIDATOR_CONVEYING   5
    // Выброс купюры по идентификации
    #define ERROR_VALIDATOR_IDENT       6
    // Выброс купюры по верификации 
    #define ERROR_VALIDATOR_VRFY        7
    // Выброс купюры по оптич.датчику 
    #define ERROR_VALIDATOR_OPT         8
    // Выброс купюры по запрету
    #define ERROR_VALIDATOR_INHIBIT     9
    // Выброс купюры по емкостному датчику
    #define ERROR_VALIDATOR_CAP         10
    // Выброс купюры по длине
    #define ERROR_VALIDATOR_LNG         11
    // Кассета заполнена
    #define ERROR_STACKER_FULL          12
    // Кассета отсутствует
    #define ERROR_STACKER_REMOVED       13
    // Замин в купюроприемнике
    #define ERROR_BV_JAMMED             14
    // Замин в кассете
    #define ERROR_ST_JAMMED             15
    // Попытка обмана
    #define ERROR_CHEATED               16
    // Ошибка стекерного мотора
    #define ERROR_FLR_STACKER           17
    // Ошибка скорости транспорт.мотора
    #define ERROR_TR_SPEED              18
    // Ошибка транспорт.мотора
    #define ERROR_FLR_TRANSPORT         19
    // Ошибка механизма выравнивания
    #define ERROR_FLR_ALIGNIN           20
    // Кассета отсутствует
    #define ERROR_FLR_INIT_CAS          21
    // Ошибка оптики
    #define ERROR_FLR_OPT               22
    // Ошибка маг.датчика
    #define ERROR_FLR_MAG               23
    // Ошибка емкостного датчика
    #define ERROR_FLR_CAP               24

    // ошибка связи с модемом
    #define ERROR_MODEM_CONN            25
  
    // ошибка связи с фискальником
    #define ERROR_FR_CONN               26

    // ВСЕ ОШИБКИ ФР ФАТАЛЬНЫЕ
    #define ERROR_FR  27
  
    #define ERRORS_NUM                  (ERROR_FR+FR_ERROR_NUMBER)
  
  
}TErrorRecord;

// структура записи журнала рабочих событий
typedef struct{
  // время создания записи
  CPU_INT32U time;
  
  // данные: для получения денег - номинал купюры, для сеанса - длительность оплаченного времени, мин.
  CPU_INT32U data;

  // тип события
  CPU_INT08U event;
    #define JOURNAL_EVENT_NO_EVENT         0  // нет события
    #define JOURNAL_EVENT_MONEY_NOTE       1  // событие получения купюры
    #define JOURNAL_EVENT_MONEY_COIN       2  // событие получения монеты (кол-во рублей) (теперь это банковский терминал)
    #define JOURNAL_EVENT_START_SESSION    3  // событие начала сеанса
    #define JOURNAL_EVENT_END_SESSION      4  // событие конца сеанса
    #define JOURNAL_EVENT_DEVICE_ON        6  // включение устройства
    #define JOURNAL_EVENT_PRINT_BILL       7  // печать чека
    #define JOURNAL_EVENT_PRINT_Z          8  // печать z-отчета
    #define JOURNAL_EVENT_PRINT_X          9  // печать x-отчета
    #define JOURNAL_EVENT_PRINT_BUF        10  // печать x-отчета
    #define JOURNAL_EVENT_CHANGE_MODE      11  // смена режима
    #define JOURNAL_EVENT_INCASSATION      12  // инкассация 
    #define JOURNAL_EVENT_PASS_FAIL        13  // неверный ввод пароля
    #define JOURNAL_EVENT_EMAIL_OK         14  // правильно отправлен email
    #define JOURNAL_EVENT_EMAIL_FAIL       15  // ошибка при отправке email
    #define JOURNAL_EVENT_PRINT_BILL_ONLINE     16   // печать чека с деньгами с банковского терминала  
    #define JOURNAL_EVENTS_NUM             17  // число событий

  // канал
  CPU_INT08U channel;
  
}TEventRecord;


// структура для хранения счетчиков
typedef struct{
  // число запусков поканально
  CPU_INT32U  CounterChannelRun[CHANNELS_NUM];
  // Суммарное время работы поканально, сек.
  CPU_INT32U  CounterChannelTime[CHANNELS_NUM];
  // Сумма денег поканально  
  CPU_INT32U  CounterChannelMoney[CHANNELS_NUM];
  // Сумма безналичных денег поканально  
  CPU_INT32U  CounterChannelBankMoney[CHANNELS_NUM];
  
  // общее число запусков 
  CPU_INT32U  CounterRun;
  // общее Суммарное время работы, сек.
  CPU_INT32U  CounterTime;
  // общее Сумма денег   
  CPU_INT32U  CounterMoney;
  
  // счетчики купюр в купюрнике по номиналам
  CPU_INT32U  CounterBillNominals[24];
  // общий счетчик купюр (всего в кассете)
  CPU_INT32U  BillsCount;
}TCounters;


// структура для хранения длинных счетчиков
// ведем пока только эти три длинных
typedef struct{
  // число запусков поканально
  CPU_INT32U  CounterChannelRunLong[CHANNELS_NUM];
  // Суммарное время работы поканально, сек.
  CPU_INT32U  CounterChannelTimeLong[CHANNELS_NUM];
  // Сумма денег поканально  
  CPU_INT32U  CounterChannelMoneyLong[CHANNELS_NUM];
  // Сумма безналичных денег поканально  
  CPU_INT32U  CounterChannelBankMoneyLong[CHANNELS_NUM];
  
  CPU_INT32U  CounterRunLong;
  CPU_INT32U  CounterTimeLong;
  CPU_INT32U  CounterMoneyLong;
  CPU_INT16U  crc;
}TCountersLong;


extern CPU_INT32U GetShortMoney();
extern void IncBillnomCounter(CPU_INT32U index);
extern void CheckLongCounters(void);
extern void SaveErrorRecord(CPU_INT32U error);
extern void SaveEventRecord(CPU_INT08U channel, CPU_INT08U event, CPU_INT32U data);
extern void SetErrorFlag(CPU_INT08U error);
extern void ClrErrorFlag(CPU_INT08U error);
extern int TstErrorFlag(CPU_INT08U error);
extern int TstCriticalErrors(void);
extern void ClearErrorJournal(void);
extern void ClearEventJournal(void);
extern void GetEventStr(char* str, char event);
extern int GetEventRecord(TEventRecord* record, CPU_INT32U index);
extern int GetErrorRecord(TErrorRecord* record, CPU_INT32U index);
extern void IncCounter(CPU_INT08U ch, CPU_INT32U time, CPU_INT32U money, CPU_INT32U moneyBank);
extern void ClearCounters(void);
extern void ErrorServer(void);
extern int TstCriticalValidatorErrors(void);
extern void ClrValidatorErrors(void);
extern void PrintEventJournalRecordEng(char* str, TEventRecord *record);
extern void GetEventStrEng(char* str, char event);
extern void ClearBillnomCounter(void);

#endif //#ifndef _JOURNAL_H_
