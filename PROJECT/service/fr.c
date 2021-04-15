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


// �������� � ��������� �������� ������� ��
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
        // ������� ������
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
        
        // ������ ����������� ������
        if (FiscGetFullStatus(DEFAULT_PASS, &FiscFullStatus, &err) != FISC_OK)
        {
            if (err)
            {
                // ���� �����-�� ���������� ������
                if (err) SetFiscalErrorByCode(err);
                FiscalConnState = FISCAL_CONN;
                ClrErrorFlag(ERROR_FR_CONN);
            }
            else
            { 
                // ������ ��� ����������
                ClearFiscalErrors();
                FiscalConnState = FISCAL_NOCONN;
                SetErrorFlag(ERROR_FR_CONN);
            }
            return -2;
        }
            
        // ����� ��������� ������
        FiscalConnState = FISCAL_CONN;
        ClrErrorFlag(ERROR_FR_CONN);
              
        CPU_INT16U flags;
        memcpy(&flags, &FiscFullStatus.Flags, sizeof(CPU_INT16U));

        // ��������� �����:
        // ���� (0 � ���, 1 � ����) 
        if (!(flags & (1L<<5)))
        {
            // ��� ����
            SetFiscalErrorByCode(FR_ERROR_CODE_a1);
            return -3;
        }
        else
        {
            ClrFiscalErrorByCode(FR_ERROR_CODE_a1);
        }
              
        ClrFiscalErrorByCode(FR_ERROR_CODE_6b);
        // ��������
        switch (FiscFullStatus.SubMode)
        {
            case 1: 
                // 1.  ��������� ���������� ������ � �� �� � ���� ������ �������� � �� ��������� �� 
                //     ����� �������, ��������� � ������� �� ��� ���������, ������ �������� �������� �� 
                //     ���������� ������. 
                SetFiscalErrorByCode(FR_ERROR_CODE_6b);
                return -4;
            case 2: 
                // 2.  �������� ���������� ������ � �� � ���� ������ �������� � ��������� ������ 
                //     �������, �� ��������� � �������. ������� �� ����� ��������� ������ � �������� 3. 
                SetFiscalErrorByCode(FR_ERROR_CODE_6b);
                return -5;
            case 3: 
                // 3.  ����� ��������� ���������� ������ � �� ���� ������� ����������� ������. ����� 
                //     ����� ��������� �������, �� ��������� � �������.
                SetFiscalErrorByCode(FR_ERROR_CODE_6b);
                // ������������ ���
                FiscPrintContinue(DEFAULT_PASS, &err);
                return -6;
            case 4: //���� ������ �������� ������ ���������� �������
            case 5: //���� ������ �������� 
                continue;
            default:
                break;
        }

        // �����
        switch (FiscFullStatus.Mode)
        {
            case 0: 
                // 0.  ������� � ������� ������.
                // ��� ������ ������� �� ������
                // OK
                ClearFiscalErrors();
                goto check_exit;
            case 1: 
                // 1.  ������ ������.
                // OK
                ClearFiscalErrors();
                goto check_exit;
            case 2: 
                // 2.  �������� �����, 24 ���� �� ���������.
                // OK
                ClearFiscalErrors();
                goto check_exit;
            case 3: 
                // 3.  �������� �����, 24 ���� ���������.
                {
                    CPU_INT32U autoclose;
                    GetData(&EnableFiscalDayClearDesc, &autoclose, 0, DATA_FLAG_SYSTEM_INDEX);

                    if (autoclose==2)
                    {
                        // ��������� ����� � ����� 
                        FiscPrintDayReportToBuf(30, &err);
                        // ��� ������ ������������ ������, �������� �� ������
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
                        // ������� ����� Z-�������
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
                        // ������ - ������ ������ �������� � ������� �����
                        SetFiscalErrorByCode(FR_ERROR_CODE_4e);
                        return -11;
                    }
                }
                goto check_exit;
            case 4: 
                // 4.  �������� �����.
                ClearFiscalErrors();
                goto check_exit;
            case 5: 
                // 5.  ���������� �� ������������� ������ ���������� ����������. 
                SetFiscalErrorByCode(FR_ERROR_CODE_4f);
                goto check_exit;
            case 6: 
                // 6.  �������� ������������� ����� ����.
                SetFiscalErrorByCode(FR_ERROR_CODE_c0);
                goto check_exit;
            case 8: 
                // 8.  �������� ��������: 
                //  8.0. �������. 
                //  8.1. �������. 
                //  8.2. ������� �������. 
                //  8.3. ������� �������. 
                //  8.4. ������������. 
                {
                    CPU_INT64U cash = 0;
                    CPU_INT08U tax[4] = {0,0,0,0};
                    FiscCloseBill(DEFAULT_PASS, &cash, &tax[0], "������� �� �������!!!", &err);
                    if (err)
                    {
                        SetFiscalErrorByCode(err);
                        return -11;
                    }
                }
                goto check_exit;
            case 12: 
                // ���� ������ Z-������
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

// ����������� ��
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
        // ������
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
            // �� ����� ���������
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

        // �������� ������
        if (FiscGetFullStatus(DEFAULT_PASS, &FiscFullStatus, &err) != FISC_OK)
        {
            if (err)
            {
                // ���� �����-�� ���������� ������
                // ���� ��������� ��������
                if (err) SetFiscalErrorByCode(err);
            }
            else
            {
                // ������ ��� ����������
                ClearFiscalErrors();
                FiscalConnState = FISCAL_NOCONN;
                SetErrorFlag(ERROR_FR_CONN);
                FPost();
                return -2;
            }
        }
        
        // ����������� 
        FiscalConnState = FISCAL_CONN;
        ClrErrorFlag(ERROR_FR_CONN);
    }
    else
    {
        // �� ��������
        FiscalConnState = FISCAL_NOCONN;
        ClearFiscalErrors();
        ClrErrorFlag(ERROR_FR_CONN);
        FPost();
        return -3;
    }
    
    FPost();
    return 0;
}

