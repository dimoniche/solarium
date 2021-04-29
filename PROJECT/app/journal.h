#ifndef _JOURNAL_H_
#define _JOURNAL_H_

#include "control.h"
#include "fiscal.h"

#define ERROR_RECORDS_NUM  256 // ����� ������� � �������
#define EVENT_RECORDS_NUM  256 // ����� ������� � �������

// ��������� ������ ���������� �������
typedef struct{
  CPU_INT32U time;
  
  // ��� ������, ���� ������� 256 ������
  CPU_INT32U error;
    // ��� ������ 
    #define ERROR_EMPTY                 0
    // ������ ����� � ����������
    #define ERROR_VALIDATOR_CONN        1
    // ����������� ������ ������ ���������
    #define ERROR_VALIDATOR_FAILURE     2

    // ������������� ������ ���������������
    // ������ ������ ��� ������
    #define ERROR_VALIDATOR_INSERTION   3
    // ������ ������ �� ���.�������
    #define ERROR_VALIDATOR_MAGNETIC    4
    // ������ ������ ��� ���������������
    #define ERROR_VALIDATOR_CONVEYING   5
    // ������ ������ �� �������������
    #define ERROR_VALIDATOR_IDENT       6
    // ������ ������ �� ����������� 
    #define ERROR_VALIDATOR_VRFY        7
    // ������ ������ �� �����.������� 
    #define ERROR_VALIDATOR_OPT         8
    // ������ ������ �� �������
    #define ERROR_VALIDATOR_INHIBIT     9
    // ������ ������ �� ���������� �������
    #define ERROR_VALIDATOR_CAP         10
    // ������ ������ �� �����
    #define ERROR_VALIDATOR_LNG         11
    // ������� ���������
    #define ERROR_STACKER_FULL          12
    // ������� �����������
    #define ERROR_STACKER_REMOVED       13
    // ����� � ���������������
    #define ERROR_BV_JAMMED             14
    // ����� � �������
    #define ERROR_ST_JAMMED             15
    // ������� ������
    #define ERROR_CHEATED               16
    // ������ ���������� ������
    #define ERROR_FLR_STACKER           17
    // ������ �������� ���������.������
    #define ERROR_TR_SPEED              18
    // ������ ���������.������
    #define ERROR_FLR_TRANSPORT         19
    // ������ ��������� ������������
    #define ERROR_FLR_ALIGNIN           20
    // ������� �����������
    #define ERROR_FLR_INIT_CAS          21
    // ������ ������
    #define ERROR_FLR_OPT               22
    // ������ ���.�������
    #define ERROR_FLR_MAG               23
    // ������ ���������� �������
    #define ERROR_FLR_CAP               24

    // ������ ����� � �������
    #define ERROR_MODEM_CONN            25
  
    // ������ ����� � ������������
    #define ERROR_FR_CONN               26

    // ��� ������ �� ���������
    #define ERROR_FR  27
  
    #define ERRORS_NUM                  (ERROR_FR+FR_ERROR_NUMBER)
  
  
}TErrorRecord;

// ��������� ������ ������� ������� �������
typedef struct{
  // ����� �������� ������
  CPU_INT32U time;
  
  // ������: ��� ��������� ����� - ������� ������, ��� ������ - ������������ ����������� �������, ���.
  CPU_INT32U data;

  // ��� �������
  CPU_INT08U event;
    #define JOURNAL_EVENT_NO_EVENT         0  // ��� �������
    #define JOURNAL_EVENT_MONEY_NOTE       1  // ������� ��������� ������
    #define JOURNAL_EVENT_MONEY_COIN       2  // ������� ��������� ������ (���-�� ������) (������ ��� ���������� ��������)
    #define JOURNAL_EVENT_START_SESSION    3  // ������� ������ ������
    #define JOURNAL_EVENT_END_SESSION      4  // ������� ����� ������
    #define JOURNAL_EVENT_DEVICE_ON        6  // ��������� ����������
    #define JOURNAL_EVENT_PRINT_BILL       7  // ������ ����
    #define JOURNAL_EVENT_PRINT_Z          8  // ������ z-������
    #define JOURNAL_EVENT_PRINT_X          9  // ������ x-������
    #define JOURNAL_EVENT_PRINT_BUF        10  // ������ x-������
    #define JOURNAL_EVENT_CHANGE_MODE      11  // ����� ������
    #define JOURNAL_EVENT_INCASSATION      12  // ���������� 
    #define JOURNAL_EVENT_PASS_FAIL        13  // �������� ���� ������
    #define JOURNAL_EVENT_EMAIL_OK         14  // ��������� ��������� email
    #define JOURNAL_EVENT_EMAIL_FAIL       15  // ������ ��� �������� email
    #define JOURNAL_EVENT_PRINT_BILL_ONLINE     16   // ������ ���� � �������� � ����������� ���������  
    #define JOURNAL_EVENTS_NUM             17  // ����� �������

  // �����
  CPU_INT08U channel;
  
}TEventRecord;


// ��������� ��� �������� ���������
typedef struct{
  // ����� �������� ����������
  CPU_INT32U  CounterChannelRun[CHANNELS_NUM];
  // ��������� ����� ������ ����������, ���.
  CPU_INT32U  CounterChannelTime[CHANNELS_NUM];
  // ����� ����� ����������  
  CPU_INT32U  CounterChannelMoney[CHANNELS_NUM];
  // ����� ����������� ����� ����������  
  CPU_INT32U  CounterChannelBankMoney[CHANNELS_NUM];
  
  // ����� ����� �������� 
  CPU_INT32U  CounterRun;
  // ����� ��������� ����� ������, ���.
  CPU_INT32U  CounterTime;
  // ����� ����� �����   
  CPU_INT32U  CounterMoney;
  
  // �������� ����� � ��������� �� ���������
  CPU_INT32U  CounterBillNominals[24];
  // ����� ������� ����� (����� � �������)
  CPU_INT32U  BillsCount;
}TCounters;


// ��������� ��� �������� ������� ���������
// ����� ���� ������ ��� ��� �������
typedef struct{
  // ����� �������� ����������
  CPU_INT32U  CounterChannelRunLong[CHANNELS_NUM];
  // ��������� ����� ������ ����������, ���.
  CPU_INT32U  CounterChannelTimeLong[CHANNELS_NUM];
  // ����� ����� ����������  
  CPU_INT32U  CounterChannelMoneyLong[CHANNELS_NUM];
  // ����� ����������� ����� ����������  
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
