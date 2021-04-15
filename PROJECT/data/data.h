#ifndef _DATA_H_
#define _DATA_H_

/*!
  �������� �������� � ������� ������� � ������
 
  ���� ��� ��������� - 32 - ������ float ��� signed long

*/

#include "cpu.h"

typedef union{
  CPU_INT32U Val32U;
  CPU_INT32S Val32S;
  CPU_FP32   ValFloat;
}TVariant32;

// ��������� ������
typedef struct{
  CPU_INT32U Min;
  CPU_INT32U Max;
}TRangeValueULONG;

typedef struct{
  CPU_INT32S Min;
  CPU_INT32S Max;
}TRangeValueSLONG;

typedef struct{
  CPU_FP32 Min;
  CPU_FP32 Max;
}TRangeValueFLOAT;


// ������������� ���������� ������
typedef struct{
  
  // ��� �����������
  CPU_INT08U Desc; 
    #define DATA_DESC_EDIT  0 // �������������
    #define DATA_DESC_VIEW  1 // ��� ��������� 
  
  // ��� ���������
  CPU_INT08U Type; 
    //#define DATA_TYPE_UCHAR  0
    //#define DATA_TYPE_SCHAR  1  
    #define DATA_TYPE_ULONG       2
    #define DATA_TYPE_SLONG       3  
    #define DATA_TYPE_FLOAT       4
    #define DATA_TYPE_TIME        5
    #define DATA_TYPE_TIME_COUNT  6
    #define DATA_TYPE_HOUR_MIN    7
  
  // ������������ ���������
  CPU_INT08U Location; 
    #define DATA_LOC_RAM    0
    #define DATA_LOC_FRAM   1

  // ������� �������
  CPU_INT08U IsArray; 
    #define DATA_NO_ARRAY    0
    #define DATA_IS_ARRAY    1

  // ������ �������
  CPU_INT32U ArraySize; 

  // ��������� �� ��������� ������� �������
  const void* ArrayIndex; //TDataDescStruct*
  
  // ��������� �� ���������� ��� ����� FRAM
  void* Data;
  
  // ��������� �� ������� ���������
  void* RangeValue;
    
  // ������� �� ���������
  void (*OnchangeFunc)(void);
  
  // �������� ����� ���������� � �������
  CPU_INT32U ArrayOffset; 
  
  // ��������� �� ������ �������� ���������
  const CPU_INT08U* Name;

  // ������� ���������� ��������� (������ �����)
  CPU_INT08U IsIndex; 
    #define DATA_NO_INDEX    0
    #define DATA_IS_INDEX    1

  // ��������� �� ������ ����� ��� ���������� ���������
  const CPU_INT08U** Items;

  // ��������� ������������� ��������� �� ��������� ��� ������ �������
  CPU_INT08U EnableInit; 
    #define DATA_INIT_DISABLE   0
    #define DATA_INIT_ENABLE    1

  // �������� �� ���������
  TVariant32 DefaultValue;
    
}TDataDescStruct;


// ��������� ��� ������� ���� ����������
typedef struct{
  const TDataDescStruct* ptr;
}TDataDescArrayStruct;

// �����
#define DATA_FLAG_SYSTEM_INDEX  0
#define DATA_FLAG_DIRECT_INDEX  1

// ������������ �������� 
#define DATA_OK    0
#define DATA_ERR  -1

// ����������� ������� 

// ��������� ������
extern int GetData(const TDataDescStruct* desc, void* buf, CPU_INT32U index, CPU_INT08U flags);
// ������ ������
extern int SetData(const TDataDescStruct* desc, void* buf, CPU_INT32U index, CPU_INT08U flags);
// ��������� ������ � ����������������� ���������
extern int GetDataStr(const TDataDescStruct* desc, CPU_INT08U* buf, CPU_INT32U index, CPU_INT08U flags);
// ��������� ������ ������ � ��������� � ���������
extern int GetDataFullStr(const TDataDescStruct* desc, CPU_INT08U* buf, CPU_INT32U index, CPU_INT08U flags);
// ��������� ������ � ������
extern int GetDataNameStr(const TDataDescStruct* desc, CPU_INT08U* buf);
// ������������� �� ���������
extern int InitDataByDefault(const TDataDescStruct* desc, CPU_INT32U index);
// �������� ���� ������������
extern int CheckAllData(void);
// ������������� 
extern int InitData(const TDataDescStruct* desc);
extern int GetDataItem(const TDataDescStruct* desc, CPU_INT08U* buf, CPU_INT32U itemindex);
extern int InitDescByDefault(const TDataDescStruct* desc);
extern int GetDataMin(const TDataDescStruct* desc, void* buf);
extern int GetDataMax(const TDataDescStruct* desc, void* buf);

#endif //#ifndef _DATA_H_