// ����������� ��
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
        // ������
        for (j = FISCAL_BAUDS_COUNT-1; j >= 0; j--)
        {
            Uart0_Init(fiscal_bauds[j]);
            i = 2;
            do
            {
                poll = FiscPollExt();
                if ((poll == FISC_READY) || (poll == FISC_BUSY)) break;
            } while (--i);
            // �� ����� ���������
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

        // �������� ������
        if (FiscGetFullStatus(DEFAULT_PASS, &FiscFullStatus, &err) != FISC_OK)
        {
            if (err)
            {
                // ���� �����-�� ���������� ������
                // ���� ��������� ��������
                if (err) SetFiscalErrorByCode(err);
            }
            else
            {
                // ������ ��� ����������
                ClearFiscalErrors();
                FiscalConnState = FISCAL_NOCONN;
                SetErrorFlag(ERROR_FR_CONN);
                FPost();
                return -2;
            }
        }
        
        // ����������� 
        FiscalConnState = FISCAL_CONN;
        ClrErrorFlag(ERROR_FR_CONN);
    }
    else
    {
        // �� ��������
        FiscalConnState = FISCAL_NOCONN;
        ClearFiscalErrors();
        ClrErrorFlag(ERROR_FR_CONN);
        FPost();
        return -3;
    }
    
    FPost();
    return 0;
}

// �������������
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

// ������
void FPend(void)
{
 CPU_INT08U err;  
 do{
   OSSemPend(FLock, 1, &err);
   if (!err) break;
   OSTimeDly(1);
 }while (err);
}

