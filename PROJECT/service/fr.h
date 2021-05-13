#ifndef _FR_H_
#define _FR_H_

// ������� �������� ������ ����
#define WAIT_PRINT_TIMEOUT  10000

// ������ ��������� �� ���������
#define DEFAULT_PASS  1

extern CPU_INT08U FiscalConnState;

extern int FReportGet(void);
extern void InitFiscal(void);
extern int IsFiscalConnected(void);
extern void FPend(void);
extern void FPost(void);
extern int PrintFiscalBill(CPU_INT32U money, CPU_INT32U time, CPU_INT32U online, CPU_INT32U nomprice, CPU_INT32U nompricetime);
extern int PrintFiscalBillRepeated(CPU_INT32U money, CPU_INT32U time);
extern void SetFiscalErrorByCode(CPU_INT08U err);
extern void ClearFiscalErrors(void);
extern int TstCriticalFiscalError(void);
extern int GetFirstCriticalFiscalError(CPU_INT08U *err);
extern void ClrFiscalErrorByCode(CPU_INT08U err);
extern void FReportPend(void);
extern void FReportPost(void);
extern CPU_INT16U FReportTest(void);
extern int CheckFiscalStatus();
extern int ConnectFiscalFast(void);

#endif //#ifndef _FR_H_
