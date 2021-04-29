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

        ClrFiscalErrorByCode(FR_ERROR_CODE_6B);
        // ��������
        switch (FiscFullStatus.SubMode)
        {
            case 1: 
                // 1.  ��������� ���������� ������ � �� �� � ���� ������ �������� � �� ��������� �� 
                //     ����� �������, ��������� � ������� �� ��� ���������, ������ �������� �������� �� 
                //     ���������� ������. 
                SetFiscalErrorByCode(FR_ERROR_CODE_6B);
                return -4;
            case 2: 
                // 2.  �������� ���������� ������ � �� � ���� ������ �������� � ��������� ������ 
                //     �������, �� ��������� � �������. ������� �� ����� ��������� ������ � �������� 3. 
                SetFiscalErrorByCode(FR_ERROR_CODE_6B);
                return -5;
            case 3: 
                // 3.  ����� ��������� ���������� ������ � �� ���� ������� ����������� ������. ����� 
                //     ����� ��������� �������, �� ��������� � �������.
                SetFiscalErrorByCode(FR_ERROR_CODE_6B);
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
                        if (err == FR_ERROR_CODE_4B)
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
                        SetFiscalErrorByCode(FR_ERROR_CODE_4E);
                        goto check_exit;
                        //return -11;
                    }
                }
                goto check_exit;
            case 4: 
                // 4.  �������� �����.
                // ��������� �����
                if (FiscOpenDay(DEFAULT_PASS, &err) != FISC_OK)
                {
                    if (err)
                    {
                        SetFiscalErrorByCode(err);
                    }
                    else
                    {
                        ClearFiscalErrors();
                        FiscalConnState = FISCAL_NOCONN;
                        SetErrorFlag(ERROR_FR_CONN);
                    }                
                    return -12;
                }
                ClearFiscalErrors();
                goto check_exit;
            case 5: 
                // 5.  ���������� �� ������������� ������ ���������� ����������. 
                SetFiscalErrorByCode(FR_ERROR_CODE_4F);
                goto check_exit;
            case 6: 
                // 6.  �������� ������������� ����� ����.
                SetFiscalErrorByCode(FR_ERROR_CODE_C0);
                goto check_exit;
            case 8: 
                // 8.  �������� ��������: 
                //  8.0. �������. 
                //  8.1. �������. 
                //  8.2. ������� �������. 
                //  8.3. ������� �������. 
                //  8.4. ������������. 
                /*
                {
                    CPU_INT64U cash = 0;
                    CPU_INT08U tax[4] = {0,0,0,0};
                    FiscCloseBill(DEFAULT_PASS, &cash, &tax[0], "������� �� �������!!!", &err);
                    if (err)
                    {
                        SetFiscalErrorByCode(err);
                        return -13;
                    }
                }
                */
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
            i = 10;
            do
            {
                OSTimeDly(100);
                Uart0_Init(fiscal_bauds[j]);
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
            i = 2;
            do
            {
                OSTimeDly(100);
                Uart0_Init(fiscal_bauds[j]);
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
  int retval = 0;
  CPU_INT32U enable;
  #if OS_CRITICAL_METHOD == 3
  OS_CPU_SR  cpu_sr = 0;
  #endif
  GetData(&EnableFiscalDesc, &enable, 0, DATA_FLAG_SYSTEM_INDEX); 
  OS_ENTER_CRITICAL();
  if ((enable) && (FiscalConnState == FISCAL_CONN)) retval=1;
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

static const CPU_INT08U fr_error_codes[FR_ERROR_NUMBER] = 
{
    FR_ERROR_CODE_1,
    FR_ERROR_CODE_2,
    FR_ERROR_CODE_3,
    FR_ERROR_CODE_4,
    FR_ERROR_CODE_5,
    FR_ERROR_CODE_6,
    FR_ERROR_CODE_7,
    FR_ERROR_CODE_8,
    FR_ERROR_CODE_9,
    FR_ERROR_CODE_10,
    FR_ERROR_CODE_11,
    FR_ERROR_CODE_12,
    FR_ERROR_CODE_14,
    FR_ERROR_CODE_15,
    FR_ERROR_CODE_16,
    FR_ERROR_CODE_17,
    FR_ERROR_CODE_20,
    FR_ERROR_CODE_2F,
    FR_ERROR_CODE_30,
    FR_ERROR_CODE_33,
    FR_ERROR_CODE_34,
    FR_ERROR_CODE_35,
    FR_ERROR_CODE_36,
    FR_ERROR_CODE_37,
    FR_ERROR_CODE_38,
    FR_ERROR_CODE_39,
    FR_ERROR_CODE_3a,
    FR_ERROR_CODE_3c,
    FR_ERROR_CODE_3D,
    FR_ERROR_CODE_3E,
    FR_ERROR_CODE_3F,
    FR_ERROR_CODE_40,
    FR_ERROR_CODE_41,
    FR_ERROR_CODE_42,
    FR_ERROR_CODE_43,
    FR_ERROR_CODE_44,
    FR_ERROR_CODE_45,
    FR_ERROR_CODE_46,
    FR_ERROR_CODE_47,
    FR_ERROR_CODE_48,
    FR_ERROR_CODE_49,
    FR_ERROR_CODE_4A,
    FR_ERROR_CODE_4B,
    FR_ERROR_CODE_4C,
    FR_ERROR_CODE_4D,
    FR_ERROR_CODE_4E,
    FR_ERROR_CODE_4F,
    FR_ERROR_CODE_50,
    FR_ERROR_CODE_51,
    FR_ERROR_CODE_52,
    FR_ERROR_CODE_53,
    FR_ERROR_CODE_54,
    FR_ERROR_CODE_55,
    FR_ERROR_CODE_56,
    FR_ERROR_CODE_58,
    FR_ERROR_CODE_59,
    FR_ERROR_CODE_5B,
    FR_ERROR_CODE_5C,
    FR_ERROR_CODE_5D,
    FR_ERROR_CODE_5E,
    FR_ERROR_CODE_5F,
    FR_ERROR_CODE_60,
    FR_ERROR_CODE_61,
    FR_ERROR_CODE_62,
    FR_ERROR_CODE_63,
    FR_ERROR_CODE_65,
    FR_ERROR_CODE_66,
    FR_ERROR_CODE_68,
    FR_ERROR_CODE_69,
    FR_ERROR_CODE_6A,
    FR_ERROR_CODE_6B,
    FR_ERROR_CODE_6C,
    FR_ERROR_CODE_6D,
    FR_ERROR_CODE_6E,
    FR_ERROR_CODE_6F,
    FR_ERROR_CODE_71,
    FR_ERROR_CODE_72,
    FR_ERROR_CODE_73,
    FR_ERROR_CODE_74,
    FR_ERROR_CODE_75,
    FR_ERROR_CODE_77,
    FR_ERROR_CODE_78,
    FR_ERROR_CODE_7A,
    FR_ERROR_CODE_7B,
    FR_ERROR_CODE_7C,
    FR_ERROR_CODE_7D,
    FR_ERROR_CODE_7E,
    FR_ERROR_CODE_7F,
    FR_ERROR_CODE_84,
    FR_ERROR_CODE_85,
    FR_ERROR_CODE_86,
    FR_ERROR_CODE_87,
    FR_ERROR_CODE_88,
    FR_ERROR_CODE_89,
    FR_ERROR_CODE_8A,
    FR_ERROR_CODE_8B,
    FR_ERROR_CODE_8C,
    FR_ERROR_CODE_8D,
    FR_ERROR_CODE_8E,
    FR_ERROR_CODE_90,
    FR_ERROR_CODE_91,
    FR_ERROR_CODE_92,
    FR_ERROR_CODE_93,
    FR_ERROR_CODE_94,
    FR_ERROR_CODE_C0,
    FR_ERROR_CODE_C2,
    FR_ERROR_CODE_C4,
    FR_ERROR_CODE_C7,
    FR_ERROR_CODE_C8
  };

// ��������� ����������� ����� ������ �� �� ������ ���� ��������
void SetFiscalErrorByCode(CPU_INT08U err)
{
  if (!err) return;
  for (unsigned char i=0; i<FR_ERROR_NUMBER; i++)
    {
      if (fr_error_codes[i] == err) 
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
      if (fr_error_codes[i] == err) 
        {
          ClrErrorFlag(ERROR_FR+i);
          break;
        }
    }
}

// ������ ���� �� ��
// time ������ � ��������
int PrintFiscalBill(CPU_INT32U money, CPU_INT32U time, CPU_INT32U online)
{
  CPU_INT08U err;
  CPU_INT64U count = (time*1000)/60;
  CPU_INT64U cash = money*100;
  CPU_INT64U price = (CPU_INT64U)(((double)money*100*60)/time);
  CPU_INT08U tax[4] = {0,0,0,0};
  CPU_INT32U format = 0;
  CPU_INT08U repeat;
  CPU_INT32U tax1;
  CPU_INT08U subj;
  CPU_INT32U subj32;
  CPU_INT32U ext;
  
  GetData(&TaxFormatDesc, &tax1, 0, DATA_FLAG_SYSTEM_INDEX);
  tax[0] = (CPU_INT08U)tax1;
  GetData(&SubjSellDesc, &subj32, 0, DATA_FLAG_SYSTEM_INDEX);
  subj = (CPU_INT08U)subj32;
  GetData(&CommandV2Desc, &ext, 0, DATA_FLAG_SYSTEM_INDEX);
  
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

      if (ext)
      {
        count *= 1000;
      }
        
      // �������� ���������� �����
      if (((ext == 0) && (FiscMakeSell(DEFAULT_PASS, &count, &price, 0, &tax[0], "������ �������, ���.", &err) != FISC_OK))
            || ((ext) && (FiscMakeSellV2(DEFAULT_PASS, &count, &price, 0, &tax[0], subj, "������ �������, ���.", &err) != FISC_OK)))
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
      if (ext) count *= 1000;
      
      if (((ext == 0) && (FiscMakeSell(DEFAULT_PASS, &count, &price, 0, &tax[0], "������ �������", &err) != FISC_OK))
          || ((ext) && (FiscMakeSellV2(DEFAULT_PASS, &count, &price, 0, &tax[0], subj, "������ �������", &err) != FISC_OK)))
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

    if (ext)
    {
        GetData(&TaxSystemDesc, &tax1, 0, DATA_FLAG_SYSTEM_INDEX);
        tax[0] = (CPU_INT08U)tax1;
    }
    
    if(online)
    {
        if(((ext) && (FiscCloseBillV2Online(DEFAULT_PASS, &cash, tax[0], "������� �� �������!!!", &err) != FISC_OK)))
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
    }
    else
    {
        if (((ext == 0) && (FiscCloseBill(DEFAULT_PASS, &cash, &tax[0], "������� �� �������!!!", &err) != FISC_OK))
            || ((ext) && (FiscCloseBillV2(DEFAULT_PASS, &cash, tax[0], "������� �� �������!!!!", &err) != FISC_OK))
           )
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

int TstFiscalErrorByCode(unsigned char code)
{
  for (unsigned char i=0; i<FR_ERROR_NUMBER; i++)
    {
      if (fr_error_codes[i] == code) 
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
      if (fr_error_codes[i] == code) 
        {
          return i;
        }
    }
  return 0;
}

#define FR_CRITICAL_ERROR_NUM   105

const CPU_INT08U  FiscalCriticalErrorsTable[FR_CRITICAL_ERROR_NUM] = 
{
    FR_ERROR_CODE_1,
    FR_ERROR_CODE_2,
    FR_ERROR_CODE_3,
    FR_ERROR_CODE_4,
    FR_ERROR_CODE_5,
    FR_ERROR_CODE_6,
    FR_ERROR_CODE_7,
    FR_ERROR_CODE_8,
    FR_ERROR_CODE_9,
    FR_ERROR_CODE_10,
    FR_ERROR_CODE_11,
    FR_ERROR_CODE_12,
    FR_ERROR_CODE_14,
    FR_ERROR_CODE_15,
    FR_ERROR_CODE_16,
    FR_ERROR_CODE_17,
    FR_ERROR_CODE_30,
    FR_ERROR_CODE_33,
    FR_ERROR_CODE_34,
    FR_ERROR_CODE_35,
    FR_ERROR_CODE_36,
    FR_ERROR_CODE_37,
    FR_ERROR_CODE_38,
    FR_ERROR_CODE_39,
    FR_ERROR_CODE_3a,
    FR_ERROR_CODE_3c,
    FR_ERROR_CODE_3D,
    FR_ERROR_CODE_3E,
    FR_ERROR_CODE_3F,
    FR_ERROR_CODE_40,
    FR_ERROR_CODE_41,
    FR_ERROR_CODE_42,
    FR_ERROR_CODE_43,
    FR_ERROR_CODE_44,
    FR_ERROR_CODE_45,
    FR_ERROR_CODE_46,
    FR_ERROR_CODE_47,
    FR_ERROR_CODE_48,
    FR_ERROR_CODE_49,
    FR_ERROR_CODE_4A,
    FR_ERROR_CODE_4B,
    FR_ERROR_CODE_4C,
    FR_ERROR_CODE_4D,
    FR_ERROR_CODE_4E,
    FR_ERROR_CODE_4F,
    FR_ERROR_CODE_50,
    FR_ERROR_CODE_51,
    FR_ERROR_CODE_52,
    FR_ERROR_CODE_53,
    FR_ERROR_CODE_54,
    FR_ERROR_CODE_55,
    FR_ERROR_CODE_56,
    FR_ERROR_CODE_58,
    FR_ERROR_CODE_59,
    FR_ERROR_CODE_5B,
    FR_ERROR_CODE_5C,
    FR_ERROR_CODE_5D,
    FR_ERROR_CODE_5E,
    FR_ERROR_CODE_5F,
    FR_ERROR_CODE_60,
    FR_ERROR_CODE_61,
    FR_ERROR_CODE_62,
    FR_ERROR_CODE_63,
    FR_ERROR_CODE_65,
    FR_ERROR_CODE_66,
    FR_ERROR_CODE_68,
    FR_ERROR_CODE_69,
    FR_ERROR_CODE_6A,
    FR_ERROR_CODE_6B,
    FR_ERROR_CODE_6C,
    FR_ERROR_CODE_6D,
    FR_ERROR_CODE_6E,
    FR_ERROR_CODE_6F,
    FR_ERROR_CODE_72,
    FR_ERROR_CODE_73,
    FR_ERROR_CODE_74,
    FR_ERROR_CODE_75,
    FR_ERROR_CODE_77,
    FR_ERROR_CODE_78,
    FR_ERROR_CODE_7A,
    FR_ERROR_CODE_7B,
    FR_ERROR_CODE_7C,
    FR_ERROR_CODE_7D,
    FR_ERROR_CODE_7E,
    FR_ERROR_CODE_7F,
    FR_ERROR_CODE_84,
    FR_ERROR_CODE_85,
    FR_ERROR_CODE_86,
    FR_ERROR_CODE_87,
    FR_ERROR_CODE_88,
    FR_ERROR_CODE_89,
    FR_ERROR_CODE_8A,
    FR_ERROR_CODE_8B,
    FR_ERROR_CODE_8C,
    FR_ERROR_CODE_8D,
    FR_ERROR_CODE_8E,
    FR_ERROR_CODE_90,
    FR_ERROR_CODE_91,
    FR_ERROR_CODE_92,
    FR_ERROR_CODE_94,
    FR_ERROR_CODE_C0,
    FR_ERROR_CODE_C2,
    FR_ERROR_CODE_C4,
    FR_ERROR_CODE_C7,
    FR_ERROR_CODE_C8
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

// ������ ����
int CanselFiscalBill(void)
{
    CPU_INT08U err;
    
    FPend();
    
    if (CheckFiscalStatus() < 0)
    {
        FPost();
        return -100;
    }

    if (FiscCanselBill(DEFAULT_PASS, &err) != FISC_OK)
    {
        FPost();
        return -101;
    }
      
    FPost();
    return 0;
}