// ����������
void FPost(void)
{
  OSTimeDly(200);
  OSSemPost(FLock);
}
/*
// ������
void FReportPend(void)
{
 CPU_INT08U err;  
 do{
   OSSemPend(FReportLock, 1, &err);
   if (!err) break;
   OSTimeDly(1);
 }while (err);
}

// ������ 2
int FReportGet(void)
{
 CPU_INT08U err;  
 OSSemPend(FReportLock, 1, &err);
 if (!err) return 1;
 return 0;
}

// ����������
void FReportPost(void)
{
  if (!FReportTest()) OSSemPost(FReportLock);
}

// ���������
// 0 - ����������
// >0 - ��������
CPU_INT16U FReportTest(void)
{
  return OSSemCheck(FReportLock); 
}

*/
// ������� ���� ������ ��
void ClearFiscalErrors(void)
{
  for (unsigned char i=ERROR_FR; i<ERROR_FR+FR_ERROR_NUMBER; i++)
    {
      ClrErrorFlag(i);
    }
}

  static const CPU_INT08U error_codes[FR_ERROR_NUMBER] = {
  0x1,//���������� ���������� �� 1, �� 2 ��� ���� 
0x2,//����������� �� 1 
0x3,//����������� �� 2 
0x4,//������������ ��������� � ������� ��������� � �� 
0x5,//��� ����������� ������ 
0x6,//�� � ������ ������ ������ 
0x7,//������������ ��������� � ������� ��� ������ ���������� �� 
0x8,//������� �� �������������� � ������ ���������� �� 
0x9,//������������ ����� ������� 
0x0A,//������ ������ �� BCD 
0x0B,//���������� ������ ������ �� ��� ������ ����� 
0x11,//�� ������� �������� 
0x12,//��������� ����� ��� ������ 
0x13,//������� ���� ������ ���� ��������� ������ � �� 
0x14,//������� ������� ������ �� ����������� 
0x15,//����� ��� ������� 
0x16,//����� �� �������,// 
0x17,//����� ������ ����� ������ ������ ��������� �����,// 
0x18,//���� ������ ����� ������ ���� ��������� �����,// 
0x19,//��� ������ � ��,// 
0x1A,//������� ��������������� � �� �����������,// 
0x1B,//��������� ����� �� ������,// 
0x1C,//� �������� ��������� ���� ������������ ������,// 
0x1D,//���������� ��������� ������ ������� ������,// 
0x1F,//����������� ������ ���������,// 
0x20,//������������ ��������� �������� ��� ����������,// 
0x21,//���������� ����� ������ ����������� ��������� ��������,// 
0x22,//�������� ����,// 
0x23,//��� ������ �����������,// 
0x24,//������� ����������� �����������,// 
0x25,//��� ����������� � ������������� �������,// 
0x28,//� �� ����� 2� ������� �������,// 
0x33,//������������ ��������� � �������,// 
0x35,//������������ �������� ��� ������ ����������,// 
0x36,//������������ ��������� � ������� ��� ������ ���������� ��,// 
0x37,//������� �� �������������� � ������ ���������� ��,// 
0x38,//������ � ���,//+ 
0x39,//���������� ������ �� ��,// 
0x3A,//������������ ���������� �� ��������� � �����,// 
0x3C,//����: �������� ��������������� �����,// 
0x3E,//������������ ���������� �� ������� � �����,// 
0x3F,//������������ ���������� �� ������� � �����,// 
0x40,//������������ ��������� ������,// 
0x41,//������������ ��������� ������ ���������,// 
0x42,//������������ ��������� ������ ����� 2,// 
0x43,//������������ ��������� ������ ����� 3,// 
0x44,//������������ ��������� ������ ����� 4 
0x45,//C���� ���� ����� ������ ������ ����� ����,// 
0x46,//�� ������� ���������� � �����,// 
0x47,//������������ ���������� �� ������� � �����,// 
0x48,//������������ ����� ����,// 
0x4A,//������ ��� - �������� ����������,// 
0x4B,//����� ���� ����������,// 
0x4C,//������������ ���������� �� ������� ������� � �����,// 
0x4D,//�������� ����������� ������� ����� ������ ����� ����,// 
0x4E,//����� ��������� 24 ����,// 
0x4F,//�������� ������,// 
0x50,//���� ������ ���������� �������,// 
0x51,//������������ ���������� ��������� � �����,// 
0x52,//������������ ���������� �� ���� ������ 2 � �����,// 
0x53,//������������ ���������� �� ���� ������ 3 � �����,// 
0x54,//������������ ���������� �� ���� ������ 4 � �����,// 
0x56,//��� ��������� ��� �������,// 
0x57,//����: ���������� �������� ���� �� ��������� � ��,// 
0x58,//�������� ������� ����������� ������,// 
0x59,//�������� ������ ������ ����������,// 
0x5B,//������������ ��������� ��������,// 
0x5C,//�������� ���������� 24�
0x5D,//������� �� ����������,// 
0x5E,//������������ ��������,// 
0x5F,//������������� ���� ����,// 
0x60,//������������ ��� ���������,// 
0x61,//������������ ��������� ����,// 
0x62,//������������ ��������� ����������,// 
0x63,//������������ ��������� ������,// 
0x64,//�� �����������,//+ 
0x65,//�� ������� ����� � ������,// 
0x66,//������������ ����� � ������,// 
0x67,//������ ����� � ��,//+ 
0x68,//�� ������� ����� �� ������� �������,// 
0x69,//������������ ����� �� ������� �������,// 
0x6A,//������ ������� � ������ ������ �� I
0x6B,//��� ������� �����,// 
0x6C,//��� ����������� �����,// 
0x6D,//�� ������� ����� �� ������,// 
0x6E,//������������ ����� �� ������,// 
0x6F,//������������ �� ������� � �����,// 
0x70,//������������ ��,// 
0x71,//������ ���������,//+ 
0x72,//������� �� �������������� � ������ ���������,// 
0x73,//������� �� �������������� � ������ ������,// 
0x74,//������ ���,// 
0x75,//������ �������,//+ 
0x76,//������ ��������: ��� ��������� � ��������������,//+ 
0x77,//������ ��������: ��� ������� � ��������,//+ 
0x78,//������ ��,// 
0x79,//������ ��,// 
0x7A,//���� �� ������������� 
0x7B,//������ ������������,// 
0x7C,//�� ��������� ����,// 
0x7D,//�������� ������ ����,// 
0x7E,//�������� �������� � ���� �����,// 
0x7F,//������������ ��������� ����� ����,// 
0x80,//������ ����� � ��,//+ 
0x81,//������ ����� � ��,//+ 
0x82,//������ ����� � ��,//+ 
0x83,//������ ����� � ��,//+ 
0x84,//������������ ����������,// 
0x85,//������������ �� �������� � �����,// 
0x86,//������������ �� �������� � �����,// 
0x87,//������������ �� ��������� ������ � �����,// 
0x88,//������������ �� ��������� ������� � �����,// 
0x89,//������������ �� �������� � �����,// 
0x8A,//������������ �� ��������� � ����,// 
0x8B,//������������ �� ������� � ����,// 
0x8C,//������������� ���� �������� � ����,// 
0x8D,//������������� ���� ������ � ����,// 
0x8E,//������� ���� ����,// 
0x8F,//����� �� ���������������,//,//
0x90,//���� ��������� ������, ������������� � ����������
0x91,//����� �� ������� ���� ������ ��� ������ ���������� ������ 
0x92,//��������� ����� 
0x93,//�������������� ��� ������ ������� 
0x94,//�������� ����� �������� � ���� 
0xA0,//������ ����� � ���� 
0xA1,//���� ����������� 
0xA2,//����: ������������ ������ ��� �������� ������� 
0xA3,//������������ ��������� ���� 
0xA4,//������ ���� 
0xA5,//������ �� � ������� ���� 
0xA6,//�������� ��������� ������ ���� 
0xA7,//���� ����������� 
0xA8,//����: �������� ���� � ����� 
0xA9,//����: ��� ����������� ������ 
0xAA,//������������ ���� (������������� ���� ���������) 
0xB0,//����: ������������ � ��������� ���������� 
0xB1,//����: ������������ � ��������� ����� 
0xB2,//����: ��� �������������� 
0xC0,//�������� ���� � ������� (����������� ���� � �����) 
0xC1,//����: �������� ���?� � �������� �������� ������ 
0xC2,//���������� ���������� � ����� ������� 
0xC3,//������������ ������ ���� � ���� 
0xC4,//������������ ������� ���� 
//0xC5,//����� ����������� ��������� ���� 
//0xC6,//���������� �������� �����������,//
//0xC7,//���� �� ������������� � ������ ������ 
//0xC8,//�����������,//�������� �� ����������� 
  };

// ��������� ����������� ����� ������ �� �� ������ ���� ��������
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

// ����� ����������� ����� ������ �� �� ������ ���� ��������
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

// ������ ���� �� ��
// time ������ � ��������
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
        // ������
        SetFiscalErrorByCode(err);
      }
      else
      {
        // ��� ����������
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

      // �������� ���������� �����
      if (FiscMakeSell(DEFAULT_PASS, &count, &price, 0, &tax[0], "������ �������, ���.", &err) != FISC_OK)
        {
          if (err) 
          {
            SetFiscalErrorByCode(err);
          }
          else
          {
            // ��� ����������
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

      // �������� ������ ����� �����
      count = 1000;
      price = money*100;
      if (FiscMakeSell(DEFAULT_PASS, &count, &price, 0, &tax[0], "������ �������", &err) != FISC_OK)
        {
          if (err)
          {
            SetFiscalErrorByCode(err);
          }
          else
          {
            // ��� ����������
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

  if (FiscCloseBill(DEFAULT_PASS, &cash, &tax[0], "������� �� �������!!!", &err) != FISC_OK)
    {
      if (err)
      {
        SetFiscalErrorByCode(err);
      }
      else
      {
        // ��� ����������
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
// ������ ���� �� �� � ���������, �� ������ �������� �����
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

// �������� ���������� ������ ��
int TstCriticalFiscalError(void)
{
  int res = 0;
  for (CPU_INT08U i=0; i<FR_CRITICAL_ERROR_NUM; i++)
    {
      res |= TstFiscalErrorByCode(FiscalCriticalErrorsTable[i]);
    }
  return res;
}

// ��������� ������� �� ������� ������������ ����� ��
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
